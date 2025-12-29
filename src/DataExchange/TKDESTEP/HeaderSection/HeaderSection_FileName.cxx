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

#include <HeaderSection_FileName.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HeaderSection_FileName, Standard_Transient)

HeaderSection_FileName::HeaderSection_FileName() = default;

void HeaderSection_FileName::Init(
  const occ::handle<TCollection_HAsciiString>&                                   aName,
  const occ::handle<TCollection_HAsciiString>&                                   aTimeStamp,
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aAuthor,
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aOrganization,
  const occ::handle<TCollection_HAsciiString>& aPreprocessorVersion,
  const occ::handle<TCollection_HAsciiString>& aOriginatingSystem,
  const occ::handle<TCollection_HAsciiString>& aAuthorisation)
{
  // --- class own fields ---
  name                = aName;
  timeStamp           = aTimeStamp;
  author              = aAuthor;
  organization        = aOrganization;
  preprocessorVersion = aPreprocessorVersion;
  originatingSystem   = aOriginatingSystem;
  authorisation       = aAuthorisation;
}

void HeaderSection_FileName::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  name = aName;
}

occ::handle<TCollection_HAsciiString> HeaderSection_FileName::Name() const
{
  return name;
}

void HeaderSection_FileName::SetTimeStamp(const occ::handle<TCollection_HAsciiString>& aTimeStamp)
{
  timeStamp = aTimeStamp;
}

occ::handle<TCollection_HAsciiString> HeaderSection_FileName::TimeStamp() const
{
  return timeStamp;
}

void HeaderSection_FileName::SetAuthor(
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aAuthor)
{
  author = aAuthor;
}

occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> HeaderSection_FileName::
  Author() const
{
  return author;
}

occ::handle<TCollection_HAsciiString> HeaderSection_FileName::AuthorValue(const int num) const
{
  return author->Value(num);
}

int HeaderSection_FileName::NbAuthor() const
{
  if (author.IsNull())
    return 0;
  return author->Length();
}

void HeaderSection_FileName::SetOrganization(
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aOrganization)
{
  organization = aOrganization;
}

occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> HeaderSection_FileName::
  Organization() const
{
  return organization;
}

occ::handle<TCollection_HAsciiString> HeaderSection_FileName::OrganizationValue(const int num) const
{
  return organization->Value(num);
}

int HeaderSection_FileName::NbOrganization() const
{
  if (organization.IsNull())
    return 0;
  return organization->Length();
}

void HeaderSection_FileName::SetPreprocessorVersion(
  const occ::handle<TCollection_HAsciiString>& aPreprocessorVersion)
{
  preprocessorVersion = aPreprocessorVersion;
}

occ::handle<TCollection_HAsciiString> HeaderSection_FileName::PreprocessorVersion() const
{
  return preprocessorVersion;
}

void HeaderSection_FileName::SetOriginatingSystem(
  const occ::handle<TCollection_HAsciiString>& aOriginatingSystem)
{
  originatingSystem = aOriginatingSystem;
}

occ::handle<TCollection_HAsciiString> HeaderSection_FileName::OriginatingSystem() const
{
  return originatingSystem;
}

void HeaderSection_FileName::SetAuthorisation(
  const occ::handle<TCollection_HAsciiString>& aAuthorisation)
{
  authorisation = aAuthorisation;
}

occ::handle<TCollection_HAsciiString> HeaderSection_FileName::Authorisation() const
{
  return authorisation;
}
