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

#ifndef _IFSelect_SelectionIterator_HeaderFile
#define _IFSelect_SelectionIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <IFSelect_HSeqOfSelection.hxx>
#include <IFSelect_TSeqOfSelection.hxx>
class IFSelect_Selection;

//! Defines an Iterator on a list of Selections
class IFSelect_SelectionIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates an empty iterator, ready to be filled
  Standard_EXPORT IFSelect_SelectionIterator();
  
  //! Creates an iterator from a Selection : it lists the Selections
  //! from which <sel> depends (given by its method FillIterator)
  Standard_EXPORT IFSelect_SelectionIterator(const Handle(IFSelect_Selection)& sel);
  
  //! Adds to an iterator the content of another one
  //! (each selection is present only once in the result)
  Standard_EXPORT void AddFromIter (IFSelect_SelectionIterator& iter);
  
  //! Adds a Selection to an iterator (if not yet noted)
  Standard_EXPORT void AddItem (const Handle(IFSelect_Selection)& sel);
  
  //! Adds a list of Selections to an iterator (this list comes
  //! from the description of a Selection or a Dispatch, etc...)
  Standard_EXPORT void AddList (const IFSelect_TSeqOfSelection& list);
  
  //! Returns True if there are more Selections to get
  Standard_EXPORT Standard_Boolean More() const;
  
  //! Sets iterator to the next item
  Standard_EXPORT void Next();

  //! Returns the current Selection being iterated
  //! Error if count of Selection has been passed
  Standard_EXPORT const Handle(IFSelect_Selection)& Value() const;

private:

  Standard_Integer thecurr;
  Handle(IFSelect_HSeqOfSelection) thelist;

};

#endif // _IFSelect_SelectionIterator_HeaderFile
