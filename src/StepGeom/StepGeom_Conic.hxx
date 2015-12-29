// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepGeom_Conic_HeaderFile
#define _StepGeom_Conic_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_Axis2Placement.hxx>
#include <StepGeom_Curve.hxx>
class TCollection_HAsciiString;
class StepGeom_Axis2Placement;


class StepGeom_Conic;
DEFINE_STANDARD_HANDLE(StepGeom_Conic, StepGeom_Curve)


class StepGeom_Conic : public StepGeom_Curve
{

public:

  
  //! Returns a Conic
  Standard_EXPORT StepGeom_Conic();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const StepGeom_Axis2Placement& aPosition);
  
  Standard_EXPORT void SetPosition (const StepGeom_Axis2Placement& aPosition);
  
  Standard_EXPORT StepGeom_Axis2Placement Position() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_Conic,StepGeom_Curve)

protected:




private:


  StepGeom_Axis2Placement position;


};







#endif // _StepGeom_Conic_HeaderFile
