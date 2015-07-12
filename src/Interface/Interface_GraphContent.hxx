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

#ifndef _Interface_GraphContent_HeaderFile
#define _Interface_GraphContent_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_EntityIterator.hxx>
#include <Standard_Integer.hxx>
class Interface_Graph;
class Standard_Transient;
class Interface_EntityIterator;


//! Defines general form for classes of graph algorithms on
//! Interfaces, this form is that of EntityIterator
//! Each sub-class fills it according to its own algorithm
//! This also allows to combine any graph result to others,
//! all being given under one unique form
class Interface_GraphContent  : public Interface_EntityIterator
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty GraphContent, ready to be filled
  Standard_EXPORT Interface_GraphContent();
  
  //! Creates with all entities designated by a Graph
  Standard_EXPORT Interface_GraphContent(const Interface_Graph& agraph);
  
  //! Creates with entities having specific Status value in a Graph
  Standard_EXPORT Interface_GraphContent(const Interface_Graph& agraph, const Standard_Integer stat);
  
  //! Creates an Iterator with Shared entities of an entity
  //! (equivalente to EntityIterator but with a Graph)
  Standard_EXPORT Interface_GraphContent(const Interface_Graph& agraph, const Handle(Standard_Transient)& ent);
  
  //! Gets all Entities designated by a Graph (once created), adds
  //! them to those already recorded
  Standard_EXPORT void GetFromGraph (const Interface_Graph& agraph);
  
  //! Gets entities from a graph which have a specific Status value
  //! (one created), adds them to those already recorded
  Standard_EXPORT void GetFromGraph (const Interface_Graph& agraph, const Standard_Integer stat);
  
  //! Returns Result under the exact form of an EntityIterator :
  //! Can be used when EntityIterator itself is required (as a
  //! returned value for instance), whitout way for a sub-class
  Standard_EXPORT Interface_EntityIterator Result();
  
  //! Does the Evaluation before starting the iteration itself
  //! (in out)
  Standard_EXPORT void Begin();
  
  //! Evaluates list of Entities to be iterated. Called by Start
  //! Default is set to doing nothing : intended to be redefined
  //! by each sub-class
  Standard_EXPORT virtual void Evaluate();




protected:





private:





};







#endif // _Interface_GraphContent_HeaderFile
