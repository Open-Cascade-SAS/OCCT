// Created on: 2002-11-26
// Created by: Vladimir ANIKIN
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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
#include <TDocStd_MultiTransactionManager.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <TDocStd_Document.hxx>
#include <Draw.hxx>
#include <DDF_Browser.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TopoDS_Shape.hxx>
#include <Draw_Interpretor.hxx>
#include <TDF_Reference.hxx>

//=======================================================================
//function : mtmCreate
//purpose  : creates new new multiple transactions' manager
//=======================================================================

static Handle(TDocStd_MultiTransactionManager) sMultiTransactionManager = 0;

static int mtmCreate (Draw_Interpretor& /*di*/, int n, const char** a)
{
  if(!sMultiTransactionManager.IsNull())
    sMultiTransactionManager->SetUndoLimit(0);

  sMultiTransactionManager = new TDocStd_MultiTransactionManager();
  if(n > 1)
    sMultiTransactionManager->SetUndoLimit(Draw::Atoi(a[1]));
  return 0;
}

//=======================================================================
//function : mtmAddDocument
//purpose  : adds a document to the transactions manager
//=======================================================================

static int mtmAddDocument (Draw_Interpretor& di, int n, const char** a)
{
  if(sMultiTransactionManager.IsNull()) {
    di << "Error   : manager is not initialised\n";
    return 1;
  }
  if(n > 1) {
    Handle(DDocStd_DrawDocument) aDrawDoc =
      Handle(DDocStd_DrawDocument)::DownCast(Draw::Get(a[1]));
    if(aDrawDoc.IsNull()) {
      di << "Error   : wrong document name\n";
      return 1;
    }
    sMultiTransactionManager->AddDocument(aDrawDoc->GetDocument());
  }
  else {
    di << "Error   : document name is not defined\n";
    return 1;
  }
  return 0;
}

//=======================================================================
//function : mtmOpenTransaction
//purpose  : opens new transaction
//=======================================================================

static int mtmOpenTransaction (Draw_Interpretor& di, int /*n*/, const char** /*a*/)
{
  if(sMultiTransactionManager.IsNull()) {
    di << "Error   : manager is not initialised\n";
    return 1;
  }
  sMultiTransactionManager->OpenCommand();
  return 0;
}

//=======================================================================
//function : mtmCommitTransaction
//purpose  : commits last opened transaction
//=======================================================================

static int mtmCommitTransaction (Draw_Interpretor& di, int n, const char** a)
{
  if(sMultiTransactionManager.IsNull()) {
    di << "Error   : manager is not initialised\n";
    return 1;
  }
  if(n > 1)
    sMultiTransactionManager->CommitCommand(a[1]);
  else
    sMultiTransactionManager->CommitCommand();
  return 0;
}

//=======================================================================
//function : mtmAbortTransaction
//purpose  : aborts last opened transaction
//=======================================================================

static int mtmAbortTransaction (Draw_Interpretor& di, int /*n*/, const char** /*a*/)
{
  if(sMultiTransactionManager.IsNull()) {
    di << "Error   : manager is not initialised\n";
    return 1;
  }
  sMultiTransactionManager->AbortCommand();
  return 0;
}

//=======================================================================
//function : mtmDump
//purpose  : dumps state of the multiple transaction manager
//=======================================================================

static int mtmDump (Draw_Interpretor& di, int /*n*/, const char** /*a*/)
{
  if(sMultiTransactionManager.IsNull()) {
    di << "Error   : manager is not initialised\n";
    return 1;
  }
  di <<         "*** Dump of MTM ***\n";
  //sMultiTransactionManager->DumpTransaction(cout);
  Standard_SStream aStream;
  sMultiTransactionManager->DumpTransaction(aStream);
  di << aStream;
  di << "\n***     End     ***\n";
  return 0;
}

//=======================================================================
//function : mtmUndo
//purpose  : undos last transaction
//=======================================================================

static int mtmUndo (Draw_Interpretor& di, int /*n*/, const char** /*a*/)
{
  if(sMultiTransactionManager.IsNull()) {
    di << "Error   : manager is not initialised\n";
    return 1;
  }
  sMultiTransactionManager->Undo();
  return 0;
}

