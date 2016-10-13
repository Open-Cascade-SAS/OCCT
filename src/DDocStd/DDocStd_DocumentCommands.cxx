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

#include <BinDrivers_DocumentStorageDriver.hxx>
#include <DDF.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <TDocStd_XLinkTool.hxx>
#include <TDF_Reference.hxx>
#include <TDocStd_Document.hxx>
#include <CDM_Document.hxx>
#include <TDocStd_Modified.hxx>
#include <TDF_Label.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_XLink.hxx>
#include <TDocStd_XLinkRoot.hxx>
#include <TDocStd_XLinkIterator.hxx>
#include <TDocStd_Application.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <Plugin.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF_Tool.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Tool.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <AIS_InteractiveContext.hxx>
// pour propagate
#include <TDocStd_XLinkTool.hxx>

#include <OSD_Function.hxx>
#include <OSD_SharedLibrary.hxx>
#include <OSD_LoadMode.hxx>

//typedef Standard_Integer (* DFBROWSER_CALL)(const Handle(TDocStd_Document)&);
//static DFBROWSER_CALL gDFunc = 0;

//=======================================================================
//function : Main
//purpose  : Main (DOC)
//=======================================================================

static Standard_Integer DDocStd_Main (Draw_Interpretor& di,Standard_Integer nb, const char** a)
{
  if (nb == 2) {
    Handle(TDocStd_Document) DOC;   
    if (!DDocStd::GetDocument(a[1],DOC)) return 1;
    DDocStd::ReturnLabel(di,DOC->Main());
    return 0;
  }   
  di << "DDocStd_Main : Error\n";
  return 1;  
}


//=======================================================================
//function : Format
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_Format (Draw_Interpretor& di,Standard_Integer n, const char** a)
{ 
  Handle(TDocStd_Document) D;
  if (n == 2) {
    if (!DDocStd::GetDocument(a[1],D)) return 1;    
    //cout << "FORMAT : " << D->StorageFormat() << endl;
    di << "FORMAT : ";
    Standard_SStream aStream;
    D->StorageFormat().Print(aStream);
    di << aStream;
    di << "\n";
    return 0;
  }  
  if (n == 3) {
    if (!DDocStd::GetDocument(a[1],D)) return 1;    
    D->ChangeStorageFormat(a[2]);  
    return 0;
  }  
  di << "DDocStd_Format : Error\n";
  return 1;
}

//=======================================================================
//function : Copy "Copy DOC entry XDOC xentry",
//=======================================================================

static Standard_Integer DDocStd_Copy (Draw_Interpretor& di,Standard_Integer n, const char** a)
{
  if (n == 5) {  
    Handle(TDocStd_Document) DOC, XDOC;  
    if (!DDocStd::GetDocument(a[1],DOC)) return 1; 
    if (!DDocStd::GetDocument(a[3],XDOC)) return 1;
    TDF_Label L,XL;
    if (!DDocStd::Find(DOC,a[2],L)) return 1; 
    if (!DDocStd::Find(XDOC,a[4],XL)) return 1;
    TDocStd_XLinkTool XLinkTool;
    XLinkTool.Copy(L,XL);
    if (!XLinkTool.IsDone()) {
      di << "DDocStd_XLinkTool : not done\n";
    }
    return 0;
  }  
  di << "DDocStd_XLinkTool : Error\n";
  return 1; 
}


//=======================================================================
//function : CopyWithLink "Copy DOC entry XDOC xentry",
//=======================================================================

static Standard_Integer DDocStd_CopyWithLink (Draw_Interpretor& di,Standard_Integer n, const char** a)
{  
  if (n == 5) {  
    Handle(TDocStd_Document) DOC, XDOC;  
    if (!DDocStd::GetDocument(a[1],DOC)) return 1; 
    if (!DDocStd::GetDocument(a[3],XDOC)) return 1;   
    TDF_Label L,XL;
    if (!DDocStd::Find(DOC,a[2],L)) return 1; 
    if (!DDocStd::Find(XDOC,a[4],XL)) return 1;
    TDocStd_XLinkTool XLinkTool;
    XLinkTool.CopyWithLink(L,XL);
    if (!XLinkTool.IsDone()) {
      di << "DDocStd_CopyWithLink : not done\n";
    }
    return 0;
  }  
  di << "DDocStd_CopyWithLink : Error\n";
  return 1; 
}

