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

//: S4134 abv 10.03.99: working methods moved from package TopoDSToGBWire
//: j1 modified by abv 22 Oct 98: CSR BUC60401
// - unused parts of code dropped
// - fixed trimming of circles and ellipses (radians used instead of degrees)
// szv#4 S4163

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <GeomToStep_MakeCurve.hxx>
#include <GeomToStep_MakeLine.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NullObject.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepGeom_TrimmingSelect.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <TopoDSToStep_WireframeBuilder.hxx>
#include <Transfer_FinderProcess.hxx>

//=================================================================================================

TopoDSToStep_WireframeBuilder::TopoDSToStep_WireframeBuilder()
    : myError(TopoDSToStep_BuilderOther)
{
  done = false;
}

//=================================================================================================

TopoDSToStep_WireframeBuilder::TopoDSToStep_WireframeBuilder(
  const TopoDS_Shape&     aShape,
  TopoDSToStep_Tool&      aTool,
  const StepData_Factors& theLocalFactors)
{
  done = false;
  Init(aShape, aTool, theLocalFactors);
}

void TopoDSToStep_WireframeBuilder::Init(const TopoDS_Shape& aShape,
                                         TopoDSToStep_Tool& /* T */,
                                         const StepData_Factors& theLocalFactors)
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> itemList =
    new NCollection_HSequence<occ::handle<Standard_Transient>>();
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>
    aPmsMap;
  done     = GetTrimmedCurveFromShape(aShape, aPmsMap, itemList, theLocalFactors);
  myResult = itemList;
}

//=================================================================================================

TopoDSToStep_BuilderError TopoDSToStep_WireframeBuilder::Error() const
{
  return myError;
}

//=================================================================================================

const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>&
  TopoDSToStep_WireframeBuilder::Value() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_WireframeBuilder::Value() - no result");
  return myResult;
}

// ============================================================================
//: S4134: abv 10 Mar 99: the methods below moved from package TopoDSToGBWire

#define Nbpt 23

static occ::handle<StepGeom_TrimmedCurve> MakeTrimmedCurve(
  const occ::handle<StepGeom_Curve>&          C,
  const occ::handle<StepGeom_CartesianPoint>& P1,
  const occ::handle<StepGeom_CartesianPoint>& P2,
  double                                      trim1,
  double                                      trim2,
  bool                                        sense)
{
  occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>> aSTS1 =
    new NCollection_HArray1<StepGeom_TrimmingSelect>(1, 2);
  StepGeom_TrimmingSelect tSel;
  tSel.SetValue(P1);
  aSTS1->SetValue(1, tSel);
  tSel.SetParameterValue(trim1);
  aSTS1->SetValue(2, tSel);

  occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>> aSTS2 =
    new NCollection_HArray1<StepGeom_TrimmingSelect>(1, 2);
  tSel.SetValue(P2);
  aSTS2->SetValue(1, tSel);
  tSel.SetParameterValue(trim2);
  aSTS2->SetValue(2, tSel);

  occ::handle<TCollection_HAsciiString> empty = new TCollection_HAsciiString("");
  occ::handle<StepGeom_TrimmedCurve>    pmsTC = new StepGeom_TrimmedCurve;
  pmsTC->Init(empty, C, aSTS1, aSTS2, sense, StepGeom_tpParameter);
  return pmsTC;
}

