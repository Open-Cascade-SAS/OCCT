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

#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>


class Vrml_IndexedFaceSet;
DEFINE_STANDARD_HANDLE(Vrml_IndexedFaceSet, Standard_Transient)

//! defines a IndexedFaceSet node of VRML specifying geometry shapes.
//! This node represents a 3D shape formed by constructing faces (polygons) from
//! vertices located at the current coordinates. IndexedFaceSet uses the indices
//! in its coordIndex to define polygonal faces. An index of -1 separates faces
//! (so a -1 at the end of the list is optional).
class Vrml_IndexedFaceSet : public Standard_Transient
{

public:

  
  Standard_EXPORT Vrml_IndexedFaceSet(const Handle(TColStd_HArray1OfInteger)& aCoordIndex, const Handle(TColStd_HArray1OfInteger)& aMaterialIndex, const Handle(TColStd_HArray1OfInteger)& aNormalIndex, const Handle(TColStd_HArray1OfInteger)& aTextureCoordIndex);
  
  Standard_EXPORT Vrml_IndexedFaceSet();
  
  Standard_EXPORT void SetCoordIndex (const Handle(TColStd_HArray1OfInteger)& aCoordIndex);
  
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) CoordIndex() const;
  
  Standard_EXPORT void SetMaterialIndex (const Handle(TColStd_HArray1OfInteger)& aMaterialIndex);
  
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) MaterialIndex() const;
  
  Standard_EXPORT void SetNormalIndex (const Handle(TColStd_HArray1OfInteger)& aNormalIndex);
  
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) NormalIndex() const;
  
  Standard_EXPORT void SetTextureCoordIndex (const Handle(TColStd_HArray1OfInteger)& aTextureCoordIndex);
  
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) TextureCoordIndex() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




  DEFINE_STANDARD_RTTIEXT(Vrml_IndexedFaceSet,Standard_Transient)

protected:




private:


  Handle(TColStd_HArray1OfInteger) myCoordIndex;
  Handle(TColStd_HArray1OfInteger) myMaterialIndex;
  Handle(TColStd_HArray1OfInteger) myNormalIndex;
  Handle(TColStd_HArray1OfInteger) myTextureCoordIndex;


};







#endif // _Vrml_IndexedFaceSet_HeaderFile
