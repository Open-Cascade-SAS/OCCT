// Created on: 2020-03-17
// Created by: Irina KRYLOVA
// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <XCAFKinObjects_KinematicPair.hxx>
#include <BRep_Builder.hxx>
#include <gp_Pln.hxx>
#include <Standard_Dump.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Standard_GUID.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDF_ChildIterator.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <XCAFDoc.hxx>
#include <XCAFKinObjects_HighOrderPairObject.hxx>
#include <XCAFKinObjects_LowOrderPairObject.hxx>
#include <XCAFKinObjects_LowOrderPairObjectWithCoupling.hxx>
#include <XCAFKinObjects_PairObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFKinObjects_KinematicPair, TDF_Attribute)

enum ChildLab
{
  ChildLab_FirstTrsf = 1,
  ChildLab_SecondTrsf,
  ChildLab_FirstGeomParam,
  ChildLab_SecondGeomParam,
};

//=================================================================================================

XCAFKinObjects_KinematicPair::XCAFKinObjects_KinematicPair()
{
}

//=================================================================================================

const Standard_GUID& XCAFKinObjects_KinematicPair::GetID()
{
  static Standard_GUID PairID("89FB0996-A8B8-4085-87A9-63AB5D56D8C1");
  return PairID;
}

//=================================================================================================

const Standard_GUID& XCAFKinObjects_KinematicPair::getUID()
{
  static Standard_GUID PairUID("89FB0996-A8B8-4085-87A9-63AB5D56D8C2");
  return PairUID;
}

//=================================================================================================

const Standard_GUID& XCAFKinObjects_KinematicPair::getLimitsID()
{
  static Standard_GUID KinematicLimitsID("8A9E9B60-7CA1-45F1-882A-42390D4DB894");
  return KinematicLimitsID;
}

//=================================================================================================

const Standard_GUID& XCAFKinObjects_KinematicPair::getParamsID()
{
  static Standard_GUID KinematicParamsID("6BE4AAD0-36BE-4D17-B65C-0B2062E04D92");
  return KinematicParamsID;
}

//=================================================================================================

Standard_Boolean XCAFKinObjects_KinematicPair::IsValidLabel(const TDF_Label& theLabel)
{
  TDataStd_UAttribute::Set(theLabel, XCAFKinObjects_KinematicPair::getUID());
  Handle(TDataStd_UAttribute) anAttr;
  return theLabel.FindAttribute(XCAFKinObjects_KinematicPair::getUID(), anAttr);
}

//=================================================================================================

Handle(XCAFKinObjects_KinematicPair) XCAFKinObjects_KinematicPair::Set(const TDF_Label& theLabel)
{
  TDataStd_UAttribute::Set(theLabel, XCAFKinObjects_KinematicPair::getUID());
  Handle(XCAFKinObjects_KinematicPair) anAttr;
  if (!theLabel.FindAttribute(XCAFKinObjects_KinematicPair::GetID(), anAttr))
  {
    anAttr = new XCAFKinObjects_KinematicPair();
    theLabel.AddAttribute(anAttr);
  }
  return anAttr;
}

//=================================================================================================