bool TopoDSToStep_WireframeBuilder::GetTrimmedCurveFromEdge(
  const TopoDS_Edge& theEdge,
  const TopoDS_Face& theFace,
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>&
                                                                       theMap,
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& theCurveList,
  const StepData_Factors&                                              theLocalFactors) const
{
  if (theEdge.Orientation() == TopAbs_INTERNAL || theEdge.Orientation() == TopAbs_EXTERNAL)
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: TopoDSToStep_WireframeBuilder::GetTrimmedCurveFromEdge: Edge is "
                 "internal or external; dropped"
              << std::endl;
#endif
    return false;
  }
  // szv#4:S4163:12Mar99 SGI warns
  TopoDS_Shape aSh    = theEdge.Oriented(TopAbs_FORWARD);
  TopoDS_Edge  anEdge = TopoDS::Edge(aSh);

  // resulting curve
  occ::handle<StepGeom_Curve> aSGC;
  if (const occ::handle<Standard_Transient>* aTransient = theMap.Seek(anEdge))
  {
    aSGC = occ::down_cast<StepGeom_Curve>(*aTransient);
  }

  BRepAdaptor_Curve aCA;
  try
  {
    OCC_CATCH_SIGNALS
    aCA.Initialize(anEdge);
  }
  catch (Standard_NullObject const&)
  {
    return false;
  }

  TopoDS_Vertex                        aVFirst, aVLast;
  occ::handle<StepGeom_CartesianPoint> aSGCP1, aSGCP2;
  for (TopExp_Explorer anExp(anEdge, TopAbs_VERTEX); anExp.More(); anExp.Next())
  {
    TopoDS_Vertex aVertex = TopoDS::Vertex(anExp.Value());
    gp_Pnt        aGpP    = BRep_Tool::Pnt(aVertex);
    if (aVertex.Orientation() == TopAbs_FORWARD)
    {
      aVFirst = aVertex;
      // 1.point for trimming
      GeomToStep_MakeCartesianPoint aGTSMCP(aGpP, theLocalFactors.LengthFactor());
      aSGCP1 = aGTSMCP.Value();
    }
    if (aVertex.Orientation() == TopAbs_REVERSED)
    {
      aVLast = aVertex;
      // 2.point for trimming
      GeomToStep_MakeCartesianPoint aGTSMCP(aGpP, theLocalFactors.LengthFactor());
      aSGCP2 = aGTSMCP.Value();
    }
  }

  double                  aFirst, aLast;
  occ::handle<Geom_Curve> aC = BRep_Tool::Curve(anEdge, aFirst, aLast);

  if (!aC.IsNull())
  {
    if (aC->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
    {
      aC = occ::down_cast<Geom_TrimmedCurve>(aC)->BasisCurve();
    }
    GeomToStep_MakeCurve aGTSMC(aC, theLocalFactors);
    if (!aGTSMC.IsDone())
    {
      return false;
    }
    const occ::handle<StepGeom_Curve>& aPMSC = aGTSMC.Value();

    // trim the curve
    double aTrim1 = aCA.FirstParameter();
    double aTrim2 = aCA.LastParameter();

    if (aVFirst.IsNull() && aVLast.IsNull() && Precision::IsInfinite(aFirst)
        && Precision::IsInfinite(aLast))
    {
      GeomToStep_MakeCurve aCurveMaker(aC, theLocalFactors);
      if (aCurveMaker.IsDone())
      {
        aSGC = aCurveMaker.Value();
        theCurveList->Append(aSGC);
        return true;
      }
      return false;
    }
    if (aVFirst.IsNull())
    {
      GeomToStep_MakeCartesianPoint aGTSMCP(aCA.Value(aFirst), theLocalFactors.LengthFactor());
      aSGCP1 = aGTSMCP.Value();
    }
    if (aVLast.IsNull())
    {
      GeomToStep_MakeCartesianPoint aGTSMCP(aCA.Value(aLast), theLocalFactors.LengthFactor());
      aSGCP2 = aGTSMCP.Value();
    }

    /* //:j1 abv 22 Oct 98: radians are used in the produced STEP file (at least by default)
       if(C->IsKind(STANDARD_TYPE(Geom_Circle)) ||
           C->IsKind(STANDARD_TYPE(Geom_Ellipse))) {
          double fact = 180. / M_PI;
          trim1 = trim1 * fact;
          trim2 = trim2 * fact;
        }
    */
    aSGC = MakeTrimmedCurve(aPMSC, aSGCP1, aSGCP2, aTrim1, aTrim2, true);
  }
  else
  {

    // -------------------------
    // a 3D Curve is constructed
    // -------------------------

    bool aIPlan = false;
    if (!theFace.IsNull())
    {
      double                    aCF, aCL;
      occ::handle<Geom2d_Curve> aC2d = BRep_Tool::CurveOnSurface(anEdge, theFace, aCF, aCL);
      occ::handle<Geom_Surface> aS   = BRep_Tool::Surface(theFace);
      if (aS->IsKind(STANDARD_TYPE(Geom_Plane)) && aC2d->IsKind(STANDARD_TYPE(Geom2d_Line)))
      {
        aIPlan = true;
      }
    }

    // to be modified : cf and cl are the topological trimming parameter
    // these are computed after ! (U1 and U2) -> cf and cl instead
    if (aIPlan)
    {
      gp_Pnt aPnt1 = aCA.Value(aCA.FirstParameter()), aPnt2 = aCA.Value(aCA.LastParameter());
      gp_Vec aV(aPnt1, aPnt2);
      double aLength = aV.Magnitude();
      if (aLength >= Precision::Confusion())
      {
        occ::handle<Geom_Line> aL = new Geom_Line(aPnt1, gp_Dir(aV));
        GeomToStep_MakeLine    aGTSML(aL, theLocalFactors);
        aSGC = aGTSML.Value();
        aSGC = MakeTrimmedCurve(aGTSML.Value(), aSGCP1, aSGCP2, 0, aLength, true);
      }
#ifdef OCCT_DEBUG
      else
        std::cout << "Warning: TopoDSToStep_WireframeBuilder::GetTrimmedCurveFromEdge: Null-length "
                     "curve not mapped"
                  << std::endl;
#endif
    }
    else
    {
      NCollection_Array1<gp_Pnt> aPoints(1, Nbpt);
      NCollection_Array1<double> aKnots(1, Nbpt);
      NCollection_Array1<int>    aMult(1, Nbpt);
      double                     aU1 = aCA.FirstParameter();
      double                     aU2 = aCA.LastParameter();
      for (int i = 1; i <= Nbpt; i++)
      {
        double aU = aU1 + (i - 1) * (aU2 - aU1) / (Nbpt - 1);
        gp_Pnt aP = aCA.Value(aU);
        aPoints.SetValue(i, aP);
        aKnots.SetValue(i, aU);
        aMult.SetValue(i, 1);
      }
      aPoints.SetValue(1, BRep_Tool::Pnt(aVFirst));
      aPoints.SetValue(Nbpt, BRep_Tool::Pnt(aVLast));
      aMult.SetValue(1, 2);
      aMult.SetValue(Nbpt, 2);
      occ::handle<Geom_Curve> aBSCurve = new Geom_BSplineCurve(aPoints, aKnots, aMult, 1);
      GeomToStep_MakeCurve    aGTSMC(aBSCurve, theLocalFactors);
      aSGC = aGTSMC.Value();
    }
  }

  if (aSGC.IsNull())
  {
    return false;
  }

  theMap.Bind(anEdge, aSGC);
  theCurveList->Append(aSGC);
  return true;
}

