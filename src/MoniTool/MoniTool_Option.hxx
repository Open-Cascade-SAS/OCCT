// Created on: 1998-12-14
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

#ifndef _MoniTool_Option_HeaderFile
#define _MoniTool_Option_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Type.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
class MoniTool_TypedValue;
class Dico_DictionaryOfTransient;
class Standard_Transient;
class TCollection_AsciiString;


class MoniTool_Option;
DEFINE_STANDARD_HANDLE(MoniTool_Option, MMgt_TShared)

//! An Option gives a way of recording an enumerated list of
//! instances of a given class, each instance being identified
//! by a case name.
//!
//! Also, an Option allows to manage basic types through a Typed
//! Value (which also applies to Static Parameter). It may record
//! an enumerated list of values for a TypedValue or Static
//! Parameter, each of them is recorded as a string (HAsciiString)
//!
//! An Option is defined by the type of the class to be optioned,
//! or (mutually exclusive) the TypedValue/Static of which values
//! are to be optioned, a specific name, a list of named values.
//! It brings a current case with its name and value
//! It may also have a default case (the first recorded one if not
//! precised)
//!
//! An Option may be created from another one, by sharing its Type
//! and its list of Items (one per case), with the same name or
//! another one. It may then be duplicated to break this sharing.
class MoniTool_Option : public MMgt_TShared
{

public:

  
  //! Creates an Option from scratch, with a Type and a Name
  Standard_EXPORT MoniTool_Option(const Handle(Standard_Type)& atype, const Standard_CString aname);
  
  //! Creates an Option for a TypedValue (for basic, non-cdl-typed,
  //! value : integer, real, string ...)
  //! If <name> is not given, the name of the TypedValue is taken
  //! Remark that Type is then enforced to TCollection_HAsciiString
  Standard_EXPORT MoniTool_Option(const Handle(MoniTool_TypedValue)& aval, const Standard_CString aname = "");
  
  //! Creates an Option from another one, the name can be redefined
  //! The Type remains the same. The list of Items, too, it can also
  //! be later duplicated by call to Duplicate
  Standard_EXPORT MoniTool_Option(const Handle(MoniTool_Option)& other, const Standard_CString aname = "");
  
  //! Adds an item : value and name (replaces it if name is already
  //! recorded)
  //! Returns True when done, False if <val> is not Kind of the
  //! definition Type
  //! For a TypedValue, val must be a HAsciiString, its content must
  //! satisfy the definition of the TypedValue
  Standard_EXPORT Standard_Boolean Add (const Standard_CString name, const Handle(Standard_Transient)& val);
  
  //! Short-cut to add an item for a TypedValue (basic type) : name
  //! is the name of the case, val is its value as a CString
  //! If val is not provided, val = name is assumed
  //! Returns True when done, False if this Option is not for a
  //! TypedValue or if the new value does not satisfy the definition
  //! of the TypedValue
  Standard_EXPORT Standard_Boolean AddBasic (const Standard_CString name, const Standard_CString val = "");
  
  //! Duplicates the list of items
  //! It starts with the same definitions as before Duplicate, but
  //! it is not longer shared with other options
  Standard_EXPORT void Duplicate();
  
  //! Returns the Name of the Option
  Standard_EXPORT const TCollection_AsciiString& Name() const;
  
  //! Returns the Type of the Option
  Standard_EXPORT Handle(Standard_Type) Type() const;
  
  //! Returns the TypedValue of the Option, or a Null Handle
  Standard_EXPORT Handle(MoniTool_TypedValue) TypedValue() const;
  
  //! Gives the value bound with a name, in val
  //! Returns True if <name> is found, False else
  //! This way of returning a Transient, bound with the Type Control
  //! avoids DownCast and ensures the value is directly usable
  Standard_EXPORT Standard_Boolean Item (const Standard_CString name, Handle(Standard_Transient)& val) const;
  
  //! Returns the list of available item names
  Standard_EXPORT Handle(TColStd_HSequenceOfAsciiString) ItemList() const;
  
  //! Returns the list of cases, other than <name>, which bring the
  //! same value as <name>
  //! Empty list (not a Null Handle) if no alias, or <name> unknown
  //! if <exact> is True (D), exact name is required, no completion
  //! if <exact> is False and <name> is not complete, but addresses
  //! only one item, completion is done and the list includes the
  //! complete name
  Standard_EXPORT Handle(TColStd_HSequenceOfAsciiString) Aliases (const Standard_CString name, const Standard_Boolean exact = Standard_True) const;
  
  //! Commands the Option to switch on an item name
  //! Returns True when done, False if <name> is not recorded
  //! (in that case, former switch remains unchanged)
  //! If no switch has been called, it is active on the last added
  //! items
  Standard_EXPORT Standard_Boolean Switch (const Standard_CString name);
  
  //! Returns the Name of the currently switched item (Case)
  Standard_EXPORT const TCollection_AsciiString& CaseName() const;
  
  //! Returns the Value of the currently switch item
  //! To be down-casted as needed before use
  Standard_EXPORT Handle(Standard_Transient) CaseValue() const;
  
  //! Returns the Value of the currently switch item
  //! This way of returning a Transient, bound with the Type Control
  //! avoids DownCast and ensures the value is directly usable
  //! For a TypedValue, returns the corresponding HAsciiString
  Standard_EXPORT void Value (Handle(Standard_Transient)& val) const;




  DEFINE_STANDARD_RTTI(MoniTool_Option,MMgt_TShared)

protected:




private:

  
  //! Returns the list of items, to be shared (to copy an option)
  Standard_EXPORT Handle(Dico_DictionaryOfTransient) Items() const;

  TCollection_AsciiString thename;
  Handle(Standard_Type) thetype;
  Handle(MoniTool_TypedValue) thevalue;
  Handle(Dico_DictionaryOfTransient) theitems;
  TCollection_AsciiString thecase;
  Handle(Standard_Transient) theval;


};







#endif // _MoniTool_Option_HeaderFile
