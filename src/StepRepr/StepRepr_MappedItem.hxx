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

#ifndef _StepRepr_MappedItem_HeaderFile
#define _StepRepr_MappedItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentationItem.hxx>
class StepRepr_RepresentationMap;
class StepRepr_RepresentationItem;
class TCollection_HAsciiString;


class StepRepr_MappedItem;
DEFINE_STANDARD_HANDLE(StepRepr_MappedItem, StepRepr_RepresentationItem)


class StepRepr_MappedItem : public StepRepr_RepresentationItem
{

public:

  
  //! Returns a MappedItem
  Standard_EXPORT StepRepr_MappedItem();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepRepr_RepresentationMap)& aMappingSource, const Handle(StepRepr_RepresentationItem)& aMappingTarget);
  
  Standard_EXPORT void SetMappingSource (const Handle(StepRepr_RepresentationMap)& aMappingSource);
  
  Standard_EXPORT Handle(StepRepr_RepresentationMap) MappingSource() const;
  
  Standard_EXPORT void SetMappingTarget (const Handle(StepRepr_RepresentationItem)& aMappingTarget);
  
  Standard_EXPORT Handle(StepRepr_RepresentationItem) MappingTarget() const;




  DEFINE_STANDARD_RTTIEXT(StepRepr_MappedItem,StepRepr_RepresentationItem)

protected:




private:


  Handle(StepRepr_RepresentationMap) mappingSource;
  Handle(StepRepr_RepresentationItem) mappingTarget;


};







#endif // _StepRepr_MappedItem_HeaderFile
