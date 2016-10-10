# UDE Mbim Client Sample

This sample demonstrates how to write emulated composite USB device with Mbim function by [UDE] (https://msdn.microsoft.com/en-us/library/windows/hardware/mt595939%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396). With this UDE client driver, PnP will load wmbclass and wmbclass can complete most of initialization steps and get response of MBIM_BASIC_CID_DEVICE_CAPS. 

Code Tour
---------

This section includes a file manifest of the files in the \\UDEMbimClientSample directory.

File | Description 
-----|------------
Driver.cpp | Contains code for driver entry and cleanup
controller.cpp | Contains callback functions associated with the host controller object
io.cpp | Contains code for IO dispatch
misc.cpp | Contains code for utility functions
usbdevice.cpp | Contains code for virtual USB device
WwanVirtualAdapter.cpp | Contains code for Mbim implementation
Trace.h | Contains trace-related definitions
udembimdescriptors.h | Contains descriptors for composite USB device

### License
UDE Mbim Client Sample is licensed under the [MIT License](https://github.com/Microsoft/UDE/blob/master/UDEMbimClientSample/License.txt).
