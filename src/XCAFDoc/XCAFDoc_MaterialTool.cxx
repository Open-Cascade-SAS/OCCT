// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <XCAFDoc_MaterialTool.ixx>

#include <XCAFDoc.hxx>
#include <XCAFDoc_Material.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_ChildIDIterator.hxx>


//=======================================================================
//function : XCAFDoc_MaterialTool
//purpose  : 
//=======================================================================

XCAFDoc_MaterialTool::XCAFDoc_MaterialTool()
{
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_MaterialTool) XCAFDoc_MaterialTool::Set(const TDF_Label& L) 
{
  Handle(XCAFDoc_MaterialTool) A;
  if (!L.FindAttribute (XCAFDoc_MaterialTool::GetID(), A)) {
    A = new XCAFDoc_MaterialTool ();
    L.AddAttribute(A);
    A->myShapeTool = XCAFDoc_DocumentTool::ShapeTool(L);
  }
  return A;
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_MaterialTool::GetID() 
{
  static Standard_GUID MatTblID ("efd212f9-6dfd-11d4-b9c8-0060b0ee281b");
  return MatTblID; 
}


//=======================================================================
//function : BaseLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_MaterialTool::BaseLabel() const
{
  return Label();
}


//=======================================================================
//function : ShapeTool
//purpose  : 
//=======================================================================

const Handle(XCAFDoc_ShapeTool)& XCAFDoc_MaterialTool::ShapeTool() 
{
  if(myShapeTool.IsNull())
    myShapeTool = XCAFDoc_DocumentTool::ShapeTool(Label());
  return myShapeTool;
}


//=======================================================================
//function : IsMaterial
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_MaterialTool::IsMaterial(const TDF_Label& lab) const
{
  Handle(XCAFDoc_Material) MatAttr;
  if(lab.FindAttribute(XCAFDoc_Material::GetID(),MatAttr)) {
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : GetMaterialLabels
//purpose  : 
//=======================================================================

void XCAFDoc_MaterialTool::GetMaterialLabels(TDF_LabelSequence& Labels) const
{
  Labels.Clear();
  TDF_ChildIterator ChildIterator( Label() ); 
  for (; ChildIterator.More(); ChildIterator.Next()) {
    TDF_Label L = ChildIterator.Value();
    if ( IsMaterial(L)) Labels.Append(L);
  }
}


//=======================================================================
//function : AddMaterial
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_MaterialTool::AddMaterial(const Handle(TCollection_HAsciiString)& aName,
                                            const Handle(TCollection_HAsciiString)& aDescription,
                                            const Standard_Real aDensity,
                                            const Handle(TCollection_HAsciiString)& aDensName,
                                            const Handle(TCollection_HAsciiString)& aDensValType) const
{
  TDF_Label MatL;
  TDF_TagSource aTag;
  MatL = aTag.NewChild ( Label() );
  XCAFDoc_Material::Set(MatL,aName,aDescription,aDensity,aDensName,aDensValType);
  TDataStd_Name::Set(MatL,TCollection_AsciiString(aName->ToCString()));
  return MatL;
}


//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void XCAFDoc_MaterialTool::SetMaterial(const TDF_Label& L,const TDF_Label& MatL) const
{
  // set reference
  Handle(TDataStd_TreeNode) refNode, mainNode;
  mainNode = TDataStd_TreeNode::Set ( MatL, XCAFDoc::MaterialRefGUID() );
  refNode  = TDataStd_TreeNode::Set ( L,       XCAFDoc::MaterialRefGUID() );
  refNode->Remove(); // abv: fix against bug in TreeNode::Append()
  mainNode->Append(refNode);
}


//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void XCAFDoc_MaterialTool::SetMaterial(const TDF_Label& L,
                                       const Handle(TCollection_HAsciiString)& aName,
                                       const Handle(TCollection_HAsciiString)& aDescription,
                                       const Standard_Real aDensity,
                                       const Handle(TCollection_HAsciiString)& aDensName,
                                       const Handle(TCollection_HAsciiString)& aDensValType) const
{
  TDF_Label MatL = AddMaterial(aName,aDescription,aDensity,aDensName,aDensValType);
  SetMaterial(L,MatL);
}


//=======================================================================
//function : GetMaterial
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_MaterialTool::GetMaterial(const TDF_Label& MatL,
                                                   Handle(TCollection_HAsciiString)& aName,
                                                   Handle(TCollection_HAsciiString)& aDescription,
                                                   Standard_Real& aDensity,
                                                   Handle(TCollection_HAsciiString)& aDensName,
                                                   Handle(TCollection_HAsciiString)& aDensValType) const
{
  Handle(XCAFDoc_Material) MatAttr;
  if(!MatL.FindAttribute(XCAFDoc_Material::GetID(),MatAttr)) {
    return Standard_False;
  }
  aName = MatAttr->GetName();
  aDescription = MatAttr->GetDescription();
  aDensity = MatAttr->GetDensity();
  aDensName = MatAttr->GetDensName();
  aDensValType = MatAttr->GetDensValType();
  
  return Standard_True;
}


//=======================================================================
//function : GetDensityForShape
//purpose  : 
//=======================================================================

Standard_Real XCAFDoc_MaterialTool::GetDensityForShape(const TDF_Label& ShapeL)
{
  Standard_Real Dens=0.0;
  Handle(TDataStd_TreeNode) Node;
  if ( ! ShapeL.FindAttribute ( XCAFDoc::MaterialRefGUID(), Node) ||
       ! Node->HasFather() ) return Dens;
  TDF_Label MatL = Node->Father()->Label();
  Handle(XCAFDoc_Material) MatAttr;
  if(!MatL.FindAttribute(XCAFDoc_Material::GetID(),MatAttr)) {
    return Dens;
  }
  // defaul dimension fo density - gram/sm^3
  // we transfer "sm" into "mm"
  Dens = MatAttr->GetDensity()*0.001; 
  return Dens;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_MaterialTool::ID() const
{
  return GetID();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_MaterialTool::Restore(const Handle(TDF_Attribute)& /*with*/) 
{
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_MaterialTool::NewEmpty() const
{
  return new XCAFDoc_MaterialTool;
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_MaterialTool::Paste(const Handle(TDF_Attribute)& /*into*/,
                                 const Handle(TDF_RelocationTable)& /*RT*/) const
{
}

