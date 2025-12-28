// Created on: 2000-03-16
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

#include <BOPTest.hxx>
#include <BRep_Builder.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepLib.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Map.hxx>

#include <stdio.h>
//
static void ProcessVertex(const TopoDS_Vertex&,
                          const NCollection_List<TopoDS_Shape>&,
                          const NCollection_List<TopoDS_Shape>&);
static void ProcessEdge(const TopoDS_Edge&, const double);

static void ReduceVertexTolerance(const TopoDS_Shape&);

static void ReduceFaceTolerance(const TopoDS_Shape&);

static void ReduceEdgeTolerance(const TopoDS_Shape&, const double);

static void PreparePCurves(const TopoDS_Shape&, Draw_Interpretor& di);
//
static int breducetolerance(Draw_Interpretor&, int, const char**);
static int btolx(Draw_Interpretor&, int, const char**);
static int bopaddpcs(Draw_Interpretor&, int, const char**);

//=================================================================================================

void BOPTest::TolerCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;

  done = true;
  // Chapter's name
  const char* g = "BOPTest commands";
  //
  theCommands.Add("breducetolerance", "use breducetolerance Shape", __FILE__, breducetolerance, g);
  theCommands.Add("btolx", "use btolx Shape [minTol=1.e-7]", __FILE__, btolx, g);
  theCommands.Add("bopaddpcs", "Use >bopaddpcs Shape", __FILE__, bopaddpcs, g);
}

//=================================================================================================

