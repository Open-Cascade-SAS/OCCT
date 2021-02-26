// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _Poly_MeshPurpose_HeaderFile
#define _Poly_MeshPurpose_HeaderFile

//! Purpose of triangulation using.
typedef unsigned int Poly_MeshPurpose;
enum
{
  // main flags
  Poly_MeshPurpose_NONE               = 0,      //!< no special use (default)
  Poly_MeshPurpose_Calculation        = 0x0001, //!< mesh for algorithms
  Poly_MeshPurpose_Presentation       = 0x0002, //!< mesh for presentation (LODs usage)
  // special purpose bits (should not be set externally)
  Poly_MeshPurpose_Active             = 0x0004, //!< mesh marked as currently active in a list
  Poly_MeshPurpose_Loaded             = 0x0008, //!< mesh has currently loaded data
  Poly_MeshPurpose_AnyFallback        = 0x0010, //!< a special flag for BRep_Tools::Triangulation() to return any other defined mesh,
                                                //   if none matching other criteria was found user-defined flags should have higher values
  Poly_MeshPurpose_USER               = 0x0020  //!< application-defined flags
};

#endif // _Poly_MeshPurpose_HeaderFile
