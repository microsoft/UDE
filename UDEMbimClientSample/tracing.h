/*++

Copyright (c) Microsoft Corporation

Module Name:

    tracing.hpp

Abstract:

    Debug stuff

--*/

#pragma once

//
// Defining control guids, including this is required to happen before
// including the tmh file (if the WppRecorder API is used)
//
#include <WppRecorder.h>

// {f3006b12-1d83-48d2-948d-6bcd002c14dc}
#define WPP_CONTROL_GUIDS \
            WPP_DEFINE_CONTROL_GUID(UDECXMBIM, (f3006b12,1d83,48d2,948d,6bcd002c14dc), \
                WPP_DEFINE_BIT(TRACE_FLAG_Driver)      \
            )

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) WPP_LEVEL_LOGGER(flags)
#define WPP_LEVEL_FLAGS_ENABLED(lvl,flags) (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)

#define TL_FATAL        0x01
#define TL_ERROR        0x02
#define TL_WARNING      0x03
#define TL_INFO         0x04
#define TL_VERBOSE      0x05

// begin_wpp config
// LogFatal{LEVEL=TL_FATAL}(FLAGS,MSG,...);
// LogError{LEVEL=TL_ERROR}(FLAGS,MSG,...);
// LogWarning{LEVEL=TL_WARNING}(FLAGS,MSG,...);
// LogInfo{LEVEL=TL_INFO}(FLAGS,MSG,...);
// LogVerbose{LEVEL=TL_INFO}(FLAGS,MSG,...);
// end_wpp

//BUG::TL_VERBOSE trace cannot be recored even if the specified level is 0xFF, so use TL_INFO instead at first.
// begin_wpp config
// USEPREFIX(FuncEntry, "%!STDPREFIX! [%!FUNC!] --> entry");
// FUNC FuncEntry{LEVEL=TL_INFO}(FLAGS);
// USEPREFIX(FuncExit, "%!STDPREFIX! [%!FUNC!] <-- exit=0x%x",EXP);
// FUNC FuncExit{LEVEL=TL_INFO}(FLAGS,EXP);
// end_wpp

#define WPP_LEVEL_FLAGS_EXP_LOGGER(lvl,flags, EXP) WPP_LEVEL_LOGGER(flags)
#define WPP_LEVEL_FLAGS_EXP_ENABLED(lvl,flags, EXP) (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)
//For WPP Inflight Trace Recorder
#define WPP_RECORDER_LEVEL_FLAGS_EXP_FILTER(lvl, FLAGS, EXP)   WPP_RECORDER_LEVEL_FLAGS_FILTER(lvl, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_EXP_ARGS(lvl, FLAGS, EXP)     WPP_RECORDER_LEVEL_FLAGS_ARGS(lvl, FLAGS)

// begin_wpp config
// CUSTOM_TYPE(CommandType, ItemEnum(MBB_COMMAND_TYPE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(ParentBusType, ItemEnum(_UCX_CONTROLLER_PARENT_BUS_TYPE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(WdfPowerDeviceState, ItemEnum(_WDF_POWER_DEVICE_STATE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(WdfIoQueueState, ItemEnum(_WDF_IO_QUEUE_STATE));
// end_wpp

