// Created on: 1993-02-26
// Created by: Remi LEQUETTE
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

#ifndef _TopLoc_SListOfItemLocation_HeaderFile
#define _TopLoc_SListOfItemLocation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Macro.hxx>
class TopLoc_SListNodeOfItemLocation;
class TopLoc_ItemLocation;


//! An SListOfItemLocation is a LISP like list of Items.
//! An SListOfItemLocation is :
//! . Empty.
//! . Or it has a Value and a  Tail  which is an other SListOfItemLocation.
//!
//! The Tail of an empty list is an empty list.
//! SListOfItemLocation are  shared.  It  means   that they  can  be
//! modified through other lists.
//! SListOfItemLocation may  be used  as Iterators. They  have Next,
//! More, and value methods. To iterate on the content
//! of the list S just do.
//!
//! SListOfItemLocation Iterator;
//! for (Iterator = S; Iterator.More(); Iterator.Next())
//! X = Iterator.Value();
class TopLoc_SListOfItemLocation 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates an empty List.
  TopLoc_SListOfItemLocation() {}
  
  //! Creates a List with <anItem> as value  and <aTail> as tail.
  Standard_EXPORT TopLoc_SListOfItemLocation(const TopLoc_ItemLocation& anItem, const TopLoc_SListOfItemLocation& aTail);
  
  //! Creates a list from an other one. The lists  are shared.
  TopLoc_SListOfItemLocation(const TopLoc_SListOfItemLocation& Other)
  : myNode(Other.myNode)
  {
  }
  
  //! Sets  a list  from  an  other  one. The  lists are
  //! shared. The list itself is returned.
  Standard_EXPORT TopLoc_SListOfItemLocation& Assign (const TopLoc_SListOfItemLocation& Other);

  //! Assignment
  TopLoc_SListOfItemLocation& operator = (const TopLoc_SListOfItemLocation& Other)
  {
    return Assign(Other);
  }
  
  //! Move constructor
  TopLoc_SListOfItemLocation (TopLoc_SListOfItemLocation&& theOther) Standard_Noexcept
    : myNode(std::move (theOther.myNode))
  {
  }

  //! Move operator
  TopLoc_SListOfItemLocation& operator= (TopLoc_SListOfItemLocation&& theOther) Standard_Noexcept
  {
    myNode = std::move (theOther.myNode);
    return *this;
  }

  //! Returne true if this list is empty
  Standard_Boolean IsEmpty() const
  {
    return myNode.IsNull();
  }
  
  //! Sets the list to be empty.
  void Clear()
  {
    myNode.Nullify();
  }

  //! Destructor
  ~TopLoc_SListOfItemLocation()
  {
    Clear();
  }
  
  //! Returns the current value of the list. An error is
  //! raised  if the list is empty.
  Standard_EXPORT const TopLoc_ItemLocation& Value() const;
  
  //! Returns the current tail of  the list. On an empty
  //! list the tail is the list itself.
  Standard_EXPORT const TopLoc_SListOfItemLocation& Tail() const;
  
  //! Replaces the list by a list with <anItem> as Value
  //! and the  list <me> as  tail.
  void Construct(const TopLoc_ItemLocation& anItem)
  {
    Assign(TopLoc_SListOfItemLocation(anItem, *this));
  }
  
  //! Replaces the list <me> by its tail.
  void ToTail()
  {
    Assign(Tail());
  }
  
  //! Returns True if the iterator  has a current value.
  //! This is !IsEmpty()
  Standard_Boolean More() const
  {
    return !IsEmpty();
  }
  
  //! Moves the iterator to the next object in the list.
  //! If the iterator is empty it will  stay empty. This is ToTail()
  void Next()
  {
    ToTail();
  }

private:
  Handle(TopLoc_SListNodeOfItemLocation) myNode;
};

#endif // _TopLoc_SListOfItemLocation_HeaderFile