void XCAFKinObjects_KinematicPair::SetObject(const Handle(XCAFKinObjects_PairObject)& theObject)
{
  Backup();

  // Common attributes
  if (!theObject->Name().IsEmpty())
    TDataStd_Name::Set(Label(), theObject->Name());
  TDataStd_Integer::Set(Label(), theObject->Type());
  TDataXtd_Plane::Set(Label().FindChild(ChildLab_FirstTrsf),
                      gp_Pln(theObject->FirstTransformation()));
  TDataXtd_Plane::Set(Label().FindChild(ChildLab_SecondTrsf),
                      gp_Pln(theObject->SecondTransformation()));

  if (theObject->HasLimits())
  {
    Handle(TDataStd_RealArray) aLimitsAttr;
    aLimitsAttr =
      TDataStd_RealArray::Set(Label(), getLimitsID(), 1, theObject->GetAllLimits()->Length());
    aLimitsAttr->ChangeArray(theObject->GetAllLimits());
  }

  // Low order pairs
  if (theObject->Type() == XCAFKinObjects_PairType_Universal
      || theObject->Type() == XCAFKinObjects_PairType_Homokinetic)
  {
    Handle(XCAFKinObjects_LowOrderPairObject) anObject =
      Handle(XCAFKinObjects_LowOrderPairObject)::DownCast(theObject);
    TDataStd_Real::Set(Label(), getParamsID(), anObject->SkewAngle());
  }

  // Low order pairs with motion coupling
  if (theObject->Type() >= XCAFKinObjects_PairType_Screw
      && theObject->Type() <= XCAFKinObjects_PairType_LinearFlexibleAndPinion)
  {
    Handle(XCAFKinObjects_LowOrderPairObjectWithCoupling) anObject =
      Handle(XCAFKinObjects_LowOrderPairObjectWithCoupling)::DownCast(theObject);
    if (!anObject->GetAllParams().IsNull() && !anObject->GetAllParams()->IsEmpty())
    {
      Handle(TDataStd_RealArray) aParamsAttr;
      aParamsAttr =
        TDataStd_RealArray::Set(Label(), getParamsID(), 1, anObject->GetAllParams()->Upper());
      aParamsAttr->ChangeArray(anObject->GetAllParams());
    }
  }

  // High order pairs
  if (theObject->Type() >= XCAFKinObjects_PairType_PointOnSurface
      && theObject->Type() <= XCAFKinObjects_PairType_LinearFlexibleAndPlanarCurve)
  {
    Handle(XCAFKinObjects_HighOrderPairObject) anObject =
      Handle(XCAFKinObjects_HighOrderPairObject)::DownCast(theObject);
    TDataStd_Integer::Set(Label(), getParamsID(), (Standard_Integer)anObject->Orientation());
    BRep_Builder aBuilder;
    if (theObject->Type() == XCAFKinObjects_PairType_PointOnSurface && !anObject->Surface().IsNull())
    {
      TopoDS_Face aFace;
      aBuilder.MakeFace(aFace, anObject->Surface(), Precision::Confusion());
      TNaming_Builder aTNBuild(Label().FindChild(ChildLab_FirstGeomParam));
      aTNBuild.Generated(aFace);
    }
    if (theObject->Type() == XCAFKinObjects_PairType_SlidingSurface
        || theObject->Type() == XCAFKinObjects_PairType_RollingSurface)
    {
      TopoDS_Face aFace1, aFace2;
      if (!anObject->FirstSurface().IsNull())
      {
        aBuilder.MakeFace(aFace1, anObject->FirstSurface(), Precision::Confusion());
        TNaming_Builder aTNBuild1(Label().FindChild(ChildLab_FirstGeomParam));
        aTNBuild1.Generated(aFace1);
      }
      if (!anObject->SecondSurface().IsNull())
      {
        aBuilder.MakeFace(aFace2, anObject->SecondSurface(), Precision::Confusion());
        TNaming_Builder aTNBuild2(Label().FindChild(ChildLab_SecondGeomParam));
        aTNBuild2.Generated(aFace2);
      }
    }
    if ((theObject->Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
         || theObject->Type() == XCAFKinObjects_PairType_LinearFlexibleAndPlanarCurve)
        && !anObject->Curve().IsNull())
    {
      TopoDS_Edge anEdge;
      aBuilder.MakeEdge(anEdge, anObject->Curve(), Precision::Confusion());
      TNaming_Builder aTNBuild(Label().FindChild(ChildLab_FirstGeomParam));
      aTNBuild.Generated(anEdge);
    }
    if (anObject->Type() == XCAFKinObjects_PairType_SlidingCurve
        || anObject->Type() == XCAFKinObjects_PairType_RollingCurve)
    {
      TopoDS_Edge anEdge1, anEdge2;
      if (!anObject->FirstCurve().IsNull())
      {
        aBuilder.MakeEdge(anEdge1, anObject->FirstCurve(), Precision::Confusion());
        TNaming_Builder aTNBuild1(Label().FindChild(ChildLab_FirstGeomParam));
        aTNBuild1.Generated(anEdge1);
      }
      if (!anObject->SecondCurve().IsNull())
      {
        aBuilder.MakeEdge(anEdge2, anObject->SecondCurve(), Precision::Confusion());
        TNaming_Builder aTNBuild2(Label().FindChild(ChildLab_SecondGeomParam));
        aTNBuild2.Generated(anEdge2);
      }
    }
  }
}

