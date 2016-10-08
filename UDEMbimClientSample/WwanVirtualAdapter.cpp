//
#include "pch.h"
#include "WwanVirtualAdapter.tmh"

NTSTATUS GetNtbParameters(
    _Out_writes_bytes_(TransferBufferLength) BYTE *TransferBuffer,
    _In_  ULONG TransferBufferLength,
    _Out_ ULONG * TransferedLength
)
{
    PNCM_NTB_PARAMETER UNALIGNED pResponse = (PNCM_NTB_PARAMETER)TransferBuffer;
    NTSTATUS status = STATUS_SUCCESS;    

    FuncEntry(TRACE_FLAG_Driver);

    if (TransferBufferLength < sizeof(NCM_NTB_PARAMETER))
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(TRACE_FLAG_Driver, "transferBufferLength is not equal to NCM_NTB_PARAMETER!");
        goto exit;
    }
    *TransferedLength = sizeof(NCM_NTB_PARAMETER);
    pResponse->wLength = sizeof(NCM_NTB_PARAMETER);
    pResponse->bmNtbFormatSupported = NCM_NTB_FORMAT_16_BIT | NCM_NTB_FORMAT_32_BIT;
    pResponse->dwNtbInMaxSize = 0x30000;
    pResponse->wNdpInDivisor = 0x20;
    pResponse->wNdpInPayloadRemainder = 0x0;
    pResponse->wNdpInAlignment = 0x4;
    pResponse->dwNtbOutMaxSize = 0x10000;
    pResponse->wNdpOutDivisor = 0x20;
    pResponse->wNdpOutPayloadRemainder = 0x0;
    pResponse->wNdpOutAlignment = 0x4;
    pResponse->wNtbOutMaxDatagrams = 0x32;

exit:
    FuncExit(TRACE_FLAG_Driver, status);
    return status;
}

NTSTATUS SetNtbInputSize(
    _In_ PUSB_CONTEXT UsbContext,
    _In_reads_bytes_(TransferBufferLength) BYTE *TransferBuffer,
    _In_ ULONG TransferBufferLength
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    FuncEntry(TRACE_FLAG_Driver);
    if (TransferBufferLength < sizeof(ULONG))
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        LogError(TRACE_FLAG_Driver, "transferBufferLength is less than ULONG!");
        goto exit;
    }
    UsbContext->MaxBulkInTransfer = *((ULONG*)TransferBuffer);

exit:
    FuncExit(TRACE_FLAG_Driver, status);
    return status;
}

NTSTATUS SetNtbFormat(
    _In_ PUSB_CONTEXT UsbContext,
    _In_ USHORT SetupValue
)
{
    NTSTATUS status = STATUS_SUCCESS;
    FuncEntry(TRACE_FLAG_Driver); 
    if (SetupValue < 0 || SetupValue>1)
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        LogError(TRACE_FLAG_Driver, "setupValue must be NCM_SET_NTB_FORMAT_16_BIT (0) or NCM_SET_NTB_FORMAT_32_BIT (1)!");
        goto exit;
    }
    UsbContext->NTBFormatSetupValue = SetupValue;

exit:
    FuncExit(TRACE_FLAG_Driver, status);
    return status;
}

NTSTATUS ResetAdapter()
{
    NTSTATUS status = STATUS_SUCCESS;
    return status;
}

