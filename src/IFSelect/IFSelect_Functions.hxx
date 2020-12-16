// Created on: 1993-07-28
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

#ifndef _IFSelect_Functions_HeaderFile
#define _IFSelect_Functions_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Standard_Boolean.hxx>
class Standard_Transient;
class IFSelect_WorkSession;
class IFSelect_Dispatch;

//! Functions gives access to all the actions which can be
//! commanded with the resources provided by IFSelect : especially
//! WorkSession and various types of Selections and Dispatches
//!
//! It works by adding functions by method Init
class IFSelect_Functions 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Takes the name of an entity, either as argument,
  //! or (if <name> is empty) on keyboard, and returns the entity
  //! name can be a label or a number (in alphanumeric),
  //! it is searched by NumberFromLabel from WorkSession.
  //! If <name> doesn't match en entity, a Null Handle is returned
  Standard_EXPORT static Handle(Standard_Transient) GiveEntity (const Handle(IFSelect_WorkSession)& WS, const Standard_CString name = "");
  
  //! Same as GetEntity, but returns the number in the model of the
  //! entity. Returns 0 for null handle
  Standard_EXPORT static Standard_Integer GiveEntityNumber (const Handle(IFSelect_WorkSession)& WS, const Standard_CString name = "");
  
  //! Computes a List of entities from a WorkSession and two idents,
  //! first and second, as follows :
  //! if <first> is a Number or Label of an entity : this entity
  //! if <first> is the name of a Selection in <WS>, and <second>
  //! not defined, the standard result of this Selection
  //! if <first> is for a Selection and <second> is defined, the
  //! standard result of this selection from the list computed
  //! with <second> (an entity or a selection)
  //! If <second> is erroneous, it is ignored
  Standard_EXPORT static Handle(TColStd_HSequenceOfTransient) GiveList (const Handle(IFSelect_WorkSession)& WS, const Standard_CString first = "", const Standard_CString second = "");
  
  //! Evaluates and returns a Dispatch, from data of a WorkSession
  //! if <mode> is False, searches for exact name of Dispatch in WS
  //! Else (D), allows a parameter between brackets :
  //! ex.: dispatch_name(parameter)
  //! The parameter can be: an integer for DispPerCount or DispPerFiles
  //! or the name of a Signature for DispPerSignature
  //! Returns Null Handle if not found not well evaluated
  Standard_EXPORT static Handle(IFSelect_Dispatch) GiveDispatch (const Handle(IFSelect_WorkSession)& WS, const Standard_CString name, const Standard_Boolean mode = Standard_True);
  
  //! Defines and loads all basic functions (as ActFunc)
  Standard_EXPORT static void Init();

};

#endif // _IFSelect_Functions_HeaderFile
