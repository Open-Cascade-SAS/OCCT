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

#include <XCAFDoc_ClippingPlaneTool.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <XCAFDoc.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(XCAFDoc_ClippingPlaneTool,
                                      TDataStd_GenericEmpty,
                                      "xcaf",
                                      "ClippingPlaneTool")

//=================================================================================================

TDF_Label XCAFDoc_ClippingPlaneTool::BaseLabel() const
{
  return Label();
}

//=================================================================================================

Standard_Boolean XCAFDoc_ClippingPlaneTool::IsClippingPlane(const TDF_Label& theLabel) const
{
  if (theLabel.Father() != Label())
    return Standard_False;

  Handle(TDataXtd_Plane) aPlaneAttribute;
  if (!theLabel.FindAttribute(TDataXtd_Plane::GetID(), aPlaneAttribute))
    return Standard_False;
  return Standard_True;
}

//=================================================================================================

Standard_Boolean XCAFDoc_ClippingPlaneTool::GetClippingPlane(const TDF_Label&            theLabel,
                                                             gp_Pln&                     thePlane,
                                                             TCollection_ExtendedString& theName,
                                                             Standard_Boolean& theCapping) const
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

//=================================================================================================

Standard_Boolean XCAFDoc_ClippingPlaneTool::GetClippingPlane(
  const TDF_Label&                  theLabel,
  gp_Pln&                           thePlane,
  Handle(TCollection_HAsciiString)& theName,
  Standard_Boolean&                 theCapping) const
{
  TCollection_ExtendedString anExtName;
  if (!GetClippingPlane(theLabel, thePlane, anExtName, theCapping))
    return Standard_False;
  theName = new TCollection_HAsciiString(anExtName);
  return Standard_True;
}

//=================================================================================================

TDF_Label XCAFDoc_ClippingPlaneTool::AddClippingPlane(
  const gp_Pln&                     thePlane,
  const TCollection_ExtendedString& theName) const
{
  TDF_Label         aLabel;
  TDF_LabelSequence aClippingPlanes;
  GetClippingPlanes(aClippingPlanes);
  for (Standard_Integer i = 1; i <= aClippingPlanes.Length(); i++)
  {
    gp_Pln                     aPlane;
    TCollection_ExtendedString aName;
    Standard_Boolean           aCapping;
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

//=================================================================================================

TDF_Label XCAFDoc_ClippingPlaneTool::AddClippingPlane(
  const gp_Pln&                           thePlane,
  const Handle(TCollection_HAsciiString)& theName) const
{
  TCollection_ExtendedString anExtName = TCollection_ExtendedString(theName->String());
  return AddClippingPlane(thePlane, anExtName);
}

//=================================================================================================

TDF_Label XCAFDoc_ClippingPlaneTool::AddClippingPlane(const gp_Pln&                     thePlane,
                                                      const TCollection_ExtendedString& theName,
                                                      const Standard_Boolean theCapping) const
{
  TDF_Label        aLabel      = AddClippingPlane(thePlane, theName);
  Standard_Integer aCappingVal = (theCapping) ? 1 : 0;
  TDataStd_Integer::Set(aLabel, aCappingVal);

  return aLabel;
}

//=================================================================================================

TDF_Label XCAFDoc_ClippingPlaneTool::AddClippingPlane(
  const gp_Pln&                           thePlane,
  const Handle(TCollection_HAsciiString)& theName,
  const Standard_Boolean                  theCapping) const
{
  TCollection_ExtendedString anExtName = TCollection_ExtendedString(theName->String());
  return AddClippingPlane(thePlane, anExtName, theCapping);
}

//=================================================================================================

Standard_Boolean XCAFDoc_ClippingPlaneTool::RemoveClippingPlane(const TDF_Label& theLabel) const
{
  Handle(TDataStd_TreeNode) Node;
  if (!IsClippingPlane(theLabel) || theLabel.FindAttribute(XCAFDoc::ViewRefPlaneGUID(), Node))
    return Standard_False;

  theLabel.ForgetAllAttributes(Standard_True);
  return Standard_True;
}

//=================================================================================================

void XCAFDoc_ClippingPlaneTool::GetClippingPlanes(TDF_LabelSequence& theLabels) const
{
  theLabels.Clear();

  TDF_ChildIDIterator ChildIDIterator(Label(), TDataXtd_Plane::GetID());
  for (; ChildIDIterator.More(); ChildIDIterator.Next())
  {
    TDF_Label aLabel = ChildIDIterator.Value()->Label();
    if (IsClippingPlane(aLabel))
      theLabels.Append(aLabel);
  }
}

//=================================================================================================

void XCAFDoc_ClippingPlaneTool::UpdateClippingPlane(const TDF_Label&                  theLabel,
                                                    const gp_Pln&                     thePlane,
                                                    const TCollection_ExtendedString& theName) const
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

//=================================================================================================

void XCAFDoc_ClippingPlaneTool::SetCapping(const TDF_Label&       theClippingPlaneL,
                                           const Standard_Boolean theCapping)
{
  if (theClippingPlaneL.Father() != Label())
    return;

  theClippingPlaneL.ForgetAttribute(TDataStd_Integer::GetID());
  Standard_Integer aCappingVal = (theCapping) ? 1 : 0;
  TDataStd_Integer::Set(theClippingPlaneL, aCappingVal);
}

//=================================================================================================

Standard_Boolean XCAFDoc_ClippingPlaneTool::GetCapping(const TDF_Label& theClippingPlaneL) const
{
  if (theClippingPlaneL.Father() != Label())
    return Standard_False;

  Handle(TDataStd_Integer) aCappingAttribute;
  if (theClippingPlaneL.FindAttribute(TDataStd_Integer::GetID(), aCappingAttribute))
    return (aCappingAttribute->Get() == 1);

  return Standard_False;
}

//=================================================================================================

Standard_Boolean XCAFDoc_ClippingPlaneTool::GetCapping(const TDF_Label&  theClippingPlaneL,
                                                       Standard_Boolean& theCapping) const
{
  if (theClippingPlaneL.Father() != Label())
    return Standard_False;

  Handle(TDataStd_Integer) aCappingAttribute;
  if (theClippingPlaneL.FindAttribute(TDataStd_Integer::GetID(), aCappingAttribute))
  {
    theCapping = (aCappingAttribute->Get() == 1);
    return Standard_True;
  }

  return Standard_False;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_ClippingPlaneTool::GetID()
{
  static Standard_GUID ColorTblID("efd213ea-6dfd-11d4-b9c8-0060b0ee281b");
  return ColorTblID;
}

//=================================================================================================

Handle(XCAFDoc_ClippingPlaneTool) XCAFDoc_ClippingPlaneTool::Set(const TDF_Label& L)
{
  Handle(XCAFDoc_ClippingPlaneTool) A;
  if (!L.FindAttribute(XCAFDoc_ClippingPlaneTool::GetID(), A))
  {
    A = new XCAFDoc_ClippingPlaneTool();
    L.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_ClippingPlaneTool::ID() const
{
  return GetID();
}

//=================================================================================================

XCAFDoc_ClippingPlaneTool::XCAFDoc_ClippingPlaneTool() {}
