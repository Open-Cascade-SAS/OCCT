// Created on: 1996-09-04
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Transfer_TransientProcess_HeaderFile
#define _Transfer_TransientProcess_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Transfer_ProcessForTransient.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_DataMap.hxx>
class Interface_InterfaceModel;
class Interface_HGraph;
class Interface_Graph;
class Interface_EntityIterator;

//! Adds specific features to the generic definition :
//! TransientProcess is intended to work from an InterfaceModel
//! to a set of application objects.
//!
//! Hence, some information about starting entities can be gotten
//! from the model : for Trace, CheckList, Integrity Status
class Transfer_TransientProcess : public Transfer_ProcessForTransient
{

public:
  //! Sets TransientProcess at initial state, with an initial size
  Standard_EXPORT Transfer_TransientProcess(const int nb = 10000);

  //! Sets an InterfaceModel, used by StartTrace, CheckList, queries
  //! on Integrity, to give information significant for each norm.
  Standard_EXPORT void SetModel(const occ::handle<Interface_InterfaceModel>& model);

  //! Returns the Model used for StartTrace
  Standard_EXPORT occ::handle<Interface_InterfaceModel> Model() const;

  //! Sets a Graph : supersedes SetModel if already done
  Standard_EXPORT void SetGraph(const occ::handle<Interface_HGraph>& HG);

  Standard_EXPORT bool HasGraph() const;

  Standard_EXPORT occ::handle<Interface_HGraph> HGraph() const;

  Standard_EXPORT const Interface_Graph& Graph() const;

  //! Sets a Context : according to receiving appli, to be
  //! interpreted by the Actor
  Standard_EXPORT void SetContext(const char* name, const occ::handle<Standard_Transient>& ctx);

  //! Returns the Context attached to a name, if set and if it is
  //! Kind of the type, else a Null Handle
  //! Returns True if OK, False if no Context
  Standard_EXPORT bool GetContext(const char*                       name,
                                  const occ::handle<Standard_Type>& type,
                                  occ::handle<Standard_Transient>&  ctx) const;

  //! Returns (modifiable) the whole definition of Context
  //! Rather for internal use (ex.: preparing and setting in once)
  Standard_EXPORT NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>&
                  Context();

  //! Specific printing to trace an entity : prints label and type
  //! (if model is set)
  Standard_EXPORT virtual void PrintTrace(const occ::handle<Standard_Transient>& start,
                                          Standard_OStream&                      S) const override;

  //! Specific number of a starting object for check-list : Number
  //! in model
  Standard_EXPORT virtual int CheckNum(const occ::handle<Standard_Transient>& ent) const override;

  //! Returns the list of sharings entities, AT ANY LEVEL, which are
  //! kind of a given type. Calls TypedSharings from Graph
  //! Returns an empty list if the Graph has not been acknowledged
  Standard_EXPORT Interface_EntityIterator
    TypedSharings(const occ::handle<Standard_Transient>& start,
                  const occ::handle<Standard_Type>&      type) const;

  //! Tells if an entity is well loaded from file (even if its data
  //! fail on checking, they are present). Mostly often, answers
  //! True. Else, there was a syntactic error in the file.
  //! A non-loaded entity MAY NOT BE transferred, unless its Report
  //! (in the model) is interpreted
  Standard_EXPORT bool IsDataLoaded(const occ::handle<Standard_Transient>& ent) const;

  //! Tells if an entity fails on data checking (load time,
  //! syntactic, or semantic check). Normally, should answer False.
  //! It is not prudent to try transferring an entity which fails on
  //! data checking
  Standard_EXPORT bool IsDataFail(const occ::handle<Standard_Transient>& ent) const;

  //! Prints statistics on a given output, according mode
  Standard_EXPORT void PrintStats(const int mode, Standard_OStream& S) const;

  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>
                  RootsForTransfer();

  DEFINE_STANDARD_RTTIEXT(Transfer_TransientProcess, Transfer_ProcessForTransient)

private:
  occ::handle<Interface_InterfaceModel>                                         themodel;
  occ::handle<Interface_HGraph>                                                 thegraph;
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> thectx;
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>           thetrroots;
};

#endif // _Transfer_TransientProcess_HeaderFile
