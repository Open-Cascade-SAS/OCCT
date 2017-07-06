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

#ifndef _StepVisual_PresentedItemRepresentation_HeaderFile
#define _StepVisual_PresentedItemRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_PresentationRepresentationSelect.hxx>
#include <Standard_Transient.hxx>
class StepVisual_PresentedItem;
class StepVisual_PresentationRepresentationSelect;


class StepVisual_PresentedItemRepresentation;
DEFINE_STANDARD_HANDLE(StepVisual_PresentedItemRepresentation, Standard_Transient)

//! Added from StepVisual Rev2 to Rev4
class StepVisual_PresentedItemRepresentation : public Standard_Transient
{

public:

  
  Standard_EXPORT StepVisual_PresentedItemRepresentation();
  
  Standard_EXPORT void Init (const StepVisual_PresentationRepresentationSelect& aPresentation, const Handle(StepVisual_PresentedItem)& aItem);
  
  Standard_EXPORT void SetPresentation (const StepVisual_PresentationRepresentationSelect& aPresentation);
  
  Standard_EXPORT StepVisual_PresentationRepresentationSelect Presentation() const;
  
  Standard_EXPORT void SetItem (const Handle(StepVisual_PresentedItem)& aItem);
  
  Standard_EXPORT Handle(StepVisual_PresentedItem) Item() const;




  DEFINE_STANDARD_RTTIEXT(StepVisual_PresentedItemRepresentation,Standard_Transient)

protected:




private:


  StepVisual_PresentationRepresentationSelect thePresentation;
  Handle(StepVisual_PresentedItem) theItem;


};







#endif // _StepVisual_PresentedItemRepresentation_HeaderFile
