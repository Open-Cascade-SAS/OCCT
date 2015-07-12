// Created on: 1999-07-22
// Created by: data exchange team
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeUpgrade_ShapeDivideClosed_HeaderFile
#define _ShapeUpgrade_ShapeDivideClosed_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <ShapeUpgrade_ShapeDivide.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Shape;


//! Divides all closed faces in the shape. Class
//! ShapeUpgrade_ClosedFaceDivide is used as divide tool.
class ShapeUpgrade_ShapeDivideClosed  : public ShapeUpgrade_ShapeDivide
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initialises tool with shape and default parameter.
  Standard_EXPORT ShapeUpgrade_ShapeDivideClosed(const TopoDS_Shape& S);
  
  //! Sets the number of cuts applied to divide closed faces.
  //! The number of resulting faces will be num+1.
  Standard_EXPORT void SetNbSplitPoints (const Standard_Integer num);




protected:





private:





};







#endif // _ShapeUpgrade_ShapeDivideClosed_HeaderFile
