// Created on: 1992-09-21
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

#ifndef _IFSelect_HeaderFile
#define _IFSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class IFSelect_WorkSession;


//! Gives tools to manage Selecting a group of Entities
//! processed by an Interface, for instance to divide up an
//! original Model (from a File) to several smaller ones
//! They use description of an Interface Model as a graph
//!
//! Remark that this corresponds to the description of a
//! "scenario" of sharing out a File. Parts of this Scenario
//! are intended to be permanently stored. IFSelect provides
//! the Transient, active counterparts (to run the Scenario).
//! But a permanent one (either as Persistent Objects or as
//! interpretable Text) must be provided elsewhere.
class IFSelect 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Saves the state of a WorkSession from IFSelect, by using a
  //! SessionFile from IFSelect. Returns True if Done, False in
  //! case of Error on Writing. <file> gives the name of the File
  //! to be produced (this avoids to export the class SessionFile).
  Standard_EXPORT static Standard_Boolean SaveSession (const Handle(IFSelect_WorkSession)& WS, const Standard_CString file);
  
  //! Restore the state of a WorkSession from IFSelect, by using a
  //! SessionFile from IFSelect. Returns True if Done, False in
  //! case of Error on Writing. <file> gives the name of the File
  //! to be used (this avoids to export the class SessionFile).
  Standard_EXPORT static Standard_Boolean RestoreSession (const Handle(IFSelect_WorkSession)& WS, const Standard_CString file);

};

#endif // _IFSelect_HeaderFile
