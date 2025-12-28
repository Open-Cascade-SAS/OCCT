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

#include <XCAFDoc_ViewTool.hxx>

#include <Standard_Type.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Label.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_View.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(XCAFDoc_ViewTool, TDataStd_GenericEmpty, "xcaf", "ViewTool")

//=================================================================================================

XCAFDoc_ViewTool::XCAFDoc_ViewTool() = default;

//=================================================================================================

occ::handle<XCAFDoc_ViewTool> XCAFDoc_ViewTool::Set(const TDF_Label& L)
{
  occ::handle<XCAFDoc_ViewTool> A;
  if (!L.FindAttribute(XCAFDoc_ViewTool::GetID(), A))
  {
    A = new XCAFDoc_ViewTool();
    L.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_ViewTool::GetID()
{
  static Standard_GUID ViewToolID("efd213e4-6dfd-11d4-b9c8-0060b0ee281b");
  return ViewToolID;
}

//=================================================================================================

TDF_Label XCAFDoc_ViewTool::BaseLabel() const
{
  return Label();
}

//=================================================================================================

bool XCAFDoc_ViewTool::IsView(const TDF_Label& theLabel) const
{
  occ::handle<XCAFDoc_View> aViewAttr;
  if (theLabel.FindAttribute(XCAFDoc_View::GetID(), aViewAttr))
  {
    return true;
  }
  return false;
}

//=================================================================================================

void XCAFDoc_ViewTool::GetViewLabels(NCollection_Sequence<TDF_Label>& theLabels) const
{
  theLabels.Clear();
  TDF_ChildIterator aChildIterator(Label());
  for (; aChildIterator.More(); aChildIterator.Next())
  {
    TDF_Label aLabel = aChildIterator.Value();
    if (IsView(aLabel))
      theLabels.Append(aLabel);
  }
}

//=================================================================================================

TDF_Label XCAFDoc_ViewTool::AddView()
{
  TDF_Label     aViewL;
  TDF_TagSource aTag;
  aViewL                          = aTag.NewChild(Label());
  occ::handle<XCAFDoc_View> aView = XCAFDoc_View::Set(aViewL);
  TCollection_AsciiString   aStr  = "View";
  TDataStd_Name::Set(aViewL, aStr);
  return aViewL;
}

//=================================================================================================

void XCAFDoc_ViewTool::SetView(const NCollection_Sequence<TDF_Label>& theShapes,
                               const NCollection_Sequence<TDF_Label>& theGDTs,
                               const NCollection_Sequence<TDF_Label>& theClippingPlanes,
                               const NCollection_Sequence<TDF_Label>& theNotes,
                               const NCollection_Sequence<TDF_Label>& theAnnotations,
                               const TDF_Label&                       theViewL) const
{
  if (!IsView(theViewL))
    return;

  occ::handle<XCAFDoc_GraphNode> aChGNode;
  occ::handle<XCAFDoc_GraphNode> aShapeGNode;
  occ::handle<XCAFDoc_GraphNode> aGDTGNode;
  occ::handle<XCAFDoc_GraphNode> aPlaneGNode;
  occ::handle<XCAFDoc_GraphNode> aNoteGNode;
  occ::handle<XCAFDoc_GraphNode> aAnnotGNode;

  if (theViewL.FindAttribute(XCAFDoc::ViewRefShapeGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aShapeGNode = aChGNode->GetFather(1);
      aShapeGNode->UnSetChild(aChGNode);
      if (aShapeGNode->NbChildren() == 0)
        aShapeGNode->ForgetAttribute(XCAFDoc::ViewRefShapeGUID());
    }
    theViewL.ForgetAttribute(XCAFDoc::ViewRefShapeGUID());
  }
  if (theViewL.FindAttribute(XCAFDoc::ViewRefGDTGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aGDTGNode = aChGNode->GetFather(1);
      aGDTGNode->UnSetChild(aChGNode);
      if (aGDTGNode->NbChildren() == 0)
        aGDTGNode->ForgetAttribute(XCAFDoc::ViewRefGDTGUID());
    }
    theViewL.ForgetAttribute(XCAFDoc::ViewRefGDTGUID());
  }
  if (theViewL.FindAttribute(XCAFDoc::ViewRefPlaneGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aPlaneGNode = aChGNode->GetFather(1);
      aPlaneGNode->UnSetChild(aChGNode);
      if (aPlaneGNode->NbChildren() == 0)
        aPlaneGNode->ForgetAttribute(XCAFDoc::ViewRefGDTGUID());
    }
    theViewL.ForgetAttribute(XCAFDoc::ViewRefPlaneGUID());
  }
  if (theViewL.FindAttribute(XCAFDoc::ViewRefNoteGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aPlaneGNode = aChGNode->GetFather(1);
      aPlaneGNode->UnSetChild(aChGNode);
      if (aPlaneGNode->NbChildren() == 0)
        aPlaneGNode->ForgetAttribute(XCAFDoc::ViewRefNoteGUID());
    }
    theViewL.ForgetAttribute(XCAFDoc::ViewRefNoteGUID());
  }
  if (theViewL.FindAttribute(XCAFDoc::ViewRefAnnotationGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aPlaneGNode = aChGNode->GetFather(1);
      aPlaneGNode->UnSetChild(aChGNode);
      if (aPlaneGNode->NbChildren() == 0)
        aPlaneGNode->ForgetAttribute(XCAFDoc::ViewRefAnnotationGUID());
    }
    theViewL.ForgetAttribute(XCAFDoc::ViewRefAnnotationGUID());
  }

  if (!theViewL.FindAttribute(XCAFDoc::ViewRefShapeGUID(), aChGNode) && theShapes.Length() > 0)
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefShapeGUID());
  }
  for (int i = theShapes.Lower(); i <= theShapes.Upper(); i++)
  {
    if (!theShapes.Value(i).FindAttribute(XCAFDoc::ViewRefShapeGUID(), aShapeGNode))
    {
      aShapeGNode = new XCAFDoc_GraphNode;
      aShapeGNode = XCAFDoc_GraphNode::Set(theShapes.Value(i));
    }
    aShapeGNode->SetGraphID(XCAFDoc::ViewRefShapeGUID());
    aShapeGNode->SetChild(aChGNode);
    aChGNode->SetFather(aShapeGNode);
  }

  if (!theViewL.FindAttribute(XCAFDoc::ViewRefGDTGUID(), aChGNode) && theGDTs.Length() > 0)
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefGDTGUID());
  }
  for (int i = theGDTs.Lower(); i <= theGDTs.Upper(); i++)
  {
    if (!theGDTs.Value(i).FindAttribute(XCAFDoc::ViewRefGDTGUID(), aGDTGNode))
    {
      aGDTGNode = new XCAFDoc_GraphNode;
      aGDTGNode = XCAFDoc_GraphNode::Set(theGDTs.Value(i));
    }
    aGDTGNode->SetGraphID(XCAFDoc::ViewRefGDTGUID());
    aGDTGNode->SetChild(aChGNode);
    aChGNode->SetFather(aGDTGNode);
  }

  if (!theViewL.FindAttribute(XCAFDoc::ViewRefPlaneGUID(), aChGNode)
      && theClippingPlanes.Length() > 0)
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefPlaneGUID());
  }
  for (int i = theClippingPlanes.Lower(); i <= theClippingPlanes.Upper(); i++)
  {
    if (!theClippingPlanes.Value(i).FindAttribute(XCAFDoc::ViewRefPlaneGUID(), aPlaneGNode))
    {
      aPlaneGNode = new XCAFDoc_GraphNode;
      aPlaneGNode = XCAFDoc_GraphNode::Set(theClippingPlanes.Value(i));
    }
    aPlaneGNode->SetGraphID(XCAFDoc::ViewRefPlaneGUID());
    aPlaneGNode->SetChild(aChGNode);
    aChGNode->SetFather(aPlaneGNode);
  }

  if (!theViewL.FindAttribute(XCAFDoc::ViewRefNoteGUID(), aChGNode) && theNotes.Length() > 0)
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefPlaneGUID());
  }
  for (int i = theNotes.Lower(); i <= theNotes.Upper(); i++)
  {
    if (!theNotes.Value(i).FindAttribute(XCAFDoc::ViewRefNoteGUID(), aNoteGNode))
    {
      aNoteGNode = new XCAFDoc_GraphNode;
      aNoteGNode = XCAFDoc_GraphNode::Set(theNotes.Value(i));
    }
    aNoteGNode->SetGraphID(XCAFDoc::ViewRefNoteGUID());
    aNoteGNode->SetChild(aChGNode);
    aChGNode->SetFather(aNoteGNode);
  }

  if (!theViewL.FindAttribute(XCAFDoc::ViewRefAnnotationGUID(), aChGNode)
      && theAnnotations.Length() > 0)
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefAnnotationGUID());
  }
  for (int i = theAnnotations.Lower(); i <= theAnnotations.Upper(); i++)
  {
    if (!theAnnotations.Value(i).FindAttribute(XCAFDoc::ViewRefAnnotationGUID(), aNoteGNode))
    {
      aAnnotGNode = new XCAFDoc_GraphNode;
      aAnnotGNode = XCAFDoc_GraphNode::Set(theNotes.Value(i));
    }
    aAnnotGNode->SetGraphID(XCAFDoc::ViewRefAnnotationGUID());
    aAnnotGNode->SetChild(aChGNode);
    aChGNode->SetFather(aAnnotGNode);
  }
}

