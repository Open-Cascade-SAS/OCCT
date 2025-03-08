// Created on: 1997-02-21
// Created by: Alexander BRIVIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _VrmlConverter_ShadedShape_HeaderFile
#define _VrmlConverter_ShadedShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_OStream.hxx>
#include <TColgp_Array1OfDir.hxx>
class TopoDS_Shape;
class VrmlConverter_Drawer;
class TopoDS_Face;
class Poly_Connect;

//! ShadedShape - computes  the  shading presentation of shapes
//! by triangulation algorithms, converts this one into VRML objects
//! and writes (adds) into anOStream.
//! All requested properties of the representation including
//! the maximal chordial deviation  are specify in aDrawer.
//! This  kind  of  the  presentation  is  converted  into
//! IndexedFaceSet ( VRML ).
class VrmlConverter_ShadedShape
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void Add(Standard_OStream&                   anOStream,
                                  const TopoDS_Shape&                 aShape,
                                  const Handle(VrmlConverter_Drawer)& aDrawer);

  Standard_EXPORT static void ComputeNormal(const TopoDS_Face&  aFace,
                                            Poly_Connect&       pc,
                                            TColgp_Array1OfDir& Nor);

protected:
private:
};

#endif // _VrmlConverter_ShadedShape_HeaderFile
