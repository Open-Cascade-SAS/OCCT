// Created by: Peter KURNEV
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

#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_Range.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

//=================================================================================================

void BOPTools_AlgoTools::UpdateVertex(const TopoDS_Vertex& aVF, const TopoDS_Vertex& aNewVertex)
{
  double aTolVF, aTolNewVertex, aDist, aNewTol;
  //
  gp_Pnt aPVF        = BRep_Tool::Pnt(aVF);
  gp_Pnt aPNewVertex = BRep_Tool::Pnt(aNewVertex);
  aTolVF             = BRep_Tool::Tolerance(aVF);
  aTolNewVertex      = BRep_Tool::Tolerance(aNewVertex);

  aDist   = aPVF.Distance(aPNewVertex);
  aNewTol = aDist + aTolNewVertex;

  if (aNewTol > aTolVF)
  {
    BRep_Builder BB;
    BB.UpdateVertex(aVF, aNewTol + BOPTools_AlgoTools::DTolerance());
  }
}

//=================================================================================================

void BOPTools_AlgoTools::UpdateVertex(const TopoDS_Edge&   aE,
                                      const double  aT,
                                      const TopoDS_Vertex& aV)
{
  double aTolV, aDist, aFirst, aLast;
  gp_Pnt        aPc;

  gp_Pnt aPv = BRep_Tool::Pnt(aV);
  aTolV      = BRep_Tool::Tolerance(aV);

  GeomAdaptor_Curve aCA(BRep_Tool::Curve(aE, aFirst, aLast));
  aCA.D0(aT, aPc);
  aDist = aPv.Distance(aPc);
  if (aDist > aTolV)
  {
    BRep_Builder BB;
    BB.UpdateVertex(aV, aDist + BOPTools_AlgoTools::DTolerance());
  }
}

//
//=================================================================================================

void BOPTools_AlgoTools::UpdateVertex(const IntTools_Curve& aC,
                                      const double   aT,
                                      const TopoDS_Vertex&  aV)
{
  double aTolV, aDist;
  gp_Pnt        aPc;

  gp_Pnt aPv = BRep_Tool::Pnt(aV);
  aTolV      = BRep_Tool::Tolerance(aV);

  GeomAdaptor_Curve aCA(aC.Curve());
  aCA.D0(aT, aPc);
  aDist = aPv.Distance(aPc);
  if (aDist > aTolV)
  {
    BRep_Builder BB;
    BB.UpdateVertex(aV, aDist + BOPTools_AlgoTools::DTolerance());
  }
}

//=================================================================================================

void BOPTools_AlgoTools::MakeSectEdge(const IntTools_Curve& aIC,
                                      const TopoDS_Vertex&  aV1,
                                      const double   aP1,
                                      const TopoDS_Vertex&  aV2,
                                      const double   aP2,
                                      TopoDS_Edge&          aNewEdge)
{
  const occ::handle<Geom_Curve>& aC = aIC.Curve();

  BRepBuilderAPI_MakeEdge aMakeEdge(aC, aV1, aV2, aP1, aP2);

  const TopoDS_Edge& aE = TopoDS::Edge(aMakeEdge.Shape());
  //
  // Range must be as it was !
  BRep_Builder aBB;
  aBB.Range(aE, aP1, aP2);
  //
  aNewEdge = aE;
}

//=================================================================================================

TopoDS_Edge BOPTools_AlgoTools::CopyEdge(const TopoDS_Edge& theEdge)
{
  TopoDS_Edge aNewEdge = TopoDS::Edge(theEdge.Oriented(TopAbs_FORWARD));
  aNewEdge.EmptyCopy();
  for (TopoDS_Iterator it(theEdge, false); it.More(); it.Next())
    BRep_Builder().Add(aNewEdge, it.Value());
  aNewEdge.Orientation(theEdge.Orientation());
  return aNewEdge;
}

//=================================================================================================