NTSTATUS SendEncapsulatedCommand(
    _In_ PIO_CONTEXT IoContext,
    _In_ USHORT SetupValue,
    _In_reads_bytes_(TransferBufferLength) BYTE *TransferBuffer,
    _In_ ULONG TransferBufferLength
)
{
    NTSTATUS status = STATUS_SUCCESS;
    PMBB_MESSAGE_HEADER pMbbHeader = NULL;

    FuncEntry(TRACE_FLAG_Driver);
    if (SetupValue != 0)
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        LogError(TRACE_FLAG_Driver, "setupValue must be 0 for SEND_ENCAPSULATE_COMMAND");
        goto exit;
    }
    else if (TransferBufferLength < sizeof(MBB_MESSAGE_HEADER))
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        LogError(TRACE_FLAG_Driver, "transferBufferLength is less than MBB_MESSAGE_HEADER!");
        goto exit;
    }

    pMbbHeader = (PMBB_MESSAGE_HEADER)TransferBuffer;
    switch (pMbbHeader->MessageType)
    {
    case MBB_MESSAGE_TYPE_OPEN:
        //((PMBB_OPEN_MESSAGE)pMbbHeader)->Register();
        status = MbbOpen(IoContext, pMbbHeader);
        break;
    case MBB_MESSAGE_TYPE_COMMAND:
        status = MbbCommand(IoContext, pMbbHeader);
        break;
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        LogError(TRACE_FLAG_Driver, "SendEncapsulatedCommand receive unexpected command: 0x%X", pMbbHeader->MessageType);
        break;
    }

exit:
    FuncExit(TRACE_FLAG_Driver, status);
    return status;
}

NTSTATUS MbbOpen(_In_ PIO_CONTEXT IoContext, PMBB_MESSAGE_HEADER MbbMessageHeader)
{
    NTSTATUS status = STATUS_SUCCESS;
    MBB_OPEN_DONE           mbbOpenDone;
    PLIST_ENTRY             createdBufferEntry = NULL;

    FuncEntry(TRACE_FLAG_Driver);

    mbbOpenDone.MessageHeader.MessageType = MBB_MESSAGE_TYPE_OPEN_DONE;
    mbbOpenDone.MessageHeader.MessageLength = sizeof(MBB_OPEN_DONE);
    mbbOpenDone.MessageHeader.MessageTransactionId = MbbMessageHeader->MessageTransactionId;
    mbbOpenDone.MbbStatus = MBB_STATUS_SUCCESS;
    WdfSpinLockAcquire(IoContext->InProgressLock);
    status = MbbEnqueBuffer(&IoContext->EncapsulatedCommandResponse, &mbbOpenDone, sizeof(MBB_OPEN_DONE), TRUE, &createdBufferEntry);
    if (!NT_SUCCESS(status))
    {
        WdfSpinLockRelease(IoContext->InProgressLock);
        goto exit;
    }

    status = MbbEnqueAvailableInterrupt(&IoContext->AvailableInterrupt);
    if (!NT_SUCCESS(status))
    {
        RemoveEntryList(createdBufferEntry);
        WdfSpinLockRelease(IoContext->InProgressLock);
        FreeBufferContent(createdBufferEntry);
        goto exit;
    }
    WdfSpinLockRelease(IoContext->InProgressLock);
exit:
    FuncExit(TRACE_FLAG_Driver, status);
    return status;
}

