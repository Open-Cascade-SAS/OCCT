// Created by: DAUTRY Philippe
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

#ifndef _TDF_Delta_HeaderFile
#define _TDF_Delta_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TDF_AttributeDelta.hxx>
#include <NCollection_List.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Transient.hxx>
#include <TDF_Label.hxx>
#include <NCollection_List.hxx>
#include <Standard_OStream.hxx>
class TDF_AttributeDelta;

//! A set of AttributeDelta for a given transaction
//! number and reference time number.
//! A delta set is available at <aSourceTime>. If
//! applied, it restores the TDF_Data in the state it
//! was at <aTargetTime>.
class TDF_Delta : public Standard_Transient
{

public:
  //! Creates a delta.
  Standard_EXPORT TDF_Delta();

  //! Returns true if there is nothing to undo.
  bool IsEmpty() const;

  //! Returns true if the Undo action of <me> is
  //! applicable at <aCurrentTime>.
  bool IsApplicable(const int aCurrentTime) const;

  //! Returns the field <myBeginTime>.
  int BeginTime() const;

  //! Returns the field <myEndTime>.
  int EndTime() const;

  //! Adds in <aLabelList> the labels of the attribute deltas.
  //! Caution: <aLabelList> is not cleared before use.
  Standard_EXPORT void Labels(NCollection_List<TDF_Label>& aLabelList) const;

  //! Returns the field <myAttDeltaList>.
  const NCollection_List<occ::handle<TDF_AttributeDelta>>& AttributeDeltas() const;

  //! Returns a name associated with this delta.
  TCollection_ExtendedString Name() const;

  //! Associates a name <theName> with this delta
  void SetName(const TCollection_ExtendedString& theName);

  Standard_EXPORT void Dump(Standard_OStream& OS) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  friend class TDF_Data;

  DEFINE_STANDARD_RTTIEXT(TDF_Delta, Standard_Transient)

protected:
  //! Validates <me> at <aBeginTime>. If applied, it
  //! restores the TDF_Data in the state it was at
  //! <anEndTime>. Reserved to TDF_Data.
  Standard_EXPORT void Validity(const int aBeginTime,
                                const int anEndTime);

  //! Adds an AttributeDelta to the list. Reserved to
  //! TDF_Data.
  Standard_EXPORT void AddAttributeDelta(const occ::handle<TDF_AttributeDelta>& anAttributeDelta);

private:
  //! Replaces Attribute Delta List
  void ReplaceDeltaList(const NCollection_List<occ::handle<TDF_AttributeDelta>>& theList);

  void BeforeOrAfterApply(const bool before) const;

  void Apply();

  int           myBeginTime;
  int           myEndTime;
  NCollection_List<occ::handle<TDF_AttributeDelta>>     myAttDeltaList;
  TCollection_ExtendedString myName;
};

#include <TDF_Delta.lxx>

#endif // _TDF_Delta_HeaderFile
