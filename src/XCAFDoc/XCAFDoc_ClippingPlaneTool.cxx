// Created on: 2016-11-29
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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


#include <TCollection_HAsciiString.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_ClippingPlaneTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_ClippingPlaneTool, TDF_Attribute)

//=======================================================================
//function : BaseLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_ClippingPlaneTool::BaseLabel() const
{
  return Label();
}

//=======================================================================
//function : IsClippingPlane
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_ClippingPlaneTool::IsClippingPlane(const TDF_Label& theLabel) const
{
  if (theLabel.Father() != Label())
    return Standard_False;

  Handle(TDataXtd_Plane) aPlaneAttribute;
  if (!theLabel.FindAttribute(TDataXtd_Plane::GetID(), aPlaneAttribute))
    return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : GetClippingPlane
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_ClippingPlaneTool::GetClippingPlane(const TDF_Label& theLabel,
  gp_Pln& thePlane, TCollection_ExtendedString& theName, Standard_Boolean &theCapping) const
{
  if (theLabel.Father() != Label())
    return Standard_False;

  Handle(TDataXtd_Plane) aPlaneAttribute;
  if (!theLabel.FindAttribute(TDataXtd_Plane::GetID(), aPlaneAttribute))
    return Standard_False;

  TDataXtd_Geometry::Plane(aPlaneAttribute->Label(), thePlane);
  Handle(TDataStd_Name) aNameAttribute;
  if (theLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttribute))
    theName = aNameAttribute->Get();

  Handle(TDataStd_Integer) aCappingAttribute;
  if (theLabel.FindAttribute(TDataStd_Integer::GetID(), aCappingAttribute))
    theCapping = (aCappingAttribute->Get() == 1);
  
  return Standard_True;
}

//=======================================================================
//function : GetClippingPlane
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_ClippingPlaneTool::GetClippingPlane(const TDF_Label& theLabel,
  gp_Pln& thePlane, Handle(TCollection_HAsciiString)& theName, Standard_Boolean &theCapping) const
{
  TCollection_ExtendedString anExtName;
  if (!GetClippingPlane(theLabel, thePlane, anExtName, theCapping))
    return Standard_False;
  theName = new TCollection_HAsciiString(anExtName);
  return Standard_True;
}

//=======================================================================
//function : AddClippingPlane
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_ClippingPlaneTool::AddClippingPlane(const gp_Pln thePlane, const TCollection_ExtendedString theName) const
{
  TDF_Label aLabel;
  TDF_LabelSequence aClippingPlanes;
  GetClippingPlanes(aClippingPlanes);
  for (Standard_Integer i = 1; i <= aClippingPlanes.Length(); i++) {
    gp_Pln aPlane;
    TCollection_ExtendedString aName;
    Standard_Boolean aCapping;
    GetClippingPlane(aClippingPlanes.Value(i), aPlane, aName, aCapping);
    if (!aName.IsEqual(theName))
      continue;
    if (aPlane.Axis().Angle(thePlane.Axis()) > Precision::Angular())
      continue;
    if (aPlane.XAxis().Angle(thePlane.XAxis()) > Precision::Angular())
      continue;
    if (aPlane.YAxis().Angle(thePlane.YAxis()) > Precision::Angular())
      continue;
    return aClippingPlanes.Value(i);
  }

  // create a new clipping plane entry  
  TDF_TagSource aTag;
  aLabel = aTag.NewChild(Label());

  TDataXtd_Plane::Set(aLabel, thePlane);
  if (!theName.IsEmpty())
    TDataStd_Name::Set(aLabel, theName);

  return aLabel;
}

//=======================================================================
//function : AddClippingPlane
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_ClippingPlaneTool::AddClippingPlane(const gp_Pln thePlane, const Handle(TCollection_HAsciiString)& theName) const
{
  TCollection_ExtendedString anExtName = TCollection_ExtendedString(theName->String());
  return AddClippingPlane(thePlane, anExtName);
}


//=======================================================================
//function : AddClippingPlane
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_ClippingPlaneTool::AddClippingPlane(const gp_Pln thePlane, const TCollection_ExtendedString theName, const Standard_Boolean theCapping) const
{
  TDF_Label aLabel = AddClippingPlane(thePlane, theName);
  Standard_Integer aCappingVal = (theCapping) ? 1 : 0;
  TDataStd_Integer::Set(aLabel, aCappingVal);
  
  return aLabel;
}

//=======================================================================
//function : AddClippingPlane
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_ClippingPlaneTool::AddClippingPlane(const gp_Pln thePlane, const Handle(TCollection_HAsciiString)& theName, const Standard_Boolean theCapping) const
{
  TCollection_ExtendedString anExtName = TCollection_ExtendedString(theName->String());
  return AddClippingPlane(thePlane, anExtName, theCapping);
}

