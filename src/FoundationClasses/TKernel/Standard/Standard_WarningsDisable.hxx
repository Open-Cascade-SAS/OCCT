// Copyright (c) 2018 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//!@file
//! Suppresses compiler warnings.
//!
//! Standard_WarningsDisable.hxx disables all compiler warnings.
//! Standard_WarningsRestore.hxx restore the previous state of warnings.
//!
//! Use these headers to wrap include directive containing external (non-OCCT)
//! header files to avoid compiler warnings to be generated for these files.
//! They should always be used in pair:
//!
//! @code
//! #include <Standard_WarningsDisable.hxx>
//! #include <dirty_header.h> // some header that can generate warnings
//! #include <Standard_WarningsRestore.hxx>
//! @endcode

#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wall"
  #pragma clang diagnostic ignored "-Wextra"
  #pragma clang diagnostic ignored "-Wshorten-64-to-32"
  #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
  #pragma warning(push, 0)
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
  // -Wall does not work here for GCC, so the only way is to list all most important warnings...
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  #pragma GCC diagnostic ignored "-Wunused-variable"
  #pragma GCC diagnostic ignored "-Wunused-parameter"
  #pragma GCC diagnostic ignored "-Wenum-compare"
  #pragma GCC diagnostic ignored "-Wreorder"
  #if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
    #pragma GCC diagnostic ignored "-Wunused-local-typedefs"
  #endif
#endif
