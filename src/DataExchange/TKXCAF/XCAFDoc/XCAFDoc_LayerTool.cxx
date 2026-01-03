// Created on: 2000-10-02
// Created by: Pavel TELKOV
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

#include <XCAFDoc_LayerTool.hxx>

#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(XCAFDoc_LayerTool, TDataStd_GenericEmpty, "xcaf", "LayerTool")

//=================================================================================================

XCAFDoc_LayerTool::XCAFDoc_LayerTool() = default;

//=================================================================================================

occ::handle<XCAFDoc_LayerTool> XCAFDoc_LayerTool::Set(const TDF_Label& L)
{
  occ::handle<XCAFDoc_LayerTool> A;
  if (!L.FindAttribute(XCAFDoc_LayerTool::GetID(), A))
  {
    A = new XCAFDoc_LayerTool();
    L.AddAttribute(A);
    A->myShapeTool = XCAFDoc_DocumentTool::ShapeTool(L);
  }
  return A;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_LayerTool::GetID()
{
  static Standard_GUID LayerTblID("efd212f4-6dfd-11d4-b9c8-0060b0ee281b");
  return LayerTblID;
}

//=================================================================================================

TDF_Label XCAFDoc_LayerTool::BaseLabel() const
{
  return Label();
}

//=================================================================================================

const occ::handle<XCAFDoc_ShapeTool>& XCAFDoc_LayerTool::ShapeTool()
{
  if (myShapeTool.IsNull())
    myShapeTool = XCAFDoc_DocumentTool::ShapeTool(Label());
  return myShapeTool;
}

//=================================================================================================

bool XCAFDoc_LayerTool::IsLayer(const TDF_Label& lab) const
{
  TCollection_ExtendedString aLayer;
  return GetLayer(lab, aLayer);
}

//=================================================================================================

bool XCAFDoc_LayerTool::GetLayer(const TDF_Label& lab, TCollection_ExtendedString& aLayer) const
{
  if (lab.Father() != Label())
    return false;
  //   occ::handle<XCAFDoc_GraphNode> aGN;
  //   if (! lab.FindAttribute (XCAFDoc::LayerRefGUID(), aGN))
  //     return false;
  occ::handle<TDataStd_Name> aName;
  bool                       status = false;
  if (lab.FindAttribute(TDataStd_Name::GetID(), aName))
  {
    aLayer = aName->Get();
    status = true;
  }
  return status;
}

//=================================================================================================

bool XCAFDoc_LayerTool::FindLayer(const TCollection_ExtendedString& aLayer, TDF_Label& lab) const
{
  lab = FindLayer(aLayer);
  return (!lab.IsNull());
}

//=================================================================================================

TDF_Label XCAFDoc_LayerTool::FindLayer(const TCollection_ExtendedString& aLayer,
                                       const bool                        theToFindWithProperty,
                                       const bool                        theToFindVisible) const
{
  TDF_ChildIterator it(Label());
  TDF_Label         lab;
  for (; it.More(); it.Next())
  {
    TDF_Label                  aLabel = it.Value();
    occ::handle<TDataStd_Name> aName;
    if (aLabel.FindAttribute(TDataStd_Name::GetID(), aName) && (aName->Get().IsEqual(aLayer))
        && (!theToFindWithProperty
            || (theToFindWithProperty && (IsVisible(aLabel) == theToFindVisible))))
    {
      lab = aLabel;
      break;
    }
  }
  return lab;
}

//=================================================================================================

TDF_Label XCAFDoc_LayerTool::AddLayer(const TCollection_ExtendedString& theLayer) const
{
  TDF_Label lab;
  if (FindLayer(theLayer, lab))
    return lab;
  TDF_TagSource              aTag;
  TDF_Label                  aLabel = TDF_TagSource::NewChild(Label());
  occ::handle<TDataStd_Name> aName  = new TDataStd_Name;
  aName->Set(aLabel, theLayer);
  return aLabel;
}

//=================================================================================================

TDF_Label XCAFDoc_LayerTool::AddLayer(const TCollection_ExtendedString& theLayer,
                                      const bool                        theToFindVisible) const
{
  TDF_Label lab = FindLayer(theLayer, true, theToFindVisible);
  if (!lab.IsNull())
    return lab;
  TDF_TagSource              aTag;
  TDF_Label                  aLabel = TDF_TagSource::NewChild(Label());
  occ::handle<TDataStd_Name> aName  = new TDataStd_Name;
  aName->Set(aLabel, theLayer);
  return aLabel;
}

//=================================================================================================

void XCAFDoc_LayerTool::RemoveLayer(const TDF_Label& lab) const
{
  lab.ForgetAllAttributes(true);
}

//=================================================================================================

void XCAFDoc_LayerTool::GetLayerLabels(NCollection_Sequence<TDF_Label>& Labels) const
{
  Labels.Clear();
  TDF_ChildIterator ChildIterator(Label());
  for (; ChildIterator.More(); ChildIterator.Next())
  {
    TDF_Label L = ChildIterator.Value();
    if (IsLayer(L))
      Labels.Append(L);
  }
}

//=================================================================================================

void XCAFDoc_LayerTool::SetLayer(const TDF_Label& L,
                                 const TDF_Label& LayerL,
                                 const bool       shapeInOneLayer) const
{
  if (shapeInOneLayer)
    UnSetLayers(L);
  occ::handle<XCAFDoc_GraphNode> FGNode;
  occ::handle<XCAFDoc_GraphNode> ChGNode;
  if (!LayerL.FindAttribute(XCAFDoc::LayerRefGUID(), FGNode))
  {
    FGNode = XCAFDoc_GraphNode::Set(LayerL);
  }
  if (!L.FindAttribute(XCAFDoc::LayerRefGUID(), ChGNode))
  {
    ChGNode = XCAFDoc_GraphNode::Set(L);
  }
  FGNode->SetGraphID(XCAFDoc::LayerRefGUID());
  ChGNode->SetGraphID(XCAFDoc::LayerRefGUID());
  FGNode->SetChild(ChGNode);
  ChGNode->SetFather(FGNode);
}

//=================================================================================================

void XCAFDoc_LayerTool::SetLayer(const TDF_Label&                  L,
                                 const TCollection_ExtendedString& aLayer,
                                 const bool                        shapeInOneLayer) const
{
  TDF_Label aLayerL = AddLayer(aLayer);
  SetLayer(L, aLayerL, shapeInOneLayer);
}

//=================================================================================================

void XCAFDoc_LayerTool::UnSetLayers(const TDF_Label& L) const
{
  occ::handle<XCAFDoc_GraphNode> ChGNode, FGNode;
  if (L.FindAttribute(XCAFDoc::LayerRefGUID(), ChGNode))
  {
    while (ChGNode->NbFathers() != 0)
    {
      FGNode = ChGNode->GetFather(1);
      FGNode->UnSetChild(ChGNode);
      //       ChGNode->GetFather(1)->UnSetChild(ChGNode);
    }
    L.ForgetAttribute(XCAFDoc::LayerRefGUID());
  }
}

//=================================================================================================

bool XCAFDoc_LayerTool::UnSetOneLayer(const TDF_Label&                  L,
                                      const TCollection_ExtendedString& aLayer) const
{
  TDF_Label alab;
  if (!FindLayer(aLayer, alab))
    return false;
  return UnSetOneLayer(L, alab);
}

//=================================================================================================

bool XCAFDoc_LayerTool::UnSetOneLayer(const TDF_Label& L, const TDF_Label& aLayerL) const
{
  occ::handle<XCAFDoc_GraphNode> FGNode, ChGNode;
  if (!L.FindAttribute(XCAFDoc::LayerRefGUID(), ChGNode))
    return false;
  if (!aLayerL.FindAttribute(XCAFDoc::LayerRefGUID(), FGNode))
    return false;
  ChGNode->UnSetFather(FGNode);
  return true;
}

//=================================================================================================

bool XCAFDoc_LayerTool::IsSet(const TDF_Label& L, const TCollection_ExtendedString& aLayer) const
{
  occ::handle<XCAFDoc_GraphNode> Node;
  occ::handle<TDataStd_Name>     aName;
  TDF_Label                      lab;
  if (L.FindAttribute(XCAFDoc::LayerRefGUID(), Node) && (Node->NbFathers() != 0))
  {
    int i = 1;
    for (; i <= Node->NbFathers(); i++)
    {
      lab = Node->GetFather(i)->Label();
      if (lab.FindAttribute(TDataStd_Name::GetID(), aName) && (aName->Get().IsEqual(aLayer)))
        return true;
    }
  }
  return false;
}

//=================================================================================================

bool XCAFDoc_LayerTool::IsSet(const TDF_Label& L, const TDF_Label& aLayerL) const
{
  occ::handle<XCAFDoc_GraphNode> Node;
  occ::handle<TDataStd_Name>     aName;
  TDF_Label                      lab;
  if (L.FindAttribute(XCAFDoc::LayerRefGUID(), Node) && (Node->NbFathers() != 0))
  {
    int i = 1;
    for (; i <= Node->NbFathers(); i++)
    {
      lab = Node->GetFather(i)->Label();
      if (lab == aLayerL)
        return true;
    }
  }
  return false;
}

//=================================================================================================

bool XCAFDoc_LayerTool::GetLayers(
  const TDF_Label&                                                L,
  occ::handle<NCollection_HSequence<TCollection_ExtendedString>>& aLayerS)
{
  aLayerS = GetLayers(L);
  return (aLayerS->Length() != 0);
}

//=================================================================================================

bool XCAFDoc_LayerTool::GetLayers(const TDF_Label& L, NCollection_Sequence<TDF_Label>& aLayerLS)
{
  aLayerLS.Clear();
  occ::handle<XCAFDoc_GraphNode> aGNode;
  if (L.FindAttribute(XCAFDoc::LayerRefGUID(), aGNode))
  {
    for (int i = 1; i <= aGNode->NbFathers(); i++)
    {
      aLayerLS.Append(aGNode->GetFather(i)->Label());
    }
  }
  return aLayerLS.Length() > 0;
}

//=================================================================================================

occ::handle<NCollection_HSequence<TCollection_ExtendedString>> XCAFDoc_LayerTool::GetLayers(
  const TDF_Label& L)
{
  occ::handle<NCollection_HSequence<TCollection_ExtendedString>> aLayerS =
    new NCollection_HSequence<TCollection_ExtendedString>;
  NCollection_Sequence<TDF_Label> aLayerLS;
  if (GetLayers(L, aLayerLS))
  {
    for (int i = 1; i <= aLayerLS.Length(); ++i)
    {
      const TDF_Label&           aLab = aLayerLS(i);
      occ::handle<TDataStd_Name> aName;
      if (aLab.FindAttribute(TDataStd_Name::GetID(), aName))
      {
        aLayerS->Append(aName->Get());
      }
    }
  }
  return aLayerS;
}

//=================================================================================================

void XCAFDoc_LayerTool::GetShapesOfLayer(const TDF_Label&                 theLayerL,
                                         NCollection_Sequence<TDF_Label>& theShLabels)
{
  theShLabels.Clear();
  occ::handle<XCAFDoc_GraphNode> aGNode;
  if (theLayerL.FindAttribute(XCAFDoc::LayerRefGUID(), aGNode))
  {
    for (int aChildInd = 1; aChildInd <= aGNode->NbChildren(); aChildInd++)
    {
      theShLabels.Append(aGNode->GetChild(aChildInd)->Label());
    }
  }
}

//=================================================================================================

bool XCAFDoc_LayerTool::IsVisible(const TDF_Label& layerL) const
{
  occ::handle<TDataStd_UAttribute> aUAttr;
  return (!layerL.FindAttribute(XCAFDoc::InvisibleGUID(), aUAttr));
}

//=================================================================================================

void XCAFDoc_LayerTool::SetVisibility(const TDF_Label& layerL, const bool isvisible) const
{
  occ::handle<TDataStd_UAttribute> aUAttr;
  if (!isvisible)
  {
    if (!layerL.FindAttribute(XCAFDoc::InvisibleGUID(), aUAttr))
    {
      TDataStd_UAttribute::Set(layerL, XCAFDoc::InvisibleGUID());
    }
  }
  else
    layerL.ForgetAttribute(XCAFDoc::InvisibleGUID());
}

//=================================================================================================

bool XCAFDoc_LayerTool::SetLayer(const TopoDS_Shape& Sh,
                                 const TDF_Label&    LayerL,
                                 const bool          shapeInOneLayer)
{
  TDF_Label aLab;
  // PTV 22.01.2003 set layer for shape with location if it is necessary
  if (!myShapeTool->Search(Sh, aLab))
    return false;
  SetLayer(aLab, LayerL, shapeInOneLayer);
  return true;
}

//=================================================================================================

bool XCAFDoc_LayerTool::SetLayer(const TopoDS_Shape&               Sh,
                                 const TCollection_ExtendedString& aLayer,
                                 const bool                        shapeInOneLayer)
{
  TDF_Label aLayerL = AddLayer(aLayer);
  return SetLayer(Sh, aLayerL, shapeInOneLayer);
}

//=================================================================================================

bool XCAFDoc_LayerTool::UnSetLayers(const TopoDS_Shape& Sh)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return false;
  UnSetLayers(aLab);
  return true;
}

