/*++
Copyright (c) Microsoft Corporation

Module Name:

    driver.h

Abstract:


--*/

#pragma once

typedef struct _WDFDRIVER_UDECXMBIM_CONTEXT {
    LIST_ENTRY  ControllerListHead;
    KSPIN_LOCK  ControllerListLock;
    ULONG       ControllerListCount;
} WDFDRIVER_UDECXMBIM_CONTEXT, *PWDFDRIVER_UDECXMBIM_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WDFDRIVER_UDECXMBIM_CONTEXT, WdfDriverGetUdecxMbimContext);

extern PWDFDRIVER_UDECXMBIM_CONTEXT g_WdfDriverUdecxMbimContext;
extern PDRIVER_OBJECT               g_UdecxMbimDriverObject;

extern "C" {
DRIVER_INITIALIZE                   DriverEntry;
}
EVT_WDF_OBJECT_CONTEXT_CLEANUP      DriverCleanup;

#define UDECXMBIM_POOL_TAG 'EDUM'


#define ALLOCATE_PAGED_POOL(_y)    ExAllocatePoolWithTag(PagedPool,_y, UDECXMBIM_POOL_TAG)

#define ALLOCATE_NONPAGED_POOL(_y) ExAllocatePoolWithTag(NonPagedPoolNx,_y, UDECXMBIM_POOL_TAG)

#define FREE_POOL(_x) {ExFreePool(_x);_x=NULL;};
