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

#include <DDocStd.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <TDF_ListIteratorOfAttributeDeltaList.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_Delta.hxx> 
#include <TDF_AttributeDelta.hxx> 
#include <TDF_DeltaOnAddition.hxx> 
#include <TDF_DeltaOnForget.hxx>
#include <TDF_DeltaOnResume.hxx>
#include <TDF_DeltaOnRemoval.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_AttributeDeltaList.hxx>
#include <TDF_ListIteratorOfAttributeDeltaList.hxx> 
#include <Standard_DomainError.hxx>



//=======================================================================
//function : UpdateXLinks 
//=======================================================================

static Standard_Integer DDocStd_UpdateXLinks(Draw_Interpretor& /*di*/,Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(a[1],D)) return 1;
  TCollection_AsciiString Entry(a[2]);
  D->UpdateReferences(Entry);
  // DDocStd::DisplayModified(a[1]);
  return 0;
}

//=======================================================================
//function : DDocStd_DumpCommand
//purpose  : DumpDocument (DOC)
//=======================================================================

static Standard_Integer DDocStd_DumpCommand (Draw_Interpretor& di,
					     Standard_Integer nb, 
					     const char** arg) 
{   
  if (nb == 2) {   
    Handle(TDocStd_Document) D;       
    if (!DDocStd::GetDocument(arg[1],D)) return 1;
    //
    TDF_AttributeDeltaList added, forgoten, resumed, removed, modified;
    Handle(TDF_AttributeDelta) AD;
    if (D->GetUndos().IsEmpty()) {   
      di << "no UNDO available\n";
      return 0;
    }
    Handle(TDF_Delta) DELTA = D->GetUndos().Last();
    TDF_ListIteratorOfAttributeDeltaList it (DELTA->AttributeDeltas());
    for (;it.More();it.Next()) {
      AD = it.Value();
      if      (AD->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition)))     {added.Append(AD);}
      else if (AD->IsKind(STANDARD_TYPE(TDF_DeltaOnForget)))       {forgoten.Append(AD);}
      else if (AD->IsKind(STANDARD_TYPE(TDF_DeltaOnResume)))       {resumed.Append(AD);}
      else if (AD->IsKind(STANDARD_TYPE(TDF_DeltaOnRemoval)))      {removed.Append(AD);}
      else if (AD->IsKind(STANDARD_TYPE(TDF_DeltaOnModification))) {modified.Append(AD);}
      else {
	throw Standard_DomainError("DDocStd_DumpCommand : unknown delta");
      }
    }
    //
    TCollection_AsciiString string;   
    //  
    TCollection_AsciiString name; // (D->Name());
    di << "ADDED    :"; 
    it.Initialize(added);
    if (it.More()) di << "\n";
    else di << " empty\n";
    for (;it.More();it.Next()) {   
      TDF_Tool::Entry (it.Value()->Label(),string);
      di << "- " << string.ToCString() << " ";      
      di <<  it.Value()->Attribute()->DynamicType()->Name();
      di << "\n";
    }
    //
    // forgoten    
    di << "FORGOTEN :";
    it.Initialize(forgoten);    
    if (it.More()) di << "\n";
    else di << " empty\n";
    for (;it.More();it.Next()) {   
      TDF_Tool::Entry (it.Value()->Label(),string);
      di << "- " << string.ToCString() << " ";
      di <<  it.Value()->Attribute()->DynamicType()->Name();
      di << "\n";
    }
    //
    // resumed
    di << "RESUMED  :"; 
    it.Initialize(resumed);
    if (it.More()) di << "\n";
    else di << " empty\n";
    for (;it.More();it.Next()) {   
      TDF_Tool::Entry (it.Value()->Label(),string);
      di << "- " << string.ToCString() << " ";
      di <<  it.Value()->Attribute()->DynamicType()->Name();
      di << "\n";
    }
    //
    // removed  
    di << "REMOVED  :";     
    it.Initialize(removed);
    if (it.More()) di << "\n";
    else di << " empty\n";
    for (;it.More();it.Next()) {   
      TDF_Tool::Entry (it.Value()->Label(),string);
      di << "- " << string.ToCString() << " "; 
      di <<  it.Value()->Attribute()->DynamicType()->Name();
      di << "\n";
    }
    //
    // modified  
    di << "MODIFIED :";   
    it.Initialize(modified);
    if (it.More()) di << "\n";
    else di << " empty\n";
    for (;it.More();it.Next()) {   
      TDF_Tool::Entry (it.Value()->Label(),string);
      di << "- " << string.ToCString() << " ";
      di <<  it.Value()->Attribute()->DynamicType()->Name();
      di << "\n";
    }
    return 0;
  } 
  di << "TDocStd_DumpCommand : Error\n";
  return 1;
}



//=======================================================================
//function : ModificationCommands
//purpose  : 
//=======================================================================

void DDocStd::ToolsCommands(Draw_Interpretor& theCommands) 
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  const char* g = "DDocStd commands";


  theCommands.Add("UpdateXLinks","UpdateXLinks DocName DocEntry",
		  __FILE__, DDocStd_UpdateXLinks, g);  

  theCommands.Add ("DumpCommand", 
                   "DumpCommand (DOC)",
		   __FILE__, DDocStd_DumpCommand, g);   

}

