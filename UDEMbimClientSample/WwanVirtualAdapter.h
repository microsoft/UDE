#pragma once

#define MBIM_VERSION_1_0        0x100

#pragma warning(disable: 4995) //Pragma deprecated

typedef enum
{
    NicUninitialized = 0,
    NicInitializing,
    NicInitialized,
    NicDataInitialized,
    NicPausing,
    NicPaused,
    NicShutdown
} NicState;

//Requset is reserved to retrive device object and USB Context
NTSTATUS GetNtbParameters(
    _Out_writes_bytes_(TransferBufferLength) BYTE *TransferBuffer,
    _In_  ULONG TransferBufferLength,
    _Out_ ULONG * TransferedLength
);

NTSTATUS SetNtbInputSize(
    _In_ PUSB_CONTEXT UsbContext,
    _In_reads_bytes_(TransferBufferLength) BYTE *TransferBuffer,
    _In_ ULONG TransferBufferLength
);

NTSTATUS SetNtbFormat(
    _In_ PUSB_CONTEXT UsbContext,
    _In_ USHORT SetupValue
);

NTSTATUS ResetAdapter();

NTSTATUS SendEncapsulatedCommand(
    _In_ PIO_CONTEXT IoContext,
    _In_ USHORT SetupValue,
    _In_reads_bytes_(TransferBufferLength) BYTE *TransferBuffer,
    _In_ ULONG TransferBufferLength
);

NTSTATUS GetEncapsulatedCommand(
    _In_ PIO_CONTEXT IoContext,
    _In_ USHORT SetupValue,
    _Out_writes_bytes_(TransferBufferLength) BYTE *TransferBuffer,
    _In_ ULONG TransferBufferLength,
    _Out_ ULONG * TransferedLength
);

NTSTATUS MbbOpen(_In_ PIO_CONTEXT IoContext, _In_ PMBB_MESSAGE_HEADER MbbMessageHeader);
NTSTATUS MbbCommand(_In_ PIO_CONTEXT IoContext, _In_ PMBB_MESSAGE_HEADER MbbMessageHeader);
NTSTATUS MbbBasicCommand(_In_ PIO_CONTEXT IoContext, _In_ PMBB_COMMAND_HEADER MbbCommandHeader);
NTSTATUS MbbDeviceCaps(_In_ PIO_CONTEXT IoContext, _In_ PMBB_COMMAND_HEADER MbbCommandHeader);
NTSTATUS MbbEnqueBuffer(_In_ LIST_ENTRY* BufferQueue, _In_ PVOID Buffer, _In_ ULONG BufferLength, _In_ BOOL RenewBuffer, _Out_opt_ PLIST_ENTRY* CreatedBufferEntry);
PLIST_ENTRY MbbRemoveBufferFromQueue(_In_ LIST_ENTRY* BufferQueue, _In_ PVOID Buffer);
NTSTATUS MbbEnqueAvailableInterrupt(LIST_ENTRY* InterruptQueue);
NTSTATUS MbbDeviceResponse(_In_ PIO_CONTEXT IoContext, _In_ PVOID Buffer, _In_ ULONG BufferLength, PMBB_COMMAND_HEADER MbbCommandHeader);
NTSTATUS MbbIndicateCommandDone(_In_ PIO_CONTEXT IoContext, _In_ PVOID Buffer, _In_ ULONG BufferLength, PMBB_COMMAND_HEADER MbbCommandHeader);
VOID MbbTryDrainInterrupt(_In_ PIO_CONTEXT IoContext);
