// Created on: 1999-11-02
// Created by: Peter KURNEV
// Copyright (c) 1999-1999 Matra Datavision
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

#include <Adaptor2d_Curve2d.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_CorrectFace2d.hxx>
#include <TopOpeBRepBuild_Tools.hxx>
#include <TopAbs_State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopOpeBRepDS_ShapeWithState.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_ShapeClassifier.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>

#include <cstdio>
// define parameter division number as 10*e^(-PI) = 0.43213918
const double PAR_T = 0.43213918;

//=======================================================================
// function TopOpeBRepBuild_Tools::FindState
// purpose  :
//=======================================================================
void TopOpeBRepBuild_Tools::FindState(
  const TopoDS_Shape&                                                       aSubsh,
  const TopAbs_State                                                        aState,
  const TopAbs_ShapeEnum                                                    aSubshEnum,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>&                aMapSubshAnc,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                   aMapProcessedSubsh,
  NCollection_DataMap<TopoDS_Shape, TopAbs_State, TopTools_ShapeMapHasher>& aMapSS)
{
  int                                      i, nSub;
  const NCollection_List<TopoDS_Shape>&    aListOfShapes = aMapSubshAnc.FindFromKey(aSubsh);
  NCollection_List<TopoDS_Shape>::Iterator anIt(aListOfShapes);
  for (; anIt.More(); anIt.Next())
  {
    const TopoDS_Shape&                                           aS = anIt.Value();
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSubshMap;
    TopExp::MapShapes(aS, aSubshEnum, aSubshMap);
    nSub = aSubshMap.Extent();
    for (i = 1; i <= nSub; i++)
    {
      const TopoDS_Shape& aSS = aSubshMap(i);
      if (aMapProcessedSubsh.Add(aSS))
      {
        aMapSS.Bind(aSS, aState);
        FindState(aSS, aState, aSubshEnum, aMapSubshAnc, aMapProcessedSubsh, aMapSS);
      }
    }
  }
}

