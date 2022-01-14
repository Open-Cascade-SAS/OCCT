// Created on: 1996-03-05
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _IFSelect_Act_HeaderFile
#define _IFSelect_Act_HeaderFile

#include <Standard.hxx>

#include <IFSelect_ActFunc.hxx>
#include <IFSelect_Activator.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Standard_Integer.hxx>
class IFSelect_SessionPilot;


class IFSelect_Act;
DEFINE_STANDARD_HANDLE(IFSelect_Act, IFSelect_Activator)

//! Act gives a simple way to define and add functions to be ran
//! from a SessionPilot, as follows :
//!
//! Define a function as
//! static IFSelect_RetStatus myfunc
//! (const Standard_CString name,
//! const Handle(IFSelect_SessionPilot)& pilot)
//! { ... }
//! When ran, it receives the exact name (string) of the called
//! function, and the SessionPilot which brings other infos
//!
//! Add it by
//! IFSelect_Act::AddFunc (name,help,myfunc);
//! for a normal function, or
//! IFSelect_Act::AddFSet (name,help,myfunc);
//! for a function which is intended to create a control item
//! name and help are given as CString
//!
//! Then, it is available for run
class IFSelect_Act : public IFSelect_Activator
{

public:

  
  //! Creates an Act with a name, help and a function
  //! mode (Add or AddSet) is given when recording
  Standard_EXPORT IFSelect_Act(const Standard_CString name, const Standard_CString help, const IFSelect_ActFunc func);
  
  //! Execution of Command Line. remark that <number> is senseless
  //! because each Act brings one and only one function
  Standard_EXPORT IFSelect_ReturnStatus Do (const Standard_Integer number, const Handle(IFSelect_SessionPilot)& pilot) Standard_OVERRIDE;
  
  //! Short Help for commands : returns the help given to create
  Standard_EXPORT Standard_CString Help (const Standard_Integer number) const Standard_OVERRIDE;
  
  //! Changes the default group name for the following Acts
  //! group empty means to come back to default from Activator
  //! Also a file name can be precised (to query by getsource)
  Standard_EXPORT static void SetGroup (const Standard_CString group, const Standard_CString file = "");
  
  //! Adds a function with its name and help : creates an Act then
  //! records it as normal function
  Standard_EXPORT static void AddFunc (const Standard_CString name, const Standard_CString help, const IFSelect_ActFunc func);
  
  //! Adds a function with its name and help : creates an Act then
  //! records it as function for XSET (i.e. to create control item)
  Standard_EXPORT static void AddFSet (const Standard_CString name, const Standard_CString help, const IFSelect_ActFunc func);




  DEFINE_STANDARD_RTTIEXT(IFSelect_Act,IFSelect_Activator)

protected:




private:


  TCollection_AsciiString thename;
  TCollection_AsciiString thehelp;
  IFSelect_ActFunc thefunc;


};







#endif // _IFSelect_Act_HeaderFile
