// Created on: 1997-05-09
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepData_ECDescr_HeaderFile
#define _StepData_ECDescr_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <StepData_EDescr.hxx>
#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_CString.hxx>
class StepData_ESDescr;
class StepData_Described;

//! Describes a Complex Entity (Plex) as a list of Simple ones
class StepData_ECDescr : public StepData_EDescr
{

public:
  //! Creates an ECDescr, empty
  Standard_EXPORT StepData_ECDescr();

  //! Adds a member
  //! Warning : members are added in alphabetic order
  Standard_EXPORT void Add(const occ::handle<StepData_ESDescr>& member);

  //! Returns the count of members
  Standard_EXPORT int NbMembers() const;

  //! Returns a Member from its rank
  Standard_EXPORT occ::handle<StepData_ESDescr> Member(const int num) const;

  //! Returns the ordered list of types
  Standard_EXPORT occ::handle<NCollection_HSequence<TCollection_AsciiString>> TypeList() const;

  //! Tells if a ESDescr matches a step type : exact or super type
  Standard_EXPORT bool Matches(const char* const steptype) const override;

  //! Returns True
  Standard_EXPORT bool IsComplex() const override;

  //! Creates a described entity (i.e. a complex one, made of one
  //! simple entity per member)
  Standard_EXPORT occ::handle<StepData_Described> NewEntity() const override;

  DEFINE_STANDARD_RTTIEXT(StepData_ECDescr, StepData_EDescr)

private:
  NCollection_Sequence<occ::handle<Standard_Transient>> thelist;
};

#endif // _StepData_ECDescr_HeaderFile