//=======================================================================
// function TopOpeBRepBuild_Tools::PropagateState
// purpose  :
//=======================================================================
void TopOpeBRepBuild_Tools::PropagateState(
  const NCollection_DataMap<TopoDS_Shape, TopAbs_State, TopTools_ShapeMapHasher>& aSplShapesState,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&            aShapesToRestMap,
  const TopAbs_ShapeEnum          aSubshEnum, // Vertex
  const TopAbs_ShapeEnum          aShapeEnum, // Edge
  TopOpeBRepTool_ShapeClassifier& aShapeClassifier,
  NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeWithState, TopTools_ShapeMapHasher>&
                                                                aMapOfShapeWithState,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& anAvoidSubshMap)
{
  int                                                                      j, nSub, nRest;
  NCollection_DataMap<TopoDS_Shape, TopAbs_State, TopTools_ShapeMapHasher> aMapSS, aMapSS1;

  NCollection_DataMap<TopoDS_Shape, TopAbs_State, TopTools_ShapeMapHasher>::Iterator anItSS(
    aSplShapesState);
  for (; anItSS.More(); anItSS.Next())
  {
    const TopoDS_Shape&                                           aShape = anItSS.Key();
    TopAbs_State                                                  aState = anItSS.Value();
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSubshapes;
    TopExp::MapShapes(aShape, aSubshEnum, aSubshapes);
    nSub = aSubshapes.Extent();
    for (j = 1; j <= nSub; j++)
      if (!anAvoidSubshMap.Contains(aSubshapes(j))) // MSV: enforce subshapes avoidance
        aMapSS.Bind(aSubshapes(j), aState);
  }

  aMapSS1 = aMapSS;

  // 1. Build the Map of ShapesAndAncestors for ShapesToRest
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aMapSubshAnc;
  nRest = aShapesToRestMap.Extent();
  for (j = 1; j <= nRest; j++)
    TopExp::MapShapesAndAncestors(aShapesToRestMap(j), aSubshEnum, aShapeEnum, aMapSubshAnc);

  // 2. Make Map Of all subshapes  aMapSS
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aProcessedSubshapes;
  anItSS.Initialize(aMapSS1);
  for (; anItSS.More(); anItSS.Next())
  {
    const TopoDS_Shape& aSubsh = anItSS.Key();
    TopAbs_State        aState = anItSS.Value();
    if (aMapSubshAnc.Contains(aSubsh))
    {
      aProcessedSubshapes.Add(aSubsh);
      FindState(aSubsh, aState, aSubshEnum, aMapSubshAnc, aProcessedSubshapes, aMapSS);
    }
  }

  // 3. Propagate the states on ShapesToRestMap
  TopoDS_Shape                                           aNullShape;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aNonPassedShapes;
  nRest = aShapesToRestMap.Extent();
  for (j = 1; j <= nRest; j++)
  {
    const TopoDS_Shape&                                           aS = aShapesToRestMap.FindKey(j);
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSubshMap;
    TopExp::MapShapes(aS, aSubshEnum, aSubshMap);
    const TopoDS_Shape& aSubsh = aSubshMap(1);
    if (aMapSS.IsBound(aSubsh))
    {
      TopAbs_State aState = aMapSS.Find(aSubsh);

      if (aState == TopAbs_ON)
      {
        aState = aShapeClassifier.StateShapeReference(aS, aNullShape);
      }
      // Add the Rest Shape to aMapOfShapeWithState
      TopOpeBRepDS_ShapeWithState aShapeWithState;
      aShapeWithState.SetState(aState);
      aShapeWithState.SetIsSplitted(false);
      aMapOfShapeWithState.Add(aS, aShapeWithState);
    }

    else
    {
      aNonPassedShapes.Add(aS);
    }
  }

  // 4. Define the states for aNonPassedShapes
  //   (for faces themselves and for theirs Wires, Edges):
  if (aNonPassedShapes.Extent())
  {
    // Build the Map of ShapesAndAncestors for aNonPassedShapes
    aMapSubshAnc.Clear();
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aMapIt;
    aMapIt.Initialize(aNonPassedShapes);
    for (; aMapIt.More(); aMapIt.Next())
      TopExp::MapShapesAndAncestors(aMapIt.Key(), aSubshEnum, aShapeEnum, aMapSubshAnc);

    aMapSS.Clear();
    aMapIt.Initialize(aNonPassedShapes);
    for (; aMapIt.More(); aMapIt.Next())
    {
      // Face
      const TopoDS_Shape& aNonPassedShape = aMapIt.Key();

      if (!aMapSS.IsBound(aNonPassedShape))
      {
        TopAbs_State aState = FindStateThroughVertex(aNonPassedShape,
                                                     aShapeClassifier,
                                                     aMapOfShapeWithState,
                                                     anAvoidSubshMap);
        aMapSS.Bind(aNonPassedShape, aState);

        // First Subshape
        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aTmpMap;
        TopExp::MapShapes(aNonPassedShape, aSubshEnum, aTmpMap);
        TopoDS_Shape aFirstSubsh;
        for (j = 1; j <= aTmpMap.Extent() && aFirstSubsh.IsNull(); j++)
          if (!anAvoidSubshMap.Contains(aTmpMap(j)))
            aFirstSubsh = aTmpMap(j);
        if (aFirstSubsh.IsNull())
          continue;
        aMapSS.Bind(aFirstSubsh, aState);

        // Propagation of aState for subshapes
        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapProcessedSubsh;
        if (aSubshEnum == TopAbs_EDGE)
          FindState1(aFirstSubsh, aState, aMapSubshAnc, aMapProcessedSubsh, aMapSS);
        else // if (aSubshEnum==TopAbs_VERTEX)
          FindState2(aFirstSubsh, aState, aMapSubshAnc, aMapProcessedSubsh, aMapSS);
      }
    }

    // Fill aShapeWithState
    TopOpeBRepDS_ShapeWithState aShapeWithState;
    aShapeWithState.SetIsSplitted(false);
    NCollection_DataMap<TopoDS_Shape, TopAbs_State, TopTools_ShapeMapHasher>::Iterator anII(aMapSS);
    for (; anII.More(); anII.Next())
    {
      aShapeWithState.SetState(anII.Value());
      if (anII.Key().ShapeType() != TopAbs_VERTEX)
        aMapOfShapeWithState.Add(anII.Key(), aShapeWithState);
    }
  }
}

