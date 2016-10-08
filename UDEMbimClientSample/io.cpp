//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++

Module Name:

    io .cpp 
    
    Device handling events for example driver.

Abstract:

   This file contains the definitions for 
   interrupt callbacks and queue callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "pch.h"

#include "io.tmh"


NTSTATUS
Io_RetrieveControlQueue(
    _In_
        WDFDEVICE  Device,
    _Out_
        WDFQUEUE * Queue
    )
{
    NTSTATUS status;
    PIO_CONTEXT pIoContext;
    WDF_IO_QUEUE_CONFIG queueConfig;

    pIoContext = WdfDeviceGetIoContext(Device);

    status = STATUS_SUCCESS;
    *Queue = NULL;

    if (pIoContext->ControlQueue == NULL) {

        WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchSequential);

        //Sequential must specify this callback
        queueConfig.EvtIoInternalDeviceControl = IoEvtControlUrb;

        status = WdfIoQueueCreate(Device,
                                    &queueConfig,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &pIoContext->ControlQueue);

        if (!NT_SUCCESS(status)) {

            LogError(TRACE_FLAG_Driver,"WdfIoQueueCreate failed for control queue %!STATUS!", status);
            goto exit;
        }
    }

    *Queue = pIoContext->ControlQueue;

exit:

    return status;
}

NTSTATUS
Io_RetrieveInterruptQueue(
    _In_
        WDFDEVICE  Device,
    _Out_
        WDFQUEUE * Queue
    )
{
    NTSTATUS status;
    PIO_CONTEXT pIoContext;
    WDF_IO_QUEUE_CONFIG queueConfig;
    WDF_OBJECT_ATTRIBUTES lockAttributes;
    //WDF_OBJECT_ATTRIBUTES EncapsulatedCommandResponseLockAttributes;

    pIoContext = WdfDeviceGetIoContext(Device);

    status = STATUS_SUCCESS;
    *Queue = NULL;

    if (pIoContext->InterruptUrbQueue == NULL) {

        WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchManual);

        status = WdfIoQueueCreate(Device,
                                    &queueConfig,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &pIoContext->InterruptUrbQueue);

        if (!NT_SUCCESS(status)) {

            LogError(TRACE_FLAG_Driver,"WdfIoQueueCreate failed for interrupt queue %!STATUS!", status);
            goto exit;
        }

        status = WdfIoQueueReadyNotify(pIoContext->InterruptUrbQueue,
                                        IoEvtInterruptUrbQueueReady,
                                        NULL);

        if (!NT_SUCCESS(status)) {

            LogError(TRACE_FLAG_Driver,"WdfIoQueueReadyNotify failed for interrupt queue %!STATUS!", status);
            goto exit;
        }
    }

    if (pIoContext->InProgressLock == NULL) {
        WDF_OBJECT_ATTRIBUTES_INIT(&lockAttributes);
        lockAttributes.ParentObject = Device;
        status = WdfSpinLockCreate(&lockAttributes, &pIoContext->InProgressLock);
        if (!NT_SUCCESS(status)) {
            LogError(TRACE_FLAG_Driver,"Failed to create spinlock InProgressLock %!STATUS!", status);
            goto exit;
        }
    }

    if (pIoContext->EncapsulatedCommandResponse.Flink == NULL)
    {
        InitializeListHead(&pIoContext->AvailableInterrupt);
        InitializeListHead(&pIoContext->EncapsulatedCommandResponse);
    }

    *Queue = pIoContext->InterruptUrbQueue;

exit:

    return status;
}

