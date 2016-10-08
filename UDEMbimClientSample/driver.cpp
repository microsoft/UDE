/*++
Copyright (c) Microsoft Corporation

Module Name:

    driver.cpp

Abstract:


--*/

#include "pch.h"

#include "driver.tmh"

//TODO: Mark functions paged in C++
//#ifdef ALLOC_PRAGMA
//#pragma alloc_text(PAGE, DriverEntry)
//#pragma alloc_text(PAGE, DriverCleanup)
//#endif

//
// This variable was used by the USB3 kernel debug extension.
//
PDRIVER_OBJECT  g_UdecxMbimDriverObject;

PWDFDRIVER_UDECXMBIM_CONTEXT g_WdfDriverUdecxMbimContext = NULL;

extern "C"
NTSTATUS
DriverEntry(
    struct _DRIVER_OBJECT   *DriverObject,
    PUNICODE_STRING         RegistryPath
    )
/*++

Routine Description:

    DriverEntry is the first routine called after a driver is loaded,
    and is responsible for initializing the driver.

--*/
{
    NTSTATUS                            status;
    WDFDRIVER                           wdfDriver;
    WDF_OBJECT_ATTRIBUTES               wdfAttributes;
    WDF_DRIVER_CONFIG                   wdfDriverConfig;

    PAGED_CODE();

    g_UdecxMbimDriverObject = DriverObject;

    WPP_INIT_TRACING(DriverObject, RegistryPath);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes,
                                            WDFDRIVER_UDECXMBIM_CONTEXT);

    wdfAttributes.EvtCleanupCallback = DriverCleanup;

    WDF_DRIVER_CONFIG_INIT(&wdfDriverConfig, &ControllerWdfEvtDeviceAdd);
    wdfDriverConfig.DriverPoolTag = UDECXMBIM_POOL_TAG;

    status = WdfDriverCreate(DriverObject,
                                RegistryPath,
                                &wdfAttributes,
                                &wdfDriverConfig,
                                &wdfDriver);

    if (!NT_SUCCESS(status)) {

        //
        // DriverCleanup will not be called since the WdfDriver object
        // failed to create. Clean-up any resources that were created
        // prior to WDF driver creation.
        //
        WPP_CLEANUP(DriverObject);
    }

    //
    // Initialize the global controller list.
    //
    g_WdfDriverUdecxMbimContext = WdfDriverGetUdecxMbimContext(wdfDriver);
    InitializeListHead(&g_WdfDriverUdecxMbimContext->ControllerListHead);
    KeInitializeSpinLock(&g_WdfDriverUdecxMbimContext->ControllerListLock);
    g_WdfDriverUdecxMbimContext->ControllerListCount = 0;

    return status;
}

VOID
DriverCleanup(
    WDFOBJECT   WdfDriver
    )
/*++

Routine Description:

    The driver's EvtCleanupCallback event callback function removes the driver's
    references on an object so that the object can be deleted.

--*/
{
    PAGED_CODE();

    WPP_CLEANUP(WdfDriverWdmGetDriverObject((WDFDRIVER)WdfDriver));

    return;
}
