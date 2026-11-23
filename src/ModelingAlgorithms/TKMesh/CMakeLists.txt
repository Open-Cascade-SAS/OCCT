project(TKMesh)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Preserve symmetric rounding in Delabella's determinant tests.
if (CMAKE_CXX_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU)$" AND NOT MSVC)
  include (CheckCXXCompilerFlag)
  check_cxx_compiler_flag ("-ffp-contract=off" OCCT_COMPILER_SUPPORTS_FFP_CONTRACT_OFF)
  if (OCCT_COMPILER_SUPPORTS_FFP_CONTRACT_OFF)
    set_property (SOURCE BRepMesh/delabella.cpp APPEND PROPERTY COMPILE_OPTIONS "-ffp-contract=off")
  endif()
endif()

# Add the precompiled header
ADD_PRECOMPILED_HEADER(TKMesh "TKMesh_pch.hxx" TRUE)
