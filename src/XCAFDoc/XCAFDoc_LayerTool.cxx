// Created on: 2000-10-02
// Created by: Pavel TELKOV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <XCAFDoc_LayerTool.ixx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <XCAFDoc.hxx>
#include <TDataStd_UAttribute.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

XCAFDoc_LayerTool::XCAFDoc_LayerTool()
{
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_LayerTool) XCAFDoc_LayerTool::Set(const TDF_Label& L) 
{
  Handle(XCAFDoc_LayerTool) A;
  if (!L.FindAttribute (XCAFDoc_LayerTool::GetID(), A)) {
    A = new XCAFDoc_LayerTool ();
    L.AddAttribute(A);
    A->myShapeTool = XCAFDoc_DocumentTool::ShapeTool(L);
  }
  return A;
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_LayerTool::GetID() 
{
  static Standard_GUID LayerTblID ("efd212f4-6dfd-11d4-b9c8-0060b0ee281b");
  return LayerTblID; 
}


//=======================================================================
//function : BaseLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_LayerTool::BaseLabel() const
{
  return Label();
}


//=======================================================================
//function : ShapeTool
//purpose  : 
//=======================================================================

const Handle(XCAFDoc_ShapeTool)& XCAFDoc_LayerTool::ShapeTool() 
{
  if (myShapeTool.IsNull())
    myShapeTool = XCAFDoc_DocumentTool::ShapeTool( Label() );
  return myShapeTool;
}


//=======================================================================
//function : IsLayer
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::IsLayer(const TDF_Label& lab) const
{
  TCollection_ExtendedString aLayer;
  return GetLayer ( lab, aLayer);
}


//=======================================================================
//function : GetLayer
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::GetLayer(const TDF_Label& lab,
					     TCollection_ExtendedString& aLayer) const
{
  if ( lab.Father() != Label() ) return Standard_False;
//   Handle(XCAFDoc_GraphNode) aGN;
//   if (! lab.FindAttribute (XCAFDoc::LayerRefGUID(), aGN))
//     return Standard_False;
  Handle(TDataStd_Name) aName;
  Standard_Boolean status = Standard_False;
  if ( lab.FindAttribute (TDataStd_Name::GetID(), aName) ) {
    aLayer = aName->Get();
    status = Standard_True;
  }
  return status;
}

//=======================================================================
//function : FindLayer
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::FindLayer(const TCollection_ExtendedString& aLayer,
					      TDF_Label& lab) const
{
  lab = FindLayer(aLayer);
  return ( !lab.IsNull() );
}


//=======================================================================
//function : FindLayer
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_LayerTool::FindLayer(const TCollection_ExtendedString& aLayer) const
{
  TDF_ChildIterator it( Label() );
  TDF_Label lab;
  for (; it.More(); it.Next()) {
    TDF_Label aLabel = it.Value();
    Handle(TDataStd_Name) aName;
    if ( aLabel.FindAttribute (TDataStd_Name::GetID(), aName) && (aName->Get().IsEqual(aLayer)) ) {
      lab =  aLabel;
      break;
    }
  }
  return lab;
}


//=======================================================================
//function : AddLayer
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_LayerTool::AddLayer(const TCollection_ExtendedString& aLayer) const
{
  TDF_Label lab;
  if ( FindLayer(aLayer, lab) )
    return lab;
  TDF_TagSource aTag;
  TDF_Label aLabel = aTag.NewChild( Label() );
  Handle(TDataStd_Name) aName = new TDataStd_Name;
  aName->Set(aLabel, aLayer);
  return aLabel;
}


//=======================================================================
//function : RemoveLayer
//purpose  : 
//=======================================================================

void XCAFDoc_LayerTool::RemoveLayer(const TDF_Label& lab) const
{
  lab.ForgetAllAttributes (Standard_True);
}


//=======================================================================
//function : GetLayerLabels
//purpose  : 
//=======================================================================

void XCAFDoc_LayerTool::GetLayerLabels(TDF_LabelSequence& Labels) const
{
  Labels.Clear();
  TDF_ChildIterator ChildIterator( Label() ); 
  for (; ChildIterator.More(); ChildIterator.Next()) {
    TDF_Label L = ChildIterator.Value();
    if ( IsLayer(L)) Labels.Append(L);
  }
}


//=======================================================================
//function : SetLayer
//purpose  : 
//=======================================================================

void XCAFDoc_LayerTool::SetLayer(const TDF_Label& L,
				 const TDF_Label& LayerL,
				 const Standard_Boolean shapeInOneLayer) const
{
  if (shapeInOneLayer) UnSetLayers( L );
  Handle(XCAFDoc_GraphNode) FGNode;
  Handle(XCAFDoc_GraphNode) ChGNode;
  if (! LayerL.FindAttribute( XCAFDoc::LayerRefGUID(), FGNode) ) {
    FGNode = new XCAFDoc_GraphNode;
    FGNode = XCAFDoc_GraphNode::Set(LayerL);
  }
  if (! L.FindAttribute( XCAFDoc::LayerRefGUID(), ChGNode) ) {
    ChGNode = new XCAFDoc_GraphNode;
    ChGNode = XCAFDoc_GraphNode::Set(L);
  }
  FGNode->SetGraphID( XCAFDoc::LayerRefGUID() );
  ChGNode->SetGraphID( XCAFDoc::LayerRefGUID() );
  FGNode->SetChild(ChGNode);
  ChGNode->SetFather(FGNode);
}


//=======================================================================
//function : SetLayer
//purpose  : 
//=======================================================================

void XCAFDoc_LayerTool::SetLayer(const TDF_Label& L,
				 const TCollection_ExtendedString& aLayer,
 				 const Standard_Boolean shapeInOneLayer) const
{
  TDF_Label aLayerL = AddLayer(aLayer);
  SetLayer(L, aLayerL, shapeInOneLayer);
}


//=======================================================================
//function : UnSetLayers
//purpose  : 
//=======================================================================

void XCAFDoc_LayerTool::UnSetLayers(const TDF_Label& L) const
{
  Handle(XCAFDoc_GraphNode) ChGNode, FGNode;
  if ( L.FindAttribute (XCAFDoc::LayerRefGUID(), ChGNode) ) {
    while (ChGNode->NbFathers()!= 0) {
      FGNode = ChGNode->GetFather(1);
      FGNode-> UnSetChild(ChGNode);
//       ChGNode->GetFather(1)->UnSetChild(ChGNode);
    }
    L.ForgetAttribute ( XCAFDoc::LayerRefGUID() );
  }
}


//=======================================================================
//function : UnSetOneLayer
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::UnSetOneLayer(const TDF_Label& L,
						  const TCollection_ExtendedString& aLayer) const
{
  TDF_Label alab;
  if ( !FindLayer(aLayer, alab) ) return Standard_False;
  
  Handle(XCAFDoc_GraphNode) FGNode, ChGNode;
  if ( !L.FindAttribute (XCAFDoc::LayerRefGUID(), ChGNode) ) return Standard_False;
  if ( !alab.FindAttribute (XCAFDoc::LayerRefGUID(), FGNode) ) return Standard_False;
  ChGNode->UnSetFather(FGNode);
  return Standard_True;
}
//=======================================================================
//function : IsSet
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::IsSet(const TDF_Label& L,
					  const TCollection_ExtendedString& aLayer) const
{
  Handle(XCAFDoc_GraphNode) Node;
  Handle(TDataStd_Name) aName;
  TDF_Label lab;
  if (L.FindAttribute(XCAFDoc::LayerRefGUID(), Node) && (Node->NbFathers() != 0 ) ) {
    Standard_Integer i = 1;
    for (; i <= Node->NbFathers(); i++) {
      lab = Node->GetFather(i)->Label();
      if (lab.FindAttribute(TDataStd_Name::GetID(), aName) && ( aName->Get().IsEqual(aLayer) ) )
	return Standard_True;
    }
  }
  return Standard_False;
}


//=======================================================================
//function : GetLayers
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::GetLayers(const TDF_Label& L,
					      Handle(TColStd_HSequenceOfExtendedString)& aLayerS) 
{
  aLayerS = GetLayers(L);
  if (aLayerS->Length() != 0) return Standard_True;
  return Standard_False;
}


//=======================================================================
//function : GetLayers
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfExtendedString) XCAFDoc_LayerTool::GetLayers(const TDF_Label& L) 
{
  Handle(TColStd_HSequenceOfExtendedString) aLayerS = new TColStd_HSequenceOfExtendedString;
  Handle(XCAFDoc_GraphNode) aGNode;
  if ( L.FindAttribute( XCAFDoc::LayerRefGUID(), aGNode) ) {
    if ( aGNode->NbFathers() == 0 ) {
      return aLayerS;
    }
    Standard_Integer i = 1;
    TDF_Label aLab;
    Handle(TDataStd_Name) aName;
    for (; i <= aGNode->NbFathers(); i++) {
      aLab = aGNode->GetFather(i)->Label();
      if ( aLab.FindAttribute(TDataStd_Name::GetID(), aName) ) {
	aLayerS->Append( aName->Get() );
// 	cout << aName->Get() <<endl;
      }
    }
  }
  return aLayerS;
}

