// Created on: 2008-04-11
// Created by: Peter KURNEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <BRepMesh_DiscretAlgoFactory.hxx>
#include <BRepMesh_DiscretFactory.hxx>
#include <BRepMesh_DiscretRoot.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawTrSurf.hxx>
#include <gp_Vec.hxx>
#include <GProp_GProps.hxx>
#include <MeshTest.hxx>
#include <MeshTest_CheckTopology.hxx>
#include <NCollection_Map.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>

static int mpnames(Draw_Interpretor&, int, const char**);
static int mpsetdefaultname(Draw_Interpretor&, int, const char**);
static int mpgetdefaultname(Draw_Interpretor&, int, const char**);
static int mpincmesh(Draw_Interpretor&, int, const char**);
static int mpparallel(Draw_Interpretor&, int, const char**);
static int triarea(Draw_Interpretor&, int, const char**);
static int tricheck(Draw_Interpretor&, int, const char**);

//=================================================================================================

void MeshTest::PluginCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
  {
    return;
  }
  done = true;
  //
  const char* g = "Mesh Commands";
  // Commands
  theCommands.Add("mpnames", "mpnames : list available meshing algorithms", __FILE__, mpnames, g);
  theCommands.Add("mpsetdefaultname",
                  "mpsetdefaultname name : set default meshing algorithm",
                  __FILE__,
                  mpsetdefaultname,
                  g);
  theCommands.Add("mpgetdefaultname",
                  "mpgetdefaultname : get default meshing algorithm name",
                  __FILE__,
                  mpgetdefaultname,
                  g);
  theCommands.Add("mpincmesh", "mpincmesh shape deflection [angle]", __FILE__, mpincmesh, g);
  theCommands.Add("mpparallel",
                  "mpparallel [toTurnOn] : show / set multi-threading flag for incremental mesh",
                  __FILE__,
                  mpparallel,
                  g);
  theCommands.Add("triarea",
                  "shape [eps]  (computes triangles and surface area)",
                  __FILE__,
                  triarea,
                  g);
  theCommands.Add("tricheck",
                  "shape [-small]  (checks triangulation of shape);\n"
                  "\"-small\"-option allows finding triangles with small area",
                  __FILE__,
                  tricheck,
                  g);
}

//=================================================================================================

static int mpnames(Draw_Interpretor&, int n, const char**)
{
  if (n != 1)
  {
    printf(" use mpnames\n");
    return 0;
  }

  const NCollection_List<occ::handle<BRepMesh_DiscretAlgoFactory>>& aFactories =
    BRepMesh_DiscretAlgoFactory::Factories();

  if (aFactories.IsEmpty())
  {
    printf(" *no algorithms registered\n");
    return 0;
  }

  printf(" *available algorithms:\n");
  for (NCollection_List<occ::handle<BRepMesh_DiscretAlgoFactory>>::Iterator anIter(aFactories);
       anIter.More();
       anIter.Next())
  {
    printf("  %s\n", anIter.Value()->Name().ToCString());
  }

  return 0;
}

//=================================================================================================

static int mpsetdefaultname(Draw_Interpretor&, int n, const char** a)
{
  if (n != 2)
  {
    printf(" use mpsetdefaultname name\n");
    return 0;
  }

  TCollection_AsciiString aName = a[1];
  if (BRepMesh_DiscretFactory::Get().SetDefaultName(aName))
  {
    printf(" *ready\n");
  }
  else
  {
    printf(" *algorithm '%s' not found\n", aName.ToCString());
  }

  return 0;
}

//=================================================================================================

static int mpgetdefaultname(Draw_Interpretor&, int n, const char**)
{
  if (n != 1)
  {
    printf(" use mpgetdefaultname\n");
    return 0;
  }

  const TCollection_AsciiString& aName = BRepMesh_DiscretFactory::Get().DefaultName();
  printf(" *default name: %s\n", aName.ToCString());

  return 0;
}

