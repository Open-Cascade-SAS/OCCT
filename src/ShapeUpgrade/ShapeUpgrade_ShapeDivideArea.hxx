// Created on: 2006-08-08
// Created by: Galina KULIKOVA
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#ifndef _ShapeUpgrade_ShapeDivideArea_HeaderFile
#define _ShapeUpgrade_ShapeDivideArea_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <ShapeUpgrade_ShapeDivide.hxx>
class TopoDS_Shape;
class ShapeUpgrade_FaceDivide;


//! Divides faces from sprcified shape  by max area criterium.
class ShapeUpgrade_ShapeDivideArea  : public ShapeUpgrade_ShapeDivide
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ShapeUpgrade_ShapeDivideArea();
  
  //! Initialize by a Shape.
  Standard_EXPORT ShapeUpgrade_ShapeDivideArea(const TopoDS_Shape& S);
  
  //! Set max area allowed for faces
    Standard_Real& MaxArea();




protected:

  
  //! Returns the tool for splitting faces.
  Standard_EXPORT virtual Handle(ShapeUpgrade_FaceDivide) GetSplitFaceTool() const Standard_OVERRIDE;




private:



  Standard_Real myMaxArea;


};


#include <ShapeUpgrade_ShapeDivideArea.lxx>





#endif // _ShapeUpgrade_ShapeDivideArea_HeaderFile