void BOPTools_AlgoTools::MakeSplitEdge(const TopoDS_Edge&   aE,
                                       const TopoDS_Vertex& aV1,
                                       const double  aP1,
                                       const TopoDS_Vertex& aV2,
                                       const double  aP2,
                                       TopoDS_Edge&         aNewEdge)
{
  TopoDS_Edge E = TopoDS::Edge(aE.Oriented(TopAbs_FORWARD));
  E.EmptyCopy();
  //
  BRep_Builder BB;
  if (!aV1.IsNull())
  {
    if (aP1 < aP2)
    {
      BB.Add(E, TopoDS::Vertex(aV1.Oriented(TopAbs_FORWARD)));
    }
    else
    {
      BB.Add(E, TopoDS::Vertex(aV1.Oriented(TopAbs_REVERSED)));
    }
  }
  if (!aV2.IsNull())
  {
    if (aP1 < aP2)
    {
      BB.Add(E, TopoDS::Vertex(aV2.Oriented(TopAbs_REVERSED)));
    }
    else
    {
      BB.Add(E, TopoDS::Vertex(aV2.Oriented(TopAbs_FORWARD)));
    }
  }

  if (aP1 < aP2)
  {
    BB.Range(E, aP1, aP2);
  }
  else
  {
    BB.Range(E, aP2, aP1);
  }

  aNewEdge = E;
  aNewEdge.Orientation(aE.Orientation());
}

//=================================================================================================

void BOPTools_AlgoTools::MakeNewVertex(const TopoDS_Vertex& aV1,
                                       const TopoDS_Vertex& aV2,
                                       TopoDS_Vertex&       aNewVertex)
{
  gp_Pnt        aPnt1 = BRep_Tool::Pnt(aV1);
  double aTol1 = BRep_Tool::Tolerance(aV1);

  gp_Pnt        aPnt2 = BRep_Tool::Pnt(aV2);
  double aTol2 = BRep_Tool::Tolerance(aV2);

  double aMaxTol, aDist;

  aDist   = aPnt1.Distance(aPnt2);
  aMaxTol = (aTol1 > aTol2) ? aTol1 : aTol2;
  aMaxTol = aMaxTol + 0.5 * aDist;

  const gp_XYZ& aXYZ1   = aPnt1.XYZ();
  const gp_XYZ& aXYZ2   = aPnt2.XYZ();
  gp_XYZ        aNewXYZ = 0.5 * (aXYZ1 + aXYZ2);

  gp_Pnt       aNewPnt(aNewXYZ);
  BRep_Builder aBB;
  aBB.MakeVertex(aNewVertex, aNewPnt, aMaxTol);
}

//=================================================================================================

void BOPTools_AlgoTools::MakeNewVertex(const gp_Pnt&       aP,
                                       const double aTol,
                                       TopoDS_Vertex&      aNewVertex)
{
  BRep_Builder aBB;
  aBB.MakeVertex(aNewVertex, aP, aTol);
}

//=================================================================================================

void BOPTools_AlgoTools::MakeNewVertex(const TopoDS_Edge&  aE1,
                                       const double aParm1,
                                       const TopoDS_Edge&  aE2,
                                       const double aParm2,
                                       TopoDS_Vertex&      aNewVertex)
{
  double aTol1, aTol2, aMaxTol, aDist;
  gp_Pnt        aPnt1, aPnt2;

  PointOnEdge(aE1, aParm1, aPnt1);
  PointOnEdge(aE2, aParm2, aPnt2);

  aTol1 = BRep_Tool::Tolerance(aE1);
  aTol2 = BRep_Tool::Tolerance(aE2);

  aDist   = aPnt1.Distance(aPnt2);
  aMaxTol = (aTol1 > aTol2) ? aTol1 : aTol2;
  aMaxTol = aMaxTol + 0.5 * aDist;

  const gp_XYZ& aXYZ1   = aPnt1.XYZ();
  const gp_XYZ& aXYZ2   = aPnt2.XYZ();
  gp_XYZ        aNewXYZ = 0.5 * (aXYZ1 + aXYZ2);

  gp_Pnt       aNewPnt(aNewXYZ);
  BRep_Builder aBB;
  aBB.MakeVertex(aNewVertex, aNewPnt, aMaxTol);
}

