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

#ifndef _IFGraph_Cumulate_HeaderFile
#define _IFGraph_Cumulate_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>
#include <Standard_Integer.hxx>
class Interface_Graph;
class Standard_Transient;
class Interface_EntityIterator;


//! this class evaluates effect of cumulated sub-parts :
//! overlapping, forgotten entities
//! Results are kept in a Graph, several question can be set
//! Basic Iteration gives entities which are part of Cumulation
class IFGraph_Cumulate  : public Interface_GraphContent
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates empty Cumulate, ready to work
  Standard_EXPORT IFGraph_Cumulate(const Interface_Graph& agraph);
  
  //! adds an entity and its shared ones to the list
  Standard_EXPORT void GetFromEntity (const Handle(Standard_Transient)& ent);
  
  //! adds a list of entities (as an iterator) as such, that is,
  //! without their shared entities (use AllShared to have them)
  Standard_EXPORT void GetFromIter (const Interface_EntityIterator& iter);
  
  //! Allows to restart on a new data set
  Standard_EXPORT void ResetData();
  
  //! Evaluates the result of cumulation
  Standard_EXPORT virtual void Evaluate() Standard_OVERRIDE;
  
  //! returns entities which are taken several times
  Standard_EXPORT Interface_EntityIterator Overlapped() const;
  
  //! returns entities which are not taken
  Standard_EXPORT Interface_EntityIterator Forgotten() const;
  
  //! Returns entities taken a given count of times
  //! (0 : same as Forgotten, 1 : same as no Overlap : default)
  Standard_EXPORT Interface_EntityIterator PerCount (const Standard_Integer count = 1) const;
  
  //! returns number of times an Entity has been counted
  //! (0 means forgotten, more than 1 means overlap, 1 is normal)
  Standard_EXPORT Standard_Integer NbTimes (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the highest number of times recorded for every Entity
  //! (0 means empty, 1 means no overlap)
  Standard_EXPORT Standard_Integer HighestNbTimes() const;




protected:





private:



  Interface_Graph thegraph;


};







#endif // _IFGraph_Cumulate_HeaderFile