NTSTATUS MbbCommand(_In_ PIO_CONTEXT IoContext, _In_ PMBB_MESSAGE_HEADER MbbMessageHeader)
{
    NTSTATUS status = STATUS_SUCCESS;
    PMBB_COMMAND_HEADER mbbCommandHeader = (PMBB_COMMAND_HEADER)MbbMessageHeader;

    FuncEntry(TRACE_FLAG_Driver);
    if (mbbCommandHeader->FragmentHeader.TotalFragments != 1)
    {
        //TODO::Code for fragmentation
        //Sample UDE client driver is using 64K as maxControlMessage, there shouldn't be any real fragmentation happening
        LogError(TRACE_FLAG_Driver, "Received fragmented MBBCommand which is not supported, will fail the request");
        status = STATUS_NOT_IMPLEMENTED;
        goto exit;
    }

    if (mbbCommandHeader->MessageHeader.MessageLength < sizeof(MBB_COMMAND_HEADER))
    {
        LogError(TRACE_FLAG_Driver, "Received command data size is incorrect");
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto exit;
    }

    LogInfo(TRACE_FLAG_Driver, "Received MBBCommand, ServiceID %!GUID!, CID 0x%x, CommandType %!CommandType!", &mbbCommandHeader->Command.ServiceId
        , mbbCommandHeader->Command.CommandId, mbbCommandHeader->CommandType);

    MBB_UUID_TO_HOST(&mbbCommandHeader->Command.ServiceId, &mbbCommandHeader->Command.ServiceId);

    if (IsEqualGUID(MBB_UUID_BASIC_CONNECT, mbbCommandHeader->Command.ServiceId))
    {
        MbbBasicCommand(IoContext, mbbCommandHeader);
    }
    else if (IsEqualGUID(MBB_UUID_SMS, mbbCommandHeader->Command.ServiceId))
    {
        //TODO::
        ;
    }
    else if (IsEqualGUID(MBB_UUID_USSD, mbbCommandHeader->Command.ServiceId))
    {
        //TODO::
        ;
    }
    else if (IsEqualGUID(MBB_UUID_PHONEBOOK, mbbCommandHeader->Command.ServiceId))
    {
        //TODO::
        ;
    }
    else if (IsEqualGUID(MBB_UUID_SAT, mbbCommandHeader->Command.ServiceId))
    {
        //TODO::
        ;
    }
    else if (IsEqualGUID(MBB_UUID_MS_VENDOR_EXTENSION, mbbCommandHeader->Command.ServiceId))
    {
        //TODO::
        ;
    }
    else if (IsEqualGUID(MBB_UUID_AUTH, mbbCommandHeader->Command.ServiceId))
    {
        //TODO::
        ;
    }
    else if (IsEqualGUID(MBB_UUID_DSS, mbbCommandHeader->Command.ServiceId))
    {
        //TODO::
        ;
    }
    else if (IsEqualGUID(MBB_UUID_MULTICARRIER, mbbCommandHeader->Command.ServiceId))
    {
        //TODO::
        ;
    }
    else
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
    }

exit:
    FuncExit(TRACE_FLAG_Driver, status);
    return status;
}

