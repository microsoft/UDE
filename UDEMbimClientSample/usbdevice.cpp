/*++
Copyright (c) Microsoft Corporation

Module Name:

    usbdevice.cpp

Abstract:


--*/

#include "pch.h"

#include "usbdevice.tmh"

NTSTATUS
Usb_Initialize(
    _In_
        WDFDEVICE WdfDevice
    )
{
    NTSTATUS                                status;
    PUSB_CONTEXT                            usbContext;
    UDECX_USB_DEVICE_STATE_CHANGE_CALLBACKS   callbacks;


    usbContext = WdfDeviceGetUsbContext(WdfDevice);

    UsbValidateConstants();

    usbContext->UdecxUsbDeviceInit = UdecxUsbDeviceInitAllocate(WdfDevice);

    if (usbContext->UdecxUsbDeviceInit == NULL) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(TRACE_FLAG_Driver, "Failed to allocate UDECXUSBDEVICE_INIT %!STATUS!", status);
        goto exit;
    }

    //
    // State changed callbacks
    //
    UDECX_USB_DEVICE_CALLBACKS_INIT(&callbacks);

    callbacks.EvtUsbDeviceLinkPowerEntry = UsbDevice_EvtUsbDeviceLinkPowerEntry;
    callbacks.EvtUsbDeviceLinkPowerExit = UsbDevice_EvtUsbDeviceLinkPowerExit;
    callbacks.EvtUsbDeviceSetFunctionSuspendAndWake = UsbDevice_EvtUsbDeviceSetFunctionSuspendAndWake;

    UdecxUsbDeviceInitSetStateChangeCallbacks(usbContext->UdecxUsbDeviceInit, &callbacks);

    //
    // Set required attributes.
    //
    UdecxUsbDeviceInitSetSpeed(usbContext->UdecxUsbDeviceInit, UdecxUsbLowSpeed);

    UdecxUsbDeviceInitSetEndpointsType(usbContext->UdecxUsbDeviceInit, UdecxEndpointTypeSimple);

    //
    // Device descriptor
    //
    status = UdecxUsbDeviceInitAddDescriptor(usbContext->UdecxUsbDeviceInit,
                                           (PUCHAR)g_UsbDeviceDescriptor,
                                           sizeof(g_UsbDeviceDescriptor));

    if (!NT_SUCCESS(status)) {

        goto exit;
    }

#ifdef USB30
    //
    // BOS descriptor
    //
    status = UdecxUsbDeviceInitAddDescriptor(usbContext->UdecxUsbDeviceInit,
                                           (PUCHAR)g_UsbBOSDescriptor,
                                           sizeof(g_UsbBOSDescriptor));

    if (!NT_SUCCESS(status)) {

        goto exit;
    }
#endif

    //
    // String descriptors
    //
    status = UdecxUsbDeviceInitAddDescriptorWithIndex(usbContext->UdecxUsbDeviceInit,
                                                    (PUCHAR)g_LanguageDescriptor,
                                                    sizeof(g_LanguageDescriptor),
                                                    0);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }

    status = UdecxUsbDeviceInitAddStringDescriptor(usbContext->UdecxUsbDeviceInit,
                                                 &g_ManufacturerStringEnUs,
                                                 g_ManufacturerIndex,
                                                 AMERICAN_ENGLISH);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }

    status = UdecxUsbDeviceInitAddStringDescriptor(usbContext->UdecxUsbDeviceInit,
                                                 &g_ProductStringEnUs,
                                                 g_ProductIndex,
                                                 AMERICAN_ENGLISH);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }

    //
    // Remaining init requires lower edge interaction.  Postpone to Usb_ReadDescriptorsAndPlugIn.
    //

exit:

    //
    // On failure in this function (or later but still before creating the UDECXUSBDEVICE),
    // UdecxUsbDeviceInit will be freed by Usb_Destroy.
    //

    return status;
}

