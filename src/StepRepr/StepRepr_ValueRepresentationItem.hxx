// Created on: 2015-09-09
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

#ifndef _StepRepr_ValueRepresentationItem_HeaderFile
#define _StepRepr_ValueRepresentationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentationItem.hxx>
class StepBasic_MeasureValueMember;
class TCollection_HAsciiString;


class StepRepr_ValueRepresentationItem;
DEFINE_STANDARD_HANDLE(StepRepr_ValueRepresentationItem, StepRepr_RepresentationItem)


class StepRepr_ValueRepresentationItem : public StepRepr_RepresentationItem
{

public:

  
  //! Returns a ValueRepresentationItem
  Standard_EXPORT StepRepr_ValueRepresentationItem();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& theName,
                             const Handle(StepBasic_MeasureValueMember)& theValueComponentMember);
  
  inline void SetValueComponentMember(const Handle(StepBasic_MeasureValueMember)& theValueComponentMember)
  {
    valueComponentMember = theValueComponentMember;
  }
  
  inline Handle(StepBasic_MeasureValueMember) ValueComponentMember() const 
  {
    return valueComponentMember;
  }

  DEFINE_STANDARD_RTTIEXT(StepRepr_ValueRepresentationItem,StepRepr_RepresentationItem)

private:
  Handle(StepBasic_MeasureValueMember) valueComponentMember;
};
#endif // _StepRepr_ValueRepresentationItem_HeaderFile
