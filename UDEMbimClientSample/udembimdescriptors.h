#ifndef   __UDEMBIMDESCRIPTOR_H__
#define   __UDEMBIMDESCRIPTOR_H__

//
// Device descriptor for MBIM Composite
//
#ifdef USB30_COMPOSITE
const UCHAR g_UsbDeviceDescriptor[18] = 
{ 
    0x12,                            // Descriptor size
    USB_DEVICE_DESCRIPTOR_TYPE,      // Device descriptor type
    0x00, 0x03,                      // USB 3.0
    0xEF, // Device class
    0x02,                            // Device subclass
    0x01,                            // Device protocol
    //Careful::Maxpacket size must be 9 or bigger for usb3.0, or device descriptor will be invalid
    0x09,                            // Maxpacket size for EP0
    0x5E, 0x04,                      // Vendor ID
    0x35, 0x07,                      // Product ID
    0x00,                            // LSB of firmware revision
    0x03,                            // MSB of firmware revision
    0x01,                            // Manufacture string index
    0x02,                            // Product string index
    0x00,                            // Serial number string index
    0x01                             // Number of configurations
};

//With MBIM + HID Input funciton
const UCHAR g_UsbConfigDescriptorSet[] {
    // Configuration Descriptor Type
    0x09,                              // Descriptor Size
    USB_CONFIGURATION_DESCRIPTOR_TYPE, // Configuration Descriptor Type
    0xA2, 0x00,                        // Length of this descriptor and all sub descriptors
    0x03,                              // Number of interfaces
    0x01,                              // Configuration number
    0x00,                              // Configuration string index
    USB_CONFIG_POWERED_MASK,           // Config characteristics - Self powered, Bit 8 is reserved and must 1
    0x00,                              // Max power consumption of device (in 2mA unit) : 0 ma

    // Interface association descriptor
    0x08,                                      // Descriptor size
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, // Interface Association Descriptor Type
    g_CommunicationInterfaceIndex,             // Interface number of first associated interface
    0x02,                                      // Number of associated interfaces
    MBIM_CC_INTERFACE_CLASS,                   // Function class code 
    MBIM_CC_INTERFACE_SUBCLASS,                // Function subclass code
    MBIM_CC_INTERFACE_PROTOCOL,                // Function protocol code
    0x00,                                      // Index of string descriptor fos this function

    // Interface descriptor (Communications interface)
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    g_CommunicationInterfaceIndex,                           // Interface number
    0x00,                           // Alternate setting number
    0x01,                           // Number of end points
    MBIM_CC_INTERFACE_CLASS,        // Interface class
    MBIM_CC_INTERFACE_SUBCLASS,     // Interface sub class
    MBIM_CC_INTERFACE_PROTOCOL,     // Interface protocol code
    0x00,                           // Interface descriptor string index

    // MBIM functional descriptors
    // CDC Header Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x10,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // CDC Union Interface Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.2
    0x05,                             // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,       // Descriptor type
    USB_CDC_UNION_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x00,                             // Controlling interface for this union
    0x01,                             // Interface number of first subordinate interface

    // Mobile Broadband Interface Model Descriptor
    // USB MBIM Rev 1.0 Sec 6.4
    0x0C,                              // Descriptor size
    USB_CDC_CS_DESCRIPTOR_TYPE,        // Descriptor type
    USB_CDC_CS_MBB_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x00, 0x01,                        // BCD of MBIM version
    0xFF, 0xFF,                        // Maximum control message segment size
    0x10,                              // Number of packet filters
    0xC0,                              // Max filter size
    0xFF, 0xFF,                        // Maximum data message segment size
    0x00,                              // Network capabilities

    //Mobile Broadband Interface Model EXTENDED FUNCTIONAL DESCRIPTOR
    0x08,                             // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,       // Descriptor type
    USB_CDC_CS_MBB_DESCRIPTOR_EXTENDED_SUBTYPE, // Functional descriptor subtype
    0x00, 0x01,                       // MBIMExtendedVersion
    0x14,                             // MaxOutstandingCommandMessages
    0xDC, 0x05,                       // MTU

    // Communications interrupt endpoint
    0x07,                         // Descriptor size
    USB_ENDPOINT_DESCRIPTOR_TYPE, // Descriptor type
    g_InterruptEndpointAddress,   // Endpoint address and description
    USB_ENDPOINT_TYPE_INTERRUPT,  // Endpoint type
    0x00, 0x04,                   // Max packet size = 1024 bytes
    0x01,                         // Servicing interval for data transfers : NA for bulk

    // Super Speed Endpoint Companion Descriptor for comm interrupt EP
    0x06,                                              // Descriptor size
    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE, // SS Endpoint Companion Descriptor Type
    0x00,                                              // Max no. of packets in a Burst : 0: Burst 1 packet at a time
    0x00,                                              // Max streams for Bulk EP = 0 (No streams)
    0x00, 0x04,                                        // Service interval for the EP : NA for Bulk
    
    // Interface Descriptor (Data interface, AltSetting 0)
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    g_DataInterfaceIndex,                           // Interface number MBIM
    0x00,                           // Alternate setting number
    0x00,                           // Number of end points
    MBIM_DC_INTERFACE_CLASS,        // Interface class
    MBIM_DC_INTERFACE_SUBCLASS,     // Interface sub class
    MBIM_DC_INTERFACE_PROTOCOL,     // Interface protocol code
    0x00,                           // Interface descriptor string index
    
    // CDC Header Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x02,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // Interface Descriptor (Data interface)
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    g_DataInterfaceIndex,                           // Interface number MBIM
    0x01,                           // Alternate setting number
    0x02,                           // Number of end points
    MBIM_DC_INTERFACE_CLASS,        // Interface class
    MBIM_DC_INTERFACE_SUBCLASS,     // Interface sub class
    MBIM_DC_INTERFACE_PROTOCOL,     // Interface protocol code
    0x00,                           // Interface descriptor string index
    
    // CDC Header Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x02,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // Bulk IN endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_BulkInEndpointAddress,        // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x04,                     // Max packet size = 1024 bytes
    0x00,                         // Servicing interval for data transfers : NA for bulk

    // Super Speed Endpoint Companion Descriptor for Bulk IN EP
    0x06,                                              // Descriptor size
    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE, // SS Endpoint Companion Descriptor Type
    0x0F,                                              // Max no. of packets in a Burst : 0: Burst 1 packet at a time
    0x00,                                              // Max streams for Bulk EP = 0 (No streams)
    0x00, 0x00,                                        // Service interval for the EP : NA for Bulk

    // Bulk OUT endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_BulkOutEndpointAddress,       // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x04,                     // Max packet size = 1024 bytes
    0x00,                           // Servicing interval for data transfers : NA for bulk

    // Super Speed Endpoint Companion Descriptor for Bulk OUT EP
    0x06,                                              // Descriptor size
    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE, // SS Endpoint Companion Descriptor Type
    0x0F,                                              // Max no. of packets in a Burst : 0: Burst 1 packet at a time
    0x00,                                              // Max streams for Bulk EP = 0 (No streams)
    0x00, 0x00,                                        // Service interval for the EP : NA for Bulk

    // Interface association descriptor
    0x08,                                      // Descriptor size
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, // Interface Association Descriptor Type
    g_MSInterfaceIndex0,                       // Interface number of first associated interface
    0x01,                                      // Number of associated interfaces
    0x3,                   // Function class code 
    0x1,                // Function subclass code
    0x2,                // Function protocol code
    0x00,                                      // Index of string descriptor fos this function

    //USB Input function
    // Interface Descriptor
    0x09, // Descriptor size
    0x04, // Interface Descriptor Type
    g_MSInterfaceIndex0, // Interface number
    0x00, // Alternate setting number
    0x01, // Number of endpoints
    0x03, // Interface class
    0x01, // Interface sub class
    0x02, // Interface protocol code
    0x00, // Interface descriptor string index
    // HID Descriptor
    0x09, // Descriptor Size
    0x21, // HID Descriptor Type
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Filled in at runtime
    // Endpoint Descriptor for Interrupt Endpoint
    0x07, // Descriptor Size
    0x05, // Endpoint Descriptor Type
    g_MSEndpointAddress0_BulkIn, // Endpoint address and description
    0x03, // Interrupt Endpoint Type
    0x04, 0x00, // Max packet size = 4 bytes
    0x0A, // Servicing interval for data transfers (125us*10)
    // Super Speed Endpoint Companion Descriptor for Interrupt Endpoint
    0x06, // Descriptor Size
    0x30, // Super Speed Endpoint Companion Descriptor Type
    0x00, // Max number of packets in a Burst : 0: Burst 1 packet at a time
    0x00, // Max streams for Bulk Endpoint = 0 (No streams)
    0x04, 0x00 // Total number of bytes transferred per service interval
};

