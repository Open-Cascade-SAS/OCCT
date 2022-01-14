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

#ifndef _IFGraph_Compare_HeaderFile
#define _IFGraph_Compare_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>
class Standard_Transient;
class Interface_EntityIterator;


//! this class evaluates effect of two compared sub-parts :
//! cumulation (union), common part (intersection-overlapping)
//! part specific to first sub-part or to the second one
//! Results are kept in a Graph, several question can be set
//! Basic Iteration gives Cumulation (union)
class IFGraph_Compare  : public Interface_GraphContent
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates empty Compare, ready to work
  Standard_EXPORT IFGraph_Compare(const Interface_Graph& agraph);
  
  //! adds an entity and its shared ones to the list :
  //! first True means adds to the first sub-list, else to the 2nd
  Standard_EXPORT void GetFromEntity (const Handle(Standard_Transient)& ent, const Standard_Boolean first);
  
  //! adds a list of entities (as an iterator) as such, that is,
  //! their shared entities are not considered (use AllShared to
  //! have them)
  //! first True means adds to the first sub-list, else to the 2nd
  Standard_EXPORT void GetFromIter (const Interface_EntityIterator& iter, const Standard_Boolean first);
  
  //! merges the second list into the first one, hence the second
  //! list is empty
  Standard_EXPORT void Merge();
  
  //! Removes the contents of second list
  Standard_EXPORT void RemoveSecond();
  
  //! Keeps only Common part, sets it as First list and clears
  //! second list
  Standard_EXPORT void KeepCommon();
  
  //! Allows to restart on a new data set
  Standard_EXPORT void ResetData();
  
  //! Recomputes result of comparing to sub-parts
  Standard_EXPORT virtual void Evaluate() Standard_OVERRIDE;
  
  //! returns entities common to the both parts
  Standard_EXPORT Interface_EntityIterator Common() const;
  
  //! returns entities which are exclusively in the first list
  Standard_EXPORT Interface_EntityIterator FirstOnly() const;
  
  //! returns entities which are exclusively in the second part
  Standard_EXPORT Interface_EntityIterator SecondOnly() const;




protected:





private:



  Interface_Graph thegraph;


};







#endif // _IFGraph_Compare_HeaderFile