#ifdef USB_MBIM
NTSTATUS
Io_RetrieveBulkInQueue(
    _In_
    WDFDEVICE  Device,
    _Out_
    WDFQUEUE * Queue
)
{
    NTSTATUS status;
    PIO_CONTEXT pIoContext;
    WDF_IO_QUEUE_CONFIG queueConfig;

    pIoContext = WdfDeviceGetIoContext(Device);

    status = STATUS_SUCCESS;
    *Queue = NULL;

    if (pIoContext->BulkInQueue == NULL) {

        WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchManual);

        status = WdfIoQueueCreate(Device,
            &queueConfig,
            WDF_NO_OBJECT_ATTRIBUTES,
            &pIoContext->BulkInQueue);

        if (!NT_SUCCESS(status)) {

            LogError(TRACE_FLAG_Driver,"WdfIoQueueCreate failed for control queue %!STATUS!", status);
            goto exit;
        }
    }

    *Queue = pIoContext->BulkInQueue;

exit:

    return status;
}

NTSTATUS
Io_RetrieveBulkOutQueue(
    _In_
    WDFDEVICE  Device,
    _Out_
    WDFQUEUE * Queue
)
{
    NTSTATUS status;
    PIO_CONTEXT pIoContext;
    WDF_IO_QUEUE_CONFIG queueConfig;

    pIoContext = WdfDeviceGetIoContext(Device);

    status = STATUS_SUCCESS;
    *Queue = NULL;

    if (pIoContext->BulkOutQueue == NULL) {

        WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchManual);

        status = WdfIoQueueCreate(Device,
            &queueConfig,
            WDF_NO_OBJECT_ATTRIBUTES,
            &pIoContext->BulkOutQueue);

        if (!NT_SUCCESS(status)) {

            LogError(TRACE_FLAG_Driver,"WdfIoQueueCreate failed for control queue %!STATUS!", status);
            goto exit;
        }
    }

    *Queue = pIoContext->BulkOutQueue;

exit:

    return status;
}
#endif