// With BMIM + any other function
//const UCHAR g_UsbConfigDescriptorSet[] {
//    // Configuration Descriptor Type
//    0x09,                              // Descriptor Size
//    USB_CONFIGURATION_DESCRIPTOR_TYPE, // Configuration Descriptor Type
//    0xA6, 0x00,                        // Length of this descriptor and all sub descriptors
//    0x03,                              // Number of interfaces
//    0x01,                              // Configuration number
//    0x00,                              // Configuration string index
//    USB_CONFIG_POWERED_MASK,           // Config characteristics - Self powered, Bit 8 is reserved and must 1
//    0x00,                              // Max power consumption of device (in 2mA unit) : 0 ma
//
//    // Interface association descriptor
//    0x08,                                      // Descriptor size
//    USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, // Interface Association Descriptor Type
//    g_CommunicationInterfaceIndex,             // Interface number of first associated interface
//    0x02,                                      // Number of associated interfaces
//    MBIM_CC_INTERFACE_CLASS,                   // Function class code 
//    MBIM_CC_INTERFACE_SUBCLASS,                // Function subclass code
//    MBIM_CC_INTERFACE_PROTOCOL,                // Function protocol code
//    0x00,                                      // Index of string descriptor fos this function
//
//    // Interface descriptor (Communications interface)
//    0x09,                           // Descriptor size
//    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
//    g_CommunicationInterfaceIndex,                           // Interface number
//    0x00,                           // Alternate setting number
//    0x01,                           // Number of end points
//    MBIM_CC_INTERFACE_CLASS,        // Interface class
//    MBIM_CC_INTERFACE_SUBCLASS,     // Interface sub class
//    MBIM_CC_INTERFACE_PROTOCOL,     // Interface protocol code
//    0x00,                           // Interface descriptor string index
//
//    // MBIM functional descriptors
//    // CDC Header Functional Descriptor
//    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
//    0x05,                          // Descriptor size 
//    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
//    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
//    0x10,                          // LSB of Class definition version
//    0x01,                          // MSB of Class definition version
//
//    // CDC Union Interface Functional Descriptor
//    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.2
//    0x05,                             // Descriptor size 
//    USB_CDC_CS_DESCRIPTOR_TYPE,       // Descriptor type
//    USB_CDC_UNION_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
//    0x00,                             // Controlling interface for this union
//    0x01,                             // Interface number of first subordinate interface
//
//    // Mobile Broadband Interface Model Descriptor
//    // USB MBIM Rev 1.0 Sec 6.4
//    0x0C,                              // Descriptor size
//    USB_CDC_CS_DESCRIPTOR_TYPE,        // Descriptor type
//    USB_CDC_CS_MBB_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
//    0x00, 0x01,                        // BCD of MBIM version
//    0xFF, 0xFF,                        // Maximum control message segment size
//    0x10,                              // Number of packet filters
//    0xC0,                              // Max filter size
//    0xFF, 0xFF,                        // Maximum data message segment size
//    0x00,                              // Network capabilities
//
//    //Mobile Broadband Interface Model EXTENDED FUNCTIONAL DESCRIPTOR
//    0x08,                             // Descriptor size 
//    USB_CDC_CS_DESCRIPTOR_TYPE,       // Descriptor type
//    USB_CDC_CS_MBB_DESCRIPTOR_EXTENDED_SUBTYPE, // Functional descriptor subtype
//    0x00, 0x01,                       // MBIMExtendedVersion
//    0x14,                             // MaxOutstandingCommandMessages
//    0xDC, 0x05,                       // MTU
//
//    // Communications interrupt endpoint
//    0x07,                         // Descriptor size
//    USB_ENDPOINT_DESCRIPTOR_TYPE, // Descriptor type
//    g_InterruptEndpointAddress,   // Endpoint address and description
//    USB_ENDPOINT_TYPE_INTERRUPT,  // Endpoint type
//    0x00, 0x04,                   // Max packet size = 1024 bytes
//    0x01,                         // Servicing interval for data transfers : NA for bulk
//
//    // Super Speed Endpoint Companion Descriptor for comm interrupt EP
//    0x06,                                              // Descriptor size
//    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE, // SS Endpoint Companion Descriptor Type
//    0x00,                                              // Max no. of packets in a Burst : 0: Burst 1 packet at a time
//    0x00,                                              // Max streams for Bulk EP = 0 (No streams)
//    0x00, 0x04,                                        // Service interval for the EP : NA for Bulk
//    
//    // Interface Descriptor (Data interface, AltSetting 0)
//    0x09,                           // Descriptor size
//    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
//    g_DataInterfaceIndex,                           // Interface number MBIM
//    0x00,                           // Alternate setting number
//    0x00,                           // Number of end points
//    MBIM_DC_INTERFACE_CLASS,        // Interface class
//    MBIM_DC_INTERFACE_SUBCLASS,     // Interface sub class
//    MBIM_DC_INTERFACE_PROTOCOL,     // Interface protocol code
//    0x00,                           // Interface descriptor string index
//    
//    // CDC Header Functional Descriptor
//    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
//    0x05,                          // Descriptor size 
//    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
//    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
//    0x02,                          // LSB of Class definition version
//    0x01,                          // MSB of Class definition version
//
//    // Interface Descriptor (Data interface)
//    0x09,                           // Descriptor size
//    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
//    g_DataInterfaceIndex,                           // Interface number MBIM
//    0x01,                           // Alternate setting number
//    0x02,                           // Number of end points
//    MBIM_DC_INTERFACE_CLASS,        // Interface class
//    MBIM_DC_INTERFACE_SUBCLASS,     // Interface sub class
//    MBIM_DC_INTERFACE_PROTOCOL,     // Interface protocol code
//    0x00,                           // Interface descriptor string index
//    
//    // CDC Header Functional Descriptor
//    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
//    0x05,                          // Descriptor size 
//    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
//    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
//    0x02,                          // LSB of Class definition version
//    0x01,                          // MSB of Class definition version
//
//    // Bulk IN endpoint
//    0x07,                           // Descriptor size 
//    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
//    g_BulkInEndpointAddress,        // Endpoint address and description
//    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
//    0x00, 0x04,                     // Max packet size = 1024 bytes
//    0x00,                         // Servicing interval for data transfers : NA for bulk
//
//    // Super Speed Endpoint Companion Descriptor for Bulk IN EP
//    0x06,                                              // Descriptor size
//    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE, // SS Endpoint Companion Descriptor Type
//    0x0F,                                              // Max no. of packets in a Burst : 0: Burst 1 packet at a time
//    0x00,                                              // Max streams for Bulk EP = 0 (No streams)
//    0x00, 0x00,                                        // Service interval for the EP : NA for Bulk
//
//    // Bulk OUT endpoint
//    0x07,                           // Descriptor size 
//    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
//    g_BulkOutEndpointAddress,       // Endpoint address and description
//    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
//    0x00, 0x04,                     // Max packet size = 1024 bytes
//    0x00,                           // Servicing interval for data transfers : NA for bulk
//
//    // Super Speed Endpoint Companion Descriptor for Bulk OUT EP
//    0x06,                                              // Descriptor size
//    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE, // SS Endpoint Companion Descriptor Type
//    0x0F,                                              // Max no. of packets in a Burst : 0: Burst 1 packet at a time
//    0x00,                                              // Max streams for Bulk EP = 0 (No streams)
//    0x00, 0x00,                                        // Service interval for the EP : NA for Bulk
//
//    //Another function
//    // Interface association descriptor
//    0x08,                                      // Descriptor size
//    USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, // Interface Association Descriptor Type
//    g_MSInterfaceIndex0,                                      // Interface number of first associated interface
//    0x01,                                      // Number of associated interfaces
//    0xFF,                   // Function class code 
//    0xFF,                // Function subclass code
//    0xFF,                // Function protocol code
//    0x00,                                      // Index of string descriptor fos this function
//
//    // Interface0 Descriptor
//    0x09,                           // Descriptor size
//    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
//    g_MSInterfaceIndex0,           // Interface number MBIM
//    0x00,                           // Alternate setting number
//    0x02,                           // Number of end points
//    0xFF,        // Interface class
//    0xFF,     // Interface sub class
//    0xFF,     // Interface protocol code
//    0x00,                           // Interface descriptor string index
//
//    // Bulk IN endpoint
//    0x07,                           // Descriptor size 
//    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
//    g_MSEndpointAddress0_BulkIn,        // Endpoint address and description
//    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
//    //Careful::Maxpacket size must be 0x0400 or bigger for usb3.0, or device descriptor will be invalid
//    0x00, 0x04,                     // Max packet size = 1024 bytes
//    0x00,                           // Servicing interval for data transfers : NA for bulk
//
//    // Super Speed Endpoint Companion Descriptor for Bulk In EP
//    0x06,                                              // Descriptor size
//    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE, // SS Endpoint Companion Descriptor Type
//    0x0F,                                              // Max no. of packets in a Burst : 0: Burst 1 packet at a time
//    0x00,                                              // Max streams for Bulk EP = 0 (No streams)
//    0x00, 0x00,                                        // Service interval for the EP : NA for Bulk
//
//    // Bulk OUT endpoint
//    0x07,                           // Descriptor size 
//    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
//    g_MSEndpointAddress0_BulkOut,       // Endpoint address and description
//    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
//    0x00, 0x04,                     // Max packet size = 1024 bytes
//    0x0,                           // Servicing interval for data transfers : NA for bulk
//        
//    // Super Speed Endpoint Companion Descriptor for Bulk OUT EP
//    0x06,                                              // Descriptor size
//    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE, // SS Endpoint Companion Descriptor Type
//    0x0F,                                              // Max no. of packets in a Burst : 0: Burst 1 packet at a time
//    0x00,                                              // Max streams for Bulk EP = 0 (No streams)
//    0x00, 0x00,                                        // Service interval for the EP : NA for Bulk
//};

