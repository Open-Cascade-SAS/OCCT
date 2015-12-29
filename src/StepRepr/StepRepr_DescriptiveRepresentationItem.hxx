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

#ifndef _StepRepr_DescriptiveRepresentationItem_HeaderFile
#define _StepRepr_DescriptiveRepresentationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentationItem.hxx>
class TCollection_HAsciiString;


class StepRepr_DescriptiveRepresentationItem;
DEFINE_STANDARD_HANDLE(StepRepr_DescriptiveRepresentationItem, StepRepr_RepresentationItem)


class StepRepr_DescriptiveRepresentationItem : public StepRepr_RepresentationItem
{

public:

  
  //! Returns a DescriptiveRepresentationItem
  Standard_EXPORT StepRepr_DescriptiveRepresentationItem();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription);
  
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& aDescription);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;




  DEFINE_STANDARD_RTTIEXT(StepRepr_DescriptiveRepresentationItem,StepRepr_RepresentationItem)

protected:




private:


  Handle(TCollection_HAsciiString) description;


};







#endif // _StepRepr_DescriptiveRepresentationItem_HeaderFile
