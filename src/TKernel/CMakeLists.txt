project(TKernel)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Set desired Memory Manager
if     ("${USE_MMGR_TYPE}" STREQUAL "FLEXIBLE")
  target_compile_definitions("TKernel" PRIVATE "OCCT_MMGT_OPT_FLEXIBLE")
elseif ("${USE_MMGR_TYPE}" STREQUAL "TBB")
  target_compile_definitions("TKernel" PRIVATE "OCCT_MMGT_OPT_TBB")
elseif ("${USE_MMGR_TYPE}" STREQUAL "JEMALLOC")
  target_compile_definitions("TKernel" PRIVATE "OCCT_MMGT_OPT_JEMALLOC")
else ()
  message (STATUS "Info: Used native memory manager")
endif()