const UCHAR g_UsbBOSDescriptor[] = 
{
    0x05,                    // Descriptor size
    USB_BOS_DESCRIPTOR_TYPE, // Descriptor type
    0x16, 0x00,              // Length of descriptor and all sub descriptors
    0x02,                    // Number of device capability descriptors

    // USB 2.0 extension
    0x07,                                  // Descriptor size
    USB_DEVICE_CAPABILITY_DESCRIPTOR_TYPE, // Device capability type descriptor
    USB_DEVICE_CAPABILITY_USB20_EXTENSION, // USB 2.0 extension capability type
    0x02, 0x00, 0x00, 0x00,                // Supported device level features

    // SuperSpeed device capability
    0x0A,                                  // Descriptor size
    USB_DEVICE_CAPABILITY_DESCRIPTOR_TYPE, // Device capability type descriptor
    USB_DEVICE_CAPABILITY_SUPERSPEED_USB,  // SuperSpeed device capability type
    0x00,                                  // Supported device level features
    0x0E, 0x00,                            // Speeds supported by the device : SS, HS and FS
    0x03,                                  // Functionality support
    0x00,                                  // U1 Device Exit latency
    0x00, 0x00,                            // U2 Device Exit latency
};
#elif defined(USB20_COMPOSITE)
const UCHAR g_UsbDeviceDescriptor[18] = 
{ 
    0x12,                            // Descriptor size
    USB_DEVICE_DESCRIPTOR_TYPE,      // Device descriptor type
    0x00, 0x02,                      // USB 2.0
    0xEF,                            // Device class
    0x02,                            // Device subclass
    0x01,                            // Device protocol
    //0x00,                            // Device class
    //0x00,                            // Device subclass
    //0x00,                            // Device protocol
    0x40,                            // Maxpacket size for EP0
    0x5E, 0x04,                      // Vendor ID
    0x35, 0x07,                      // Product ID
    0x00,                            // LSB of firmware revision
    0x01,                            // MSB of firmware revision
    0x01,                            // Manufacture string index
    0x02,                            // Product string index
    0x00,                            // Serial number string index
    0x01                             // Number of configurations
};

