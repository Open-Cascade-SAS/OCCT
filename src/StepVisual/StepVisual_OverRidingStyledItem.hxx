// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepVisual_OverRidingStyledItem_HeaderFile
#define _StepVisual_OverRidingStyledItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_StyledItem.hxx>
#include <StepVisual_HArray1OfPresentationStyleAssignment.hxx>
class StepVisual_StyledItem;
class TCollection_HAsciiString;
class StepRepr_RepresentationItem;


class StepVisual_OverRidingStyledItem;
DEFINE_STANDARD_HANDLE(StepVisual_OverRidingStyledItem, StepVisual_StyledItem)


class StepVisual_OverRidingStyledItem : public StepVisual_StyledItem
{

public:

  
  //! Returns a OverRidingStyledItem
  Standard_EXPORT StepVisual_OverRidingStyledItem();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles, const Handle(Standard_Transient)& aItem, const Handle(StepVisual_StyledItem)& aOverRiddenStyle);
  
  Standard_EXPORT void SetOverRiddenStyle (const Handle(StepVisual_StyledItem)& aOverRiddenStyle);
  
  Standard_EXPORT Handle(StepVisual_StyledItem) OverRiddenStyle() const;




  DEFINE_STANDARD_RTTIEXT(StepVisual_OverRidingStyledItem,StepVisual_StyledItem)

protected:




private:


  Handle(StepVisual_StyledItem) overRiddenStyle;


};







#endif // _StepVisual_OverRidingStyledItem_HeaderFile