NTSTATUS
Usb_ReadDescriptorsAndPlugIn(
    _In_
        WDFDEVICE WdfDevice
    )
{
    NTSTATUS                        status;
    PUSB_CONTEXT                    usbContext;
    PUSB_CONFIGURATION_DESCRIPTOR   pComputedConfigDescSet;
    WDF_OBJECT_ATTRIBUTES           attributes;
    PUDECX_USBDEVICE_CONTEXT          deviceContext;
    UDECX_USB_DEVICE_PLUG_IN_OPTIONS  pluginOptions;
    usbContext = WdfDeviceGetUsbContext(WdfDevice);
    pComputedConfigDescSet = NULL;

    //
    // Compute configuration descriptor dynamically.
    //
    pComputedConfigDescSet = (PUSB_CONFIGURATION_DESCRIPTOR)
        ExAllocatePoolWithTag(NonPagedPoolNx, sizeof(g_UsbConfigDescriptorSet), UDECXMBIM_POOL_TAG);

    if (pComputedConfigDescSet == NULL) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(TRACE_FLAG_Driver,"Failed to allocate %d bytes for temporary config descriptor %!STATUS!",
                    sizeof(g_UsbConfigDescriptorSet), status);
        goto exit;
    }

    RtlCopyMemory(pComputedConfigDescSet,
                    g_UsbConfigDescriptorSet,
                    sizeof(g_UsbConfigDescriptorSet));

    status = UdecxUsbDeviceInitAddDescriptor(usbContext->UdecxUsbDeviceInit,
                                           (PUCHAR) pComputedConfigDescSet,
                                           sizeof(g_UsbConfigDescriptorSet));

    if (!NT_SUCCESS(status)) {

        goto exit;
    }

    //
    // Create emulated USB device
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, UDECX_USBDEVICE_CONTEXT);

    status = UdecxUsbDeviceCreate(&usbContext->UdecxUsbDeviceInit,
                                &attributes,
                                &usbContext->VirtualMbimDevice);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }

    deviceContext = UdecxDeviceGetContext(usbContext->VirtualMbimDevice);
    deviceContext->WdfDevice = WdfDevice;

    //
    // Create static endpoints.
    //
    status = UsbCreateControlEndpoint(WdfDevice);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }
#ifdef USB_MBIM
    status = UsbCreateInterruptEndpoint(WdfDevice);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }
    status = UsbCreateBulkInEndpoint(WdfDevice);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }

    status = UsbCreateBulkOutEndpoint(WdfDevice);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }
#ifdef USB30_COMPOSITE
    //Interface 0
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress0_BulkIn);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress0_BulkOut);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    ////Interface 1
    //status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress1_Interrupt);
    //if (!NT_SUCCESS(status)) {
    //    goto exit;
    //}
    //status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress1_BulkIn);
    //if (!NT_SUCCESS(status)) {
    //    goto exit;
    //}
    //status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress1_BulkOut);
    //if (!NT_SUCCESS(status)) {
    //    goto exit;
    //}

    ////Interface 2
    //status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress2_Interrupt);
    //if (!NT_SUCCESS(status)) {
    //    goto exit;
    //}
    //status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress2_BulkIn);
    //if (!NT_SUCCESS(status)) {
    //    goto exit;
    //}
    //status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress2_BulkOut);
    //if (!NT_SUCCESS(status)) {
    //    goto exit;
    //}

    ////Interface 3
    //status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress3_Interrupt);
    //if (!NT_SUCCESS(status)) {
    //    goto exit;
    //}
    //status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress3_BulkIn);
    //if (!NT_SUCCESS(status)) {
    //    goto exit;
    //}
    //status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress3_BulkOut);
    //if (!NT_SUCCESS(status)) {
    //    goto exit;
    //}
