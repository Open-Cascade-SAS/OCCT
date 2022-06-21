// Created on: 1999-05-06
// Created by: Pavel DURANDIN
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

#ifndef _ShapeUpgrade_ShapeDivideAngle_HeaderFile
#define _ShapeUpgrade_ShapeDivideAngle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <ShapeUpgrade_ShapeDivide.hxx>
class TopoDS_Shape;


//! Splits all surfaces of revolution, cylindrical, toroidal,
//! conical, spherical surfaces in the given shape so that
//! each resulting segment covers not more than defined number
//! of degrees (to segments less than 90).
class ShapeUpgrade_ShapeDivideAngle  : public ShapeUpgrade_ShapeDivide
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_ShapeDivideAngle(const Standard_Real MaxAngle);
  
  //! Initialize by a Shape.
  Standard_EXPORT ShapeUpgrade_ShapeDivideAngle(const Standard_Real MaxAngle, const TopoDS_Shape& S);
  
  //! Resets tool for splitting face with given angle
  Standard_EXPORT void InitTool (const Standard_Real MaxAngle);
  
  //! Set maximal angle (calls InitTool)
  Standard_EXPORT void SetMaxAngle (const Standard_Real MaxAngle);
  
  //! Returns maximal angle
  Standard_EXPORT Standard_Real MaxAngle() const;




protected:





private:





};







#endif // _ShapeUpgrade_ShapeDivideAngle_HeaderFile