//=================================================================================================

static int mpincmesh(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
  {
    printf(" use mpincmesh shape deflection [angle]\n");
    return 0;
  }

  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull())
  {
    printf(" null shape is not allowed here\n");
    return 0;
  }

  double aDeflection = Draw::Atof(a[2]);
  double aAngle      = 0.5;
  if (n > 3)
  {
    aAngle = Draw::Atof(a[3]);
  }

  occ::handle<BRepMesh_DiscretRoot> aMeshAlgo =
    BRepMesh_DiscretFactory::Get().Discret(aS, aDeflection, aAngle);

  if (aMeshAlgo.IsNull())
  {
    printf(" *Cannot create the meshing algorithm\n");
    return 0;
  }

  aMeshAlgo->Perform();
  if (!aMeshAlgo->IsDone())
  {
    printf(" *Meshing not done\n");
  }

  return 0;
}

// #######################################################################
static int triarea(Draw_Interpretor& di, int n, const char** a)
{

  if (n < 2)
    return 1;

  TopoDS_Shape shape = DBRep::Get(a[1]);
  if (shape.IsNull())
    return 1;
  double anEps = -1.;
  if (n > 2)
    anEps = Draw::Atof(a[2]);

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapF;
  TopExp::MapShapes(shape, TopAbs_FACE, aMapF);

  // detect if a shape has triangulation
  bool hasPoly = false;
  int  i;
  for (i = 1; i <= aMapF.Extent(); i++)
  {
    const TopoDS_Face&              aFace = TopoDS::Face(aMapF(i));
    TopLoc_Location                 aLoc;
    occ::handle<Poly_Triangulation> aPoly = BRep_Tool::Triangulation(aFace, aLoc);
    if (!aPoly.IsNull())
    {
      hasPoly = true;
      break;
    }
  }

  // compute area by triangles
  double aTriArea = 0;
  if (hasPoly)
  {
    for (i = 1; i <= aMapF.Extent(); i++)
    {
      const TopoDS_Face&              aFace = TopoDS::Face(aMapF(i));
      TopLoc_Location                 aLoc;
      occ::handle<Poly_Triangulation> aPoly = BRep_Tool::Triangulation(aFace, aLoc);
      if (aPoly.IsNull())
      {
        std::cout << "face " << i << " has no triangulation" << std::endl;
        continue;
      }
      for (int j = 1; j <= aPoly->NbTriangles(); j++)
      {
        const Poly_Triangle tri = aPoly->Triangle(j);
        int                 n1, n2, n3;
        tri.Get(n1, n2, n3);
        const gp_Pnt p1 = aPoly->Node(n1);
        const gp_Pnt p2 = aPoly->Node(n2);
        const gp_Pnt p3 = aPoly->Node(n3);
        gp_Vec       v1(p1, p2);
        gp_Vec       v2(p1, p3);
        double       ar = v1.CrossMagnitude(v2);
        aTriArea += ar;
      }
    }
    aTriArea /= 2;
  }

  // compute area by geometry
  GProp_GProps props;
  if (anEps <= 0.)
    BRepGProp::SurfaceProperties(shape, props);
  else
    BRepGProp::SurfaceProperties(shape, props, anEps);
  double aGeomArea = props.Mass();

  di << aTriArea << " " << aGeomArea << "\n";
  return 0;
}

// #######################################################################
bool IsEqual(const BRepMesh_Edge& theFirst, const BRepMesh_Edge& theSecond)
{
  return theFirst.IsEqual(theSecond);
}

