// Author: Kirill Gavrilov
// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#ifndef _RWMesh_NodeAttributes_HeaderFile
#define _RWMesh_NodeAttributes_HeaderFile

#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <XCAFPrs_Style.hxx>

class TDataStd_NamedData;

//! Attributes of the node.
struct RWMesh_NodeAttributes
{
  TCollection_AsciiString    Name;      //!< name for the user
  TCollection_AsciiString    RawName;   //!< name within low-level format structure
  Handle(TDataStd_NamedData) NamedData; //!< optional metadata
  XCAFPrs_Style              Style;     //!< presentation style
};
typedef NCollection_DataMap<TopoDS_Shape, RWMesh_NodeAttributes, TopTools_ShapeMapHasher> RWMesh_NodeAttributeMap;

#endif // _RWMesh_NodeAttributes_HeaderFile
