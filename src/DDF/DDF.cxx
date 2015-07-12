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

//      	-------
// Version:	0.0
//Version	Date		Purpose
//		0.0	Feb 10 1997	Creation

#include <DDF.hxx>
#include <DDF_Data.hxx>
#include <Draw.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

//=======================================================================
//function : AddLabel
//purpose  : 
//=======================================================================
Standard_Boolean DDF::AddLabel 

(
 const Handle(TDF_Data)& DF,
 const Standard_CString  Entry,
 TDF_Label&              Label
) 
{
  TDF_Tool::Label (DF,Entry,Label,Standard_True);
  return Standard_True;
}


//=======================================================================
//function : FindLabel
//purpose  : 
//=======================================================================

Standard_Boolean DDF::FindLabel (const Handle(TDF_Data)& DF,
				const Standard_CString  Entry,
				      TDF_Label&        Label,   
                                const Standard_Boolean  Complain)
{
  Label.Nullify();
  TDF_Tool::Label(DF,Entry,Label,Standard_False);
  if (Label.IsNull() && Complain) cout << "No label for entry " << Entry <<endl;
  return !Label.IsNull();
}


//=======================================================================
//function : GetDF
//purpose  : 
//=======================================================================

Standard_Boolean DDF::GetDF (Standard_CString&       Name,
			     Handle(TDF_Data)&       DF,
                             const Standard_Boolean  Complain)
{ 
  Handle(Standard_Transient) t = Draw::Get(Name, Complain);
  Handle(DDF_Data) DDF = Handle(DDF_Data)::DownCast (t);
  //Handle(DDF_Data) DDF = Handle(DDF_Data)::DownCast (Draw::Get(Name, Complain)); 
  if (!DDF.IsNull()) {
    DF = DDF->DataFramework(); 
    return Standard_True;
  } 
  if (Complain) cout <<"framework "<<Name<<" not found "<< endl; 
  return Standard_False;
}


//=======================================================================
//function : Find
//purpose  : Finds an attribute.
//=======================================================================

Standard_Boolean DDF::Find (const Handle(TDF_Data)& DF,
			    const Standard_CString  Entry,
			    const Standard_GUID&    ID,
			    Handle(TDF_Attribute)&  A,
			    const Standard_Boolean  Complain) 
{
  TDF_Label L;
  if (FindLabel(DF,Entry,L,Complain)) {
    if (L.FindAttribute(ID,A)) return Standard_True;
    if (Complain) cout <<"attribute not found for entry : "<< Entry <<endl; 
  }
  return Standard_False;   
}


//=======================================================================
//function : ReturnLabel
//purpose  : 
//=======================================================================
 
Draw_Interpretor& DDF::ReturnLabel(Draw_Interpretor& di, const TDF_Label& L)
{
  TCollection_AsciiString S;
  TDF_Tool::Entry(L,S);
  di << S.ToCString();
  return di;
}


//=======================================================================
//function : AllCommands
//purpose  : 
//=======================================================================

void DDF::AllCommands(Draw_Interpretor& theCommands) 
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  DDF::BasicCommands         (theCommands);
  DDF::DataCommands          (theCommands);
  DDF::TransactionCommands   (theCommands);
  DDF::BrowserCommands       (theCommands);
  // define the TCL variable DDF
  const char* com = "set DDF";
  theCommands.Eval(com);
}