#elif defined(USB20_COMPOSITE)
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress0_BulkIn);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress0_BulkOut);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //Interface 1
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress1_Interrupt);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress1_BulkIn);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress1_BulkOut);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //Interface 2
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress2_Interrupt);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress2_BulkIn);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress2_BulkOut);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //Interface 3
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress3_Interrupt);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress3_BulkIn);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MSEndpointAddress3_BulkOut);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }
#endif
#endif
#ifdef USB_MOUSE
    status = UsbDevice_CreateAndAddEndpoint(WdfDevice, WdfIoQueueDispatchManual, g_MouseInterruptEndpointAddress);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }
#endif
//#endif
    //
    // This begins USB communication and prevents us from modifying descriptors and simple endpoints.
    //
    UDECX_USB_DEVICE_PLUG_IN_OPTIONS_INIT(&pluginOptions);
#ifdef USB30
    pluginOptions.Usb30PortNumber = 2;
#else
    pluginOptions.Usb20PortNumber = 1;
#endif
    status = UdecxUsbDevicePlugIn(usbContext->VirtualMbimDevice, &pluginOptions);

exit:

    //
    // Free temporary allocation always.
    //
    if (pComputedConfigDescSet != NULL) {

        ExFreePoolWithTag(pComputedConfigDescSet, UDECXMBIM_POOL_TAG);
        pComputedConfigDescSet = NULL;
    }

    return status;
}

NTSTATUS
Usb_Disconnect(
    _In_
        WDFDEVICE WdfDevice
    )
{
    NTSTATUS status;
    PUSB_CONTEXT pUsbContext;


    pUsbContext = WdfDeviceGetUsbContext(WdfDevice);

    status = UdecxUsbDevicePlugOutAndDelete(pUsbContext->VirtualMbimDevice);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }

exit:

    return status;
}


VOID
Usb_Destroy(
    _In_
        WDFDEVICE WdfDevice
    )
{
    PUSB_CONTEXT pUsbContext;

    pUsbContext = WdfDeviceGetUsbContext(WdfDevice);

    //
    // Free device init in case we didn't successfully create the device.
    //
    if (pUsbContext!=NULL && pUsbContext->UdecxUsbDeviceInit != NULL) {

        UdecxUsbDeviceInitFree(pUsbContext->UdecxUsbDeviceInit);
        pUsbContext->UdecxUsbDeviceInit = NULL;
    }

    return;
}

VOID
Usb_UdecxUsbEndpointEvtReset(
    _In_ UCXCONTROLLER,
    _In_ UCXENDPOINT,
    _In_ WDFREQUEST
    )
{
    // TODO: endpoint reset. will require a different function prototype
}

NTSTATUS
UsbCreateControlEndpoint(
    _In_
        WDFDEVICE WdfDevice
    )
{
    NTSTATUS                    status;
    PUSB_CONTEXT                pUsbContext;
    WDFQUEUE                    controlQueue;
    UDECX_USB_ENDPOINT_CALLBACKS  callbacks;
    PUDECXUSBENDPOINT_INIT        endpointInit;

    UNREFERENCED_PARAMETER(callbacks);

    pUsbContext = WdfDeviceGetUsbContext(WdfDevice);
    endpointInit = NULL;

    status = Io_RetrieveControlQueue(WdfDevice, &controlQueue);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }

    endpointInit = UdecxUsbSimpleEndpointInitAllocate(pUsbContext->VirtualMbimDevice);

    if (endpointInit == NULL) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(TRACE_FLAG_Driver,"Failed to allocate endpoint init %!STATUS!", status);
        goto exit;
    }

    UdecxUsbEndpointInitSetEndpointAddress(endpointInit, USB_DEFAULT_ENDPOINT_ADDRESS);

    UDECX_USB_ENDPOINT_CALLBACKS_INIT(&callbacks, UsbEndpointReset);
    UdecxUsbEndpointInitSetCallbacks(endpointInit, &callbacks);

    status = UdecxUsbEndpointCreate(&endpointInit,
        WDF_NO_OBJECT_ATTRIBUTES,
        &pUsbContext->VirtualMbimDeviceControlEndpoint);

    if (!NT_SUCCESS(status)) {

        LogError(TRACE_FLAG_Driver,"UdecxUsbEndpointCreate failed for control endpoint %!STATUS!", status);
        goto exit;
    }

    UdecxUsbEndpointSetWdfIoQueue(pUsbContext->VirtualMbimDeviceControlEndpoint,
        controlQueue);

