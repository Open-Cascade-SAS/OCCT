// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _DDF_HeaderFile
#define _DDF_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Draw_Interpretor.hxx>
class TDF_Data;
class TDF_Label;
class Standard_GUID;
class TDF_Attribute;


//! Provides facilities to manipulate data framework
//! in a Draw-Commands environment.
class DDF 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Search in draw  directory the framewok  identified
  //! by its name <Name>. returns True if found. In that
  //! case <DF> is set.
  Standard_EXPORT static Standard_Boolean GetDF (Standard_CString& Name, Handle(TDF_Data)& DF, const Standard_Boolean Complain = Standard_True);
  
  //! Search in <DF>  the label identified by its  entry
  //! <Entry>.  returns  <True> if  found. In  that case
  //! <Label> is set.
  Standard_EXPORT static Standard_Boolean FindLabel (const Handle(TDF_Data)& DF, const Standard_CString Entry, TDF_Label& Label, const Standard_Boolean Complain = Standard_True);
  
  //! Search in <DF> the  label identified by its entry
  //! <Entry>.   if label doesn't  exist, create  and add
  //! the Label in <DF>. In that case return True.
  Standard_EXPORT static Standard_Boolean AddLabel (const Handle(TDF_Data)& DF, const Standard_CString Entry, TDF_Label& Label);
  
  //! Search   in <DF> the  attribute  identified by its
  //! <ID> and its <entry>.  returns <True> if found. In
  //! that case A is set.
  Standard_EXPORT static Standard_Boolean Find (const Handle(TDF_Data)& DF, const Standard_CString Entry, const Standard_GUID& ID, Handle(TDF_Attribute)& A, const Standard_Boolean Complain = Standard_True);
  
  //! Safe variant for arbitrary type of argument
  template <class T> 
  static Standard_Boolean Find (const Handle(TDF_Data)& DF, const Standard_CString Entry, const Standard_GUID& ID, Handle(T)& A, const Standard_Boolean Complain = Standard_True)
  {
    Handle(TDF_Attribute) anAttr = A;
    return Find (DF, Entry, ID, anAttr, Complain) && ! (A = Handle(T)::DownCast(anAttr)).IsNull();
  }

  Standard_EXPORT static Draw_Interpretor& ReturnLabel (Draw_Interpretor& theCommands, const TDF_Label& L);
  
  Standard_EXPORT static void AllCommands (Draw_Interpretor& theCommands);
  
  //! Basic commands.
  Standard_EXPORT static void BasicCommands (Draw_Interpretor& theCommands);
  
  //! Data framework commands
  //! create, clear & copy.
  Standard_EXPORT static void DataCommands (Draw_Interpretor& theCommands);
  
  //! open commit abort a transaction
  //! undo facilities.
  Standard_EXPORT static void TransactionCommands (Draw_Interpretor& theCommands);
  
  //! Browser commands .
  Standard_EXPORT static void BrowserCommands (Draw_Interpretor& theCommands);

};

#endif // _DDF_HeaderFile
