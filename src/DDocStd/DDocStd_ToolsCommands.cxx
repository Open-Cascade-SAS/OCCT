// Created on: 2000-03-01
// Created by: Denis PASCAL
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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
      di << "no UNDO available" << "\n";
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
	Standard_DomainError::Raise("DDocStd_DumpCommand : unknown delta");
      }
    }
    //
    TCollection_AsciiString string;   
    //  
    TCollection_AsciiString name; // (D->Name());
    di << "ADDED    :"; 
    it.Initialize(added);
    if (it.More()) di << "\n";
    else di << " empty" << "\n";
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
    else di << " empty" << "\n";
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
    else di << " empty" << "\n";
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
    else di << " empty" << "\n";
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
    else di << " empty" << "\n";
    for (;it.More();it.Next()) {   
      TDF_Tool::Entry (it.Value()->Label(),string);
      di << "- " << string.ToCString() << " ";
      di <<  it.Value()->Attribute()->DynamicType()->Name();
      di << "\n";
    }
    return 0;
  } 
  di << "TDocStd_DumpCommand : Error" << "\n";
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

