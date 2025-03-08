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

#ifndef _StepRepr_Representation_HeaderFile
#define _StepRepr_Representation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepRepr_RepresentationContext;
class StepRepr_RepresentationItem;

class StepRepr_Representation;
DEFINE_STANDARD_HANDLE(StepRepr_Representation, Standard_Transient)

class StepRepr_Representation : public Standard_Transient
{

public:
  //! Returns a Representation
  Standard_EXPORT StepRepr_Representation();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&             aName,
                            const Handle(StepRepr_HArray1OfRepresentationItem)& aItems,
                            const Handle(StepRepr_RepresentationContext)&       aContextOfItems);

  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& aName);

  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  Standard_EXPORT void SetItems(const Handle(StepRepr_HArray1OfRepresentationItem)& aItems);

  Standard_EXPORT Handle(StepRepr_HArray1OfRepresentationItem) Items() const;

  Standard_EXPORT Handle(StepRepr_RepresentationItem) ItemsValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbItems() const;

  Standard_EXPORT void SetContextOfItems(
    const Handle(StepRepr_RepresentationContext)& aContextOfItems);

  Standard_EXPORT Handle(StepRepr_RepresentationContext) ContextOfItems() const;

  DEFINE_STANDARD_RTTIEXT(StepRepr_Representation, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString)             name;
  Handle(StepRepr_HArray1OfRepresentationItem) items;
  Handle(StepRepr_RepresentationContext)       contextOfItems;
};

#endif // _StepRepr_Representation_HeaderFile
