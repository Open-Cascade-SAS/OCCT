// Created on: 1992-11-17
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

#ifndef _IFSelect_Dispatch_HeaderFile
#define _IFSelect_Dispatch_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class IFSelect_Selection;
class IFSelect_SelectionIterator;
class TCollection_AsciiString;
class Interface_EntityIterator;
class Interface_Graph;
class IFGraph_SubPartsIterator;

class IFSelect_Dispatch;
DEFINE_STANDARD_HANDLE(IFSelect_Dispatch, Standard_Transient)

//! This class allows to describe how a set of Entities has to be
//! dispatched into resulting Packets : a Packet is a sub-set of
//! the initial set of entities.
//!
//! Thus, it can generate zero, one, or more Packets according
//! input set and criterium of dispatching. And it can let apart
//! some entities : it is the Remainder, which can be recovered
//! by a specific Selection (RemainderFromDispatch).
//!
//! Depending of sub-classes, a Dispatch can potentially generate
//! a limited or not count of packet, and a remainder or none.
//!
//! The input set is read from a specified Selection, attached to
//! the Dispatch : the Final Selection of the Dispatch. The input
//! is the Unique Root Entities list of the Final Selection
class IFSelect_Dispatch : public Standard_Transient
{

public:
  //! Sets a Root Name as an HAsciiString
  //! To reset it, give a Null Handle (then, a ShareOut will have
  //! to define the Default Root Name)
  Standard_EXPORT void SetRootName(const Handle(TCollection_HAsciiString)& name);

  //! Returns True if a specific Root Name has been set
  //! (else, the Default Root Name has to be used)
  Standard_EXPORT Standard_Boolean HasRootName() const;

  //! Returns the Root Name for files produced by this dispatch
  //! It is empty if it has not been set or if it has been reset
  Standard_EXPORT const Handle(TCollection_HAsciiString)& RootName() const;

  //! Stores (or Changes) the Final Selection for a Dispatch
  Standard_EXPORT void SetFinalSelection(const Handle(IFSelect_Selection)& sel);

  //! Returns the Final Selection of a Dispatch
  //! we 'd like : C++ : return const &
  Standard_EXPORT Handle(IFSelect_Selection) FinalSelection() const;

  //! Returns the complete list of source Selections (starting
  //! from FinalSelection)
  Standard_EXPORT IFSelect_SelectionIterator Selections() const;

  //! Returns True if a Dispatch can have a Remainder, i.e. if its
  //! criterium can let entities apart. It is a potential answer,
  //! remainder can be empty at run-time even if answer is True.
  //! (to attach a RemainderFromDispatch Selection is not allowed if
  //! answer is True).
  //! Default answer given here is False (can be redefined)
  Standard_EXPORT virtual Standard_Boolean CanHaveRemainder() const;

  //! Returns True if a Dispatch generates a count of Packets always
  //! less than or equal to a maximum value : it can be computed
  //! from the total count of Entities to be dispatched : <nbent>.
  //! If answer is False, no limited maximum is expected for account
  //! If answer is True, expected maximum is given in argument <max>
  //! Default answer given here is False (can be redefined)
  Standard_EXPORT virtual Standard_Boolean LimitedMax(const Standard_Integer nbent,
                                                      Standard_Integer&      max) const;

  //! Returns a text which defines the way a Dispatch produces
  //! packets (which will become files) from its Input
  Standard_EXPORT virtual TCollection_AsciiString Label() const = 0;

  //! Gets Unique Root Entities from the Final Selection, given an
  //! input Graph
  //! This the starting step for an Evaluation (Packets - Remainder)
  Standard_EXPORT Interface_EntityIterator GetEntities(const Interface_Graph& G) const;

  //! Returns the list of produced Packets into argument <pack>.
  //! Each Packet corresponds to a Part, the Entities listed are the
  //! Roots given by the Selection. Input is given as a Graph.
  //! Thus, to create a file from a packet, it suffices to take the
  //! entities listed in a Part of Packets (that is, a Packet)
  //! without worrying about Shared entities
  //! This method can raise an Exception if data are not coherent
  Standard_EXPORT virtual void Packets(const Interface_Graph&    G,
                                       IFGraph_SubPartsIterator& packs) const = 0;

  //! Returns the list of all Input Entities (see GetEntities) which
  //! are put in a Packet. That is, Entities listed in GetEntities
  //! but not in Remainder (see below). Input is given as a Graph.
  Standard_EXPORT Interface_EntityIterator Packeted(const Interface_Graph& G) const;

  //! Returns Remainder which is a set of Entities. Can be empty.
  //! Default evaluation is empty (has to be redefined if
  //! CanHaveRemainder is redefined to return True).
  Standard_EXPORT virtual Interface_EntityIterator Remainder(const Interface_Graph& G) const;

  DEFINE_STANDARD_RTTIEXT(IFSelect_Dispatch, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString) thename;
  Handle(IFSelect_Selection)       thefinal;
};

#endif // _IFSelect_Dispatch_HeaderFile
