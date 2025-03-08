// Created on: 1992-09-23
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFGraph_SubPartsIterator_HeaderFile
#define _IFGraph_SubPartsIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_Graph.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Interface_InterfaceModel;
class Standard_Transient;
class Interface_EntityIterator;
class Interface_GraphContent;

//! defines general form for graph classes of which result is
//! not a single iteration on Entities, but a nested one :
//! External iteration works on sub-parts, identified by each
//! class (according to its algorithm)
//! Internal Iteration concerns Entities of a sub-part
//! Sub-Parts are assumed to be disjoined; if they are not,
//! the first one has priority
//!
//! A SubPartsIterator can work in two steps : first, load
//! entities which have to be processed
//! then, analyse to set those entities into sub-parts
class IFGraph_SubPartsIterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates with a Graph, whole or parts of it
  //! whole True  : works on the entire Model
  //! whole False : empty, ready to be filled
  //! SubPartIterator is set to load entities
  Standard_EXPORT IFGraph_SubPartsIterator(const Interface_Graph& agraph,
                                           const Standard_Boolean whole);

  //! Creates a SubPartIterator from another one and gets its Data
  //! Note that only non-empty sub-parts are taken into account
  //! PartNum is set to the last one
  Standard_EXPORT IFGraph_SubPartsIterator(IFGraph_SubPartsIterator& other);

  //! Gets Parts from another SubPartsIterator (in addition to the
  //! ones already recorded)
  //! Error if both SubPartsIterators are not based on the same Model
  Standard_EXPORT void GetParts(IFGraph_SubPartsIterator& other);

  //! Returns the Model with which this Iterator was created
  Standard_EXPORT Handle(Interface_InterfaceModel) Model() const;

  //! Adds an empty part and sets it to receive entities
  Standard_EXPORT void AddPart();

  //! Returns count of registered parts
  Standard_EXPORT Standard_Integer NbParts() const;

  //! Returns numero of part which currently receives entities
  //! (0 at load time)
  Standard_EXPORT Standard_Integer PartNum() const;

  //! Sets SubPartIterator to get Entities (by GetFromEntity &
  //! GetFromIter) into load status, to be analysed later
  Standard_EXPORT void SetLoad();

  //! Sets numero of receiving part to a new value
  //! Error if not in range (1-NbParts)
  Standard_EXPORT void SetPartNum(const Standard_Integer num);

  //! Adds an Entity : into load status if in Load mode, to the
  //! current part if there is one. If shared is True, adds
  //! also its shared ones (shared at all levels)
  Standard_EXPORT void GetFromEntity(const Handle(Standard_Transient)& ent,
                                     const Standard_Boolean            shared);

  //! Adds a list of Entities (into Load mode or to a Part),
  //! given as an Iterator
  Standard_EXPORT void GetFromIter(const Interface_EntityIterator& iter);

  //! Erases data (parts, entities) : "me" becomes empty and in
  //! load status
  Standard_EXPORT void Reset();

  //! Called by Clear, this method allows evaluation just before
  //! iteration; its default is doing nothing, it is designed to
  //! be redefined
  Standard_EXPORT virtual void Evaluate();

  //! Returns entities which where loaded (not set into a sub-part)
  Standard_EXPORT Interface_GraphContent Loaded() const;

  //! Same as above, but under the form of a Graph
  Standard_EXPORT Interface_Graph LoadedGraph() const;

  //! Returns True if an Entity is loaded (either set into a
  //! sub-part or not)
  Standard_EXPORT Standard_Boolean IsLoaded(const Handle(Standard_Transient)& ent) const;

  //! Returns True if an Entity is Present in a sub-part
  Standard_EXPORT Standard_Boolean IsInPart(const Handle(Standard_Transient)& ent) const;

  //! Returns number of the sub-part in which an Entity has been set
  //! if it is not in a sub-part (or not loaded at all), Returns 0
  Standard_EXPORT Standard_Integer EntityPartNum(const Handle(Standard_Transient)& ent) const;

  //! Sets iteration to its beginning; calls Evaluate
  Standard_EXPORT void Start();

  //! Returns True if there are more sub-parts to iterate on
  //! Note : an empty sub-part is not taken in account by Iteration
  Standard_EXPORT Standard_Boolean More();

  //! Sets iteration to the next sub-part
  //! if there is not, IsSingle-Entities will raises an exception
  Standard_EXPORT void Next();

  //! Returns True if current sub-part is single (has only one Entity)
  //! Error if there is no sub-part to iterate now
  Standard_EXPORT Standard_Boolean IsSingle() const;

  //! Returns the first entity of current sub-part, that is for a
  //! Single one, the only one it contains
  //! Error : same as above (end of iteration)
  Standard_EXPORT Handle(Standard_Transient) FirstEntity() const;

  //! Returns current sub-part, not as a "Value", but as an Iterator
  //! on Entities it contains
  //! Error : same as above (end of iteration)
  Standard_EXPORT Interface_EntityIterator Entities() const;

  Standard_EXPORT virtual ~IFGraph_SubPartsIterator();

protected:
  Interface_Graph thegraph;

private:
  //! Returns the Graph used by <me>. Used to create another
  //! SubPartsIterator from <me>
  Standard_EXPORT const Interface_Graph& Graph() const;

  Handle(TColStd_HSequenceOfInteger) theparts;
  Handle(TColStd_HSequenceOfInteger) thefirsts;
  Standard_Integer                   thepart;
  Standard_Integer                   thecurr;
};

#endif // _IFGraph_SubPartsIterator_HeaderFile