//=======================================================================
//function : RemoveClippingPlane
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_ClippingPlaneTool::RemoveClippingPlane(const TDF_Label& theLabel) const
{
  Handle(TDataStd_TreeNode) Node;
  if (!IsClippingPlane(theLabel) || theLabel.FindAttribute(XCAFDoc::ViewRefPlaneGUID(), Node))
    return Standard_False;

  theLabel.ForgetAllAttributes(Standard_True);
  return Standard_True;
}

//=======================================================================
//function : GetClippingPlanes
//purpose  : 
//=======================================================================

void XCAFDoc_ClippingPlaneTool::GetClippingPlanes(TDF_LabelSequence& theLabels) const
{
  theLabels.Clear();

  TDF_ChildIDIterator ChildIDIterator(Label(), TDataXtd_Plane::GetID());
  for (; ChildIDIterator.More(); ChildIDIterator.Next()) {
    TDF_Label aLabel = ChildIDIterator.Value()->Label();
    if (IsClippingPlane(aLabel)) theLabels.Append(aLabel);
  }
}

//=======================================================================
//function : UpdateClippingPlane
//purpose  : 
//=======================================================================

void XCAFDoc_ClippingPlaneTool::UpdateClippingPlane(const TDF_Label& theLabel,
  const gp_Pln thePlane, const TCollection_ExtendedString theName) const
{
  if (theLabel.Father() != Label())
    return;

  Handle(TDataXtd_Plane) aPlaneAttribute;
  if (!theLabel.FindAttribute(TDataXtd_Plane::GetID(), aPlaneAttribute))
    return;
  theLabel.ForgetAttribute(TDataXtd_Plane::GetID());
  TDataXtd_Plane::Set(theLabel, thePlane);
  theLabel.ForgetAttribute(TDataStd_Name::GetID());
  TDataStd_Name::Set(theLabel, theName);
}

//=======================================================================
//function : SetCapping
//purpose  : 
//=======================================================================

void XCAFDoc_ClippingPlaneTool::SetCapping(const TDF_Label& theClippingPlaneL, const Standard_Boolean theCapping)
{
  if (theClippingPlaneL.Father() != Label())
    return;

  theClippingPlaneL.ForgetAttribute(TDataStd_Integer::GetID());
  Standard_Integer aCappingVal = (theCapping) ? 1 : 0;
  TDataStd_Integer::Set(theClippingPlaneL, aCappingVal);
}

//=======================================================================
//function : GetCapping
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_ClippingPlaneTool::GetCapping(const TDF_Label& theClippingPlaneL) const
{
  if (theClippingPlaneL.Father() != Label())
    return Standard_False;

  Handle(TDataStd_Integer) aCappingAttribute;
  if (theClippingPlaneL.FindAttribute(TDataStd_Integer::GetID(), aCappingAttribute))
    return (aCappingAttribute->Get() == 1);

  return Standard_False;
}

//=======================================================================
//function : GetCapping
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_ClippingPlaneTool::GetCapping(const TDF_Label& theClippingPlaneL, Standard_Boolean &theCapping) const
{
  if (theClippingPlaneL.Father() != Label())
    return Standard_False;

  Handle(TDataStd_Integer) aCappingAttribute;
  if (theClippingPlaneL.FindAttribute(TDataStd_Integer::GetID(), aCappingAttribute)) {
    theCapping = (aCappingAttribute->Get() == 1);
    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_ClippingPlaneTool::GetID() 
{
  static Standard_GUID ColorTblID ("efd213ea-6dfd-11d4-b9c8-0060b0ee281b");
  return ColorTblID; 
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_ClippingPlaneTool) XCAFDoc_ClippingPlaneTool::Set(const TDF_Label& L)
{
  Handle(XCAFDoc_ClippingPlaneTool) A;
  if (!L.FindAttribute(XCAFDoc_ClippingPlaneTool::GetID(), A)) {
    A = new XCAFDoc_ClippingPlaneTool();
    L.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_ClippingPlaneTool::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_ClippingPlaneTool::Restore(const Handle(TDF_Attribute)& /*with*/)
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_ClippingPlaneTool::NewEmpty() const
{
  return new XCAFDoc_ClippingPlaneTool;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_ClippingPlaneTool::Paste(const Handle(TDF_Attribute)& /*into*/,
  const Handle(TDF_RelocationTable)& /*RT*/) const
{
}

//=======================================================================
//function : XCAFDoc_ClippingPlaneTool
//purpose  : 
//=======================================================================

XCAFDoc_ClippingPlaneTool::XCAFDoc_ClippingPlaneTool()
{
}