//=================================================================================================

void TopOpeBRepBuild_Tools::FindState2(
  const TopoDS_Shape&                                                       aSubsh,
  const TopAbs_State                                                        aState,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>&                aMapSubshAnc,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                   aMapProcessedSubsh,
  NCollection_DataMap<TopoDS_Shape, TopAbs_State, TopTools_ShapeMapHasher>& aMapSS)
{
  int                                      i, nSub;
  const NCollection_List<TopoDS_Shape>&    aListOfShapes = aMapSubshAnc.FindFromKey(aSubsh);
  NCollection_List<TopoDS_Shape>::Iterator anIt(aListOfShapes);
  for (; anIt.More(); anIt.Next())
  {
    // Shape
    const TopoDS_Shape& aShape = anIt.Value();
    aMapSS.Bind(aShape, aState);

    // Subshape
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSubshMap;
    TopExp::MapShapes(aShape, TopAbs_VERTEX, aSubshMap);
    nSub = aSubshMap.Extent();
    for (i = 1; i <= nSub; i++)
    {
      const TopoDS_Shape& aSS = aSubshMap(i);
      if (aMapProcessedSubsh.Add(aSS))
      {
        aMapSS.Bind(aSS, aState);
        FindState2(aSS, aState, aMapSubshAnc, aMapProcessedSubsh, aMapSS);
      }
    }
  }
}

//=================================================================================================

void TopOpeBRepBuild_Tools::FindState1(
  const TopoDS_Shape&                                                       aSubsh,
  const TopAbs_State                                                        aState,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>&                aMapSubshAnc,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                   aMapProcessedSubsh,
  NCollection_DataMap<TopoDS_Shape, TopAbs_State, TopTools_ShapeMapHasher>& aMapSS)
{
  int                                      i, nSub, j, nW;
  const NCollection_List<TopoDS_Shape>&    aListOfShapes = aMapSubshAnc.FindFromKey(aSubsh);
  NCollection_List<TopoDS_Shape>::Iterator anIt(aListOfShapes);
  for (; anIt.More(); anIt.Next())
  {
    // Face
    const TopoDS_Shape& aShape = anIt.Value();
    aMapSS.Bind(aShape, aState);
    // Wire
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aWireMap;
    TopExp::MapShapes(aShape, TopAbs_WIRE, aWireMap);
    nW = aWireMap.Extent();
    for (j = 1; j <= nW; j++)
      aMapSS.Bind(aWireMap(j), aState);
    // Edge
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSubshMap;
    TopExp::MapShapes(aShape, TopAbs_EDGE, aSubshMap);
    nSub = aSubshMap.Extent();
    for (i = 1; i <= nSub; i++)
    {
      const TopoDS_Shape& aSS = aSubshMap(i);
      if (aMapProcessedSubsh.Add(aSS))
      {
        aMapSS.Bind(aSS, aState);
        FindState1(aSS, aState, aMapSubshAnc, aMapProcessedSubsh, aMapSS);
      }
    }
  }
}

//=================================================================================================

TopAbs_State TopOpeBRepBuild_Tools::FindStateThroughVertex(
  const TopoDS_Shape&             aShape,
  TopOpeBRepTool_ShapeClassifier& aShapeClassifier,
  NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeWithState, TopTools_ShapeMapHasher>&
                                                                aMapOfShapeWithState,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& anAvoidSubshMap)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSubshMap;
  TopExp::MapShapes(aShape, TopAbs_VERTEX, aSubshMap);

  TopoDS_Shape aSubsh;
  int          i;
  for (i = 1; i <= aSubshMap.Extent() && aSubsh.IsNull(); i++)
    if (!anAvoidSubshMap.Contains(aSubshMap(i)))
      aSubsh = aSubshMap(i);
  if (aSubsh.IsNull())
  {
    // try an edge
    aSubshMap.Clear();
    TopExp::MapShapes(aShape, TopAbs_EDGE, aSubshMap);
    for (i = 1; i <= aSubshMap.Extent() && aSubsh.IsNull(); i++)
      if (!anAvoidSubshMap.Contains(aSubshMap(i)))
        aSubsh = aSubshMap(i);
    if (aSubsh.IsNull())
    {
#ifdef OCCT_DEBUG
      std::cout << "FindStateThroughVertex: warning: all vertices are avoided" << std::endl;
#endif
      return TopAbs_UNKNOWN; // failure
    }
  }

  TopoDS_Shape                aNullShape;
  TopAbs_State                aState = aShapeClassifier.StateShapeReference(aSubsh, aNullShape);
  TopOpeBRepDS_ShapeWithState aShapeWithState;
  aShapeWithState.SetState(aState);
  aShapeWithState.SetIsSplitted(false);
  aMapOfShapeWithState.Add(aShape, aShapeWithState);
  SpreadStateToChild(aShape, aState, aMapOfShapeWithState);
  return aState;
}