VOID
IoEvtControlUrb(
    _In_ 
        WDFQUEUE Queue,
    _In_
        WDFREQUEST Request,
    _In_
        size_t OutputBufferLength,
    _In_
        size_t InputBufferLength,
    _In_
        ULONG IoControlCode
)
/*++

Routine Description:

    Handle all applicable USB MBIM control requests.

Arguments:

    Queue - Queue on which the request arrived

    Request - Request to process

    OutputBufferLength - Ignored

    InputBufferLength - Ignored

    IoControlCode - IOCTL_INTERNAL_USB_SUBMIT_URB

Return Value:

    NTSTATUS

--*/
{
    WDF_USB_CONTROL_SETUP_PACKET setupPacket;
    NTSTATUS status;
    PUCHAR transferBuffer;
    ULONG transferBufferLength, transferedLength = 0;
    WDFDEVICE wdfDevice;
    PUSB_CONTEXT usbContext;
    PIO_CONTEXT  ioContext;
//    ULONG lengthRead = 0;

    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    NT_VERIFY(IoControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB);
    
    wdfDevice = WdfIoQueueGetDevice(Queue);
    usbContext = WdfDeviceGetUsbContext(wdfDevice);
    ioContext = WdfDeviceGetIoContext(wdfDevice);

    status = UdecxUrbRetrieveBuffer(Request, &transferBuffer, &transferBufferLength);

    if (!NT_SUCCESS(status)) {

        //
        // Could mean there is no buffer on the request
        //
        transferBuffer = NULL;
        transferBufferLength = 0;
        status = STATUS_SUCCESS;
    }
    //Initlize transferedLength and update it later to real length if it was changed.
    transferedLength = transferBufferLength;
    status = UdecxUrbRetrieveControlSetupPacket(Request, &setupPacket);

    if (!NT_SUCCESS(status)) {

        LogError(TRACE_FLAG_Driver,"WdfRequest %p is not a control URB? UdecxUrbRetrieveControlSetupPacket %!STATUS!",
                    Request, status);
        UdecxUrbCompleteWithNtStatus(Request, status);
        goto exit;
    }

    if (setupPacket.Packet.bm.Request.Recipient != BmRequestToInterface ||
        setupPacket.Packet.bm.Request.Type != BmRequestClass)
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        LogError(TRACE_FLAG_Driver, "Unrecognized control request 0x%02x 0x%02x wValue: 0x%04x wIndex: 0x%04x "
            "wLength: 0x%04x %!STATUS!", setupPacket.Packet.bm.Byte,
            setupPacket.Packet.bRequest, setupPacket.Packet.wValue.Value,
            setupPacket.Packet.wIndex.Value, setupPacket.Packet.wLength, status);
        goto exit;
    }
    else if (setupPacket.Packet.wIndex.Value != g_CommunicationInterfaceIndex)
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        LogError(TRACE_FLAG_Driver, "Unrecognized control request whose wIndex (0x%x) is not g_CommunicationInterfaceIndex (0x%x) "
            , setupPacket.Packet.wIndex.Value, g_CommunicationInterfaceIndex);
        goto exit;
    }

    if (setupPacket.Packet.bm.Request.Dir == BmRequestDeviceToHost)
    {
        switch (setupPacket.Packet.bRequest)
        {
        case GET_NTB_PARAMETERS:
            status = GetNtbParameters(transferBuffer, transferBufferLength, &transferedLength);
            break;
        case GET_ENCAPSULATE_RESPONSE:
            status = GetEncapsulatedCommand(ioContext, setupPacket.Packet.wValue.Value, transferBuffer, transferBufferLength, &transferedLength);
            break;
        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
            LogError(TRACE_FLAG_Driver, "Unrecognized control request 0x%02x 0x%02x wValue: 0x%04x wIndex: 0x%04x "
                "wLength: 0x%04x %!STATUS!", setupPacket.Packet.bm.Byte,
                setupPacket.Packet.bRequest, setupPacket.Packet.wValue.Value,
                setupPacket.Packet.wIndex.Value, setupPacket.Packet.wLength, status);
            goto exit;
        }
    }
    else
    {
        switch (setupPacket.Packet.bRequest)
        {
        case SEND_ENCAPSULATE_COMMAND:
            status = SendEncapsulatedCommand(ioContext, setupPacket.Packet.wValue.Value, transferBuffer, transferBufferLength);
            break;
        case RESET_FUNCTION:
            status = ResetAdapter();
            break;
        case SET_NTB_INPUT_SIZE:
            status = SetNtbInputSize(usbContext, transferBuffer, transferBufferLength);
            break;
        case SET_NBT_FORMAT:
            status = SetNtbFormat(usbContext, setupPacket.Packet.wValue.Value);
            break;
        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
            LogError(TRACE_FLAG_Driver, "Unrecognized control request 0x%02x 0x%02x wValue: 0x%04x wIndex: 0x%04x "
                "wLength: 0x%04x %!STATUS!", setupPacket.Packet.bm.Byte,
                setupPacket.Packet.bRequest, setupPacket.Packet.wValue.Value,
                setupPacket.Packet.wIndex.Value, setupPacket.Packet.wLength, status);
            goto exit;
        }
    }

exit:
    CompleteURB(Request, transferedLength, status);
    return;
}

VOID
IoEvtInterruptUrbQueueReady(
    _In_ 
        WDFQUEUE Queue,
    _In_
        WDFCONTEXT Context
    )
{
    UNREFERENCED_PARAMETER(Context);
    WDFDEVICE   wdfDevice;
    PIO_CONTEXT     ioContext;
    LogVerbose(TRACE_FLAG_Driver,"Interrupt URB queue ready (new state %!WdfIoQueueState!",
               WdfIoQueueGetState(Queue, NULL, NULL));

    wdfDevice = WdfIoQueueGetDevice(Queue);
    ioContext = WdfDeviceGetIoContext(wdfDevice);
    MbbTryDrainInterrupt(ioContext);
}

VOID CompleteURB(
    _In_ WDFREQUEST request, 
    _In_ ULONG transferBufferLength,
    NTSTATUS status    
    )
{
    if (NT_SUCCESS(status)) {
        UdecxUrbSetBytesCompleted(request, transferBufferLength);
    }

    UdecxUrbCompleteWithNtStatus(request, status);
}