exit:

    if (endpointInit != NULL) {

        NT_ASSERT(!NT_SUCCESS(status));
        UdecxUsbEndpointInitFree(endpointInit);
        endpointInit = NULL;
    }

    return status;
}

NTSTATUS
UsbCreateInterruptEndpoint(
    _In_
        WDFDEVICE WdfDevice
    )
{
    NTSTATUS                    status;
    PUSB_CONTEXT                pUsbContext;
    PUDECXUSBENDPOINT_INIT        endpointInit;
    WDFQUEUE                    interruptQueue;
    UDECX_USB_ENDPOINT_CALLBACKS  callbacks;

    UNREFERENCED_PARAMETER(callbacks);

    pUsbContext = WdfDeviceGetUsbContext(WdfDevice);
    endpointInit = NULL;

    status = Io_RetrieveInterruptQueue(WdfDevice, &interruptQueue);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }

    endpointInit = UdecxUsbSimpleEndpointInitAllocate(pUsbContext->VirtualMbimDevice);

    if (endpointInit == NULL) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(TRACE_FLAG_Driver,"Failed to allocate endpoint init %!STATUS!", status);
        goto exit;
    }

    UdecxUsbEndpointInitSetEndpointAddress(endpointInit, g_InterruptEndpointAddress);

    UDECX_USB_ENDPOINT_CALLBACKS_INIT(&callbacks, UsbEndpointReset);
    UdecxUsbEndpointInitSetCallbacks(endpointInit, &callbacks);

    status = UdecxUsbEndpointCreate(&endpointInit,
        WDF_NO_OBJECT_ATTRIBUTES,
        &pUsbContext->VirtualMbimDeviceInterruptEndpoint);

    if (!NT_SUCCESS(status)) {

        LogError(TRACE_FLAG_Driver,"UdecxUsbEndpointCreate failed for interrupt endpoint %!STATUS!", status);
        goto exit;
    }

    UdecxUsbEndpointSetWdfIoQueue(pUsbContext->VirtualMbimDeviceInterruptEndpoint,
        interruptQueue);

exit:

    if (endpointInit != NULL) {

        NT_ASSERT(!NT_SUCCESS(status));
        UdecxUsbEndpointInitFree(endpointInit);
        endpointInit = NULL;
    }

    return status;
}

#ifdef USB_MBIM
NTSTATUS
UsbCreateBulkInEndpoint(
    _In_
    WDFDEVICE WdfDevice
)
{
    NTSTATUS                    status;
    PUSB_CONTEXT                pUsbContext;
    WDFQUEUE                    bulkInQueue;
    UDECX_USB_ENDPOINT_CALLBACKS  callbacks;
    PUDECXUSBENDPOINT_INIT        endpointInit;

    UNREFERENCED_PARAMETER(callbacks);

    pUsbContext = WdfDeviceGetUsbContext(WdfDevice);
    endpointInit = NULL;

    status = Io_RetrieveBulkInQueue(WdfDevice, &bulkInQueue);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }

    endpointInit = UdecxUsbSimpleEndpointInitAllocate(pUsbContext->VirtualMbimDevice);

    if (endpointInit == NULL) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(TRACE_FLAG_Driver,"Failed to allocate endpoint init %!STATUS!", status);
        goto exit;
    }

    UdecxUsbEndpointInitSetEndpointAddress(endpointInit, g_BulkInEndpointAddress);

    UDECX_USB_ENDPOINT_CALLBACKS_INIT(&callbacks, UsbEndpointReset);
    UdecxUsbEndpointInitSetCallbacks(endpointInit, &callbacks);

    status = UdecxUsbEndpointCreate(&endpointInit,
        WDF_NO_OBJECT_ATTRIBUTES,
        &pUsbContext->VirtualMbimDeviceBulkInEndpoint);

    if (!NT_SUCCESS(status)) {

        LogError(TRACE_FLAG_Driver,"UdecxUsbEndpointCreate failed for control endpoint %!STATUS!", status);
        goto exit;
    }

    UdecxUsbEndpointSetWdfIoQueue(pUsbContext->VirtualMbimDeviceBulkInEndpoint,
        bulkInQueue);