//=================================================================================================

void TopOpeBRepBuild_Tools::SpreadStateToChild(
  const TopoDS_Shape& aShape,
  const TopAbs_State  aState,
  NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeWithState, TopTools_ShapeMapHasher>&
    aMapOfShapeWithState)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aChildMap;
  TopExp::MapShapes(aShape, TopAbs_FACE, aChildMap);
  TopExp::MapShapes(aShape, TopAbs_WIRE, aChildMap);
  TopExp::MapShapes(aShape, TopAbs_EDGE, aChildMap);

  TopOpeBRepDS_ShapeWithState aShapeWithState;
  aShapeWithState.SetState(aState);
  aShapeWithState.SetIsSplitted(false);

  int i, n = aChildMap.Extent();
  for (i = 1; i <= n; i++)
  {
    aMapOfShapeWithState.Add(aChildMap(i), aShapeWithState);
  }
}

//=================================================================================================

void TopOpeBRepBuild_Tools::PropagateStateForWires(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aFacesToRestMap,
  NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeWithState, TopTools_ShapeMapHasher>&
    aMapOfShapeWithState)
{
  int i, j, nF, nW, k, nE;

  nF = aFacesToRestMap.Extent();
  for (i = 1; i <= nF; i++)
  {
    const TopoDS_Shape& aF = aFacesToRestMap(i);
    if (aMapOfShapeWithState.Contains(aF))
    {
      const TopOpeBRepDS_ShapeWithState& aSWS = aMapOfShapeWithState.FindFromKey(aF);
      TopAbs_State                       aSt  = aSWS.State();

      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aWireMap;
      TopExp::MapShapes(aF, TopAbs_WIRE, aWireMap);
      nW = aWireMap.Extent();
      for (j = 1; j <= nW; j++)
      {
        const TopoDS_Shape&         aW = aWireMap(j);
        TopOpeBRepDS_ShapeWithState aWireSWS;
        aWireSWS.SetState(aSt);
        aWireSWS.SetIsSplitted(false);
        aMapOfShapeWithState.Add(aW, aWireSWS);

        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aEdgeMap;
        TopExp::MapShapes(aW, TopAbs_EDGE, aEdgeMap);
        nE = aEdgeMap.Extent();
        for (k = 1; k <= nE; k++)
        {
          const TopoDS_Shape& aE = aEdgeMap(k);
          if (!aMapOfShapeWithState.Contains(aE))
          {
            TopOpeBRepDS_ShapeWithState anEdgeSWS;
            anEdgeSWS.SetState(aSt);
            anEdgeSWS.SetIsSplitted(false);
            aMapOfShapeWithState.Add(aE, anEdgeSWS);
          }
        }
      }
    }
  }
}

//=================================================================================================

void TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(const TopoDS_Face& aFObj,
                                                  const TopoDS_Edge& anEdgeObj,
                                                  gp_Vec&            aNormal)
{
  const TopoDS_Edge&        aEd = anEdgeObj;
  const TopoDS_Face&        aFS = aFObj;
  double                    f2 = 0., l2 = 0., tolpc = 0., f = 0., l = 0., par = 0.;
  occ::handle<Geom2d_Curve> C2D = FC2D_CurveOnSurface(aEd, aFS, f2, l2, tolpc, true);

  BRepAdaptor_Curve aCA(aEd);
  f   = aCA.FirstParameter();
  l   = aCA.LastParameter();
  par = f * PAR_T + (1 - PAR_T) * l;

  gp_Pnt2d aUV1;
  C2D->D0(par, aUV1);

  gp_Pnt              aP;
  gp_Vec              aTg1, aTg2;
  BRepAdaptor_Surface aSA1(aFS);
  aSA1.D1(aUV1.X(), aUV1.Y(), aP, aTg1, aTg2);
  aNormal = aTg1 ^ aTg2;
}

