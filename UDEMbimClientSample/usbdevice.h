/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    usbdevice.h

Environment:

    Kernel-mode Driver Framework

--*/

#pragma once

typedef struct _USB_CONTEXT {

    PUDECXUSBDEVICE_INIT  UdecxUsbDeviceInit;
    UDECXUSBDEVICE        VirtualMbimDevice;
    UDECXUSBENDPOINT      VirtualMbimDeviceControlEndpoint;
    UDECXUSBENDPOINT      VirtualMbimDeviceInterruptEndpoint;
    UDECXUSBENDPOINT      VirtualMbimDeviceBulkInEndpoint;
    UDECXUSBENDPOINT      VirtualMbimDeviceBulkOutEndpoint;
    ULONG                 MaxBulkInTransfer;
    USHORT                NTBFormatSetupValue;
} USB_CONTEXT, *PUSB_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(USB_CONTEXT, WdfDeviceGetUsbContext);

typedef struct _UDECX_USBDEVICE_CONTEXT {

    WDFDEVICE   WdfDevice;
} UDECX_USBDEVICE_CONTEXT, *PUDECX_USBDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(UDECX_USBDEVICE_CONTEXT, UdecxDeviceGetContext);

NTSTATUS
Usb_Initialize(
    _In_
        WDFDEVICE WdfDevice
    );

NTSTATUS
Usb_ReadDescriptorsAndPlugIn(
    _In_
        WDFDEVICE WdfDevice
    );

NTSTATUS
Usb_Disconnect(
    _In_
        WDFDEVICE WdfDevice
    );

VOID
Usb_Destroy(
    _In_
        WDFDEVICE WdfDevice
    );

//
// Private functions
//
NTSTATUS
UsbCreateControlEndpoint(
    _In_
        WDFDEVICE WdfDevice
    );

NTSTATUS
UsbCreateInterruptEndpoint(
    _In_
        WDFDEVICE WdfDevice
    );
NTSTATUS
UsbCreateBulkInEndpoint(
    _In_
    WDFDEVICE WdfDevice
);

NTSTATUS
UsbCreateBulkOutEndpoint(
    _In_
    WDFDEVICE WdfDevice
);

NTSTATUS
UsbDevice_CreateAndAddEndpoint(
    _In_ WDFDEVICE wdfDevice,
    _In_ WDF_IO_QUEUE_DISPATCH_TYPE queueDispatchType,
    _In_ UCHAR endpointAddress
);

EVT_UDECX_USB_DEVICE_ENDPOINTS_CONFIGURE      UsbDevice_EvtUsbDeviceEndpointsConfigure;

EVT_UDECX_USB_DEVICE_D0_ENTRY                         UsbDevice_EvtUsbDeviceLinkPowerEntry;
EVT_UDECX_USB_DEVICE_D0_EXIT                          UsbDevice_EvtUsbDeviceLinkPowerExit;
EVT_UDECX_USB_DEVICE_SET_FUNCTION_SUSPEND_AND_WAKE    UsbDevice_EvtUsbDeviceSetFunctionSuspendAndWake;

EVT_UDECX_USB_ENDPOINT_RESET UsbEndpointReset;

//
// Hard-coded descriptors (copied from real USB device)
//
#ifdef USB20_MOUSE

//Descriptor for "Microsoft USB IntelliMouse Optical"
/*const UCHAR g_UsbDeviceDescriptor[] = {
	0x12, // Descriptor Size
	1, // Device Descriptor Type
	0x10,1, //USB Version
	0, // Device class
	0, // Device sub-class
	0, // Device protocol
	8, // Maxpacket size
	0x5E,	4, // Vendor ID
	0x39,	0, // Product ID 
	0,	3, //Device version
	1, //Manufacture string index
	3, //Product string index
	0, //Serial number string index
	1 //Number of configurations
};

const UCHAR g_UsbConfigDescriptorSet[] = {
	9,2,34,0,1,1,0,160,50,
	9,4,0,0,1,3,1,2,0,
	9,33,0,0,0,0,0,0,0, // HID desc - fill in at runtime
	7,5,129,3,4,0,10
};*/