exit:

    if (endpointInit != NULL) {

        NT_ASSERT(!NT_SUCCESS(status));
        UdecxUsbEndpointInitFree(endpointInit);
        endpointInit = NULL;
    }

    return status;
}

NTSTATUS
UsbCreateBulkOutEndpoint(
    _In_
    WDFDEVICE WdfDevice
)
{
    NTSTATUS                    status;
    PUSB_CONTEXT                pUsbContext;
    WDFQUEUE                    bulkOutQueue;
    UDECX_USB_ENDPOINT_CALLBACKS  callbacks;
    PUDECXUSBENDPOINT_INIT        endpointInit;

    UNREFERENCED_PARAMETER(callbacks);

    pUsbContext = WdfDeviceGetUsbContext(WdfDevice);
    endpointInit = NULL;

    status = Io_RetrieveBulkOutQueue(WdfDevice, &bulkOutQueue);

    if (!NT_SUCCESS(status)) {

        goto exit;
    }

    endpointInit = UdecxUsbSimpleEndpointInitAllocate(pUsbContext->VirtualMbimDevice);

    if (endpointInit == NULL) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(TRACE_FLAG_Driver,"Failed to allocate endpoint init %!STATUS!", status);
        goto exit;
    }

    UdecxUsbEndpointInitSetEndpointAddress(endpointInit, g_BulkOutEndpointAddress);

    UDECX_USB_ENDPOINT_CALLBACKS_INIT(&callbacks, UsbEndpointReset);
    UdecxUsbEndpointInitSetCallbacks(endpointInit, &callbacks);

    status = UdecxUsbEndpointCreate(&endpointInit,
        WDF_NO_OBJECT_ATTRIBUTES,
        &pUsbContext->VirtualMbimDeviceBulkOutEndpoint);

    if (!NT_SUCCESS(status)) {

        LogError(TRACE_FLAG_Driver,"UdecxUsbEndpointCreate failed for control endpoint %!STATUS!", status);
        goto exit;
    }

    UdecxUsbEndpointSetWdfIoQueue(pUsbContext->VirtualMbimDeviceBulkOutEndpoint,
        bulkOutQueue);

exit:

    if (endpointInit != NULL) {

        NT_ASSERT(!NT_SUCCESS(status));
        UdecxUsbEndpointInitFree(endpointInit);
        endpointInit = NULL;
    }

    return status;
}
#endif

VOID
UsbEndpointReset(
    _In_
        UDECXUSBENDPOINT UdecxUsbEndpoint,
    _In_
        WDFREQUEST     Request
)
{
    UNREFERENCED_PARAMETER(UdecxUsbEndpoint);
    UNREFERENCED_PARAMETER(Request);
}

