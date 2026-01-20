// Created on: 1994-06-16
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _HeaderSection_FileName_HeaderFile
#define _HeaderSection_FileName_HeaderFile

#include <Standard.hxx>

#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;

class HeaderSection_FileName : public Standard_Transient
{

public:
  //! Returns a FileName
  Standard_EXPORT HeaderSection_FileName();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&        aName,
                            const occ::handle<TCollection_HAsciiString>&        aTimeStamp,
                            const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aAuthor,
                            const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aOrganization,
                            const occ::handle<TCollection_HAsciiString>&        aPreprocessorVersion,
                            const occ::handle<TCollection_HAsciiString>&        aOriginatingSystem,
                            const occ::handle<TCollection_HAsciiString>&        aAuthorisation);

  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& aName);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  Standard_EXPORT void SetTimeStamp(const occ::handle<TCollection_HAsciiString>& aTimeStamp);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> TimeStamp() const;

  Standard_EXPORT void SetAuthor(const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aAuthor);

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> Author() const;

  Standard_EXPORT occ::handle<TCollection_HAsciiString> AuthorValue(const int num) const;

  Standard_EXPORT int NbAuthor() const;

  Standard_EXPORT void SetOrganization(
    const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aOrganization);

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> Organization() const;

  Standard_EXPORT occ::handle<TCollection_HAsciiString> OrganizationValue(
    const int num) const;

  Standard_EXPORT int NbOrganization() const;

  Standard_EXPORT void SetPreprocessorVersion(
    const occ::handle<TCollection_HAsciiString>& aPreprocessorVersion);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> PreprocessorVersion() const;

  Standard_EXPORT void SetOriginatingSystem(
    const occ::handle<TCollection_HAsciiString>& aOriginatingSystem);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> OriginatingSystem() const;

  Standard_EXPORT void SetAuthorisation(const occ::handle<TCollection_HAsciiString>& aAuthorisation);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Authorisation() const;

  DEFINE_STANDARD_RTTIEXT(HeaderSection_FileName, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString>        name;
  occ::handle<TCollection_HAsciiString>        timeStamp;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> author;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> organization;
  occ::handle<TCollection_HAsciiString>        preprocessorVersion;
  occ::handle<TCollection_HAsciiString>        originatingSystem;
  occ::handle<TCollection_HAsciiString>        authorisation;
};

#endif // _HeaderSection_FileName_HeaderFile
