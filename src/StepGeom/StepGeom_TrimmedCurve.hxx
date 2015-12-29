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

#ifndef _StepGeom_TrimmedCurve_HeaderFile
#define _StepGeom_TrimmedCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_HArray1OfTrimmingSelect.hxx>
#include <Standard_Boolean.hxx>
#include <StepGeom_TrimmingPreference.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <Standard_Integer.hxx>
class StepGeom_Curve;
class TCollection_HAsciiString;
class StepGeom_TrimmingSelect;


class StepGeom_TrimmedCurve;
DEFINE_STANDARD_HANDLE(StepGeom_TrimmedCurve, StepGeom_BoundedCurve)


class StepGeom_TrimmedCurve : public StepGeom_BoundedCurve
{

public:

  
  //! Returns a TrimmedCurve
  Standard_EXPORT StepGeom_TrimmedCurve();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_Curve)& aBasisCurve, const Handle(StepGeom_HArray1OfTrimmingSelect)& aTrim1, const Handle(StepGeom_HArray1OfTrimmingSelect)& aTrim2, const Standard_Boolean aSenseAgreement, const StepGeom_TrimmingPreference aMasterRepresentation);
  
  Standard_EXPORT void SetBasisCurve (const Handle(StepGeom_Curve)& aBasisCurve);
  
  Standard_EXPORT Handle(StepGeom_Curve) BasisCurve() const;
  
  Standard_EXPORT void SetTrim1 (const Handle(StepGeom_HArray1OfTrimmingSelect)& aTrim1);
  
  Standard_EXPORT Handle(StepGeom_HArray1OfTrimmingSelect) Trim1() const;
  
  Standard_EXPORT StepGeom_TrimmingSelect Trim1Value (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbTrim1() const;
  
  Standard_EXPORT void SetTrim2 (const Handle(StepGeom_HArray1OfTrimmingSelect)& aTrim2);
  
  Standard_EXPORT Handle(StepGeom_HArray1OfTrimmingSelect) Trim2() const;
  
  Standard_EXPORT StepGeom_TrimmingSelect Trim2Value (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbTrim2() const;
  
  Standard_EXPORT void SetSenseAgreement (const Standard_Boolean aSenseAgreement);
  
  Standard_EXPORT Standard_Boolean SenseAgreement() const;
  
  Standard_EXPORT void SetMasterRepresentation (const StepGeom_TrimmingPreference aMasterRepresentation);
  
  Standard_EXPORT StepGeom_TrimmingPreference MasterRepresentation() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_TrimmedCurve,StepGeom_BoundedCurve)

protected:




private:


  Handle(StepGeom_Curve) basisCurve;
  Handle(StepGeom_HArray1OfTrimmingSelect) trim1;
  Handle(StepGeom_HArray1OfTrimmingSelect) trim2;
  Standard_Boolean senseAgreement;
  StepGeom_TrimmingPreference masterRepresentation;


};







#endif // _StepGeom_TrimmedCurve_HeaderFile
