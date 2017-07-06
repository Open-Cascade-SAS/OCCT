// Created on: 1999-08-31
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

#ifndef _ShapeUpgrade_Tool_HeaderFile
#define _ShapeUpgrade_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
class ShapeBuild_ReShape;


class ShapeUpgrade_Tool;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_Tool, Standard_Transient)

//! Tool is a root class for splitting classes
//! Provides context for recording changes, basic
//! precision value and limit (minimal and maximal)
//! values for tolerances
class ShapeUpgrade_Tool : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT ShapeUpgrade_Tool();
  
  //! Copy all fields from another Root object
  Standard_EXPORT void Set (const Handle(ShapeUpgrade_Tool)& tool);
  
  //! Sets context
    void SetContext (const Handle(ShapeBuild_ReShape)& context);
  
  //! Returns context
    Handle(ShapeBuild_ReShape) Context() const;
  
  //! Sets basic precision value
    void SetPrecision (const Standard_Real preci);
  
  //! Returns basic precision value
    Standard_Real Precision() const;
  
  //! Sets minimal allowed tolerance
    void SetMinTolerance (const Standard_Real mintol);
  
  //! Returns minimal allowed tolerance
    Standard_Real MinTolerance() const;
  
  //! Sets maximal allowed tolerance
    void SetMaxTolerance (const Standard_Real maxtol);
  
  //! Returns maximal allowed tolerance
    Standard_Real MaxTolerance() const;
  
  //! Returns tolerance limited by [myMinTol,myMaxTol]
    Standard_Real LimitTolerance (const Standard_Real toler) const;




  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_Tool,Standard_Transient)

protected:




private:


  Handle(ShapeBuild_ReShape) myContext;
  Standard_Real myPrecision;
  Standard_Real myMinTol;
  Standard_Real myMaxTol;


};


#include <ShapeUpgrade_Tool.lxx>





#endif // _ShapeUpgrade_Tool_HeaderFile