//=================================================================================================

void XCAFDoc_ViewTool::SetView(const NCollection_Sequence<TDF_Label>& theShapeLabels,
                               const NCollection_Sequence<TDF_Label>& theGDTLabels,
                               const NCollection_Sequence<TDF_Label>& theClippingPlaneLabels,
                               const TDF_Label&                       theViewL) const
{
  if (!IsView(theViewL))
    return;

  occ::handle<XCAFDoc_GraphNode> aChGNode;
  occ::handle<XCAFDoc_GraphNode> aShapeGNode;
  occ::handle<XCAFDoc_GraphNode> aGDTGNode;
  occ::handle<XCAFDoc_GraphNode> aPlaneGNode;

  if (theViewL.FindAttribute(XCAFDoc::ViewRefShapeGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aShapeGNode = aChGNode->GetFather(1);
      aShapeGNode->UnSetChild(aChGNode);
      if (aShapeGNode->NbChildren() == 0)
        aShapeGNode->ForgetAttribute(XCAFDoc::ViewRefShapeGUID());
    }
    theViewL.ForgetAttribute(XCAFDoc::ViewRefShapeGUID());
  }
  if (theViewL.FindAttribute(XCAFDoc::ViewRefGDTGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aGDTGNode = aChGNode->GetFather(1);
      aGDTGNode->UnSetChild(aChGNode);
      if (aGDTGNode->NbChildren() == 0)
        aGDTGNode->ForgetAttribute(XCAFDoc::ViewRefGDTGUID());
    }
    theViewL.ForgetAttribute(XCAFDoc::ViewRefGDTGUID());
  }
  if (theViewL.FindAttribute(XCAFDoc::ViewRefPlaneGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aPlaneGNode = aChGNode->GetFather(1);
      aPlaneGNode->UnSetChild(aChGNode);
      if (aPlaneGNode->NbChildren() == 0)
        aPlaneGNode->ForgetAttribute(XCAFDoc::ViewRefGDTGUID());
    }
    theViewL.ForgetAttribute(XCAFDoc::ViewRefPlaneGUID());
  }

  if (!theViewL.FindAttribute(XCAFDoc::ViewRefShapeGUID(), aChGNode) && theShapeLabels.Length() > 0)
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefShapeGUID());
  }
  for (int i = theShapeLabels.Lower(); i <= theShapeLabels.Upper(); i++)
  {
    if (!theShapeLabels.Value(i).FindAttribute(XCAFDoc::ViewRefShapeGUID(), aShapeGNode))
    {
      aShapeGNode = new XCAFDoc_GraphNode;
      aShapeGNode = XCAFDoc_GraphNode::Set(theShapeLabels.Value(i));
    }
    aShapeGNode->SetGraphID(XCAFDoc::ViewRefShapeGUID());
    aShapeGNode->SetChild(aChGNode);
    aChGNode->SetFather(aShapeGNode);
  }

  if (!theViewL.FindAttribute(XCAFDoc::ViewRefGDTGUID(), aChGNode) && theGDTLabels.Length() > 0)
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefGDTGUID());
  }
  for (int i = theGDTLabels.Lower(); i <= theGDTLabels.Upper(); i++)
  {
    if (!theGDTLabels.Value(i).FindAttribute(XCAFDoc::ViewRefGDTGUID(), aGDTGNode))
    {
      aGDTGNode = new XCAFDoc_GraphNode;
      aGDTGNode = XCAFDoc_GraphNode::Set(theGDTLabels.Value(i));
    }
    aGDTGNode->SetGraphID(XCAFDoc::ViewRefGDTGUID());
    aGDTGNode->SetChild(aChGNode);
    aChGNode->SetFather(aGDTGNode);
  }

  if (!theViewL.FindAttribute(XCAFDoc::ViewRefPlaneGUID(), aChGNode)
      && theClippingPlaneLabels.Length() > 0)
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefPlaneGUID());
  }
  for (int i = theClippingPlaneLabels.Lower(); i <= theClippingPlaneLabels.Upper(); i++)
  {
    if (!theClippingPlaneLabels.Value(i).FindAttribute(XCAFDoc::ViewRefPlaneGUID(), aPlaneGNode))
    {
      aPlaneGNode = new XCAFDoc_GraphNode;
      aPlaneGNode = XCAFDoc_GraphNode::Set(theClippingPlaneLabels.Value(i));
    }
    aPlaneGNode->SetGraphID(XCAFDoc::ViewRefPlaneGUID());
    aPlaneGNode->SetChild(aChGNode);
    aChGNode->SetFather(aPlaneGNode);
  }
}

