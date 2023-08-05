// Created on: 1995-12-08
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Interface_Static_HeaderFile
#define _Interface_Static_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Interface_StaticSatisfies.hxx>
#include <Interface_TypedValue.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
class TCollection_HAsciiString;

class Interface_Static;
DEFINE_STANDARD_HANDLE(Interface_Static, Interface_TypedValue)

//! This class gives a way to manage meaningful static variables,
//! used as "global" parameters in various procedures.
//!
//! A Static brings a specification (its type, constraints if any)
//! and a value. Its basic form is a string, it can be specified
//! as integer or real or enumerative string, and queried as such.
//! Its string content, which is a Handle(HAsciiString) can be
//! shared by other data structures, hence gives a direct on line
//! access to its value.
//!
//! All this description is inherited from TypedValue
//!
//! A Static can be given an initial value, it can be filled from,
//! either a set of Resources (an applicative feature which
//! accesses and manages parameter files), or environment or
//! internal definition : these define families of Static.
//! In addition, it supports a status for reinitialisation : an
//! initialisation procedure can ask if the value of the Static
//! has changed from its last call, in this case does something
//! then marks the Status "uptodate", else it does nothing.
//!
//! Statics are named and recorded then accessed in an alphabetic
//! dictionary
class Interface_Static : public Interface_TypedValue
{

public:

  
  //! Creates and records a Static, with a family and a name
  //! family can report to a name of resource or to a system or
  //! internal definition. The name must be unique.
  //!
  //! type gives the type of the parameter, default is free text
  //! Also available : Integer, Real, Enum, Entity (i.e. Object)
  //! More precise specifications, titles, can be given to the
  //! Static once created
  //!
  //! init gives an initial value. If it is not given, the Static
  //! begin as "not set", its value is empty
  Standard_EXPORT Interface_Static(const Standard_CString family, const Standard_CString name, const Interface_ParamType type = Interface_ParamText, const Standard_CString init = "");
  
  //! Creates a new Static with same definition as another one
  //! (value is copied, except for Entity : it remains null)
  Standard_EXPORT Interface_Static(const Standard_CString family, const Standard_CString name, const Handle(Interface_Static)& other);
  
  //! Writes the properties of a
  //! parameter in the diagnostic file. These include:
  //! - Name
  //! - Family,
  //! - Wildcard (if it has one)
  //! - Current status (empty  string if it was updated or
  //! if it is the original one)
  //! - Value
  Standard_EXPORT void PrintStatic (Standard_OStream& S) const;
  
  //! Returns the family. It can be : a resource name for applis,
  //! an internal name between : $e (environment variables),
  //! $l (other, purely local)
  Standard_EXPORT Standard_CString Family() const;
  
  //! Sets a "wild-card" static : its value will be considered
  //! if <me> is not properly set. (reset by set a null one)
  Standard_EXPORT void SetWild (const Handle(Interface_Static)& wildcard);
  
  //! Returns the wildcard static, which can be (is most often) null
  Standard_EXPORT Handle(Interface_Static) Wild() const;
  
  //! Records a Static has "uptodate", i.e. its value has been taken
  //! into account by a reinitialisation procedure
  //! This flag is reset at each successful SetValue
  Standard_EXPORT void SetUptodate();
  
  //! Returns the status "uptodate"
  Standard_EXPORT Standard_Boolean UpdatedStatus() const;
  
  //! Declares a new Static (by calling its constructor)
  //! If this name is already taken, does nothing and returns False
  //! Else, creates it and returns True
  //! For additional definitions, get the Static then edit it
  Standard_EXPORT static Standard_Boolean Init (const Standard_CString family, const Standard_CString name, const Interface_ParamType type, const Standard_CString init = "");
  
  //! As Init with ParamType, but type is given as a character
  //! This allows a simpler call
  //! Types : 'i' Integer, 'r' Real, 't' Text, 'e' Enum, 'o' Object
  //! '=' for same definition as, <init> gives the initial Static
  //! Returns False if <type> does not match this list
  Standard_EXPORT static Standard_Boolean Init (const Standard_CString family, const Standard_CString name, const Standard_Character type, const Standard_CString init = "");
  
  //! Returns a Static from its name. Null Handle if not present
  Standard_EXPORT static Handle(Interface_Static) Static (const Standard_CString name);
  
  //! Returns True if a Static named <name> is present, False else
  Standard_EXPORT static Standard_Boolean IsPresent (const Standard_CString name);
  
  //! Returns a part of the definition of a Static, as a CString
  //! The part is designated by its name, as a CString
  //! If the required value is not a string, it is converted to a
  //! CString then returned
  //! If <name> is not present, or <part> not defined for <name>,
  //! this function returns an empty string
  //!
  //! Allowed parts for CDef :
  //! family : the family
  //! type  : the type ("integer","real","text","enum")
  //! label : the label
  //! satis : satisfy function name if any
  //! rmin : minimum real value
  //! rmax : maximum real value
  //! imin : minimum integer value
  //! imax : maximum integer value
  //! enum nn (nn : value of an integer) : enum value for nn
  //! unit : unit definition for a real
  Standard_EXPORT static Standard_CString CDef (const Standard_CString name, const Standard_CString part);
  