//=================================================================================================

Handle(XCAFKinObjects_PairObject) XCAFKinObjects_KinematicPair::GetObject() const
{
  // Type
  Handle(TDataStd_Integer)      aTypeAttr;
  Handle(XCAFKinObjects_PairObject) anObject = new XCAFKinObjects_PairObject();
  if (Label().FindAttribute(TDataStd_Integer::GetID(), aTypeAttr))
  {
    int aType = aTypeAttr->Get();
    if (aType >= XCAFKinObjects_PairType_FullyConstrained && aType <= XCAFKinObjects_PairType_Unconstrained)
      anObject = new XCAFKinObjects_LowOrderPairObject();
    else if (aType >= XCAFKinObjects_PairType_Screw
             && aType <= XCAFKinObjects_PairType_LinearFlexibleAndPinion)
      anObject = new XCAFKinObjects_LowOrderPairObjectWithCoupling();
    else if (aType >= XCAFKinObjects_PairType_PointOnSurface
             && aType <= XCAFKinObjects_PairType_LinearFlexibleAndPlanarCurve)
      anObject = new XCAFKinObjects_HighOrderPairObject();
    anObject->SetType((XCAFKinObjects_PairType)aType);
  }

  // Name
  Handle(TDataStd_Name) aNameAttr;
  if (Label().FindAttribute(TDataStd_Name::GetID(), aNameAttr))
    anObject->SetName(aNameAttr->Get());

  // Transformations
  Handle(TDataXtd_Plane) aTrsfAttr;
  if (Label().FindChild(ChildLab_FirstTrsf).FindAttribute(TDataXtd_Plane::GetID(), aTrsfAttr))
  {
    gp_Pln aPlane;
    TDataXtd_Geometry::Plane(aTrsfAttr->Label(), aPlane);
    anObject->SetFirstTransformation(aPlane.Position());
  }
  if (Label().FindChild(ChildLab_SecondTrsf).FindAttribute(TDataXtd_Plane::GetID(), aTrsfAttr))
  {
    gp_Pln aPlane;
    TDataXtd_Geometry::Plane(aTrsfAttr->Label(), aPlane);
    anObject->SetSecondTransformation(aPlane.Position());
  }

  // Limits
  Handle(TDataStd_RealArray) aLimitsAttr;
  if (Label().FindAttribute(getLimitsID(), aLimitsAttr))
  {
    Handle(TColStd_HArray1OfReal) aLimitsArray = aLimitsAttr->Array();
    anObject->SetAllLimits(aLimitsArray);
  }

  // Low order pairs
  if (anObject->Type() == XCAFKinObjects_PairType_Universal
      || anObject->Type() == XCAFKinObjects_PairType_Homokinetic)
  {
    Handle(XCAFKinObjects_LowOrderPairObject) aDefObject =
      Handle(XCAFKinObjects_LowOrderPairObject)::DownCast(anObject);
    Handle(TDataStd_Real) aParamAttr;
    if (Label().FindAttribute(getParamsID(), aParamAttr))
    {
      aDefObject->SetSkewAngle(aParamAttr->Get());
    }
  }

  // Low order pairs with motion coupling
  if (anObject->Type() >= XCAFKinObjects_PairType_Screw
      && anObject->Type() <= XCAFKinObjects_PairType_LinearFlexibleAndPinion)
  {
    Handle(XCAFKinObjects_LowOrderPairObjectWithCoupling) aDefObject =
      Handle(XCAFKinObjects_LowOrderPairObjectWithCoupling)::DownCast(anObject);
    Handle(TDataStd_RealArray) aParamsAttr;
    if (Label().FindAttribute(getParamsID(), aParamsAttr))
    {
      Handle(TColStd_HArray1OfReal) aParamsArray = aParamsAttr->Array();
      aDefObject->SetAllParams(aParamsAttr->Array());
    }
  }

  // High order pairs
  if (anObject->Type() >= XCAFKinObjects_PairType_PointOnSurface
      && anObject->Type() <= XCAFKinObjects_PairType_LinearFlexibleAndPlanarCurve)
  {
    Handle(XCAFKinObjects_HighOrderPairObject) aDefObject =
      Handle(XCAFKinObjects_HighOrderPairObject)::DownCast(anObject);
    Handle(TDataStd_Integer) anOrienAttr;
    if (Label().FindAttribute(getParamsID(), anOrienAttr))
    {
      aDefObject->SetOrientation(anOrienAttr->Get() != 0);
    }

    Handle(TNaming_NamedShape) aNS;
    if (anObject->Type() == XCAFKinObjects_PairType_PointOnSurface)
    {
      if (Label()
            .FindChild(ChildLab_FirstGeomParam)
            .FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Face aFace = TopoDS::Face(TNaming_Tool::GetShape(aNS));
        aDefObject->SetSurface(BRep_Tool::Surface(aFace));
      }
    }
    if (anObject->Type() == XCAFKinObjects_PairType_SlidingSurface
        || anObject->Type() == XCAFKinObjects_PairType_RollingSurface)
    {
      if (Label()
            .FindChild(ChildLab_FirstGeomParam)
            .FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Face aFace = TopoDS::Face(TNaming_Tool::GetShape(aNS));
        aDefObject->SetFirstSurface(BRep_Tool::Surface(aFace));
      }
      if (Label()
            .FindChild(ChildLab_SecondGeomParam)
            .FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Face aFace = TopoDS::Face(TNaming_Tool::GetShape(aNS));
        aDefObject->SetSecondSurface(BRep_Tool::Surface(aFace));
      }
    }
    if (anObject->Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
        || anObject->Type() == XCAFKinObjects_PairType_LinearFlexibleAndPlanarCurve)
    {
      if (Label()
            .FindChild(ChildLab_FirstGeomParam)
            .FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Edge   anEdge = TopoDS::Edge(TNaming_Tool::GetShape(aNS));
        Standard_Real aFirst, aLast;
        aDefObject->SetCurve(BRep_Tool::Curve(anEdge, aFirst, aLast));
      }
    }
    if (anObject->Type() == XCAFKinObjects_PairType_SlidingCurve
        || anObject->Type() == XCAFKinObjects_PairType_RollingCurve)
    {
      if (Label()
            .FindChild(ChildLab_FirstGeomParam)
            .FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Edge   anEdge = TopoDS::Edge(TNaming_Tool::GetShape(aNS));
        Standard_Real aFirst, aLast;
        aDefObject->SetFirstCurve(BRep_Tool::Curve(anEdge, aFirst, aLast));
      }
      if (Label()
            .FindChild(ChildLab_SecondGeomParam)
            .FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Edge   anEdge = TopoDS::Edge(TNaming_Tool::GetShape(aNS));
        Standard_Real aFirst, aLast;
        aDefObject->SetSecondCurve(BRep_Tool::Curve(anEdge, aFirst, aLast));
      }
    }
  }

  return anObject;
}

//=================================================================================================

const Standard_GUID& XCAFKinObjects_KinematicPair::ID() const
{
  return GetID();
}

//=================================================================================================

void XCAFKinObjects_KinematicPair::Restore(const Handle(TDF_Attribute)& /*theWith*/) {}

//=================================================================================================

Handle(TDF_Attribute) XCAFKinObjects_KinematicPair::NewEmpty() const
{
  return new XCAFKinObjects_KinematicPair();
}

//=================================================================================================

void XCAFKinObjects_KinematicPair::Paste(const Handle(TDF_Attribute)& /*theInfo*/,
                                     const Handle(TDF_RelocationTable)& /*theRT*/) const
{
}

//=================================================================================================

void XCAFKinObjects_KinematicPair::DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)
}
