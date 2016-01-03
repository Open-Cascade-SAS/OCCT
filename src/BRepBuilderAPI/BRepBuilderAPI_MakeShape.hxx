// Created on: 1993-07-21
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepBuilderAPI_MakeShape_HeaderFile
#define _BRepBuilderAPI_MakeShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepBuilderAPI_Command.hxx>
#include <Standard_Boolean.hxx>
class StdFail_NotDone;
class TopoDS_Shape;


//! This    is  the  root     class for     all  shape
//! constructions.  It stores the result.
//!
//! It  provides deferred methods to trace the history
//! of sub-shapes.
class BRepBuilderAPI_MakeShape  : public BRepBuilderAPI_Command
{
public:

  DEFINE_STANDARD_ALLOC

  //! This is  called by  Shape().  It does  nothing but
  //! may be redefined.
  Standard_EXPORT virtual void Build();
  
  //! Returns a shape built by the shape construction algorithm.
  //! Raises exception StdFail_NotDone if the shape was not built.
  Standard_EXPORT virtual const TopoDS_Shape& Shape();
  Standard_EXPORT operator TopoDS_Shape();
  
  //! Returns the  list   of shapes generated   from the
  //! shape <S>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& S);
  
  //! Returns the list  of shapes modified from the shape
  //! <S>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified (const TopoDS_Shape& S);
  
  //! Returns true if the shape S has been deleted.
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& S);




protected:

  
  Standard_EXPORT BRepBuilderAPI_MakeShape();


  TopoDS_Shape myShape;
  TopTools_ListOfShape myGenerated;


private:





};







#endif // _BRepBuilderAPI_MakeShape_HeaderFile
