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

#ifndef _IFGraph_Articulations_HeaderFile
#define _IFGraph_Articulations_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_Graph.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Interface_GraphContent.hxx>
#include <Standard_Boolean.hxx>
class Interface_Graph;
class Standard_Transient;
class Interface_EntityIterator;


//! this class gives entities which are Articulation points
//! in a whole Model or in a sub-part
//! An Articulation Point divides the graph in two (or more)
//! disconnected sub-graphs
//! Identifying Articulation Points allows improving
//! efficiency of spliting a set of Entities into sub-sets
class IFGraph_Articulations  : public Interface_GraphContent
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates Articulations to evaluate a Graph
  //! whole True : works on the whole Model
  //! whole False : remains empty, ready to work on a sub-part
  Standard_EXPORT IFGraph_Articulations(const Interface_Graph& agraph, const Standard_Boolean whole);
  
  //! adds an entity and its shared ones to the list
  Standard_EXPORT void GetFromEntity (const Handle(Standard_Transient)& ent);
  
  //! adds a list of entities (as an iterator)
  Standard_EXPORT void GetFromIter (const Interface_EntityIterator& iter);
  
  //! Allows to restart on a new data set
  Standard_EXPORT void ResetData();
  
  //! Evaluates the list of Articulation points
  Standard_EXPORT virtual void Evaluate() Standard_OVERRIDE;




protected:





private:

  
  //! basic routine of computation
  //! (see book Sedgewick "Algorithms", p 392)
  Standard_EXPORT Standard_Integer Visit (const Standard_Integer num);


  Interface_Graph thegraph;
  Standard_Integer thenow;
  Handle(TColStd_HSequenceOfInteger) thelist;


};







#endif // _IFGraph_Articulations_HeaderFile