  //! Returns a part of the definition of a Static, as an Integer
  //! The part is designated by its name, as a CString
  //! If the required value is not a string, returns zero
  //! For a Boolean, 0 for false, 1 for true
  //! If <name> is not present, or <part> not defined for <name>,
  //! this function returns zero
  //!
  //! Allowed parts for IDef :
  //! imin, imax : minimum or maximum integer value
  //! estart : starting number for enum
  //! ecount : count of enum values (starting from estart)
  //! ematch : exact match status
  //! eval val : case determined from a string
  Standard_EXPORT static Standard_Integer IDef (const Standard_CString name, const Standard_CString part);
  
  //! Returns True if <name> is present AND set
  //! <proper> True (D) : considers this item only
  //! <proper> False    : if not set and attached to a wild-card,
  //! considers this wild-card
  Standard_EXPORT static Standard_Boolean IsSet (const Standard_CString name, const Standard_Boolean proper = Standard_True);
  
  //! Returns the value of the
  //! parameter identified by the string name.
  //! If the specified parameter does not exist, an empty
  //! string is returned.
  //! Example
  //! Interface_Static::CVal("write.step.schema");
  //! which could return:
  //! "AP214"
  Standard_EXPORT static Standard_CString CVal (const Standard_CString name);
  
  //! Returns the integer value of
  //! the translation parameter identified by the string name.
  //! Returns the value 0 if the parameter does not exist.
  //! Example
  //! Interface_Static::IVal("write.step.schema");
  //! which could return: 3
  Standard_EXPORT static Standard_Integer IVal (const Standard_CString name);
  
  //! Returns the value of a static
  //! translation parameter identified by the string name.
  //! Returns the value 0.0 if the parameter does not exist.
  Standard_EXPORT static Standard_Real RVal (const Standard_CString name);
  
  //! Modifies the value of the
  //! parameter identified by name. The modification is specified
  //! by the string val. false is returned if the parameter does not exist.
  //! Example
  //! Interface_Static::SetCVal
  //! ("write.step.schema","AP203")
  //! This syntax specifies a switch from the default STEP 214 mode to STEP 203 mode.
  Standard_EXPORT static Standard_Boolean SetCVal (const Standard_CString name, const Standard_CString val);
  
  //! Modifies the value of the
  //! parameter identified by name. The modification is specified
  //! by the integer value val. false is returned if the
  //! parameter does not exist.
  //! Example
  //! Interface_Static::SetIVal
  //! ("write.step.schema", 3)
  //! This syntax specifies a switch from the default STEP 214 mode to STEP 203 mode.S
  Standard_EXPORT static Standard_Boolean SetIVal (const Standard_CString name, const Standard_Integer val);
  
  //! Modifies the value of a
  //! translation parameter. false is returned if the
  //! parameter does not exist. The modification is specified
  //! by the real number value val.
  Standard_EXPORT static Standard_Boolean SetRVal (const Standard_CString name, const Standard_Real val);
  
  //! Sets a Static to be "uptodate"
  //! Returns False if <name> is not present
  //! This status can be used by a reinitialisation procedure to
  //! rerun if a value has been changed
  Standard_EXPORT static Standard_Boolean Update (const Standard_CString name);
  
  //! Returns the status "uptodate" from a Static
  //! Returns False if <name> is not present
  Standard_EXPORT static Standard_Boolean IsUpdated (const Standard_CString name);
  
  //! Returns a list of names of statics :
  //! <mode> = 0 (D) : criter is for family
  //! <mode> = 1 : criter is regexp on names, takes final items
  //! (ignore wild cards)
  //! <mode> = 2 : idem but take only wilded, not final items
  //! <mode> = 3 : idem, take all items matching criter
  //! idem + 100 : takes only non-updated items
  //! idem + 200 : takes only updated items
  //! criter empty (D) : returns all names
  //! else returns names which match the given criter
  //! Remark : families beginning by '$' are not listed by criter ""
  //! they are listed only by criter "$"
  //!
  //! This allows for instance to set new values after having loaded
  //! or reloaded a resource, then to update them as required
  Standard_EXPORT static Handle(TColStd_HSequenceOfHAsciiString) Items (const Standard_Integer mode = 0, const Standard_CString criter = "");
  
  //! Initializes all standard static parameters, which can be used
  //! by every function. statics specific of a norm or a function
  //! must be defined around it
  Standard_EXPORT static void Standards();

  //! Fills given string-to-string map with all static data
  Standard_EXPORT static void FillMap(NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>& theMap);

  DEFINE_STANDARD_RTTIEXT(Interface_Static,Interface_TypedValue)

protected:




private:


  TCollection_AsciiString thefamily;
  TCollection_AsciiString thename;
  TCollection_AsciiString thelabel;
  Interface_ParamType thetype;
  Handle(Standard_Type) theotyp;
  Handle(Interface_Static) thewild;
  Standard_Integer thelims;
  Standard_Integer theintlow;
  Standard_Integer theintup;
  Standard_Real therealow;
  Standard_Real therealup;
  TCollection_AsciiString theunidef;
  Handle(TColStd_HArray1OfAsciiString) theenums;
  NCollection_DataMap<TCollection_AsciiString, Standard_Integer> theeadds;
  Interface_StaticSatisfies thesatisf;
  TCollection_AsciiString thesatisn;
  Standard_Boolean theupdate;
  Standard_Integer theival;
  Handle(TCollection_HAsciiString) thehval;
  Handle(Standard_Transient) theoval;


};







#endif // _Interface_Static_HeaderFile
