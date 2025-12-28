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

#include <MoniTool_Macros.hxx>
#include <Standard_Type.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_PDescr.hxx>
#include <StepData_Simple.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_ESDescr, StepData_EDescr)

StepData_ESDescr::StepData_ESDescr(const char* name)
    : thenom(name)
{
  // Constructor for Simple Entity Descriptor with the given type name
}

void StepData_ESDescr::SetNbFields(const int nb)
{
  // Set the number of fields for this entity descriptor, preserving existing field data
  int minb, i, oldnb = NbFields();
  thenames.Clear(); // Clear name-to-index mapping
  if (nb == 0)
  {
    thedescr.Nullify();
    return;
  }
  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> li = new NCollection_HArray1<occ::handle<Standard_Transient>>(1, nb);
  if (oldnb == 0)
  {
    thedescr = li;
    return;
  }
  // Copy existing field descriptors up to the minimum of old and new sizes
  minb = (oldnb > nb ? nb : oldnb);
  for (i = 1; i <= minb; i++)
  {
    DeclareAndCast(StepData_PDescr, pde, thedescr->Value(i));
    if (!pde.IsNull())
      thenames.Bind(pde->Name(), i); // Rebuild name-to-index mapping
    li->SetValue(i, pde);
  }
  thedescr = li;
}

void StepData_ESDescr::SetField(const int         num,
                                const char*         name,
                                const occ::handle<StepData_PDescr>& descr)
{
  // Set field descriptor at specified position with given name and parameter descriptor
  if (num < 1 || num > NbFields())
    return;
  occ::handle<StepData_PDescr> pde = new StepData_PDescr;
  pde->SetFrom(descr); // Copy descriptor properties
  pde->SetName(name);  // Set field name
  thedescr->SetValue(num, pde);
  thenames.Bind(name, num); // Update name-to-index mapping
}

void StepData_ESDescr::SetBase(const occ::handle<StepData_ESDescr>& base)
{
  thebase = base;
  //  Need to ACCUMULATE the fields from the base and its superclasses
}

void StepData_ESDescr::SetSuper(const occ::handle<StepData_ESDescr>& super)
{
  // Set the superclass descriptor, handling inheritance hierarchy
  occ::handle<StepData_ESDescr> sup = super->Base();
  if (sup.IsNull())
    sup = super;
  if (!thebase.IsNull())
    thebase->SetSuper(sup); // Delegate to base if exists
  else
    thesuper = sup; // Otherwise set directly
}

const char* StepData_ESDescr::TypeName() const
{
  return thenom.ToCString();
}

const TCollection_AsciiString& StepData_ESDescr::StepType() const
{
  return thenom;
}

occ::handle<StepData_ESDescr> StepData_ESDescr::Base() const
{
  return thebase;
}

occ::handle<StepData_ESDescr> StepData_ESDescr::Super() const
{
  return thesuper;
}

bool StepData_ESDescr::IsSub(const occ::handle<StepData_ESDescr>& other) const
{
  // Check if this descriptor is a subclass of the given descriptor
  occ::handle<StepData_ESDescr> oth = other->Base();
  if (oth.IsNull())
    oth = other;
  if (!thebase.IsNull())
    return thebase->IsSub(oth); // Delegate to base if exists
  occ::handle<Standard_Transient> t1 = this;
  if (oth == t1)
    return true; // Same descriptor
  if (oth == thesuper)
    return true; // Direct superclass
  else if (thesuper.IsNull())
    return false;     // No superclass
  return thesuper->IsSub(oth); // Check recursively up the hierarchy
}

int StepData_ESDescr::NbFields() const
{
  return (thedescr.IsNull() ? 0 : thedescr->Length());
}

int StepData_ESDescr::Rank(const char* name) const
{
  int rank;
  if (!thenames.Find(name, rank))
    return 0;
  return rank;
}

const char* StepData_ESDescr::Name(const int num) const
{
  if (num < 1)
    return "";
  if (num > NbFields())
    return "";
  DeclareAndCast(StepData_PDescr, pde, thedescr->Value(num));
  return pde->Name();
}

occ::handle<StepData_PDescr> StepData_ESDescr::Field(const int num) const
{
  return GetCasted(StepData_PDescr, thedescr->Value(num));
}

occ::handle<StepData_PDescr> StepData_ESDescr::NamedField(const char* name) const
{
  occ::handle<StepData_PDescr> pde;
  int        rank = Rank(name);
  if (rank > 0)
    pde = GetCasted(StepData_PDescr, thedescr->Value(rank));
  return pde;
}

bool StepData_ESDescr::Matches(const char* name) const
{
  // Check if this descriptor matches the given type name (including inheritance)
  if (thenom.IsEqual(name))
    return true; // Direct match
  if (thesuper.IsNull())
    return false;        // No superclass to check
  return thesuper->Matches(name); // Check superclass hierarchy
}

bool StepData_ESDescr::IsComplex() const
{
  return false;
}

occ::handle<StepData_Described> StepData_ESDescr::NewEntity() const
{
  // Create a new simple entity instance based on this descriptor
  occ::handle<StepData_Simple> ent = new StepData_Simple(this);
  return ent;
}
