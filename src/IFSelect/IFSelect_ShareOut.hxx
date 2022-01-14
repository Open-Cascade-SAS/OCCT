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

#ifndef _IFSelect_ShareOut_HeaderFile
#define _IFSelect_ShareOut_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_TSeqOfDispatch.hxx>
#include <IFSelect_SequenceOfGeneralModifier.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class IFSelect_Dispatch;
class IFSelect_GeneralModifier;
class IFSelect_Modifier;
class TCollection_AsciiString;

class IFSelect_ShareOut;
DEFINE_STANDARD_HANDLE(IFSelect_ShareOut, Standard_Transient)

//! This class gathers the information required to produce one or
//! several file(s) from the content of an InterfaceModel (passing
//! through the creation of intermediate Models).
//!
//! It can correspond to a complete Divide up of a set of Entities
//! intended to be exhaustive and to limit duplications. Or to a
//! simple Extraction of some Entities, in order to work on them.
//!
//! A ShareOut is composed of a list of Dispatches.
//! To Each Dispatch in the ShareOut, is bound an Id. Number
//! This Id. Number allows to identify a Display inside the
//! ShareOut in a stable way (for instance, to attach file names)
//!
//! ShareOut can be seen as a "passive" description, activated
//! through a ShareOutResult, which gives the InterfaceModel on
//! which to work, as a unique source. Thus it is easy to change
//! it without coherence problems
//!
//! Services about it are provided by the class ShareOutResult
//! which is a service class : simulation (list of files and of
//! entities per file; "forgotten" entities; duplicated entities),
//! exploitation (generation of derivated Models, each of them
//! generating an output file)
class IFSelect_ShareOut : public Standard_Transient
{

public:

  //! Creates an empty ShareOut
  Standard_EXPORT IFSelect_ShareOut();
  
  //! Removes in one operation all the Dispatches with their Idents
  //! Also clears all information about Names, and all Results but
  //! naming information which are :
  //! - kept if <onlydisp> is True.
  //! - cleared if <onlydisp> is False (complete clearing)
  //! If <onlydisp> is True, that's all. Else, clears also Modifiers
  Standard_EXPORT void Clear (const Standard_Boolean onlydisp);
  
  //! Clears all data produced (apart from Dispatches, etc...)
  //! if <alsoname> is True, all is cleared. Else, information
  //! about produced Names are kept (to maintain unicity of naming
  //! across clearings)
  Standard_EXPORT void ClearResult (const Standard_Boolean alsoname);
  
  //! Removes an item, which can be, either a Dispatch (removed from
  //! the list of Dispatches), or a GeneralModifier (removed from
  //! the list of Model Modifiers or from the list of File Modifiers
  //! according to its type).
  //! Returns True if done, False if has not been found or if it is
  //! neither a Dispatch, nor a Modifier.
  Standard_EXPORT Standard_Boolean RemoveItem (const Handle(Standard_Transient)& item);
  
  //! Returns the rank of last run item (ClearResult resets it to 0)
  Standard_EXPORT Standard_Integer LastRun() const;

  //! Records a new value for the rank of last run item
  Standard_EXPORT void SetLastRun (const Standard_Integer last);

  //! Returns the count of Dispatches
  Standard_EXPORT Standard_Integer NbDispatches() const;
  
  //! Returns the Rank of a Dispatch, given its Value (Handle).
  //! Returns 0 if the Dispatch is unknown in the ShareOut
  Standard_EXPORT Standard_Integer DispatchRank (const Handle(IFSelect_Dispatch)& disp) const;
  
  //! Returns a Dispatch, given its rank in the list
  Standard_EXPORT const Handle(IFSelect_Dispatch)& Dispatch (const Standard_Integer num) const;
  
  //! Adds a Dispatch to the list
  Standard_EXPORT void AddDispatch (const Handle(IFSelect_Dispatch)& disp);
  
  //! Removes a Dispatch, given its rank in the list
  //! Returns True if done, False if rank is not between
  //! (LastRun + 1) and (NbDispatches)
  Standard_EXPORT Standard_Boolean RemoveDispatch (const Standard_Integer rank);
  
  //! Sets a Modifier to be applied on all Dispatches to be run
  //! If <modifier> is a ModelModifier, adds it to the list of
  //! Model Modifiers; else to the list of File Modifiers
  //! By default (atnum = 0) at the end of the list, else at <atnum>
  //! Each Modifier is used, after each copy of a packet of Entities
  //! into a Model : its criteria are checked and if they are OK,
  //! the method Perform of this Modifier is run.
  Standard_EXPORT void AddModifier (const Handle(IFSelect_GeneralModifier)& modifier, const Standard_Integer atnum);
  
  //! Sets a Modifier to be applied on the Dispatch <dispnum>
  //! If <modifier> is a ModelModifier, adds it to the list of
  //! Model Modifiers; else to the list of File Modifiers
  //! This is the same list as for all Dispatches, but the
  //! Modifier is qualified to be applied to one Dispatch only
  //! Then, <atnum> refers to the entire list
  //! By default (atnum = 0) at the end of the list, else at <atnum>
  //! Remark : if the Modifier was already in the list and if
  //! <atnum> = 0, the Modifier is not moved, but only qualified
  //! for a Dispatch
  Standard_EXPORT void AddModifier (const Handle(IFSelect_GeneralModifier)& modifier, const Standard_Integer dispnum, const Standard_Integer atnum);
  
  //! Adds a Modifier to the list of Modifiers : Model Modifiers if
  //! <formodel> is True, File Modifiers else (internal).
  Standard_EXPORT void AddModif (const Handle(IFSelect_GeneralModifier)& modifier, const Standard_Boolean formodel, const Standard_Integer atnum = 0);
  
