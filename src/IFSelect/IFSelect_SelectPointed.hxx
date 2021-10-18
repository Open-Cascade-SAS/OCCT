// Created on: 1994-05-30
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IFSelect_SelectPointed_HeaderFile
#define _IFSelect_SelectPointed_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <IFSelect_SelectBase.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Interface_CopyControl;
class IFSelect_Transformer;
class Interface_EntityIterator;
class Interface_Graph;
class TCollection_AsciiString;


class IFSelect_SelectPointed;
DEFINE_STANDARD_HANDLE(IFSelect_SelectPointed, IFSelect_SelectBase)

//! This type of Selection is intended to describe a direct
//! selection without an explicit criterium, for instance the
//! result of picking viewed entities on a graphic screen
//!
//! It can also be used to provide a list as internal alternate
//! input : this use implies to clear the list once queried
class IFSelect_SelectPointed : public IFSelect_SelectBase
{

public:

  
  //! Creates a SelectPointed
  Standard_EXPORT IFSelect_SelectPointed();
  
  //! Clears the list of selected items
  //! Also says the list is unset
  //! All Add* methods and SetList say the list is set
  Standard_EXPORT void Clear();
  
  //! Tells if the list has been set. Even if empty
  Standard_EXPORT Standard_Boolean IsSet() const;
  
  //! As SetList but with only one entity
  //! If <ent> is Null, the list is said as being set but is empty
  Standard_EXPORT void SetEntity (const Handle(Standard_Transient)& item);
  
  //! Sets a given list to define the list of selected items
  //! <list> can be empty or null : in this case, the list is said
  //! as being set, but it is empty
  //!
  //! To use it as an alternate input, one shot :
  //! - SetList or SetEntity to define the input list
  //! - RootResult to get it
  //! - then Clear to drop it
  Standard_EXPORT void SetList (const Handle(TColStd_HSequenceOfTransient)& list);
  
  //! Adds an item. Returns True if Done, False if <item> is already
  //! in the selected list
  Standard_EXPORT Standard_Boolean Add (const Handle(Standard_Transient)& item);
  
  //! Removes an item. Returns True if Done, False if <item> was not
  //! in the selected list
  Standard_EXPORT Standard_Boolean Remove (const Handle(Standard_Transient)& item);
  
  //! Toggles status of an item : adds it if not pointed or removes
  //! it if already pointed. Returns the new status (Pointed or not)
  Standard_EXPORT Standard_Boolean Toggle (const Handle(Standard_Transient)& item);
  
  //! Adds all the items defined in a list. Returns True if at least
  //! one item has been added, False else
  Standard_EXPORT Standard_Boolean AddList (const Handle(TColStd_HSequenceOfTransient)& list);
  
  //! Removes all the items defined in a list. Returns True if at
  //! least one item has been removed, False else
  Standard_EXPORT Standard_Boolean RemoveList (const Handle(TColStd_HSequenceOfTransient)& list);
  
  //! Toggles status of all the items defined in a list : adds it if
  //! not pointed or removes it if already pointed.
  Standard_EXPORT Standard_Boolean ToggleList (const Handle(TColStd_HSequenceOfTransient)& list);
  
  //! Returns the rank of an item in the selected list, or 0.
  Standard_EXPORT Standard_Integer Rank (const Handle(Standard_Transient)& item) const;
  
  //! Returns the count of selected items
  Standard_EXPORT Standard_Integer NbItems() const;
  
  //! Returns an item given its rank, or a Null Handle
  Standard_EXPORT Handle(Standard_Transient) Item (const Standard_Integer num) const;
  
  //! Rebuilds the selected list. Any selected entity which has a
  //! bound result is replaced by this result, else it is removed.
  Standard_EXPORT void Update (const Handle(Interface_CopyControl)& control);
  
  //! Rebuilds the selected list, by querying a Transformer
  //! (same principle as from a CopyControl)
  Standard_EXPORT void Update (const Handle(IFSelect_Transformer)& trf);
  
  //! Returns the list of selected items. Only the selected entities
  //! which are present in the graph are given (this result assures
  //! uniqueness).
  Standard_EXPORT Interface_EntityIterator RootResult (const Interface_Graph& G) const Standard_OVERRIDE;
  
  //! Returns a text which identifies the type of selection made.
  //! It is "Pointed Entities"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectPointed,IFSelect_SelectBase)

protected:




private:


  Standard_Boolean theset;
  TColStd_SequenceOfTransient theitems;


};







#endif // _IFSelect_SelectPointed_HeaderFile
