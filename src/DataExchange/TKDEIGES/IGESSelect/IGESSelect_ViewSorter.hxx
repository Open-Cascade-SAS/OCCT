// Created on: 1994-05-31
// Created by: Christian CAILLET
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

#ifndef _IGESSelect_ViewSorter_HeaderFile
#define _IGESSelect_ViewSorter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Integer.hxx>
class IGESData_IGESModel;
class IGESData_IGESEntity;
class Interface_InterfaceModel;
class Interface_Graph;
class IFSelect_PacketList;

//! Sorts IGES Entities on the views and drawings.
//! In a first step, it splits a set of entities according the
//! different views they are attached to.
//! Then, packets according single views (+ drawing frames), or
//! according drawings (which refer to the views) can be determined
//!
//! It is a TShared, hence it can be a workomg field of a non-
//! mutable object (a Dispatch for instance)
class IGESSelect_ViewSorter : public Standard_Transient
{

public:
  //! Creates a ViewSorter, empty. SetModel remains to be called
  Standard_EXPORT IGESSelect_ViewSorter();

  //! Sets the Model (for PacketList)
  Standard_EXPORT void SetModel(const occ::handle<IGESData_IGESModel>& model);

  //! Clears recorded data
  Standard_EXPORT void Clear();

  //! Adds an item according its type : AddEntity,AddList,AddModel
  Standard_EXPORT bool Add(const occ::handle<Standard_Transient>& ent);

  //! Adds an IGES entity. Records the view it is attached to.
  //! Records directly <ent> if it is a ViewKindEntity or a Drawing
  //! Returns True if added, False if already in the map
  Standard_EXPORT bool AddEntity(const occ::handle<IGESData_IGESEntity>& igesent);

  //! Adds a list of entities by adding each of the items
  Standard_EXPORT void AddList(const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list);

  //! Adds all the entities contained in a Model
  Standard_EXPORT void AddModel(const occ::handle<Interface_InterfaceModel>& model);

  //! Returns the count of already recorded
  Standard_EXPORT int NbEntities() const;

  //! Prepares the result to keep only sets attached to Single Views
  //! If <alsoframes> is given True, it keeps also the Drawings as
  //! specific sets, in order to get their frames.
  //! Entities attached to no single view are put in Remaining List.
  //!
  //! Result can then be read by the methods NbSets,SetItem,SetList,
  //! RemainingList(final = True)
  Standard_EXPORT void SortSingleViews(const bool alsoframes);

  //! Prepares the result to the sets attached to Drawings :
  //! All the single views referenced by a Drawing become bound to
  //! the set for this Drawing
  //!
  //! Entities or Views which correspond to no Drawing are put into
  //! the Remaining List.
  //!
  //! Result can then be read by the methods NbSets,SetItem,SetList,
  //! RemainingList(final = True)
  Standard_EXPORT void SortDrawings(const Interface_Graph& G);

  //! Returns the count of sets recorded, one per distinct item.
  //! The Remaining List is not counted.
  //! If <final> is False, the sets are attached to distinct views
  //! determined by the method Add.
  //! If <final> is True, they are the sets determined by the last
  //! call to, either SortSingleViews, or SortDrawings.
  //!
  //! Warning : Drawings directly recorded are also counted as sets, because
  //! of their Frame (which is made of Annotations)
  Standard_EXPORT int NbSets(const bool final) const;

  //! Returns the Item which is attached to a set of entities
  //! For <final> and definition of sets, see method NbSets.
  //! This item can be a kind of View or a Drawing
  Standard_EXPORT occ::handle<IGESData_IGESEntity> SetItem(const int num,
                                                      const bool final) const;

  //! Returns the complete content of the determined Sets, which
  //! include Duplicated and Remaining (duplication 0) lists
  //! For <final> and definition of sets, see method NbSets.
  Standard_EXPORT occ::handle<IFSelect_PacketList> Sets(const bool final) const;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_ViewSorter, Standard_Transient)

private:
  occ::handle<IGESData_IGESModel>    themodel;
  NCollection_IndexedMap<occ::handle<Standard_Transient>> themap;
  NCollection_IndexedMap<occ::handle<Standard_Transient>> theitems;
  NCollection_IndexedMap<occ::handle<Standard_Transient>> thefinals;
  NCollection_Sequence<int>     theinditem;
  NCollection_Sequence<int>     theindfin;
};

#endif // _IGESSelect_ViewSorter_HeaderFile
