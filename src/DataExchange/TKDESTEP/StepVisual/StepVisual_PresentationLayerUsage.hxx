// Created on: 1997-03-26
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepVisual_PresentationLayerUsage_HeaderFile
#define _StepVisual_PresentationLayerUsage_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepVisual_PresentationLayerAssignment;
class StepVisual_PresentationRepresentation;

//! Added from StepVisual Rev2 to Rev4
class StepVisual_PresentationLayerUsage : public Standard_Transient
{

public:
  Standard_EXPORT StepVisual_PresentationLayerUsage();

  Standard_EXPORT void Init(const occ::handle<StepVisual_PresentationLayerAssignment>& aAssignment,
                            const occ::handle<StepVisual_PresentationRepresentation>&  aPresentation);

  Standard_EXPORT void SetAssignment(
    const occ::handle<StepVisual_PresentationLayerAssignment>& aAssignment);

  Standard_EXPORT occ::handle<StepVisual_PresentationLayerAssignment> Assignment() const;

  Standard_EXPORT void SetPresentation(
    const occ::handle<StepVisual_PresentationRepresentation>& aPresentation);

  Standard_EXPORT occ::handle<StepVisual_PresentationRepresentation> Presentation() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_PresentationLayerUsage, Standard_Transient)

private:
  occ::handle<StepVisual_PresentationLayerAssignment> theAssignment;
  occ::handle<StepVisual_PresentationRepresentation>  thePresentation;
};

#endif // _StepVisual_PresentationLayerUsage_HeaderFile
