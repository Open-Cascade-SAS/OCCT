// Created on: 1994-06-06
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepDS_InterferenceIterator_HeaderFile
#define _TopOpeBRepDS_InterferenceIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepDS_Interference.hxx>

#include <NCollection_List.hxx>
#include <Standard_Boolean.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <Standard_Integer.hxx>
class TopOpeBRepDS_Interference;

//! Iterate on interferences of a list, matching
//! conditions on interferences.
//! Nota:
//! inheritance of ListIteratorOfListOfInterference from
//! TopOpeBRepDS has not been done because of the
//! impossibility of naming the classical More, Next
//! methods which are declared as static in
//! TCollection_ListIteratorOfList ... . ListIteratorOfList
//! has benn placed as a field of InterferenceIterator.
class TopOpeBRepDS_InterferenceIterator
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepDS_InterferenceIterator();

  //! Creates an iterator on the Interference of list <L>.
  Standard_EXPORT TopOpeBRepDS_InterferenceIterator(
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L);

  //! re-initialize interference iteration process on
  //! the list of interference <L>.
  //! Conditions are not modified.
  Standard_EXPORT void Init(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L);

  //! define a condition on interference iteration process.
  //! Interference must match the Geometry Kind <ST>
  Standard_EXPORT void GeometryKind(const TopOpeBRepDS_Kind GK);

  //! define a condition on interference iteration process.
  //! Interference must match the Geometry <G>
  Standard_EXPORT void Geometry(const int G);

  //! define a condition on interference iteration process.
  //! Interference must match the Support Kind <ST>
  Standard_EXPORT void SupportKind(const TopOpeBRepDS_Kind ST);

  //! define a condition on interference iteration process.
  //! Interference must match the Support <S>
  Standard_EXPORT void Support(const int S);

  //! reach for an interference matching the conditions
  //! (if defined).
  Standard_EXPORT void Match();

  //! Returns True if the Interference <I> matches the
  //! conditions (if defined).
  //! If no conditions defined, returns True.
  Standard_EXPORT virtual bool MatchInterference(
    const occ::handle<TopOpeBRepDS_Interference>& I) const;

  //! Returns True if there is a current Interference in
  //! the iteration.
  Standard_EXPORT bool More() const;

  //! Move to the next Interference.
  Standard_EXPORT void Next();

  //! Returns the current Interference, matching the
  //! conditions (if defined).
  Standard_EXPORT const occ::handle<TopOpeBRepDS_Interference>& Value() const;

  Standard_EXPORT NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator&
                  ChangeIterator();

private:
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator myIterator;
  bool                                                               myGKDef;
  TopOpeBRepDS_Kind                                                  myGK;
  bool                                                               myGDef;
  int                                                                myG;
  bool                                                               mySKDef;
  TopOpeBRepDS_Kind                                                  mySK;
  bool                                                               mySDef;
  int                                                                myS;
};

#endif // _TopOpeBRepDS_InterferenceIterator_HeaderFile