int btolx(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
  {
    di << " use btolx Shape [minTol=1.e-7]\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get(a[1]);

  if (aS.IsNull())
  {
    di << " Null shape is not allowed\n";
    return 1;
  }
  //
  double aTolEMin = 1.e-7;
  if (n == 3)
  {
    aTolEMin = Draw::Atof(a[2]);
  }
  //
  // Edge Tolerances
  ReduceEdgeTolerance(aS, aTolEMin);
  //
  // Face Tolerances
  ReduceFaceTolerance(aS);
  //
  // Vertex Tolerances
  ReduceVertexTolerance(aS);
  //
  BRepLib::SameParameter(aS, 1.e-7, true);
  //
  DBRep::Set(a[1], aS);
  return 0;
}

//=================================================================================================

void ReduceEdgeTolerance(const TopoDS_Shape& aS, const double aTolTreshold)
{
  int                                                           i, aNbE;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aEMap;
  //
  TopExp::MapShapes(aS, TopAbs_EDGE, aEMap);
  //
  aNbE = aEMap.Extent();
  for (i = 1; i <= aNbE; i++)
  {
    const TopoDS_Edge& aE = TopoDS::Edge(aEMap(i));

    ProcessEdge(aE, aTolTreshold);
  }
}

//=================================================================================================

void ReduceFaceTolerance(const TopoDS_Shape& aS)
{
  int                                                           i, j, aNbF, aNbE;
  double                                                        aTolE, aTolx, aTolEMin;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFMap, aEMap;
  //
  aTolEMin = 1.e-7;
  //
  TopExp::MapShapes(aS, TopAbs_FACE, aFMap);
  aNbF = aFMap.Extent();
  for (i = 1; i <= aNbF; i++)
  {
    aTolx                        = 1.e6;
    const TopoDS_Face&       aF  = TopoDS::Face(aFMap(i));
    occ::handle<BRep_TFace>& aTF = *((occ::handle<BRep_TFace>*)&aF.TShape());
    //
    TopExp::MapShapes(aF, TopAbs_EDGE, aEMap);
    aNbE = aEMap.Extent();
    for (j = 1; j <= aNbE; ++j)
    {
      const TopoDS_Edge& aE = TopoDS::Edge(aEMap(j));
      aTolE                 = BRep_Tool::Tolerance(aE);
      if (aTolE < aTolx)
      {
        aTolx = aTolE;
      }
    }
    aTolE = (aTolx > aTolEMin) ? aTolx : aTolEMin;
    aTF->Tolerance(aTolE);
  }
}

//=================================================================================================

void ReduceVertexTolerance(const TopoDS_Shape& aS)
{
  int i, aNbV;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aVEMap, aVFMap;

  TopExp::MapShapesAndUniqueAncestors(aS, TopAbs_VERTEX, TopAbs_EDGE, aVEMap);
  TopExp::MapShapesAndUniqueAncestors(aS, TopAbs_VERTEX, TopAbs_FACE, aVFMap);

  aNbV = aVEMap.Extent();
  for (i = 1; i <= aNbV; i++)
  {
    const TopoDS_Vertex&                  aV  = TopoDS::Vertex(aVEMap.FindKey(i));
    const NCollection_List<TopoDS_Shape>& aLE = aVEMap(i);
    const NCollection_List<TopoDS_Shape>& aLF = aVFMap.FindFromKey(aV);

    ProcessVertex(aV, aLE, aLF);
  }
}

//=================================================================================================

void ProcessEdge(const TopoDS_Edge& aE, const double aTolTreshold)
{
  int      i, aNb = 23;
  double   aD2, aTolMax2, aT1, aT2, aT, dT;
  gp_Pnt   aPC3D, aP3D;
  gp_Pnt2d aPC2D;

  // NCollection_List<TopoDS_Shape>::Iterator anIt;// Wng in Gcc 3.0
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr;
  //
  occ::handle<Geom_Curve> aC3D = BRep_Tool::Curve(aE, aT1, aT2);
  if (aC3D.IsNull())
  {
    return;
  }
  //
  dT = (aT2 - aT1) / aNb;
  //
  occ::handle<BRep_TEdge>& TE   = *((occ::handle<BRep_TEdge>*)&aE.TShape());
  const TopLoc_Location&   Eloc = aE.Location();
  //
  aTolMax2                                                            = -1.e6;
  const NCollection_List<occ::handle<BRep_CurveRepresentation>>& aLCR = TE->Curves();
  //
  itcr.Initialize(aLCR);
  for (; itcr.More(); itcr.Next())
  {
    const occ::handle<BRep_CurveRepresentation>& cr  = itcr.Value();
    const TopLoc_Location&                       loc = cr->Location();
    TopLoc_Location                              L   = (Eloc * loc); //.Predivided(aV.Location());
    //
    // 3D-Curve
    if (cr->IsCurve3D())
    {
      continue;
    }
    //
    // 2D-Curve
    else if (cr->IsCurveOnSurface())
    {
      const occ::handle<Geom2d_Curve>& aC2D = cr->PCurve();
      if (aC2D.IsNull())
      {
        continue;
      }
      // Surface
      const occ::handle<Geom_Surface>& aS = cr->Surface();
      //
      // 2D-point treatment
      for (i = 0; i <= aNb; ++i)
      {
        aT = aT1 + i * dT;
        if (i == aNb)
        {
          aT = aT2;
        }
        aPC3D = aC3D->Value(aT);
        aPC2D = aC2D->Value(aT);
        aS->D0(aPC2D.X(), aPC2D.Y(), aP3D);
        aP3D.Transform(L.Transformation());
        aD2 = aPC3D.SquareDistance(aP3D);
        if (aD2 > aTolMax2)
        {
          aTolMax2 = aD2;
        }
      }
    } // if (cr->IsCurveOnSurface())
  } // for (; itcr.More(); itcr.Next())

  // #########################################################
  //
  if (aTolMax2 < 0.)
  {
    return;
  }
  //
  //
  aTolMax2 = sqrt(aTolMax2);

  // printf(" aTolMax=%15.10lf, aTolWas=%15.10lf\n", aTolMax2, aTolE);

  double aTolSet;
  aTolSet = (aTolMax2 > aTolTreshold) ? aTolMax2 : aTolTreshold;

  TE->Tolerance(aTolSet);
}

//=================================================================================================

void ProcessVertex(const TopoDS_Vertex&                  aV,
                   const NCollection_List<TopoDS_Shape>& aLE,
                   const NCollection_List<TopoDS_Shape>& aLF)
{
  double             aTol, aD2, aTolMax2, aTolE, aParam;
  gp_Pnt             aPC3D;
  gp_Pnt2d           aPC2D;
  TopAbs_Orientation anOrV;

  NCollection_List<TopoDS_Shape>::Iterator anIt;
  TopExp_Explorer                          aVExp;

  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr;
  //
  aTolMax2 = -1.e6;
  //
  occ::handle<BRep_TVertex>& TV    = *((occ::handle<BRep_TVertex>*)&aV.TShape());
  const gp_Pnt&              aPV3D = TV->Pnt();
  aTol                             = BRep_Tool::Tolerance(aV);
  //
  anIt.Initialize(aLE);
  for (; anIt.More(); anIt.Next())
  {
    const TopoDS_Edge& aE = TopoDS::Edge(anIt.Value());
    //
    occ::handle<BRep_TEdge>& TE   = *((occ::handle<BRep_TEdge>*)&aE.TShape());
    const TopLoc_Location&   Eloc = aE.Location();
    //
    aVExp.Init(aE, TopAbs_VERTEX);
    for (; aVExp.More(); aVExp.Next())
    {
      const TopoDS_Vertex& aVx = TopoDS::Vertex(aVExp.Current());
      //
      if (!aVx.IsSame(aV))
      {
        continue;
      }
      //
      anOrV = aVx.Orientation();
      if (!(anOrV == TopAbs_FORWARD || anOrV == TopAbs_REVERSED))
      {
        continue;
      }
      //
      const NCollection_List<occ::handle<BRep_CurveRepresentation>>& aLCR = TE->Curves();
      itcr.Initialize(aLCR);
      for (; itcr.More(); itcr.Next())
      {
        const occ::handle<BRep_CurveRepresentation>& cr  = itcr.Value();
        const TopLoc_Location&                       loc = cr->Location();
        TopLoc_Location                              L   = (Eloc * loc).Predivided(aV.Location());
        //
        // 3D-Curve
        if (cr->IsCurve3D())
        {
          const occ::handle<Geom_Curve>& aC3D = cr->Curve3D();
          //
          if (aC3D.IsNull())
          {
            continue;
          }
          // 3D-point treatment
          aParam = BRep_Tool::Parameter(aVx, aE);
          aPC3D  = aC3D->Value(aParam);
          aPC3D.Transform(L.Transformation());
          aD2 = aPV3D.SquareDistance(aPC3D);
          if (aD2 > aTolMax2)
          {
            aTolMax2 = aD2;
          }
          //
        } // if (cr->IsCurve3D())
        //
        // 2D-Curve
        else if (cr->IsCurveOnSurface())
        {
          const occ::handle<Geom2d_Curve>& aC2D = cr->PCurve();
          if (aC2D.IsNull())
          {
            continue;
          }
          // Surface
          const occ::handle<Geom_Surface>& aS = cr->Surface();
          //
          // 2D-point treatment
          aParam = BRep_Tool::Parameter(aVx, aE, aS, L);
          aPC2D  = aC2D->Value(aParam);
          aS->D0(aPC2D.X(), aPC2D.Y(), aPC3D);
          aPC3D.Transform(L.Transformation());
          aD2 = aPV3D.SquareDistance(aPC3D);
          if (aD2 > aTolMax2)
          {
            aTolMax2 = aD2;
          }
        } // if (cr->IsCurveOnSurface())

      } // for (; itcr.More(); itcr.Next())
    } // for (; aVExp.More(); aVExp.Next())
  } // for (; anIt.More(); anIt.Next())
  // #########################################################
  //
  //  Reducing
  if (aTolMax2 < 0.)
  {
    return;
  }
  //
  aTolMax2 = sqrt(aTolMax2);
  if (aTolMax2 > aTol)
  {
    return;
  }
  //
  anIt.Initialize(aLE);
  for (; anIt.More(); anIt.Next())
  {
    const TopoDS_Edge& aE = TopoDS::Edge(anIt.Value());

    aTolE = BRep_Tool::Tolerance(aE);
    if (aTolMax2 < aTolE)
    {
      aTolMax2 = aTolE;
    }
  }
  //
  anIt.Initialize(aLF);
  for (; anIt.More(); anIt.Next())
  {
    const TopoDS_Face& aF = TopoDS::Face(anIt.Value());

    aTolE = BRep_Tool::Tolerance(aF);
    if (aTolMax2 < aTolE)
    {
      aTolMax2 = aTolE;
    }
  }
  //
  if (aTolMax2 > aTol)
  {
    return;
  }
  //
  // Update Tolerance
  TV->Tolerance(aTolMax2);
}

//=================================================================================================

int breducetolerance(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
  {
    di << " use bupdatetolerance Shape\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get(a[1]);

  if (aS.IsNull())
  {
    di << " Null shape is not allowed \n";
    return 1;
  }
  ReduceVertexTolerance(aS);
  DBRep::Set(a[1], aS);

  return 0;
}

//
//=======================================================================
// function : bopaddpcs
// purpose  : Some Edges do not contain P-Curves on Faces to which
//           they belong to.
//           These faces usually based on Geom_Plane surface.
//           To prevent sophisticated treatment the Command "bopaddpcs:
//           adds P-Curves for the edges .
//=======================================================================
int bopaddpcs(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
  {
    di << " Use >bopaddpcs Shape\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get(a[1]);

  if (aS.IsNull())
  {
    di << " Null shape is not allowed \n";
    return 1;
  }
  //
  PreparePCurves(aS, di);
  //
  DBRep::Set(a[1], aS);
  return 0;
}

//=================================================================================================

void PreparePCurves(const TopoDS_Shape& aShape, Draw_Interpretor& di)
{
  int    i, aNbE;
  double aTolE, aT1, aT2;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                            aEFMap;
  TopLoc_Location           aLoc;
  occ::handle<Geom_Curve>   aC3D;
  occ::handle<Geom2d_Curve> aC2D;
  BRep_Builder              aBB;
  //
  TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, aEFMap);
  //
  aNbE = aEFMap.Extent();
  for (i = 1; i <= aNbE; ++i)
  {
    const TopoDS_Edge& aE = TopoDS::Edge(aEFMap.FindKey(i));
    //
    if (BRep_Tool::Degenerated(aE))
    {
      continue;
    }
    //
    aC3D = BRep_Tool::Curve(aE, aT1, aT2);
    if (aC3D.IsNull())
    {
      continue;
    }
    aTolE = BRep_Tool::Tolerance(aE);
    //
    const NCollection_List<TopoDS_Shape>&    aLF = aEFMap(i);
    NCollection_List<TopoDS_Shape>::Iterator aFIt(aLF);
    for (; aFIt.More(); aFIt.Next())
    {
      const TopoDS_Face& aF = TopoDS::Face(aFIt.Value());
      //
      // Map of surfaces on which the edge lays .
      NCollection_IndexedMap<occ::handle<Standard_Transient>> aSCRMap;
      occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&aE.TShape());
      const NCollection_List<occ::handle<BRep_CurveRepresentation>>&    aLCR = TE->Curves();
      NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr;
      itcr.Initialize(aLCR);
      for (; itcr.More(); itcr.Next())
      {
        const occ::handle<BRep_CurveRepresentation>& cr = itcr.Value();
        //
        if (cr->IsCurveOnSurface())
        {
          const occ::handle<Geom_Surface>& aSCR = cr->Surface();
          aSCRMap.Add(aSCR);
        }
        //
      }
      //
      const occ::handle<Geom_Surface>& aS = BRep_Tool::Surface(aF, aLoc);
      if (!aSCRMap.Contains(aS))
      {
        // try to obtain 2D curve
        aC2D = BRep_Tool::CurveOnSurface(aE, aS, aLoc, aT1, aT2);
        if (aC2D.IsNull())
        {
          di << " Warning: Can not obtain P-Curve\n";
          continue;
        }
        else
        {
          aBB.UpdateEdge(aE, aC2D, aF, aTolE);
        }
      }
    }
  }
}
