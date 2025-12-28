// Created on: 1993-04-05
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Interface_ShareFlags_HeaderFile
#define _Interface_ShareFlags_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_BitMap.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Integer.hxx>
class Interface_InterfaceModel;
class Interface_GeneralLib;
class Interface_GTool;
class Interface_Protocol;
class Interface_Graph;
class Standard_Transient;
class Interface_EntityIterator;

//! This class only says for each Entity of a Model, if it is
//! Shared or not by one or more other(s) of this Model
//! It uses the General Service "Shared".
class Interface_ShareFlags
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a ShareFlags from a Model and builds required data
  //! (flags) by calling the General Service Library given as
  //! argument <lib>
  Standard_EXPORT Interface_ShareFlags(const occ::handle<Interface_InterfaceModel>& amodel,
                                       const Interface_GeneralLib&             lib);

  //! Same as above, but GeneralLib is detained by a GTool
  Standard_EXPORT Interface_ShareFlags(const occ::handle<Interface_InterfaceModel>& amodel,
                                       const occ::handle<Interface_GTool>&          gtool);

  //! Same as above, but GeneralLib is defined through a Protocol
  Standard_EXPORT Interface_ShareFlags(const occ::handle<Interface_InterfaceModel>& amodel,
                                       const occ::handle<Interface_Protocol>&       protocol);

  //! Same as above, but works with the GTool of the Model
  Standard_EXPORT Interface_ShareFlags(const occ::handle<Interface_InterfaceModel>& amodel);

  //! Creates a ShareFlags by querying information from a Graph
  //! (remark that Graph also has a method IsShared)
  Standard_EXPORT Interface_ShareFlags(const Interface_Graph& agraph);

  //! Returns the Model used for the evaluation
  Standard_EXPORT occ::handle<Interface_InterfaceModel> Model() const;

  //! Returns True if <ent> is Shared by one or more other
  //! Entity(ies) of the Model
  Standard_EXPORT bool IsShared(const occ::handle<Standard_Transient>& ent) const;

  //! Returns the Entities which are not Shared (see their flags)
  Standard_EXPORT Interface_EntityIterator RootEntities() const;

  //! Returns the count of root entities
  Standard_EXPORT int NbRoots() const;

  //! Returns a root entity according its rank in the list of roots
  //! By default, it returns the first one
  Standard_EXPORT occ::handle<Standard_Transient> Root(const int num = 1) const;

private:
  //! Computes flags at Creation time
  //!
  //! Normally, gtool suffices. But if a Graph is created from a
  //! GeneralLib directly, it cannot be used
  //! If <gtool> is defined, it has priority
  Standard_EXPORT void Evaluate(const Interface_GeneralLib&    lib,
                                const occ::handle<Interface_GTool>& gtool);

  occ::handle<Interface_InterfaceModel>     themodel;
  Interface_BitMap                     theflags;
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> theroots;
};

#endif // _Interface_ShareFlags_HeaderFile
