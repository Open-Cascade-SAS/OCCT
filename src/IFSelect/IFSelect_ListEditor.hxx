// Created on: 1998-07-28
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _IFSelect_ListEditor_HeaderFile
#define _IFSelect_ListEditor_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
class Interface_TypedValue;
class Interface_InterfaceModel;
class TCollection_HAsciiString;


class IFSelect_ListEditor;
DEFINE_STANDARD_HANDLE(IFSelect_ListEditor, Standard_Transient)

//! A ListEditor is an auxiliary operator for Editor/EditForm
//! I.E. it works on parameter values expressed as strings
//!
//! For a parameter which is a list, it may not be edited in once
//! by just setting a new value (as a string)
//!
//! Firstly, a list can be long (and tedious to be accessed flat)
//! then requires a better way of accessing
//!
//! Moreover, not only its VALUES may be changed (SetValue), but
//! also its LENGTH : items may be added or removed ...
//!
//! Hence, the way of editing a parameter as a list is :
//! - edit it separately, with the help of a ListEditor
//! - it remains possible to prepare a new list of values apart
//! - then give the new list in once to the EditForm
//!
//! An EditList is produced by the Editor, with a basic definition
//! This definition (brought by this class) can be redefined
//! Hence the Editor may produce a specific ListEditor as needed
class IFSelect_ListEditor : public Standard_Transient
{

public:

  
  //! Creates a ListEditor with absolutely no constraint
  Standard_EXPORT IFSelect_ListEditor();
  
  //! Creates a ListEditor, for which items of the list to edit are
  //! defined by <def>, and <max> describes max length :
  //! 0 (D) means no limit
  //! value > 0 means : no more the <max> items are allowed
  Standard_EXPORT IFSelect_ListEditor(const Handle(Interface_TypedValue)& def, const Standard_Integer max = 0);
  
  //! Loads a Model. It is used to check items of type Entity(Ident)
  Standard_EXPORT void LoadModel (const Handle(Interface_InterfaceModel)& model);
  
  //! Loads the original values for the list
  //! Remark : If its length is mor then MaxLength, editions remain
  //! allowed, except Add
  Standard_EXPORT void LoadValues (const Handle(TColStd_HSequenceOfHAsciiString)& vals);
  
  //! Declares this ListEditor to have been touched (whatever action)
  Standard_EXPORT void SetTouched();
  
  //! Clears all editions already recorded
  Standard_EXPORT void ClearEdit();
  
  //! Loads a new list to replace the older one, in once !
  //! By default (can be redefined) checks the length of the list
  //! and the value of each item according to the def
  //! Items are all recorded as Modified
  //!
  //! If no def has been given at creation time, no check is done
  //! Returns True when done, False if checks have failed ... a
  //! specialisation may also lock it by returning always False ...
  Standard_EXPORT virtual Standard_Boolean LoadEdited (const Handle(TColStd_HSequenceOfHAsciiString)& list);
  
  //! Sets a new value for the item <num> (in edited list)
  //! <val> may be a Null Handle, then the value will be cleared but
  //! not removed
  //! Returns True when done. False if <num> is out of range or if
  //! <val> does not satisfy the definition
  Standard_EXPORT virtual Standard_Boolean SetValue (const Standard_Integer num, const Handle(TCollection_HAsciiString)& val);
  
  //! Adds a new item. By default appends (at the end of the list)
  //! Can insert before a given rank <num>, if positive
  //! Returns True when done. False if MaxLength may be overpassed
  //! or if <val> does not satisfy the definition
  Standard_EXPORT virtual Standard_Boolean AddValue (const Handle(TCollection_HAsciiString)& val, const Standard_Integer atnum = 0);
  
  //! Removes items from the list
  //! By default removes one item. Else, count given by <howmany>
  //! Remove from rank <num> included. By default, from the end
  //! Returns True when done, False (and does not work) if case of
  //! out of range of if <howmany> is greater than current length
  Standard_EXPORT virtual Standard_Boolean Remove (const Standard_Integer num = 0, const Standard_Integer howmany = 1);
  
  //! Returns the value from which the edition started
  Standard_EXPORT Handle(TColStd_HSequenceOfHAsciiString) OriginalValues() const;
  
  //! Returns the result of the edition
  Standard_EXPORT Handle(TColStd_HSequenceOfHAsciiString) EditedValues() const;
  
  //! Returns count of values, edited (D) or original
  Standard_EXPORT Standard_Integer NbValues (const Standard_Boolean edited = Standard_True) const;
  
  //! Returns a value given its rank. Edited (D) or Original
  //! A Null String means the value is cleared but not removed
  Standard_EXPORT Handle(TCollection_HAsciiString) Value (const Standard_Integer num, const Standard_Boolean edited = Standard_True) const;
  
  //! Tells if a value (in edited list) has been changed, i.e.
  //! either modified-value, or added
  Standard_EXPORT Standard_Boolean IsChanged (const Standard_Integer num) const;
  
  //! Tells if a value (in edited list) has been modified-value
  //! (not added)
  Standard_EXPORT Standard_Boolean IsModified (const Standard_Integer num) const;
  
  //! Tells if a value (in edited list) has been added (new one)
  Standard_EXPORT Standard_Boolean IsAdded (const Standard_Integer num) const;
  
  //! Tells if at least one edition (SetValue-AddValue-Remove) has
  //! been recorded
  Standard_EXPORT Standard_Boolean IsTouched() const;




  DEFINE_STANDARD_RTTIEXT(IFSelect_ListEditor,Standard_Transient)

protected:




private:


  Standard_Integer themax;
  Handle(Interface_TypedValue) thedef;
  Standard_Integer thetouc;
  Handle(TColStd_HSequenceOfHAsciiString) theorig;
  Handle(TColStd_HSequenceOfHAsciiString) theedit;
  Handle(TColStd_HSequenceOfInteger) thestat;
  Handle(Interface_InterfaceModel) themodl;


};







#endif // _IFSelect_ListEditor_HeaderFile