NTSTATUS
UsbDevice_CreateAndAddEndpoint(
    _In_ WDFDEVICE wdfDevice,
    _In_ WDF_IO_QUEUE_DISPATCH_TYPE queueDispatchType,
    _In_ UCHAR endpointAddress
)
{
    NTSTATUS                    status;
    PUSB_CONTEXT                pUsbContext;
    WDF_IO_QUEUE_CONFIG         queueConfig;
    WDFQUEUE                    queue;
    UDECX_USB_ENDPOINT_CALLBACKS  callbacks;
    PUDECXUSBENDPOINT_INIT        endpointInit;
    UDECXUSBENDPOINT              endpoint;


    UNREFERENCED_PARAMETER(callbacks);

    pUsbContext = WdfDeviceGetUsbContext(wdfDevice);
    endpointInit = NULL;
    
    WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, queueDispatchType);

    status = WdfIoQueueCreate(wdfDevice,
        &queueConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        &queue);

    if (!NT_SUCCESS(status)) {

        LogError(TRACE_FLAG_Driver, "WdfIoQueueCreate failed for control queue %!STATUS!", status);
        goto exit;
    }
    
    endpointInit = UdecxUsbSimpleEndpointInitAllocate(pUsbContext->VirtualMbimDevice);

    if (endpointInit == NULL) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(TRACE_FLAG_Driver, "Failed to allocate endpoint init %!STATUS!", status);
        goto exit;
    }

    UdecxUsbEndpointInitSetEndpointAddress(endpointInit, endpointAddress);

    UDECX_USB_ENDPOINT_CALLBACKS_INIT(&callbacks, UsbEndpointReset);
    UdecxUsbEndpointInitSetCallbacks(endpointInit, &callbacks);

    status = UdecxUsbEndpointCreate(&endpointInit,
        WDF_NO_OBJECT_ATTRIBUTES,
        &endpoint);

    if (!NT_SUCCESS(status)) {

        LogError(TRACE_FLAG_Driver, "UdecxUsbEndpointCreate failed for control endpoint %!STATUS!", status);
        goto exit;
    }

    UdecxUsbEndpointSetWdfIoQueue(endpoint,
        queue);

exit:

    if (endpointInit != NULL) {

        NT_ASSERT(!NT_SUCCESS(status));
        UdecxUsbEndpointInitFree(endpointInit);
        endpointInit = NULL;
    }

    return status;
}

VOID
UsbDevice_EvtUsbDeviceEndpointsConfigure(
    _In_
        UDECXUSBDEVICE                    UdecxUsbDevice,
    _In_
        WDFREQUEST                      Request,
    _In_
        PUDECX_ENDPOINTS_CONFIGURE_PARAMS Params
)
{
    UNREFERENCED_PARAMETER(UdecxUsbDevice);
    UNREFERENCED_PARAMETER(Params);

    WdfRequestComplete(Request, STATUS_SUCCESS);
}

NTSTATUS
UsbDevice_EvtUsbDeviceLinkPowerEntry(
    _In_
        WDFDEVICE       UdecxWdfDevice,
    _In_
        UDECXUSBDEVICE    UdecxUsbDevice
)
{
    UNREFERENCED_PARAMETER(UdecxWdfDevice);
    UNREFERENCED_PARAMETER(UdecxUsbDevice);

    return STATUS_SUCCESS;
}

NTSTATUS
UsbDevice_EvtUsbDeviceLinkPowerExit(
    _In_
        WDFDEVICE                   UdecxWdfDevice,
    _In_
        UDECXUSBDEVICE                UdecxUsbDevice,
    _In_
        UDECX_USB_DEVICE_WAKE_SETTING WakeSetting
)
{
    UNREFERENCED_PARAMETER(UdecxWdfDevice);
    UNREFERENCED_PARAMETER(UdecxUsbDevice);
    UNREFERENCED_PARAMETER(WakeSetting);

    return STATUS_SUCCESS;
}

NTSTATUS
UsbDevice_EvtUsbDeviceSetFunctionSuspendAndWake(
    _In_
        WDFDEVICE                      UdecxWdfDevice,
    _In_
        UDECXUSBDEVICE                   UdecxUsbDevice,
    _In_
        ULONG                          Interface,
    _In_
        UDECX_USB_DEVICE_FUNCTION_POWER  FunctionPower
)
{
    UNREFERENCED_PARAMETER(UdecxWdfDevice);
    UNREFERENCED_PARAMETER(UdecxUsbDevice);
    UNREFERENCED_PARAMETER(Interface);
    UNREFERENCED_PARAMETER(FunctionPower);

    return STATUS_SUCCESS;
}