//=======================================================================
//function : UpdateLink (D,[xrefentry])
//=======================================================================

static Standard_Integer DDocStd_UpdateLink (Draw_Interpretor& di,Standard_Integer nb, const char** a)
{  
  if (nb == 2 || nb == 3) {  
    Handle(TDocStd_Document) DOC;  
    if (!DDocStd::GetDocument(a[1],DOC)) return 1; 
    Handle(TDF_Reference) REF;
    TDocStd_XLinkTool XLinkTool;  
    if (nb == 3) {
      if (!DDocStd::Find(DOC,a[2],TDF_Reference::GetID(),REF)) return 1;
      XLinkTool.UpdateLink(REF->Label());
      if (!XLinkTool.IsDone()) {
	di << "DDocStd_UpdateXLink : not done\n";
      }
    }
    else {
      for (TDocStd_XLinkIterator xit (DOC); xit.More(); xit.Next()) {
	XLinkTool.UpdateLink(xit.Value()->Label());
	if (!XLinkTool.IsDone()) {
	  di << "DDocStd_UpdateXLink : not done\n";
	}
      }
    }
    return 0;
  }
  di << "DDocStd_UpdateXLink : Error\n";
  return 1; 
}

//=======================================================================
//function : UndoLimit
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_UndoLimit (Draw_Interpretor& di,Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  
  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(a[1],D)) return 1;
  
  if (n > 2) {
    Standard_Integer lim = Draw::Atoi(a[2]);
    D->SetUndoLimit(lim);
  }
  
  // display current values
  di << D->GetUndoLimit() << " ";
  di << D->GetAvailableUndos() << " ";
  di << D->GetAvailableRedos();
  return 0;
}

//=======================================================================
//function : Undo, Redo
//purpose  : Undo (DOC)
//=======================================================================

static Standard_Integer DDocStd_Undo (Draw_Interpretor& di,Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  
  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(a[1],D)) return 1;
  
  Standard_Integer i,step = 1;
  if (n > 2) {
    step = Draw::Atoi(a[2]);
  }

  // test if the command was undo or redo
  Standard_Boolean undo = a[0][0] == 'U';

  for (i = 1; i <= step; i++) {
    if (undo) {
      if (!D->Undo()) di << "Undo not done\n";
    }
    else {
      if (!D->Redo()) di << "Redo not done\n";
    }
  }

  // Redraw the viewer.
  Handle(AIS_InteractiveContext) IC;
  if (TPrsStd_AISViewer::Find(D->Main(), IC))
      IC->UpdateCurrentViewer();
  
  return 0;
}

//=======================================================================
//function : NewCommand
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_NewCommand(Draw_Interpretor& /*di*/,Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(a[1],D)) return 1;
  D->NewCommand();
  return 0;
}

//=======================================================================
//function : OpenCommand
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_OpenCommand(Draw_Interpretor& /*di*/,Standard_Integer n, const char** a)
{
  if (n < 2) return 1;

  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(a[1],D)) return 1;
  D->OpenCommand();
  return 0;
}

//=======================================================================
//function : AbortCommand
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_AbortCommand(Draw_Interpretor& /*di*/,Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(a[1],D)) return 1;
  D->AbortCommand();
  return 0;
}

//=======================================================================
//function : CommitCommand
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_CommitCommand(Draw_Interpretor& /*di*/,Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(a[1],D)) return 1;
  D->CommitCommand();
  return 0;
}


//=======================================================================
//function : DDocStd_DumpDocument
//purpose  : DumpDocument (DOC)
//=======================================================================