//=================================================================================================

void XCAFDoc_ViewTool::SetView(const NCollection_Sequence<TDF_Label>& theShapeLabels,
                               const NCollection_Sequence<TDF_Label>& theGDTLabels,
                               const TDF_Label&                       theViewL) const
{
  if (!IsView(theViewL))
    return;

  occ::handle<XCAFDoc_GraphNode> aChGNode;
  occ::handle<XCAFDoc_GraphNode> aShapeGNode;
  occ::handle<XCAFDoc_GraphNode> aGDTGNode;

  if (theViewL.FindAttribute(XCAFDoc::ViewRefShapeGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aShapeGNode = aChGNode->GetFather(1);
      aShapeGNode->UnSetChild(aChGNode);
      if (aShapeGNode->NbChildren() == 0)
        aShapeGNode->ForgetAttribute(XCAFDoc::ViewRefShapeGUID());
    }
    theViewL.ForgetAttribute(XCAFDoc::ViewRefShapeGUID());
  }
  if (theViewL.FindAttribute(XCAFDoc::ViewRefGDTGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aGDTGNode = aChGNode->GetFather(1);
      aGDTGNode->UnSetChild(aChGNode);
      if (aGDTGNode->NbChildren() == 0)
        aGDTGNode->ForgetAttribute(XCAFDoc::ViewRefGDTGUID());
    }
    theViewL.ForgetAttribute(XCAFDoc::ViewRefGDTGUID());
  }

  if (!theViewL.FindAttribute(XCAFDoc::ViewRefShapeGUID(), aChGNode) && theShapeLabels.Length() > 0)
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefShapeGUID());
  }
  for (int i = theShapeLabels.Lower(); i <= theShapeLabels.Upper(); i++)
  {
    if (!theShapeLabels.Value(i).FindAttribute(XCAFDoc::ViewRefShapeGUID(), aShapeGNode))
    {
      aShapeGNode = new XCAFDoc_GraphNode;
      aShapeGNode = XCAFDoc_GraphNode::Set(theShapeLabels.Value(i));
    }
    aShapeGNode->SetGraphID(XCAFDoc::ViewRefShapeGUID());
    aShapeGNode->SetChild(aChGNode);
    aChGNode->SetFather(aShapeGNode);
  }

  if (!theViewL.FindAttribute(XCAFDoc::ViewRefGDTGUID(), aChGNode) && theGDTLabels.Length() > 0)
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefGDTGUID());
  }
  for (int i = theGDTLabels.Lower(); i <= theGDTLabels.Upper(); i++)
  {
    if (!theGDTLabels.Value(i).FindAttribute(XCAFDoc::ViewRefGDTGUID(), aGDTGNode))
    {
      aGDTGNode = new XCAFDoc_GraphNode;
      aGDTGNode = XCAFDoc_GraphNode::Set(theGDTLabels.Value(i));
    }
    aGDTGNode->SetGraphID(XCAFDoc::ViewRefGDTGUID());
    aGDTGNode->SetChild(aChGNode);
    aChGNode->SetFather(aGDTGNode);
  }
}