//=================================================================================================

void TopOpeBRepBuild_Tools::GetNormalInNearestPoint(const TopoDS_Face& F,
                                                    const TopoDS_Edge& E,
                                                    gp_Vec&            aNormal)
{
  double f2 = 0., l2 = 0., tolpc = 0., par = 0.;

  gp_Vec2d aTangent;

  occ::handle<Geom2d_Curve> C2D = FC2D_CurveOnSurface(E, F, f2, l2, tolpc, true);

  par = f2 * PAR_T + (1 - PAR_T) * l2;

  gp_Pnt2d aP;
  C2D->D1(par, aP, aTangent);

  double Xnorm = -aTangent.Y();
  double Ynorm = aTangent.X();

  double step = TopOpeBRepTool_TOOL::minDUV(F);
  step *= 1e-2;

  gp_Vec2d aPV(aP.X(), aP.Y());
  gp_Dir2d aStepV(Xnorm, Ynorm);
  gp_Vec2d aNorm2d = aPV + gp_Vec2d(step * aStepV);

  double newU = aNorm2d.X();
  double newV = aNorm2d.Y();
  gp_Vec aTg1, aTg2;
  gp_Pnt aP1;

  BRepAdaptor_Surface BS(F);
  BS.D1(newU, newV, aP1, aTg1, aTg2);

  gp_Pnt2d                aP2d(newU, newV);
  BRepTopAdaptor_FClass2d FC(F, Precision::PConfusion());
  TopAbs_State            aState = FC.Perform(aP2d);

  // point out of face: try to go at another direction
  if (aState == TopAbs_OUT)
  {
    aStepV.Reverse();
    aNorm2d = aPV + gp_Vec2d(step * aStepV);

    newU = aNorm2d.X();
    newV = aNorm2d.Y();

    BS.D1(newU, newV, aP1, aTg1, aTg2);

    // in principle, we must check again
    //     aP2d.SetX(newU); aP2d.SetY(newV);
    //     BRepClass_FaceClassifier FC(Fex, aP2d, 1e-7);
    //     TopAbs_State aState = FC.State();
  }

  aNormal = aTg1 ^ aTg2;
}

//=================================================================================================

bool TopOpeBRepBuild_Tools::GetTangentToEdgeEdge(const TopoDS_Face&, // aFObj,
                                                 const TopoDS_Edge& anEdgeObj,
                                                 const TopoDS_Edge& aOriEObj,
                                                 gp_Vec&            aTangent)
{

  if (BRep_Tool::Degenerated(aOriEObj) || BRep_Tool::Degenerated(anEdgeObj))
  {
    return TopOpeBRepBuild_Tools::GetTangentToEdge(anEdgeObj, aTangent);
  }

  TopoDS_Edge aEd = anEdgeObj, aEOri = aOriEObj;

  double f = 0., l = 0., par = 0., parOri = 0.;

  BRepAdaptor_Curve aCA(aEd);
  BRepAdaptor_Curve aCAOri(aEOri);

  f = aCA.FirstParameter();
  l = aCA.LastParameter();

  par = f * PAR_T + (1 - PAR_T) * l;

  gp_Pnt aP;
  gp_Vec aTgPiece;
  aCA.D1(par, aP, aTgPiece);
  aTangent = aTgPiece;

  gp_Pnt aPOri;
  gp_Vec aTgOri;
  /////
  occ::handle<Geom_Curve> GCOri      = aCAOri.Curve().Curve();
  occ::handle<Geom_Curve> aCopyCurve = occ::down_cast<Geom_Curve>(GCOri->Copy());

  const TopLoc_Location& aLoc  = aEOri.Location();
  gp_Trsf                aTrsf = aLoc.Transformation();
  aCopyCurve->Transform(aTrsf);

  GeomAPI_ProjectPointOnCurve aPP(aP,
                                  aCopyCurve,
                                  aCopyCurve->FirstParameter(),
                                  aCopyCurve->LastParameter());
#ifdef OCCT_DEBUG
//  gp_Pnt aNP = aPP.NearestPoint();
#endif
  parOri = aPP.LowerDistanceParameter();

  aCopyCurve->D1(parOri, aPOri, aTgOri); // aPOri must be equal aNP !
  // printf(" aNP  ={%lf, %lf, %lf}\n", aNP.X(), aNP.Y(), aNP.Z());
  // printf(" aPOri={%lf, %lf, %lf}\n", aPOri.X(), aPOri.Y(), aPOri.Z());
  if (aEd.Orientation() == TopAbs_REVERSED)
    aTangent.Reverse();

  if (aTgOri * aTgPiece < 0.)
  {
    aTangent.Reverse();
    return true;
  }
  return false;
}

