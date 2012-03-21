// Created on: 2008-04-11
// Created by: Peter KURNEV
// Copyright (c) 2008-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <MeshTest.ixx>
#include <Draw_Interpretor.hxx>
#include <TColStd_MapOfAsciiString.hxx>
#include <BRepMesh_DiscretFactory.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_MapIteratorOfMapOfAsciiString.hxx>
#include <BRepMesh_FactoryError.hxx>
#include <BRepMesh_DiscretRoot.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Bnd_Box.hxx>
#include <BRepMesh_PDiscretRoot.hxx>
#include <DBRep.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <Poly_Triangulation.hxx>
#include <gp_Vec.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <DrawTrSurf.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <MeshTest_CheckTopology.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Polygon2D.hxx>
#include <Standard.hxx>

static Standard_Integer mpnames           (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer mpsetdefaultname  (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer mpgetdefaultname  (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer mpsetfunctionname (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer mpgetfunctionname (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer mperror           (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer mpincmesh         (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer mpparallel        (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer triarea           (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer tricheck          (Draw_Interpretor& , Standard_Integer , const char** );

//=======================================================================
//function : PluginCommands
//purpose  : 
//=======================================================================
void MeshTest::PluginCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) {
    return;
  }
  done = Standard_True;
  //
  const char* g = "Mesh Commands";
  // Commands
  theCommands.Add("mpnames"          , "use mpnames"          , __FILE__, mpnames    , g);
  theCommands.Add("mpsetdefaultname" , "use mpsetdefaultname" , __FILE__, mpsetdefaultname     , g);
  theCommands.Add("mpgetdefaultname" , "use mpgetdefaultname" , __FILE__, mpgetdefaultname     , g);
  theCommands.Add("mpsetfunctionname", "use mpsetfunctionname", __FILE__, mpsetfunctionname     , g);
  theCommands.Add("mpgetfunctionname", "use mpgetfunctionname", __FILE__, mpgetfunctionname     , g);
  theCommands.Add("mperror"          , "use mperror"          , __FILE__, mperror     , g);
  theCommands.Add("mpincmesh"        , "use mpincmesh"        , __FILE__, mpincmesh      , g);
  theCommands.Add("mpparallel"       , "mpparallel [toTurnOn] : show / set multi-threading flag for incremental mesh",
    __FILE__, mpparallel, g);
  theCommands.Add("triarea","shape [eps]  (computes triangles and surface area)",__FILE__, triarea, g);
  theCommands.Add("tricheck", "shape   (checks triangulation of shape)", __FILE__, tricheck, g);
  
}

//=======================================================================
//function : mpnames
//purpose  : 
//=======================================================================
static Standard_Integer mpnames (Draw_Interpretor& , Standard_Integer n, const char** )
{
  Standard_Integer aNb;
  TColStd_MapIteratorOfMapOfAsciiString aIt;
  //
  if (n!=1) {
    printf(" use mpnames\n");
    return 0;
  }
  //
  const TColStd_MapOfAsciiString& aMN=BRepMesh_DiscretFactory::Get().Names();
  aNb=aMN.Extent();
  if (!aNb) {
    printf(" *no names found\n");
    return 0;
  }
  //
  printf(" *available names:\n");
  aIt.Initialize(aMN);
  for (; aIt.More(); aIt.Next()) {
    const TCollection_AsciiString& aName=aIt.Key();
    printf("  %s\n", aName.ToCString());
  }
  //
  return 0;
}
//=======================================================================
//function : mpsetdefaultname
//purpose  : 
//=======================================================================
static Standard_Integer mpsetdefaultname (Draw_Interpretor& , Standard_Integer n, const char**a )
{
  TCollection_AsciiString aName;
  //
  if (n!=2) {
    printf(" use mpsetdefaultname name\n");
    return 0;
  }
  //
  aName=a[1];
  //
  if (BRepMesh_DiscretFactory::Get().SetDefaultName (aName))
    printf(" *ready\n");
  else
    printf(" *fault\n");
  //
  return 0;
}
//=======================================================================
//function : mpgetdefaultname
//purpose  : 
//=======================================================================
static Standard_Integer mpgetdefaultname (Draw_Interpretor& , Standard_Integer n, const char** )
{
  if (n!=1) {
    printf(" use mpgetdefaultname\n");
    return 0;
  }
  //
  const TCollection_AsciiString& aName=BRepMesh_DiscretFactory::Get().DefaultName();
  printf(" *default name: %s\n", aName.ToCString());
  //
  return 0;
}
//=======================================================================
//function : mpsetfunctionname
//purpose  : 
//=======================================================================
static Standard_Integer mpsetfunctionname (Draw_Interpretor& , Standard_Integer n, const char**a )
{
  TCollection_AsciiString aName;
  //
  if (n!=2) {
    printf(" use mpsetfunctionname name\n");
    return 0;
  }
  //
  aName=a[1];
  //
  if (BRepMesh_DiscretFactory::Get().SetFunctionName (aName))
    printf(" *ready\n");
  else
    printf(" *fault\n");
  //
  return 0;
}
//=======================================================================
//function : mpgetdefaultname
//purpose  : 
//=======================================================================
static Standard_Integer mpgetfunctionname (Draw_Interpretor& , Standard_Integer n, const char** )
{
  if (n!=1) {
    printf(" use mpgetfunctionname\n");
    return 0;
  }
  //
  const TCollection_AsciiString& aName=BRepMesh_DiscretFactory::Get().FunctionName();
  printf(" *function name: %s\n", aName.ToCString());
  //
  return 0;
}
//=======================================================================
//function : mperror
//purpose  : 
//=======================================================================
static Standard_Integer mperror (Draw_Interpretor& , Standard_Integer n, const char** )
{
  BRepMesh_FactoryError aErr;
  //
  if (n!=1) {
    printf(" use mperror\n");
    return 0;
  }
  //
  aErr=BRepMesh_DiscretFactory::Get().ErrorStatus();
  printf(" *ErrorStatus: %d\n", (int)aErr);
  //
  return 0;
}

//=======================================================================
//function :mpincmesh
//purpose  : 
//=======================================================================
static Standard_Integer mpincmesh (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  Standard_Real aDeflection, aAngle;
  TopoDS_Shape aS;
  //
  if (n<3) {
    printf(" use mpincmesh s deflection [angle]\n");
    return 0;
  }
  //
  aS=DBRep::Get(a[1]);
  if (aS.IsNull()) {
    printf(" null shapes is not allowed here\n");
    return 0;
  }
  //
  aDeflection=atof(a[2]);
  aAngle=0.5;
  if (n>3) {
    aAngle=atof(a[3]);
  }
  //
  Handle(BRepMesh_DiscretRoot) aMeshAlgo = BRepMesh_DiscretFactory::Get().Discret (aS,
                                                                                   aDeflection,
                                                                                   aAngle);
  //
  BRepMesh_FactoryError aErr = BRepMesh_DiscretFactory::Get().ErrorStatus();
  if (aErr != BRepMesh_FE_NOERROR)
  {
    printf(" *Factory::Get().ErrorStatus()=%d\n", (int)aErr);
  }
  //
  if (aMeshAlgo.IsNull())
  {
    printf(" *Can not create the algo\n");
    return 0;
  }
  //
  aMeshAlgo->Perform();
  if (!aMeshAlgo->IsDone())
  {
    printf(" *Not done\n");
  }
  //
  return 0;
}

//#######################################################################
static Standard_Integer triarea (Draw_Interpretor& di, int n, const char ** a)
{

  if (n < 2) return 1;

  TopoDS_Shape shape = DBRep::Get(a[1]);
  if (shape.IsNull()) return 1;
  Standard_Real anEps = -1.;
  if (n > 2)
    anEps = atof(a[2]);

  TopTools_IndexedMapOfShape aMapF;
  TopExp::MapShapes (shape, TopAbs_FACE, aMapF);

  // detect if a shape has triangulation
  Standard_Boolean hasPoly = Standard_False;
  int i;
  for (i=1; i <= aMapF.Extent(); i++) {
    const TopoDS_Face& aFace = TopoDS::Face(aMapF(i));
    TopLoc_Location aLoc;
    Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
    if (!aPoly.IsNull()) {
      hasPoly = Standard_True;
      break;
    }
  }

  // compute area by triangles
  double aTriArea=0;
  if (hasPoly) {
    for (i=1; i <= aMapF.Extent(); i++) {
      const TopoDS_Face& aFace = TopoDS::Face(aMapF(i));
      TopLoc_Location aLoc;
      Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
      if (aPoly.IsNull()) {
        cout << "face "<<i<<" has no triangulation"<<endl;
        continue;
      }
      const Poly_Array1OfTriangle& triangles = aPoly->Triangles();
      const TColgp_Array1OfPnt& nodes = aPoly->Nodes();
      for (int j=triangles.Lower(); j <= triangles.Upper(); j++) {
        const Poly_Triangle& tri = triangles(j);
        int n1, n2, n3;
        tri.Get (n1, n2, n3);
        const gp_Pnt& p1 = nodes(n1);
        const gp_Pnt& p2 = nodes(n2);
        const gp_Pnt& p3 = nodes(n3);
        gp_Vec v1(p1, p2);
        gp_Vec v2(p1, p3);
        double ar = v1.CrossMagnitude(v2);
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

//#######################################################################
static Standard_Integer tricheck (Draw_Interpretor& di, int n, const char ** a)
{
  if (n < 2) return 1;

  TopoDS_Shape shape = DBRep::Get(a[1]);
  if (shape.IsNull()) return 1;

  TopTools_IndexedMapOfShape aMapF;
  TopExp::MapShapes (shape, TopAbs_FACE, aMapF);
  Standard_CString name = ".";

  // execute check
  MeshTest_CheckTopology aCheck(shape);
  aCheck.Perform(di);

  // dump info on free links inside the triangulation
  Standard_Integer nbFree = 0;
  Standard_Integer nbFac = aCheck.NbFacesWithFL(), i, k;
  if (nbFac > 0) {
    for (k=1; k <= nbFac; k++) {
      Standard_Integer nbEdge = aCheck.NbFreeLinks(k);
      Standard_Integer iF = aCheck.GetFaceNumWithFL(k);
      nbFree += nbEdge;
      di << "free links of face " << iF << "\n";
      const TopoDS_Face& aFace = TopoDS::Face(aMapF.FindKey(iF));
      TopLoc_Location aLoc;
      Handle(Poly_Triangulation) aT = BRep_Tool::Triangulation(aFace, aLoc);
      const TColgp_Array1OfPnt& aPoints = aT->Nodes();
      const TColgp_Array1OfPnt2d& aPoints2d = aT->UVNodes();
      const gp_Trsf& trsf = aLoc.Transformation();
      TColgp_Array1OfPnt pnts(1,2);
      TColgp_Array1OfPnt2d pnts2d(1,2);
      for (i=1; i <= nbEdge; i++) {
        Standard_Integer n1, n2;
        aCheck.GetFreeLink(k, i, n1, n2);
        di << "{" << n1 << " " << n2 << "} ";
        pnts(1) = aPoints(n1).Transformed(trsf);
        pnts(2) = aPoints(n2).Transformed(trsf);
        Handle(Poly_Polygon3D) poly = new Poly_Polygon3D (pnts);
        DrawTrSurf::Set (name, poly);
        DrawTrSurf::Set (name, pnts(1));
        DrawTrSurf::Set (name, pnts(2));
        pnts2d(1) = aPoints2d(n1);
        pnts2d(2) = aPoints2d(n2);
        Handle(Poly_Polygon2D) poly2d = new Poly_Polygon2D (pnts2d);
        DrawTrSurf::Set (name, poly2d);
        DrawTrSurf::Set (name, pnts2d(1));
        DrawTrSurf::Set (name, pnts2d(2));
      }
      di << "\n";
    }
  }

  // dump info on cross face errors
  Standard_Integer nbErr = aCheck.NbCrossFaceErrors();
  if (nbErr > 0) {
    di << "cross face errors: {face1, node1, face2, node2, distance}" << "\n";
    for (i=1; i <= nbErr; i++) {
      Standard_Integer iF1, n1, iF2, n2;
      Standard_Real aVal;
      aCheck.GetCrossFaceError(i, iF1, n1, iF2, n2, aVal);
      di << "{" << iF1 << " " << n1 << " " << iF2 << " " << n2 << " " << aVal << "} ";
    }
    di << "\n";
  }

  // dump info on edges
  Standard_Integer nbAsync = aCheck.NbAsyncEdges();
  if (nbAsync > 0) {
    di << "async edges:" << "\n";
    for (i=1; i <= nbAsync; i++) {
      Standard_Integer ie = aCheck.GetAsyncEdgeNum(i);
      di << ie << " ";
    }
    di << "\n";
  }

  // dump info on free nodes
  Standard_Integer nbFreeNodes = aCheck.NbFreeNodes();
  if (nbFreeNodes > 0) {
    di << "free nodes (in pairs: face / node): " << "\n";
    for (i=1; i <= nbFreeNodes; i++) {
      Standard_Integer iface, inode;
      aCheck.GetFreeNodeNum(i, iface, inode);
      di << "{" << iface << " " << inode << "} ";
    }
    di << "\n";
  }

  // output errors summary to DRAW
  if ( nbFree > 0 || nbErr > 0 || nbAsync > 0 || nbFreeNodes > 0 )
    di << "Free_links " << nbFree
       << " Cross_face_errors " << nbErr
       << " Async_edges " << nbAsync 
       << " Free_nodes " << nbFreeNodes << "\n";
  return 0;
}

//=======================================================================
//function : mpparallel
//purpose  :
//=======================================================================
static int mpparallel (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc == 2)
  {
    Standard_Boolean isParallelOn = atoi (argv[1]) == 1;
    BRepMesh_IncrementalMesh::SetParallelDefault (isParallelOn);
    if (isParallelOn)
      Standard::SetReentrant(Standard_True);
  }
  std::cout << "Incremental Mesh, multi-threading "
            << (BRepMesh_IncrementalMesh::IsParallelDefault() ? "ON\n" : "OFF\n");
  return 0;
}
