// Created on: 1994-06-17
// Created by: Modeling
// Copyright (c) 1994-1999 Matra Datavision
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

#include <algorithm>
#include <cmath>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepGProp.hxx>
#include <BRepLib.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <ChFiDS_ErrorStatus.hxx>
#include <ChFiDS_Spine.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Geom_Surface.hxx>
#include <GProp_GProps.hxx>
#include <Law_Linear.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

namespace
{
typedef NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>
  EdgeFaceMap;

double volumeOf(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}

//! True if the shape is valid, or becomes valid under the SameParameter pass callers apply next.
bool isValidResult(const TopoDS_Shape& theShape)
{
  if (BRepCheck_Analyzer(theShape).IsValid())
  {
    return true;
  }
  BRepBuilderAPI_Copy aCopy(theShape);
  BRepLib::SameParameter(aCopy.Shape(), Precision::Confusion(), true);
  return BRepCheck_Analyzer(aCopy.Shape()).IsValid();
}

//! True if the shape is one valid solid whose every edge borders two faces.
bool isClosedValidSolid(const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
  {
    return false;
  }
  int aNbSolids = 0;
  for (TopExp_Explorer anExp(theShape, TopAbs_SOLID); anExp.More(); anExp.Next())
  {
    ++aNbSolids;
  }
  if (aNbSolids != 1 || !BRepCheck_Analyzer(theShape).IsValid())
  {
    return false;
  }
  EdgeFaceMap anEdgeFaces;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, anEdgeFaces);
  for (int anIdx = 1; anIdx <= anEdgeFaces.Extent(); ++anIdx)
  {
    if (!BRep_Tool::Degenerated(TopoDS::Edge(anEdgeFaces.FindKey(anIdx)))
        && anEdgeFaces.FindFromIndex(anIdx).Extent() != 2)
    {
      return false;
    }
  }
  return true;
}

//! Fillet arc in the section of a right-angle convex edge, the edge at the origin.
struct SectionArc
{
  double myCx, myCy; //!< centre
  double myQ1, myQ2; //!< ends of the arc on face 1 and on face 2
  double myArea;     //!< area between the arc and the two faces, the part the fillet removes
};

//! Arc of radius theR on faces reaching theA and theB, through the far edge of one that runs out.
bool pivotArc(const double theA, const double theB, const double theR, SectionArc& theArc)
{
  const double aTol = Precision::Confusion();
  if (theR < std::min(theA, theB) - aTol)
  {
    return false;
  }
  const double aCx =
    theR >= theA - aTol ? std::sqrt(std::max(2. * theA * theR - theA * theA, 0.)) : -1.;
  const double aCy =
    theR >= theB - aTol ? std::sqrt(std::max(2. * theB * theR - theB * theB, 0.)) : -1.;
  if (aCx >= 0. && aCx <= theB + aTol)
  {
    theArc.myCx = theR;
    theArc.myCy = aCx;
    theArc.myQ1 = theA;
    theArc.myQ2 = aCx;
  }
  else if (aCy >= 0. && aCy <= theA + aTol)
  {
    theArc.myCx = aCy;
    theArc.myCy = theR;
    theArc.myQ1 = aCy;
    theArc.myQ2 = theB;
  }
  else
  {
    const double aChord = std::hypot(theA, theB);
    if (theR < aChord / 2.)
    {
      return false;
    }
    const double aH = std::sqrt(theR * theR - aChord * aChord / 4.);
    theArc.myCx     = theA / 2. + aH * theB / aChord;
    theArc.myCy     = theB / 2. + aH * theA / aChord;
    theArc.myQ1     = theA;
    theArc.myQ2     = theB;
  }
  const double aTheta =
    2. * std::asin(std::min(1., std::hypot(theArc.myQ1, theArc.myQ2) / (2. * theR)));
  theArc.myArea = theArc.myQ1 * theArc.myQ2 / 2. - theR * theR / 2. * (aTheta - std::sin(aTheta));
  return true;
}