NTSTATUS MbbBasicCommand(_In_ PIO_CONTEXT IoContext, _In_ PMBB_COMMAND_HEADER MbbCommandHeader)
{
    NTSTATUS status = STATUS_SUCCESS;
    FuncEntry(TRACE_FLAG_Driver);

    switch (MbbCommandHeader->Command.CommandId)
    {
    case MBB_BASIC_CID_DEVICE_CAPS:
        MbbDeviceCaps(IoContext, MbbCommandHeader);
        break;
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    FuncExit(TRACE_FLAG_Driver, status);
    return status;
}

NTSTATUS MbbDeviceCaps(_In_ PIO_CONTEXT IoContext, _In_ PMBB_COMMAND_HEADER MbbCommandHeader)
{
    NTSTATUS status = STATUS_SUCCESS;
    FuncEntry(TRACE_FLAG_Driver);
    if (MbbCommandHeader->CommandType == MBB_COMMAND_TYPE_SET)
    {
        LogError(TRACE_FLAG_Driver, "MBB_BASIC_CID_DEVICE_CAPS is Query only");
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto exit;
    }

    MBB_DEVICE_CAPS mbbDeviceCaps;
    mbbDeviceCaps.DeviceType = MbbDeviceTypeEmbedded;
    mbbDeviceCaps.CellularClass = MbbCellularClassGsm;
    mbbDeviceCaps.VoiceClass = MbbVoiceClassNoVoice;
    mbbDeviceCaps.SimClass = MbbSimClassSimLogical;
    mbbDeviceCaps.DataClass = MbbDataClassLte;
    mbbDeviceCaps.SmsCaps = MbbSmsCapsNone;
    mbbDeviceCaps.ControlCaps = MbbControlCapsHwRadioSwitch;
    mbbDeviceCaps.dwMaxSessions = 8;
    mbbDeviceCaps.CustomDataClass.Offset = 0;
    mbbDeviceCaps.CustomDataClass.Size = 0;
    mbbDeviceCaps.DeviceIdString.Offset = 0;
    mbbDeviceCaps.DeviceIdString.Size = 0;
    mbbDeviceCaps.FirmwareInfo.Offset = 0;
    mbbDeviceCaps.FirmwareInfo.Size = 0;
    mbbDeviceCaps.HardwareInfo.Offset = 0;
    mbbDeviceCaps.HardwareInfo.Size = 0;
    *mbbDeviceCaps.DataBuffer = 'A';

    MbbDeviceResponse(IoContext, &mbbDeviceCaps, sizeof(MBB_DEVICE_CAPS), MbbCommandHeader);

exit:
    FuncExit(TRACE_FLAG_Driver, status);
    return status;
}

NTSTATUS GetEncapsulatedCommand(
    _In_ PIO_CONTEXT IoContext,
    _In_ USHORT SetupValue,
    _Out_writes_bytes_(TransferBufferLength) BYTE *TransferBuffer,
    _In_ ULONG TransferBufferLength,
    _Out_ ULONG * TransferedLength
)
{
    NTSTATUS status = STATUS_SUCCESS;
    PBUFFER_CONTENT bufferContent;
    PLIST_ENTRY     entry = NULL;

    FuncEntry(TRACE_FLAG_Driver);
    if (SetupValue != 0)
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        LogError(TRACE_FLAG_Driver, "setupValue must be 0 for GET_ENCAPSULATE_RESPONSE");
        goto exit;
    }    
    do
    {
        WdfSpinLockAcquire(IoContext->InProgressLock);
        if (IsListEmpty(&IoContext->EncapsulatedCommandResponse))
        {
            WdfSpinLockRelease(IoContext->InProgressLock);
            status = STATUS_INVALID_DEVICE_REQUEST;
            LogError(TRACE_FLAG_Driver, "No availabe EncapsulatedCommandResponse");
            goto exit;
        }
        else
        {
            entry = RemoveHeadList(&IoContext->EncapsulatedCommandResponse);
            bufferContent = (PBUFFER_CONTENT)CONTAINING_RECORD(entry, BUFFER_CONTENT, BufferLink);
            if (TransferBufferLength < bufferContent->BufferLength)
            {
                status = STATUS_INSUFFICIENT_RESOURCES;
                LogError(TRACE_FLAG_Driver, "TransferBufferLength(0x%x) is less than required(0x%x)", TransferBufferLength, bufferContent->BufferLength);
                InsertHeadList(&IoContext->EncapsulatedCommandResponse, entry);
                WdfSpinLockRelease(IoContext->InProgressLock);
                goto exit;
            }
        }
        WdfSpinLockRelease(IoContext->InProgressLock);
        
        *TransferedLength = bufferContent->BufferLength;
        RtlCopyMemory(TransferBuffer, bufferContent->Buffer, *TransferedLength);
        FREE_POOL(bufferContent->Buffer);
        FREE_POOL(bufferContent);
    } while (FALSE);

exit:
    FuncExit(TRACE_FLAG_Driver, status);
    return status;
}

NTSTATUS MbbEnqueBuffer(_In_ LIST_ENTRY* BufferQueue, _In_ PVOID Buffer, _In_ ULONG BufferLength, _In_ BOOL RenewBuffer, _Out_opt_ PLIST_ENTRY* CreatedBufferEntry)
{
    NTSTATUS status = STATUS_SUCCESS;
    PBUFFER_CONTENT       bufferEntry;

    FuncEntry(TRACE_FLAG_Driver);

    bufferEntry = (PBUFFER_CONTENT)ALLOCATE_NONPAGED_POOL(sizeof(BUFFER_CONTENT));
    if (bufferEntry == NULL)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(TRACE_FLAG_Driver, "Cannot allocate bufferEntry");
        goto exit;
    }

    if (CreatedBufferEntry != NULL)
    {
        *CreatedBufferEntry = (PLIST_ENTRY)bufferEntry;
    }
    bufferEntry->BufferLength = BufferLength;
    if (RenewBuffer)
    {
        bufferEntry->Buffer = (PVOID)ALLOCATE_NONPAGED_POOL(BufferLength);
        if (bufferEntry->Buffer == NULL)
        {
            FREE_POOL(bufferEntry);
            status = STATUS_INSUFFICIENT_RESOURCES;
            LogError(TRACE_FLAG_Driver, "Cannot allocate buffer");
            goto exit;
        }

        RtlCopyMemory(bufferEntry->Buffer, Buffer, BufferLength);
    }
    else
    {
        bufferEntry->Buffer = Buffer;
    }
    InsertTailList(BufferQueue, &bufferEntry->BufferLink);

