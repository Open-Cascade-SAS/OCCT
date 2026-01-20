// Created on: 1996-12-26
// Created by: Alexander BRIVIN and Dmitry TARASOV
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Vrml_IndexedFaceSet_HeaderFile
#define _Vrml_IndexedFaceSet_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>

//! defines a IndexedFaceSet node of VRML specifying geometry shapes.
//! This node represents a 3D shape formed by constructing faces (polygons) from
//! vertices located at the current coordinates. IndexedFaceSet uses the indices
//! in its coordIndex to define polygonal faces. An index of -1 separates faces
//! (so a -1 at the end of the list is optional).
class Vrml_IndexedFaceSet : public Standard_Transient
{

public:
  Standard_EXPORT Vrml_IndexedFaceSet(const occ::handle<NCollection_HArray1<int>>& aCoordIndex,
                                      const occ::handle<NCollection_HArray1<int>>& aMaterialIndex,
                                      const occ::handle<NCollection_HArray1<int>>& aNormalIndex,
                                      const occ::handle<NCollection_HArray1<int>>& aTextureCoordIndex);

  Standard_EXPORT Vrml_IndexedFaceSet();

  Standard_EXPORT void SetCoordIndex(const occ::handle<NCollection_HArray1<int>>& aCoordIndex);

  Standard_EXPORT occ::handle<NCollection_HArray1<int>> CoordIndex() const;

  Standard_EXPORT void SetMaterialIndex(const occ::handle<NCollection_HArray1<int>>& aMaterialIndex);

  Standard_EXPORT occ::handle<NCollection_HArray1<int>> MaterialIndex() const;

  Standard_EXPORT void SetNormalIndex(const occ::handle<NCollection_HArray1<int>>& aNormalIndex);

  Standard_EXPORT occ::handle<NCollection_HArray1<int>> NormalIndex() const;

  Standard_EXPORT void SetTextureCoordIndex(
    const occ::handle<NCollection_HArray1<int>>& aTextureCoordIndex);

  Standard_EXPORT occ::handle<NCollection_HArray1<int>> TextureCoordIndex() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream) const;

  DEFINE_STANDARD_RTTIEXT(Vrml_IndexedFaceSet, Standard_Transient)

private:
  occ::handle<NCollection_HArray1<int>> myCoordIndex;
  occ::handle<NCollection_HArray1<int>> myMaterialIndex;
  occ::handle<NCollection_HArray1<int>> myNormalIndex;
  occ::handle<NCollection_HArray1<int>> myTextureCoordIndex;
};

#endif // _Vrml_IndexedFaceSet_HeaderFile