//=================================================================================================

bool TopOpeBRepBuild_Tools::GetTangentToEdge(const TopoDS_Edge& anEdgeObj, gp_Vec& aTangent)
{
  const TopoDS_Edge& aEd = anEdgeObj;

  double f = 0., l = 0., par = 0.;

  BRepAdaptor_Curve aCA(aEd);

  f = aCA.FirstParameter();
  l = aCA.LastParameter();

  par = f * PAR_T + (1 - PAR_T) * l;
  gp_Pnt aP;
  aCA.D1(par, aP, aTangent);

  return true;
}

//=================================================================================================

bool TopOpeBRepBuild_Tools::GetAdjacentFace(
  const TopoDS_Shape&                                        aFaceObj,
  const TopoDS_Shape&                                        anEObj,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>& anEdgeFaceMap,
  TopoDS_Shape&                                              anAdjFaceObj)
{
  const NCollection_List<TopoDS_Shape>&    aListOfAdjFaces = anEdgeFaceMap.FindFromKey(anEObj);
  NCollection_List<TopoDS_Shape>::Iterator anIt(aListOfAdjFaces);
  TopoDS_Shape                             anAdjShape;
  for (; anIt.More(); anIt.Next())
  {
    if (anIt.Value() != aFaceObj)
    {
      anAdjShape = anIt.Value();
      break;
    }
  }

  if (!anAdjShape.IsNull())
  {
    anAdjFaceObj = TopoDS::Face(anAdjShape);
    return true;
  }
  else
  {
    return false;
  }
}

//=================================================================================================

void TopOpeBRepBuild_Tools::UpdatePCurves(const TopoDS_Wire& aWire,
                                          const TopoDS_Face& fromFace,
                                          const TopoDS_Face& toFace)
{
  TopExp_Explorer aExp(aWire, TopAbs_EDGE);

  for (; aExp.More(); aExp.Next())
  {
    TopoDS_Shape aEdge = aExp.Current();
    UpdateEdgeOnFace(TopoDS::Edge(aEdge), fromFace, toFace);
  }
}

//=================================================================================================

void TopOpeBRepBuild_Tools::UpdateEdgeOnFace(const TopoDS_Edge& aEdgeToUpdate,
                                             const TopoDS_Face& fromFace,
                                             const TopoDS_Face& toFace)
{
  BRep_Builder BB;

  double                    tolE = BRep_Tool::Tolerance(TopoDS::Edge(aEdgeToUpdate));
  double                    f2 = 0., l2 = 0., tolpc = 0.;
  occ::handle<Geom2d_Curve> C2D;

  if (BRep_Tool::Degenerated(aEdgeToUpdate))
  {
    // we can not compute PCurve for Degenerated Edge
    // so we take as it was on old face and after (in CorrectFace2D)
    //  we will adjust this PCurve
    C2D = FC2D_CurveOnSurface(aEdgeToUpdate, fromFace, f2, l2, tolpc, true);
    double                           tol    = std::max(tolE, tolpc);
    occ::handle<Geom2d_Curve>        C2Dn   = occ::down_cast<Geom2d_Curve>(C2D->Copy());
    occ::handle<Geom2d_TrimmedCurve> newC2D = new Geom2d_TrimmedCurve(C2Dn, f2, l2);
    BB.UpdateEdge(aEdgeToUpdate, newC2D, toFace, tol);
  }
  else
  { // not degenerated edge

    if (BRep_Tool::IsClosed(aEdgeToUpdate, fromFace))
    {
      UpdateEdgeOnPeriodicalFace(aEdgeToUpdate, fromFace, toFace);
    }
    else
    {
      C2D        = FC2D_CurveOnSurface(aEdgeToUpdate, toFace, f2, l2, tolpc, true);
      double tol = std::max(tolE, tolpc);
      BB.UpdateEdge(aEdgeToUpdate, C2D, toFace, tol);
    }
  }
}

