// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//      	-------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Feb 10 1997	Creation



#include <DDF.ixx>

#include <DDF_Data.hxx>

#include <Draw.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TCollection_AsciiString.hxx>

#include <TDF_ChildIterator.hxx>
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