//=================================================================================================

void XCAFDoc_ViewTool::SetClippingPlanes(
  const NCollection_Sequence<TDF_Label>& theClippingPlaneLabels,
  const TDF_Label&                       theViewL) const
{
  if (!IsView(theViewL))
    return;

  occ::handle<XCAFDoc_GraphNode> aChGNode;
  occ::handle<XCAFDoc_GraphNode> aPlaneGNode;

  if (theViewL.FindAttribute(XCAFDoc::ViewRefPlaneGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aPlaneGNode = aChGNode->GetFather(1);
      aPlaneGNode->UnSetChild(aChGNode);
      if (aPlaneGNode->NbChildren() == 0)
        aPlaneGNode->ForgetAttribute(XCAFDoc::ViewRefGDTGUID());
    }
    theViewL.ForgetAttribute(XCAFDoc::ViewRefPlaneGUID());
  }

  if (!theViewL.FindAttribute(XCAFDoc::ViewRefPlaneGUID(), aChGNode)
      && theClippingPlaneLabels.Length() > 0)
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theViewL);
    aChGNode->SetGraphID(XCAFDoc::ViewRefPlaneGUID());
  }
  for (int i = theClippingPlaneLabels.Lower(); i <= theClippingPlaneLabels.Upper(); i++)
  {
    if (!theClippingPlaneLabels.Value(i).FindAttribute(XCAFDoc::ViewRefPlaneGUID(), aPlaneGNode))
    {
      aPlaneGNode = new XCAFDoc_GraphNode;
      aPlaneGNode = XCAFDoc_GraphNode::Set(theClippingPlaneLabels.Value(i));
    }
    aPlaneGNode->SetGraphID(XCAFDoc::ViewRefPlaneGUID());
    aPlaneGNode->SetChild(aChGNode);
    aChGNode->SetFather(aPlaneGNode);
  }
}