  //! Returns count of Modifiers (which apply to complete Models) :
  //! Model Modifiers if <formodel> is True, File Modifiers else
  Standard_EXPORT Standard_Integer NbModifiers (const Standard_Boolean formodel) const;
  
  //! Returns a Modifier of the list, given its rank :
  //! Model Modifiers if <formodel> is True, File Modifiers else
  Standard_EXPORT Handle(IFSelect_GeneralModifier) GeneralModifier (const Standard_Boolean formodel, const Standard_Integer num) const;
  
  //! Returns a Modifier of the list of Model Modifiers, duely casted
  Standard_EXPORT Handle(IFSelect_Modifier) ModelModifier (const Standard_Integer num) const;
  
  //! Gives the rank of a Modifier in the list, 0 if not in the list
  //! Model Modifiers if <modifier> is kind of ModelModifer,
  //! File Modifiers else
  Standard_EXPORT Standard_Integer ModifierRank (const Handle(IFSelect_GeneralModifier)& modifier) const;
  
  //! Removes a Modifier, given it rank in the list :
  //! Model Modifiers if <formodel> is True, File Modifiers else
  //! Returns True if done, False if <num> is out of range
  Standard_EXPORT Standard_Boolean RemoveModifier (const Standard_Boolean formodel, const Standard_Integer num);
  
  //! Changes the rank of a modifier in the list :
  //! Model Modifiers if <formodel> is True, File Modifiers else
  //! from <before> to <after>
  //! Returns True if done, False else (before or after out of range)
  Standard_EXPORT Standard_Boolean ChangeModifierRank (const Standard_Boolean formodel, const Standard_Integer befor, const Standard_Integer after);
  
  //! Attaches a Root Name to a Dispatch given its rank, as an
  //! HAsciiString (standard form). A Null Handle resets this name.
  //! Returns True if OK, False if this Name is already attached,
  //! for a Dispatch or for Default, or <num> out of range
  Standard_EXPORT Standard_Boolean SetRootName (const Standard_Integer num, const Handle(TCollection_HAsciiString)& name);
  
  //! Returns True if the Dispatch of rank <num> has an attached
  //! Root Name. False else, or if num is out of range
  Standard_EXPORT Standard_Boolean HasRootName (const Standard_Integer num) const;
  
  //! Returns the Root bound to a Dispatch, given its rank
  //! Returns a Null Handle if not defined
  Standard_EXPORT Handle(TCollection_HAsciiString) RootName (const Standard_Integer num) const;
  
  //! Returns an integer value about a given root name :
  //! - positive : it's the rank of the Dispatch which has this name
  //! - null : this root name is unknown
  //! - negative (-1) : this root name is the default root name
  Standard_EXPORT Standard_Integer RootNumber (const Handle(TCollection_HAsciiString)& name) const;
  
  //! Defines or Changes the general Prefix (which is prepended to
  //! complete file name generated). If this method is not call,
  //! Prefix remains empty
  Standard_EXPORT void SetPrefix (const Handle(TCollection_HAsciiString)& pref);
  
  //! Defines or Changes the Default Root Name to a new value (which
  //! is used for dispatches which have no attached root name).
  //! If this method is not called, DefaultRootName remains empty
  //! Returns True if OK, False if this Name is already attached,
  //! for a Dispatch or for Default
  Standard_EXPORT Standard_Boolean SetDefaultRootName (const Handle(TCollection_HAsciiString)& defrt);
  
  //! Defines or Changes the general Extension (which is appended to
  //! complete file name generated). If this method is not call,
  //! Extension remains empty
  Standard_EXPORT void SetExtension (const Handle(TCollection_HAsciiString)& ext);
  
  //! Returns the general Prefix. Can be empty.
  Standard_EXPORT Handle(TCollection_HAsciiString) Prefix() const;
  
  //! Returns the Default Root Name. Can be empty.
  Standard_EXPORT Handle(TCollection_HAsciiString) DefaultRootName() const;
  
  //! Returns the general Extension. Can be empty (not recommended)
  Standard_EXPORT Handle(TCollection_HAsciiString) Extension() const;
  
  //! Computes the complete file name for a Packet of a Dispatch,
  //! given Dispatch Number (Rank), Packet Number, and Count of
  //! Packets generated by this Dispatch (0 if unknown)
  //!
  //! File Name is made of following strings, concatenated :
  //! General Prefix, Root Name for Dispatch, Packet Suffix, and
  //! General Extension. If no Root Name is specified for a
  //! Dispatch, DefaultRootName is considered (and pnum is not used,
  //! but <thenbdefs> is incremented and used
  //! Error if no Root is defined for this <idnum>
  Standard_EXPORT TCollection_AsciiString FileName (const Standard_Integer dnum, const Standard_Integer pnum, const Standard_Integer nbpack = 0);

  DEFINE_STANDARD_RTTIEXT(IFSelect_ShareOut,Standard_Transient)


private:

  IFSelect_TSeqOfDispatch thedisps;
  IFSelect_SequenceOfGeneralModifier themodelmodifiers;
  IFSelect_SequenceOfGeneralModifier thefilemodifiers;
  Handle(TCollection_HAsciiString) thepref;
  Handle(TCollection_HAsciiString) thedefrt;
  Handle(TCollection_HAsciiString) theext;
  Standard_Integer thenbdefs;
  Standard_Integer thelastrun;

};

#endif // _IFSelect_ShareOut_HeaderFile
