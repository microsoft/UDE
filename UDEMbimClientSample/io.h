//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++

Module Name:

    io.h

Abstract:

   This file contains the declarations for 
   interrupt callbacks and queue callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#pragma once

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(IO_CONTEXT, WdfDeviceGetIoContext);

NTSTATUS
Io_RetrieveControlQueue(
    _In_
        WDFDEVICE Device,
    _Out_
        WDFQUEUE * Queue
    );

NTSTATUS
Io_RetrieveInterruptQueue(
    _In_
        WDFDEVICE Device,
    _Out_
        WDFQUEUE * Queue
    );

NTSTATUS
Io_RetrieveBulkInQueue(
    _In_
    WDFDEVICE Device,
    _Out_
    WDFQUEUE * Queue
);

NTSTATUS
Io_RetrieveBulkOutQueue(
    _In_
    WDFDEVICE Device,
    _Out_
    WDFQUEUE * Queue
);

VOID CompleteURB(
    _In_ WDFREQUEST request,
    _In_ ULONG transferBufferLength,
    NTSTATUS status
);

//
// Private functions
//
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL IoEvtControlUrb;
EVT_WDF_IO_QUEUE_STATE                      IoEvtInterruptUrbQueueReady;