bool TopoDSToStep_WireframeBuilder::GetTrimmedCurveFromFace(
  const TopoDS_Face& aFace,
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>& aMap,
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& aCurveList,
  const StepData_Factors&                                              theLocalFactors) const
{
  TopoDS_Shape    curShape;
  TopoDS_Edge     curEdge;
  TopExp_Explorer exp;
  // clang-format off
  bool result = false; //szv#4:S4163:12Mar99 `done` hid one from this, initialisation needed
  // clang-format on

  for (exp.Init(aFace, TopAbs_EDGE); exp.More(); exp.Next())
  {
    curShape = exp.Current();
    curEdge  = TopoDS::Edge(curShape);
    if (GetTrimmedCurveFromEdge(curEdge, aFace, aMap, aCurveList, theLocalFactors))
      result = true;
  }
  return result;
}

bool TopoDSToStep_WireframeBuilder::GetTrimmedCurveFromShape(
  const TopoDS_Shape& aShape,
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>& aMap,
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& aCurveList,
  const StepData_Factors&                                              theLocalFactors) const
{
  TopoDS_Iterator It;
  // clang-format off
  bool result = false; //szv#4:S4163:12Mar99 `done` hid one from this, initialisation needed
  // clang-format on

  // szv#4:S4163:12Mar99 optimized
  switch (aShape.ShapeType())
  {
    case TopAbs_EDGE: {
      const TopoDS_Edge& curEdge = TopoDS::Edge(aShape);
      TopoDS_Face        nulFace;
      result = GetTrimmedCurveFromEdge(curEdge, nulFace, aMap, aCurveList, theLocalFactors);
      break;
    }
    case TopAbs_WIRE: {
      TopoDS_Face     nulFace;
      TopoDS_Shape    curShape;
      TopoDS_Edge     curEdge;
      TopExp_Explorer exp;

      for (exp.Init(aShape, TopAbs_EDGE); exp.More(); exp.Next())
      {
        curShape = exp.Current();
        curEdge  = TopoDS::Edge(curShape);
        if (GetTrimmedCurveFromEdge(curEdge, nulFace, aMap, aCurveList, theLocalFactors))
          result = true;
      }
      break;
    }
    case TopAbs_FACE: {
      const TopoDS_Face& curFace = TopoDS::Face(aShape);
      result = GetTrimmedCurveFromFace(curFace, aMap, aCurveList, theLocalFactors);
      break;
    }
    case TopAbs_SHELL: {
      TopoDS_Shell Sh = TopoDS::Shell(aShape);
      It.Initialize(Sh);
      for (; It.More(); It.Next())
      {
        TopoDS_Face curFace = TopoDS::Face(It.Value());
        if (GetTrimmedCurveFromFace(curFace, aMap, aCurveList, theLocalFactors))
          result = true;
#ifdef OCCT_DEBUG
        if (!result)
        {
          std::cout << "ERROR extracting trimmedCurve from Face" << std::endl;
          // BRepTools::Dump(curFace,std::cout);  std::cout<<std::endl;
        }
#endif
      }
      break;
    }
    case TopAbs_SOLID: {
      It.Initialize(aShape);
      for (; It.More(); It.Next())
      {
        if (It.Value().ShapeType() == TopAbs_SHELL)
        {
          if (GetTrimmedCurveFromShape(It.Value(), aMap, aCurveList, theLocalFactors))
            result = true;
        }
      }
      break;
    }
    case TopAbs_COMPOUND: {
      It.Initialize(aShape);
      for (; It.More(); It.Next())
      {
        /*	  if  ((It.Value().ShapeType() == TopAbs_SHELL) ||
                   (It.Value().ShapeType() == TopAbs_COMPOUND)) {
                result = GetTrimmedCurveFromShape(It.Value(), aMap, aCurveList);
                break;
              }
              else if (It.Value().ShapeType() == TopAbs_FACE) {
                result = GetTrimmedCurveFromFace(TopoDS::Face(It.Value()), aMap, aCurveList);
                break;
              } */
        if (GetTrimmedCurveFromShape(It.Value(), aMap, aCurveList, theLocalFactors))
          result = true;
      }
      break;
    }
    default:
      break;
  }
  return result;
}
