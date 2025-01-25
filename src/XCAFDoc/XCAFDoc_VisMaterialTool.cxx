// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <XCAFDoc_VisMaterialTool.hxx>

#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Label.hxx>
#include <TNaming_NamedShape.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_VisMaterial.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_VisMaterialTool, TDF_Attribute)

//=================================================================================================

const Standard_GUID& XCAFDoc_VisMaterialTool::GetID()
{
  static Standard_GUID THE_VIS_MAT_TOOL_ID("87B511CE-DA15-4A5E-98AF-E3F46AB5B6E8");
  return THE_VIS_MAT_TOOL_ID;
}

//=================================================================================================

Handle(XCAFDoc_VisMaterialTool) XCAFDoc_VisMaterialTool::Set(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_VisMaterialTool) aTool;
  if (!theLabel.FindAttribute(XCAFDoc_VisMaterialTool::GetID(), aTool))
  {
    aTool = new XCAFDoc_VisMaterialTool();
    theLabel.AddAttribute(aTool);
    aTool->myShapeTool = XCAFDoc_DocumentTool::ShapeTool(theLabel);
  }
  return aTool;
}

//=================================================================================================

XCAFDoc_VisMaterialTool::XCAFDoc_VisMaterialTool()
{
  //
}

//=================================================================================================

const Handle(XCAFDoc_ShapeTool)& XCAFDoc_VisMaterialTool::ShapeTool()
{
  if (myShapeTool.IsNull())
  {
    myShapeTool = XCAFDoc_DocumentTool::ShapeTool(Label());
  }
  return myShapeTool;
}

//=================================================================================================

Handle(XCAFDoc_VisMaterial) XCAFDoc_VisMaterialTool::GetMaterial(const TDF_Label& theMatLabel)
{
  Handle(XCAFDoc_VisMaterial) aMatAttrib;
  theMatLabel.FindAttribute(XCAFDoc_VisMaterial::GetID(), aMatAttrib);
  return aMatAttrib;
}

//=================================================================================================

TDF_Label XCAFDoc_VisMaterialTool::AddMaterial(const Handle(XCAFDoc_VisMaterial)& theMat,
                                               const TCollection_AsciiString&     theName) const
{
  TDF_TagSource aTag;
  TDF_Label     aLab = aTag.NewChild(Label());
  aLab.AddAttribute(theMat);
  if (!theName.IsEmpty())
  {
    TDataStd_Name::Set(aLab, theName);
  }
  return aLab;
}

//=================================================================================================

TDF_Label XCAFDoc_VisMaterialTool::AddMaterial(const TCollection_AsciiString& theName) const
{
  Handle(XCAFDoc_VisMaterial) aNewMat = new XCAFDoc_VisMaterial();
  TDF_TagSource               aTag;
  TDF_Label                   aLab = aTag.NewChild(Label());
  aLab.AddAttribute(aNewMat);
  if (!theName.IsEmpty())
  {
    TDataStd_Name::Set(aLab, theName);
  }
  return aLab;
}

//=================================================================================================

void XCAFDoc_VisMaterialTool::RemoveMaterial(const TDF_Label& theLabel) const
{
  theLabel.ForgetAllAttributes(true);
}

//=================================================================================================

void XCAFDoc_VisMaterialTool::GetMaterials(TDF_LabelSequence& theLabels) const
{
  theLabels.Clear();
  for (TDF_ChildIDIterator aChildIDIterator(Label(), XCAFDoc_VisMaterial::GetID());
       aChildIDIterator.More();
       aChildIDIterator.Next())
  {
    const TDF_Label aLabel = aChildIDIterator.Value()->Label();
    if (IsMaterial(aLabel))
    {
      theLabels.Append(aLabel);
    }
  }
}

//=================================================================================================