const UCHAR g_UsbConfigDescriptorSet[] {
    // Configuration Descriptor Type
    0x09,                              // Descriptor Size
    USB_CONFIGURATION_DESCRIPTOR_TYPE, // Configuration Descriptor Type
    0x08, 0x01,                        // Length of this descriptor and all sub descriptors
    0x06,                              // Number of interfaces
    0x01,                              // Configuration number
    0x00,                              // Configuration string index
    0xA0,           // Config characteristics - Self powered
    0x64,                              // Max power consumption of device (in 2mA unit) : 0 ma

    // Interface association descriptor
    0x08,                                      // Descriptor size
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, // Interface Association Descriptor Type
    g_CommunicationInterfaceIndex,                                      // Interface number of first associated interface
    0x02,                                      // Number of associated interfaces
    MBIM_CC_INTERFACE_CLASS,                   // Function class code 
    MBIM_CC_INTERFACE_SUBCLASS,                // Function subclass code
    MBIM_CC_INTERFACE_PROTOCOL,                // Function protocol code
    0x00,                                      // Index of string descriptor fos this function

    // Interface descriptor (Communications interface)
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    g_CommunicationInterfaceIndex,  // Interface number
    0x00,                           // Alternate setting number
    0x01,                           // Number of end points
    MBIM_CC_INTERFACE_CLASS,        // Interface class
    MBIM_CC_INTERFACE_SUBCLASS,     // Interface sub class
    MBIM_CC_INTERFACE_PROTOCOL,     // Interface protocol code
    0x00,                           // Interface descriptor string index

    // MBIM functional descriptors
    // CDC Header Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x02,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // CDC Union Interface Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.2
    0x05,                             // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,       // Descriptor type
    USB_CDC_UNION_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x00,                             // Controlling interface for this union
    0x01,                             // Interface number of first subordinate interface

    // Mobile Broadband Interface Model Descriptor
    // USB MBIM Rev 1.0 Sec 6.4
    0x0C,                              // Descriptor size
    USB_CDC_CS_DESCRIPTOR_TYPE,        // Descriptor type
    USB_CDC_CS_MBB_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x00, 0x01,                        // BCD of MBIM version
    0x00, 0x10,                        // Maximum control message segment size
    0x10,                              // Number of packet filters
    0xC0,                              // Max filter size
    0xDC, 0x05,                        // Maximum data message segment size
    0x2D,                              // Network capabilities

    //Mobile Broadband Interface Model EXTENDED FUNCTIONAL DESCRIPTOR
    0x08,                             // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,       // Descriptor type
    USB_CDC_CS_MBB_DESCRIPTOR_EXTENDED_SUBTYPE, // Functional descriptor subtype
    0x00, 0x01,                       // MBIMExtendedVersion
    0x14,                             // MaxOutstandingCommandMessages
    0xDC, 0x05,                       // MTU

    // Communications interrupt endpoint
    0x07,                         // Descriptor size
    USB_ENDPOINT_DESCRIPTOR_TYPE, // Descriptor type
    g_InterruptEndpointAddress,   // Endpoint address and description
    USB_ENDPOINT_TYPE_INTERRUPT,  // Endpoint type
    0x08, 0x00,                   // Max packet size = 1024 bytes
    0x0A,                         // Servicing interval for data transfers : NA for bulk

    // Interface Descriptor (Data interface, AltSetting 0)
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    0x01,                           // Interface number MBIM
    0x00,                           // Alternate setting number
    0x00,                           // Number of end points
    MBIM_DC_INTERFACE_CLASS,        // Interface class
    MBIM_DC_INTERFACE_SUBCLASS,     // Interface sub class
    MBIM_DC_INTERFACE_PROTOCOL,     // Interface protocol code
    0x00,                           // Interface descriptor string index
    
    // CDC Header Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x02,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // Interface Descriptor (Data interface, AltSetting 1)
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    g_DataInterfaceIndex,           // Interface number MBIM
    0x01,                           // Alternate setting number
    0x02,                           // Number of end points
    MBIM_DC_INTERFACE_CLASS,        // Interface class
    MBIM_DC_INTERFACE_SUBCLASS,     // Interface sub class
    MBIM_DC_INTERFACE_PROTOCOL,     // Interface protocol code
    0x00,                           // Interface descriptor string index

    // CDC Header Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x02,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // Bulk IN endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_BulkInEndpointAddress,        // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x02,                     // Max packet size = 1024 bytes
    0x0A,                           // Servicing interval for data transfers : NA for bulk

    // Bulk OUT endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_BulkOutEndpointAddress,       // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x02,                     // Max packet size = 1024 bytes
    0x0A,                           // Servicing interval for data transfers : NA for bulk

    //Another function
    // Interface association descriptor
    0x08,                                      // Descriptor size
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, // Interface Association Descriptor Type
    g_MSInterfaceIndex0,                                      // Interface number of first associated interface
    0x04,                                      // Number of associated interfaces
    0xFF,                   // Function class code 
    0xFF,                // Function subclass code
    0xFF,                // Function protocol code
    0x00,                                      // Index of string descriptor fos this function

    // Interface0 Descriptor
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    g_MSInterfaceIndex0,           // Interface number MBIM
    0x00,                           // Alternate setting number
    0x02,                           // Number of end points
    0xFF,        // Interface class
    0xFF,     // Interface sub class
    0xFF,     // Interface protocol code
    0x00,                           // Interface descriptor string index

    // Bulk IN endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_MSEndpointAddress0_BulkIn,        // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x02,                     // Max packet size = 1024 bytes
    0x00,                           // Servicing interval for data transfers : NA for bulk

    // Bulk OUT endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_MSEndpointAddress0_BulkOut,       // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x02,                     // Max packet size = 1024 bytes
    0x0,                           // Servicing interval for data transfers : NA for bulk

    // Interface1 Descriptor
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    g_MSInterfaceIndex1,           // Interface number MBIM
    0x00,                           // Alternate setting number
    0x03,                           // Number of end points
    0xFF,        // Interface class
    0x00,     // Interface sub class
    0x00,     // Interface protocol code
    0x00,                           // Interface descriptor string index

    // CDC Header Functional Descriptor
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x10,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // CDC Header Functional Descriptor
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    0x01, // Functional descriptor subtype
    0x00,                          // LSB of Class definition version
    0x00,                          // MSB of Class definition version

    // CDC Header Functional Descriptor
    0x04,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    0x02, // Functional descriptor subtype
    0x02,                          // LSB of Class definition version

    // CDC Header Functional Descriptor
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    0x06, // Functional descriptor subtype
    0x00,                          // LSB of Class definition version
    0x00,                          // MSB of Class definition version

    // interrupt endpoint
    0x07,                         // Descriptor size
    USB_ENDPOINT_DESCRIPTOR_TYPE, // Descriptor type
    g_MSEndpointAddress1_Interrupt,   // Endpoint address and description
    USB_ENDPOINT_TYPE_INTERRUPT,  // Endpoint type
    0x0c, 0x00,                   // Max packet size = 1024 bytes
    0x09,                         // Servicing interval for data transfers : NA for bulk

    // Bulk IN endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_MSEndpointAddress1_BulkIn,        // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x02,                     // Max packet size = 1024 bytes
    0x00,                           // Servicing interval for data transfers : NA for bulk

    // Bulk OUT endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_MSEndpointAddress1_BulkOut,       // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x02,                     // Max packet size = 1024 bytes
    0x0,                           // Servicing interval for data transfers : NA for bulk

    // Interface2 Descriptor
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    g_MSInterfaceIndex2,           // Interface number MBIM
    0x00,                           // Alternate setting number
    0x03,                           // Number of end points
    0xFF,        // Interface class
    0x00,     // Interface sub class
    0x00,     // Interface protocol code
    0x00,                           // Interface descriptor string index

    // CDC Header Functional Descriptor
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x10,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // CDC Header Functional Descriptor
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    0x01, // Functional descriptor subtype
    0x00,                          // LSB of Class definition version
    0x00,                          // MSB of Class definition version

    // CDC Header Functional Descriptor
    0x04,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    0x02, // Functional descriptor subtype
    0x02,                          // LSB of Class definition version

    // CDC Header Functional Descriptor
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    0x06, // Functional descriptor subtype
    0x00,                          // LSB of Class definition version
    0x00,                          // MSB of Class definition version

    // interrupt endpoint
    0x07,                         // Descriptor size
    USB_ENDPOINT_DESCRIPTOR_TYPE, // Descriptor type
    g_MSEndpointAddress2_Interrupt,   // Endpoint address and description
    USB_ENDPOINT_TYPE_INTERRUPT,  // Endpoint type
    0x0c, 0x00,                   // Max packet size = 1024 bytes
    0x09,                         // Servicing interval for data transfers : NA for bulk

    // Bulk IN endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_MSEndpointAddress2_BulkIn,        // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x02,                     // Max packet size = 1024 bytes
    0x00,                           // Servicing interval for data transfers : NA for bulk

    // Bulk OUT endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_MSEndpointAddress2_BulkOut,       // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x02,                     // Max packet size = 1024 bytes
    0x0,                           // Servicing interval for data transfers : NA for bulk

    // Interface3 Descriptor
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    g_MSInterfaceIndex3,           // Interface number MBIM
    0x00,                           // Alternate setting number
    0x03,                           // Number of end points
    0xFF,        // Interface class
    0xFF,     // Interface sub class
    0xFF,     // Interface protocol code
    0x00,                           // Interface descriptor string index

    // interrupt endpoint
    0x07,                         // Descriptor size
    USB_ENDPOINT_DESCRIPTOR_TYPE, // Descriptor type
    g_MSEndpointAddress3_Interrupt,   // Endpoint address and description
    USB_ENDPOINT_TYPE_INTERRUPT,  // Endpoint type
    0x0A, 0x00,                   // Max packet size = 1024 bytes
    0x09,                         // Servicing interval for data transfers : NA for bulk

    // Bulk IN endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_MSEndpointAddress3_BulkIn,        // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x02,                     // Max packet size = 1024 bytes
    0x00,                           // Servicing interval for data transfers : NA for bulk

    // Bulk OUT endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_MSEndpointAddress3_BulkOut,       // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x02,                     // Max packet size = 1024 bytes
    0x0,                           // Servicing interval for data transfers : NA for bulk
};
#elif defined(USB20_MBIM)
const UCHAR g_UsbDeviceDescriptor[18] = 
{ 
    0x12,                            // Descriptor size
    USB_DEVICE_DESCRIPTOR_TYPE,      // Device descriptor type
    0x00, 0x02,                      // USB 2.0
    0xEF,                            // Device class
    0x02,                            // Device subclass
    0x01,                            // Device protocol
    0x40,                            // Maxpacket size for EP0
    0x5E, 0x04,                      // Vendor ID
    0x35, 0x07,                      // Product ID
    0x00,                            // LSB of firmware revision
    0x01,                            // MSB of firmware revision
    0x01,                            // Manufacture string index
    0x02,                            // Product string index
    0x00,                            // Serial number string index
    0x01                             // Number of configurations
};

