# A toolchain file to configure a Makefile Generators or the Ninja generator to target Android for cross-compiling.
# Set CMAKE_ANDROID_NDK variable equal to your Android NDK path.

set (CMAKE_SYSTEM_NAME       Android)
set (CMAKE_SYSTEM_VERSION    15) # API level
set (CMAKE_ANDROID_ARCH_ABI  armeabi-v7a)
set (CMAKE_ANDROID_NDK       "")
set (CMAKE_ANDROID_STL_TYPE  gnustl_shared)
