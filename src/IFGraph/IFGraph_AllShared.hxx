// Created on: 1992-09-30
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

#ifndef _IFGraph_AllShared_HeaderFile
#define _IFGraph_AllShared_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>
class Interface_Graph;
class Standard_Transient;
class Interface_EntityIterator;


//! this class determines all Entities shared by some specific
//! ones, at any level (those which will be lead in a Transfer
//! for instance)
class IFGraph_AllShared  : public Interface_GraphContent
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates an AllShared from a graph, empty ready to be filled
  Standard_EXPORT IFGraph_AllShared(const Interface_Graph& agraph);
  
  //! creates an AllShared which memrizes Entities shared by a given
  //! one, at any level, including itself
  Standard_EXPORT IFGraph_AllShared(const Interface_Graph& agraph, const Handle(Standard_Transient)& ent);
  
  //! adds an entity and its shared ones to the list (allows to
  //! cumulate all Entities shared by some ones)
  Standard_EXPORT void GetFromEntity (const Handle(Standard_Transient)& ent);
  
  //! Adds Entities from an EntityIterator and all their shared
  //! ones at any level
  Standard_EXPORT void GetFromIter (const Interface_EntityIterator& iter);
  
  //! Allows to restart on a new data set
  Standard_EXPORT void ResetData();
  
  //! does the specific evaluation (shared entities atall levels)
  Standard_EXPORT virtual void Evaluate() Standard_OVERRIDE;




protected:





private:



  Interface_Graph thegraph;


};







#endif // _IFGraph_AllShared_HeaderFile