//=======================================================================
//function : mtmRedo
//purpose  : redos last transaction
//=======================================================================

static int mtmRedo (Draw_Interpretor& di, int /*n*/, const char** /*a*/)
{
  if(sMultiTransactionManager.IsNull()) {
    di << "Error   : manager is not initialised\n";
    return 1;
  }
  sMultiTransactionManager->Redo();
  return 0;
}

//=======================================================================
//function : mtmNestedMode
//purpose  : redos last transaction
//=======================================================================

static int mtmNestedMode (Draw_Interpretor& di, int n, const char** a)
{
  if(sMultiTransactionManager.IsNull()) {
    di << "Error   : manager is not initialised\n";
    return 1;
  }
  Standard_Boolean aMode = Standard_False;
  if(n > 1) {
    aMode = Draw::Atoi(a[1]) ? Standard_True : Standard_False;
  }
  sMultiTransactionManager->SetNestedTransactionMode(aMode);
  return 0;
}

//=======================================================================
//function : XAttributeValue
//purpose  : 
//=======================================================================

static Standard_Integer XAttributeValue (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if ( argc <4 ) { di << "ERROR: Too few args\n"; return 0; }
  Handle(DDF_Browser) browser =
    Handle(DDF_Browser)::DownCast (Draw::Get(argv[1], Standard_True)); 
  if ( browser.IsNull() ) { di << "ERROR: Not a browser: " << argv[1] << "\n"; return 0; }

  TDF_Label lab;
  TDF_Tool::Label(browser->Data(),argv[2],lab);
  if ( lab.IsNull() ) { di << "ERROR: label is Null: " << argv[2] << "\n"; return 0; }
  
  Standard_Integer num = Draw::Atoi ( argv[3] );
  TDF_AttributeIterator itr(lab,Standard_False);
  for (Standard_Integer i=1; itr.More() && i < num; i++) itr.Next();
  
  if ( ! itr.More() ) { di << "ERROR: Attribute #" << num << " not found\n"; return 0; }
    
  const Handle(TDF_Attribute)& att = itr.Value();
  if ( att->IsKind(STANDARD_TYPE(TDataStd_TreeNode)) )
  {
    Handle(TDataStd_TreeNode) TN = Handle(TDataStd_TreeNode)::DownCast(att);
    TCollection_AsciiString ref;
    if ( TN->HasFather() ) 
    {
      TDF_Tool::Entry ( TN->Father()->Label(), ref );
      di << " ==> " << ref.ToCString();
    }
    else 
    {
      di << " <== (" << ref.ToCString();
      Handle(TDataStd_TreeNode) child = TN->First();
      while ( ! child.IsNull() ) 
      {
        TDF_Tool::Entry ( child->Label(), ref );
        if ( child != TN->First() ) di << ", ";
        di << ref.ToCString();
        child = child->Next();
      }
      di << ")";
    }
  }
  else if ( att->IsKind(STANDARD_TYPE(TDF_Reference)) )
  {
    Handle(TDF_Reference) val = Handle(TDF_Reference)::DownCast ( att );
    TCollection_AsciiString ref;
    TDF_Tool::Entry ( val->Get(), ref );
    di << "==> " << ref.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_Integer)) ) 
  {
    Handle(TDataStd_Integer) val = Handle(TDataStd_Integer)::DownCast ( att );
    TCollection_AsciiString str ( val->Get() );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_Real)) ) 
  {
    Handle(TDataStd_Real) val = Handle(TDataStd_Real)::DownCast ( att );
    TCollection_AsciiString str ( val->Get() );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_Name)) )
  {
    Handle(TDataStd_Name) val = Handle(TDataStd_Name)::DownCast ( att );
    TCollection_AsciiString str ( val->Get(), '?' );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_Comment)) )
  {
    Handle(TDataStd_Comment) val = Handle(TDataStd_Comment)::DownCast ( att );
    TCollection_AsciiString str ( val->Get(), '?' );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_AsciiString)) )
  {
    Handle(TDataStd_AsciiString) val = Handle(TDataStd_AsciiString)::DownCast ( att );
    TCollection_AsciiString str ( val->Get(), '?' );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_IntegerArray)) )
  {
    Handle(TDataStd_IntegerArray) val = Handle(TDataStd_IntegerArray)::DownCast ( att );
    for ( Standard_Integer j=val->Lower(); j <= val->Upper(); j++ )
    {
      if ( j > val->Lower() ) di << ", ";
      TCollection_AsciiString str ( val->Value(j) );
      di << str.ToCString();
    }
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_RealArray)) )
  {
    Handle(TDataStd_RealArray) val = Handle(TDataStd_RealArray)::DownCast ( att );
    for ( Standard_Integer j=val->Lower(); j <= val->Upper(); j++ ) 
    {
      if ( j > val->Lower() ) di << ", ";
      TCollection_AsciiString str ( val->Value(j) );
      di << str.ToCString();
    }
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_ByteArray)) )
  {
    Handle(TDataStd_ByteArray) val = Handle(TDataStd_ByteArray)::DownCast ( att );
    for ( Standard_Integer j=val->Lower(); j <= val->Upper(); j++ ) 
    {
      if ( j > val->Lower() ) di << ", ";
      TCollection_AsciiString str ( val->Value(j) );
      di << str.ToCString();
    }
  }
  else if ( att->IsKind(STANDARD_TYPE(TNaming_NamedShape)) )
  {
    Handle(TNaming_NamedShape) val = Handle(TNaming_NamedShape)::DownCast ( att );
    TopoDS_Shape S = val->Get();
    di << S.TShape()->DynamicType()->Name();
    if ( ! S.Location().IsIdentity() ) di << "(located)";
  }
    
  return 0;
}

