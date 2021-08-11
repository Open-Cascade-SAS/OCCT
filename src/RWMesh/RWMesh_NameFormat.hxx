// Copyright: Open CASCADE 2021
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

#ifndef _RWMesh_NameFormat_HeaderFile
#define _RWMesh_NameFormat_HeaderFile

//! Name format preference for XCAF shape labels.
enum RWMesh_NameFormat
{
  RWMesh_NameFormat_Empty,                     //!< omit the name
  RWMesh_NameFormat_Product,                   //!< return Product name
                                               //!  (e.g. from XCAFDoc_ShapeTool::GetReferredShape(), which could be shared by multiple Instances)
  RWMesh_NameFormat_Instance,                  //!< return Instance name
  RWMesh_NameFormat_InstanceOrProduct,         //!< return Instance name when available and Product name otherwise
  RWMesh_NameFormat_ProductOrInstance,         //!< return Product name when available and Instance name otherwise
  RWMesh_NameFormat_ProductAndInstance,        //!< generate "Product [Instance]" name
  RWMesh_NameFormat_ProductAndInstanceAndOcaf, //!< generate name combining Product+Instance+Ocaf (useful for debugging purposes)
};

#endif // _RWMesh_NameFormat_HeaderFile
