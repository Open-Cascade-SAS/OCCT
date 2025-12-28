// Created on: 1995-06-14
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#include <gp_Pnt2d.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_FaceEdgeFiller.hxx>
#include <TopOpeBRep_FaceEdgeIntersector.hxx>
#include <TopOpeBRep_PointGeomTool.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_InterferenceTool.hxx>
#include <TopOpeBRepDS_Point.hxx>

#ifdef OCCT_DEBUG
extern void FEINT_DUMPPOINTS(TopOpeBRep_FaceEdgeIntersector&   FEINT,
                             const TopOpeBRepDS_DataStructure& BDS);
#endif

//=================================================================================================

TopOpeBRep_FaceEdgeFiller::TopOpeBRep_FaceEdgeFiller() {}

//=================================================================================================

void TopOpeBRep_FaceEdgeFiller::Insert(const TopoDS_Shape&                             F,
                                       const TopoDS_Shape&                             E,
                                       TopOpeBRep_FaceEdgeIntersector&                 FEINT,
                                       const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  const TopoDS_Face& FF = TopoDS::Face(F);
  const TopoDS_Edge& EE = TopoDS::Edge(E);

  // exit if no point.
  FEINT.InitPoint();
  if (!FEINT.MorePoint())
    return;

  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();

  TopAbs_Orientation FFori = FF.Orientation();

  // --- Add <FF,EE> in BDS
  int FFindex = BDS.AddShape(FF, 1);
  BDS.AddShape(EE, 2);

  // --- get list of interferences connected to edges <FF>,<EE>
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& EIL1 = BDS.ChangeShapeInterferences(FF);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& EIL2 = BDS.ChangeShapeInterferences(EE);

  occ::handle<TopOpeBRepDS_Interference> EPI; // edge/point interference
  occ::handle<TopOpeBRepDS_Interference> EVI; // edge/vertex interference

  for (; FEINT.MorePoint(); FEINT.NextPoint())
  {
    gp_Pnt2d pUV;
    FEINT.UVPoint(pUV);
    double parE = FEINT.Parameter();

#ifdef OCCT_DEBUG
    TopAbs_Orientation      EEori = EE.Orientation();
    TopOpeBRepDS_Transition TFF   = FEINT.Transition(1, EEori);
    (void)TFF;
#endif
    TopOpeBRepDS_Transition TEE = FEINT.Transition(2, FFori);
    TEE.Index(FFindex);

    TopoDS_Vertex V1;
    bool          isvertexF = FEINT.IsVertex(1, V1);
    TopoDS_Vertex V2;
    bool          isvertexE = FEINT.IsVertex(2, V2);
    bool          isvertex  = isvertexF || isvertexE;

    int  DSPindex;
    bool EPIfound;

    if (!isvertex)
    {
      // create a new geometry in DS ( and get its index <DSPindex> )
      // or get the index <DSPindex> of an equal geometry
      // among those of Edge/Point interferences connected to <FF> (or <EE>).

      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itEIL1(EIL1);
      EPIfound = GetGeometry(itEIL1, FEINT, DSPindex, BDS);
      if (!EPIfound)
        DSPindex = MakeGeometry(FEINT, BDS);

      // sur FF
      // NYI : FacePointInterference : on ajoute un point INTERNAL a F
      //      FPI = new TopOpeBRepDS_FacePointInterference(TopAbs_INTERNAL,
      //					       TopOpeBRepDS_FACE,FFindex,
      //					       TopOpeBRepDS_POINT,DSPindex,
      //					       pUV);
      //      StoreInterference(FPI,EIL1,BDS);

      // sur EE
      EPI = TopOpeBRepDS_InterferenceTool::MakeCurveInterference(TEE,
                                                                 TopOpeBRepDS_FACE,
                                                                 FFindex,
                                                                 TopOpeBRepDS_POINT,
                                                                 DSPindex,
                                                                 parE);
      StoreInterference(EPI, EIL2, BDS);
    }

    else if (isvertexF)
    {
      // si aussi vertex de E, in privilegie le vertex de la face
      int Vindex = BDS.AddShape(V1, 1);

      // sur FF
      // NYI pour toute arete de F qui accede V , une EVI
      //      EVI = new TopOpeBRepDS_EdgeVertexInterference(Transition ?? <-> TFF,
      //						    TopOpeBRepDS_EDGE,E de F,
      //						    TopOpeBRepDS_VERTEX,Vindex,
      //						    param de V sur E de F);
      //      StoreInterference(EVI,EIL1,BDS);

      // sur EE
      EPI = new TopOpeBRepDS_CurvePointInterference(TEE,
                                                    TopOpeBRepDS_FACE,
                                                    FFindex,
                                                    TopOpeBRepDS_VERTEX,
                                                    Vindex,
                                                    parE);
      StoreInterference(EPI, EIL2, BDS);
    }

    else if (isvertexE)
    {
      int Vindex = BDS.AddShape(V2, 2);

      // sur FF
      // NYI : FaceVertexInterference : on ajoute un vertex INTERNAL a F
      //      FVI = new TopOpeBRepDS_FaceVertexInterference(TopAbs_INTERNAL,
      //					       TopOpeBRepDS_FACE,FFindex,
      //					       TopOpeBRepDS_VERTEX,Vindex,
      //					       pUV);
      //      StoreInterference(FPI,EIL1,BDS);

      // sur EE
      EPI = new TopOpeBRepDS_CurvePointInterference(TEE,
                                                    TopOpeBRepDS_FACE,
                                                    FFindex,
                                                    TopOpeBRepDS_VERTEX,
                                                    Vindex,
                                                    parE);
      StoreInterference(EPI, EIL2, BDS);
    }
  }
}