//=======================================================================
//function : mtmRemoveDocument
//purpose  : removes a document from the transactions manager
//=======================================================================

static int mtmRemoveDocument (Draw_Interpretor& di, int n, const char** a)
{
  if(sMultiTransactionManager.IsNull()) {
    di << "Error   : manager is not initialised\n";
    return 1;
  }
  if(n > 1) {
    Handle(DDocStd_DrawDocument) aDrawDoc =
       Handle(DDocStd_DrawDocument)::DownCast(Draw::Get(a[1]));
    if(aDrawDoc.IsNull()) {
      di << "Error   : wrong document name\n";
      return 1;
    }
    sMultiTransactionManager->RemoveDocument(aDrawDoc->GetDocument());
  }
  else {
    di << "Error   : document name is not defined\n";
    return 1;
  }
  return 0;
}

//=======================================================================
//function : MTMCommands
//purpose  : 
//=======================================================================

void DDocStd::MTMCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  const char* g = "MTM test commands";

  theCommands.Add("mtmCreate",
                  "\t [undo limit]         creates new new multiple transactions' manager",
                  __FILE__, mtmCreate, g);

  theCommands.Add("mtmAdd",
                  "\t <document name>      adds a document to the transactions' manager",
                  __FILE__, mtmAddDocument, g);

  theCommands.Add("mtmRemove",
                  "\t <document name>      removes a document from the transactions' manager",
                  __FILE__, mtmRemoveDocument, g);

  theCommands.Add("mtmOpen",
                  "\t                      opens new transaction",
                  __FILE__, mtmOpenTransaction, g);

  theCommands.Add("mtmCommit",
                  "\t [<transaction name>] commits last opened transaction",
                  __FILE__, mtmCommitTransaction, g);

  theCommands.Add("mtmAbort",
                  "\t                      aborts last opened transaction",
                  __FILE__, mtmAbortTransaction, g);

  theCommands.Add("mtmDump",
                  "\t                      dumps state of the multiple transactions' manager",
                  __FILE__, mtmDump, g);

  theCommands.Add("mtmUndo",
                  "\t                      undos last transaction",
                  __FILE__, mtmUndo, g);

  theCommands.Add("mtmRedo",
                  "\t                      redos last transaction",
                  __FILE__, mtmRedo, g);

  theCommands.Add("mtmNestedMode",
                  "\t [0/1]                sets nested mode if 1 and usets if 0 (default 0)",
                  __FILE__, mtmNestedMode, g);

  theCommands.Add ("XAttributeValue", "Doc label #attribute: internal command for browser",
                   __FILE__, XAttributeValue, g);
  
}
