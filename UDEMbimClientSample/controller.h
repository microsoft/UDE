/*++
Copyright (c) Microsoft Corporation

Module Name:

    controller.h

Abstract:


--*/

#pragma once

typedef struct _WDFDEVICE_CONTEXT {

    LIST_ENTRY ControllerListEntry;
    KEVENT ResetCompleteEvent;
    BOOLEAN AllowOnlyResetInterrupts;
    WDFQUEUE DefaultQueue;

} WDFDEVICE_CONTEXT, *PWDFDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WDFDEVICE_CONTEXT, WdfDeviceGetContext);

typedef struct _IO_CONTEXT {

    WDFQUEUE ControlQueue;
    WDFQUEUE InterruptUrbQueue;
    WDFQUEUE BulkInQueue;
    WDFQUEUE BulkOutQueue;
    WDFSPINLOCK InProgressLock;
    LIST_ENTRY AvailableInterrupt;
    LIST_ENTRY EncapsulatedCommandResponse;
} IO_CONTEXT, *PIO_CONTEXT;

typedef struct _BUFFER_CONTENT
{
    LIST_ENTRY  BufferLink;
    PVOID       Buffer;
    ULONG       BufferLength;
} BUFFER_CONTENT, *PBUFFER_CONTENT;

VOID MbbCleanupBufferQueue(_In_ LIST_ENTRY* BufferQueue);
VOID FreeBufferContent(PLIST_ENTRY BufferEntry);

//
// Public functions
//
EVT_WDF_DRIVER_DEVICE_ADD       ControllerWdfEvtDeviceAdd;

//
// Private functions
//
EVT_WDF_DEVICE_PREPARE_HARDWARE                 ControllerWdfEvtDevicePrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE                 ControllerWdfEvtDeviceReleaseHardware;
EVT_WDF_DEVICE_D0_ENTRY                         ControllerWdfEvtDeviceD0Entry;
EVT_WDF_DEVICE_D0_EXIT                          ControllerWdfEvtDeviceD0Exit;
EVT_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED ControllerWdfEvtDeviceD0EntryPostInterruptsEnabled;
EVT_WDF_DEVICE_D0_EXIT_PRE_INTERRUPTS_DISABLED  ControllerWdfEvtDeviceD0ExitPreInterruptsDisabled;
EVT_WDF_OBJECT_CONTEXT_CLEANUP                  ControllerWdfEvtCleanupCallback;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL              ControllerEvtIoDeviceControl;

EVT_UDECX_WDF_DEVICE_QUERY_USB_CAPABILITY         ControllerEvtUdecxWdfDeviceQueryUsbCapability;

NTSTATUS
ControllerCreateWdfDeviceWithNameAndSymLink(
    _Inout_
        PWDFDEVICE_INIT * WdfDeviceInit,
    _In_
        PWDF_OBJECT_ATTRIBUTES WdfDeviceAttributes,
    _Out_
        WDFDEVICE * WdfDevice
    );

typedef struct _REQUEST_CONTEXT {

} REQUEST_CONTEXT, *PREQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE(REQUEST_CONTEXT);

#define RESET_INPUT_REPORT_LENGTH 2

#define USB_HOST_DEVINTERFACE_REF_STRING L"GUID_DEVINTERFACE_USB_HOST_CONTROLLER"

#define MAX_SUFFIX_SIZE                         9*sizeof(WCHAR) // all ULONGs fit in 9 characters
#define BASE_DEVICE_NAME                        L"\\Device\\MBIMUDEClient"
#define BASE_SYMBOLIC_LINK_NAME                 L"\\DosDevices\\MBIMUDEClient"

#define DeviceNameSize                          sizeof(BASE_DEVICE_NAME)+MAX_SUFFIX_SIZE
#define SymLinkNameSize                         sizeof(BASE_SYMBOLIC_LINK_NAME)+MAX_SUFFIX_SIZE
