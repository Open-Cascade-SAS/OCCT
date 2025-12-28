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

#include <XCAFDoc_DimTolTool.hxx>

#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Map.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(XCAFDoc_DimTolTool,
                                      TDataStd_GenericEmpty,
                                      "xcaf",
                                      "DimTolTool")

//=================================================================================================

XCAFDoc_DimTolTool::XCAFDoc_DimTolTool() {}

//=================================================================================================

occ::handle<XCAFDoc_DimTolTool> XCAFDoc_DimTolTool::Set(const TDF_Label& L)
{
  occ::handle<XCAFDoc_DimTolTool> A;
  if (!L.FindAttribute(XCAFDoc_DimTolTool::GetID(), A))
  {
    A = new XCAFDoc_DimTolTool();
    L.AddAttribute(A);
    A->myShapeTool = XCAFDoc_DocumentTool::ShapeTool(L);
  }
  return A;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_DimTolTool::GetID()
{
  static Standard_GUID DGTTblID("72afb19b-44de-11d8-8776-001083004c77");
  return DGTTblID;
}

//=================================================================================================

TDF_Label XCAFDoc_DimTolTool::BaseLabel() const
{
  return Label();
}

//=================================================================================================

const occ::handle<XCAFDoc_ShapeTool>& XCAFDoc_DimTolTool::ShapeTool()
{
  if (myShapeTool.IsNull())
    myShapeTool = XCAFDoc_DocumentTool::ShapeTool(Label());
  return myShapeTool;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::IsDimTol(const TDF_Label& theDimTolL) const
{
  occ::handle<XCAFDoc_DimTol> aDimTolAttr;
  if (theDimTolL.FindAttribute(XCAFDoc_DimTol::GetID(), aDimTolAttr))
  {
    return true;
  }
  return false;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::IsDimension(const TDF_Label& theDimTolL) const
{
  occ::handle<XCAFDoc_Dimension> aDimTolAttr;
  if (theDimTolL.FindAttribute(XCAFDoc_Dimension::GetID(), aDimTolAttr))
  {
    return true;
  }
  return false;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::IsGeomTolerance(const TDF_Label& theDimTolL) const
{
  occ::handle<XCAFDoc_GeomTolerance> aDimTolAttr;
  if (theDimTolL.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aDimTolAttr))
  {
    return true;
  }
  return false;
}

//=================================================================================================

void XCAFDoc_DimTolTool::GetDimTolLabels(NCollection_Sequence<TDF_Label>& theLabels) const
{
  theLabels.Clear();
  TDF_ChildIterator aChildIterator(Label());
  for (; aChildIterator.More(); aChildIterator.Next())
  {
    TDF_Label aL = aChildIterator.Value();
    if (IsDimTol(aL))
      theLabels.Append(aL);
  }
}

//=================================================================================================

void XCAFDoc_DimTolTool::GetDimensionLabels(NCollection_Sequence<TDF_Label>& theLabels) const
{
  theLabels.Clear();
  TDF_ChildIterator aChildIterator(Label());
  for (; aChildIterator.More(); aChildIterator.Next())
  {
    TDF_Label aL = aChildIterator.Value();
    if (IsDimension(aL))
      theLabels.Append(aL);
  }
}

//=================================================================================================

void XCAFDoc_DimTolTool::GetGeomToleranceLabels(NCollection_Sequence<TDF_Label>& theLabels) const
{
  theLabels.Clear();
  TDF_ChildIterator aChildIterator(Label());
  for (; aChildIterator.More(); aChildIterator.Next())
  {
    TDF_Label aL = aChildIterator.Value();
    if (IsGeomTolerance(aL))
      theLabels.Append(aL);
  }
}

//=================================================================================================

bool XCAFDoc_DimTolTool::FindDimTol(const int                                       kind,
                                    const occ::handle<NCollection_HArray1<double>>& aVal,
                                    const occ::handle<TCollection_HAsciiString>&    aName,
                                    const occ::handle<TCollection_HAsciiString>&    aDescription,
                                    TDF_Label&                                      lab) const
{
  TDF_ChildIDIterator it(Label(), XCAFDoc_DimTol::GetID());
  for (; it.More(); it.Next())
  {
    TDF_Label                   DimTolL = it.Value()->Label();
    occ::handle<XCAFDoc_DimTol> DimTolAttr;
    if (!DimTolL.FindAttribute(XCAFDoc_DimTol::GetID(), DimTolAttr))
      continue;
    int                                      kind1         = DimTolAttr->GetKind();
    occ::handle<NCollection_HArray1<double>> aVal1         = DimTolAttr->GetVal();
    occ::handle<TCollection_HAsciiString>    aName1        = DimTolAttr->GetName();
    occ::handle<TCollection_HAsciiString>    aDescription1 = DimTolAttr->GetDescription();
    bool                                     IsEqual       = true;
    if (!(kind1 == kind))
      continue;
    if (!(aName == aName1))
      continue;
    if (!(aDescription == aDescription1))
      continue;
    if (kind < 20)
    { // dimension
      for (int i = 1; i <= aVal->Length(); i++)
      {
        if (std::abs(aVal->Value(i) - aVal1->Value(i)) > Precision::Confusion())
          IsEqual = false;
      }
    }
    else if (kind < 50)
    { // tolerance
      if (std::abs(aVal->Value(1) - aVal1->Value(1)) > Precision::Confusion())
        IsEqual = false;
    }
    if (IsEqual)
    {
      lab = DimTolL;
      return true;
    }
  }
  return false;
}

//=================================================================================================

TDF_Label XCAFDoc_DimTolTool::FindDimTol(
  const int                                       kind,
  const occ::handle<NCollection_HArray1<double>>& aVal,
  const occ::handle<TCollection_HAsciiString>&    aName,
  const occ::handle<TCollection_HAsciiString>&    aDescription) const
{
  TDF_Label L;
  FindDimTol(kind, aVal, aName, aDescription, L);
  return L;
}

//=================================================================================================

TDF_Label XCAFDoc_DimTolTool::AddDimTol(
  const int                                       kind,
  const occ::handle<NCollection_HArray1<double>>& aVal,
  const occ::handle<TCollection_HAsciiString>&    aName,
  const occ::handle<TCollection_HAsciiString>&    aDescription) const
{
  TDF_Label     DimTolL;
  TDF_TagSource aTag;
  DimTolL = aTag.NewChild(Label());
  XCAFDoc_DimTol::Set(DimTolL, kind, aVal, aName, aDescription);
  TCollection_AsciiString str = "DGT:";
  if (kind < 20)
    str.AssignCat("Dimension");
  else
    str.AssignCat("Tolerance");
  TDataStd_Name::Set(DimTolL, str);
  return DimTolL;
}

//=================================================================================================

TDF_Label XCAFDoc_DimTolTool::AddDimension()
{
  TDF_Label     aDimTolL;
  TDF_TagSource aTag;
  aDimTolL                            = aTag.NewChild(Label());
  occ::handle<XCAFDoc_Dimension> aDim = XCAFDoc_Dimension::Set(aDimTolL);
  TCollection_AsciiString        aStr = "DGT:Dimension";
  TDataStd_Name::Set(aDimTolL, aStr);
  return aDimTolL;
}

//=================================================================================================

TDF_Label XCAFDoc_DimTolTool::AddGeomTolerance()
{
  TDF_Label     aDimTolL;
  TDF_TagSource aTag;
  aDimTolL                                = aTag.NewChild(Label());
  occ::handle<XCAFDoc_GeomTolerance> aTol = XCAFDoc_GeomTolerance::Set(aDimTolL);
  TCollection_AsciiString            aStr = "DGT:Tolerance";
  TDataStd_Name::Set(aDimTolL, aStr);
  return aDimTolL;
}

//=================================================================================================

void XCAFDoc_DimTolTool::SetDimension(const TDF_Label& theL, const TDF_Label& theDimTolL) const
{
  TDF_Label nullLab;
  SetDimension(theL, nullLab, theDimTolL);
}

//=================================================================================================

void XCAFDoc_DimTolTool::SetDimension(const TDF_Label& theFirstL,
                                      const TDF_Label& theSecondL,
                                      const TDF_Label& theDimTolL) const
{
  NCollection_Sequence<TDF_Label> aFirstLS, aSecondLS;
  if (!theFirstL.IsNull())
    aFirstLS.Append(theFirstL);
  if (!theSecondL.IsNull())
    aSecondLS.Append(theSecondL);
  SetDimension(aFirstLS, aSecondLS, theDimTolL);
}

//=================================================================================================

void XCAFDoc_DimTolTool::SetDimension(const NCollection_Sequence<TDF_Label>& theFirstL,
                                      const NCollection_Sequence<TDF_Label>& theSecondL,
                                      const TDF_Label&                       theDimTolL) const
{
  if (!IsDimension(theDimTolL) || theFirstL.Length() == 0)
  {
    return;
  }

  occ::handle<XCAFDoc_GraphNode> aChGNode;
  occ::handle<XCAFDoc_GraphNode> aFGNode;
  occ::handle<XCAFDoc_GraphNode> aSecondFGNode;

  if (theDimTolL.FindAttribute(XCAFDoc::DimensionRefFirstGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aFGNode = aChGNode->GetFather(1);
      aFGNode->UnSetChild(aChGNode);
      if (aFGNode->NbChildren() == 0)
        aFGNode->ForgetAttribute(XCAFDoc::DimensionRefFirstGUID());
    }
    theDimTolL.ForgetAttribute(XCAFDoc::DimensionRefFirstGUID());
  }
  if (theDimTolL.FindAttribute(XCAFDoc::DimensionRefSecondGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aFGNode = aChGNode->GetFather(1);
      aFGNode->UnSetChild(aChGNode);
      if (aFGNode->NbChildren() == 0)
        aFGNode->ForgetAttribute(XCAFDoc::DimensionRefSecondGUID());
    }
    theDimTolL.ForgetAttribute(XCAFDoc::DimensionRefSecondGUID());
  }

  if (!theDimTolL.FindAttribute(XCAFDoc::DimensionRefFirstGUID(), aChGNode))
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theDimTolL);
    aChGNode->SetGraphID(XCAFDoc::DimensionRefFirstGUID());
  }
  for (int i = theFirstL.Lower(); i <= theFirstL.Upper(); i++)
  {
    if (!theFirstL.Value(i).FindAttribute(XCAFDoc::DimensionRefFirstGUID(), aFGNode))
    {
      aFGNode = new XCAFDoc_GraphNode;
      aFGNode = XCAFDoc_GraphNode::Set(theFirstL.Value(i));
    }
    aFGNode->SetGraphID(XCAFDoc::DimensionRefFirstGUID());
    aFGNode->SetChild(aChGNode);
    aChGNode->SetFather(aFGNode);
  }

  if (!theDimTolL.FindAttribute(XCAFDoc::DimensionRefSecondGUID(), aChGNode)
      && theSecondL.Length() > 0)
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theDimTolL);
    aChGNode->SetGraphID(XCAFDoc::DimensionRefSecondGUID());
  }
  for (int i = theSecondL.Lower(); i <= theSecondL.Upper(); i++)
  {
    if (!theSecondL.Value(i).FindAttribute(XCAFDoc::DimensionRefSecondGUID(), aSecondFGNode))
    {
      aSecondFGNode = new XCAFDoc_GraphNode;
      aSecondFGNode = XCAFDoc_GraphNode::Set(theSecondL.Value(i));
    }
    aSecondFGNode->SetGraphID(XCAFDoc::DimensionRefSecondGUID());
    aSecondFGNode->SetChild(aChGNode);
    aChGNode->SetFather(aSecondFGNode);
  }
}