//Descriptor for "Microsoft USB IntelliMouse Optical"
const UCHAR g_UsbDeviceDescriptor[] = {
	0x12, // Descriptor Size
	1, // Device Descriptor Type
	0x0,2, //USB Version
    0xEF,                            // Device class
    0x02,                            // Device subclass
    0x01,                            // Device protocol
	0x40, // Maxpacket size
	0x5E,	4, // Vendor ID
	0x39,	0, // Product ID 
	0,	3, //Device version
    g_ManufacturerIndex, //Manufacture string index
    g_ProductIndex, //Product string index
	0, //Serial number string index
	1 //Number of configurations
};

const UCHAR g_UsbConfigDescriptorSet[] = {
    // Configuration Descriptor
    9, // Descriptor Size
    2, // Configuration Descriptor
    0x2A, 0, // Length of this descriptor and all sub descriptors
    1, // Number of interfaces
    1, // Configuration number
    0, // Configuration string index
    0xA0,  // Config characteristics
    0x32, // Max power consumption of device (in 2mA unit) : 100mA
    
    // Interface association descriptor
    0x08,                                      // Descriptor size
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, // Interface Association Descriptor Type
    0x00,                                      // Interface number of first associated interface
    0x02,                                      // Number of associated interfaces
    3,                   // Function class code 
    1,                // Function subclass code
    2,                // Function protocol code
    0x00,                                      // Index of string descriptor fos this function

    // Interface Descriptor
	9, // Descriptor size
    4, // Interface Descriptor Type
    0, // Interface number
    0, // Alternate setting number
    1, // Number of endpoints
    3, // Interface class
    1, // Interface sub class
    2, // Interface protocol code
    0, // Interface descriptor string index
    // HID Descriptor
	9, // Descriptor Size
    33, // HID Descriptor Type
    0,0,0,0,0,0,0, // Filled in at runtime
	7, // Descriptor Size
    5, // Endpoint Descriptor Type
    g_MouseInterruptEndpointAddress, // Endpoint address and description
    3, // Interrupt Endpoint Type
    4, 0, // Max packet size = 4 bytes
    10 // Servicing interval for data transfers
};
#elif defined(USB30_MOUSE)
//const UCHAR g_UsbDeviceDescriptor[] = {
//    // Device Descriptor
//    0x12, // Descriptor Size
//    0x01, // Device Descriptor Type
//    0x00, 0x03, // USB 3.0
//    0x00, // Device class
//    0x00, // Device sub-class
//    0x00, // Device protocol
//    0x09, // Maxpacket size for EP0 : 2^9
//    0x5E, 0x04, // Vendor ID
//    0x39, 0x00, // Product ID 
//    0x00, // LSB of firmware version
//    0x03, // MSB of firmware version
//    0x01, // Manufacture string index
//    0x03, // Product string index
//    0x00, // Serial number string index
//    0x01 // Number of configurations
//};
//
//const UCHAR g_UsbConfigDescriptorSet[] = {
//    // Configuration Descriptor
//    0x09, // Descriptor Size
//    0x02, // Configuration Descriptor Type
//    0x28, 0x00, // Length of this descriptor and all sub descriptors // change
//    0x01, // Number of interfaces
//    0x01, // Configuration number
//    0x00, // Configuration string index
//    0xA0, // Config characteristics
//    0x32, // Max power consumption of device (in 2mA unit) : 100mA
//    // Interface Descriptor
//    0x09, // Descriptor size
//    0x04, // Interface Descriptor Type
//    0x00, // Interface number
//    0x00, // Alternate setting number
//    0x01, // Number of endpoints
//    0x03, // Interface class
//    0x01, // Interface sub class
//    0x02, // Interface protocol code
//    0x00, // Interface descriptor string index
//    // HID Descriptor
//    0x09, // Descriptor Size
//    0x21, // HID Descriptor Type
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Filled in at runtime
//    // Endpoint Descriptor for Interrupt Endpoint
//    0x07, // Descriptor Size
//    0x05, // Endpoint Descriptor Type
//    g_MouseInterruptEndpointAddress, // Endpoint address and description
//    0x03, // Interrupt Endpoint Type
//    0x04, 0x00, // Max packet size = 4 bytes
//    0x0A, // Servicing interval for data transfers (125us*10)
//    // Super Speed Endpoint Companion Descriptor for Interrupt Endpoint
//    0x06, // Descriptor Size
//    0x30, // Super Speed Endpoint Companion Descriptor Type
//    0x00, // Max number of packets in a Burst : 0: Burst 1 packet at a time
//    0x00, // Max streams for Bulk Endpoint = 0 (No streams)
//    0x04, 0x00 // Total number of bytes transferred per service interval
//};

