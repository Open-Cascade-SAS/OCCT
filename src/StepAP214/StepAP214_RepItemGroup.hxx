// Created on: 2000-05-10
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

#ifndef _StepAP214_RepItemGroup_HeaderFile
#define _StepAP214_RepItemGroup_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_Group.hxx>
#include <Standard_Boolean.hxx>
class StepRepr_RepresentationItem;
class TCollection_HAsciiString;


class StepAP214_RepItemGroup;
DEFINE_STANDARD_HANDLE(StepAP214_RepItemGroup, StepBasic_Group)

//! Representation of STEP entity RepItemGroup
class StepAP214_RepItemGroup : public StepBasic_Group
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepAP214_RepItemGroup();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aGroup_Name, const Standard_Boolean hasGroup_Description, const Handle(TCollection_HAsciiString)& aGroup_Description, const Handle(TCollection_HAsciiString)& aRepresentationItem_Name);
  
  //! Returns data for supertype RepresentationItem
  Standard_EXPORT Handle(StepRepr_RepresentationItem) RepresentationItem() const;
  
  //! Set data for supertype RepresentationItem
  Standard_EXPORT void SetRepresentationItem (const Handle(StepRepr_RepresentationItem)& RepresentationItem);




  DEFINE_STANDARD_RTTIEXT(StepAP214_RepItemGroup,StepBasic_Group)

protected:




private:


  Handle(StepRepr_RepresentationItem) theRepresentationItem;


};







#endif // _StepAP214_RepItemGroup_HeaderFile
