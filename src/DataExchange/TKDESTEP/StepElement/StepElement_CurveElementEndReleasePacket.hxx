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

#ifndef _StepElement_CurveElementEndReleasePacket_HeaderFile
#define _StepElement_CurveElementEndReleasePacket_HeaderFile

#include <Standard.hxx>

#include <StepElement_CurveElementFreedom.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>

class StepElement_CurveElementEndReleasePacket;
DEFINE_STANDARD_HANDLE(StepElement_CurveElementEndReleasePacket, Standard_Transient)

//! Representation of STEP entity CurveElementEndReleasePacket
class StepElement_CurveElementEndReleasePacket : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepElement_CurveElementEndReleasePacket();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const StepElement_CurveElementFreedom& aReleaseFreedom,
                            const Standard_Real                    aReleaseStiffness);

  //! Returns field ReleaseFreedom
  Standard_EXPORT StepElement_CurveElementFreedom ReleaseFreedom() const;

  //! Set field ReleaseFreedom
  Standard_EXPORT void SetReleaseFreedom(const StepElement_CurveElementFreedom& ReleaseFreedom);

  //! Returns field ReleaseStiffness
  Standard_EXPORT Standard_Real ReleaseStiffness() const;

  //! Set field ReleaseStiffness
  Standard_EXPORT void SetReleaseStiffness(const Standard_Real ReleaseStiffness);

  DEFINE_STANDARD_RTTIEXT(StepElement_CurveElementEndReleasePacket, Standard_Transient)

protected:
private:
  StepElement_CurveElementFreedom theReleaseFreedom;
  Standard_Real                   theReleaseStiffness;
};

#endif // _StepElement_CurveElementEndReleasePacket_HeaderFile
