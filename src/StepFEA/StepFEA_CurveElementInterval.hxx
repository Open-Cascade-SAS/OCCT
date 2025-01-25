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

#ifndef _StepFEA_CurveElementInterval_HeaderFile
#define _StepFEA_CurveElementInterval_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepFEA_CurveElementLocation;
class StepBasic_EulerAngles;

class StepFEA_CurveElementInterval;
DEFINE_STANDARD_HANDLE(StepFEA_CurveElementInterval, Standard_Transient)

//! Representation of STEP entity CurveElementInterval
class StepFEA_CurveElementInterval : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_CurveElementInterval();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(StepFEA_CurveElementLocation)& aFinishPosition,
                            const Handle(StepBasic_EulerAngles)&        aEuAngles);

  //! Returns field FinishPosition
  Standard_EXPORT Handle(StepFEA_CurveElementLocation) FinishPosition() const;

  //! Set field FinishPosition
  Standard_EXPORT void SetFinishPosition(
    const Handle(StepFEA_CurveElementLocation)& FinishPosition);

  //! Returns field EuAngles
  Standard_EXPORT Handle(StepBasic_EulerAngles) EuAngles() const;

  //! Set field EuAngles
  Standard_EXPORT void SetEuAngles(const Handle(StepBasic_EulerAngles)& EuAngles);

  DEFINE_STANDARD_RTTIEXT(StepFEA_CurveElementInterval, Standard_Transient)

protected:
private:
  Handle(StepFEA_CurveElementLocation) theFinishPosition;
  Handle(StepBasic_EulerAngles)        theEuAngles;
};

#endif // _StepFEA_CurveElementInterval_HeaderFile
