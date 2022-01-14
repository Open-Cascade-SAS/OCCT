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

#ifndef _IFSelect_Selection_HeaderFile
#define _IFSelect_Selection_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class IFSelect_SelectionIterator;
class TCollection_AsciiString;

class IFSelect_Selection;
DEFINE_STANDARD_HANDLE(IFSelect_Selection, Standard_Transient)

//! A Selection allows to define a set of Interface Entities.
//! Entities to be put on an output file should be identified in
//! a way as independent from such or such execution as possible.
//! This permits to handle comprehensive criteria, and to replay
//! them when a new variant of an input file has to be processed.
//!
//! Its input can be, either an Interface Model (the very source),
//! or another-other Selection(s) or any other output.
//! All list computations start from an input Graph (from IFGraph)
class IFSelect_Selection : public Standard_Transient
{

public:

  //! Returns the list of selected entities, computed from Input
  //! given as a Graph. Specific to each class of Selection
  //! Note that uniqueness of each entity is not required here
  //! This method can raise an exception as necessary
  Standard_EXPORT virtual Interface_EntityIterator RootResult (const Interface_Graph& G) const = 0;

  //! Returns the list of selected entities, each of them being
  //! unique. Default definition works from RootResult. According
  //! HasUniqueResult, UniqueResult returns directly RootResult,
  //! or build a Unique Result from it with a Graph.
  Standard_EXPORT Interface_EntityIterator UniqueResult (const Interface_Graph& G) const;

  //! Returns the list of entities involved by a Selection, i.e.
  //! UniqueResult plus the shared entities (directly or not)
  Standard_EXPORT virtual Interface_EntityIterator CompleteResult (const Interface_Graph& G) const;
  
  //! Puts in an Iterator the Selections from which "me" depends
  //! (there can be zero, or one, or a list).
  //! Specific to each class of Selection
  Standard_EXPORT virtual void FillIterator (IFSelect_SelectionIterator& iter) const = 0;
  
  //! Returns a text which defines the criterium applied by a
  //! Selection (can be used to be printed, displayed ...)
  //! Specific to each class
  Standard_EXPORT virtual TCollection_AsciiString Label() const = 0;

  DEFINE_STANDARD_RTTIEXT(IFSelect_Selection,Standard_Transient)

protected:

  //! Returns True if RootResult guarantees uniqueness for each
  //! Entity. Called by UniqueResult.
  //! Default answer is False. Can be redefined.
  Standard_EXPORT virtual Standard_Boolean HasUniqueResult() const;

};

#endif // _IFSelect_Selection_HeaderFile
