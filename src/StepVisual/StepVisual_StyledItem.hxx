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

#ifndef _StepVisual_StyledItem_HeaderFile
#define _StepVisual_StyledItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_HArray1OfPresentationStyleAssignment.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_StyledItemTarget.hxx>
#include <Standard_Integer.hxx>
class StepRepr_RepresentationItem;
class TCollection_HAsciiString;
class StepVisual_PresentationStyleAssignment;


class StepVisual_StyledItem;
DEFINE_STANDARD_HANDLE(StepVisual_StyledItem, StepRepr_RepresentationItem)


class StepVisual_StyledItem : public StepRepr_RepresentationItem
{

public:

  
  //! Returns a StyledItem
  Standard_EXPORT StepVisual_StyledItem();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles, const Handle(Standard_Transient)& aItem);
  
  Standard_EXPORT void SetStyles (const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles);
  
  Standard_EXPORT Handle(StepVisual_HArray1OfPresentationStyleAssignment) Styles() const;
  
  Standard_EXPORT Handle(StepVisual_PresentationStyleAssignment) StylesValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbStyles() const;
  
  Standard_EXPORT void SetItem (const Handle(StepRepr_RepresentationItem)& aItem);
  
  Standard_EXPORT Handle(StepRepr_RepresentationItem) Item() const;

  Standard_EXPORT void SetItem (const StepVisual_StyledItemTarget& aItem);
  
  Standard_EXPORT StepVisual_StyledItemTarget ItemAP242() const;



  DEFINE_STANDARD_RTTIEXT(StepVisual_StyledItem,StepRepr_RepresentationItem)

protected:




private:


  Handle(StepVisual_HArray1OfPresentationStyleAssignment) myStyles;
  // May be StepRepr_RepresentationItem for AP214(203) and StepVisual_StyledItemTarget for AP242
  Handle(Standard_Transient) myItem;

};







#endif // _StepVisual_StyledItem_HeaderFile
