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

#ifndef _StepFEA_FeaRepresentationItem_HeaderFile
#define _StepFEA_FeaRepresentationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentationItem.hxx>

class StepFEA_FeaRepresentationItem;
DEFINE_STANDARD_HANDLE(StepFEA_FeaRepresentationItem, StepRepr_RepresentationItem)

//! Representation of STEP entity FeaRepresentationItem
class StepFEA_FeaRepresentationItem : public StepRepr_RepresentationItem
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_FeaRepresentationItem();

  DEFINE_STANDARD_RTTIEXT(StepFEA_FeaRepresentationItem, StepRepr_RepresentationItem)

protected:
private:
};

#endif // _StepFEA_FeaRepresentationItem_HeaderFile
