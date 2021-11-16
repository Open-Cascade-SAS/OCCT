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

#ifndef _StepRepr_CompoundRepresentationItem_HeaderFile
#define _StepRepr_CompoundRepresentationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;


class StepRepr_CompoundRepresentationItem;
DEFINE_STANDARD_HANDLE(StepRepr_CompoundRepresentationItem, StepRepr_RepresentationItem)

//! Added for Dimensional Tolerances
class StepRepr_CompoundRepresentationItem : public StepRepr_RepresentationItem
{

public:

  
  Standard_EXPORT StepRepr_CompoundRepresentationItem();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepRepr_HArray1OfRepresentationItem)& item_element);
  
  Standard_EXPORT Handle(StepRepr_HArray1OfRepresentationItem) ItemElement() const;
  
  Standard_EXPORT Standard_Integer NbItemElement() const;
  
  Standard_EXPORT void SetItemElement (const Handle(StepRepr_HArray1OfRepresentationItem)& item_element);
  
  Standard_EXPORT Handle(StepRepr_RepresentationItem) ItemElementValue (const Standard_Integer num) const;
  
  Standard_EXPORT void SetItemElementValue (const Standard_Integer num, const Handle(StepRepr_RepresentationItem)& anelement);




  DEFINE_STANDARD_RTTIEXT(StepRepr_CompoundRepresentationItem,StepRepr_RepresentationItem)

protected:




private:


  Handle(StepRepr_HArray1OfRepresentationItem) theItemElement;


};







#endif // _StepRepr_CompoundRepresentationItem_HeaderFile
