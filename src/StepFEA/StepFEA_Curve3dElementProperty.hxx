// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_Curve3dElementProperty_HeaderFile
#define _StepFEA_Curve3dElementProperty_HeaderFile

#include <Standard.hxx>

#include <StepFEA_HArray1OfCurveElementInterval.hxx>
#include <StepFEA_HArray1OfCurveElementEndOffset.hxx>
#include <StepFEA_HArray1OfCurveElementEndRelease.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;


class StepFEA_Curve3dElementProperty;
DEFINE_STANDARD_HANDLE(StepFEA_Curve3dElementProperty, Standard_Transient)

//! Representation of STEP entity Curve3dElementProperty
class StepFEA_Curve3dElementProperty : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepFEA_Curve3dElementProperty();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aPropertyId, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepFEA_HArray1OfCurveElementInterval)& aIntervalDefinitions, const Handle(StepFEA_HArray1OfCurveElementEndOffset)& aEndOffsets, const Handle(StepFEA_HArray1OfCurveElementEndRelease)& aEndReleases);
  
  //! Returns field PropertyId
  Standard_EXPORT Handle(TCollection_HAsciiString) PropertyId() const;
  
  //! Set field PropertyId
  Standard_EXPORT void SetPropertyId (const Handle(TCollection_HAsciiString)& PropertyId);
  
  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;
  
  //! Set field Description
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& Description);
  
  //! Returns field IntervalDefinitions
  Standard_EXPORT Handle(StepFEA_HArray1OfCurveElementInterval) IntervalDefinitions() const;
  
  //! Set field IntervalDefinitions
  Standard_EXPORT void SetIntervalDefinitions (const Handle(StepFEA_HArray1OfCurveElementInterval)& IntervalDefinitions);
  
  //! Returns field EndOffsets
  Standard_EXPORT Handle(StepFEA_HArray1OfCurveElementEndOffset) EndOffsets() const;
  
  //! Set field EndOffsets
  Standard_EXPORT void SetEndOffsets (const Handle(StepFEA_HArray1OfCurveElementEndOffset)& EndOffsets);
  
  //! Returns field EndReleases
  Standard_EXPORT Handle(StepFEA_HArray1OfCurveElementEndRelease) EndReleases() const;
  
  //! Set field EndReleases
  Standard_EXPORT void SetEndReleases (const Handle(StepFEA_HArray1OfCurveElementEndRelease)& EndReleases);




  DEFINE_STANDARD_RTTIEXT(StepFEA_Curve3dElementProperty,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) thePropertyId;
  Handle(TCollection_HAsciiString) theDescription;
  Handle(StepFEA_HArray1OfCurveElementInterval) theIntervalDefinitions;
  Handle(StepFEA_HArray1OfCurveElementEndOffset) theEndOffsets;
  Handle(StepFEA_HArray1OfCurveElementEndRelease) theEndReleases;


};







#endif // _StepFEA_Curve3dElementProperty_HeaderFile