static Standard_Integer DDocStd_DumpDocument (Draw_Interpretor& di,
					      Standard_Integer nb, 
					      const char** arg) 
{   
  if (nb == 2) {   
    Handle(TDocStd_Document) D;       
    if (!DDocStd::GetDocument(arg[1],D)) return 1;     
    di << "\n"; 
    // document name
    if (D->IsSaved()) 
      di << "DOCUMENT      : " << TCollection_AsciiString(D->GetName(),'?').ToCString();
    else 
      di << "DOCUMENT      : not saved";
    di << "\n";
    // format
    //cout << "FORMAT        : " << D->StorageFormat();
    di << "FORMAT        : ";
    Standard_SStream aStream;
    D->StorageFormat().Print(aStream);
    di << aStream;
    di << "\n";
    // command
    di << "COMMAND       : ";
    if (D->HasOpenCommand()) di << " Is Open";
    else di << " Is Not Open";
    // undo
    di << "UNDO          :"; 
    di << " limit :" << D->GetUndoLimit();
    di << " undos :" << D->GetAvailableUndos() << " ";  
    di << " redos :" << D->GetAvailableRedos(); 
    di << "\n";
    // cout << "CURRENT :";   
    //     TCollection_AsciiString string;  
    //     TDF_Tool::Entry(D->CurrentLabel(),string); 
    //     cout << string;
    //     cout << endl;
    // modified  
    di << "MODIFIED      : ";
    if (D->IsModified()) di << "true";
    else di << "false";  
    di << "\n"; 
    if (!TDocStd_Modified::IsEmpty(D->Main())) {
      di << "MODIFICATIONS : ";     
      TDF_MapIteratorOfLabelMap it (D->GetModified()); 
      if (!it.More()) di << "VALID\n"; 
      else {
	TCollection_AsciiString string;  
	for (;it.More();it.Next()) { 
	  TDF_Tool::Entry(it.Key(),string);
	  di << string.ToCString() << " "; 
	}
	di << "\n";
      } 
    }
    return 0;
  } 
  di << "DDocStd_DumpDocument : Error\n";
  return 1;
}

//=======================================================================
//function : SetModified
//purpose  : Set modifications in a document
//=======================================================================

static Standard_Integer DDocStd_SetModified (Draw_Interpretor& di,Standard_Integer n, const char** a)
{
  if (n > 2) {
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(a[1],D)) return 1;
    TDF_Label L;
    for (Standard_Integer i = 2; i < n; i++) {
      if (DDocStd::Find(D,a[i],L)) D->SetModified(L);
    }
    return 0; 
  }
  di << "DDocStd_SetModified : Error\n";
  return 1;
}

//=======================================================================
//function : Propagate
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_Propagate (Draw_Interpretor& di,Standard_Integer /*n*/, const char** /*a*/)
{
//   if (n == 2) {
//     Handle(TDocStd_Document) D;
//     if (!DDocStd::GetDocument(a[1],D)) return 1;
//     if (D->IsValid()) {
//       cout << "the document is valid" << endl;
//       return 0;
//     }
//     Handle(TDesign_Function) F; 
//     if (!D->Main().FindAttribute(TDesign_Function::GetID(),F)) {
//       cout << "no function found at main" << endl;
//       return 0;
//     }
//     TFunction_Logbook mdf (Standard_True);   
//     for (TDF_MapIteratorOfLabelMap it (D->GetModified());it.More();it.Next()) {
//       mdf.SetTouched(it.Key());
//     }  
//     F->Execute(mdf);
//     D->PurgeModified();
//     return 0;
//   }
  di << "DDocStd_Propagate : not implemented\n";
  return 1;
}

//=======================================================================
//function : DDocStd_StoreTriangulation
//purpose  :
//=======================================================================

