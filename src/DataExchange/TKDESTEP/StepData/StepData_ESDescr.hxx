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

#ifndef _StepData_ESDescr_HeaderFile
#define _StepData_ESDescr_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepData_EDescr.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>
class StepData_PDescr;
class StepData_Described;

//! This class is intended to describe the authorized form for a
//! Simple (not Plex) Entity, as a list of fields
class StepData_ESDescr : public StepData_EDescr
{

public:
  //! Creates an ESDescr with a type name
  Standard_EXPORT StepData_ESDescr(const char* const name);

  //! Sets a new count of fields
  //! Each one is described by a PDescr
  Standard_EXPORT void SetNbFields(const int nb);

  //! Sets a PDescr to describe a field
  //! A Field is designated by its rank and name
  Standard_EXPORT void SetField(const int                           num,
                                const char* const                   name,
                                const occ::handle<StepData_PDescr>& descr);

  //! Sets an ESDescr as based on another one
  //! Hence, if there are inherited fields, the derived ESDescr
  //! cumulates all them, while the base just records its own ones
  Standard_EXPORT void SetBase(const occ::handle<StepData_ESDescr>& base);

  //! Sets an ESDescr as "super-type". Applies an a base (non
  //! derived) ESDescr
  Standard_EXPORT void SetSuper(const occ::handle<StepData_ESDescr>& super);

  //! Returns the type name given at creation time
  Standard_EXPORT const char* TypeName() const;

  //! Returns the type name as an AsciiString
  Standard_EXPORT const TCollection_AsciiString& StepType() const;

  //! Returns the basic ESDescr, null if <me> is not derived
  Standard_EXPORT occ::handle<StepData_ESDescr> Base() const;

  //! Returns the super-type ESDescr, null if <me> is root
  Standard_EXPORT occ::handle<StepData_ESDescr> Super() const;

  //! Tells if <me> is sub-type of (or equal to) another one
  Standard_EXPORT bool IsSub(const occ::handle<StepData_ESDescr>& other) const;

  //! Returns the count of fields
  Standard_EXPORT int NbFields() const;

  //! Returns the rank of a field from its name. 0 if unknown
  Standard_EXPORT int Rank(const char* const name) const;

  //! Returns the name of a field from its rank. empty if outofrange
  Standard_EXPORT const char* Name(const int num) const;

  //! Returns the PDescr for the field <num> (or Null)
  Standard_EXPORT occ::handle<StepData_PDescr> Field(const int num) const;

  //! Returns the PDescr for the field named <name> (or Null)
  Standard_EXPORT occ::handle<StepData_PDescr> NamedField(const char* const name) const;

  //! Tells if a ESDescr matches a step type : exact or super type
  Standard_EXPORT bool Matches(const char* const steptype) const override;

  //! Returns False
  Standard_EXPORT bool IsComplex() const override;

  //! Creates a described entity (i.e. a simple one)
  Standard_EXPORT occ::handle<StepData_Described> NewEntity() const override;

  DEFINE_STANDARD_RTTIEXT(StepData_ESDescr, StepData_EDescr)

private:
  TCollection_AsciiString                                           thenom;
  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> thedescr;
  NCollection_DataMap<TCollection_AsciiString, int>                 thenames;
  occ::handle<StepData_ESDescr>                                     thebase;
  occ::handle<StepData_ESDescr>                                     thesuper;
};

#endif // _StepData_ESDescr_HeaderFile
