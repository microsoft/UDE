/*++
Copyright (c) Microsoft Corporation

Module Name:

    pch.h

Abstract:


--*/

#ifndef __PCH_H__
#define __PCH_H__

//
// Device is using simple endpoints
//

//
// Device described as 3.0 device
//
//#define USB30
//#define USB20_MBIM
#define USB30_MBIM
//#define USB20_MOUSE
//#define USB30_MOUSE
//#define USB20_COMPOSITE

#ifdef USB20_MBIM
#define USB_MBIM
#define USB20_COMPOSITE
#endif

#ifdef USB30_MBIM
#define USB30
#define USB_MBIM
#define USB30_COMPOSITE
#endif

#ifdef USB20_MOUSE
#define USB_MOUSE
#endif

#ifdef USB30_MOUSE
#define USB30
#define USB_MOUSE
#endif


#ifdef __cplusplus
extern "C" {
#endif

#include <initguid.h>

#include <ntddk.h>
#include <wdf.h>

#include <ntstrsafe.h>
#include <ntintsafe.h>
#include <driverspecs.h>
#include <limits.h>

#include <wdfcx.h>
//#include <devpkey.h>
#include <acpiioct.h>
#include <wdmguid.h>
#include <WppRecorder.h>

#include <usb.h>

#pragma warning(push)
#include <wdfusb.h>
#pragma warning(pop)
#include "tracing.h"
#include "ucxclass.h"

#include <windef.h>
#include <mbbncm.h>
#include <mbbmessages.h>
#ifdef __cplusplus
} // extern "C"
#endif

//#ifdef USB_MBIM
const UCHAR g_UDE_VendorCode = 0x19;

const UCHAR g_InterruptEndpointAddress = 0x88;
const UCHAR g_MouseInterruptEndpointAddress = 0x81;

const UCHAR g_BulkInEndpointAddress = 0x89;
const UCHAR g_BulkOutEndpointAddress = 0x09;
//Confirm::Interface number seems must start from 0
const UCHAR g_CommunicationInterfaceIndex = 0;
const UCHAR g_DataInterfaceIndex = 1;

//Careful::Interface number must be continious within a IAD, IAD is optional, 
//Without IAD, all interfaces with continious numbers will be recognized as a single function
const UCHAR g_MSInterfaceIndex0 = 10;
const UCHAR g_MSInterfaceIndex1 = 11;
const UCHAR g_MSInterfaceIndex2 = 12;
const UCHAR g_MSInterfaceIndex3 = 13;
const UCHAR g_MSEndpointAddress0_BulkIn = 0x81;
const UCHAR g_MSEndpointAddress0_BulkOut = 0x1;
const UCHAR g_MSEndpointAddress1_Interrupt = 0x83;
const UCHAR g_MSEndpointAddress1_BulkIn = 0x82;
const UCHAR g_MSEndpointAddress1_BulkOut = 0x2;
const UCHAR g_MSEndpointAddress2_Interrupt = 0x85;
const UCHAR g_MSEndpointAddress2_BulkIn = 0x84;
const UCHAR g_MSEndpointAddress2_BulkOut = 0x4;
const UCHAR g_MSEndpointAddress3_Interrupt = 0x87;
const UCHAR g_MSEndpointAddress3_BulkIn = 0x86;
const UCHAR g_MSEndpointAddress3_BulkOut = 0x6;

const UCHAR g_LanguageDescriptor[] = { 4,3,9,4 };
const USHORT AMERICAN_ENGLISH = 0x409;

const UCHAR g_ManufacturerIndex = 1;
DECLARE_CONST_UNICODE_STRING(g_ManufacturerStringEnUs, L"Microsoft");

const UCHAR g_ProductIndex = 2;
DECLARE_CONST_UNICODE_STRING(g_ProductStringEnUs, L"UDE Client");
//#endif

#include "udecx.h"

#include "usbdevice.h"
#include "udembimdescriptors.h"
#include "controller.h"
#include "driver.h"
#include "io.h"
#include "misc.h"
#include "WwanVirtualAdapter.h"


#endif