// ===============
// private methods
// ===============

//-----------------------------------------------------------------------
// Search, among a list of interferences accessed by the iterator <IT>,
// a geometry whose 3D point is identical yo the 3D point of a DS point <DSP>.
// TheDSPointGeomTool is a tool able to access the 3D point of the DS point
// DSP,and to test identity (according to tolerance connected to DSP) of the
// DS points.
// return True if such an interference has been found, False else.
// if True, iterator <IT> points (by the Value() method) on the first
// interference accessing an identical 3D point.
//-----------------------------------------------------------------------
//=================================================================================================

bool TopOpeBRep_FaceEdgeFiller::ScanInterfList(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator& IT,
  const TopOpeBRepDS_Point&                                           DSP,
  const TopOpeBRepDS_DataStructure&                                   BDS) const
{
  for (; IT.More(); IT.Next())
  {
    int                       DSPG     = IT.Value()->Geometry();
    const TopOpeBRepDS_Point& otherDSP = BDS.Point(DSPG);
    if (TopOpeBRep_PointGeomTool::IsEqual(DSP, otherDSP))
      return true;
  }
  return false;
}

//=================================================================================================

bool TopOpeBRep_FaceEdgeFiller::GetGeometry(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator& IT,
  const TopOpeBRep_FaceEdgeIntersector&                               FEINT,
  int&                                                                G,
  const TopOpeBRepDS_DataStructure&                                   BDS) const
{
  TopOpeBRepDS_Point DSP   = TopOpeBRep_PointGeomTool::MakePoint(FEINT);
  bool               found = ScanInterfList(IT, DSP, BDS);
  if (found)
    G = IT.Value()->Geometry();
  return found;
}

//=================================================================================================

bool TopOpeBRep_FaceEdgeFiller::GetGeometry(
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L,
  const TopOpeBRepDS_Point&                                       DSP,
  int&                                                            G,
  TopOpeBRepDS_DataStructure&                                     BDS) const
{
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itI(L);
  bool Ifound = ScanInterfList(itI, DSP, BDS);
  if (Ifound)
    G = itI.Value()->Geometry();
  else
    G = BDS.AddPoint(DSP);
  return Ifound; // interference found or not
}

//-----------------------------------------------------------------------
// StoreInterference : Append an interference I to a list of interference LI
//                   : Append I to the interf. list connected to I Geometry()
//-----------------------------------------------------------------------
//=================================================================================================

void TopOpeBRep_FaceEdgeFiller::StoreInterference(
  const occ::handle<TopOpeBRepDS_Interference>&             I,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
  TopOpeBRepDS_DataStructure&                               BDS) const
{
  // append I to list LI
  LI.Append(I);

  int G = I->Geometry();

  // append I to list of interference connected to G = I->Geometry()
  switch (I->GeometryType())
  {

    case TopOpeBRepDS_SOLID:
    case TopOpeBRepDS_FACE:
    case TopOpeBRepDS_EDGE:
    case TopOpeBRepDS_VERTEX:
      //    appendtoG = true;
      //    BDS.ChangeShapeInterferences(G).Append(I);
      break;

    case TopOpeBRepDS_SURFACE:
      BDS.ChangeSurfaceInterferences(G).Append(I);
      break;

    case TopOpeBRepDS_CURVE:
      BDS.ChangeCurveInterferences(G).Append(I);
      break;

    case TopOpeBRepDS_POINT:
      //    appendtoG = true;
      //    BDS.ChangePointInterferences(G).Append(I);
      break;
    default:
      break;
  }
}

//=================================================================================================

int TopOpeBRep_FaceEdgeFiller::MakeGeometry(TopOpeBRep_FaceEdgeIntersector& FEINT,
                                            TopOpeBRepDS_DataStructure&     BDS) const
{
  int G = 0;

  TopoDS_Vertex V1;
  bool          isvertexF = FEINT.IsVertex(1, V1);
  TopoDS_Vertex V2;
  bool          isvertexE = FEINT.IsVertex(2, V2);
  bool          isvertex  = isvertexF || isvertexE;

  if (isvertex)
  {
    if (isvertexF)
      G = BDS.AddShape(V1, 1);
    else if (isvertexE)
      G = BDS.AddShape(V2, 2);
  }
  else
    G = BDS.AddPoint(TopOpeBRep_PointGeomTool::MakePoint(FEINT));
  return G;
}