//! Direction and width of a rectangular planar face from theEdge to the far edge, and its normal.
bool rectangleWidth(const TopoDS_Face& theFace,
                    const TopoDS_Edge& theEdge,
                    const gp_Pnt&      theOrigin,
                    const gp_Dir&      theAxis,
                    gp_Dir&            theAcross,
                    double&            theWidth,
                    gp_Dir&            theNormal)
{
  BRepAdaptor_Surface aSurf(theFace);
  if (aSurf.GetType() != GeomAbs_Plane)
  {
    return false;
  }
  gp_Pnt aPnt;
  gp_Vec aDU, aDV;
  aSurf.D1(0., 0., aPnt, aDU, aDV);
  const gp_Vec aNorm = aDU.Crossed(aDV);
  if (aNorm.SquareMagnitude() < gp::Resolution())
  {
    return false;
  }
  theNormal = gp_Dir(aNorm);
  if (theFace.Orientation() == TopAbs_REVERSED)
  {
    theNormal.Reverse();
  }
  int aNbEdges = 0, aNbParallel = 0;
  for (TopExp_Explorer anExp(theFace, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    BRepAdaptor_Curve  aCurve(anEdge);
    ++aNbEdges;
    if (aCurve.GetType() != GeomAbs_Line)
    {
      return false;
    }
    const double aDot = std::abs(aCurve.Line().Direction().Dot(theAxis));
    if (aDot > 1. - 1.e-9)
    {
      if (anEdge.IsSame(theEdge))
      {
        continue;
      }
      ++aNbParallel;
      const gp_Vec aVec(theOrigin, aCurve.Value(aCurve.FirstParameter()));
      const gp_Vec aPerp = aVec - gp_Vec(theAxis) * aVec.Dot(gp_Vec(theAxis));
      theWidth           = aPerp.Magnitude();
      if (theWidth < Precision::Confusion())
      {
        return false;
      }
      theAcross = gp_Dir(aPerp);
    }
    else if (aDot > 1.e-9)
    {
      return false;
    }
  }
  return aNbEdges == 4 && aNbParallel == 1;
}

//! Cuts the pivot fillet of theRadius on theEdge out of theShape, or returns a null shape.
TopoDS_Shape pivotCut(const TopoDS_Shape& theShape,
                      const TopoDS_Edge&  theEdge,
                      const double        theRadius)
{
  BRepAdaptor_Curve aCurve(theEdge);
  if (aCurve.GetType() != GeomAbs_Line)
  {
    return TopoDS_Shape();
  }
  int aNbSolids = 0;
  for (TopExp_Explorer anExp(theShape, TopAbs_SOLID); anExp.More(); anExp.Next())
  {
    ++aNbSolids;
  }
  EdgeFaceMap anEdgeFaces;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, anEdgeFaces);
  if (aNbSolids != 1 || !anEdgeFaces.Contains(theEdge))
  {
    return TopoDS_Shape();
  }
  const NCollection_List<TopoDS_Shape>& aFaces = anEdgeFaces.FindFromKey(theEdge);
  if (aFaces.Extent() != 2 || aFaces.First().IsSame(aFaces.Last()))
  {
    return TopoDS_Shape();
  }
  const gp_Dir aAxis   = aCurve.Line().Direction();
  const gp_Pnt aOrigin = aCurve.Value(aCurve.FirstParameter());
  const double aLength = aCurve.LastParameter() - aCurve.FirstParameter();
  gp_Dir       aU, aV, aN1, aN2;
  double       aA = 0., aB = 0.;
  if (!rectangleWidth(TopoDS::Face(aFaces.First()), theEdge, aOrigin, aAxis, aU, aA, aN1)
      || !rectangleWidth(TopoDS::Face(aFaces.Last()), theEdge, aOrigin, aAxis, aV, aB, aN2))
  {
    return TopoDS_Shape();
  }
  // A convex right angle: each face leads into the material side of the other.
  if (std::abs(aU.Dot(aV)) > 1.e-7 || aU.Dot(aN2) > -0.5 || aV.Dot(aN1) > -0.5)
  {
    return TopoDS_Shape();
  }
  SectionArc anArc;
  if (!pivotArc(aA, aB, theRadius, anArc))
  {
    return TopoDS_Shape();
  }

  // The tool: the region between the arc and the faces, enlarged and swept along the edge.
  const double aMargin = 0.1 * (aA + aB + aLength);
  const gp_Vec aStart  = gp_Vec(aAxis) * (-aMargin);
  auto         aPoint  = [&](const double theU, const double theW) {
    return aOrigin.Translated(gp_Vec(aU) * theU + gp_Vec(aV) * theW + aStart);
  };
  const gp_Pnt aQ1  = aPoint(anArc.myQ1, 0.);
  const gp_Pnt aQ2  = aPoint(0., anArc.myQ2);
  const gp_Pnt aCen = aPoint(anArc.myCx, anArc.myCy);
  const gp_Pnt aMid = aPoint((anArc.myQ1) / 2., (anArc.myQ2) / 2.);
  gp_Vec       aToMid(aCen, aMid);
  if (aToMid.SquareMagnitude() < gp::Resolution())
  {
    return TopoDS_Shape();
  }
  aToMid.Normalize();
  const gp_Pnt            aArcMid = aCen.Translated(aToMid * theRadius);
  const gp_Pnt            aS1     = aPoint(anArc.myQ1 + aMargin, -aMargin);
  const gp_Pnt            aS2     = aPoint(-aMargin, anArc.myQ2 + aMargin);
  const gp_Pnt            aS0     = aPoint(-aMargin, -aMargin);
  BRepBuilderAPI_MakeWire aWire;
  aWire.Add(BRepBuilderAPI_MakeEdge(aS1, aQ1).Edge());
  aWire.Add(BRepBuilderAPI_MakeEdge(GC_MakeArcOfCircle(aQ1, aArcMid, aQ2).Value()).Edge());
  aWire.Add(BRepBuilderAPI_MakeEdge(aQ2, aS2).Edge());
  aWire.Add(BRepBuilderAPI_MakeEdge(aS2, aS0).Edge());
  aWire.Add(BRepBuilderAPI_MakeEdge(aS0, aS1).Edge());
  if (!aWire.IsDone())
  {
    return TopoDS_Shape();
  }
  BRepBuilderAPI_MakeFace aFace(aWire.Wire(), true);
  if (!aFace.IsDone())
  {
    return TopoDS_Shape();
  }
  TopoDS_Shape aTool =
    BRepPrimAPI_MakePrism(aFace.Face(), gp_Vec(aAxis) * (aLength + 2. * aMargin)).Shape();
  if (volumeOf(aTool) < 0.)
  {
    aTool.Reverse();
  }
  BRepAlgoAPI_Cut aCut(theShape, aTool);
  if (!aCut.IsDone() || aCut.HasErrors() || !isClosedValidSolid(aCut.Shape()))
  {
    return TopoDS_Shape();
  }
  // The section is the same along the edge, so the volume that went is the area times the length.
  const double aVolume  = volumeOf(theShape);
  const double aRemoved = anArc.myArea * aLength;
  if (std::abs(aVolume - volumeOf(aCut.Shape()) - aRemoved) > 1.e-6 * aRemoved + 1.e-10 * aVolume)
  {
    return TopoDS_Shape();
  }
  return aCut.Shape();
}

