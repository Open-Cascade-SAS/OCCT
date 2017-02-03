// Created on: 1992-10-28
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

#ifndef _Interface_CheckIterator_HeaderFile
#define _Interface_CheckIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_HSequenceOfCheck.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Interface_CheckStatus.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
class Interface_InterfaceModel;
class Interface_IntVal;
class Standard_NoSuchObject;
class Interface_Check;
class Standard_Transient;
class Message_Messenger;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Result of a Check operation (especially from InterfaceModel)
class Interface_CheckIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty CheckIterator
  Standard_EXPORT Interface_CheckIterator();
  
  //! Creates a CheckIterator with a name (displayed by Print as a
  //! title)
  Standard_EXPORT Interface_CheckIterator(const Standard_CString name);
  
  //! Sets / Changes the name
  Standard_EXPORT void SetName (const Standard_CString name);
  
  //! Returns the recorded name (can be empty)
  Standard_EXPORT Standard_CString Name() const;
  
  //! Defines a Model, used to locate entities (not required, if it
  //! is absent, entities are simply less documented)
  Standard_EXPORT void SetModel (const Handle(Interface_InterfaceModel)& model);
  
  //! Returns the stored model (can be a null handle)
  Standard_EXPORT Handle(Interface_InterfaceModel) Model() const;
  
  //! Clears the list of checks
  Standard_EXPORT void Clear();
  
  //! Merges another CheckIterator into <me>, i.e. adds each of its
  //! Checks. Content of <other> remains unchanged.
  //! Takes also the Model but not the Name
  Standard_EXPORT void Merge (Interface_CheckIterator& other);
  
  //! Adds a Check to the list to be iterated
  //! This Check is Accompanied by Entity Number in the Model
  //! (0 for Global Check or Entity unknown in the Model), if 0 and
  //! Model is recorded in <me>, it is computed
  Standard_EXPORT void Add (const Handle(Interface_Check)& ach, const Standard_Integer num = 0);
  
  //! Returns the Check which was attached to an Entity given its
  //! Number in the Model. <num>=0 is for the Global Check.
  //! If no Check was recorded for this Number, returns an empty
  //! Check.
  //! Remark : Works apart from the iteration methods (no interference)
  Standard_EXPORT const Handle(Interface_Check)& Check (const Standard_Integer num) const;
  
  //! Returns the Check attached to an Entity
  //! If no Check was recorded for this Entity, returns an empty
  //! Check.
  //! Remark : Works apart from the iteration methods (no interference)
  Standard_EXPORT const Handle(Interface_Check)& Check (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the Check bound to an Entity Number (0 : Global)
  //! in order to be consulted or completed on the spot
  //! I.e. returns the Check if is already exists, or adds it then
  //! returns the new empty Check
  Standard_EXPORT Handle(Interface_Check)& CCheck (const Standard_Integer num);
  
  //! Returns the Check bound to an Entity, in order to be consulted
  //! or completed on the spot
  //! I.e. returns the Check if is already exists, or adds it then
  //! returns the new empty Check
  Standard_EXPORT Handle(Interface_Check)& CCheck (const Handle(Standard_Transient)& ent);
  
  //! Returns True if : no Fail has been recorded if <failsonly> is
  //! True, no Check at all if <failsonly> is False
  Standard_EXPORT Standard_Boolean IsEmpty (const Standard_Boolean failsonly) const;
  
  //! Returns worst status among : OK, Warning, Fail
  Standard_EXPORT Interface_CheckStatus Status() const;
  
  //! Tells if this check list complies with a given status :
  //! OK (i.e. empty),  Warning (at least one Warning, but no Fail),
  //! Fail (at least one), Message (not OK), NoFail, Any
  Standard_EXPORT Standard_Boolean Complies (const Interface_CheckStatus status) const;
  
  //! Returns a CheckIterator which contains the checks which comply
  //! with a given status
  //! Each check is added completely (no split Warning/Fail)
  Standard_EXPORT Interface_CheckIterator Extract (const Interface_CheckStatus status) const;
  
  //! Returns a CheckIterator which contains the check which comply
  //! with a message, plus some conditions as follows :
  //! <incl> = 0 : <mess> exactly matches one of the messages
  //! <incl> < 0 : <mess> is contained by one of the messages
  //! <incl> > 0 : <mess> contains one of the messages
  //! For <status> : for CheckWarning and CheckFail, considers only
  //! resp. Warning or Check messages. for CheckAny, considers all
  //! other values are ignored (answer will be false)
  //! Each Check which complies is entirely taken
  Standard_EXPORT Interface_CheckIterator Extract (const Standard_CString mess, const Standard_Integer incl, const Interface_CheckStatus status) const;
  
  //! Removes the messages of all Checks, under these conditions :
  //! <incl> = 0 : <mess> exactly matches one of the messages
  //! <incl> < 0 : <mess> is contained by one of the messages
  //! <incl> > 0 : <mess> contains one of the messages
  //! For <status> : for CheckWarning and CheckFail, considers only
  //! resp. Warning or Check messages. for CheckAny, considers all
  //! other values are ignored (nothing is done)
  //! Returns True if at least one message has been removed, False else
  Standard_EXPORT Standard_Boolean Remove (const Standard_CString mess, const Standard_Integer incl, const Interface_CheckStatus status);
  
  //! Returns the list of entities concerned by a Check
  //! Only fails if <failsonly> is True, else all non-empty checks
  //! If <global> is true, adds the model for a global check
  //! Else, global check is ignored
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) Checkeds (const Standard_Boolean failsonly, const Standard_Boolean global) const;
  
  //! Starts Iteration. Thus, it is possible to restart it
  //! Remark : an iteration may be done with a const Iterator
  //! While its content is modified (through a pointer), this allows
  //! to give it as a const argument to a function
  Standard_EXPORT void Start() const;
  
  //! Returns True if there are more Checks to get
  Standard_EXPORT Standard_Boolean More() const;
  
  //! Sets Iteration to next Item
  Standard_EXPORT void Next() const;
  
  //! Returns Check currently Iterated
  //! It brings all other informations (status, messages, ...)
  //! The Number of the Entity in the Model is given by Number below
  Standard_EXPORT const Handle(Interface_Check)& Value() const;
  
  //! Returns Number of Entity for the Check currently iterated
  //! or 0 for GlobalCheck
  Standard_EXPORT Standard_Integer Number() const;
  
  //! Prints the list of Checks with their attached Numbers
  //! If <failsonly> is True, prints only Fail messages
  //! If <failsonly> is False, prints all messages
  //! If <final> = 0 (D), prints also original messages if different
  //! If <final> < 0, prints only original messages
  //! If <final> > 0, prints only final messages
  //! It uses the recorded Model if it is defined
  //! Remark : Works apart from the iteration methods (no interference)
  Standard_EXPORT void Print (const Handle(Message_Messenger)& S, const Standard_Boolean failsonly, const Standard_Integer final = 0) const;
  
  //! Works as Print without a model, but for entities which have
  //! no attached number (Number not positive), tries to compute
  //! this Number from <model> and displays "original" or "computed"
  Standard_EXPORT void Print (const Handle(Message_Messenger)& S, const Handle(Interface_InterfaceModel)& model, const Standard_Boolean failsonly, const Standard_Integer final = 0) const;
  
  //! Clears data of iteration
  Standard_EXPORT void Destroy();
~Interface_CheckIterator()
{
  Destroy();
}




protected:





private:



  Handle(Interface_HSequenceOfCheck) thelist;
  Handle(TColStd_HSequenceOfInteger) thenums;
  Handle(Interface_InterfaceModel) themod;
  TCollection_AsciiString thename;
  Handle(Interface_IntVal) thecurr;


};







#endif // _Interface_CheckIterator_HeaderFile
