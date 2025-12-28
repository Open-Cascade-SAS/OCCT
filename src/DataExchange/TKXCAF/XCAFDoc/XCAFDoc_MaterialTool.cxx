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

#include <XCAFDoc_MaterialTool.hxx>

#include <Standard_Type.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_Material.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(XCAFDoc_MaterialTool,
                                      TDataStd_GenericEmpty,
                                      "xcaf",
                                      "MaterialTool")

//=================================================================================================

XCAFDoc_MaterialTool::XCAFDoc_MaterialTool() = default;

//=================================================================================================

occ::handle<XCAFDoc_MaterialTool> XCAFDoc_MaterialTool::Set(const TDF_Label& L)
{
  occ::handle<XCAFDoc_MaterialTool> A;
  if (!L.FindAttribute(XCAFDoc_MaterialTool::GetID(), A))
  {
    A = new XCAFDoc_MaterialTool();
    L.AddAttribute(A);
    A->myShapeTool = XCAFDoc_DocumentTool::ShapeTool(L);
  }
  return A;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_MaterialTool::GetID()
{
  static Standard_GUID MatTblID("efd212f9-6dfd-11d4-b9c8-0060b0ee281b");
  return MatTblID;
}

//=================================================================================================

TDF_Label XCAFDoc_MaterialTool::BaseLabel() const
{
  return Label();
}

//=================================================================================================

const occ::handle<XCAFDoc_ShapeTool>& XCAFDoc_MaterialTool::ShapeTool()
{
  if (myShapeTool.IsNull())
    myShapeTool = XCAFDoc_DocumentTool::ShapeTool(Label());
  return myShapeTool;
}

//=================================================================================================

bool XCAFDoc_MaterialTool::IsMaterial(const TDF_Label& lab) const
{
  occ::handle<XCAFDoc_Material> MatAttr;
  if (lab.FindAttribute(XCAFDoc_Material::GetID(), MatAttr))
  {
    return true;
  }
  return false;
}

//=================================================================================================

void XCAFDoc_MaterialTool::GetMaterialLabels(NCollection_Sequence<TDF_Label>& Labels) const
{
  Labels.Clear();
  TDF_ChildIterator ChildIterator(Label());
  for (; ChildIterator.More(); ChildIterator.Next())
  {
    TDF_Label L = ChildIterator.Value();
    if (IsMaterial(L))
      Labels.Append(L);
  }
}

//=================================================================================================

TDF_Label XCAFDoc_MaterialTool::AddMaterial(
  const occ::handle<TCollection_HAsciiString>& aName,
  const occ::handle<TCollection_HAsciiString>& aDescription,
  const double                                 aDensity,
  const occ::handle<TCollection_HAsciiString>& aDensName,
  const occ::handle<TCollection_HAsciiString>& aDensValType) const
{
  TDF_Label     MatL;
  TDF_TagSource aTag;
  MatL = aTag.NewChild(Label());
  XCAFDoc_Material::Set(MatL, aName, aDescription, aDensity, aDensName, aDensValType);
  TDataStd_Name::Set(MatL, TCollection_AsciiString(aName->ToCString()));
  return MatL;
}

//=================================================================================================

void XCAFDoc_MaterialTool::SetMaterial(const TDF_Label& L, const TDF_Label& MatL) const
{
  // set reference
  occ::handle<TDataStd_TreeNode> refNode, mainNode;
  mainNode = TDataStd_TreeNode::Set(MatL, XCAFDoc::MaterialRefGUID());
  refNode  = TDataStd_TreeNode::Set(L, XCAFDoc::MaterialRefGUID());
  refNode->Remove(); // abv: fix against bug in TreeNode::Append()
  mainNode->Append(refNode);
}

//=================================================================================================

void XCAFDoc_MaterialTool::SetMaterial(
  const TDF_Label&                             L,
  const occ::handle<TCollection_HAsciiString>& aName,
  const occ::handle<TCollection_HAsciiString>& aDescription,
  const double                                 aDensity,
  const occ::handle<TCollection_HAsciiString>& aDensName,
  const occ::handle<TCollection_HAsciiString>& aDensValType) const
{
  TDF_Label MatL = AddMaterial(aName, aDescription, aDensity, aDensName, aDensValType);
  SetMaterial(L, MatL);
}

//=================================================================================================

bool XCAFDoc_MaterialTool::GetMaterial(const TDF_Label&                       MatL,
                                       occ::handle<TCollection_HAsciiString>& aName,
                                       occ::handle<TCollection_HAsciiString>& aDescription,
                                       double&                                aDensity,
                                       occ::handle<TCollection_HAsciiString>& aDensName,
                                       occ::handle<TCollection_HAsciiString>& aDensValType)
{
  occ::handle<XCAFDoc_Material> MatAttr;
  if (!MatL.FindAttribute(XCAFDoc_Material::GetID(), MatAttr))
  {
    return false;
  }
  aName        = MatAttr->GetName();
  aDescription = MatAttr->GetDescription();
  aDensity     = MatAttr->GetDensity();
  aDensName    = MatAttr->GetDensName();
  aDensValType = MatAttr->GetDensValType();

  return true;
}

//=================================================================================================

double XCAFDoc_MaterialTool::GetDensityForShape(const TDF_Label& ShapeL)
{
  double                         Dens = 0.0;
  occ::handle<TDataStd_TreeNode> Node;
  if (!ShapeL.FindAttribute(XCAFDoc::MaterialRefGUID(), Node) || !Node->HasFather())
    return Dens;
  TDF_Label                     MatL = Node->Father()->Label();
  occ::handle<XCAFDoc_Material> MatAttr;
  if (!MatL.FindAttribute(XCAFDoc_Material::GetID(), MatAttr))
  {
    return Dens;
  }
  // default dimension fo density - gram/sm^3
  // we transfer "sm" into "mm"
  Dens = MatAttr->GetDensity() * 0.001;
  return Dens;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_MaterialTool::ID() const
{
  return GetID();
}

//=================================================================================================

void XCAFDoc_MaterialTool::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  NCollection_Sequence<TDF_Label> aLabels;
  GetMaterialLabels(aLabels);
  for (NCollection_Sequence<TDF_Label>::Iterator aMaterialLabelIt(aLabels); aMaterialLabelIt.More();
       aMaterialLabelIt.Next())
  {
    TCollection_AsciiString aMaterialLabel;
    TDF_Tool::Entry(aMaterialLabelIt.Value(), aMaterialLabel);
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aMaterialLabel)
  }
}