//=================================================================================================

void XCAFDoc_DimTolTool::SetGeomTolerance(const TDF_Label& theL, const TDF_Label& theGeomTolL) const
{
  NCollection_Sequence<TDF_Label> aSeq;
  aSeq.Append(theL);
  SetGeomTolerance(aSeq, theGeomTolL);
}

//=================================================================================================

void XCAFDoc_DimTolTool::SetGeomTolerance(const NCollection_Sequence<TDF_Label>& theL,
                                          const TDF_Label&                       theGeomTolL) const
{
  //  // set reference
  //  occ::handle<TDataStd_TreeNode> refNode, mainNode;
  //  refNode = TDataStd_TreeNode::Set ( theDimTolL, XCAFDoc::GeomToleranceRefGUID() );
  //  mainNode  = TDataStd_TreeNode::Set ( theL,       XCAFDoc::GeomToleranceRefGUID() );
  //  refNode->Remove(); // abv: fix against bug in TreeNode::Append()
  //  mainNode->Append(refNode);

  if (!IsGeomTolerance(theGeomTolL) || theL.Length() == 0)
  {
    return;
  }

  occ::handle<XCAFDoc_GraphNode> aChGNode;
  occ::handle<XCAFDoc_GraphNode> aFGNode;

  if (theGeomTolL.FindAttribute(XCAFDoc::GeomToleranceRefGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aFGNode = aChGNode->GetFather(1);
      aFGNode->UnSetChild(aChGNode);
      if (aFGNode->NbChildren() == 0)
        aFGNode->ForgetAttribute(XCAFDoc::GeomToleranceRefGUID());
    }
    theGeomTolL.ForgetAttribute(XCAFDoc::GeomToleranceRefGUID());
  }

  if (!theGeomTolL.FindAttribute(XCAFDoc::GeomToleranceRefGUID(), aChGNode))
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theGeomTolL);
    aChGNode->SetGraphID(XCAFDoc::GeomToleranceRefGUID());
  }
  for (int i = theL.Lower(); i <= theL.Upper(); i++)
  {
    if (!theL.Value(i).FindAttribute(XCAFDoc::GeomToleranceRefGUID(), aFGNode))
    {
      aFGNode = new XCAFDoc_GraphNode;
      aFGNode = XCAFDoc_GraphNode::Set(theL.Value(i));
    }
    aFGNode->SetGraphID(XCAFDoc::GeomToleranceRefGUID());
    aFGNode->SetChild(aChGNode);
    aChGNode->SetFather(aFGNode);
  }
}