const UCHAR g_UsbConfigDescriptorSet[] {
    // Configuration Descriptor Type
    0x09,                              // Descriptor Size
    USB_CONFIGURATION_DESCRIPTOR_TYPE, // Configuration Descriptor Type
    0x69, 0x00,                        // Length of this descriptor and all sub descriptors
    0x02,                              // Number of interfaces
    0x01,                              // Configuration number
    0x00,                              // Configuration string index
    0xA0,           // Config characteristics - Self powered
    0x64,                              // Max power consumption of device (in 2mA unit) : 0 ma

    // Interface association descriptor
    0x08,                                      // Descriptor size
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, // Interface Association Descriptor Type
    g_CommunicationInterfaceIndex,                                      // Interface number of first associated interface
    0x02,                                      // Number of associated interfaces
    MBIM_CC_INTERFACE_CLASS,                   // Function class code 
    MBIM_CC_INTERFACE_SUBCLASS,                // Function subclass code
    MBIM_CC_INTERFACE_PROTOCOL,                // Function protocol code
    0x00,                                      // Index of string descriptor fos this function

    // Interface descriptor (Communications interface)
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    g_CommunicationInterfaceIndex,  // Interface number
    0x00,                           // Alternate setting number
    0x01,                           // Number of end points
    MBIM_CC_INTERFACE_CLASS,        // Interface class
    MBIM_CC_INTERFACE_SUBCLASS,     // Interface sub class
    MBIM_CC_INTERFACE_PROTOCOL,     // Interface protocol code
    0x00,                           // Interface descriptor string index

    // MBIM functional descriptors
    // CDC Header Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x02,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // CDC Union Interface Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.2
    0x05,                             // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,       // Descriptor type
    USB_CDC_UNION_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x00,                             // Controlling interface for this union
    0x01,                             // Interface number of first subordinate interface

    // Mobile Broadband Interface Model Descriptor
    // USB MBIM Rev 1.0 Sec 6.4
    0x0C,                              // Descriptor size
    USB_CDC_CS_DESCRIPTOR_TYPE,        // Descriptor type
    USB_CDC_CS_MBB_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x00, 0x01,                        // BCD of MBIM version
    0x00, 0x10,                        // Maximum control message segment size
    0x10,                              // Number of packet filters
    0xC0,                              // Max filter size
    0xDC, 0x05,                        // Maximum data message segment size
    0x2D,                              // Network capabilities

    //Mobile Broadband Interface Model EXTENDED FUNCTIONAL DESCRIPTOR
    0x08,                             // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,       // Descriptor type
    USB_CDC_CS_MBB_DESCRIPTOR_EXTENDED_SUBTYPE, // Functional descriptor subtype
    0x00, 0x01,                       // MBIMExtendedVersion
    0x14,                             // MaxOutstandingCommandMessages
    0xDC, 0x05,                       // MTU

    // Communications interrupt endpoint
    0x07,                         // Descriptor size
    USB_ENDPOINT_DESCRIPTOR_TYPE, // Descriptor type
    g_InterruptEndpointAddress,   // Endpoint address and description
    USB_ENDPOINT_TYPE_INTERRUPT,  // Endpoint type
    0x08, 0x00,                   // Max packet size = 1024 bytes
    0x0A,                         // Servicing interval for data transfers : NA for bulk

    // Interface Descriptor (Data interface, AltSetting 0)
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    0x01,                           // Interface number MBIM
    0x00,                           // Alternate setting number
    0x00,                           // Number of end points
    MBIM_DC_INTERFACE_CLASS,        // Interface class
    MBIM_DC_INTERFACE_SUBCLASS,     // Interface sub class
    MBIM_DC_INTERFACE_PROTOCOL,     // Interface protocol code
    0x00,                           // Interface descriptor string index
    
    // CDC Header Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x02,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // Interface Descriptor (Data interface, AltSetting 1)
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    g_DataInterfaceIndex,           // Interface number MBIM
    0x01,                           // Alternate setting number
    0x02,                           // Number of end points
    MBIM_DC_INTERFACE_CLASS,        // Interface class
    MBIM_DC_INTERFACE_SUBCLASS,     // Interface sub class
    MBIM_DC_INTERFACE_PROTOCOL,     // Interface protocol code
    0x00,                           // Interface descriptor string index

    // CDC Header Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x02,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // Bulk IN endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_BulkInEndpointAddress,        // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x02,                     // Max packet size = 1024 bytes
    0x0A,                           // Servicing interval for data transfers : NA for bulk

    // Bulk OUT endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_BulkOutEndpointAddress,       // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x02,                     // Max packet size = 1024 bytes
    0x0A,                           // Servicing interval for data transfers : NA for bulk
};
#elif defined(USB30_MBIM)
const UCHAR g_UsbDeviceDescriptor[18] = 
{ 
    0x12,                            // Descriptor size
    USB_DEVICE_DESCRIPTOR_TYPE,      // Device descriptor type
    0x00, 0x03,                      // USB 3.0
    0xEF, // Device class
    0x02,                            // Device subclass
    0x01,                            // Device protocol
    //Maxpacket size must be 9 for usb3.0, or device descriptor will be invalid
    0x09,                            // Maxpacket size for EP0
    0x5E, 0x04,                      // Vendor ID
    0x35, 0x07,                      // Product ID
    0x00,                            // LSB of firmware revision
    0x03,                            // MSB of firmware revision
    0x01,                            // Manufacture string index
    0x02,                            // Product string index
    0x00,                            // Serial number string index
    0x01                             // Number of configurations
};