//=================================================================================================

void XCAFDoc_ViewTool::RemoveView(const TDF_Label& theViewL)
{
  if (!IsView(theViewL))
    return;

  occ::handle<XCAFDoc_GraphNode> aChGNode;
  occ::handle<XCAFDoc_GraphNode> aShapeGNode;
  occ::handle<XCAFDoc_GraphNode> aGDTGNode;
  occ::handle<XCAFDoc_GraphNode> aPlaneGNode;

  if (theViewL.FindAttribute(XCAFDoc::ViewRefShapeGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aShapeGNode = aChGNode->GetFather(1);
      aShapeGNode->UnSetChild(aChGNode);
      if (aShapeGNode->NbChildren() == 0)
        aShapeGNode->ForgetAttribute(XCAFDoc::ViewRefShapeGUID());
    }
  }
  if (theViewL.FindAttribute(XCAFDoc::ViewRefGDTGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aGDTGNode = aChGNode->GetFather(1);
      aGDTGNode->UnSetChild(aChGNode);
      if (aGDTGNode->NbChildren() == 0)
        aGDTGNode->ForgetAttribute(XCAFDoc::ViewRefGDTGUID());
    }
  }
  if (theViewL.FindAttribute(XCAFDoc::ViewRefPlaneGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aPlaneGNode = aChGNode->GetFather(1);
      aPlaneGNode->UnSetChild(aChGNode);
      if (aPlaneGNode->NbChildren() == 0)
        aPlaneGNode->ForgetAttribute(XCAFDoc::ViewRefGDTGUID());
    }
  }
  theViewL.ForgetAllAttributes();
}

//=================================================================================================

