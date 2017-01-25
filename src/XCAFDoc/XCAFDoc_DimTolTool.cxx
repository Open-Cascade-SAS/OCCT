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
#include <Precision.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_MapOfAsciiString.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_ShapeTool.hxx>


IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_DimTolTool,TDF_Attribute)

//=======================================================================
//function : XCAFDoc_DimTolTool
//purpose  : 
//=======================================================================
XCAFDoc_DimTolTool::XCAFDoc_DimTolTool()
{
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_DimTolTool) XCAFDoc_DimTolTool::Set(const TDF_Label& L) 
{
  Handle(XCAFDoc_DimTolTool) A;
  if (!L.FindAttribute (XCAFDoc_DimTolTool::GetID(), A)) {
    A = new XCAFDoc_DimTolTool ();
    L.AddAttribute(A);
    A->myShapeTool = XCAFDoc_DocumentTool::ShapeTool(L);
  }
  return A;
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_DimTolTool::GetID() 
{
  static Standard_GUID DGTTblID ("72afb19b-44de-11d8-8776-001083004c77");
  return DGTTblID; 
}


//=======================================================================
//function : BaseLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DimTolTool::BaseLabel() const
{
  return Label();
}


//=======================================================================
//function : ShapeTool
//purpose  : 
//=======================================================================

const Handle(XCAFDoc_ShapeTool)& XCAFDoc_DimTolTool::ShapeTool() 
{
  if(myShapeTool.IsNull())
    myShapeTool = XCAFDoc_DocumentTool::ShapeTool(Label());
  return myShapeTool;
}


//=======================================================================
//function : IsDimTol
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::IsDimTol(const TDF_Label& theDimTolL) const
{
  Handle(XCAFDoc_DimTol) aDimTolAttr;
  if(theDimTolL.FindAttribute(XCAFDoc_DimTol::GetID(),aDimTolAttr)) {
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : IsDimension
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::IsDimension(const TDF_Label& theDimTolL) const
{
  Handle(XCAFDoc_Dimension) aDimTolAttr;
  if(theDimTolL.FindAttribute(XCAFDoc_Dimension::GetID(),aDimTolAttr)) {
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : IsGeomTolerance
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::IsGeomTolerance(const TDF_Label& theDimTolL) const
{
  Handle(XCAFDoc_GeomTolerance) aDimTolAttr;
  if(theDimTolL.FindAttribute(XCAFDoc_GeomTolerance::GetID(),aDimTolAttr)) {
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : GetDimTolLabels
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::GetDimTolLabels(TDF_LabelSequence& theLabels) const
{
  theLabels.Clear();
  TDF_ChildIterator aChildIterator( Label() ); 
  for (; aChildIterator.More(); aChildIterator.Next()) {
    TDF_Label aL = aChildIterator.Value();
    if ( IsDimTol(aL)) theLabels.Append(aL);
  }
}

//=======================================================================
//function : GetDimensionLabels
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::GetDimensionLabels(TDF_LabelSequence& theLabels) const
{
  theLabels.Clear();
  TDF_ChildIterator aChildIterator( Label() ); 
  for (; aChildIterator.More(); aChildIterator.Next()) {
    TDF_Label aL = aChildIterator.Value();
    if ( IsDimension(aL)) theLabels.Append(aL);
  }
}

//=======================================================================
//function : GetGeomToleranceLabels
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::GetGeomToleranceLabels(TDF_LabelSequence& theLabels) const
{
  theLabels.Clear();
  TDF_ChildIterator aChildIterator( Label() ); 
  for (; aChildIterator.More(); aChildIterator.Next()) {
    TDF_Label aL = aChildIterator.Value();
    if ( IsGeomTolerance(aL)) theLabels.Append(aL);
  }
}

//=======================================================================
//function : FindDimTol
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::FindDimTol(const Standard_Integer kind,
                                                const Handle(TColStd_HArray1OfReal)& aVal,
                                                const Handle(TCollection_HAsciiString)& aName,
                                                const Handle(TCollection_HAsciiString)& aDescription,
                                                TDF_Label& lab) const
{
  TDF_ChildIDIterator it(Label(),XCAFDoc_DimTol::GetID());
  for(; it.More(); it.Next()) {
    TDF_Label DimTolL = it.Value()->Label();
    Handle(XCAFDoc_DimTol) DimTolAttr;
    if(!DimTolL.FindAttribute(XCAFDoc_DimTol::GetID(),DimTolAttr)) continue;
    Standard_Integer kind1 = DimTolAttr->GetKind();
    Handle(TColStd_HArray1OfReal) aVal1 = DimTolAttr->GetVal();
    Handle(TCollection_HAsciiString) aName1 = DimTolAttr->GetName();
    Handle(TCollection_HAsciiString) aDescription1 = DimTolAttr->GetDescription();
    Standard_Boolean IsEqual = Standard_True;
    if(!(kind1==kind)) continue;
    if(!(aName==aName1)) continue;
    if(!(aDescription==aDescription1)) continue;
    if(kind<20) {  //dimension
      for(Standard_Integer i=1; i<=aVal->Length(); i++) {
        if(Abs(aVal->Value(i)-aVal1->Value(i))>Precision::Confusion())
          IsEqual = Standard_False;
      }
    }
    else if(kind<50) { //tolerance
      if(Abs(aVal->Value(1)-aVal1->Value(1))>Precision::Confusion())
        IsEqual = Standard_False;
    }
    if(IsEqual) {
      lab = DimTolL;
      return Standard_True;
    }
  }
  return Standard_False;
}


//=======================================================================
//function : FindDimTol
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DimTolTool::FindDimTol(const Standard_Integer kind,
                                         const Handle(TColStd_HArray1OfReal)& aVal,
                                         const Handle(TCollection_HAsciiString)& aName,
                                         const Handle(TCollection_HAsciiString)& aDescription) const
{
  TDF_Label L;
  FindDimTol(kind,aVal,aName,aDescription,L);
  return L;
}


//=======================================================================
//function : AddDimTol
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DimTolTool::AddDimTol(const Standard_Integer kind,
                                        const Handle(TColStd_HArray1OfReal)& aVal,
                                        const Handle(TCollection_HAsciiString)& aName,
                                        const Handle(TCollection_HAsciiString)& aDescription) const
{
  TDF_Label DimTolL;
  TDF_TagSource aTag;
  DimTolL = aTag.NewChild ( Label() );
  XCAFDoc_DimTol::Set(DimTolL,kind,aVal,aName,aDescription);
  TCollection_AsciiString str = "DGT:";
  if(kind<20) str.AssignCat("Dimension");
  else str.AssignCat("Tolerance");
  TDataStd_Name::Set(DimTolL,str);
  return DimTolL;
}

//=======================================================================
//function : AddDimension
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DimTolTool::AddDimension()
{
  TDF_Label aDimTolL;
  TDF_TagSource aTag;
  aDimTolL = aTag.NewChild ( Label() );
  Handle(XCAFDoc_Dimension) aDim = XCAFDoc_Dimension::Set(aDimTolL);
  TCollection_AsciiString aStr = "DGT:Dimension";
  TDataStd_Name::Set(aDimTolL,aStr);
  return aDimTolL;
}

//=======================================================================
//function : AddGeomTolerance
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DimTolTool::AddGeomTolerance()
{
  TDF_Label aDimTolL;
  TDF_TagSource aTag;
  aDimTolL = aTag.NewChild ( Label() );
  Handle(XCAFDoc_GeomTolerance) aTol = XCAFDoc_GeomTolerance::Set(aDimTolL);
  TCollection_AsciiString aStr = "DGT:Tolerance";
  TDataStd_Name::Set(aDimTolL,aStr);
  return aDimTolL;
}

//=======================================================================
//function : SetDimension
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::SetDimension(const TDF_Label& theL,
                                   const TDF_Label& theDimTolL) const
{
  TDF_Label nullLab;
  SetDimension(theL, nullLab, theDimTolL);
}

//=======================================================================
//function : SetDimension
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::SetDimension(const TDF_Label& theFirstL,
                                   const TDF_Label& theSecondL,
                                   const TDF_Label& theDimTolL) const
{
  TDF_LabelSequence aFirstLS, aSecondLS;
  if(!theFirstL.IsNull())
    aFirstLS.Append(theFirstL);
  if(!theSecondL.IsNull())
    aSecondLS.Append(theSecondL);
  SetDimension(aFirstLS, aSecondLS, theDimTolL);
}

//=======================================================================
//function : SetDimension
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::SetDimension(const TDF_LabelSequence& theFirstL,
                                   const TDF_LabelSequence& theSecondL,
                                   const TDF_Label& theDimTolL) const
{
  if(!IsDimension(theDimTolL) || theFirstL.Length() == 0)
  {
    return;
  }

  Handle(XCAFDoc_GraphNode) aChGNode;
  Handle(XCAFDoc_GraphNode) aFGNode;
  Handle(XCAFDoc_GraphNode) aSecondFGNode;

  if ( theDimTolL.FindAttribute (XCAFDoc::DimensionRefFirstGUID(), aChGNode) ) {
    while (aChGNode->NbFathers() > 0) {
      aFGNode = aChGNode->GetFather(1);
      aFGNode->UnSetChild(aChGNode);
      if(aFGNode->NbChildren() == 0)
        aFGNode->ForgetAttribute( XCAFDoc::DimensionRefFirstGUID() );
    }
    theDimTolL.ForgetAttribute ( XCAFDoc::DimensionRefFirstGUID() );
  }
  if ( theDimTolL.FindAttribute (XCAFDoc::DimensionRefSecondGUID(), aChGNode) ) {
    while (aChGNode->NbFathers() > 0) {
      aFGNode = aChGNode->GetFather(1);
      aFGNode->UnSetChild(aChGNode);
      if(aFGNode->NbChildren() == 0)
        aFGNode->ForgetAttribute( XCAFDoc::DimensionRefSecondGUID() );
    }
    theDimTolL.ForgetAttribute ( XCAFDoc::DimensionRefSecondGUID() );
  }

  if (!theDimTolL.FindAttribute(XCAFDoc::DimensionRefFirstGUID(), aChGNode)) {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theDimTolL);
    aChGNode->SetGraphID(XCAFDoc::DimensionRefFirstGUID());
  }
  for(Standard_Integer i = theFirstL.Lower(); i <= theFirstL.Upper(); i++)
  {
    if (!theFirstL.Value(i).FindAttribute(XCAFDoc::DimensionRefFirstGUID(), aFGNode) ) {
      aFGNode = new XCAFDoc_GraphNode;
      aFGNode = XCAFDoc_GraphNode::Set(theFirstL.Value(i));
    }
    aFGNode->SetGraphID(XCAFDoc::DimensionRefFirstGUID());
    aFGNode->SetChild(aChGNode);
    aChGNode->SetFather(aFGNode);
  }

  if (!theDimTolL.FindAttribute(XCAFDoc::DimensionRefSecondGUID(), aChGNode) && theSecondL.Length() > 0) {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theDimTolL);
    aChGNode->SetGraphID(XCAFDoc::DimensionRefSecondGUID());
  }
  for(Standard_Integer i = theSecondL.Lower(); i <= theSecondL.Upper(); i++)
  {
    if(!theSecondL.Value(i).FindAttribute(XCAFDoc::DimensionRefSecondGUID(), aSecondFGNode) ) {
      aSecondFGNode = new XCAFDoc_GraphNode;
      aSecondFGNode = XCAFDoc_GraphNode::Set(theSecondL.Value(i));
    }
    aSecondFGNode->SetGraphID(XCAFDoc::DimensionRefSecondGUID());
    aSecondFGNode->SetChild(aChGNode);
    aChGNode->SetFather(aSecondFGNode);
  }
}

//=======================================================================
//function : SetGeomTolerance
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::SetGeomTolerance(const TDF_Label& theL,
                                   const TDF_Label& theDimTolL) const
{
  TDF_LabelSequence aSeq;
  aSeq.Append(theL);
  SetGeomTolerance(aSeq, theDimTolL);
}

//=======================================================================
//function : SetGeomTolerance
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::SetGeomTolerance(const TDF_LabelSequence& theL,
                                   const TDF_Label& theDimTolL) const
{
  //  // set reference
  //  Handle(TDataStd_TreeNode) refNode, mainNode;
  //  refNode = TDataStd_TreeNode::Set ( theDimTolL, XCAFDoc::GeomToleranceRefGUID() );
  //  mainNode  = TDataStd_TreeNode::Set ( theL,       XCAFDoc::GeomToleranceRefGUID() );
  //  refNode->Remove(); // abv: fix against bug in TreeNode::Append()
  //  mainNode->Append(refNode);
  
  if(!IsGeomTolerance(theDimTolL) ||  theL.Length() == 0)
  {
    return;
  }

  Handle(XCAFDoc_GraphNode) aChGNode;
  Handle(XCAFDoc_GraphNode) aFGNode;

  //Handle(XCAFDoc_GraphNode) ChGNode, FGNode;
  if ( theDimTolL.FindAttribute (XCAFDoc::GeomToleranceRefGUID(), aChGNode) ) {
    while (aChGNode->NbFathers() > 0) {
      aFGNode = aChGNode->GetFather(1);
      aFGNode->UnSetChild(aChGNode);
      if(aFGNode->NbChildren() == 0)
        aFGNode->ForgetAttribute( XCAFDoc::GeomToleranceRefGUID() );
    }
    theDimTolL.ForgetAttribute ( XCAFDoc::GeomToleranceRefGUID() );
  }

  if (!theDimTolL.FindAttribute(XCAFDoc::GeomToleranceRefGUID(), aChGNode)) {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theDimTolL);
    aChGNode->SetGraphID(XCAFDoc::GeomToleranceRefGUID());
  }
  for(Standard_Integer i = theL.Lower(); i <= theL.Upper(); i++)
  {
    if (!theL.Value(i).FindAttribute(XCAFDoc::GeomToleranceRefGUID(), aFGNode) ) {
      aFGNode = new XCAFDoc_GraphNode;
      aFGNode = XCAFDoc_GraphNode::Set(theL.Value(i));
    }
    aFGNode->SetGraphID(XCAFDoc::GeomToleranceRefGUID());
    aFGNode->SetChild(aChGNode);
    aChGNode->SetFather(aFGNode);
  }
}

//=======================================================================
//function : SetDimTol
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::SetDimTol(const TDF_Label& theL,
                                   const TDF_Label& theDimTolL) const
{
  // set reference
  Handle(TDataStd_TreeNode) refNode, mainNode;
  refNode = TDataStd_TreeNode::Set ( theDimTolL, XCAFDoc::DimTolRefGUID() );
  mainNode  = TDataStd_TreeNode::Set (theL,       XCAFDoc::DimTolRefGUID() );
  refNode->Remove(); // abv: fix against bug in TreeNode::Append()
  mainNode->Append(refNode);
}


//=======================================================================
//function : SetDimTol
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DimTolTool::SetDimTol(const TDF_Label& L,
                                        const Standard_Integer kind,
                                        const Handle(TColStd_HArray1OfReal)& aVal,
                                        const Handle(TCollection_HAsciiString)& aName,
                                        const Handle(TCollection_HAsciiString)& aDescription) const
{
  TDF_Label DimTolL = AddDimTol(kind,aVal,aName,aDescription);
  SetDimTol(L,DimTolL);
  return DimTolL;
}


//=======================================================================
//function : GetRefShapeLabel
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::GetRefShapeLabel(const TDF_Label& theL,
                                                      TDF_LabelSequence& theShapeLFirst,
                                                      TDF_LabelSequence& theShapeLSecond) const
{
  theShapeLFirst.Clear();
  theShapeLSecond.Clear();
  Handle(TDataStd_TreeNode) aNode;
  if( !theL.FindAttribute(XCAFDoc::DimTolRefGUID(),aNode) || !aNode->HasFather() ) {
    if( !theL.FindAttribute(XCAFDoc::DatumRefGUID(),aNode) || !aNode->HasFather() ) {
      Handle(XCAFDoc_GraphNode) aGNode;
      if( theL.FindAttribute(XCAFDoc::GeomToleranceRefGUID(),aGNode) && aGNode->NbFathers() > 0 ) {
        for(Standard_Integer i = 1; i <= aGNode->NbFathers(); i++)
        {
          theShapeLFirst.Append(aGNode->GetFather(i)->Label());
        }
        return Standard_True;
      }
      else if (theL.FindAttribute(XCAFDoc::DatumRefGUID(), aGNode) && aGNode->NbFathers() > 0) {
        for (Standard_Integer i = 1; i <= aGNode->NbFathers(); i++)
        {
          theShapeLFirst.Append(aGNode->GetFather(i)->Label());
        }
        return Standard_True;
      }
      else if( theL.FindAttribute(XCAFDoc::DimensionRefFirstGUID(),aGNode) && aGNode->NbFathers() > 0 ) {
        for(Standard_Integer i = 1; i <= aGNode->NbFathers(); i++)
        {
          theShapeLFirst.Append(aGNode->GetFather(i)->Label());
        }
        if( theL.FindAttribute(XCAFDoc::DimensionRefSecondGUID(),aGNode) && aGNode->NbFathers() > 0 ) {
          for(Standard_Integer i = 1; i <= aGNode->NbFathers(); i++)
          {
            theShapeLSecond.Append(aGNode->GetFather(i)->Label());
          }
        }
        return Standard_True;
      }
      else
      {
        return Standard_False;
      }
    }
  }

  theShapeLFirst.Append(aNode->Father()->Label());
  return Standard_True;
}

//=======================================================================
//function : GetRefDimensionLabels
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::GetRefDimensionLabels(const TDF_Label& theShapeL,
                                                     TDF_LabelSequence& theDimTols) const
{
  Handle(XCAFDoc_GraphNode) aGNode;
  Standard_Boolean aResult = Standard_False;
  if( theShapeL.FindAttribute(XCAFDoc::DimensionRefFirstGUID(),aGNode) && aGNode->NbChildren() > 0 ) {
    for(Standard_Integer i = 1; i <= aGNode->NbChildren(); i++)
    {
      theDimTols.Append(aGNode->GetChild(i)->Label());
    }
    aResult = Standard_True;
  }
  if( theShapeL.FindAttribute(XCAFDoc::DimensionRefSecondGUID(),aGNode) && aGNode->NbChildren() > 0 ) {
    for(Standard_Integer i = 1; i <= aGNode->NbChildren(); i++)
    {
      theDimTols.Append(aGNode->GetChild(i)->Label());
    }
    aResult = Standard_True;
  }
  return aResult;
}

//=======================================================================
//function : GetRefGeomToleranceLabels
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::GetRefGeomToleranceLabels(const TDF_Label& theShapeL,
                                                     TDF_LabelSequence& theDimTols) const
{
  Handle(XCAFDoc_GraphNode) aGNode;
  if( !theShapeL.FindAttribute(XCAFDoc::GeomToleranceRefGUID(),aGNode) ||
    aGNode->NbChildren() == 0 ) {
    return Standard_False;
  }
  for(Standard_Integer i = 1; i <= aGNode->NbChildren(); i++)
  {
    theDimTols.Append(aGNode->GetChild(i)->Label());
  }
  return Standard_True;
}

//=======================================================================
//function : GetRefDatumLabel
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::GetRefDatumLabel(const TDF_Label& theShapeL,
                                                     TDF_LabelSequence& theDatum) const
{
  Handle(XCAFDoc_GraphNode) aGNode;
  if (!theShapeL.FindAttribute(XCAFDoc::DatumRefGUID(), aGNode)) {
    return Standard_False;
  }
  for (Standard_Integer i = 1; i <= aGNode->NbChildren(); i++) {
    theDatum.Append(aGNode->GetChild(i)->Label());
  }
  return Standard_True;
}

//=======================================================================
//function : GetDimTol
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::GetDimTol(const TDF_Label& DimTolL,
                                               Standard_Integer& kind,
                                               Handle(TColStd_HArray1OfReal)& aVal,
                                               Handle(TCollection_HAsciiString)& aName,
                                               Handle(TCollection_HAsciiString)& aDescription) const
{
  Handle(XCAFDoc_DimTol) DimTolAttr;
  if(!DimTolL.FindAttribute(XCAFDoc_DimTol::GetID(),DimTolAttr)) {
    return Standard_False;
  }
  kind = DimTolAttr->GetKind();
  aVal = DimTolAttr->GetVal();
  aName = DimTolAttr->GetName();
  aDescription = DimTolAttr->GetDescription();
  
  return Standard_True;
}


//=======================================================================
//function : IsDatum
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::IsDatum(const TDF_Label& theDimTolL) const
{
  Handle(XCAFDoc_Datum) aDatumAttr;
  if(theDimTolL.FindAttribute(XCAFDoc_Datum::GetID(),aDatumAttr)) {
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : GetDatumLabels
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::GetDatumLabels(TDF_LabelSequence& theLabels) const
{
  theLabels.Clear();
  TDF_ChildIterator aChildIterator( Label() ); 
  for (; aChildIterator.More(); aChildIterator.Next()) {
    TDF_Label L = aChildIterator.Value();
    if ( IsDatum(L)) theLabels.Append(L);
  }
}

//=======================================================================
//function : FindDatum
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::FindDatum(const Handle(TCollection_HAsciiString)& aName,
                                               const Handle(TCollection_HAsciiString)& aDescription,
                                               const Handle(TCollection_HAsciiString)& anIdentification,
                                               TDF_Label& lab) const
{
  TDF_ChildIDIterator it(Label(),XCAFDoc_Datum::GetID());
  for(; it.More(); it.Next()) {
    Handle(TCollection_HAsciiString) aName1, aDescription1, anIdentification1;
    TDF_Label aLabel = it.Value()->Label();
    if ( !GetDatum( aLabel, aName1, aDescription1, anIdentification1 ) )
      continue;
    if(!(aName==aName1)) continue;
    if(!(aDescription==aDescription1)) continue;
    if(!(anIdentification==anIdentification1)) continue;
    lab = aLabel;
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : AddDatum
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DimTolTool::AddDatum(const Handle(TCollection_HAsciiString)& aName,
                                       const Handle(TCollection_HAsciiString)& aDescription,
                                       const Handle(TCollection_HAsciiString)& anIdentification) const
{
  TDF_Label DatumL;
  TDF_TagSource aTag;
  DatumL = aTag.NewChild ( Label() );
  XCAFDoc_Datum::Set(DatumL,aName,aDescription,anIdentification);
  TDataStd_Name::Set(DatumL,"DGT:Datum");
  return DatumL;
}

//=======================================================================
//function : AddDatum
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DimTolTool::AddDatum()
{
  TDF_Label aDatumL;
  TDF_TagSource aTag;
  aDatumL = aTag.NewChild ( Label() );
  Handle(XCAFDoc_Datum) aDat = XCAFDoc_Datum::Set(aDatumL);
  TDataStd_Name::Set(aDatumL,"DGT:Datum");
  return aDatumL;
}

//=======================================================================
//function : SetDatum
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::SetDatum(const TDF_LabelSequence& theL,
                                  const TDF_Label& theDatumL) const
{
  if (!IsDatum(theDatumL))
  {
    return;
  }

  Handle(XCAFDoc_GraphNode) aChGNode;
  Handle(XCAFDoc_GraphNode) aFGNode;

  if (theDatumL.FindAttribute(XCAFDoc::DatumRefGUID(), aChGNode)) {
    while (aChGNode->NbFathers() > 0) {
      aFGNode = aChGNode->GetFather(1);
      aFGNode->UnSetChild(aChGNode);
      if (aFGNode->NbChildren() == 0)
        aFGNode->ForgetAttribute(XCAFDoc::DatumRefGUID());
    }
    theDatumL.ForgetAttribute(XCAFDoc::DatumRefGUID());
  }

  if (!theDatumL.FindAttribute(XCAFDoc::DatumRefGUID(), aChGNode)) {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theDatumL);
    aChGNode->SetGraphID(XCAFDoc::DatumRefGUID());
  }
  for (Standard_Integer i = theL.Lower(); i <= theL.Upper(); i++)
  {
    if (!theL.Value(i).FindAttribute(XCAFDoc::DatumRefGUID(), aFGNode)) {
      aFGNode = new XCAFDoc_GraphNode;
      aFGNode = XCAFDoc_GraphNode::Set(theL.Value(i));
    }
    aFGNode->SetGraphID(XCAFDoc::DatumRefGUID());
    aFGNode->SetChild(aChGNode);
    aChGNode->SetFather(aFGNode);
  }
}

//=======================================================================
//function : SetDatum
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::SetDatum(const TDF_Label& L,
                                  const TDF_Label& TolerL,
                                  const Handle(TCollection_HAsciiString)& aName,
                                  const Handle(TCollection_HAsciiString)& aDescription,
                                  const Handle(TCollection_HAsciiString)& anIdentification) const
{
  TDF_Label DatumL;
  if(!FindDatum(aName,aDescription,anIdentification,DatumL))
    DatumL = AddDatum(aName,aDescription,anIdentification);
  TDF_LabelSequence aLabels;
  aLabels.Append(L);
  SetDatum(aLabels,DatumL);
  // set reference
  Handle(XCAFDoc_GraphNode) FGNode;
  Handle(XCAFDoc_GraphNode) ChGNode;
  if (! TolerL.FindAttribute( XCAFDoc::DatumTolRefGUID(), FGNode) ) {
    FGNode = new XCAFDoc_GraphNode;
    FGNode = XCAFDoc_GraphNode::Set(TolerL);
  }
  if (! DatumL.FindAttribute( XCAFDoc::DatumTolRefGUID(), ChGNode) ) {
    ChGNode = new XCAFDoc_GraphNode;
    ChGNode = XCAFDoc_GraphNode::Set(DatumL);
  }
  FGNode->SetGraphID( XCAFDoc::DatumTolRefGUID() );
  ChGNode->SetGraphID( XCAFDoc::DatumTolRefGUID() );
  FGNode->SetChild(ChGNode);
  ChGNode->SetFather(FGNode);
}

//=======================================================================
//function : SetDatumToGeomTol
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::SetDatumToGeomTol(const TDF_Label& theDatumL,
                                           const TDF_Label& theTolerL) const
{
  // set reference
  Handle(XCAFDoc_GraphNode) aFGNode;
  Handle(XCAFDoc_GraphNode) aChGNode;
  if (! theTolerL.FindAttribute( XCAFDoc::DatumTolRefGUID(), aFGNode) ) {
    aFGNode = new XCAFDoc_GraphNode;
    aFGNode = XCAFDoc_GraphNode::Set(theTolerL);
  }
  if (! theDatumL.FindAttribute( XCAFDoc::DatumTolRefGUID(), aChGNode) ) {
    aChGNode = new XCAFDoc_GraphNode;
    aChGNode = XCAFDoc_GraphNode::Set(theDatumL);
  }
  aFGNode->SetGraphID( XCAFDoc::DatumTolRefGUID() );
  aChGNode->SetGraphID( XCAFDoc::DatumTolRefGUID() );
  aFGNode->SetChild(aChGNode);
  aChGNode->SetFather(aFGNode);
}

//=======================================================================
//function : GetDatum
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::GetDatum(const TDF_Label& theDatumL,
                                              Handle(TCollection_HAsciiString)& theName,
                                              Handle(TCollection_HAsciiString)& theDescription,
                                              Handle(TCollection_HAsciiString)& theIdentification) const
{
  Handle(XCAFDoc_Datum) aDatumAttr;
  if( theDatumL.IsNull() || 
      !theDatumL.FindAttribute(XCAFDoc_Datum::GetID(),aDatumAttr) )
    return Standard_False;
  
  theName = aDatumAttr->GetName();
  theDescription = aDatumAttr->GetDescription();
  theIdentification = aDatumAttr->GetIdentification();
  return Standard_True;
}

//=======================================================================
//function : GetDatumTolerLabels
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::GetDatumOfTolerLabels(const TDF_Label& theDimTolL,
                                                         TDF_LabelSequence& theDatums) const
{
  Handle(XCAFDoc_GraphNode) aNode;
  if( !theDimTolL.FindAttribute(XCAFDoc::DatumTolRefGUID(),aNode) )
    return Standard_False;

  for(Standard_Integer i=1; i<=aNode->NbChildren(); i++) {
    Handle(XCAFDoc_GraphNode) aDatumNode = aNode->GetChild(i);
    theDatums.Append(aDatumNode->Label());
  }
  return Standard_True;
}

//=======================================================================
//function : GetDatumWthObjectsTolerLabels
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::GetDatumWithObjectOfTolerLabels(const TDF_Label& theDimTolL,
                                                                     TDF_LabelSequence& theDatums) const
{
  Handle(XCAFDoc_GraphNode) aNode;
  if( !theDimTolL.FindAttribute(XCAFDoc::DatumTolRefGUID(),aNode) )
    return Standard_False;

  TColStd_MapOfAsciiString aDatumNameMap;
  for(Standard_Integer i=1; i<=aNode->NbChildren(); i++) {
    Handle(XCAFDoc_GraphNode) aDatumNode = aNode->GetChild(i);
    TDF_Label aDatumL = aDatumNode->Label();
    Handle(XCAFDoc_Datum) aDatumAttr;
    if (!aDatumL.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr)) 
      continue;
    Handle(XCAFDimTolObjects_DatumObject) aDatumObj = aDatumAttr->GetObject();
    if (aDatumObj.IsNull())
      continue;
    Handle(TCollection_HAsciiString) aName = aDatumObj->GetName();
    if (!aDatumNameMap.Add(aName->String())) {
      // the datum has already been appended to sequence, due to one of its datum targets
      continue;
    }
    theDatums.Append(aDatumNode->Label());
  }
  return Standard_True;
}

//=======================================================================
//function : GetTolerDatumLabels
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::GetTolerOfDatumLabels(const TDF_Label& theDatumL,
                                                         TDF_LabelSequence& theTols) const
{
  Handle(XCAFDoc_GraphNode) aNode;
  if( !theDatumL.FindAttribute(XCAFDoc::DatumTolRefGUID(),aNode) )
    return Standard_False;
  for(Standard_Integer i=1; i<=aNode->NbFathers(); i++) {
    Handle(XCAFDoc_GraphNode) aDatumNode = aNode->GetFather(i);
    theTols.Append(aDatumNode->Label());
  }
  return Standard_True;
}

//=======================================================================
//function : IsLocked
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::IsLocked(const TDF_Label& theViewL) const
{
  Handle(TDataStd_UAttribute) anAttr;
  return theViewL.FindAttribute(XCAFDoc::LockGUID(), anAttr);
}

//=======================================================================
//function : Lock
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::Lock(const TDF_Label& theViewL) const
{
  TDataStd_UAttribute::Set(theViewL, XCAFDoc::LockGUID());
}

//=======================================================================
//function : Unlock
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::Unlock(const TDF_Label& theViewL) const
{
  theViewL.ForgetAttribute(XCAFDoc::LockGUID());
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_DimTolTool::ID() const
{
  return GetID();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::Restore(const Handle(TDF_Attribute)& /*with*/) 
{
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_DimTolTool::NewEmpty() const
{
  return new XCAFDoc_DimTolTool;
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::Paste(const Handle(TDF_Attribute)& /*into*/,
                               const Handle(TDF_RelocationTable)& /*RT*/) const
{
}

