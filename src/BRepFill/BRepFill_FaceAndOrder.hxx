// Created on: 1998-09-03
// Created by: Julia GERASIMOVA
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepFill_FaceAndOrder_HeaderFile
#define _BRepFill_FaceAndOrder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
#include <GeomAbs_Shape.hxx>
class BRepFill_Filling;
class TopoDS_Face;


//! A structure containing Face and Order of constraint
class BRepFill_FaceAndOrder 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepFill_FaceAndOrder();
  
  Standard_EXPORT BRepFill_FaceAndOrder(const TopoDS_Face& aFace, const GeomAbs_Shape anOrder);


friend class BRepFill_Filling;


protected:





private:



  TopoDS_Face myFace;
  GeomAbs_Shape myOrder;


};







#endif // _BRepFill_FaceAndOrder_HeaderFile