//=================================================================================================

void TopOpeBRepBuild_Tools::UpdateEdgeOnPeriodicalFace(const TopoDS_Edge& aEdgeToUpdate,
                                                       const TopoDS_Face& fromFace,
                                                       const TopoDS_Face& toFace)
{
  bool               DiffOriented = false;
  BRep_Builder       BB;
  TopoDS_Edge        newE  = aEdgeToUpdate; // newE.Orientation(TopAbs_FORWARD);
  const TopoDS_Face& fFace = fromFace;      // fFace.Orientation(TopAbs_FORWARD);
  const TopoDS_Face& tFace = toFace;        // tFace.Orientation(TopAbs_FORWARD);
  double             fc = 0., lc = 0.;

  occ::handle<Geom2d_Curve> cc = BRep_Tool::CurveOnSurface(newE, tFace, fc, lc);

  if (!cc.IsNull())
  {
    // std::cout << "Pcurves exist" << std::endl;
    return;
  }

  gp_Vec aN1, aN2;
  TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(TopoDS::Face(fromFace),
                                               TopoDS::Edge(aEdgeToUpdate),
                                               aN1);

  TopOpeBRepBuild_Tools::GetNormalToFaceOnEdge(TopoDS::Face(toFace),
                                               TopoDS::Edge(aEdgeToUpdate),
                                               aN2);

  if (aN1 * aN2 < 0)
    DiffOriented = true;

  double tolE = BRep_Tool::Tolerance(newE);
  double f2 = 0., l2 = 0., tolpc = 0., tol = 0.;

  // first  PCurve
  occ::handle<Geom2d_Curve> C2D = FC2D_CurveOnSurface(newE, tFace, f2, l2, tolpc, true);

  tol = std::max(tolpc, tolE);

  BRepAdaptor_Surface aBAS(fFace);
  gp_Vec2d            aTrV;

  double   ff = 0., lf = 0., fr = 0., lr = 0.;
  gp_Pnt2d aUVf, aUVr;

  occ::handle<Geom2d_Curve> oldC2DFor = BRep_Tool::CurveOnSurface(newE, // FC2D_CurveOnSurface(newE,
                                                                  fFace,
                                                                  ff,
                                                                  lf); //, tolpc, true);
  newE.Reverse();
  occ::handle<Geom2d_Curve> oldC2DRev = BRep_Tool::CurveOnSurface(newE, // FC2D_CurveOnSurface(newE,
                                                                  fFace,
                                                                  fr,
                                                                  lr); //, tolpc, true);

  oldC2DFor->D0(ff, aUVf);
  oldC2DRev->D0(fr, aUVr);

  if (!DiffOriented)
    aTrV = gp_Vec2d(aUVf, aUVr);
  else
    aTrV = gp_Vec2d(aUVr, aUVf);

  gp_Vec2d aux(gp_Pnt2d(0., 0.), gp_Pnt2d(1., 1.));
  double   scalar = aux * aTrV;
  bool     dir    = scalar >= 0.;

  // compute right order of pcurves
  gp_Vec2d aYVec(gp_Pnt2d(0., 0.), gp_Pnt2d(0., 1.));
  gp_Pnt2d aUVfv, aUVlv;
  C2D->D0(f2, aUVfv);
  C2D->D0(l2, aUVlv);
  gp_Vec2d C2DVec(aUVfv, aUVlv);

  bool firstOrder = true;
  scalar          = aYVec * C2DVec;
  if (fabs(scalar) <= 1e-10)
  { // compute along X axe
    gp_Vec2d aXVec(gp_Pnt2d(0., 0.), gp_Pnt2d(1., 0.));
    scalar     = aXVec * C2DVec;
    firstOrder = scalar >= 0.;
  }
  else
    firstOrder = scalar <= 0.;

  occ::handle<Geom2d_Curve> aTrC = occ::down_cast<Geom2d_Curve>(C2D->Copy());
  aTrC->Translate(aTrV);

  if (dir)
  {
    if (firstOrder)
      BB.UpdateEdge(aEdgeToUpdate, C2D, aTrC, toFace, tol);
    else
      BB.UpdateEdge(aEdgeToUpdate, aTrC, C2D, toFace, tol);
  }
  else
  {
    if (!firstOrder)
      BB.UpdateEdge(aEdgeToUpdate, C2D, aTrC, toFace, tol);
    else
      BB.UpdateEdge(aEdgeToUpdate, aTrC, C2D, toFace, tol);
  }
}

