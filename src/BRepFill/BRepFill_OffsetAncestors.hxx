// Created on: 1995-09-01
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepFill_OffsetAncestors_HeaderFile
#define _BRepFill_OffsetAncestors_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
class StdFail_NotDone;
class BRepFill_OffsetWire;
class TopoDS_Edge;
class TopoDS_Shape;


//! this class is used to find the generating shapes
//! of an OffsetWire.
class BRepFill_OffsetAncestors 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepFill_OffsetAncestors();
  
  Standard_EXPORT BRepFill_OffsetAncestors(BRepFill_OffsetWire& Paral);
  
  Standard_EXPORT void Perform (BRepFill_OffsetWire& Paral);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Boolean HasAncestor (const TopoDS_Edge& S1) const;
  
  //! may return a Null Shape if S1 is not a subShape
  //! of <Paral>;
  //! if Perform is not done.
  Standard_EXPORT const TopoDS_Shape& Ancestor (const TopoDS_Edge& S1) const;




protected:





private:



  Standard_Boolean myIsPerform;
  TopTools_DataMapOfShapeShape myMap;


};







#endif // _BRepFill_OffsetAncestors_HeaderFile
