// Created on: 2000-03-01
// Created by: Denis PASCAL
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <CDF_Session.hxx>
#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>

#include <StdLDrivers.hxx>
#include <BinLDrivers.hxx>
#include <XmlLDrivers.hxx>
#include <StdDrivers.hxx>
#include <BinDrivers.hxx>
#include <XmlDrivers.hxx>

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

const Handle(TDocStd_Application)& DDocStd::GetApplication()
{
  static Handle(TDocStd_Application) anApp;
  if (anApp.IsNull())
  {
    anApp = new TDocStd_Application;

    // Initialize standard document formats at creation - they should
    // be available even if this DRAW plugin is not loaded by pload command
    StdLDrivers::DefineFormat(anApp);
    BinLDrivers::DefineFormat(anApp);
    XmlLDrivers::DefineFormat(anApp);
    StdDrivers::DefineFormat(anApp);
    BinDrivers::DefineFormat(anApp);
    XmlDrivers::DefineFormat(anApp);
  }
  return anApp;
}


//=======================================================================
//function : GetDocument
//purpose  : 
//=======================================================================

Standard_Boolean DDocStd::GetDocument (Standard_CString&         Name,
				       Handle(TDocStd_Document)& DOC,
				       const Standard_Boolean    Complain)
{

  Handle(Draw_Drawable3D) D = Draw::Get(Name,Standard_False);

  Handle(DDocStd_DrawDocument) DD = Handle(DDocStd_DrawDocument)::DownCast (D);
  if (DD.IsNull()) {
    if (Complain) cout << Name << " is not a Document" << endl; 
    return Standard_False;
  }
  Handle(TDocStd_Document) STDDOC = DD->GetDocument();
  if (!STDDOC.IsNull()) {
    DOC = STDDOC;
    return Standard_True;
  }
  if (Complain) cout << Name << " is not a CAF Document" << endl; 
  return Standard_False;
}


//=======================================================================
//function : Label
//purpose  : try to retrieve a label 
//=======================================================================

Standard_Boolean DDocStd::Find (const Handle(TDocStd_Document)& D,
				const Standard_CString  Entry,
				TDF_Label& Label,   
				const Standard_Boolean  Complain)
{
  Label.Nullify();
  TDF_Tool::Label(D->GetData(),Entry,Label,Standard_False);
  if (Label.IsNull() && Complain) cout<<"No label for entry "<<Entry<<endl;
  return !Label.IsNull();
}

//=======================================================================
//function : Find
//purpose  : Try to retrieve an attribute.
//=======================================================================

Standard_Boolean DDocStd::Find (const Handle(TDocStd_Document)& D,
				const Standard_CString  Entry,
				const Standard_GUID&    ID,
				Handle(TDF_Attribute)&  A,
				const Standard_Boolean  Complain) 
{
  TDF_Label L;
  if (Find(D,Entry,L,Complain)) {
    if (L.FindAttribute(ID,A)) return Standard_True;
    if (Complain) cout <<"attribute not found for entry : "<< Entry <<endl; 
  }
  return Standard_False;   
}


//=======================================================================
//function : ReturnLabel
//purpose  : 
//=======================================================================
 
Draw_Interpretor& DDocStd::ReturnLabel(Draw_Interpretor& di, const TDF_Label& L)
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

void DDocStd::AllCommands(Draw_Interpretor& theCommands) 
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  // define commands
  DDocStd::ApplicationCommands(theCommands);
  DDocStd::DocumentCommands(theCommands);
  DDocStd::ToolsCommands(theCommands);
  DDocStd::MTMCommands(theCommands);
  DDocStd::ShapeSchemaCommands(theCommands);
}
