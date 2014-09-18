#OpenCl

SET (3RDPARTY_OPENCL_ADDITIONAL_PATH_FOR_HEADER $ENV{AMDAPPSDKROOT}/include
                                                  $ENV{INTELOCLSDKROOT}/include
                                                  $ENV{NVSDKCOMPUTE_ROOT}/OpenCL/common/inc
                                                  $ENV{ATISTREAMSDKROOT}/include)


IF(${COMPILER_BITNESS} STREQUAL 32)
  SET (3RDPARTY_OPENCL_ADDITIONAL_PATH_FOR_LIB $ENV{AMDAPPSDKROOT}/lib/x86
                                               $ENV{INTELOCLSDKROOT}/lib/x86
                                               $ENV{NVSDKCOMPUTE_ROOT}/OpenCL/common/lib/Win32
                                               $ENV{ATISTREAMSDKROOT}/lib/x86)
ELSEIF(${COMPILER_BITNESS} STREQUAL 64)
  SET (3RDPARTY_OPENCL_ADDITIONAL_PATH_FOR_LIB $ENV{AMDAPPSDKROOT}/lib/x86_64
                                               $ENV{INTELOCLSDKROOT}/lib/x64
                                               $ENV{NVSDKCOMPUTE_ROOT}/OpenCL/common/lib/x64
                                               $ENV{ATISTREAMSDKROOT}/lib/x86_64)
ENDIF()

THIRDPARTY_PRODUCT("OPENCL" "CL/cl.h" "OpenCL" "OpenCLd")

# if CL/cl.h isn't found (and 3RDPARTY_OPENCL_INCLUDE_DIR isn't defined)
# then try to find OpenCL/cl.h (all other variable won't be changed)
IF(NOT 3RDPARTY_OPENCL_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_OPENCL_INCLUDE_DIR}")
  THIRDPARTY_PRODUCT("OPENCL" "OpenCL/cl.h" "OpenCL" "OpenCLd")
ENDIF()