//=======================================================================
//function : GetShapesOfLayer
//purpose  : 
//=======================================================================

void XCAFDoc_LayerTool::GetShapesOfLayer(const TDF_Label& layerL,
					 TDF_LabelSequence& ShLabels) const
{
  ShLabels.Clear();
  Handle(XCAFDoc_GraphNode) aGNode;
  if ( layerL.FindAttribute( XCAFDoc::LayerRefGUID(), aGNode) ) {
    for (Standard_Integer i = 1; i <= aGNode->NbChildren(); i++) {
      ShLabels.Append( aGNode->GetChild(i)->Label() );
    }
  }
}


//=======================================================================
//function : IsVisible
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::IsVisible (const TDF_Label& layerL) const
{
  Handle(TDataStd_UAttribute) aUAttr;
  return (!layerL.FindAttribute(XCAFDoc::InvisibleGUID(), aUAttr));
}


//=======================================================================
//function : SetVisibility
//purpose  : 
//=======================================================================

void XCAFDoc_LayerTool::SetVisibility (const TDF_Label& layerL,
				       const Standard_Boolean isvisible) const
{
  Handle(TDataStd_UAttribute) aUAttr;
  if (! isvisible ) {
    if (!layerL.FindAttribute(XCAFDoc::InvisibleGUID(), aUAttr)) {
      aUAttr->Set( layerL, XCAFDoc::InvisibleGUID() );
    }
  }
  else layerL.ForgetAttribute( XCAFDoc::InvisibleGUID() );
}
       

