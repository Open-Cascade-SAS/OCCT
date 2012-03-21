// Created on: 1999-05-21
// Created by: Denis PASCAL
// Copyright (c) 1999-1999 Matra Datavision
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



#include <TDocStd_XLinkTool.ixx>

// standard copy
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_LabelList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_Tool.hxx>
#include <TDF_IDList.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_ClosureMode.hxx>
#include <TDF_ClosureTool.hxx>
#include <TDF_CopyTool.hxx>
#include <TDF_ComparisonTool.hxx>
#include <TDF_Reference.hxx>
#include <TDocStd_XLink.hxx> 
#include <TDataStd_TreeNode.hxx>

#include <Standard_DomainError.hxx>


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
      Standard_DomainError::Raise("TDocStd_XLinkTool::Copy : not self-contained");
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
    Standard_DomainError::Raise(" TDocStd_CopyWithLink : already a ref");
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
    Standard_DomainError::Raise(" TDocStd_XLinkTool::UpdateLink : not ref registred");
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