//! The most contours the fallback intersects: each costs a fillet and a Boolean operation.
const int THE_MAX_CONTOURS = 8;

//! Intersection of each contour filleted alone, after Compute() failed; null if not applicable.
TopoDS_Shape fallbackFillets(ChFi3d_FilBuilder& theBuilder, const ChFi3d_FilletShape theShape)
{
  const int aNbContours = theBuilder.NbElements();
  if (aNbContours < 1 || aNbContours > THE_MAX_CONTOURS)
  {
    return TopoDS_Shape();
  }
  const TopoDS_Shape&                                           aShape = theBuilder.InitialShape();
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aVertices;
  for (int aContour = 1; aContour <= aNbContours; ++aContour)
  {
    const occ::handle<ChFiDS_Spine> aSpine = theBuilder.Value(aContour);
    if (aSpine.IsNull() || aSpine->NbEdges() < 1 || !theBuilder.IsConstant(aContour))
    {
      return TopoDS_Shape();
    }
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aOwn;
    for (int anEdge = 1; anEdge <= aSpine->NbEdges(); ++anEdge)
    {
      TopExp::MapShapes(aSpine->Edges(anEdge), TopAbs_VERTEX, aOwn);
    }
    for (int aVertex = 1; aVertex <= aOwn.Extent(); ++aVertex)
    {
      if (aVertices.Contains(aOwn(aVertex)))
      {
        return TopoDS_Shape();
      }
    }
    for (int aVertex = 1; aVertex <= aOwn.Extent(); ++aVertex)
    {
      aVertices.Add(aOwn(aVertex));
    }
  }

  const double aVolume = volumeOf(aShape);
  TopoDS_Shape aResult;
  double       aSumRemoved = 0., aMaxRemoved = 0.;
  for (int aContour = 1; aContour <= aNbContours; ++aContour)
  {
    const TopoDS_Edge& aFirst  = theBuilder.Value(aContour)->Edges(1);
    const double       aRadius = theBuilder.Radius(aContour);
    TopoDS_Shape       aOne;
    if (aNbContours > 1)
    {
      BRepFilletAPI_MakeFillet aSingle(aShape, theShape);
      aSingle.Add(aRadius, aFirst);
      aSingle.Build();
      if (aSingle.IsDone() && isClosedValidSolid(aSingle.Shape()))
      {
        aOne = aSingle.Shape();
      }
    }
    if (aOne.IsNull() && theBuilder.Value(aContour)->NbEdges() == 1)
    {
      aOne = pivotCut(aShape, aFirst, aRadius);
    }
    if (aOne.IsNull())
    {
      return TopoDS_Shape();
    }
    const double aRemoved = aVolume - volumeOf(aOne);
    if (aRemoved <= 0.)
    {
      // Only fillets that take material away are an intersection.
      return TopoDS_Shape();
    }
    aSumRemoved += aRemoved;
    aMaxRemoved = std::max(aMaxRemoved, aRemoved);
    if (aResult.IsNull())
    {
      aResult = aOne;
      continue;
    }
    BRepAlgoAPI_Common aCommon(aResult, aOne);
    if (!aCommon.IsDone() || aCommon.HasErrors() || !isClosedValidSolid(aCommon.Shape()))
    {
      return TopoDS_Shape();
    }
    aResult = aCommon.Shape();
  }
  // What the intersection removes is at least what any one fillet removes and at most the sum.
  const double aTol     = 1.e-6 * aVolume;
  const double aRemoved = aVolume - volumeOf(aResult);
  if (aRemoved < aMaxRemoved - aTol || aRemoved > aSumRemoved + aTol)
  {
    return TopoDS_Shape();
  }
  return aResult;
}
} // namespace