//=================================================================================================

bool TopOpeBRepBuild_Tools::IsDegEdgesTheSame(const TopoDS_Shape& anE1, const TopoDS_Shape& anE2)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aVMap1, aVMap2;
  TopExp::MapShapes(anE1, TopAbs_VERTEX, aVMap1);
  TopExp::MapShapes(anE2, TopAbs_VERTEX, aVMap2);

  if (!aVMap1.Extent() || !aVMap2.Extent())
    return false;

  return aVMap1(1).IsSame(aVMap2(1));
}

//=======================================================================
// function : NormalizeFace
// purpose  : remove all INTERNAL and EXTERNAL edges from the face
//=======================================================================
void TopOpeBRepBuild_Tools::NormalizeFace(const TopoDS_Shape& oldFace, TopoDS_Shape& corrFace)
{
  double tolF1;

  TopLoc_Location Loc;
  TopoDS_Face     aF1 = TopoDS::Face(oldFace), aNewFace;

  aF1.Orientation(TopAbs_FORWARD);

  occ::handle<Geom_Surface> Surf = BRep_Tool::Surface(aF1, Loc);
  tolF1                          = BRep_Tool::Tolerance(aF1);
  BRep_Builder BB;
  BB.MakeFace(aNewFace, Surf, Loc, tolF1);

  TopExp_Explorer aFExp(aF1, TopAbs_WIRE);
  for (; aFExp.More(); aFExp.Next())
  {
    int          NbGoodEdges = 0;
    TopoDS_Shape aWire       = aFExp.Current();
    aWire.Orientation(TopAbs_FORWARD);
    TopoDS_Wire aNewWire;

    BB.MakeWire(aNewWire);

    TopExp_Explorer aWExp(aWire, TopAbs_EDGE);
    for (; aWExp.More(); aWExp.Next())
    {
      TopoDS_Shape anEdge = aWExp.Current();

      if (anEdge.Orientation() == TopAbs_EXTERNAL || anEdge.Orientation() == TopAbs_INTERNAL)
        continue;

      BB.Add(aNewWire, TopoDS::Edge(anEdge));
      NbGoodEdges++;
    }
    // keep wire  orientation
    aNewWire.Orientation(aFExp.Current().Orientation()); // aWire.Orientation());

    if (NbGoodEdges) // we add new wire only if it contains at least one edge
      BB.Add(aNewFace, aNewWire);
  }
  // keep face  orientation
  aNewFace.Orientation(oldFace.Orientation());

  corrFace = aNewFace;
}

//=======================================================================
// function : CorrectFace2d
// purpose  : adjust PCurves of periodical face in 2d
//=======================================================================
void TopOpeBRepBuild_Tools::CorrectFace2d(
  const TopoDS_Shape&                         oldFace,
  TopoDS_Shape&                               corrFace,
  const NCollection_IndexedMap<TopoDS_Shape>& aSourceShapes,
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&
    aMapOfCorrect2dEdges)
{
  TopOpeBRepBuild_CorrectFace2d aCorrectFace2d(TopoDS::Face(oldFace),
                                               aSourceShapes,
                                               aMapOfCorrect2dEdges);

  aCorrectFace2d.Perform();
  corrFace = oldFace;
}
