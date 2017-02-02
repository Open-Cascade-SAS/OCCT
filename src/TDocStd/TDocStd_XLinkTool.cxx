// Created on: 1999-05-21
// Created by: Denis PASCAL
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Standard_DomainError.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_ClosureMode.hxx>
#include <TDF_ClosureTool.hxx>
#include <TDF_ComparisonTool.hxx>
#include <TDF_CopyTool.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_IDList.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelList.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_Reference.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_XLink.hxx>
#include <TDocStd_XLinkTool.hxx>

// standard copy
//=======================================================================
//function : TDocStd_XLinkTool
//purpose  : 
//=======================================================================
TDocStd_XLinkTool::TDocStd_XLinkTool () {
  isDone = Standard_False;
  myRT = new TDF_RelocationTable();
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

void TDocStd_XLinkTool::Copy (const TDF_Label& target,
			   const TDF_Label& source)
{
  Handle(TDocStd_Document) TARGET,SOURCE;
  TARGET = TDocStd_Document::Get(target);  
  SOURCE = TDocStd_Document::Get(source);
  if (TARGET != SOURCE) {
    if (!TDF_Tool::IsSelfContained(source)) {
      throw Standard_DomainError("TDocStd_XLinkTool::Copy : not self-contained");
    }
  }


  // Remove TreeNode, then resotre, if present
  Handle(TDataStd_TreeNode) aFather, aPrev, aNext;
  Handle(TDataStd_TreeNode) anOldFather, anOldPrev, anOldNext;
  Handle(TDataStd_TreeNode) aNode, anOldNode;
  if(TDataStd_TreeNode::Find(source, aNode)) {
    aFather = aNode->Father();
    aPrev = aNode->Previous();
    aNext = aNode->Next();
    aNode->Remove();
  }
  if(TDataStd_TreeNode::Find(target, anOldNode)) {
    anOldFather = anOldNode->Father();
    anOldPrev = anOldNode->Previous();
    anOldNext = anOldNode->Next();
    anOldNode->Remove();
  }

  myRT = new TDF_RelocationTable(Standard_True);  
  myDS = new TDF_DataSet;
  Handle(TDF_DataSet) DS = new TDF_DataSet();   
  TDF_ClosureMode mode(Standard_True); // descendant plus reference
  myDS->AddLabel(source);
  myRT->SetRelocation(source,target);
  TDF_IDFilter filter (Standard_False); // on prend tout
  TDF_ClosureTool::Closure(myDS,filter,mode);
  TDF_CopyTool::Copy(myDS,myRT);   
  //TopTools_DataMapOfShapeShape M; // removed to avoid dependence with TNaming
  //TNaming::ChangeShapes(target,M);// should be used as postfix after Copy

  if(!aNode.IsNull()) {    
    if(!aPrev.IsNull())
      aPrev->InsertAfter(aNode);
    else if(!aNext.IsNull()) {
      aNext->InsertBefore(aNode);
    } else if (!aFather.IsNull())
      aNode->SetFather(aFather);
  }

  if(!anOldNode.IsNull()) {
    if(TDataStd_TreeNode::Find(target, anOldNode)) {
      if(!anOldPrev.IsNull())
	anOldPrev->InsertAfter(anOldNode);
      else if(!anOldNext.IsNull()) {
	anOldNext->InsertBefore(anOldNode);
      } else if (!anOldFather.IsNull())
	anOldNode->SetFather(anOldFather);
    }
  }

  isDone = Standard_True;
}

//=======================================================================
//function : CopyWithLink
//purpose  : 
//=======================================================================

void TDocStd_XLinkTool::CopyWithLink (const TDF_Label& target,
				   const TDF_Label& source)
{  
  Handle(TDF_Reference) REF;
  if (target.FindAttribute(TDF_Reference::GetID(),REF)) {
    throw Standard_DomainError(" TDocStd_CopyWithLink : already a ref");
  }
  Copy(target,source);
  if (isDone) {
    TCollection_AsciiString xlabelentry, xdocentry;
    TDF_Tool::Entry(source,xlabelentry);
    Handle(TDocStd_Document) aSourceD = TDocStd_Document::Get(source);
    Handle(TDocStd_Document) aTargetD = TDocStd_Document::Get(target);
    Standard_Integer aDocEntry = 0;
    if(aSourceD != aTargetD)
      aDocEntry = aTargetD->CreateReference(aSourceD);
    xdocentry = aDocEntry;

    Handle(TDocStd_XLink) X =  TDocStd_XLink::Set(target);
    X->LabelEntry(xlabelentry);
    X->DocumentEntry(xdocentry);
    TDF_Reference::Set(target,source);
    isDone = Standard_True;
  }
}


//=======================================================================
//function : UpdateLink
//purpose  : 
//=======================================================================

void TDocStd_XLinkTool::UpdateLink (const TDF_Label& label)
{
  Handle(TDF_Reference) REF;
  if (!label.FindAttribute(TDF_Reference::GetID(),REF)) {
    throw Standard_DomainError(" TDocStd_XLinkTool::UpdateLink : not ref registred");
  }
  TDocStd_XLinkTool XLinkTool;
  Copy (label,REF->Get());
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean TDocStd_XLinkTool::IsDone () const 
{
  return isDone;
}

//=======================================================================
//function : RelocationTable
//purpose  : 
//=======================================================================

Handle(TDF_RelocationTable) TDocStd_XLinkTool::RelocationTable () const 
{
  return myRT;
}

//=======================================================================
//function : DataSet
//purpose  : 
//=======================================================================

Handle(TDF_DataSet) TDocStd_XLinkTool::DataSet () const 
{
  return myDS;
}