//=================================================================================================

BRepFilletAPI_MakeFillet::BRepFilletAPI_MakeFillet(const TopoDS_Shape&      S,
                                                   const ChFi3d_FilletShape FShape)
    : myBuilder(S, FShape)
{
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetParams(const double Tang,
                                         const double Tesp,
                                         const double T2d,
                                         const double TApp3d,
                                         const double TolApp2d,
                                         const double Fleche)
{
  myBuilder.SetParams(Tang, Tesp, T2d, TApp3d, TolApp2d, Fleche);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetContinuity(const GeomAbs_Shape InternalContinuity,
                                             const double        AngleTol)
{
  myBuilder.SetContinuity(InternalContinuity, AngleTol);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const TopoDS_Edge& E)
{
  myBuilder.Add(E);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const double Radius, const TopoDS_Edge& E)
{
  // myBuilder.Add(Radius,E);
  myBuilder.Add(E);
  int IinC;
  int IC = myBuilder.Contains(E, IinC);
  if (IC)
  {
    SetRadius(Radius, IC, IinC);
  }
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const double R1, const double R2, const TopoDS_Edge& E)
{
  myBuilder.Add(E);
  int IinC;
  int IC = myBuilder.Contains(E, IinC);
  if (IC)
  {
    SetRadius(R1, R2, IC, IinC);
  }
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const occ::handle<Law_Function>& L, const TopoDS_Edge& E)
{
  // myBuilder.Add(L,E);
  myBuilder.Add(E);
  int IinC;
  int IC = myBuilder.Contains(E, IinC);
  if (IC)
  {
    SetRadius(L, IC, IinC);
  }
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const NCollection_Array1<gp_Pnt2d>& UandR, const TopoDS_Edge& E)
{
  myBuilder.Add(E);
  int IinC;
  int IC = myBuilder.Contains(E, IinC);
  if (IC)
  {
    SetRadius(UandR, IC, IinC);
  }
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const double Radius, const int IC, const int IinC)
{
  gp_XY FirstUandR(0., Radius), LastUandR(1., Radius);
  myBuilder.SetRadius(FirstUandR, IC, IinC);
  myBuilder.SetRadius(LastUandR, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const double R1,
                                         const double R2,
                                         const int    IC,
                                         const int    IinC)
{
  double r1, r2;

  if (std::abs(R1 - R2) < Precision::Confusion())
  {
    r1 = r2 = (R1 + R2) * 0.5;
  }
  else
  {
    r1 = R1;
    r2 = R2;
  }
  gp_XY FirstUandR(0., r1), LastUandR(1., r2);
  myBuilder.SetRadius(FirstUandR, IC, IinC);
  myBuilder.SetRadius(LastUandR, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const occ::handle<Law_Function>& L,
                                         const int                        IC,
                                         const int                        IinC)
{
  myBuilder.SetRadius(L, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const NCollection_Array1<gp_Pnt2d>& UandR,
                                         const int                           IC,
                                         const int                           IinC)
{
  if (UandR.Length() == 1)
  {
    SetRadius(UandR(UandR.Lower()).Y(), IC, IinC);
  }
  else if (UandR.Length() == 2)
  {
    SetRadius(UandR(UandR.Lower()).Y(), UandR(UandR.Upper()).Y(), IC, IinC);
  }
  else
  {
    double Uf = UandR(UandR.Lower()).X();
    double Ul = UandR(UandR.Upper()).X();
    for (int i = UandR.Lower(); i <= UandR.Upper(); i++)
    {
      double Ucur = UandR(i).X();
      Ucur        = (Ucur - Uf) / (Ul - Uf);
      gp_XY newUandR(Ucur, UandR(i).Y());
      myBuilder.SetRadius(newUandR, IC, IinC);
    }
  }
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::IsConstant(const int IC)
{
  return myBuilder.IsConstant(IC);
}

//=================================================================================================

double BRepFilletAPI_MakeFillet::Radius(const int IC)
{
  return myBuilder.Radius(IC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::ResetContour(const int IC)
{
  myBuilder.ResetContour(IC);
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::IsConstant(const int IC, const TopoDS_Edge& E)
{
  return myBuilder.IsConstant(IC, E);
}

//=================================================================================================

double BRepFilletAPI_MakeFillet::Radius(const int IC, const TopoDS_Edge& E)
{
  return myBuilder.Radius(IC, E);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const double Radius, const int IC, const TopoDS_Edge& E)
{
  myBuilder.SetRadius(Radius, IC, E);
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::GetBounds(const int IC, const TopoDS_Edge& E, double& F, double& L)
{
  return myBuilder.GetBounds(IC, E, F, L);
}

//=================================================================================================

occ::handle<Law_Function> BRepFilletAPI_MakeFillet::GetLaw(const int IC, const TopoDS_Edge& E)
{
  return myBuilder.GetLaw(IC, E);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetLaw(const int                        IC,
                                      const TopoDS_Edge&               E,
                                      const occ::handle<Law_Function>& L)
{
  myBuilder.SetLaw(IC, E, L);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const double Radius, const int IC, const TopoDS_Vertex& V)
{
  myBuilder.SetRadius(Radius, IC, V);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetFilletShape(const ChFi3d_FilletShape FShape)
{
  myBuilder.SetFilletShape(FShape);
}

//=================================================================================================

ChFi3d_FilletShape BRepFilletAPI_MakeFillet::GetFilletShape() const
{
  return myBuilder.GetFilletShape();
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbContours() const
{
  return myBuilder.NbElements();
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::Contour(const TopoDS_Edge& E) const
{
  return myBuilder.Contains(E);
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbEdges(const int I) const
{
  const occ::handle<ChFiDS_Spine>& Spine = myBuilder.Value(I);
  int                              n     = Spine->NbEdges();
  return n;
}

//=================================================================================================

const TopoDS_Edge& BRepFilletAPI_MakeFillet::Edge(const int I, const int J) const
{
  const occ::handle<ChFiDS_Spine>& Spine = myBuilder.Value(I);
  const TopoDS_Edge&               S     = Spine->Edges(J);
  return S;
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Remove(const TopoDS_Edge& E)
{
  myBuilder.Remove(E);
}

//=================================================================================================

double BRepFilletAPI_MakeFillet::Length(const int IC) const
{
  return myBuilder.Length(IC);
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeFillet::FirstVertex(const int IC) const
{
  return myBuilder.FirstVertex(IC);
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeFillet::LastVertex(const int IC) const
{
  return myBuilder.LastVertex(IC);
}

//=================================================================================================

double BRepFilletAPI_MakeFillet::Abscissa(const int IC, const TopoDS_Vertex& V) const
{
  return myBuilder.Abscissa(IC, V);
}

//=================================================================================================

double BRepFilletAPI_MakeFillet::RelativeAbscissa(const int IC, const TopoDS_Vertex& V) const
{
  return myBuilder.RelativeAbscissa(IC, V);
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::ClosedAndTangent(const int IC) const
{
  return myBuilder.ClosedAndTangent(IC);
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::Closed(const int IC) const
{
  return myBuilder.Closed(IC);
}

//=================================================================================================

occ::handle<TopOpeBRepBuild_HBuilder> BRepFilletAPI_MakeFillet::Builder() const
{
  return myBuilder.Builder();
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Build(const Message_ProgressRange& /*theRange*/)
{
  myBuilder.Compute();
  if (!myBuilder.IsDone() || !isValidResult(myBuilder.Shape()))
  {
    // Fillets that cross each other or run out of face: see fallbackFillets.
    TopoDS_Shape aFallback;
    try
    {
      aFallback = fallbackFillets(myBuilder, myBuilder.GetFilletShape());
    }
    catch (Standard_Failure const&)
    {
    }
    if (!aFallback.IsNull())
    {
      // The failed computation holds nothing of this result: clear its faulty contours.
      myBuilder.Reset();
      Done();
      myShape = aFallback;
      myMap.Clear();
      for (TopExp_Explorer anExp(myShape, TopAbs_FACE); anExp.More(); anExp.Next())
      {
        myMap.Add(anExp.Current());
      }
    }
  }
  else
  {
    Done();
    myShape = myBuilder.Shape();

    // creation of the Map.
    TopExp_Explorer ex;
    for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next())
    {
      myMap.Add(ex.Current());
    }
  }
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Reset()
{
  NotDone();
  myBuilder.Reset();
  myMap.Clear();
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbSurfaces() const
{
  return (myBuilder.Builder()->DataStructure())->NbSurfaces();
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepFilletAPI_MakeFillet::NewFaces(const int I)
{
  return (*(NCollection_List<TopoDS_Shape>*)&(myBuilder.Builder()->NewFaces(I)));
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Simulate(const int IC)
{
  myBuilder.Simulate(IC);
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbSurf(const int IC) const
{
  return myBuilder.NbSurf(IC);
}

//=================================================================================================

occ::handle<NCollection_HArray1<ChFiDS_CircSection>> BRepFilletAPI_MakeFillet::Sect(
  const int IC,
  const int IS) const
{
  return myBuilder.Sect(IC, IS);
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepFilletAPI_MakeFillet::Generated(const TopoDS_Shape& EorV)
{
  return myBuilder.Generated(EorV);
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepFilletAPI_MakeFillet::Modified(const TopoDS_Shape& F)
{
  myGenerated.Clear();

  if (myBuilder.Builder()->IsSplit(F, TopAbs_OUT))
  {
    NCollection_List<TopoDS_Shape>::Iterator It(myBuilder.Builder()->Splits(F, TopAbs_OUT));
    for (; It.More(); It.Next())
    {
      myGenerated.Append(It.Value());
    }
  }
  if (myBuilder.Builder()->IsSplit(F, TopAbs_IN))
  {
    NCollection_List<TopoDS_Shape>::Iterator It(myBuilder.Builder()->Splits(F, TopAbs_IN));
    for (; It.More(); It.Next())
    {
      myGenerated.Append(It.Value());
    }
  }
  if (myBuilder.Builder()->IsSplit(F, TopAbs_ON))
  {
    NCollection_List<TopoDS_Shape>::Iterator It(myBuilder.Builder()->Splits(F, TopAbs_ON));
    for (; It.More(); It.Next())
    {
      myGenerated.Append(It.Value());
    }
  }
  return myGenerated;
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::IsDeleted(const TopoDS_Shape& F)
{
  return !(myMap.Contains(F) || myBuilder.Builder()->IsSplit(F, TopAbs_OUT)
           || myBuilder.Builder()->IsSplit(F, TopAbs_IN)
           || myBuilder.Builder()->IsSplit(F, TopAbs_ON));
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbFaultyContours() const
{
  return myBuilder.NbFaultyContours();
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::FaultyContour(const int I) const
{
  return myBuilder.FaultyContour(I);
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbComputedSurfaces(const int IC) const
{
  return myBuilder.NbComputedSurfaces(IC);
}

//=================================================================================================

occ::handle<Geom_Surface> BRepFilletAPI_MakeFillet::ComputedSurface(const int IC,
                                                                    const int IS) const
{
  return myBuilder.ComputedSurface(IC, IS);
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbFaultyVertices() const
{
  return myBuilder.NbFaultyVertices();
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeFillet::FaultyVertex(const int IV) const
{
  return myBuilder.FaultyVertex(IV);
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::HasResult() const
{
  return myBuilder.HasResult();
}

//=================================================================================================

TopoDS_Shape BRepFilletAPI_MakeFillet::BadShape() const
{
  return myBuilder.BadShape();
}

//=================================================================================================

ChFiDS_ErrorStatus BRepFilletAPI_MakeFillet::StripeStatus(const int IC) const
{
  return myBuilder.StripeStatus(IC);
}