const UCHAR g_UsbBOSDescriptor[] = {
    0x05, // Descriptor Size
    0x0F, // BOS Descriptor Type
    0x16, 0x00, // Length of this descriptor and all sub descriptors
    0x02, // Number of device capability descriptors
    // USB 2.0 extension
    0x07, // Descriptor size
    0x10, // Device Capability Descriptor Type
    0x02, // USB 2.0 extension capability type
    0x02, 0x00, 0x00, 0x00, // Supported device level features (only Link Power Management set)
    // Super Speed Device Capability
    0x0A, // Descriptor size
    0x10, // Device Capability Descriptor Type
    0x03, // Super Speed device capability type
    0x00, // Supported device level features
    0x0E, 0x00, // Speeds supported by the device : SS, HS and FS
    0x03, // Functionality support
    0x00, // U1 Device Exit latency
    0x00, 0x00, // U2 Device Exit latency
};
#endif

//
// Generic descriptor asserts
//
FORCEINLINE
VOID
UsbValidateConstants(
    )
{
    //
    // C_ASSERT doesn't treat these expressions as constant, so use NT_ASSERT
    //
    NT_ASSERT(((PUSB_STRING_DESCRIPTOR)g_LanguageDescriptor)->bString[0] == AMERICAN_ENGLISH);
    //NT_ASSERT(((PUSB_STRING_DESCRIPTOR)g_LanguageDescriptor)->bString[1] == PRC_CHINESE);
    NT_ASSERT(((PUSB_DEVICE_DESCRIPTOR)g_UsbDeviceDescriptor)->iManufacturer == g_ManufacturerIndex);
    NT_ASSERT(((PUSB_DEVICE_DESCRIPTOR)g_UsbDeviceDescriptor)->iProduct == g_ProductIndex);

    NT_ASSERT(((PUSB_DEVICE_DESCRIPTOR)g_UsbDeviceDescriptor)->bLength ==
              sizeof(USB_DEVICE_DESCRIPTOR));
    NT_ASSERT(sizeof(g_UsbDeviceDescriptor) == sizeof(USB_DEVICE_DESCRIPTOR));
    NT_ASSERT(((PUSB_CONFIGURATION_DESCRIPTOR)g_UsbConfigDescriptorSet)->wTotalLength ==
              sizeof(g_UsbConfigDescriptorSet));
    NT_ASSERT(((PUSB_STRING_DESCRIPTOR)g_LanguageDescriptor)->bLength ==
              sizeof(g_LanguageDescriptor));

    NT_ASSERT(((PUSB_DEVICE_DESCRIPTOR)g_UsbDeviceDescriptor)->bDescriptorType ==
              USB_DEVICE_DESCRIPTOR_TYPE);
    NT_ASSERT(((PUSB_CONFIGURATION_DESCRIPTOR)g_UsbConfigDescriptorSet)->bDescriptorType ==
              USB_CONFIGURATION_DESCRIPTOR_TYPE);
    NT_ASSERT(((PUSB_STRING_DESCRIPTOR)g_LanguageDescriptor)->bDescriptorType ==
              USB_STRING_DESCRIPTOR_TYPE);
}
