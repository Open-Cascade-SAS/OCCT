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

#ifndef _StepBasic_Person_HeaderFile
#define _StepBasic_Person_HeaderFile

#include <Standard.hxx>

#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;

class StepBasic_Person : public Standard_Transient
{

public:
  //! Returns a Person
  Standard_EXPORT StepBasic_Person();

  Standard_EXPORT void Init(
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
    const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aSuffixTitles);

  Standard_EXPORT void SetId(const occ::handle<TCollection_HAsciiString>& aId);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Id() const;

  Standard_EXPORT void SetLastName(const occ::handle<TCollection_HAsciiString>& aLastName);

  Standard_EXPORT void UnSetLastName();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> LastName() const;

  Standard_EXPORT bool HasLastName() const;

  Standard_EXPORT void SetFirstName(const occ::handle<TCollection_HAsciiString>& aFirstName);

  Standard_EXPORT void UnSetFirstName();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> FirstName() const;

  Standard_EXPORT bool HasFirstName() const;

  Standard_EXPORT void SetMiddleNames(
    const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aMiddleNames);

  Standard_EXPORT void UnSetMiddleNames();

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>
                  MiddleNames() const;

  Standard_EXPORT bool HasMiddleNames() const;

  Standard_EXPORT occ::handle<TCollection_HAsciiString> MiddleNamesValue(const int num) const;

  Standard_EXPORT int NbMiddleNames() const;

  Standard_EXPORT void SetPrefixTitles(
    const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aPrefixTitles);

  Standard_EXPORT void UnSetPrefixTitles();

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>
                  PrefixTitles() const;

  Standard_EXPORT bool HasPrefixTitles() const;

  Standard_EXPORT occ::handle<TCollection_HAsciiString> PrefixTitlesValue(const int num) const;

  Standard_EXPORT int NbPrefixTitles() const;

  Standard_EXPORT void SetSuffixTitles(
    const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& aSuffixTitles);

  Standard_EXPORT void UnSetSuffixTitles();

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>
                  SuffixTitles() const;

  Standard_EXPORT bool HasSuffixTitles() const;

  Standard_EXPORT occ::handle<TCollection_HAsciiString> SuffixTitlesValue(const int num) const;

  Standard_EXPORT int NbSuffixTitles() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_Person, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString>                                   id;
  occ::handle<TCollection_HAsciiString>                                   lastName;
  occ::handle<TCollection_HAsciiString>                                   firstName;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> middleNames;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> prefixTitles;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> suffixTitles;
  bool                                                                    hasLastName;
  bool                                                                    hasFirstName;
  bool                                                                    hasMiddleNames;
  bool                                                                    hasPrefixTitles;
  bool                                                                    hasSuffixTitles;
};

#endif // _StepBasic_Person_HeaderFile