//=================================================================================================

void BOPTools_AlgoTools::MakeNewVertex(const TopoDS_Edge&  aE1,
                                       const double aParm1,
                                       const TopoDS_Face&  aF1,
                                       TopoDS_Vertex&      aNewVertex)
{
  double aTol1, aTol2, aMaxTol;
  gp_Pnt        aPnt;

  PointOnEdge(aE1, aParm1, aPnt);

  aTol1 = BRep_Tool::Tolerance(aE1);
  aTol2 = BRep_Tool::Tolerance(aF1);
  //
  aMaxTol = aTol1 + aTol2 + BOPTools_AlgoTools::DTolerance();
  //
  BRep_Builder aBB;
  aBB.MakeVertex(aNewVertex, aPnt, aMaxTol);
}

//=================================================================================================

void BOPTools_AlgoTools::PointOnEdge(const TopoDS_Edge& aE, const double aParm, gp_Pnt& aPnt)
{
  double      f, l;
  occ::handle<Geom_Curve> C1 = BRep_Tool::Curve(aE, f, l);
  C1->D0(aParm, aPnt);
}

//=================================================================================================

void BOPTools_AlgoTools::CorrectRange(const TopoDS_Edge&    aE1,
                                      const TopoDS_Edge&    aE2,
                                      const IntTools_Range& aSR,
                                      IntTools_Range&       aNewSR)
{
  int  i;
  double     aRes, aTolE1, aTolE2, aTF, aTL, dT;
  BRepAdaptor_Curve aBC;
  GeomAbs_CurveType aCT;
  gp_Pnt            aP;
  gp_Vec            aDer;
  //
  aNewSR = aSR;
  //
  aBC.Initialize(aE1);
  aCT = aBC.GetType();
  if (aCT == GeomAbs_Line)
  {
    return;
  }
  //
  dT  = Precision::PConfusion();
  aTF = aSR.First();
  aTL = aSR.Last();
  //
  aTolE1 = BRep_Tool::Tolerance(aE1);
  aTolE2 = BRep_Tool::Tolerance(aE2);
  //
  for (i = 0; i < 2; ++i)
  {
    aRes = 2. * (aTolE1 + aTolE2);
    //
    if (aCT == GeomAbs_BezierCurve || aCT == GeomAbs_BSplineCurve || aCT == GeomAbs_OffsetCurve
        || aCT == GeomAbs_OtherCurve)
    {

      if (!i)
      {
        aBC.D1(aTF, aP, aDer);
      }
      else
      {
        aBC.D1(aTL, aP, aDer);
      }
      //
      double aMgn = aDer.Magnitude();

      if (aMgn > 1.e-12)
      {
        aRes = aRes / aMgn;
      }
      else
      {
        aRes = aBC.Resolution(aRes);
      }
    } // if (aCT==GeomAbs_BezierCurve||...
    else
    {
      aRes = aBC.Resolution(aRes);
    }
    //
    if (!i)
    {
      aNewSR.SetFirst(aTF + aRes);
    }
    else
    {
      aNewSR.SetLast(aTL - aRes);
    }
    //
    if ((aNewSR.Last() - aNewSR.First()) < dT)
    {
      aNewSR = aSR;
    }
    // aNewSR=((aNewSR.Last()-aNewSR.First()) < dT) ? aSR : aNewSR;
  }
}

//=================================================================================================