//=======================================================================
//function : SetLayer
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::SetLayer(const TopoDS_Shape& Sh,
					     const TDF_Label& LayerL, 
					     const Standard_Boolean shapeInOneLayer)
{
  TDF_Label aLab;
//   if (! myShapeTool->FindShape(Sh, aLab) ) return Standard_False;
  // PTV 22.01.2003 set layer for shape with location if it is necessary
  if (! myShapeTool->Search( Sh, aLab ) ) return Standard_False;
  SetLayer(aLab, LayerL, shapeInOneLayer);
  return Standard_True;
}


//=======================================================================
//function : SetLayer
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::SetLayer(const TopoDS_Shape& Sh,
					     const TCollection_ExtendedString& aLayer,
					     const Standard_Boolean shapeInOneLayer)
{
  TDF_Label aLayerL = AddLayer(aLayer);
  return SetLayer(Sh, aLayerL, shapeInOneLayer);
}


//=======================================================================
//function : UnSetLayers
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::UnSetLayers(const TopoDS_Shape& Sh) 
{
  TDF_Label aLab;
  if (! myShapeTool->FindShape(Sh, aLab) ) return Standard_False;
  UnSetLayers(aLab);
  return Standard_True;
}


//=======================================================================
//function : UnSetOneLayer
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::UnSetOneLayer(const TopoDS_Shape& Sh,
						  const TCollection_ExtendedString& aLayer)
{
  TDF_Label aLab;
  if (! myShapeTool->FindShape(Sh, aLab) ) return Standard_False;
  return UnSetOneLayer(aLab, aLayer);
}

//=======================================================================
//function : IsSet
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::IsSet(const TopoDS_Shape& Sh,
					  const TCollection_ExtendedString& aLayer) 
{
  TDF_Label aLab;
  if (! myShapeTool->FindShape(Sh, aLab) ) return Standard_False;
  return IsSet(aLab, aLayer);
}


//=======================================================================
//function : GetLayers
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::GetLayers(const TopoDS_Shape& Sh,
					      Handle(TColStd_HSequenceOfExtendedString)& aLayerS) 
{
  TDF_Label aLab;
  if (! myShapeTool->FindShape(Sh, aLab) ) return Standard_False;
  return GetLayers(aLab, aLayerS);
}


//=======================================================================
//function : GetLayers
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfExtendedString) XCAFDoc_LayerTool::GetLayers(const TopoDS_Shape& Sh) 
{
  Handle(TColStd_HSequenceOfExtendedString) aLayerS = new TColStd_HSequenceOfExtendedString;
  TDF_Label aLab;
  if ( myShapeTool->FindShape(Sh, aLab) )
    aLayerS = GetLayers(aLab);
  return aLayerS;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_LayerTool::ID() const
{
  return GetID();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_LayerTool::Restore(const Handle(TDF_Attribute)& /* with */) 
{
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_LayerTool::NewEmpty() const
{
  return new XCAFDoc_LayerTool;
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_LayerTool::Paste(const Handle(TDF_Attribute)& /* into */,
			      const Handle(TDF_RelocationTable)& /* RT */) const
{
}