exit:
    FuncExit(TRACE_FLAG_Driver, status);
    return status;
}

PLIST_ENTRY MbbRemoveBufferFromQueue(_In_ LIST_ENTRY* BufferQueue, _In_ PVOID Buffer)
{
    for (PLIST_ENTRY listEntry = BufferQueue->Flink;
        listEntry != BufferQueue;
        listEntry = listEntry->Flink)
    {
        if (((PBUFFER_CONTENT)listEntry)->Buffer == Buffer)
        {
            RemoveEntryList(listEntry);
            return listEntry;
        }
    }
    return NULL;
}

VOID MbbCleanupBufferQueue(_In_ LIST_ENTRY* BufferQueue)
{
    PLIST_ENTRY listEntry = BufferQueue->Flink;
    while (listEntry != BufferQueue)
    {
        RemoveEntryList(listEntry);
        FreeBufferContent(listEntry);
        listEntry = BufferQueue->Flink;
    }
}

VOID FreeBufferContent(PLIST_ENTRY BufferEntry)
{
    if (BufferEntry != NULL)
    {
        FREE_POOL(((PBUFFER_CONTENT)BufferEntry)->Buffer);
        FREE_POOL(BufferEntry);
    }
}

NTSTATUS MbbDeviceResponse(_In_ PIO_CONTEXT IoContext, _In_ PVOID Buffer, _In_ ULONG BufferLength, _In_ PMBB_COMMAND_HEADER MbbCommandHeader)
{
    NTSTATUS status = STATUS_SUCCESS;
    if (MbbCommandHeader->MessageHeader.MessageTransactionId == 0)
    {
        //TODO::Add code for unsolicated indication
    }
    else
    {
        status = MbbIndicateCommandDone(IoContext, Buffer, BufferLength, MbbCommandHeader);
    }
    return status;
}

NTSTATUS MbbIndicateCommandDone(_In_ PIO_CONTEXT IoContext, _In_ PVOID Buffer, _In_ ULONG BufferLength, PMBB_COMMAND_HEADER MbbCommandHeader)
{
    NTSTATUS status = STATUS_SUCCESS;
    PMBB_COMMAND_DONE_HEADER        commandDoneHeader = NULL;
    ULONG                           messageLength = sizeof(MBB_COMMAND_DONE_HEADER) + BufferLength;
    PLIST_ENTRY                     createdBufferEntry = NULL;

    FuncEntry(TRACE_FLAG_Driver);
    //TODO::Fragmentation is not implemented


    commandDoneHeader = (PMBB_COMMAND_DONE_HEADER)ALLOCATE_NONPAGED_POOL(messageLength);
    if (commandDoneHeader == NULL)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(TRACE_FLAG_Driver, "Cannot allocate commandDoneHeader");
        goto exit;
    }

    commandDoneHeader->MessageHeader.MessageType = MBB_MESSAGE_TYPE_COMMAND_DONE;
    commandDoneHeader->MessageHeader.MessageLength = messageLength;
    commandDoneHeader->MessageHeader.MessageTransactionId = MbbCommandHeader->MessageHeader.MessageTransactionId;
    commandDoneHeader->FragmentHeader.TotalFragments = 0;
    commandDoneHeader->FragmentHeader.CurrentFragment = 0;
    commandDoneHeader->Command = MbbCommandHeader->Command;
    commandDoneHeader->MbbStatus = MBB_STATUS_SUCCESS;
    commandDoneHeader->InformationBufferLength = BufferLength;
    RtlCopyMemory(commandDoneHeader + 1, Buffer, BufferLength);

    WdfSpinLockAcquire(IoContext->InProgressLock);
    status = MbbEnqueBuffer(&IoContext->EncapsulatedCommandResponse, commandDoneHeader, messageLength, FALSE, &createdBufferEntry);
    if (!NT_SUCCESS(status))
    {
        WdfSpinLockRelease(IoContext->InProgressLock);
        FREE_POOL(commandDoneHeader);
        goto exit;
    }

    status = MbbEnqueAvailableInterrupt(&IoContext->AvailableInterrupt);
    if (!NT_SUCCESS(status))
    {
        RemoveEntryList(createdBufferEntry);
        WdfSpinLockRelease(IoContext->InProgressLock);
        FreeBufferContent(createdBufferEntry);
        goto exit;
    }
    WdfSpinLockRelease(IoContext->InProgressLock);
    MbbTryDrainInterrupt(IoContext);