static int tricheck(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
    return 1;

  TopoDS_Shape shape = DBRep::Get(a[1]);
  if (shape.IsNull())
    return 1;

  const bool isToFindSmallTriangles = (n >= 3) ? (strcmp(a[2], "-small") == 0) : false;

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapF;
  TopExp::MapShapes(shape, TopAbs_FACE, aMapF);
  const char* const name = ".";

  // execute check
  MeshTest_CheckTopology aCheck(shape);
  aCheck.Perform(di);

  // dump info on free links inside the triangulation
  int nbFree = 0;
  int nbFac  = aCheck.NbFacesWithFL(), i, k;
  if (nbFac > 0)
  {
    for (k = 1; k <= nbFac; k++)
    {
      int nbEdge = aCheck.NbFreeLinks(k);
      int iF     = aCheck.GetFaceNumWithFL(k);
      nbFree += nbEdge;
      di << "free links of face " << iF << "\n";

      const TopoDS_Shape&             aShape = aMapF.FindKey(iF);
      const TopoDS_Face&              aFace  = TopoDS::Face(aShape);
      TopLoc_Location                 aLoc;
      occ::handle<Poly_Triangulation> aT   = BRep_Tool::Triangulation(aFace, aLoc);
      const gp_Trsf&                  trsf = aLoc.Transformation();

      NCollection_Array1<gp_Pnt>   pnts(1, 2);
      NCollection_Array1<gp_Pnt2d> pnts2d(1, 2);
      for (i = 1; i <= nbEdge; i++)
      {
        int n1, n2;
        aCheck.GetFreeLink(k, i, n1, n2);
        di << "{" << n1 << " " << n2 << "} ";
        pnts(1)                          = aT->Node(n1).Transformed(trsf);
        pnts(2)                          = aT->Node(n2).Transformed(trsf);
        occ::handle<Poly_Polygon3D> poly = new Poly_Polygon3D(pnts);
        DrawTrSurf::Set(name, poly);
        DrawTrSurf::Set(name, pnts(1));
        DrawTrSurf::Set(name, pnts(2));
        if (aT->HasUVNodes())
        {
          pnts2d(1)                          = aT->UVNode(n1);
          pnts2d(2)                          = aT->UVNode(n2);
          occ::handle<Poly_Polygon2D> poly2d = new Poly_Polygon2D(pnts2d);
          DrawTrSurf::Set(name, poly2d);
          DrawTrSurf::Set(name, pnts2d(1));
          DrawTrSurf::Set(name, pnts2d(2));
        }
      }
      di << "\n";
    }
  }

  // dump info on cross face errors
  int nbErr = aCheck.NbCrossFaceErrors();
  if (nbErr > 0)
  {
    di << "cross face errors: {face1, node1, face2, node2, distance}\n";
    for (i = 1; i <= nbErr; i++)
    {
      int    iF1, n1, iF2, n2;
      double aVal;
      aCheck.GetCrossFaceError(i, iF1, n1, iF2, n2, aVal);
      di << "{" << iF1 << " " << n1 << " " << iF2 << " " << n2 << " " << aVal << "} ";
    }
    di << "\n";
  }

  // dump info on edges
  int nbAsync = aCheck.NbAsyncEdges();
  if (nbAsync > 0)
  {
    di << "async edges:\n";
    for (i = 1; i <= nbAsync; i++)
    {
      int ie = aCheck.GetAsyncEdgeNum(i);
      di << ie << " ";
    }
    di << "\n";
  }

  // dump info on free nodes
  int nbFreeNodes = aCheck.NbFreeNodes();
  if (nbFreeNodes > 0)
  {
    di << "free nodes (in pairs: face / node): \n";
    for (i = 1; i <= nbFreeNodes; i++)
    {
      int iface, inode;
      aCheck.GetFreeNodeNum(i, iface, inode);

      const TopoDS_Face&              aFace = TopoDS::Face(aMapF.FindKey(iface));
      TopLoc_Location                 aLoc;
      occ::handle<Poly_Triangulation> aT   = BRep_Tool::Triangulation(aFace, aLoc);
      const gp_Trsf&                  trsf = aLoc.Transformation();
      DrawTrSurf::Set(name, aT->Node(inode).Transformed(trsf));
      if (aT->HasUVNodes())
      {
        DrawTrSurf::Set(name, aT->UVNode(inode));
      }

      di << "{" << iface << " " << inode << "} ";
    }
    di << "\n";
  }

  const int aNbSmallTriangles = isToFindSmallTriangles ? aCheck.NbSmallTriangles() : 0;
  if (aNbSmallTriangles > 0)
  {
    di << "triangles with null area (in pairs: face / triangle): \n";
    for (i = 1; i <= aNbSmallTriangles; i++)
    {
      int aFaceId = 0, aTriID = 0;
      aCheck.GetSmallTriangle(i, aFaceId, aTriID);

      const TopoDS_Face&                    aFace = TopoDS::Face(aMapF.FindKey(aFaceId));
      TopLoc_Location                       aLoc;
      const gp_Trsf&                        aTrsf = aLoc.Transformation();
      const occ::handle<Poly_Triangulation> aT    = BRep_Tool::Triangulation(aFace, aLoc);
      const Poly_Triangle&                  aTri  = aT->Triangle(aTriID);
      int                                   aN1, aN2, aN3;
      aTri.Get(aN1, aN2, aN3);

      NCollection_Array1<gp_Pnt> aPoles(1, 4);
      aPoles(1) = aPoles(4) = aT->Node(aN1).Transformed(aTrsf);
      aPoles(2)             = aT->Node(aN2).Transformed(aTrsf);
      aPoles(3)             = aT->Node(aN3).Transformed(aTrsf);

      NCollection_Array1<int> aMults(1, 4);
      aMults(1) = aMults(4) = 2;
      aMults(2) = aMults(3) = 1;

      NCollection_Array1<double> aKnots(1, 4);
      aKnots(1) = 1.0;
      aKnots(2) = 2.0;
      aKnots(3) = 3.0;
      aKnots(4) = 4.0;

      occ::handle<Geom_BSplineCurve> aBS = new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);

      DrawTrSurf::Set(name, aBS);

      if (aT->HasUVNodes())
      {
        NCollection_Array1<gp_Pnt2d> aPoles2d(1, 4);
        aPoles2d(1) = aPoles2d(4) = aT->UVNode(aN1);
        aPoles2d(2)               = aT->UVNode(aN2);
        aPoles2d(3)               = aT->UVNode(aN3);

        occ::handle<Geom2d_BSplineCurve> aBS2d =
          new Geom2d_BSplineCurve(aPoles2d, aKnots, aMults, 1);

        DrawTrSurf::Set(name, aBS2d);
      }

      di << "{" << aFaceId << " " << aTriID << "} ";
    }

    di << "\n";
  }

  // output errors summary to DRAW
  if (nbFree > 0 || nbErr > 0 || nbAsync > 0 || nbFreeNodes > 0 || (aNbSmallTriangles > 0))
  {
    di << "Free_links " << nbFree << " Cross_face_errors " << nbErr << " Async_edges " << nbAsync
       << " Free_nodes " << nbFreeNodes << " Small triangles " << aNbSmallTriangles << "\n";
  }

  int             aFaceId = 1;
  TopExp_Explorer aFaceExp(shape, TopAbs_FACE);
  for (; aFaceExp.More(); aFaceExp.Next(), ++aFaceId)
  {
    const TopoDS_Shape& aShape = aFaceExp.Current();
    const TopoDS_Face&  aFace  = TopoDS::Face(aShape);

    TopLoc_Location                 aLoc;
    occ::handle<Poly_Triangulation> aT = BRep_Tool::Triangulation(aFace, aLoc);

    // Iterate boundary edges
    NCollection_Map<BRepMesh_Edge> aBoundaryEdgeMap;
    TopExp_Explorer                anExp(aShape, TopAbs_EDGE);
    for (; anExp.More(); anExp.Next())
    {
      TopLoc_Location                          anEdgeLoc;
      const TopoDS_Edge&                       anEdge = TopoDS::Edge(anExp.Current());
      occ::handle<Poly_PolygonOnTriangulation> aPoly =
        BRep_Tool::PolygonOnTriangulation(anEdge, aT, aLoc);
      if (aPoly.IsNull())
      {
        continue;
      }

      const NCollection_Array1<int>& anIndices = aPoly->Nodes();
      int                            aLower    = anIndices.Lower();
      int                            anUpper   = anIndices.Upper();

      int aPrevNode = -1;
      for (int j = aLower; j <= anUpper; ++j)
      {
        int aNodeIdx = anIndices.Value(j);
        if (j != aLower)
        {
          BRepMesh_Edge aLink(aPrevNode, aNodeIdx, BRepMesh_Frontier);
          aBoundaryEdgeMap.Add(aLink);
        }
        aPrevNode = aNodeIdx;
      }
    }

    if (aBoundaryEdgeMap.Size() == 0)
    {
      break;
    }

    NCollection_Map<BRepMesh_Edge> aFreeEdgeMap;
    const int                      aTriNum = aT->NbTriangles();
    for (int aTriIndx = 1; aTriIndx <= aTriNum; aTriIndx++)
    {
      const Poly_Triangle aTri         = aT->Triangle(aTriIndx);
      int                 aTriNodes[3] = {aTri.Value(1), aTri.Value(2), aTri.Value(3)};

      for (int j = 1; j <= 3; ++j)
      {
        int aLastId  = aTriNodes[j % 3];
        int aFirstId = aTriNodes[j - 1];

        BRepMesh_Edge aLink(aFirstId, aLastId, BRepMesh_Free);
        if (!aBoundaryEdgeMap.Contains(aLink))
        {
          if (!aFreeEdgeMap.Add(aLink))
          {
            aFreeEdgeMap.Remove(aLink);
          }
        }
      }
    }

    if (aFreeEdgeMap.Size() != 0)
    {
      di << "Not connected mesh inside face " << aFaceId << "\n";

      const gp_Trsf& trsf = aLoc.Transformation();

      NCollection_Array1<gp_Pnt>               pnts(1, 2);
      NCollection_Array1<gp_Pnt2d>             pnts2d(1, 2);
      NCollection_Map<BRepMesh_Edge>::Iterator aMapIt(aFreeEdgeMap);
      for (; aMapIt.More(); aMapIt.Next())
      {
        const BRepMesh_Edge& aLink = aMapIt.Key();
        di << "{" << aLink.FirstNode() << " " << aLink.LastNode() << "} ";
        pnts(1)                          = aT->Node(aLink.FirstNode()).Transformed(trsf);
        pnts(2)                          = aT->Node(aLink.LastNode()).Transformed(trsf);
        occ::handle<Poly_Polygon3D> poly = new Poly_Polygon3D(pnts);
        DrawTrSurf::Set(name, poly);
        DrawTrSurf::Set(name, pnts(1));
        DrawTrSurf::Set(name, pnts(2));
        if (aT->HasUVNodes())
        {
          pnts2d(1)                          = aT->UVNode(aLink.FirstNode());
          pnts2d(2)                          = aT->UVNode(aLink.LastNode());
          occ::handle<Poly_Polygon2D> poly2d = new Poly_Polygon2D(pnts2d);
          DrawTrSurf::Set(name, poly2d);
          DrawTrSurf::Set(name, pnts2d(1));
          DrawTrSurf::Set(name, pnts2d(2));
        }
      }
      di << "\n";
    }
  }
  return 0;
}

//=================================================================================================

static int mpparallel(Draw_Interpretor& /*di*/, int argc, const char** argv)
{
  if (argc == 2)
  {
    bool isParallelOn = Draw::Atoi(argv[1]) == 1;
    BRepMesh_IncrementalMesh::SetParallelDefault(isParallelOn);
  }
  std::cout << "Incremental Mesh, multi-threading "
            << (BRepMesh_IncrementalMesh::IsParallelDefault() ? "ON\n" : "OFF\n");
  return 0;
}
