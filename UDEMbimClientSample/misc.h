/*++
Copyright (c) Microsoft Corporation

Module Name:

    misc.h

Abstract:


--*/

#pragma once

NTSTATUS
Misc_WdfDeviceAllocateIoContext(
    _In_
        WDFDEVICE Object
    );

NTSTATUS
Misc_WdfDeviceAllocateUsbContext(
    _In_
        WDFDEVICE Object
    );
