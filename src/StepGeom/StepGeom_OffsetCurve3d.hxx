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

#ifndef _StepGeom_OffsetCurve3d_HeaderFile
#define _StepGeom_OffsetCurve3d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <StepData_Logical.hxx>
#include <StepGeom_Curve.hxx>
class StepGeom_Curve;
class StepGeom_Direction;
class TCollection_HAsciiString;


class StepGeom_OffsetCurve3d;
DEFINE_STANDARD_HANDLE(StepGeom_OffsetCurve3d, StepGeom_Curve)


class StepGeom_OffsetCurve3d : public StepGeom_Curve
{

public:

  
  //! Returns a OffsetCurve3d
  Standard_EXPORT StepGeom_OffsetCurve3d();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_Curve)& aBasisCurve, const Standard_Real aDistance, const StepData_Logical aSelfIntersect, const Handle(StepGeom_Direction)& aRefDirection);
  
  Standard_EXPORT void SetBasisCurve (const Handle(StepGeom_Curve)& aBasisCurve);
  
  Standard_EXPORT Handle(StepGeom_Curve) BasisCurve() const;
  
  Standard_EXPORT void SetDistance (const Standard_Real aDistance);
  
  Standard_EXPORT Standard_Real Distance() const;
  
  Standard_EXPORT void SetSelfIntersect (const StepData_Logical aSelfIntersect);
  
  Standard_EXPORT StepData_Logical SelfIntersect() const;
  
  Standard_EXPORT void SetRefDirection (const Handle(StepGeom_Direction)& aRefDirection);
  
  Standard_EXPORT Handle(StepGeom_Direction) RefDirection() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_OffsetCurve3d,StepGeom_Curve)

protected:




private:


  Handle(StepGeom_Curve) basisCurve;
  Standard_Real distance;
  StepData_Logical selfIntersect;
  Handle(StepGeom_Direction) refDirection;


};







#endif // _StepGeom_OffsetCurve3d_HeaderFile
