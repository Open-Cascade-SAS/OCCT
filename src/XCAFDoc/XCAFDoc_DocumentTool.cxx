// Created on: 2000-08-30
// Created by: data exchange team
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
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ClippingPlaneTool.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_NotesTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ViewTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_DocumentTool,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& XCAFDoc_DocumentTool::GetID() 
{
  static Standard_GUID DocumentToolID ("efd212ec-6dfd-11d4-b9c8-0060b0ee281b");
  return DocumentToolID; 
}

namespace {
//=======================================================================
//function : GetRefID
//purpose  : Returns a reference id to find a tree node attribute at the root
//           label
//=======================================================================

static const Standard_GUID& GetDocumentToolRefID() 
{
  static Standard_GUID DocumentToolRefID ("efd212eb-6dfd-11d4-b9c8-0060b0ee281b");
  return DocumentToolRefID; 
}
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_DocumentTool) XCAFDoc_DocumentTool::Set(const TDF_Label& L,
                                                       const Standard_Boolean IsAcces)
{
  Handle(XCAFDoc_DocumentTool) A;
  TDF_Label aL = DocLabel (L);
  if (!aL.FindAttribute (XCAFDoc_DocumentTool::GetID(), A)) {
    if (!IsAcces)
      aL = L;

    A = new XCAFDoc_DocumentTool;
    aL.AddAttribute(A);
    A->Init();
    // set ShapeTool, ColorTool and LayerTool attributes
    XCAFDoc_ShapeTool::Set(ShapesLabel(L));
    XCAFDoc_ColorTool::Set(ColorsLabel(L));
    XCAFDoc_LayerTool::Set(LayersLabel(L));
    XCAFDoc_DimTolTool::Set(DGTsLabel(L));
    XCAFDoc_MaterialTool::Set(MaterialsLabel(L));
    XCAFDoc_NotesTool::Set(NotesLabel(L));
    XCAFDoc_ViewTool::Set(ViewsLabel(L));
    XCAFDoc_ClippingPlaneTool::Set(ClippingPlanesLabel(L));
  }
  return A;
}


//=======================================================================
//function : DocLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::DocLabel(const TDF_Label& acces) 
{
  TDF_Label DocL, RootL = acces.Root();
  const Standard_GUID& aRefGuid = GetDocumentToolRefID();
  Handle(TDataStd_TreeNode) aRootNode, aLabNode;

  if (RootL.FindAttribute (aRefGuid, aRootNode)) {
    aLabNode = aRootNode->First();
    DocL = aLabNode->Label();
    return DocL;
  }

  DocL = RootL.FindChild(1);
  return DocL;
}


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

XCAFDoc_DocumentTool::XCAFDoc_DocumentTool()
{
}


//=======================================================================
//function : ShapesLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::ShapesLabel(const TDF_Label& acces) 
{
  TDF_Label L = DocLabel(acces).FindChild(1,Standard_True);
  TDataStd_Name::Set(L, "Shapes");
  return L;
}


//=======================================================================
//function : ColorsLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::ColorsLabel(const TDF_Label& acces) 
{
  TDF_Label L = DocLabel(acces).FindChild(2,Standard_True);
  TDataStd_Name::Set(L, "Colors");
  return L;
}


//=======================================================================
//function : LayersLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::LayersLabel(const TDF_Label& acces) 
{
  TDF_Label L = DocLabel(acces).FindChild(3,Standard_True);
  TDataStd_Name::Set(L, "Layers");
  return L;
}


//=======================================================================
//function : DGTsLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::DGTsLabel(const TDF_Label& acces) 
{
  TDF_Label L = DocLabel(acces).FindChild(4,Standard_True);
  TDataStd_Name::Set(L, "D&GTs");
  return L;
}


//=======================================================================
//function : MaterialsLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::MaterialsLabel(const TDF_Label& acces) 
{
  TDF_Label L = DocLabel(acces).FindChild(5,Standard_True);
  TDataStd_Name::Set(L, "Materials");
  return L;
}


//=======================================================================
//function : ViewsLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::ViewsLabel(const TDF_Label& acces)
{
  TDF_Label L = DocLabel(acces).FindChild(7, Standard_True);
  TDataStd_Name::Set(L, "Views");
  return L;
}

//=======================================================================
//function : ClippingPlanesLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::ClippingPlanesLabel(const TDF_Label& acces)
{
  TDF_Label L = DocLabel(acces).FindChild(8, Standard_True);
  TDataStd_Name::Set(L, "Clipping Planes");
  return L;
}

