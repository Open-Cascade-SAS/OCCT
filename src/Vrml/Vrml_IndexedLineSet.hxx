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

#ifndef _Vrml_IndexedLineSet_HeaderFile
#define _Vrml_IndexedLineSet_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>


class Vrml_IndexedLineSet;
DEFINE_STANDARD_HANDLE(Vrml_IndexedLineSet, Standard_Transient)

//! defines a IndexedLineSet node of VRML specifying geometry shapes.
//! This node represents a 3D shape formed by constructing polylines from vertices
//! located at the current coordinates. IndexedLineSet uses the indices in its coordIndex
//! field to specify the polylines. An index of -1 separates one polyline from the next
//! (thus, a final -1 is optional). the current polyline has ended and the next one begins.
//! Treatment of the current material and normal binding is as follows: The PER_PART binding
//! specifies a material or normal for each segment of the line. The PER_FACE binding
//! specifies a material or normal for each polyline. PER_VERTEX specifies a material or
//! normal for each vertex. The corresponding _INDEXED bindings are the same, but use
//! the materialIndex or normalIndex indices. The DEFAULT material binding is equal
//! to OVERALL. The DEFAULT normal binding is equal to  PER_VERTEX_INDEXED;
//! if insufficient normals exist in the state, the lines will be drawn unlit. The same
//! rules for texture coordinate generation as IndexedFaceSet are used.
class Vrml_IndexedLineSet : public Standard_Transient
{

public:

  
  Standard_EXPORT Vrml_IndexedLineSet(const Handle(TColStd_HArray1OfInteger)& aCoordIndex, const Handle(TColStd_HArray1OfInteger)& aMaterialIndex, const Handle(TColStd_HArray1OfInteger)& aNormalIndex, const Handle(TColStd_HArray1OfInteger)& aTextureCoordIndex);
  
  Standard_EXPORT Vrml_IndexedLineSet();
  
  Standard_EXPORT void SetCoordIndex (const Handle(TColStd_HArray1OfInteger)& aCoordIndex);
  
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) CoordIndex() const;
  
  Standard_EXPORT void SetMaterialIndex (const Handle(TColStd_HArray1OfInteger)& aMaterialIndex);
  
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) MaterialIndex() const;
  
  Standard_EXPORT void SetNormalIndex (const Handle(TColStd_HArray1OfInteger)& aNormalIndex);
  
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) NormalIndex() const;
  
  Standard_EXPORT void SetTextureCoordIndex (const Handle(TColStd_HArray1OfInteger)& aTextureCoordIndex);
  
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) TextureCoordIndex() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




  DEFINE_STANDARD_RTTIEXT(Vrml_IndexedLineSet,Standard_Transient)

protected:




private:


  Handle(TColStd_HArray1OfInteger) myCoordIndex;
  Handle(TColStd_HArray1OfInteger) myMaterialIndex;
  Handle(TColStd_HArray1OfInteger) myNormalIndex;
  Handle(TColStd_HArray1OfInteger) myTextureCoordIndex;


};







#endif // _Vrml_IndexedLineSet_HeaderFile
