// Created on: 2000-01-13
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepVisual_DraughtingModel_HeaderFile
#define _StepVisual_DraughtingModel_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_Representation.hxx>

class StepVisual_DraughtingModel;
DEFINE_STANDARD_HANDLE(StepVisual_DraughtingModel, StepRepr_Representation)

//! Representation of STEP entity DraughtingModel
class StepVisual_DraughtingModel : public StepRepr_Representation
{

public:
  //! Empty constructor
  Standard_EXPORT StepVisual_DraughtingModel();

  DEFINE_STANDARD_RTTIEXT(StepVisual_DraughtingModel, StepRepr_Representation)

protected:
private:
};

#endif // _StepVisual_DraughtingModel_HeaderFile
