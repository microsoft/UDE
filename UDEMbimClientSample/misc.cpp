/*++
Copyright (c) Microsoft Corporation

Module Name:

    misc.cpp

Abstract:


--*/

#include "pch.h"

#include "misc.tmh"

NTSTATUS
Misc_WdfDeviceAllocateIoContext(
    _In_
        WDFDEVICE Object
    )
/*++

Routine Description:

    Object context allocation helper

Arguments:

    Object - WDF object upon which to allocate the new context

Return value:

    NTSTATUS. Could fail on allocation failure or the same context type already exists on the object

--*/
{
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, IO_CONTEXT);

    status = WdfObjectAllocateContext(Object, &attributes, NULL);

    if (!NT_SUCCESS(status)) {

        LogError(TRACE_FLAG_Driver,"Unable to allocate new context for WDF object %p", Object);
        goto exit;
    }

exit:

    return status;
}

NTSTATUS
Misc_WdfDeviceAllocateUsbContext(
    _In_
        WDFDEVICE Object
    )
/*++

Routine Description:

    Object context allocation helper

Arguments:

    Object - WDF object upon which to allocate the new context

Return value:

    NTSTATUS. Could fail on allocation failure or the same context type already exists on the object

--*/
{
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;
    
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, USB_CONTEXT);

    status = WdfObjectAllocateContext(Object, &attributes, NULL);

    if (!NT_SUCCESS(status)) {

        LogError(TRACE_FLAG_Driver,"Unable to allocate new context for WDF object %p", Object);
        goto exit;
    }

exit:

    return status;
}
