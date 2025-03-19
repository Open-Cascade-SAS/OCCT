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

XCAFDoc_LayerTool::XCAFDoc_LayerTool() {}

//=================================================================================================

Handle(XCAFDoc_LayerTool) XCAFDoc_LayerTool::Set(const TDF_Label& L)
{
  Handle(XCAFDoc_LayerTool) A;
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

const Handle(XCAFDoc_ShapeTool)& XCAFDoc_LayerTool::ShapeTool()
{
  if (myShapeTool.IsNull())
    myShapeTool = XCAFDoc_DocumentTool::ShapeTool(Label());
  return myShapeTool;
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::IsLayer(const TDF_Label& lab) const
{
  TCollection_ExtendedString aLayer;
  return GetLayer(lab, aLayer);
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::GetLayer(const TDF_Label&            lab,
                                             TCollection_ExtendedString& aLayer) const
{
  if (lab.Father() != Label())
    return Standard_False;
  //   Handle(XCAFDoc_GraphNode) aGN;
  //   if (! lab.FindAttribute (XCAFDoc::LayerRefGUID(), aGN))
  //     return Standard_False;
  Handle(TDataStd_Name) aName;
  Standard_Boolean      status = Standard_False;
  if (lab.FindAttribute(TDataStd_Name::GetID(), aName))
  {
    aLayer = aName->Get();
    status = Standard_True;
  }
  return status;
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::FindLayer(const TCollection_ExtendedString& aLayer,
                                              TDF_Label&                        lab) const
{
  lab = FindLayer(aLayer);
  return (!lab.IsNull());
}

//=================================================================================================

TDF_Label XCAFDoc_LayerTool::FindLayer(const TCollection_ExtendedString& aLayer,
                                       const Standard_Boolean            theToFindWithProperty,
                                       const Standard_Boolean            theToFindVisible) const
{
  TDF_ChildIterator it(Label());
  TDF_Label         lab;
  for (; it.More(); it.Next())
  {
    TDF_Label             aLabel = it.Value();
    Handle(TDataStd_Name) aName;
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
  TDF_TagSource         aTag;
  TDF_Label             aLabel = aTag.NewChild(Label());
  Handle(TDataStd_Name) aName  = new TDataStd_Name;
  aName->Set(aLabel, theLayer);
  return aLabel;
}

//=================================================================================================

TDF_Label XCAFDoc_LayerTool::AddLayer(const TCollection_ExtendedString& theLayer,
                                      const Standard_Boolean            theToFindVisible) const
{
  TDF_Label lab = FindLayer(theLayer, Standard_True, theToFindVisible);
  if (!lab.IsNull())
    return lab;
  TDF_TagSource         aTag;
  TDF_Label             aLabel = aTag.NewChild(Label());
  Handle(TDataStd_Name) aName  = new TDataStd_Name;
  aName->Set(aLabel, theLayer);
  return aLabel;
}

//=================================================================================================

void XCAFDoc_LayerTool::RemoveLayer(const TDF_Label& lab) const
{
  lab.ForgetAllAttributes(Standard_True);
}

//=================================================================================================

void XCAFDoc_LayerTool::GetLayerLabels(TDF_LabelSequence& Labels) const
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

void XCAFDoc_LayerTool::SetLayer(const TDF_Label&       L,
                                 const TDF_Label&       LayerL,
                                 const Standard_Boolean shapeInOneLayer) const
{
  if (shapeInOneLayer)
    UnSetLayers(L);
  Handle(XCAFDoc_GraphNode) FGNode;
  Handle(XCAFDoc_GraphNode) ChGNode;
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
                                 const Standard_Boolean            shapeInOneLayer) const
{
  TDF_Label aLayerL = AddLayer(aLayer);
  SetLayer(L, aLayerL, shapeInOneLayer);
}

//=================================================================================================

void XCAFDoc_LayerTool::UnSetLayers(const TDF_Label& L) const
{
  Handle(XCAFDoc_GraphNode) ChGNode, FGNode;
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

Standard_Boolean XCAFDoc_LayerTool::UnSetOneLayer(const TDF_Label&                  L,
                                                  const TCollection_ExtendedString& aLayer) const
{
  TDF_Label alab;
  if (!FindLayer(aLayer, alab))
    return Standard_False;
  return UnSetOneLayer(L, alab);
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::UnSetOneLayer(const TDF_Label& L,
                                                  const TDF_Label& aLayerL) const
{
  Handle(XCAFDoc_GraphNode) FGNode, ChGNode;
  if (!L.FindAttribute(XCAFDoc::LayerRefGUID(), ChGNode))
    return Standard_False;
  if (!aLayerL.FindAttribute(XCAFDoc::LayerRefGUID(), FGNode))
    return Standard_False;
  ChGNode->UnSetFather(FGNode);
  return Standard_True;
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::IsSet(const TDF_Label&                  L,
                                          const TCollection_ExtendedString& aLayer) const
{
  Handle(XCAFDoc_GraphNode) Node;
  Handle(TDataStd_Name)     aName;
  TDF_Label                 lab;
  if (L.FindAttribute(XCAFDoc::LayerRefGUID(), Node) && (Node->NbFathers() != 0))
  {
    Standard_Integer i = 1;
    for (; i <= Node->NbFathers(); i++)
    {
      lab = Node->GetFather(i)->Label();
      if (lab.FindAttribute(TDataStd_Name::GetID(), aName) && (aName->Get().IsEqual(aLayer)))
        return Standard_True;
    }
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::IsSet(const TDF_Label& L, const TDF_Label& aLayerL) const
{
  Handle(XCAFDoc_GraphNode) Node;
  Handle(TDataStd_Name)     aName;
  TDF_Label                 lab;
  if (L.FindAttribute(XCAFDoc::LayerRefGUID(), Node) && (Node->NbFathers() != 0))
  {
    Standard_Integer i = 1;
    for (; i <= Node->NbFathers(); i++)
    {
      lab = Node->GetFather(i)->Label();
      if (lab == aLayerL)
        return Standard_True;
    }
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::GetLayers(const TDF_Label&                           L,
                                              Handle(TColStd_HSequenceOfExtendedString)& aLayerS)
{
  aLayerS = GetLayers(L);
  return (aLayerS->Length() != 0);
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::GetLayers(const TDF_Label& L, TDF_LabelSequence& aLayerLS)
{
  aLayerLS.Clear();
  Handle(XCAFDoc_GraphNode) aGNode;
  if (L.FindAttribute(XCAFDoc::LayerRefGUID(), aGNode))
  {
    for (Standard_Integer i = 1; i <= aGNode->NbFathers(); i++)
    {
      aLayerLS.Append(aGNode->GetFather(i)->Label());
    }
  }
  return aLayerLS.Length() > 0;
}

//=================================================================================================

Handle(TColStd_HSequenceOfExtendedString) XCAFDoc_LayerTool::GetLayers(const TDF_Label& L)
{
  Handle(TColStd_HSequenceOfExtendedString) aLayerS = new TColStd_HSequenceOfExtendedString;
  TDF_LabelSequence                         aLayerLS;
  if (GetLayers(L, aLayerLS))
  {
    for (Standard_Integer i = 1; i <= aLayerLS.Length(); ++i)
    {
      const TDF_Label&      aLab = aLayerLS(i);
      Handle(TDataStd_Name) aName;
      if (aLab.FindAttribute(TDataStd_Name::GetID(), aName))
      {
        aLayerS->Append(aName->Get());
      }
    }
  }
  return aLayerS;
}

//=================================================================================================

void XCAFDoc_LayerTool::GetShapesOfLayer(const TDF_Label& theLayerL, TDF_LabelSequence& theShLabels)
{
  theShLabels.Clear();
  Handle(XCAFDoc_GraphNode) aGNode;
  if (theLayerL.FindAttribute(XCAFDoc::LayerRefGUID(), aGNode))
  {
    for (Standard_Integer aChildInd = 1; aChildInd <= aGNode->NbChildren(); aChildInd++)
    {
      theShLabels.Append(aGNode->GetChild(aChildInd)->Label());
    }
  }
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::IsVisible(const TDF_Label& layerL) const
{
  Handle(TDataStd_UAttribute) aUAttr;
  return (!layerL.FindAttribute(XCAFDoc::InvisibleGUID(), aUAttr));
}

//=================================================================================================

void XCAFDoc_LayerTool::SetVisibility(const TDF_Label&       layerL,
                                      const Standard_Boolean isvisible) const
{
  Handle(TDataStd_UAttribute) aUAttr;
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

Standard_Boolean XCAFDoc_LayerTool::SetLayer(const TopoDS_Shape&    Sh,
                                             const TDF_Label&       LayerL,
                                             const Standard_Boolean shapeInOneLayer)
{
  TDF_Label aLab;
  // PTV 22.01.2003 set layer for shape with location if it is necessary
  if (!myShapeTool->Search(Sh, aLab))
    return Standard_False;
  SetLayer(aLab, LayerL, shapeInOneLayer);
  return Standard_True;
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::SetLayer(const TopoDS_Shape&               Sh,
                                             const TCollection_ExtendedString& aLayer,
                                             const Standard_Boolean            shapeInOneLayer)
{
  TDF_Label aLayerL = AddLayer(aLayer);
  return SetLayer(Sh, aLayerL, shapeInOneLayer);
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::UnSetLayers(const TopoDS_Shape& Sh)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return Standard_False;
  UnSetLayers(aLab);
  return Standard_True;
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::UnSetOneLayer(const TopoDS_Shape&               Sh,
                                                  const TCollection_ExtendedString& aLayer)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return Standard_False;
  return UnSetOneLayer(aLab, aLayer);
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::UnSetOneLayer(const TopoDS_Shape& Sh, const TDF_Label& aLayerL)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return Standard_False;
  return UnSetOneLayer(aLab, aLayerL);
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::IsSet(const TopoDS_Shape&               Sh,
                                          const TCollection_ExtendedString& aLayer)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return Standard_False;
  return IsSet(aLab, aLayer);
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::IsSet(const TopoDS_Shape& Sh, const TDF_Label& aLayerL)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return Standard_False;
  return IsSet(aLab, aLayerL);
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::GetLayers(const TopoDS_Shape&                        Sh,
                                              Handle(TColStd_HSequenceOfExtendedString)& aLayerS)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return Standard_False;
  return GetLayers(aLab, aLayerS);
}

//=================================================================================================

Standard_Boolean XCAFDoc_LayerTool::GetLayers(const TopoDS_Shape& Sh, TDF_LabelSequence& aLayerLS)
{
  TDF_Label aLab;
  if (!myShapeTool->Search(Sh, aLab))
    return Standard_False;
  return GetLayers(aLab, aLayerLS);
}

//=================================================================================================

Handle(TColStd_HSequenceOfExtendedString) XCAFDoc_LayerTool::GetLayers(const TopoDS_Shape& Sh)
{
  Handle(TColStd_HSequenceOfExtendedString) aLayerS = new TColStd_HSequenceOfExtendedString;
  TDF_Label                                 aLab;
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

void XCAFDoc_LayerTool::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  TDF_LabelSequence aLabels;
  GetLayerLabels(aLabels);
  for (TDF_LabelSequence::Iterator aLayerLabelIt(aLabels); aLayerLabelIt.More();
       aLayerLabelIt.Next())
  {
    TCollection_AsciiString aLayerLabel;
    TDF_Tool::Entry(aLayerLabelIt.Value(), aLayerLabel);
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aLayerLabel)
  }
}