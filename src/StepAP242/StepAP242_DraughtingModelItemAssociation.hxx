// Created on: 2015-10-29
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepAP242_DraughtingModelItemAssociation_HeaderFile
#define _StepAP242_DraughtingModelItemAssociation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepAP242_ItemIdentifiedRepresentationUsage.hxx>

class StepAP242_DraughtingModelItemAssociation;
DEFINE_STANDARD_HANDLE(StepAP242_DraughtingModelItemAssociation, StepAP242_ItemIdentifiedRepresentationUsage)
//! Added for Dimensional Tolerances
class StepAP242_DraughtingModelItemAssociation : public StepAP242_ItemIdentifiedRepresentationUsage
{

public:
  
  Standard_EXPORT StepAP242_DraughtingModelItemAssociation();

  DEFINE_STANDARD_RTTIEXT(StepAP242_DraughtingModelItemAssociation, StepAP242_ItemIdentifiedRepresentationUsage)

};
#endif // _StepAP242_DraughtingModelItemAssociation_HeaderFile