bool XCAFDoc_ViewTool::GetRefShapeLabel(const TDF_Label&                 theViewL,
                                        NCollection_Sequence<TDF_Label>& theShapeLabels) const
{
  theShapeLabels.Clear();
  occ::handle<TDataStd_TreeNode> aNode;
  if (!theViewL.FindAttribute(XCAFDoc::ViewRefGUID(), aNode) || !aNode->HasFather())
  {
    occ::handle<XCAFDoc_GraphNode> aGNode;
    if (theViewL.FindAttribute(XCAFDoc::ViewRefShapeGUID(), aGNode) && aGNode->NbFathers() > 0)
    {
      for (int i = 1; i <= aGNode->NbFathers(); i++)
        theShapeLabels.Append(aGNode->GetFather(i)->Label());
      return true;
    }
    else
      return false;
  }

  theShapeLabels.Append(aNode->Father()->Label());
  return true;
}

//=================================================================================================

bool XCAFDoc_ViewTool::GetRefGDTLabel(const TDF_Label&                 theViewL,
                                      NCollection_Sequence<TDF_Label>& theGDTLabels) const
{
  theGDTLabels.Clear();
  occ::handle<TDataStd_TreeNode> aNode;
  if (!theViewL.FindAttribute(XCAFDoc::ViewRefGUID(), aNode) || !aNode->HasFather())
  {
    occ::handle<XCAFDoc_GraphNode> aGNode;
    if (theViewL.FindAttribute(XCAFDoc::ViewRefGDTGUID(), aGNode) && aGNode->NbFathers() > 0)
    {
      for (int i = 1; i <= aGNode->NbFathers(); i++)
        theGDTLabels.Append(aGNode->GetFather(i)->Label());
      return true;
    }
    else
      return false;
  }

  theGDTLabels.Append(aNode->Father()->Label());
  return true;
}

//=================================================================================================

bool XCAFDoc_ViewTool::GetRefClippingPlaneLabel(
  const TDF_Label&                 theViewL,
  NCollection_Sequence<TDF_Label>& theClippingPlaneLabels) const
{
  theClippingPlaneLabels.Clear();
  occ::handle<TDataStd_TreeNode> aNode;
  if (!theViewL.FindAttribute(XCAFDoc::ViewRefGUID(), aNode) || !aNode->HasFather())
  {
    occ::handle<XCAFDoc_GraphNode> aGNode;
    if (theViewL.FindAttribute(XCAFDoc::ViewRefPlaneGUID(), aGNode) && aGNode->NbFathers() > 0)
    {
      for (int i = 1; i <= aGNode->NbFathers(); i++)
        theClippingPlaneLabels.Append(aGNode->GetFather(i)->Label());
      return true;
    }
    else
      return false;
  }

  theClippingPlaneLabels.Append(aNode->Father()->Label());
  return true;
}

//=================================================================================================

bool XCAFDoc_ViewTool::GetRefNoteLabel(const TDF_Label&                 theViewL,
                                       NCollection_Sequence<TDF_Label>& theNoteLabels) const
{
  theNoteLabels.Clear();
  occ::handle<TDataStd_TreeNode> aNode;
  if (!theViewL.FindAttribute(XCAFDoc::ViewRefGUID(), aNode) || !aNode->HasFather())
  {
    occ::handle<XCAFDoc_GraphNode> aGNode;
    if (theViewL.FindAttribute(XCAFDoc::ViewRefNoteGUID(), aGNode) && aGNode->NbFathers() > 0)
    {
      for (int i = 1; i <= aGNode->NbFathers(); i++)
        theNoteLabels.Append(aGNode->GetFather(i)->Label());
      return true;
    }
    else
      return false;
  }

  theNoteLabels.Append(aNode->Father()->Label());
  return true;
}

//=================================================================================================

bool XCAFDoc_ViewTool::GetRefAnnotationLabel(
  const TDF_Label&                 theViewL,
  NCollection_Sequence<TDF_Label>& theAnnotationLabels) const
{
  theAnnotationLabels.Clear();
  occ::handle<TDataStd_TreeNode> aNode;
  if (!theViewL.FindAttribute(XCAFDoc::ViewRefGUID(), aNode) || !aNode->HasFather())
  {
    occ::handle<XCAFDoc_GraphNode> aGNode;
    if (theViewL.FindAttribute(XCAFDoc::ViewRefAnnotationGUID(), aGNode) && aGNode->NbFathers() > 0)
    {
      for (int i = 1; i <= aGNode->NbFathers(); i++)
        theAnnotationLabels.Append(aGNode->GetFather(i)->Label());
      return true;
    }
    else
      return false;
  }

  theAnnotationLabels.Append(aNode->Father()->Label());
  return true;
}

