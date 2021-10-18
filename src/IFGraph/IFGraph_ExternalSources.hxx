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

#ifndef _IFGraph_ExternalSources_HeaderFile
#define _IFGraph_ExternalSources_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>
#include <Standard_Boolean.hxx>
class Standard_Transient;
class Interface_EntityIterator;


//! this class gives entities which are Source of entities of
//! a sub-part, but are not contained by this sub-part
class IFGraph_ExternalSources  : public Interface_GraphContent
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates empty ExternalSources, ready to work
  Standard_EXPORT IFGraph_ExternalSources(const Interface_Graph& agraph);
  
  //! adds an entity and its shared ones to the list
  Standard_EXPORT void GetFromEntity (const Handle(Standard_Transient)& ent);
  
  //! adds a list of entities (as an iterator) with shared ones
  Standard_EXPORT void GetFromIter (const Interface_EntityIterator& iter);
  
  //! Allows to restart on a new data set
  Standard_EXPORT void ResetData();
  
  //! Evaluates external sources of a set of entities
  Standard_EXPORT virtual void Evaluate() Standard_OVERRIDE;
  
  //! Returns True if no External Source are found
  //! It means that we have a "root" set
  //! (performs an Evaluation as necessary)
  Standard_EXPORT Standard_Boolean IsEmpty();




protected:





private:



  Interface_Graph thegraph;


};







#endif // _IFGraph_ExternalSources_HeaderFile
