// Created on: 2016-10-19
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Label.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_View.hxx>
#include <XCAFDoc_ViewTool.hxx>


IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_ViewTool, TDF_Attribute)

//=======================================================================
//function : XCAFDoc_ViewTool
//purpose  : 
//=======================================================================
XCAFDoc_ViewTool::XCAFDoc_ViewTool()
{
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(XCAFDoc_ViewTool) XCAFDoc_ViewTool::Set(const TDF_Label& L) 
{
  Handle(XCAFDoc_ViewTool) A;
  if (!L.FindAttribute (XCAFDoc_ViewTool::GetID(), A)) {
    A = new XCAFDoc_ViewTool ();
    L.AddAttribute(A);
  }
  return A;
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& XCAFDoc_ViewTool::GetID() 
{
  static Standard_GUID ViewToolID ("efd213e4-6dfd-11d4-b9c8-0060b0ee281b");
  return ViewToolID; 
}


//=======================================================================
//function : BaseLabel
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_ViewTool::BaseLabel() const
{
  return Label();
}

//=======================================================================
//function : IsView
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDoc_ViewTool::IsView(const TDF_Label& theLabel) const
{
  Handle(XCAFDoc_View) aViewAttr;
  if(theLabel.FindAttribute(XCAFDoc_View::GetID(), aViewAttr)) {
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : GetViewLabels
//purpose  : 
//=======================================================================
void XCAFDoc_ViewTool::GetViewLabels(TDF_LabelSequence& theLabels) const
{
  theLabels.Clear();
  TDF_ChildIterator aChildIterator( Label() ); 
  for (; aChildIterator.More(); aChildIterator.Next()) {
    TDF_Label aLabel = aChildIterator.Value();
    if ( IsView(aLabel)) theLabels.Append(aLabel);
  }
}

//=======================================================================
//function : AddView
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_ViewTool::AddView()
{
  TDF_Label aViewL;
  TDF_TagSource aTag;
  aViewL = aTag.NewChild ( Label() );
  Handle(XCAFDoc_View) aView = XCAFDoc_View::Set(aViewL);
  TCollection_AsciiString aStr = "View";
  TDataStd_Name::Set(aViewL, aStr);
  return aViewL;
}

//=======================================================================
//function : SetView
//purpose  : 
//=======================================================================
void XCAFDoc_ViewTool::SetView(const TDF_LabelSequence& theShapeLabels,
                               const TDF_LabelSequence& theGDTLabels,
                               const TDF_Label& theViewL) const
{
  if(!IsView(theViewL))
    return;

  Handle(XCAFDoc_GraphNode) aChGNode;
  Handle(XCAFDoc_GraphNode) aShapeGNode;
  Handle(XCAFDoc_GraphNode) aGDTGNode;
  
  if ( theViewL.FindAttribute (XCAFDoc::ViewRefShapeGUID(), aChGNode) ) {
    while (aChGNode->NbFathers() > 0) {
      aShapeGNode = aChGNode->GetFather(1);
      aShapeGNode->UnSetChild(aChGNode);
      if(aShapeGNode->NbChildren() == 0)
        aShapeGNode->ForgetAttribute( XCAFDoc::ViewRefShapeGUID() );
    }
    theViewL.ForgetAttribute ( XCAFDoc::ViewRefShapeGUID() );
  }
  if ( theViewL.FindAttribute (XCAFDoc::ViewRefGDTGUID(), aChGNode) ) {
    while (aChGNode->NbFathers() > 0) {
      aShapeGNode = aChGNode->GetFather(1);
      aShapeGNode->UnSetChild(aChGNode);
      if(aShapeGNode->NbChildren() == 0)
        aShapeGNode->ForgetAttribute( XCAFDoc::ViewRefGDTGUID() );
    }
    theViewL.ForgetAttribute ( XCAFDoc::ViewRefGDTGUID() );
  }
  
  if (!theViewL.FindAttribute(XCAFDoc::ViewRefShapeGUID(), aChGNode) && theShapeLabels.Length() > 0) {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefShapeGUID());
  }
  for(Standard_Integer i = theShapeLabels.Lower(); i <= theShapeLabels.Upper(); i++)
  {
    if (!theShapeLabels.Value(i).FindAttribute(XCAFDoc::ViewRefShapeGUID(), aShapeGNode) ) {
      aShapeGNode = new XCAFDoc_GraphNode;
      aShapeGNode = XCAFDoc_GraphNode::Set(theShapeLabels.Value(i));
    }
    aShapeGNode->SetGraphID(XCAFDoc::ViewRefShapeGUID());
    aShapeGNode->SetChild(aChGNode);
    aChGNode->SetFather(aShapeGNode);
  }
  
  if (!theViewL.FindAttribute(XCAFDoc::ViewRefGDTGUID(), aChGNode) && theGDTLabels.Length() > 0) {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefGDTGUID());
  }
  for(Standard_Integer i = theGDTLabels.Lower(); i <= theGDTLabels.Upper(); i++)
  {
    if(!theGDTLabels.Value(i).FindAttribute(XCAFDoc::ViewRefGDTGUID(), aGDTGNode) ) {
      aGDTGNode = new XCAFDoc_GraphNode;
      aGDTGNode = XCAFDoc_GraphNode::Set(theGDTLabels.Value(i));
    }
    aGDTGNode->SetGraphID(XCAFDoc::ViewRefGDTGUID());
    aGDTGNode->SetChild(aChGNode);
    aChGNode->SetFather(aGDTGNode);
  }
}

//=======================================================================
//function : GetRefShapeLabel
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDoc_ViewTool::GetRefShapeLabel(const TDF_Label& theViewL,
                                                    TDF_LabelSequence& theShapeLabels) const
{
  theShapeLabels.Clear();
  Handle(TDataStd_TreeNode) aNode;
  if( !theViewL.FindAttribute(XCAFDoc::ViewRefGUID(), aNode) || !aNode->HasFather() ) {
    Handle(XCAFDoc_GraphNode) aGNode;
    if( theViewL.FindAttribute(XCAFDoc::ViewRefShapeGUID(), aGNode) && aGNode->NbFathers() > 0 ) {
      for(Standard_Integer i = 1; i <= aGNode->NbFathers(); i++)
        theShapeLabels.Append(aGNode->GetFather(i)->Label());
      return Standard_True;
    }
    else 
      return Standard_False;
  }

  theShapeLabels.Append(aNode->Father()->Label());
  return Standard_True;
}

//=======================================================================
//function : GetRefGDTLabel
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDoc_ViewTool::GetRefGDTLabel(const TDF_Label& theViewL,
                                                  TDF_LabelSequence& theGDTLabels) const
{
  theGDTLabels.Clear();
  Handle(TDataStd_TreeNode) aNode;
  if( !theViewL.FindAttribute(XCAFDoc::ViewRefGUID(), aNode) || !aNode->HasFather() ) {
    Handle(XCAFDoc_GraphNode) aGNode;
    if( theViewL.FindAttribute(XCAFDoc::ViewRefGDTGUID(), aGNode) && aGNode->NbFathers() > 0 ) {
      for(Standard_Integer i = 1; i <= aGNode->NbFathers(); i++)
        theGDTLabels.Append(aGNode->GetFather(i)->Label());
      return Standard_True;
    }
    else 
      return Standard_False;
  }

  theGDTLabels.Append(aNode->Father()->Label());
  return Standard_True;
}

//=======================================================================
//function : GetViewLabelsForShape
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDoc_ViewTool::GetViewLabelsForShape(const TDF_Label& theShapeL,
                                                         TDF_LabelSequence& theViews) const
{
  Handle(XCAFDoc_GraphNode) aGNode;
  Standard_Boolean aResult = Standard_False;
  if (theShapeL.FindAttribute(XCAFDoc::ViewRefShapeGUID(), aGNode) && aGNode->NbChildren() > 0) {
    for(Standard_Integer i = 1; i <= aGNode->NbChildren(); i++)
    {
      theViews.Append(aGNode->GetChild(i)->Label());
    }
    aResult = Standard_True;
  }
  return aResult;
}

//=======================================================================
//function : GetViewLabelsForGDT
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDoc_ViewTool::GetViewLabelsForGDT(const TDF_Label& theGDTL,
                                                       TDF_LabelSequence& theViews) const
{
  Handle(XCAFDoc_GraphNode) aGNode;
  Standard_Boolean aResult = Standard_False;
  if (theGDTL.FindAttribute(XCAFDoc::ViewRefGDTGUID(), aGNode) && aGNode->NbChildren() > 0) {
    for(Standard_Integer i = 1; i <= aGNode->NbChildren(); i++)
    {
      theViews.Append(aGNode->GetChild(i)->Label());
    }
    aResult = Standard_True;
  }
  return aResult;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& XCAFDoc_ViewTool::ID() const
{
  return GetID();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void XCAFDoc_ViewTool::Restore(const Handle(TDF_Attribute)& /*with*/) 
{
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XCAFDoc_ViewTool::NewEmpty() const
{
  return new XCAFDoc_ViewTool;
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XCAFDoc_ViewTool::Paste(const Handle(TDF_Attribute)& /*into*/,
                             const Handle(TDF_RelocationTable)& /*RT*/) const
{
}

