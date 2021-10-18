// Created on: 1993-07-27
// Created by: Christian CAILLET
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

#ifndef _IFSelect_Activator_HeaderFile
#define _IFSelect_Activator_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <IFSelect_ReturnStatus.hxx>
class IFSelect_SessionPilot;


class IFSelect_Activator;
DEFINE_STANDARD_HANDLE(IFSelect_Activator, Standard_Transient)

//! Defines the general frame for working with a SessionPilot.
//! Each Activator treats a set of Commands. Commands are given as
//! alphanumeric strings. They can be of two main forms :
//! - classic, to list, evaluate, enrich the session (by itself) :
//! no specific remark, its complete execution must be described
//! - creation of a new item : instead of creatinf it plus adding
//! it to the session (which is a classic way), it is possible
//! to create it and make it recorded by the SessionPilot :
//! then, the Pilot will add it to the session; this way allows
//! the Pilot to manage itself named items
//!
//! In order to make easier the use of Activator, this class
//! provides a simple way to Select an Actor for a Command :
//! each sub-class of SectionActor defines the command titles it
//! recognizes, plus attaches a Number, unique for this sub-class,
//! to each distinct command title.
//!
//! Each time an action is required, the corresponding Number
//! can then be given to help the selection of the action to do.
//!
//! The result of an Execution must indicate if it is worth to be
//! recorded or not : see method Do
class IFSelect_Activator : public Standard_Transient
{

public:

  
  //! Records, in a Dictionary available for all the Activators,
  //! the command title an Activator can process, attached with
  //! its number, proper for this Activator
  //! <mode> allows to distinguish various execution modes
  //! 0: default mode; 1 : for xset
  Standard_EXPORT static void Adding (const Handle(IFSelect_Activator)& actor, const Standard_Integer number, const Standard_CString command, const Standard_Integer mode);
  
  //! Allows a self-definition by an Activator of the Commands it
  //! processes, call the class method Adding (mode 0)
  Standard_EXPORT void Add (const Standard_Integer number, const Standard_CString command) const;
  
  //! Same as Add but specifies that this command is candidate for
  //! xset (creation of items, xset : named items; mode 1)
  Standard_EXPORT void AddSet (const Standard_Integer number, const Standard_CString command) const;
  
  //! Removes a Command, if it is recorded (else, does nothing)
  Standard_EXPORT static void Remove (const Standard_CString command);
  
  //! Selects, for a Command given by its title, an actor with its
  //! command number. Returns True if found, False else
  Standard_EXPORT static Standard_Boolean Select (const Standard_CString command, Standard_Integer& number, Handle(IFSelect_Activator)& actor);
  
  //! Returns mode recorded for a command. -1 if not found
  Standard_EXPORT static Standard_Integer Mode (const Standard_CString command);
  
  //! Returns, for a root of command title, the list of possible
  //! commands.
  //! <mode> : -1 (D) for all commands if <commands> is empty
  //! -1 + command : about a Group , >= 0 see Adding
  //! By default, it returns the whole list of known commands.
  Standard_EXPORT static Handle(TColStd_HSequenceOfAsciiString) Commands (const Standard_Integer mode = -1, const Standard_CString command = "");
  
  //! Tries to execute a Command Line. <number> is the number of the
  //! command for this Activator. It Must forecast to record the
  //! result of the execution, for need of Undo-Redo
  //! Must Returns : 0 for a void command (not to be recorded),
  //! 1 if execution OK, -1 if command incorrect, -2 if error
  //! on execution
  Standard_EXPORT virtual IFSelect_ReturnStatus Do (const Standard_Integer number, const Handle(IFSelect_SessionPilot)& pilot) = 0;
  
  //! Sends a short help message for a given command identified by
  //! it number for this Activator (must take one line max)
  Standard_EXPORT virtual Standard_CString Help (const Standard_Integer number) const = 0;
  
  Standard_EXPORT Standard_CString Group() const;
  
  Standard_EXPORT Standard_CString File() const;
  
  //! Group and SetGroup define a "Group of commands" which
  //! correspond to an Activator. Default is "XSTEP"
  //! Also a file may be attached
  Standard_EXPORT void SetForGroup (const Standard_CString group, const Standard_CString file = "");




  DEFINE_STANDARD_RTTIEXT(IFSelect_Activator,Standard_Transient)

protected:

  
  //! Sets the default values
  Standard_EXPORT IFSelect_Activator();



private:


  TCollection_AsciiString thegroup;
  TCollection_AsciiString thefile;


};







#endif // _IFSelect_Activator_HeaderFile
