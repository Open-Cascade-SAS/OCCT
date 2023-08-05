// Copyright (c) 2015-2021 OPEN CASCADE SAS
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

#ifndef _RWObj_ObjMaterialMap_HeaderFiler
#define _RWObj_ObjMaterialMap_HeaderFiler

#include <NCollection_DataMap.hxx>
#include <RWMesh_MaterialMap.hxx>

//! Material MTL file writer for OBJ export.
class RWObj_ObjMaterialMap : public RWMesh_MaterialMap
{
  DEFINE_STANDARD_RTTIEXT(RWObj_ObjMaterialMap, RWMesh_MaterialMap)
public:

  //! Main constructor.
  Standard_EXPORT RWObj_ObjMaterialMap (const TCollection_AsciiString& theFile);

  //! Destructor, will emit error message if file was not closed.
  Standard_EXPORT virtual ~RWObj_ObjMaterialMap();

  //! Add material
  Standard_EXPORT virtual TCollection_AsciiString AddMaterial (const XCAFPrs_Style& theStyle) Standard_OVERRIDE;

  //! Virtual method actually defining the material (e.g. export to the file).
  Standard_EXPORT virtual void DefineMaterial (const XCAFPrs_Style& theStyle,
                                               const TCollection_AsciiString& theKey,
                                               const TCollection_AsciiString& theName) Standard_OVERRIDE;

private:

  FILE* myFile;
  NCollection_DataMap<Handle(Image_Texture), TCollection_AsciiString> myImageMap;

};

#endif // _RWObj_ObjMaterialMap_HeaderFiler
