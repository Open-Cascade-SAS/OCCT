// Created on: 2001-04-24
// Created by: Christian CAILLET
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _StepRepr_ValueRange_HeaderFile
#define _StepRepr_ValueRange_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_CompoundRepresentationItem.hxx>

class StepRepr_ValueRange;
DEFINE_STANDARD_HANDLE(StepRepr_ValueRange, StepRepr_CompoundRepresentationItem)

//! Added for Dimensional Tolerances
class StepRepr_ValueRange : public StepRepr_CompoundRepresentationItem
{

public:
  Standard_EXPORT StepRepr_ValueRange();

  DEFINE_STANDARD_RTTIEXT(StepRepr_ValueRange, StepRepr_CompoundRepresentationItem)

protected:
private:
};

#endif // _StepRepr_ValueRange_HeaderFile
