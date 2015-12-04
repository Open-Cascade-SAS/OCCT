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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_LayerTool,TDF_Attribute)

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
  return UnSetOneLayer (L, alab);
}

//=======================================================================
//function : UnSetOneLayer
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::UnSetOneLayer(const TDF_Label& L,
						  const TDF_Label& aLayerL) const
{
  Handle(XCAFDoc_GraphNode) FGNode, ChGNode;
  if ( !L.FindAttribute (XCAFDoc::LayerRefGUID(), ChGNode) ) return Standard_False;
  if ( !aLayerL.FindAttribute (XCAFDoc::LayerRefGUID(), FGNode) ) return Standard_False;
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
//function : IsSet
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::IsSet(const TDF_Label& L,
					  const TDF_Label& aLayerL) const
{
  Handle(XCAFDoc_GraphNode) Node;
  Handle(TDataStd_Name) aName;
  TDF_Label lab;
  if (L.FindAttribute(XCAFDoc::LayerRefGUID(), Node) && (Node->NbFathers() != 0 ) ) {
    Standard_Integer i = 1;
    for (; i <= Node->NbFathers(); i++) {
      lab = Node->GetFather(i)->Label();
      if (lab == aLayerL)
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
  return (aLayerS->Length() != 0);
}

//=======================================================================
//function : GetLayers
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::GetLayers(const TDF_Label& L,
					      TDF_LabelSequence& aLayerLS) 
{
  aLayerLS.Clear();
  Handle(XCAFDoc_GraphNode) aGNode;
  if ( L.FindAttribute( XCAFDoc::LayerRefGUID(), aGNode) ) {
    for (Standard_Integer i = 1; i <= aGNode->NbFathers(); i++) {
      aLayerLS.Append (aGNode->GetFather(i)->Label());
    }
  }
  return aLayerLS.Length() > 0;
}

//=======================================================================
//function : GetLayers
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfExtendedString) XCAFDoc_LayerTool::GetLayers(const TDF_Label& L) 
{
  Handle(TColStd_HSequenceOfExtendedString) aLayerS = new TColStd_HSequenceOfExtendedString;
  TDF_LabelSequence aLayerLS;
  if ( GetLayers( L, aLayerLS ) ) {
    for (Standard_Integer i = 1; i <= aLayerLS.Length(); ++i) {
      const TDF_Label& aLab = aLayerLS(i);
      Handle(TDataStd_Name) aName;
      if ( aLab.FindAttribute( TDataStd_Name::GetID(), aName ) ) {
        aLayerS->Append( aName->Get() );
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
//function : UnSetOneLayer
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::UnSetOneLayer(const TopoDS_Shape& Sh,
						  const TDF_Label& aLayerL)
{
  TDF_Label aLab;
  if (! myShapeTool->FindShape(Sh, aLab) ) return Standard_False;
  return UnSetOneLayer(aLab, aLayerL);
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
//function : IsSet
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_LayerTool::IsSet(const TopoDS_Shape& Sh,
					  const TDF_Label& aLayerL) 
{
  TDF_Label aLab;
  if (! myShapeTool->FindShape(Sh, aLab) ) return Standard_False;
  return IsSet(aLab, aLayerL);
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

Standard_Boolean XCAFDoc_LayerTool::GetLayers(const TopoDS_Shape& Sh,
					      TDF_LabelSequence& aLayerLS) 
{
  TDF_Label aLab;
  if (! myShapeTool->FindShape(Sh, aLab) ) return Standard_False;
  return GetLayers(aLab, aLayerLS);
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

