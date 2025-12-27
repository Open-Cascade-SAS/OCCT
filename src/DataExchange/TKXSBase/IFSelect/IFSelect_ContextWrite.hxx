// Created on: 1996-01-26
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

#ifndef _IFSelect_ContextWrite_HeaderFile
#define _IFSelect_ContextWrite_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_CheckIterator.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
class Interface_InterfaceModel;
class Interface_Protocol;
class IFSelect_AppliedModifiers;
class Interface_HGraph;
class IFSelect_GeneralModifier;
class Interface_Graph;
class Standard_Transient;
class Interface_Check;

//! This class gathers various information used by File Modifiers
//! apart from the writer object, which is specific of the norm
//! and of the physical format
//!
//! These information are controlled by an object AppliedModifiers
//! (if it is not defined, no modification is allowed on writing)
//!
//! Furthermore, in return, ContextModif can record Checks, either
//! one for all, or one for each Entity. It supports trace too.
class IFSelect_ContextWrite
{
public:
  DEFINE_STANDARD_ALLOC

  //! Prepares a ContextWrite with these information :
  //! - the model which is to be written
  //! - the protocol to be used
  //! - the filename
  //! - an object AppliedModifiers to work. It gives a list of
  //! FileModifiers to be ran, and for each one it can give
  //! a restricted list of entities (in the model), else all
  //! the model is considered
  Standard_EXPORT IFSelect_ContextWrite(const occ::handle<Interface_InterfaceModel>&  model,
                                        const occ::handle<Interface_Protocol>&        proto,
                                        const occ::handle<IFSelect_AppliedModifiers>& applieds,
                                        const char*                   filename);

  //! Same as above but with an already computed Graph
  Standard_EXPORT IFSelect_ContextWrite(const occ::handle<Interface_HGraph>&          hgraph,
                                        const occ::handle<Interface_Protocol>&        proto,
                                        const occ::handle<IFSelect_AppliedModifiers>& applieds,
                                        const char*                   filename);

  //! Returns the Model
  Standard_EXPORT occ::handle<Interface_InterfaceModel> Model() const;

  //! Returns the Protocol;
  Standard_EXPORT occ::handle<Interface_Protocol> Protocol() const;

  //! Returns the File Name
  Standard_EXPORT const char* FileName() const;

  //! Returns the object AppliedModifiers
  Standard_EXPORT occ::handle<IFSelect_AppliedModifiers> AppliedModifiers() const;

  //! Returns the Graph, either given when created, else created
  //! the first time it is queried
  Standard_EXPORT const Interface_Graph& Graph();

  //! Returns the count of recorded File Modifiers
  Standard_EXPORT int NbModifiers() const;

  //! Sets active the File Modifier n0 <numod>
  //! Then, it prepares the list of entities to consider, if any
  //! Returns False if <numod> out of range
  Standard_EXPORT bool SetModifier(const int numod);

  //! Returns the currently active File Modifier. Cast to be done
  //! Null if not properly set : must be test IsNull after casting
  Standard_EXPORT occ::handle<IFSelect_GeneralModifier> FileModifier() const;

  //! Returns True if no modifier is currently set
  Standard_EXPORT bool IsForNone() const;

  //! Returns True if the current modifier is to be applied to
  //! the whole model. Else, a restricted list of selected entities
  //! is defined, it can be exploited by the File Modifier
  Standard_EXPORT bool IsForAll() const;

  //! Returns the total count of selected entities
  Standard_EXPORT int NbEntities() const;

  //! Starts an iteration on selected items. It takes into account
  //! IsForAll/IsForNone, by really iterating on all selected items.
  Standard_EXPORT void Start();

  //! Returns True until the iteration has finished
  Standard_EXPORT bool More() const;

  //! Advances the iteration
  Standard_EXPORT void Next();

  //! Returns the current selected entity in the model
  Standard_EXPORT occ::handle<Standard_Transient> Value() const;

  //! Adds a Check to the CheckList. If it is empty, nothing is done
  //! If it concerns an Entity from the Model (by SetEntity)
  //! to which another Check is attached, it is merged to it.
  //! Else, it is added or merged as to GlobalCheck.
  Standard_EXPORT void AddCheck(const occ::handle<Interface_Check>& check);

  //! Adds a Warning Message for an Entity from the Model
  //! If <start> is not an Entity from the model (e.g. the
  //! model itself) this message is added to Global Check.
  Standard_EXPORT void AddWarning(const occ::handle<Standard_Transient>& start,
                                  const char*            mess,
                                  const char*            orig = "");

  //! Adds a Fail Message for an Entity from the Model
  //! If <start> is not an Entity from the model (e.g. the
  //! model itself) this message is added to Global Check.
  Standard_EXPORT void AddFail(const occ::handle<Standard_Transient>& start,
                               const char*            mess,
                               const char*            orig = "");

  //! Returns a Check given an Entity number (in the Model)
  //! by default a Global Check. Creates it the first time.
  //! It can then be acknowledged on the spot, in condition that the
  //! caller works by reference ("Interface_Check& check = ...")
  Standard_EXPORT occ::handle<Interface_Check> CCheck(const int num = 0);

  //! Returns a Check attached to an Entity from the Model
  //! It can then be acknowledged on the spot, in condition that the
  //! caller works by reference ("Interface_Check& check = ...")
  Standard_EXPORT occ::handle<Interface_Check> CCheck(const occ::handle<Standard_Transient>& start);

  //! Returns the complete CheckList
  Standard_EXPORT Interface_CheckIterator CheckList() const;

private:
  occ::handle<Interface_InterfaceModel>  themodel;
  occ::handle<Interface_Protocol>        theproto;
  TCollection_AsciiString           thefile;
  occ::handle<IFSelect_AppliedModifiers> theapply;
  occ::handle<Interface_HGraph>          thehgraf;
  Interface_CheckIterator           thecheck;
  int                  thenumod;
  int                  thenbent;
  int                  thecurr;
  occ::handle<IFSelect_GeneralModifier>  themodif;
};

#endif // _IFSelect_ContextWrite_HeaderFile
