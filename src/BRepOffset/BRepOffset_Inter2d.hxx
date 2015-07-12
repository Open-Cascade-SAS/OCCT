// Created on: 1996-08-30
// Created by: Yves FRICAUD
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

#ifndef _BRepOffset_Inter2d_HeaderFile
#define _BRepOffset_Inter2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <Standard_Real.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
class BRepAlgo_AsDes;
class TopoDS_Face;
class BRepOffset_Offset;


//! Computes the intersections betwwen edges on a face
//! stores result is SD as AsDes from BRepOffset.
class BRepOffset_Inter2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Computes the intersections between the edges stored
  //! is AsDes as descendants of <F> . Intersections is computed
  //! between two edges if one of them is bound in NewEdges.
  Standard_EXPORT static void Compute (const Handle(BRepAlgo_AsDes)& AsDes, const TopoDS_Face& F, const TopTools_IndexedMapOfShape& NewEdges, const Standard_Real Tol);
  
  Standard_EXPORT static void ConnexIntByInt (const TopoDS_Face& FI, BRepOffset_Offset& OFI, TopTools_DataMapOfShapeShape& MES, const TopTools_DataMapOfShapeShape& Build, const Handle(BRepAlgo_AsDes)& AsDes, const Standard_Real Offset, const Standard_Real Tol);




protected:





private:





};







#endif // _BRepOffset_Inter2d_HeaderFile
