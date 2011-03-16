#include <XCAFDoc_DimTolTool.ixx>

#include <XCAFDoc.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <Precision.hxx>


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

Standard_Boolean XCAFDoc_DimTolTool::IsDimTol(const TDF_Label& DimTolL) const
{
  Handle(XCAFDoc_DimTol) DimTolAttr;
  if(DimTolL.FindAttribute(XCAFDoc_DimTol::GetID(),DimTolAttr)) {
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : GetDimTolLabels
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::GetDimTolLabels(TDF_LabelSequence& Labels) const
{
  Labels.Clear();
  TDF_ChildIterator ChildIterator( Label() ); 
  for (; ChildIterator.More(); ChildIterator.Next()) {
    TDF_Label L = ChildIterator.Value();
    if ( IsDimTol(L)) Labels.Append(L);
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
//function : SetDimTol
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::SetDimTol(const TDF_Label& L,
                                   const TDF_Label& DimTolL) const
{
  // set reference
  Handle(TDataStd_TreeNode) refNode, mainNode;
//  mainNode = TDataStd_TreeNode::Set ( DimTolL, XCAFDoc::DimTolRefGUID() );
//  refNode  = TDataStd_TreeNode::Set ( L,       XCAFDoc::DimTolRefGUID() );
  refNode = TDataStd_TreeNode::Set ( DimTolL, XCAFDoc::DimTolRefGUID() );
  mainNode  = TDataStd_TreeNode::Set ( L,       XCAFDoc::DimTolRefGUID() );
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

Standard_Boolean XCAFDoc_DimTolTool::GetRefShapeLabel(const TDF_Label& L,
                                                      TDF_Label& ShapeL) const
{
  Handle(TDataStd_TreeNode) Node;
  if( !L.FindAttribute(XCAFDoc::DimTolRefGUID(),Node) || !Node->HasFather() ) {
    if( !L.FindAttribute(XCAFDoc::DatumRefGUID(),Node) || !Node->HasFather() ) {
      return Standard_False;
    }
  }
  ShapeL = Node->Father()->Label();
  return Standard_True;
}


//=======================================================================
//function : GetRefDGTLabels
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DimTolTool::GetRefDGTLabels(const TDF_Label& ShapeL,
                                                     TDF_LabelSequence &DimTols) const
{
  Handle(TDataStd_TreeNode) Node;
  if( !ShapeL.FindAttribute(XCAFDoc::DimTolRefGUID(),Node) ||
       !Node->HasFirst() ) {
    return Standard_False;
  }
  Handle(TDataStd_TreeNode) Last = Node->First();
  DimTols.Append(Last->Label());
  while(Last->HasNext()) {
    Last = Last->Next();
    DimTols.Append(Last->Label());
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

Standard_Boolean XCAFDoc_DimTolTool::IsDatum(const TDF_Label& DimTolL) const
{
  Handle(XCAFDoc_Datum) DatumAttr;
  if(DimTolL.FindAttribute(XCAFDoc_Datum::GetID(),DatumAttr)) {
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : GetDatumLabels
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::GetDatumLabels(TDF_LabelSequence& Labels) const
{
  Labels.Clear();
  TDF_ChildIterator ChildIterator( Label() ); 
  for (; ChildIterator.More(); ChildIterator.Next()) {
    TDF_Label L = ChildIterator.Value();
    if ( IsDatum(L)) Labels.Append(L);
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
//function : SetDatum
//purpose  : 
//=======================================================================

void XCAFDoc_DimTolTool::SetDatum(const TDF_Label& L,
                                  const TDF_Label& DatumL) const
{
  // set reference
  Handle(TDataStd_TreeNode) refNode, mainNode;
  refNode = TDataStd_TreeNode::Set ( DatumL, XCAFDoc::DatumRefGUID() );
  mainNode  = TDataStd_TreeNode::Set ( L, XCAFDoc::DatumRefGUID() );
  refNode->Remove();
  mainNode->Append(refNode);
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
  SetDatum(L,DatumL);
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

Standard_Boolean XCAFDoc_DimTolTool::GetDatumTolerLabels(const TDF_Label& DimTolL,
                                                         TDF_LabelSequence &Datums) const
{
  Handle(XCAFDoc_GraphNode) Node;
  if( !DimTolL.FindAttribute(XCAFDoc::DatumTolRefGUID(),Node) )
    return Standard_False;
  for(Standard_Integer i=1; i<=Node->NbChildren(); i++) {
    Handle(XCAFDoc_GraphNode) DatumNode = Node->GetChild(i);
    Datums.Append(DatumNode->Label());
  }
  return Standard_True;
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