static Standard_Integer DDocStd_StoreTriangulation (Draw_Interpretor& theDi,
                                                    Standard_Integer theNbArgs,
                                                    const char** theArgVec)
{
  const Handle(TDocStd_Application)& anApp = DDocStd::GetApplication();
  Handle(BinDrivers_DocumentStorageDriver) aDriverXCaf = Handle(BinDrivers_DocumentStorageDriver)::DownCast(anApp->WriterFromFormat ("BinXCAF"));
  Handle(BinDrivers_DocumentStorageDriver) aDriverOcaf = Handle(BinDrivers_DocumentStorageDriver)::DownCast(anApp->WriterFromFormat ("BinOcaf"));
  if (aDriverXCaf.IsNull()
   || aDriverOcaf.IsNull())
  {
    std::cout << "Error: BinXCAF or BinOcaf storage formats are not registered\n";
    return 1;
  }

  if (theNbArgs == 1)
  {
    theDi << (aDriverXCaf->IsWithTriangles() ? "1" : "0");
    return 0;
  }
  else if (theNbArgs != 2)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  const Standard_Boolean toEnable = (Draw::Atoi (theArgVec[1]) != 0);
  aDriverXCaf->SetWithTriangles (anApp->MessageDriver(), toEnable);
  aDriverOcaf->SetWithTriangles (anApp->MessageDriver(), toEnable);
  return 0;
}

//=======================================================================
//function : DocumentCommands
//purpose  : 
//=======================================================================

void DDocStd::DocumentCommands(Draw_Interpretor& theCommands) 
{
  
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  
  
  const char* g = "DDocStd commands";
  
  // Data Framework Access
  
  theCommands.Add("Main","Main (DOC)",
  		  __FILE__, DDocStd_Main, g);  

  
  // DUMP
  
  theCommands.Add ("Format", 
                   "Format (DOC, [format])",
		   __FILE__, DDocStd_Format, g); 

  theCommands.Add ("DumpDocument", 
                   "DumpDocument (DOC)",
		   __FILE__, DDocStd_DumpDocument, g);   

  theCommands.Add ("StoreTriangulation",
                   "StoreTriangulation [toStore={0|1}]"
                   "\nSetup BinXCAF/BinOcaf storage drivers to write triangulation",
                   __FILE__, DDocStd_StoreTriangulation, g);

  // XREF

  theCommands.Add("Copy","Copy DOC entry XDOC xentry",
		  __FILE__, DDocStd_Copy, g);  

  theCommands.Add("CopyWithLink","CopyWithLink DOC entry XDOC xentry",
		  __FILE__, DDocStd_CopyWithLink, g);  

  theCommands.Add("UpdateLink","UpdateLink DOC [entry]",
		  __FILE__, DDocStd_UpdateLink, g);  


  // UNDO/REDO

  theCommands.Add("UndoLimit","UndoLimit DOC (Value), return UndoLimit Undos Redos",
		  __FILE__, DDocStd_UndoLimit, g);
  
  theCommands.Add("Undo","Undo DOC (steps = 1)",
		  __FILE__, DDocStd_Undo, g);
  
  theCommands.Add("Redo","Redo DOC (steps = 1)",
		  __FILE__, DDocStd_Undo, g);
  
  theCommands.Add("NewCommand","NewCommand DOC",
		  __FILE__, DDocStd_NewCommand, g);  

  theCommands.Add("OpenCommand","OpenCommand DOC",
		  __FILE__, DDocStd_OpenCommand, g);  

  theCommands.Add("AbortCommand","AbortCommand DOC",
		  __FILE__, DDocStd_AbortCommand, g);    

  theCommands.Add("CommitCommand","CommitCommand DOC",
		  __FILE__, DDocStd_CommitCommand, g);    


  // Modif and Propagation

  theCommands.Add("SetModified","SetModified DOC Label1 Label2 ....",
 		  __FILE__, DDocStd_SetModified, g);

  theCommands.Add("Propagate","Propagate DOC",
		  __FILE__, DDocStd_Propagate, g);

}

