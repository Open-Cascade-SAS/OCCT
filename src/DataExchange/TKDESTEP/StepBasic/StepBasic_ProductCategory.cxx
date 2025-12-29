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

#include <StepBasic_ProductCategory.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductCategory, Standard_Transient)

StepBasic_ProductCategory::StepBasic_ProductCategory() = default;

void StepBasic_ProductCategory::Init(const occ::handle<TCollection_HAsciiString>& aName,
                                     const bool                                   hasAdescription,
                                     const occ::handle<TCollection_HAsciiString>& aDescription)
{
  // --- classe own fields ---
  name           = aName;
  hasDescription = hasAdescription;
  description    = aDescription;
}

void StepBasic_ProductCategory::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  name = aName;
}

occ::handle<TCollection_HAsciiString> StepBasic_ProductCategory::Name() const
{
  return name;
}

void StepBasic_ProductCategory::SetDescription(
  const occ::handle<TCollection_HAsciiString>& aDescription)
{
  description    = aDescription;
  hasDescription = true;
}

void StepBasic_ProductCategory::UnSetDescription()
{
  hasDescription = false;
  description.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_ProductCategory::Description() const
{
  return description;
}

bool StepBasic_ProductCategory::HasDescription() const
{
  return hasDescription;
}