//=================================================================================================

void XCAFDoc_DimTolTool::SetDimTol(const TDF_Label& theL, const TDF_Label& theDimTolL) const
{
  // set reference
  occ::handle<TDataStd_TreeNode> refNode, mainNode;
  refNode  = TDataStd_TreeNode::Set(theDimTolL, XCAFDoc::DimTolRefGUID());
  mainNode = TDataStd_TreeNode::Set(theL, XCAFDoc::DimTolRefGUID());
  refNode->Remove(); // abv: fix against bug in TreeNode::Append()
  mainNode->Append(refNode);
}

//=================================================================================================

TDF_Label XCAFDoc_DimTolTool::SetDimTol(
  const TDF_Label&                                L,
  const int                                       kind,
  const occ::handle<NCollection_HArray1<double>>& aVal,
  const occ::handle<TCollection_HAsciiString>&    aName,
  const occ::handle<TCollection_HAsciiString>&    aDescription) const
{
  TDF_Label DimTolL = AddDimTol(kind, aVal, aName, aDescription);
  SetDimTol(L, DimTolL);
  return DimTolL;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::GetRefShapeLabel(const TDF_Label&                 theL,
                                          NCollection_Sequence<TDF_Label>& theShapeLFirst,
                                          NCollection_Sequence<TDF_Label>& theShapeLSecond)
{
  theShapeLFirst.Clear();
  theShapeLSecond.Clear();
  occ::handle<TDataStd_TreeNode> aNode;
  if (!theL.FindAttribute(XCAFDoc::DimTolRefGUID(), aNode) || !aNode->HasFather())
  {
    if (!theL.FindAttribute(XCAFDoc::DatumRefGUID(), aNode) || !aNode->HasFather())
    {
      occ::handle<XCAFDoc_GraphNode> aGNode;
      if (theL.FindAttribute(XCAFDoc::GeomToleranceRefGUID(), aGNode) && aGNode->NbFathers() > 0)
      {
        for (int i = 1; i <= aGNode->NbFathers(); i++)
        {
          theShapeLFirst.Append(aGNode->GetFather(i)->Label());
        }
        return true;
      }
      else if (theL.FindAttribute(XCAFDoc::DatumRefGUID(), aGNode) && aGNode->NbFathers() > 0)
      {
        for (int i = 1; i <= aGNode->NbFathers(); i++)
        {
          theShapeLFirst.Append(aGNode->GetFather(i)->Label());
        }
        return true;
      }
      else if (theL.FindAttribute(XCAFDoc::DimensionRefFirstGUID(), aGNode)
               && aGNode->NbFathers() > 0)
      {
        for (int i = 1; i <= aGNode->NbFathers(); i++)
        {
          theShapeLFirst.Append(aGNode->GetFather(i)->Label());
        }
        if (theL.FindAttribute(XCAFDoc::DimensionRefSecondGUID(), aGNode)
            && aGNode->NbFathers() > 0)
        {
          for (int i = 1; i <= aGNode->NbFathers(); i++)
          {
            theShapeLSecond.Append(aGNode->GetFather(i)->Label());
          }
        }
        return true;
      }
      else
      {
        return false;
      }
    }
  }

  theShapeLFirst.Append(aNode->Father()->Label());
  return true;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::GetRefDimensionLabels(const TDF_Label&                 theShapeL,
                                               NCollection_Sequence<TDF_Label>& theDimTols) const
{
  occ::handle<XCAFDoc_GraphNode> aGNode;
  bool                           aResult = false;
  if (theShapeL.FindAttribute(XCAFDoc::DimensionRefFirstGUID(), aGNode) && aGNode->NbChildren() > 0)
  {
    for (int i = 1; i <= aGNode->NbChildren(); i++)
    {
      theDimTols.Append(aGNode->GetChild(i)->Label());
    }
    aResult = true;
  }
  if (theShapeL.FindAttribute(XCAFDoc::DimensionRefSecondGUID(), aGNode)
      && aGNode->NbChildren() > 0)
  {
    for (int i = 1; i <= aGNode->NbChildren(); i++)
    {
      theDimTols.Append(aGNode->GetChild(i)->Label());
    }
    aResult = true;
  }
  return aResult;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::GetRefGeomToleranceLabels(
  const TDF_Label&                 theShapeL,
  NCollection_Sequence<TDF_Label>& theDimTols) const
{
  occ::handle<XCAFDoc_GraphNode> aGNode;
  if (!theShapeL.FindAttribute(XCAFDoc::GeomToleranceRefGUID(), aGNode)
      || aGNode->NbChildren() == 0)
  {
    return false;
  }
  for (int i = 1; i <= aGNode->NbChildren(); i++)
  {
    theDimTols.Append(aGNode->GetChild(i)->Label());
  }
  return true;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::GetRefDatumLabel(const TDF_Label&                 theShapeL,
                                          NCollection_Sequence<TDF_Label>& theDatum) const
{
  occ::handle<XCAFDoc_GraphNode> aGNode;
  if (!theShapeL.FindAttribute(XCAFDoc::DatumRefGUID(), aGNode))
  {
    return false;
  }
  for (int i = 1; i <= aGNode->NbChildren(); i++)
  {
    theDatum.Append(aGNode->GetChild(i)->Label());
  }
  return true;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::GetDimTol(const TDF_Label&                          DimTolL,
                                   int&                                      kind,
                                   occ::handle<NCollection_HArray1<double>>& aVal,
                                   occ::handle<TCollection_HAsciiString>&    aName,
                                   occ::handle<TCollection_HAsciiString>&    aDescription) const
{
  occ::handle<XCAFDoc_DimTol> DimTolAttr;
  if (!DimTolL.FindAttribute(XCAFDoc_DimTol::GetID(), DimTolAttr))
  {
    return false;
  }
  kind         = DimTolAttr->GetKind();
  aVal         = DimTolAttr->GetVal();
  aName        = DimTolAttr->GetName();
  aDescription = DimTolAttr->GetDescription();

  return true;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::IsDatum(const TDF_Label& theDimTolL) const
{
  occ::handle<XCAFDoc_Datum> aDatumAttr;
  if (theDimTolL.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
  {
    return true;
  }
  return false;
}

//=================================================================================================

void XCAFDoc_DimTolTool::GetDatumLabels(NCollection_Sequence<TDF_Label>& theLabels) const
{
  theLabels.Clear();
  TDF_ChildIterator aChildIterator(Label());
  for (; aChildIterator.More(); aChildIterator.Next())
  {
    TDF_Label L = aChildIterator.Value();
    if (IsDatum(L))
      theLabels.Append(L);
  }
}

//=================================================================================================

bool XCAFDoc_DimTolTool::FindDatum(const occ::handle<TCollection_HAsciiString>& aName,
                                   const occ::handle<TCollection_HAsciiString>& aDescription,
                                   const occ::handle<TCollection_HAsciiString>& anIdentification,
                                   TDF_Label&                                   lab) const
{
  TDF_ChildIDIterator it(Label(), XCAFDoc_Datum::GetID());
  for (; it.More(); it.Next())
  {
    occ::handle<TCollection_HAsciiString> aName1, aDescription1, anIdentification1;
    TDF_Label                             aLabel = it.Value()->Label();
    if (!GetDatum(aLabel, aName1, aDescription1, anIdentification1))
      continue;
    if (!(aName == aName1))
      continue;
    if (!(aDescription == aDescription1))
      continue;
    if (!(anIdentification == anIdentification1))
      continue;
    lab = aLabel;
    return true;
  }
  return false;
}

//=================================================================================================

TDF_Label XCAFDoc_DimTolTool::AddDatum(
  const occ::handle<TCollection_HAsciiString>& aName,
  const occ::handle<TCollection_HAsciiString>& aDescription,
  const occ::handle<TCollection_HAsciiString>& anIdentification) const
{
  TDF_Label     DatumL;
  TDF_TagSource aTag;
  DatumL = aTag.NewChild(Label());
  XCAFDoc_Datum::Set(DatumL, aName, aDescription, anIdentification);
  TDataStd_Name::Set(DatumL, "DGT:Datum");
  return DatumL;
}

//=================================================================================================

TDF_Label XCAFDoc_DimTolTool::AddDatum()
{
  TDF_Label     aDatumL;
  TDF_TagSource aTag;
  aDatumL                         = aTag.NewChild(Label());
  occ::handle<XCAFDoc_Datum> aDat = XCAFDoc_Datum::Set(aDatumL);
  TDataStd_Name::Set(aDatumL, "DGT:Datum");
  return aDatumL;
}

//=================================================================================================

void XCAFDoc_DimTolTool::SetDatum(const NCollection_Sequence<TDF_Label>& theL,
                                  const TDF_Label&                       theDatumL) const
{
  if (!IsDatum(theDatumL))
  {
    return;
  }

  occ::handle<XCAFDoc_GraphNode> aChGNode;
  occ::handle<XCAFDoc_GraphNode> aFGNode;

  if (theDatumL.FindAttribute(XCAFDoc::DatumRefGUID(), aChGNode))
  {
    while (aChGNode->NbFathers() > 0)
    {
      aFGNode = aChGNode->GetFather(1);
      aFGNode->UnSetChild(aChGNode);
      if (aFGNode->NbChildren() == 0)
        aFGNode->ForgetAttribute(XCAFDoc::DatumRefGUID());
    }
    theDatumL.ForgetAttribute(XCAFDoc::DatumRefGUID());
  }

  if (!theDatumL.FindAttribute(XCAFDoc::DatumRefGUID(), aChGNode))
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theDatumL);
    aChGNode->SetGraphID(XCAFDoc::DatumRefGUID());
  }
  for (int i = theL.Lower(); i <= theL.Upper(); i++)
  {
    if (!theL.Value(i).FindAttribute(XCAFDoc::DatumRefGUID(), aFGNode))
    {
      aFGNode = new XCAFDoc_GraphNode;
      aFGNode = XCAFDoc_GraphNode::Set(theL.Value(i));
    }
    aFGNode->SetGraphID(XCAFDoc::DatumRefGUID());
    aFGNode->SetChild(aChGNode);
    aChGNode->SetFather(aFGNode);
  }
}

//=================================================================================================

void XCAFDoc_DimTolTool::SetDatum(
  const TDF_Label&                             L,
  const TDF_Label&                             TolerL,
  const occ::handle<TCollection_HAsciiString>& aName,
  const occ::handle<TCollection_HAsciiString>& aDescription,
  const occ::handle<TCollection_HAsciiString>& anIdentification) const
{
  TDF_Label DatumL;
  if (!FindDatum(aName, aDescription, anIdentification, DatumL))
    DatumL = AddDatum(aName, aDescription, anIdentification);
  NCollection_Sequence<TDF_Label> aLabels;
  aLabels.Append(L);
  SetDatum(aLabels, DatumL);
  // set reference
  occ::handle<XCAFDoc_GraphNode> FGNode;
  occ::handle<XCAFDoc_GraphNode> ChGNode;
  if (!TolerL.FindAttribute(XCAFDoc::DatumTolRefGUID(), FGNode))
  {
    FGNode = new XCAFDoc_GraphNode;
    FGNode = XCAFDoc_GraphNode::Set(TolerL);
  }
  if (!DatumL.FindAttribute(XCAFDoc::DatumTolRefGUID(), ChGNode))
  {
    ChGNode = new XCAFDoc_GraphNode;
    ChGNode = XCAFDoc_GraphNode::Set(DatumL);
  }
  FGNode->SetGraphID(XCAFDoc::DatumTolRefGUID());
  ChGNode->SetGraphID(XCAFDoc::DatumTolRefGUID());
  FGNode->SetChild(ChGNode);
  ChGNode->SetFather(FGNode);
}

//=================================================================================================

void XCAFDoc_DimTolTool::SetDatumToGeomTol(const TDF_Label& theDatumL,
                                           const TDF_Label& theTolerL) const
{
  // set reference
  occ::handle<XCAFDoc_GraphNode> aFGNode;
  occ::handle<XCAFDoc_GraphNode> aChGNode;
  if (!theTolerL.FindAttribute(XCAFDoc::DatumTolRefGUID(), aFGNode))
  {
    aFGNode = new XCAFDoc_GraphNode;
    aFGNode = XCAFDoc_GraphNode::Set(theTolerL);
  }
  if (!theDatumL.FindAttribute(XCAFDoc::DatumTolRefGUID(), aChGNode))
  {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theDatumL);
  }
  aFGNode->SetGraphID(XCAFDoc::DatumTolRefGUID());
  aChGNode->SetGraphID(XCAFDoc::DatumTolRefGUID());
  aFGNode->SetChild(aChGNode);
  aChGNode->SetFather(aFGNode);
}

//=================================================================================================

bool XCAFDoc_DimTolTool::GetDatum(const TDF_Label&                       theDatumL,
                                  occ::handle<TCollection_HAsciiString>& theName,
                                  occ::handle<TCollection_HAsciiString>& theDescription,
                                  occ::handle<TCollection_HAsciiString>& theIdentification) const
{
  occ::handle<XCAFDoc_Datum> aDatumAttr;
  if (theDatumL.IsNull() || !theDatumL.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
    return false;

  theName           = aDatumAttr->GetName();
  theDescription    = aDatumAttr->GetDescription();
  theIdentification = aDatumAttr->GetIdentification();
  return true;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::GetDatumOfTolerLabels(const TDF_Label&                 theDimTolL,
                                               NCollection_Sequence<TDF_Label>& theDatums)
{
  occ::handle<XCAFDoc_GraphNode> aNode;
  if (!theDimTolL.FindAttribute(XCAFDoc::DatumTolRefGUID(), aNode))
    return false;

  for (int i = 1; i <= aNode->NbChildren(); i++)
  {
    occ::handle<XCAFDoc_GraphNode> aDatumNode = aNode->GetChild(i);
    theDatums.Append(aDatumNode->Label());
  }
  return true;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::GetDatumWithObjectOfTolerLabels(const TDF_Label& theDimTolL,
                                                         NCollection_Sequence<TDF_Label>& theDatums)
{
  occ::handle<XCAFDoc_GraphNode> aNode;
  if (!theDimTolL.FindAttribute(XCAFDoc::DatumTolRefGUID(), aNode))
    return false;

  NCollection_Map<TCollection_AsciiString> aDatumNameMap;
  for (int i = 1; i <= aNode->NbChildren(); i++)
  {
    occ::handle<XCAFDoc_GraphNode> aDatumNode = aNode->GetChild(i);
    TDF_Label                      aDatumL    = aDatumNode->Label();
    occ::handle<XCAFDoc_Datum>     aDatumAttr;
    if (!aDatumL.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
      continue;
    occ::handle<XCAFDimTolObjects_DatumObject> aDatumObj = aDatumAttr->GetObject();
    if (aDatumObj.IsNull())
      continue;
    occ::handle<TCollection_HAsciiString> aName = aDatumObj->GetName();
    if (!aDatumNameMap.Add(aName->String()))
    {
      // the datum has already been appended to sequence, due to one of its datum targets
      continue;
    }
    theDatums.Append(aDatumNode->Label());
  }
  return true;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::GetTolerOfDatumLabels(const TDF_Label&                 theDatumL,
                                               NCollection_Sequence<TDF_Label>& theTols) const
{
  occ::handle<XCAFDoc_GraphNode> aNode;
  if (!theDatumL.FindAttribute(XCAFDoc::DatumTolRefGUID(), aNode))
    return false;
  for (int i = 1; i <= aNode->NbFathers(); i++)
  {
    occ::handle<XCAFDoc_GraphNode> aDatumNode = aNode->GetFather(i);
    theTols.Append(aDatumNode->Label());
  }
  return true;
}

//=================================================================================================

bool XCAFDoc_DimTolTool::IsLocked(const TDF_Label& theViewL) const
{
  occ::handle<TDataStd_UAttribute> anAttr;
  return theViewL.FindAttribute(XCAFDoc::LockGUID(), anAttr);
}

//=================================================================================================

void XCAFDoc_DimTolTool::Lock(const TDF_Label& theViewL) const
{
  TDataStd_UAttribute::Set(theViewL, XCAFDoc::LockGUID());
}

//=================================================================================================

void XCAFDoc_DimTolTool::Unlock(const TDF_Label& theViewL) const
{
  theViewL.ForgetAttribute(XCAFDoc::LockGUID());
}

//=================================================================================================

const Standard_GUID& XCAFDoc_DimTolTool::ID() const
{
  return GetID();
}

//=================================================================================================

void XCAFDoc_DimTolTool::GetGDTPresentations(
  NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape>& theGDTLabelToShape) const
{
  NCollection_Sequence<TDF_Label> aGDTs;
  GetDimensionLabels(aGDTs);
  for (int i = 1; i <= aGDTs.Length(); i++)
  {
    occ::handle<XCAFDoc_Dimension> aDimAttr;
    const TDF_Label&               aCL = aGDTs.Value(i);
    if (!aCL.FindAttribute(XCAFDoc_Dimension::GetID(), aDimAttr))
      continue;
    occ::handle<XCAFDimTolObjects_DimensionObject> anObject = aDimAttr->GetObject();
    if (anObject.IsNull())
      continue;
    TopoDS_Shape aShape = anObject->GetPresentation();
    if (!aShape.IsNull())
      theGDTLabelToShape.Add(aCL, aShape);
  }

  aGDTs.Clear();
  GetGeomToleranceLabels(aGDTs);
  for (int i = 1; i <= aGDTs.Length(); i++)
  {
    occ::handle<XCAFDoc_GeomTolerance> aGTAttr;
    const TDF_Label&                   aCL = aGDTs.Value(i);
    if (!aCL.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGTAttr))
      continue;
    occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObject = aGTAttr->GetObject();
    if (anObject.IsNull())
      continue;
    TopoDS_Shape aShape = anObject->GetPresentation();
    if (!aShape.IsNull())
      theGDTLabelToShape.Add(aCL, aShape);
  }

  aGDTs.Clear();
  GetDatumLabels(aGDTs);
  for (int i = 1; i <= aGDTs.Length(); i++)
  {
    occ::handle<XCAFDoc_Datum> aGTAttr;
    const TDF_Label&           aCL = aGDTs.Value(i);
    if (!aCL.FindAttribute(XCAFDoc_Datum::GetID(), aGTAttr))
      continue;
    occ::handle<XCAFDimTolObjects_DatumObject> anObject = aGTAttr->GetObject();
    if (anObject.IsNull())
      continue;
    TopoDS_Shape aShape = anObject->GetPresentation();
    if (!aShape.IsNull())
      theGDTLabelToShape.Add(aCL, aShape);
  }
}

//=================================================================================================

void XCAFDoc_DimTolTool::SetGDTPresentations(
  NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape>& theGDTLabelToPrs)
{
  for (int i = 1; i <= theGDTLabelToPrs.Extent(); i++)
  {
    const TDF_Label&               aCL = theGDTLabelToPrs.FindKey(i);
    occ::handle<XCAFDoc_Dimension> aDimAttrDim;
    if (aCL.FindAttribute(XCAFDoc_Dimension::GetID(), aDimAttrDim))
    {
      occ::handle<XCAFDimTolObjects_DimensionObject> anObject = aDimAttrDim->GetObject();
      if (anObject.IsNull())
        continue;
      const TopoDS_Shape& aPrs = theGDTLabelToPrs.FindFromIndex(i);
      anObject->SetPresentation(aPrs, anObject->GetPresentationName());
      aDimAttrDim->SetObject(anObject);
      continue;
    }
    occ::handle<XCAFDoc_GeomTolerance> aDimAttrG;
    if (aCL.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aDimAttrG))
    {
      occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObject = aDimAttrG->GetObject();
      if (anObject.IsNull())
        continue;
      const TopoDS_Shape& aPrs = theGDTLabelToPrs.FindFromIndex(i);
      anObject->SetPresentation(aPrs, anObject->GetPresentationName());
      aDimAttrG->SetObject(anObject);
      continue;
    }
    occ::handle<XCAFDoc_Datum> aDimAttrD;
    if (aCL.FindAttribute(XCAFDoc_Datum::GetID(), aDimAttrD))
    {
      occ::handle<XCAFDimTolObjects_DatumObject> anObject = aDimAttrD->GetObject();
      if (anObject.IsNull())
        continue;
      const TopoDS_Shape& aPrs = theGDTLabelToPrs.FindFromIndex(i);
      anObject->SetPresentation(aPrs, anObject->GetPresentationName());
      aDimAttrD->SetObject(anObject);
      continue;
    }
  }
}

//=================================================================================================

void XCAFDoc_DimTolTool::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  NCollection_Sequence<TDF_Label> aLabels;
  GetDimensionLabels(aLabels);
  for (NCollection_Sequence<TDF_Label>::Iterator aDimLabelIt(aLabels); aDimLabelIt.More();
       aDimLabelIt.Next())
  {
    TCollection_AsciiString aDimensionLabel;
    TDF_Tool::Entry(aDimLabelIt.Value(), aDimensionLabel);
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aDimensionLabel)
  }

  aLabels.Clear();
  GetGeomToleranceLabels(aLabels);
  for (NCollection_Sequence<TDF_Label>::Iterator aGeomToleranceLabelIt(aLabels);
       aGeomToleranceLabelIt.More();
       aGeomToleranceLabelIt.Next())
  {
    TCollection_AsciiString aGeomToleranceLabel;
    TDF_Tool::Entry(aGeomToleranceLabelIt.Value(), aGeomToleranceLabel);
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aGeomToleranceLabel)
  }

  aLabels.Clear();
  GetDimTolLabels(aLabels);
  for (NCollection_Sequence<TDF_Label>::Iterator aDimTolLabelIt(aLabels); aDimTolLabelIt.More();
       aDimTolLabelIt.Next())
  {
    TCollection_AsciiString aDimTolLabelLabel;
    TDF_Tool::Entry(aDimTolLabelIt.Value(), aDimTolLabelLabel);
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aDimTolLabelLabel)
  }

  aLabels.Clear();
  GetDatumLabels(aLabels);
  for (NCollection_Sequence<TDF_Label>::Iterator aDatumLabelIt(aLabels); aDatumLabelIt.More();
       aDatumLabelIt.Next())
  {
    TCollection_AsciiString aDatumLabel;
    TDF_Tool::Entry(aDatumLabelIt.Value(), aDatumLabel);
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aDatumLabel)
  }
}