const UCHAR g_UsbConfigDescriptorSet[] {
    // Configuration Descriptor Type
    0x09,                              // Descriptor Size
    USB_CONFIGURATION_DESCRIPTOR_TYPE, // Configuration Descriptor Type
    0x7B, 0x00,                        // Length of this descriptor and all sub descriptors
    0x02,                              // Number of interfaces
    0x01,                              // Configuration number
    0x00,                              // Configuration string index
    USB_CONFIG_POWERED_MASK,           // Config characteristics - Self powered, Bit 8 is reserved and must 1
    0x00,                              // Max power consumption of device (in 2mA unit) : 0 ma

    // Interface association descriptor
    0x08,                                      // Descriptor size
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, // Interface Association Descriptor Type
    0x00,                                      // Interface number of first associated interface
    0x02,                                      // Number of associated interfaces
    MBIM_CC_INTERFACE_CLASS,                   // Function class code 
    MBIM_CC_INTERFACE_SUBCLASS,                // Function subclass code
    MBIM_CC_INTERFACE_PROTOCOL,                // Function protocol code
    0x00,                                      // Index of string descriptor fos this function

    // Interface descriptor (Communications interface)
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    0x00,                           // Interface number
    0x00,                           // Alternate setting number
    0x01,                           // Number of end points
    MBIM_CC_INTERFACE_CLASS,        // Interface class
    MBIM_CC_INTERFACE_SUBCLASS,     // Interface sub class
    MBIM_CC_INTERFACE_PROTOCOL,     // Interface protocol code
    0x00,                           // Interface descriptor string index

    // MBIM functional descriptors
    // CDC Header Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x10,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // CDC Union Interface Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.2
    0x05,                             // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,       // Descriptor type
    USB_CDC_UNION_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x00,                             // Controlling interface for this union
    0x01,                             // Interface number of first subordinate interface

    // Mobile Broadband Interface Model Descriptor
    // USB MBIM Rev 1.0 Sec 6.4
    0x0C,                              // Descriptor size
    USB_CDC_CS_DESCRIPTOR_TYPE,        // Descriptor type
    USB_CDC_CS_MBB_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x00, 0x01,                        // BCD of MBIM version
    0xFF, 0xFF,                        // Maximum control message segment size
    0x10,                              // Number of packet filters
    0xC0,                              // Max filter size
    0xFF, 0xFF,                        // Maximum data message segment size
    0x00,                              // Network capabilities

    //Mobile Broadband Interface Model EXTENDED FUNCTIONAL DESCRIPTOR
    0x08,                             // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,       // Descriptor type
    USB_CDC_CS_MBB_DESCRIPTOR_EXTENDED_SUBTYPE, // Functional descriptor subtype
    0x00, 0x01,                       // MBIMExtendedVersion
    0x14,                             // MaxOutstandingCommandMessages
    0xDC, 0x05,                       // MTU

    // Communications interrupt endpoint
    0x07,                         // Descriptor size
    USB_ENDPOINT_DESCRIPTOR_TYPE, // Descriptor type
    g_InterruptEndpointAddress,   // Endpoint address and description
    USB_ENDPOINT_TYPE_INTERRUPT,  // Endpoint type
    0x00, 0x04,                   // Max packet size = 1024 bytes
    0x01,                         // Servicing interval for data transfers : NA for bulk

    // Super Speed Endpoint Companion Descriptor for comm interrupt EP
    0x06,                                              // Descriptor size
    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE, // SS Endpoint Companion Descriptor Type
    0x00,                                              // Max no. of packets in a Burst : 0: Burst 1 packet at a time
    0x00,                                              // Max streams for Bulk EP = 0 (No streams)
    0x00, 0x04,                                        // Service interval for the EP : NA for Bulk
    
    // Interface Descriptor (Data interface, AltSetting 0)
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    0x01,                           // Interface number MBIM
    0x00,                           // Alternate setting number
    0x00,                           // Number of end points
    MBIM_DC_INTERFACE_CLASS,        // Interface class
    MBIM_DC_INTERFACE_SUBCLASS,     // Interface sub class
    MBIM_DC_INTERFACE_PROTOCOL,     // Interface protocol code
    0x00,                           // Interface descriptor string index
    
    // CDC Header Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x02,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // Interface Descriptor (Data interface)
    0x09,                           // Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,  // Interface Descriptor type
    0x01,                           // Interface number MBIM
    0x01,                           // Alternate setting number
    0x02,                           // Number of end points
    MBIM_DC_INTERFACE_CLASS,        // Interface class
    MBIM_DC_INTERFACE_SUBCLASS,     // Interface sub class
    MBIM_DC_INTERFACE_PROTOCOL,     // Interface protocol code
    0x00,                           // Interface descriptor string index
    
    // CDC Header Functional Descriptor
    // USB Class Definitions for Communications Devices Rev 1.2 Sec 5.2.3.1
    0x05,                          // Descriptor size 
    USB_CDC_CS_DESCRIPTOR_TYPE,    // Descriptor type
    USB_CDC_CS_DESCRIPTOR_SUBTYPE, // Functional descriptor subtype
    0x02,                          // LSB of Class definition version
    0x01,                          // MSB of Class definition version

    // Bulk IN endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_BulkInEndpointAddress,        // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x04,                     // Max packet size = 1024 bytes
    0x00,                         // Servicing interval for data transfers : NA for bulk

    // Super Speed Endpoint Companion Descriptor for Bulk IN EP
    0x06,                                              // Descriptor size
    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE, // SS Endpoint Companion Descriptor Type
    0x0F,                                              // Max no. of packets in a Burst : 0: Burst 1 packet at a time
    0x00,                                              // Max streams for Bulk EP = 0 (No streams)
    0x00, 0x00,                                        // Service interval for the EP : NA for Bulk

    // Bulk OUT endpoint
    0x07,                           // Descriptor size 
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // Descriptor type
    g_BulkOutEndpointAddress,       // Endpoint address and description
    USB_ENDPOINT_TYPE_BULK,         // Endpoint type
    0x00, 0x04,                     // Max packet size = 1024 bytes
    0x00,                           // Servicing interval for data transfers : NA for bulk

    // Super Speed Endpoint Companion Descriptor for Bulk OUT EP
    0x06,                                              // Descriptor size
    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE, // SS Endpoint Companion Descriptor Type
    0x0F,                                              // Max no. of packets in a Burst : 0: Burst 1 packet at a time
    0x00,                                              // Max streams for Bulk EP = 0 (No streams)
    0x00, 0x00,                                        // Service interval for the EP : NA for Bulk

};

