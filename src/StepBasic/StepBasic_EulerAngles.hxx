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

#ifndef _StepBasic_EulerAngles_HeaderFile
#define _StepBasic_EulerAngles_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Transient.hxx>

class StepBasic_EulerAngles;
DEFINE_STANDARD_HANDLE(StepBasic_EulerAngles, Standard_Transient)

//! Representation of STEP entity EulerAngles
class StepBasic_EulerAngles : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepBasic_EulerAngles();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TColStd_HArray1OfReal)& aAngles);

  //! Returns field Angles
  Standard_EXPORT Handle(TColStd_HArray1OfReal) Angles() const;

  //! Set field Angles
  Standard_EXPORT void SetAngles(const Handle(TColStd_HArray1OfReal)& Angles);

  DEFINE_STANDARD_RTTIEXT(StepBasic_EulerAngles, Standard_Transient)

protected:
private:
  Handle(TColStd_HArray1OfReal) theAngles;
};

#endif // _StepBasic_EulerAngles_HeaderFile
