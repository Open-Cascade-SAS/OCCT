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

#include <XCAFDoc_DocumentTool.hxx>

#include <Standard_Type.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ClippingPlaneTool.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_LengthUnit.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_NotesTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ViewTool.hxx>
#include <XCAFDoc_VisMaterialTool.hxx>
#include <UnitsMethods.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(XCAFDoc_DocumentTool,
                                      TDataStd_GenericEmpty,
                                      "xcaf",
                                      "DocumentTool")

//=================================================================================================

const Standard_GUID& XCAFDoc_DocumentTool::GetID()
{
  static Standard_GUID DocumentToolID("efd212ec-6dfd-11d4-b9c8-0060b0ee281b");
  return DocumentToolID;
}

namespace
{
//=======================================================================
// function : GetRefID
// purpose  : Returns a reference id to find a tree node attribute at the root
//           label
//=======================================================================

static const Standard_GUID& GetDocumentToolRefID()
{
  static Standard_GUID DocumentToolRefID("efd212eb-6dfd-11d4-b9c8-0060b0ee281b");
  return DocumentToolRefID;
}
} // namespace

//=================================================================================================

occ::handle<XCAFDoc_DocumentTool> XCAFDoc_DocumentTool::Set(const TDF_Label&       L,
                                                       const bool IsAcces)
{
  occ::handle<XCAFDoc_DocumentTool> A;
  TDF_Label                    aL = DocLabel(L);
  if (!aL.FindAttribute(XCAFDoc_DocumentTool::GetID(), A))
  {
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

//=================================================================================================

TDF_Label XCAFDoc_DocumentTool::DocLabel(const TDF_Label& acces)
{
  TDF_Label                 DocL, RootL = acces.Root();
  const Standard_GUID&      aRefGuid = GetDocumentToolRefID();
  occ::handle<TDataStd_TreeNode> aRootNode, aLabNode;

  if (RootL.FindAttribute(aRefGuid, aRootNode))
  {
    aLabNode = aRootNode->First();
    DocL     = aLabNode->Label();
    return DocL;
  }

  DocL = RootL.FindChild(1);
  return DocL;
}

//=================================================================================================

XCAFDoc_DocumentTool::XCAFDoc_DocumentTool() {}

//=================================================================================================

TDF_Label XCAFDoc_DocumentTool::ShapesLabel(const TDF_Label& acces)
{
  TDF_Label L = DocLabel(acces).FindChild(1, true);
  TDataStd_Name::Set(L, "Shapes");
  return L;
}

//=================================================================================================

TDF_Label XCAFDoc_DocumentTool::ColorsLabel(const TDF_Label& acces)
{
  TDF_Label L = DocLabel(acces).FindChild(2, true);
  TDataStd_Name::Set(L, "Colors");
  return L;
}

//=================================================================================================

TDF_Label XCAFDoc_DocumentTool::LayersLabel(const TDF_Label& acces)
{
  TDF_Label L = DocLabel(acces).FindChild(3, true);
  TDataStd_Name::Set(L, "Layers");
  return L;
}

//=================================================================================================

TDF_Label XCAFDoc_DocumentTool::DGTsLabel(const TDF_Label& acces)
{
  TDF_Label L = DocLabel(acces).FindChild(4, true);
  TDataStd_Name::Set(L, "D&GTs");
  return L;
}

//=================================================================================================

TDF_Label XCAFDoc_DocumentTool::MaterialsLabel(const TDF_Label& acces)
{
  TDF_Label L = DocLabel(acces).FindChild(5, true);
  TDataStd_Name::Set(L, "Materials");
  return L;
}

//=================================================================================================

TDF_Label XCAFDoc_DocumentTool::ViewsLabel(const TDF_Label& acces)
{
  TDF_Label L = DocLabel(acces).FindChild(7, true);
  TDataStd_Name::Set(L, "Views");
  return L;
}

//=================================================================================================

TDF_Label XCAFDoc_DocumentTool::ClippingPlanesLabel(const TDF_Label& acces)
{
  TDF_Label L = DocLabel(acces).FindChild(8, true);
  TDataStd_Name::Set(L, "Clipping Planes");
  return L;
}

//=================================================================================================

TDF_Label XCAFDoc_DocumentTool::NotesLabel(const TDF_Label& acces)
{
  TDF_Label L = DocLabel(acces).FindChild(9, true);
  TDataStd_Name::Set(L, "Notes");
  return L;
}

//=================================================================================================

TDF_Label XCAFDoc_DocumentTool::VisMaterialLabel(const TDF_Label& theLabel)
{
  TDF_Label aLabel = DocLabel(theLabel).FindChild(10, true);
  TDataStd_Name::Set(aLabel, "VisMaterials");
  return aLabel;
}

//=================================================================================================

occ::handle<XCAFDoc_ShapeTool> XCAFDoc_DocumentTool::ShapeTool(const TDF_Label& acces)
{
  return XCAFDoc_ShapeTool::Set(ShapesLabel(acces));
}

//=================================================================================================

bool XCAFDoc_DocumentTool::CheckShapeTool(const TDF_Label& theAcces)
{
  TDF_Label aLabel = DocLabel(theAcces).FindChild(1, false);
  if (aLabel.IsNull())
  {
    return false;
  }
  return aLabel.IsAttribute(XCAFDoc_ShapeTool::GetID());
}

//=================================================================================================

occ::handle<XCAFDoc_ColorTool> XCAFDoc_DocumentTool::ColorTool(const TDF_Label& acces)
{
  return XCAFDoc_ColorTool::Set(ColorsLabel(acces));
}

//=================================================================================================

bool XCAFDoc_DocumentTool::CheckColorTool(const TDF_Label& theAcces)
{
  TDF_Label aLabel = DocLabel(theAcces).FindChild(2, false);
  if (aLabel.IsNull())
  {
    return false;
  }
  return aLabel.IsAttribute(XCAFDoc_ColorTool::GetID());
}

//=================================================================================================

occ::handle<XCAFDoc_VisMaterialTool> XCAFDoc_DocumentTool::VisMaterialTool(const TDF_Label& theLabel)
{
  return XCAFDoc_VisMaterialTool::Set(VisMaterialLabel(theLabel));
}

//=================================================================================================

bool XCAFDoc_DocumentTool::CheckVisMaterialTool(const TDF_Label& theAcces)
{
  TDF_Label aLabel = DocLabel(theAcces).FindChild(10, false);
  if (aLabel.IsNull())
  {
    return false;
  }
  return aLabel.IsAttribute(XCAFDoc_VisMaterialTool::GetID());
}

//=================================================================================================

occ::handle<XCAFDoc_LayerTool> XCAFDoc_DocumentTool::LayerTool(const TDF_Label& acces)
{
  return XCAFDoc_LayerTool::Set(LayersLabel(acces));
}

//=================================================================================================

bool XCAFDoc_DocumentTool::CheckLayerTool(const TDF_Label& theAcces)
{
  TDF_Label aLabel = DocLabel(theAcces).FindChild(3, false);
  if (aLabel.IsNull())
  {
    return false;
  }
  return aLabel.IsAttribute(XCAFDoc_LayerTool::GetID());
}

//=================================================================================================

occ::handle<XCAFDoc_DimTolTool> XCAFDoc_DocumentTool::DimTolTool(const TDF_Label& acces)
{
  return XCAFDoc_DimTolTool::Set(DGTsLabel(acces));
}

//=================================================================================================

bool XCAFDoc_DocumentTool::CheckDimTolTool(const TDF_Label& theAcces)
{
  TDF_Label aLabel = DocLabel(theAcces).FindChild(4, false);
  if (aLabel.IsNull())
  {
    return false;
  }
  return aLabel.IsAttribute(XCAFDoc_DimTolTool::GetID());
}

//=================================================================================================

occ::handle<XCAFDoc_MaterialTool> XCAFDoc_DocumentTool::MaterialTool(const TDF_Label& acces)
{
  return XCAFDoc_MaterialTool::Set(MaterialsLabel(acces));
}

//=================================================================================================

bool XCAFDoc_DocumentTool::CheckMaterialTool(const TDF_Label& theAcces)
{
  TDF_Label aLabel = DocLabel(theAcces).FindChild(5, false);
  if (aLabel.IsNull())
  {
    return false;
  }
  return aLabel.IsAttribute(XCAFDoc_MaterialTool::GetID());
}

//=================================================================================================

occ::handle<XCAFDoc_ViewTool> XCAFDoc_DocumentTool::ViewTool(const TDF_Label& acces)
{
  return XCAFDoc_ViewTool::Set(ViewsLabel(acces));
}

//=================================================================================================

bool XCAFDoc_DocumentTool::CheckViewTool(const TDF_Label& theAcces)
{
  TDF_Label aLabel = DocLabel(theAcces).FindChild(7, false);
  if (aLabel.IsNull())
  {
    return false;
  }
  return aLabel.IsAttribute(XCAFDoc_ViewTool::GetID());
}

//=================================================================================================

occ::handle<XCAFDoc_ClippingPlaneTool> XCAFDoc_DocumentTool::ClippingPlaneTool(const TDF_Label& acces)
{
  return XCAFDoc_ClippingPlaneTool::Set(ClippingPlanesLabel(acces));
}

//=================================================================================================

bool XCAFDoc_DocumentTool::CheckClippingPlaneTool(const TDF_Label& theAcces)
{
  TDF_Label aLabel = DocLabel(theAcces).FindChild(8, false);
  if (aLabel.IsNull())
  {
    return false;
  }
  return aLabel.IsAttribute(XCAFDoc_ClippingPlaneTool::GetID());
}

//=================================================================================================

occ::handle<XCAFDoc_NotesTool> XCAFDoc_DocumentTool::NotesTool(const TDF_Label& acces)
{
  return XCAFDoc_NotesTool::Set(NotesLabel(acces));
}

//=================================================================================================

bool XCAFDoc_DocumentTool::CheckNotesTool(const TDF_Label& theAcces)
{
  TDF_Label aLabel = DocLabel(theAcces).FindChild(9, false);
  if (aLabel.IsNull())
  {
    return false;
  }
  return aLabel.IsAttribute(XCAFDoc_NotesTool::GetID());
}

//=================================================================================================

bool XCAFDoc_DocumentTool::GetLengthUnit(const occ::handle<TDocStd_Document>& theDoc,
                                                     double&                  theResult,
                                                     const UnitsMethods_LengthUnit   theBaseUnit)
{
  if (theDoc.IsNull())
  {
    return false;
  }
  occ::handle<XCAFDoc_LengthUnit> aLengthAttr;
  if (theDoc->Main().Root().FindAttribute(XCAFDoc_LengthUnit::GetID(), aLengthAttr))
  {
    theResult = aLengthAttr->GetUnitValue()
                * UnitsMethods::GetLengthUnitScale(UnitsMethods_LengthUnit_Meter, theBaseUnit);
    return true;
  }
  return false;
}

//=================================================================================================

bool XCAFDoc_DocumentTool::GetLengthUnit(const occ::handle<TDocStd_Document>& theDoc,
                                                     double&                  theResult)
{
  if (theDoc.IsNull())
  {
    return false;
  }
  occ::handle<XCAFDoc_LengthUnit> aLengthAttr;
  if (theDoc->Main().Root().FindAttribute(XCAFDoc_LengthUnit::GetID(), aLengthAttr))
  {
    theResult = aLengthAttr->GetUnitValue();
    return true;
  }
  return false;
}

//=================================================================================================

void XCAFDoc_DocumentTool::SetLengthUnit(const occ::handle<TDocStd_Document>& theDoc,
                                         const double             theUnitValue,
                                         const UnitsMethods_LengthUnit   theBaseUnit)
{
  // Sets length unit info
  TCollection_AsciiString aUnitName = UnitsMethods::DumpLengthUnit(theUnitValue, theBaseUnit);
  const double     aScaleFactor =
    theUnitValue * UnitsMethods::GetLengthUnitScale(theBaseUnit, UnitsMethods_LengthUnit_Meter);
  XCAFDoc_LengthUnit::Set(theDoc->Main().Root(), aUnitName, aScaleFactor);
}

//=================================================================================================

void XCAFDoc_DocumentTool::SetLengthUnit(const occ::handle<TDocStd_Document>& theDoc,
                                         const double             theUnitValue)
{
  // Sets length unit info
  TCollection_AsciiString aUnitName =
    UnitsMethods::DumpLengthUnit(theUnitValue, UnitsMethods_LengthUnit_Meter);
  XCAFDoc_LengthUnit::Set(theDoc->Main().Root(), aUnitName, theUnitValue);
}

//=================================================================================================

const Standard_GUID& XCAFDoc_DocumentTool::ID() const
{
  return GetID();
}

//=================================================================================================

bool XCAFDoc_DocumentTool::AfterRetrieval(const bool /* forceIt */)
{
  Init();
  return true;
}

//=================================================================================================

void XCAFDoc_DocumentTool::Init() const
{
  TDF_Label                 DocL = Label(), RootL = DocL.Root();
  const Standard_GUID&      aRefGuid = GetDocumentToolRefID();
  occ::handle<TDataStd_TreeNode> aRootNode, aLabNode;

  if (!RootL.FindAttribute(aRefGuid, aRootNode))
  {
    occ::handle<TDataStd_TreeNode> aRootNodeNew = TDataStd_TreeNode::Set(RootL, aRefGuid);
    occ::handle<TDataStd_TreeNode> aLNode       = TDataStd_TreeNode::Set(DocL, aRefGuid);
    aLNode->SetFather(aRootNodeNew);
    aRootNodeNew->SetFirst(aLNode);
  }
}

//=================================================================================================

bool XCAFDoc_DocumentTool::IsXCAFDocument(const occ::handle<TDocStd_Document>& D)
{
  TDF_Label                 RootL    = D->Main().Root();
  const Standard_GUID&      aRefGuid = GetDocumentToolRefID();
  occ::handle<TDataStd_TreeNode> aRootNode;
  return RootL.FindAttribute(aRefGuid, aRootNode);
}
