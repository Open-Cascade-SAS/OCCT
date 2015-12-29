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


#include <Standard_Type.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_SiUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_SiUnit,StepBasic_NamedUnit)

StepBasic_SiUnit::StepBasic_SiUnit ()  {}

void StepBasic_SiUnit::Init(const Standard_Boolean hasAprefix,
                            const StepBasic_SiPrefix aPrefix,
                            const StepBasic_SiUnitName aName)
{
  // --- classe own fields ---
  hasPrefix = hasAprefix;
  prefix = aPrefix;
  name = aName;
  // --- classe inherited fields ---
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_SiUnit::SetPrefix(const StepBasic_SiPrefix aPrefix)
{
  prefix = aPrefix;
  hasPrefix = Standard_True;
}


void StepBasic_SiUnit::UnSetPrefix()
{
  hasPrefix = Standard_False;
}


StepBasic_SiPrefix StepBasic_SiUnit::Prefix() const
{
  return prefix;
}


Standard_Boolean StepBasic_SiUnit::HasPrefix() const
{
  return hasPrefix;
}


void StepBasic_SiUnit::SetName(const StepBasic_SiUnitName aName)
{
  name = aName;
}


StepBasic_SiUnitName StepBasic_SiUnit::Name() const
{
  return name;
}


void StepBasic_SiUnit::SetDimensions(const Handle(StepBasic_DimensionalExponents)& /*aDimensions*/)
{
  // WARNING : the field is redefined.
  // field set up forbidden.
#ifdef OCCT_DEBUG
  cout << "Field is redefined, SetUp Forbidden" << endl;
#endif
}


Handle(StepBasic_DimensionalExponents) StepBasic_SiUnit::Dimensions() const
{
  // WARNING : the field is redefined.
  // method body is not yet automaticly wrote

  // attention : dimensional exponent retourne incorrect (pointeur NULL).
  // on devrait, en fonction du nom de l unite SI, construire un dimensional
  // exponents coherent. (du style .METRE. => (1,0,0,0,0,0) ... )

  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  return aDimensions;

}