exit:
    FuncExit(TRACE_FLAG_Driver, status);
    return status;
}

NTSTATUS MbbEnqueAvailableInterrupt(LIST_ENTRY* InterruptQueue)
{
    USB_CDC_NOTIFICATION    responseAvailable;
    responseAvailable.bmRequestType = 0xA1;
    responseAvailable.bNotificationCode = USB_CDC_NOTIFICATION_RESPONSE_AVAILABLE;
    responseAvailable.wValue = 0;
    responseAvailable.wIndex = 0;
    responseAvailable.wLength = 0;
    return MbbEnqueBuffer(InterruptQueue, &responseAvailable, sizeof(USB_CDC_NOTIFICATION), TRUE, NULL);
}

VOID MbbTryDrainInterrupt(_In_ PIO_CONTEXT IoContext)
{
    WDFREQUEST  request;
    NTSTATUS        status = STATUS_SUCCESS;
    PUCHAR          transferBuffer;
    ULONG           transferBufferLength;
    PBUFFER_CONTENT interrupt = NULL;
    PLIST_ENTRY        entry = NULL;

    FuncEntry(TRACE_FLAG_Driver);

    do
    {
        //bool NoAvailableInterrupt = FALSE;
        WdfSpinLockAcquire(IoContext->InProgressLock);
        if (IsListEmpty(&IoContext->AvailableInterrupt))
        {
            WdfSpinLockRelease(IoContext->InProgressLock);
            goto exit;
        }
        status = WdfIoQueueRetrieveNextRequest(IoContext->InterruptUrbQueue, &request);
        if (!NT_SUCCESS(status))
        {
            WdfSpinLockRelease(IoContext->InProgressLock);
            if (status == STATUS_NO_MORE_ENTRIES)
            {
                LogVerbose(TRACE_FLAG_Driver, "No more interrupt request in queue");
                goto exit;
            }
            else
            {
                LogError(TRACE_FLAG_Driver, "WdfIoQueueRetrieveNextRequest failed (0x%x)", status);
                goto exit;
            }
        }

        status = UdecxUrbRetrieveBuffer(request, &transferBuffer, &transferBufferLength);
        if (!NT_SUCCESS(status))
        {
            //
            // Could mean there is no buffer on the request
            //
            WdfSpinLockRelease(IoContext->InProgressLock);
            status = STATUS_SUCCESS;
            CompleteURB(request, 0, status);
            continue;
        }

        entry = RemoveHeadList(&IoContext->AvailableInterrupt);
        interrupt = CONTAINING_RECORD(entry, BUFFER_CONTENT, BufferLink);
        if (transferBufferLength < interrupt->BufferLength)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LogError(TRACE_FLAG_Driver, "transferBufferLength(0x%x) is less than required(0x%x)", transferBufferLength, interrupt->BufferLength);
            InsertHeadList(&IoContext->AvailableInterrupt, entry);
            WdfSpinLockRelease(IoContext->InProgressLock);
            CompleteURB(request, 0, status);
            continue;
        }
        WdfSpinLockRelease(IoContext->InProgressLock);

        ASSERT(interrupt != NULL);
        RtlCopyMemory(transferBuffer, interrupt->Buffer, interrupt->BufferLength);
        CompleteURB(request, interrupt->BufferLength, status);
        FREE_POOL(interrupt->Buffer);
        FREE_POOL(interrupt);
    } while (TRUE);
exit:
    FuncExit(TRACE_FLAG_Driver, status);
    return;
}