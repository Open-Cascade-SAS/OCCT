// Created on: 1992-02-03
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

#ifndef _Interface_EntityIterator_HeaderFile
#define _Interface_EntityIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HSequenceOfTransient.hxx>
#include <Standard_Type.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Interface_IntVal;
class Standard_Transient;


//! Defines an Iterator on Entities.
//! Allows considering of various criteria
class Interface_EntityIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines an empty iterator (see AddList & AddItem)
  Standard_EXPORT Interface_EntityIterator();
  
  //! Defines an iterator on a list, directly i.e. without copying it
  Standard_EXPORT Interface_EntityIterator(const Handle(TColStd_HSequenceOfTransient)& list);
  
  //! Gets a list of entities and adds its to the iteration list
  Standard_EXPORT void AddList (const Handle(TColStd_HSequenceOfTransient)& list);
  
  //! Adds to the iteration list a defined entity
  Standard_EXPORT void AddItem (const Handle(Standard_Transient)& anentity);
  
  //! same as AddItem (kept for compatibility)
  Standard_EXPORT void GetOneItem (const Handle(Standard_Transient)& anentity);
  
  //! Selects entities with are Kind of a given type,  keep only
  //! them (is keep is True) or reject only them (if keep is False)
  Standard_EXPORT void SelectType (const Handle(Standard_Type)& atype, const Standard_Boolean keep);
  
  //! Returns count of entities which will be iterated on
  //! Calls Start if not yet done
  Standard_EXPORT Standard_Integer NbEntities() const;
  
  //! Returns count of entities of a given type (kind of)
  Standard_EXPORT Standard_Integer NbTyped (const Handle(Standard_Type)& type) const;
  
  //! Returns the list of entities of a given type (kind of)
  Standard_EXPORT Interface_EntityIterator Typed (const Handle(Standard_Type)& type) const;
  
  //! Allows re-iteration (useless for the first iteration)
  Standard_EXPORT virtual void Start() const;
  
  //! Says if there are other entities (vertices) to iterate
  //! the first time, calls Start
  Standard_EXPORT Standard_Boolean More() const;
  
  //! Sets iteration to the next entity (vertex) to give
  Standard_EXPORT void Next() const;
  
  //! Returns the current Entity iterated, to be used by Interface
  //! tools
  Standard_EXPORT const Handle(Standard_Transient)& Value() const;
  
  //! Returns the content of the Iterator, accessed through a Handle
  //! to be used by a frontal-engine logic
  //! Returns an empty Sequence if the Iterator is empty
  //! Calls Start if not yet done
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) Content() const;
  
  //! Clears data of iteration
  Standard_EXPORT void Destroy();

  //! Destructor
  Standard_EXPORT virtual ~Interface_EntityIterator();

protected:

  
  //! Allows subclasses of EntityIterator to reevaluate an iteration
  Standard_EXPORT void Reset();




private:



  Handle(Interface_IntVal) thecurr;
  Handle(TColStd_HSequenceOfTransient) thelist;


};







#endif // _Interface_EntityIterator_HeaderFile