//=================================================================================================

bool XCAFDoc_LayerTool::UnSetOneLayer(const TopoDS_Shape&               Sh,
                                      const TCollection_ExtendedString& aLayer)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return false;
  return UnSetOneLayer(aLab, aLayer);
}

//=================================================================================================

bool XCAFDoc_LayerTool::UnSetOneLayer(const TopoDS_Shape& Sh, const TDF_Label& aLayerL)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return false;
  return UnSetOneLayer(aLab, aLayerL);
}

//=================================================================================================

bool XCAFDoc_LayerTool::IsSet(const TopoDS_Shape& Sh, const TCollection_ExtendedString& aLayer)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return false;
  return IsSet(aLab, aLayer);
}

//=================================================================================================

bool XCAFDoc_LayerTool::IsSet(const TopoDS_Shape& Sh, const TDF_Label& aLayerL)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return false;
  return IsSet(aLab, aLayerL);
}

//=================================================================================================

bool XCAFDoc_LayerTool::GetLayers(
  const TopoDS_Shape&                                             Sh,
  occ::handle<NCollection_HSequence<TCollection_ExtendedString>>& aLayerS)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return false;
  return GetLayers(aLab, aLayerS);
}

//=================================================================================================

bool XCAFDoc_LayerTool::GetLayers(const TopoDS_Shape& Sh, NCollection_Sequence<TDF_Label>& aLayerLS)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return false;
  return GetLayers(aLab, aLayerLS);
}

//=================================================================================================

occ::handle<NCollection_HSequence<TCollection_ExtendedString>> XCAFDoc_LayerTool::GetLayers(
  const TopoDS_Shape& Sh)
{
  occ::handle<NCollection_HSequence<TCollection_ExtendedString>> aLayerS =
    new NCollection_HSequence<TCollection_ExtendedString>;
  TDF_Label aLab;
  if (myShapeTool->Search(Sh, aLab))
    aLayerS = GetLayers(aLab);
  return aLayerS;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_LayerTool::ID() const
{
  return GetID();
}

//=================================================================================================

void XCAFDoc_LayerTool::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  NCollection_Sequence<TDF_Label> aLabels;
  GetLayerLabels(aLabels);
  for (NCollection_Sequence<TDF_Label>::Iterator aLayerLabelIt(aLabels); aLayerLabelIt.More();
       aLayerLabelIt.Next())
  {
    TCollection_AsciiString aLayerLabel;
    TDF_Tool::Entry(aLayerLabelIt.Value(), aLayerLabel);
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aLayerLabel)
  }
}