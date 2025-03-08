// Author: Kirill Gavrilov
// Copyright (c) 2015-2019 OPEN CASCADE SAS
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

#ifndef _RWObj_SubMesh_HeaderFile
#define _RWObj_SubMesh_HeaderFile

#include <TCollection_AsciiString.hxx>

//! Sub-mesh definition for OBJ reader.
struct RWObj_SubMesh
{
  TCollection_AsciiString Object;      //!< name of active object
  TCollection_AsciiString Group;       //!< name of active group
  TCollection_AsciiString SmoothGroup; //!< name of active smoothing group
  TCollection_AsciiString Material;    //!< name of active material
};

#endif // _RWObj_SubMesh_HeaderFile
