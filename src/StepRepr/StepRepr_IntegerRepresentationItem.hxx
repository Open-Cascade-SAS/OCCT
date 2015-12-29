// Created on: 2015-09-03
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

#ifndef _StepRepr_IntegerRepresentationItem_HeaderFile
#define _StepRepr_IntegerRepresentationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentationItem.hxx>
class TCollection_HAsciiString;


class StepRepr_IntegerRepresentationItem;
DEFINE_STANDARD_HANDLE(StepRepr_IntegerRepresentationItem, StepRepr_RepresentationItem)


class StepRepr_IntegerRepresentationItem : public StepRepr_RepresentationItem
{

public:

  
  //! Returns a IntegerRepresentationItem
  Standard_EXPORT StepRepr_IntegerRepresentationItem();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& theName, const Standard_Integer theValue);
  
  inline void SetValue(const Standard_Integer theValue)
  {
    value = theValue;
  }
  
  inline Standard_Integer Value() const 
  {
    return value;
  }

  DEFINE_STANDARD_RTTIEXT(StepRepr_IntegerRepresentationItem,StepRepr_RepresentationItem)

private:
  Standard_Integer value;
};
#endif // _StepRepr_IntegerRepresentationItem_HeaderFile
