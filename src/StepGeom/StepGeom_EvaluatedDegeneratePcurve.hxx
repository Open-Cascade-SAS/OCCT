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

#ifndef _StepGeom_EvaluatedDegeneratePcurve_HeaderFile
#define _StepGeom_EvaluatedDegeneratePcurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_DegeneratePcurve.hxx>
class StepGeom_CartesianPoint;
class TCollection_HAsciiString;
class StepGeom_Surface;
class StepRepr_DefinitionalRepresentation;


class StepGeom_EvaluatedDegeneratePcurve;
DEFINE_STANDARD_HANDLE(StepGeom_EvaluatedDegeneratePcurve, StepGeom_DegeneratePcurve)


class StepGeom_EvaluatedDegeneratePcurve : public StepGeom_DegeneratePcurve
{

public:

  
  //! Returns a EvaluatedDegeneratePcurve
  Standard_EXPORT StepGeom_EvaluatedDegeneratePcurve();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_Surface)& aBasisSurface, const Handle(StepRepr_DefinitionalRepresentation)& aReferenceToCurve, const Handle(StepGeom_CartesianPoint)& aEquivalentPoint);
  
  Standard_EXPORT void SetEquivalentPoint (const Handle(StepGeom_CartesianPoint)& aEquivalentPoint);
  
  Standard_EXPORT Handle(StepGeom_CartesianPoint) EquivalentPoint() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_EvaluatedDegeneratePcurve,StepGeom_DegeneratePcurve)

protected:




private:


  Handle(StepGeom_CartesianPoint) equivalentPoint;


};







#endif // _StepGeom_EvaluatedDegeneratePcurve_HeaderFile
