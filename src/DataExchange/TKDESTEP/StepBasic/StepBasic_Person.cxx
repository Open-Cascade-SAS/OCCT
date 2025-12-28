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

#include <StepBasic_Person.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_Person, Standard_Transient)

StepBasic_Person::StepBasic_Person() {}

void StepBasic_Person::Init(
  const occ::handle<TCollection_HAsciiString>&                                   aId,
  const bool                                                                     hasAlastName,
  const occ::handle<TCollection_HAsciiString>&                                   aLastName,
  const bool                                                                     hasAfirstName,
  const occ::handle<TCollection_HAsciiString>&                                   aFirstName,
  const bool                                                                     hasAmiddleNames,
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aMiddleNames,
  const bool                                                                     hasAprefixTitles,
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aPrefixTitles,
  const bool                                                                     hasAsuffixTitles,
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aSuffixTitles)
{
  // --- classe own fields ---
  id              = aId;
  hasLastName     = hasAlastName;
  lastName        = aLastName;
  hasFirstName    = hasAfirstName;
  firstName       = aFirstName;
  hasMiddleNames  = hasAmiddleNames;
  middleNames     = aMiddleNames;
  hasPrefixTitles = hasAprefixTitles;
  prefixTitles    = aPrefixTitles;
  hasSuffixTitles = hasAsuffixTitles;
  suffixTitles    = aSuffixTitles;
}

void StepBasic_Person::SetId(const occ::handle<TCollection_HAsciiString>& aId)
{
  id = aId;
}

occ::handle<TCollection_HAsciiString> StepBasic_Person::Id() const
{
  return id;
}

void StepBasic_Person::SetLastName(const occ::handle<TCollection_HAsciiString>& aLastName)
{
  lastName    = aLastName;
  hasLastName = true;
}

void StepBasic_Person::UnSetLastName()
{
  hasLastName = false;
  lastName.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Person::LastName() const
{
  return lastName;
}

bool StepBasic_Person::HasLastName() const
{
  return hasLastName;
}

void StepBasic_Person::SetFirstName(const occ::handle<TCollection_HAsciiString>& aFirstName)
{
  firstName    = aFirstName;
  hasFirstName = true;
}

void StepBasic_Person::UnSetFirstName()
{
  hasFirstName = false;
  firstName.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Person::FirstName() const
{
  return firstName;
}

bool StepBasic_Person::HasFirstName() const
{
  return hasFirstName;
}

void StepBasic_Person::SetMiddleNames(
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aMiddleNames)
{
  middleNames    = aMiddleNames;
  hasMiddleNames = true;
}

void StepBasic_Person::UnSetMiddleNames()
{
  hasMiddleNames = false;
  middleNames.Nullify();
}

occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> StepBasic_Person::
  MiddleNames() const
{
  return middleNames;
}

bool StepBasic_Person::HasMiddleNames() const
{
  return hasMiddleNames;
}

occ::handle<TCollection_HAsciiString> StepBasic_Person::MiddleNamesValue(const int num) const
{
  return middleNames->Value(num);
}

int StepBasic_Person::NbMiddleNames() const
{
  if (middleNames.IsNull())
    return 0;
  return middleNames->Length();
}

void StepBasic_Person::SetPrefixTitles(
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aPrefixTitles)
{
  prefixTitles    = aPrefixTitles;
  hasPrefixTitles = true;
}

void StepBasic_Person::UnSetPrefixTitles()
{
  hasPrefixTitles = false;
  prefixTitles.Nullify();
}

occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> StepBasic_Person::
  PrefixTitles() const
{
  return prefixTitles;
}

bool StepBasic_Person::HasPrefixTitles() const
{
  return hasPrefixTitles;
}

occ::handle<TCollection_HAsciiString> StepBasic_Person::PrefixTitlesValue(const int num) const
{
  return prefixTitles->Value(num);
}

int StepBasic_Person::NbPrefixTitles() const
{
  if (prefixTitles.IsNull())
    return 0;
  return prefixTitles->Length();
}

void StepBasic_Person::SetSuffixTitles(
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aSuffixTitles)
{
  suffixTitles    = aSuffixTitles;
  hasSuffixTitles = true;
}

void StepBasic_Person::UnSetSuffixTitles()
{
  hasSuffixTitles = false;
  suffixTitles.Nullify();
}

occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> StepBasic_Person::
  SuffixTitles() const
{
  return suffixTitles;
}

bool StepBasic_Person::HasSuffixTitles() const
{
  return hasSuffixTitles;
}

occ::handle<TCollection_HAsciiString> StepBasic_Person::SuffixTitlesValue(const int num) const
{
  return suffixTitles->Value(num);
}

int StepBasic_Person::NbSuffixTitles() const
{
  if (suffixTitles.IsNull())
    return 0;
  return suffixTitles->Length();
}