//Not correct, need check where is wrong
const UCHAR g_UsbBOSDescriptor[] = 
{
    0x05,                    // Descriptor size
    USB_BOS_DESCRIPTOR_TYPE, // Descriptor type
    0x16, 0x00,              // Length of descriptor and all sub descriptors
    0x02,                    // Number of device capability descriptors

    // USB 2.0 extension
    0x07,                                  // Descriptor size
    USB_DEVICE_CAPABILITY_DESCRIPTOR_TYPE, // Device capability type descriptor
    USB_DEVICE_CAPABILITY_USB20_EXTENSION, // USB 2.0 extension capability type
    0x02, 0x00, 0x00, 0x00,                // Supported device level features

    // SuperSpeed device capability
    0x0A,                                  // Descriptor size
    USB_DEVICE_CAPABILITY_DESCRIPTOR_TYPE, // Device capability type descriptor
    USB_DEVICE_CAPABILITY_SUPERSPEED_USB,  // SuperSpeed device capability type
    0x00,                                  // Supported device level features
    0x0E, 0x00,                            // Speeds supported by the device : SS, HS and FS
    0x03,                                  // Functionality support
    0x00,                                  // U1 Device Exit latency
    0x00, 0x00,                            // U2 Device Exit latency

    //Below container ID descriptor is recommended for hub, and optional for other device
    //It cannot be used in MBIM device, with it USBView think the device descriptor is wrong
    //// Microsoft OS 2.0 Platform Capability Descriptor Header
    //0x1C,                                  // Descriptor Size
    //USB_DEVICE_CAPABILITY_DESCRIPTOR_TYPE, // Descriptor Type 
    //USB_DEVICE_CAPABILITY_PLATFORM,        // Device capability type
    //0x00,                                  // Reserved
    //0xDF, 0x60, 0xDD, 0xD8,                // MS_OS_20_Platform_Capability_ID
    //0x89, 0x45, 0xC7, 0x4C,                // {D8DD60DF-4589-4CC7-9CD2-659D9E648A9F}
    //0x9C, 0xD2, 0x65, 0x9D,                //
    //0x9E, 0x64, 0x8A, 0x9F,                //

    //// Descriptor Set for Windows Blue or higher
    //0x00, 0x00, 0x03, 0x06,   // Windows version - 0c06030000 for Windows Blue
    //0x1E, 0x00,               // Length - size of MS OS 2.0 descriptor set: 33 bytes
    //g_UDE_VendorCode,       // Vendor Code
    //0x00                      // bFlags
};
#endif

#endif