//=======================================================================
//function : NotesLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::NotesLabel(const TDF_Label& acces)
{
  TDF_Label L = DocLabel(acces).FindChild(9, Standard_True);
  TDataStd_Name::Set(L, "Notes");
  return L;
}

//=======================================================================
//function : ShapeTool
//purpose  : 
//=======================================================================

 Handle(XCAFDoc_ShapeTool) XCAFDoc_DocumentTool::ShapeTool(const TDF_Label& acces) 
{
  return XCAFDoc_ShapeTool::Set(ShapesLabel(acces));
}


//=======================================================================
//function : ColorTool
//purpose  : 
//=======================================================================

Handle(XCAFDoc_ColorTool) XCAFDoc_DocumentTool::ColorTool (const TDF_Label& acces) 
{
  return XCAFDoc_ColorTool::Set(ColorsLabel(acces));
}


//=======================================================================
//function : LayerTool
//purpose  : 
//=======================================================================

Handle(XCAFDoc_LayerTool) XCAFDoc_DocumentTool::LayerTool (const TDF_Label& acces) 
{
  return XCAFDoc_LayerTool::Set(LayersLabel(acces));
}


//=======================================================================
//function : DimTolTool
//purpose  : 
//=======================================================================

Handle(XCAFDoc_DimTolTool) XCAFDoc_DocumentTool::DimTolTool(const TDF_Label& acces) 
{
  return XCAFDoc_DimTolTool::Set(DGTsLabel(acces));
}


//=======================================================================
//function : MaterialTool
//purpose  : 
//=======================================================================

Handle(XCAFDoc_MaterialTool) XCAFDoc_DocumentTool::MaterialTool(const TDF_Label& acces) 
{
  return XCAFDoc_MaterialTool::Set(MaterialsLabel(acces));
}

//=======================================================================
//function : ViewTool
//purpose  : 
//=======================================================================

Handle(XCAFDoc_ViewTool) XCAFDoc_DocumentTool::ViewTool(const TDF_Label& acces)
{
  return XCAFDoc_ViewTool::Set(ViewsLabel(acces));
}

//=======================================================================
//function : ClippingPlaneTool
//purpose  : 
//=======================================================================

Handle(XCAFDoc_ClippingPlaneTool) XCAFDoc_DocumentTool::ClippingPlaneTool(const TDF_Label& acces)
{
  return XCAFDoc_ClippingPlaneTool::Set(ClippingPlanesLabel(acces));
}

//=======================================================================
//function : ClippingPlaneTool
//purpose  : 
//=======================================================================

Handle(XCAFDoc_NotesTool) XCAFDoc_DocumentTool::NotesTool(const TDF_Label& acces)
{
  return XCAFDoc_NotesTool::Set(NotesLabel(acces));
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_DocumentTool::ID() const
{
  return GetID();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_DocumentTool::Restore(const Handle(TDF_Attribute)& /* with */) 
{
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_DocumentTool::NewEmpty() const
{
  return new XCAFDoc_DocumentTool;
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_DocumentTool::Paste (const Handle(TDF_Attribute)& /* into */,
				  const Handle(TDF_RelocationTable)& /* RT */) const
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void XCAFDoc_DocumentTool::Init() const
{
  TDF_Label DocL = Label(), RootL = DocL.Root();
  const Standard_GUID& aRefGuid = GetDocumentToolRefID();
  Handle(TDataStd_TreeNode) aRootNode, aLabNode;

  if (!RootL.FindAttribute (aRefGuid, aRootNode)) {
    Handle(TDataStd_TreeNode) aRootNodeNew = TDataStd_TreeNode::Set (RootL, aRefGuid);
    Handle(TDataStd_TreeNode) aLNode = TDataStd_TreeNode::Set (DocL, aRefGuid);
    aLNode->SetFather (aRootNodeNew);
    aRootNodeNew->SetFirst (aLNode);
  }
}


//=======================================================================
//function : IsXCAFDocument
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DocumentTool::IsXCAFDocument(const  Handle(TDocStd_Document)& D)
{
  TDF_Label RootL = D->Main().Root();
  const Standard_GUID& aRefGuid = GetDocumentToolRefID();
  Handle(TDataStd_TreeNode) aRootNode;
  return RootL.FindAttribute (aRefGuid, aRootNode);
}