void BOPTools_AlgoTools::CorrectRange(const TopoDS_Edge&    aE,
                                      const TopoDS_Face&    aF,
                                      const IntTools_Range& aSR,
                                      IntTools_Range&       aNewSR)
{
  int  i;
  double     aRes, aTolF, aTF, aTL, dT;
  BRepAdaptor_Curve aBC;
  GeomAbs_CurveType aCT;
  gp_Pnt            aP;
  gp_Vec            aDer;
  //
  aNewSR = aSR;
  //
  dT  = Precision::PConfusion();
  aTF = aSR.First();
  aTL = aSR.Last();
  //
  aBC.Initialize(aE);
  aCT = aBC.GetType();
  //
  aTolF = BRep_Tool::Tolerance(aF);
  //
  for (i = 0; i < 2; ++i)
  {
    aRes = aTolF;

    if (aCT == GeomAbs_BezierCurve || aCT == GeomAbs_BSplineCurve || aCT == GeomAbs_OffsetCurve
        || aCT == GeomAbs_OtherCurve)
    {

      if (!i)
      {
        aBC.D1(aTF, aP, aDer);
      }
      else
      {
        aBC.D1(aTL, aP, aDer);
      }
      //
      double aMgn = aDer.Magnitude();

      if (aMgn > 1.e-12)
      {
        aRes = aRes / aMgn;
      }
      else
      {
        aRes = aBC.Resolution(aRes);
      }
    } // if (aCT==GeomAbs_BezierCurve||...
    else
    {
      aRes = aBC.Resolution(aRes);
    }
    //
    if (!i)
    {
      aNewSR.SetFirst(aTF + aRes);
    }
    else
    {
      aNewSR.SetLast(aTL - aRes);
    }
    //
    if ((aNewSR.Last() - aNewSR.First()) < dT)
    {
      aNewSR = aSR;
    }
  }
}

namespace
{

//=======================================================================
// function : dimension
// purpose  : returns dimension of elementary shape
//=======================================================================
static int dimension(const TopoDS_Shape& theS)
{
  switch (theS.ShapeType())
  {
    case TopAbs_VERTEX:
      return 0;
    case TopAbs_EDGE:
    case TopAbs_WIRE:
      return 1;
    case TopAbs_FACE:
    case TopAbs_SHELL:
      return 2;
    case TopAbs_SOLID:
    case TopAbs_COMPSOLID:
      return 3;
    default:
      return -1;
  }
}

} // namespace

//=================================================================================================

void BOPTools_AlgoTools::Dimensions(const TopoDS_Shape& theS,
                                    int&   theDMin,
                                    int&   theDMax)
{
  theDMin = theDMax = dimension(theS);
  if (theDMax >= 0)
    return;

  NCollection_List<TopoDS_Shape> aLS;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>  aMFence;
  TreatCompound(theS, aLS, &aMFence);
  if (aLS.IsEmpty())
  {
    // empty shape
    theDMin = theDMax = -1;
    return;
  }

  theDMin = 3;
  theDMax = 0;
  for (NCollection_List<TopoDS_Shape>::Iterator it(aLS); it.More(); it.Next())
  {
    int aDim = dimension(it.Value());
    if (aDim < theDMin)
      theDMin = aDim;
    if (aDim > theDMax)
      theDMax = aDim;
  }
}

//=================================================================================================

int BOPTools_AlgoTools::Dimension(const TopoDS_Shape& theS)
{
  int aDMin, aDMax;
  Dimensions(theS, aDMin, aDMax);
  return (aDMin == aDMax) ? aDMin : -1;
}

//=================================================================================================

void BOPTools_AlgoTools::TreatCompound(const TopoDS_Shape&   theS,
                                       NCollection_List<TopoDS_Shape>& theLS,
                                       NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>*  theMFence)
{
  TopAbs_ShapeEnum aType = theS.ShapeType();
  if (aType != TopAbs_COMPOUND)
  {
    if (!theMFence || theMFence->Add(theS))
    {
      theLS.Append(theS);
    }
    return;
  }

  for (TopoDS_Iterator it(theS); it.More(); it.Next())
  {
    TreatCompound(it.Value(), theLS, theMFence);
  }
}