void XCAFDoc_VisMaterialTool::SetShapeMaterial(const TDF_Label& theShapeLabel,
                                               const TDF_Label& theMaterialLabel) const
{
  if (theMaterialLabel.IsNull())
  {
    theShapeLabel.ForgetAttribute(XCAFDoc::VisMaterialRefGUID());
    return;
  }

  // set reference
  Handle(TDataStd_TreeNode) aMainNode =
    TDataStd_TreeNode::Set(theMaterialLabel, XCAFDoc::VisMaterialRefGUID());
  Handle(TDataStd_TreeNode) aRefNode =
    TDataStd_TreeNode::Set(theShapeLabel, XCAFDoc::VisMaterialRefGUID());
  aRefNode->Remove(); // abv: fix against bug in TreeNode::Append()
  aMainNode->Prepend(aRefNode);
}

//=================================================================================================

void XCAFDoc_VisMaterialTool::UnSetShapeMaterial(const TDF_Label& theShapeLabel) const
{
  theShapeLabel.ForgetAttribute(XCAFDoc::VisMaterialRefGUID());
}

//=================================================================================================

Standard_Boolean XCAFDoc_VisMaterialTool::IsSetShapeMaterial(const TDF_Label& theLabel) const
{
  Handle(TDataStd_TreeNode) aNode;
  return theLabel.FindAttribute(XCAFDoc::VisMaterialRefGUID(), aNode) && aNode->HasFather();
}

//=================================================================================================

Standard_Boolean XCAFDoc_VisMaterialTool::GetShapeMaterial(const TDF_Label& theShapeLabel,
                                                           TDF_Label&       theMaterialLabel)
{
  Handle(TDataStd_TreeNode) aNode;
  if (!theShapeLabel.FindAttribute(XCAFDoc::VisMaterialRefGUID(), aNode) || !aNode->HasFather())
  {
    return Standard_False;
  }

  theMaterialLabel = aNode->Father()->Label();
  return Standard_True;
}

//=================================================================================================

Handle(XCAFDoc_VisMaterial) XCAFDoc_VisMaterialTool::GetShapeMaterial(
  const TDF_Label& theShapeLabel)
{
  TDF_Label aMatLabel;
  return GetShapeMaterial(theShapeLabel, aMatLabel) ? GetMaterial(aMatLabel)
                                                    : Handle(XCAFDoc_VisMaterial)();
}

//=================================================================================================

Standard_Boolean XCAFDoc_VisMaterialTool::SetShapeMaterial(const TopoDS_Shape& theShape,
                                                           const TDF_Label&    theMaterialLabel)
{
  TDF_Label aShapeLabel;
  if (!ShapeTool()->Search(theShape, aShapeLabel))
  {
    return Standard_False;
  }

  SetShapeMaterial(aShapeLabel, theMaterialLabel);
  return Standard_True;
}

//=================================================================================================

Standard_Boolean XCAFDoc_VisMaterialTool::UnSetShapeMaterial(const TopoDS_Shape& theShape)
{
  TDF_Label aShapeLabel;
  if (!ShapeTool()->Search(theShape, aShapeLabel))
  {
    return Standard_False;
  }

  UnSetShapeMaterial(aShapeLabel);
  return Standard_True;
}

//=================================================================================================

Standard_Boolean XCAFDoc_VisMaterialTool::IsSetShapeMaterial(const TopoDS_Shape& theShape)
{
  TDF_Label aShapeLabel;
  return ShapeTool()->Search(theShape, aShapeLabel) && IsSetShapeMaterial(aShapeLabel);
}

//=================================================================================================

Standard_Boolean XCAFDoc_VisMaterialTool::GetShapeMaterial(const TopoDS_Shape& theShape,
                                                           TDF_Label&          theMaterialLabel)
{
  TDF_Label aShapeLabel;
  return ShapeTool()->Search(theShape, aShapeLabel)
         && GetShapeMaterial(aShapeLabel, theMaterialLabel);
}

//=================================================================================================

Handle(XCAFDoc_VisMaterial) XCAFDoc_VisMaterialTool::GetShapeMaterial(const TopoDS_Shape& theShape)
{
  TDF_Label aMatLabel;
  return GetShapeMaterial(theShape, aMatLabel) ? GetMaterial(aMatLabel)
                                               : Handle(XCAFDoc_VisMaterial)();
}