//=================================================================================================

bool XCAFDoc_ViewTool::GetViewLabelsForShape(const TDF_Label&                 theShapeL,
                                             NCollection_Sequence<TDF_Label>& theViews) const
{
  occ::handle<XCAFDoc_GraphNode> aGNode;
  bool                           aResult = false;
  if (theShapeL.FindAttribute(XCAFDoc::ViewRefShapeGUID(), aGNode) && aGNode->NbChildren() > 0)
  {
    for (int i = 1; i <= aGNode->NbChildren(); i++)
    {
      theViews.Append(aGNode->GetChild(i)->Label());
    }
    aResult = true;
  }
  return aResult;
}

//=================================================================================================

bool XCAFDoc_ViewTool::GetViewLabelsForGDT(const TDF_Label&                 theGDTL,
                                           NCollection_Sequence<TDF_Label>& theViews) const
{
  occ::handle<XCAFDoc_GraphNode> aGNode;
  bool                           aResult = false;
  if (theGDTL.FindAttribute(XCAFDoc::ViewRefGDTGUID(), aGNode) && aGNode->NbChildren() > 0)
  {
    for (int i = 1; i <= aGNode->NbChildren(); i++)
    {
      theViews.Append(aGNode->GetChild(i)->Label());
    }
    aResult = true;
  }
  return aResult;
}

//=================================================================================================

bool XCAFDoc_ViewTool::GetViewLabelsForClippingPlane(
  const TDF_Label&                 theClippingPlaneL,
  NCollection_Sequence<TDF_Label>& theViews) const
{
  occ::handle<XCAFDoc_GraphNode> aGNode;
  bool                           aResult = false;
  if (theClippingPlaneL.FindAttribute(XCAFDoc::ViewRefPlaneGUID(), aGNode)
      && aGNode->NbChildren() > 0)
  {
    for (int i = 1; i <= aGNode->NbChildren(); i++)
    {
      theViews.Append(aGNode->GetChild(i)->Label());
    }
    aResult = true;
  }
  return aResult;
}

//=================================================================================================

bool XCAFDoc_ViewTool::GetViewLabelsForNote(const TDF_Label&                 theNoteL,
                                            NCollection_Sequence<TDF_Label>& theViews) const
{
  occ::handle<XCAFDoc_GraphNode> aGNode;
  bool                           aResult = false;
  if (theNoteL.FindAttribute(XCAFDoc::ViewRefNoteGUID(), aGNode) && aGNode->NbChildren() > 0)
  {
    for (int i = 1; i <= aGNode->NbChildren(); i++)
    {
      theViews.Append(aGNode->GetChild(i)->Label());
    }
    aResult = true;
  }
  return aResult;
}

//=================================================================================================

bool XCAFDoc_ViewTool::GetViewLabelsForAnnotation(const TDF_Label&                 theAnnotationL,
                                                  NCollection_Sequence<TDF_Label>& theViews) const
{
  occ::handle<XCAFDoc_GraphNode> aGNode;
  bool                           aResult = false;
  if (theAnnotationL.FindAttribute(XCAFDoc::ViewRefAnnotationGUID(), aGNode)
      && aGNode->NbChildren() > 0)
  {
    for (int i = 1; i <= aGNode->NbChildren(); i++)
    {
      theViews.Append(aGNode->GetChild(i)->Label());
    }
    aResult = true;
  }
  return aResult;
}

//=================================================================================================

bool XCAFDoc_ViewTool::IsLocked(const TDF_Label& theViewL) const
{
  occ::handle<TDataStd_UAttribute> anAttr;
  return theViewL.FindAttribute(XCAFDoc::LockGUID(), anAttr);
}

//=================================================================================================

void XCAFDoc_ViewTool::Lock(const TDF_Label& theViewL) const
{
  TDataStd_UAttribute::Set(theViewL, XCAFDoc::LockGUID());
}

//=================================================================================================

void XCAFDoc_ViewTool::Unlock(const TDF_Label& theViewL) const
{
  theViewL.ForgetAttribute(XCAFDoc::LockGUID());
}

//=================================================================================================

const Standard_GUID& XCAFDoc_ViewTool::ID() const
{
  return GetID();
}
