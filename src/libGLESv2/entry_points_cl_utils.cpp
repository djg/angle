//
// Copyright 2021 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// entry_points_cl_utils.cpp: These helpers are used in CL entry point routines.

#include "libGLESv2/entry_points_cl_utils.h"

#include "libGLESv2/cl_dispatch_table.h"

#include "libANGLE/CLPlatform.h"

#ifdef ANGLE_ENABLE_CL_PASSTHROUGH
#    include "libANGLE/renderer/cl/CLPlatformCL.h"
#endif
#ifdef ANGLE_ENABLE_VULKAN
#    include "libANGLE/renderer/vulkan/CLPlatformVk.h"
#endif

namespace cl
{

void InitBackEnds(bool isIcd)
{
    static bool initialized = false;
    if (initialized)
    {
        return;
    }
    initialized = true;

#ifdef ANGLE_ENABLE_CL_PASSTHROUGH
    rx::CLPlatformImpl::InitList initListCL = rx::CLPlatformCL::GetPlatforms(isIcd);
    while (!initListCL.empty())
    {
        Platform::CreatePlatform(gCLIcdDispatchTable, initListCL.front());
        initListCL.pop_front();
    }
#endif

#ifdef ANGLE_ENABLE_VULKAN
    rx::CLPlatformImpl::InitList initListVk = rx::CLPlatformVk::GetPlatforms();
    while (!initListVk.empty())
    {
        Platform::CreatePlatform(gCLIcdDispatchTable, initListVk.front());
        initListVk.pop_front();
    }
#endif
}

}  // namespace cl
