// Created on: 2002-05-28
// Created by: QA Admin
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <QABugs.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <BRepBuilderAPI_MakePolygon.hxx>
#include <TopoDS_Face.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>

#include <BRepAlgoAPI_Fuse.hxx>

#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRep_Tool.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp.hxx>
#include <ChFi3d_FilletShape.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_BSplineSurface.hxx>

#include <ShapeUpgrade_UnifySameDomain.hxx>

static Standard_Integer OCC426 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc != 8) {
    di << "Usage : " << argv[0] << " shape1 shape2 shape3 shape4 shape5 shape6 shape7\n";
    return 1;
  }

  BRepBuilderAPI_MakePolygon W1;
  W1.Add(gp_Pnt(10, 0, 0));
  W1.Add(gp_Pnt(20, 0, 0));
  W1.Add(gp_Pnt(20, 0, 10));
  W1.Add(gp_Pnt(10, 0, 10));
  W1.Add(gp_Pnt(10, 0, 0));

  Standard_Boolean OnlyPlane1 = Standard_False;
  TopoDS_Face F1 = BRepBuilderAPI_MakeFace(W1.Wire(), OnlyPlane1);

  gp_Pnt P1(0, 0, 0);
  gp_Dir D1(0, 0, 30);
  gp_Ax1 A1(P1,D1);
  Standard_Real angle1 = 360 * (M_PI / 180.0);
  TopoDS_Shape rs1 = BRepPrimAPI_MakeRevol(F1, A1, angle1);

  BRepBuilderAPI_MakePolygon W2;
  Standard_Real f1 =  7.0710678118654752440;
  Standard_Real f2 = 14.1421356237309504880;
  W2.Add(gp_Pnt(f1, f1, 10));
  W2.Add(gp_Pnt(f2, f2, 10));
  W2.Add(gp_Pnt(f2, f2, 20));
  W2.Add(gp_Pnt(f1, f1, 20));
  W2.Add(gp_Pnt(f1, f1, 10));

  Standard_Boolean OnlyPlane2 = Standard_False;
  TopoDS_Face F2 = BRepBuilderAPI_MakeFace(W2.Wire(), OnlyPlane2);

  gp_Pnt P2(0, 0, 0);
  gp_Dir D2(0, 0, 30);
  gp_Ax1 A2(P2,D2);
  Standard_Real angle2 = 270 * (M_PI / 180.0);
  TopoDS_Shape rs2 = BRepPrimAPI_MakeRevol(F2, A2, angle2);

  BRepBuilderAPI_MakePolygon W3;
  W3.Add(gp_Pnt(10, 0, 20));
  W3.Add(gp_Pnt(20, 0, 20));
  W3.Add(gp_Pnt(20, 0, 30));
  W3.Add(gp_Pnt(10, 0, 30));
  W3.Add(gp_Pnt(10, 0, 20));

  Standard_Boolean OnlyPlane3 = Standard_False;
  TopoDS_Face F3 = BRepBuilderAPI_MakeFace(W3.Wire(), OnlyPlane3);

  gp_Pnt P3(0, 0, 0);
  gp_Dir D3(0, 0, 30);
  gp_Ax1 A3(P3,D3);
  Standard_Real angle3 = 360 * (M_PI / 180.0);
  TopoDS_Shape rs3 = BRepPrimAPI_MakeRevol(F3, A3, angle3);

  di << "fuse32 = BRepAlgoAPI_Fuse(rs3, rs2)\n";
  di << "fuse321 = BRepAlgoAPI_Fuse(fuse32, rs1)\n";
  TopoDS_Shape fuse32 = BRepAlgoAPI_Fuse(rs3, rs2).Shape();
  TopoDS_Shape fuse321 = BRepAlgoAPI_Fuse(fuse32, rs1).Shape();

  // unify the faces of the Fuse result
  ShapeUpgrade_UnifySameDomain anUnify(fuse321, Standard_True, Standard_True, Standard_True);
  anUnify.Build();
  const TopoDS_Shape& aFuseUnif = anUnify.Shape();

  //Give the mass claculation of the shpae "aFuseUnif"
  GProp_GProps G;
  BRepGProp::VolumeProperties(aFuseUnif, G);
  di<<" \n";
  di<<"Mass: "<<G.Mass()<<"\n\n";

  di << "Trianglating Faces .....\n";
  TopExp_Explorer ExpFace;

  for (ExpFace.Init (aFuseUnif,TopAbs_FACE); ExpFace.More(); ExpFace.Next())
    {
      TopoDS_Face TopologicalFace = TopoDS::Face (ExpFace.Current());
      TopologicalFace.Orientation (TopAbs_FORWARD) ;
      BRepMesh_IncrementalMesh IM(TopologicalFace, 1);
      TopLoc_Location loc;
      Handle(Poly_Triangulation) facing = BRep_Tool::Triangulation(TopologicalFace, loc);
      if (facing.IsNull())
      {
        di << "Triangulation FAILED for this face\n";
        continue;
      }
      di << "No of Triangles = " << facing->NbTriangles() << "\n";
    }
  di<<"Triangulation of all Faces Completed. \n\n";

  TopTools_IndexedDataMapOfShapeListOfShape edgemap;
  TopExp::MapShapesAndAncestors(aFuseUnif, TopAbs_EDGE, TopAbs_SOLID, edgemap);
  di << "No. of Edges: " << edgemap.Extent() << "\n";
  ChFi3d_FilletShape FShape = ChFi3d_Rational;
  BRepFilletAPI_MakeFillet blend(aFuseUnif,FShape);
  di << "Adding Edges ..... \n";
  for(int i = 1; i <= edgemap.Extent(); i++)
    {
      // cout << "Adding Edge : " << i << endl;
      TopoDS_Edge edg = TopoDS::Edge( edgemap.FindKey(i) );
      if(!edg.IsNull()) blend.Add(1, edg);
    }
  di << "All Edges added !  Now Building the Blend ... \n";
  di<<" \n";
  blend.Build();

  //DBRep::Set ( argv[1], fuse321 );
  DBRep::Set ( argv[1], blend );
  DBRep::Set ( argv[2], rs1 );
  DBRep::Set ( argv[3], rs2 );
  DBRep::Set ( argv[4], rs3 );
  DBRep::Set ( argv[5], fuse32 );
  DBRep::Set ( argv[6], fuse321 );
  DBRep::Set ( argv[7], aFuseUnif );

  return 0;
}

#include <Geom_SurfaceOfRevolution.hxx>
//=======================================================================
//function : isPeriodic
//purpose  :
//=======================================================================
static Standard_Integer isPeriodic(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  try
    {
    OCC_CATCH_SIGNALS
      // 1. Verify amount of arguments of the command
      if (argc < 2) { di << "isperiodic FAULTY. Use : isperiodic surfaceOfRevolution"; return 0;}
      // 2. Retrieve surface
      Handle(Geom_Surface) aSurf = DrawTrSurf::GetSurface(argv[1]);
      if(aSurf.IsNull()) {di << "isperiodic FAULTY. argument of command is not a surface"; return 0;}
      Handle(Geom_SurfaceOfRevolution) aRevolSurf = Handle(Geom_SurfaceOfRevolution)::DownCast(aSurf);
      if(aRevolSurf.IsNull()) {di << "isperiodic FAULTY. argument of command is not a surface of revolution"; return 0;}
      // 3. Verify whether entry surface is u-periodic and v-periodic
      if(aRevolSurf->IsUPeriodic()) {di << "Surface is u-periodic \n";} else {di << "Surface is not u-periodic \n";}
      if(aRevolSurf->IsVPeriodic()) {di << "Surface is v-periodic \n";} else {di << "Surface is not v-periodic \n";}
    }
  catch (Standard_Failure) {di << "isperiodic Exception \n" ;return 0;}

  return 0;
}

#include <Precision.hxx>
#include <Extrema_ExtPS.hxx>
#include <GeomAdaptor_Surface.hxx>
//=======================================================================
//function : OCC486
//purpose  :
//=======================================================================
static Standard_Integer OCC486(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  try
    {
    OCC_CATCH_SIGNALS
      if (argc < 2) { di << "OCC486 FAULTY. Use : OCC486 surf x y z du dv"; return 1;}
      
      Standard_Real du = 0;
      Standard_Real dv = 0;

      Handle(Geom_Surface) GS;
      GS = DrawTrSurf::GetSurface(argv[1]);
      if (GS.IsNull()) { di << "OCC486 FAULTY. Null surface /n";return 1;}
      gp_Pnt P3D( Draw::Atof(argv[2]),Draw::Atof(argv[3]),Draw::Atof(argv[4]) );

      Standard_Real Tol = Precision::PConfusion();
      Extrema_ExtPS myExtPS;
      if (argc > 5) du = Draw::Atof(argv[5]);
      if (argc > 6) dv = Draw::Atof(argv[6]);

      Standard_Real uf, ul, vf, vl;
      GS->Bounds(uf, ul, vf, vl);

      GeomAdaptor_Surface aSurf(GS);
      myExtPS.Initialize (aSurf, uf-du, ul+du, vf-dv, vl+dv, Tol, Tol );
      myExtPS.Perform ( P3D );
      Standard_Integer nPSurf = ( myExtPS.IsDone() ? myExtPS.NbExt() : 0 );

      if ( nPSurf > 0 )
      {
        //Standard_Real distMin = myExtPS.Value ( 1 );
        Standard_Real distMin = myExtPS.SquareDistance ( 1 );
        Standard_Integer indMin=1;
        for (Standard_Integer sol = 2; sol <= nPSurf ; sol++)
        {
          //Standard_Real dist = myExtPS.Value(sol);
          Standard_Real dist = myExtPS.SquareDistance(sol);
          if ( distMin > dist )
          {
            distMin = dist;
            indMin = sol;
          }
        }
        distMin = sqrt(distMin);
        Standard_Real S, T;
        myExtPS.Point(indMin).Parameter ( S, T );
        gp_Pnt aCheckPnt = aSurf.Value( S, T );
        Standard_Real aCheckDist = P3D.Distance(aCheckPnt);
        di << "Solution is : U = "<< S << "\t V = "<< T << "\n";
        di << "Solution is : X = "<< aCheckPnt.X() << "\t Y = "<< aCheckPnt.Y() << "\t Z = "<< aCheckPnt.Z() << "\n";
        di << "ExtremaDistance = " << distMin  << "\n";
        di << "CheckDistance = " << aCheckDist << "\n";

        if(fabs(distMin - aCheckDist) < Precision::Confusion()) return 0;
        else return 1;
      }
      else return 1;
    }
  catch (Standard_Failure) {di << "OCC486 Exception \n" ;return 1;}
}

#include <GC_MakeArcOfCircle.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepOffsetAPI_DraftAngle.hxx>
//=======================================================================
//function : OCC712
//purpose  :
//=======================================================================
static Standard_Integer OCC712 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if (argc != 3) {
    di << "Usage : " << argv[0] << " draftAngle slabThick\n";
    return 1;
  }
  //NOTE: Case:1 - draftAngle = 15, slabThick = 30 --> Fails
  //      Case:2   draftAngle = 10, slabThick = 30 --> Ok
  //      Case:3   draftAngle = 10, slabThick = 40 --> Ok
  //
  //      --------------------------------------------------
  Standard_Real draftAngle = Draw::Atof(argv[1]);
  Standard_Real slabThick = Draw::Atof(argv[2]);

  Standard_Real f1 = 75;
  Standard_Real f2 = 35;

  gp_Pnt p1(-f2,  f2, 0);
  gp_Pnt p2(  0,  f1, 0);
  gp_Pnt p3( f2,  f2, 0);
  gp_Pnt p4( f1,   0, 0);
  gp_Pnt p5( f2, -f2, 0);
  gp_Pnt p6(  0, -f1, 0);
  gp_Pnt p7(-f2, -f2, 0);
  gp_Pnt p8(-f1,   0, 0);

  GC_MakeArcOfCircle arc1(p1, p2, p3);
  GC_MakeArcOfCircle arc2(p3, p4, p5);
  GC_MakeArcOfCircle arc3(p5, p6, p7);
  GC_MakeArcOfCircle arc4(p7, p8, p1);

  TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(arc1.Value());
  TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(arc2.Value());
  TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(arc3.Value());
  TopoDS_Edge e4 = BRepBuilderAPI_MakeEdge(arc4.Value());

  BRepBuilderAPI_MakeWire MW;
  MW.Add(e1);
  MW.Add(e2);
  MW.Add(e3);
  MW.Add(e4);

  if (!MW.IsDone())
    {
      di << "my Wire not done\n";
      return 1;
    }
  TopoDS_Wire W = MW.Wire();

  TopoDS_Face F = BRepBuilderAPI_MakeFace(W);
  if ( F.IsNull())
    {
      di << " Error in Face creation \n";
      return 1;
    }

  Handle(Geom_Surface) surf = BRep_Tool::Surface(F);
  Handle (Geom_Plane) P = Handle(Geom_Plane)::DownCast(surf);
  gp_Pln slabPln = P->Pln();

  try
    {
    OCC_CATCH_SIGNALS
      gp_Dir slabDir(0, 0, 1);
      gp_Vec slabVect(slabDir);
      slabVect *= slabThick;

      BRepPrimAPI_MakePrism slab(F, slabVect, Standard_True);
      if ( ! slab.IsDone() )
        {
	  di << " Error in Slab creation \n";
	  return 1;
        }

      TopoDS_Shape slabShape = slab.Shape();
      if (fabs(draftAngle) > 0.01)
        {
	  Standard_Real angle = draftAngle*(M_PI / 180.0);
	  BRepOffsetAPI_DraftAngle draftSlab(slabShape);

	  TopoDS_Shape fShape = slab.FirstShape();
	  TopoDS_Shape lShape = slab.LastShape();

	  TopExp_Explorer ex;
	  for(ex.Init(slabShape, TopAbs_FACE); ex.More(); ex.Next())
            {
	      TopoDS_Face aFace = TopoDS::Face(ex.Current());
	      if(aFace.IsSame(fShape) || aFace.IsSame(lShape)) continue;
	      draftSlab.Add(aFace, slabDir, angle, slabPln);
	      if (!draftSlab.AddDone())
                {
		  di << " Error in Add \n";
		  return 1;
                }
            }

	  di << "All Faces added. Building... \n"; //cout.flush();
	  draftSlab.Build();
	  di << "Build done...\n"; //cout.flush();
	  if (!draftSlab.IsDone())  //--------------> STEP:1
            {
	      di << " Error in Build \n";
	      return 1;
            }
	  slabShape = draftSlab.Shape();
	  DBRep::Set(argv[1], slabShape);
        }
    }
  catch ( Standard_Failure ) //--------------------> STEP:2
    {
      di << " Error in Draft Slab \n";
      return 1;
    }
  return 0;
}

//=======================================================================
//  performTriangulation
//=======================================================================

Standard_Integer performTriangulation (TopoDS_Shape aShape, Draw_Interpretor& di)
{
  int failed=0, total=0;
  TopExp_Explorer ExpFace;
  Handle(Poly_Triangulation) facing;

  for (ExpFace.Init(aShape,TopAbs_FACE); ExpFace.More(); ExpFace.Next())
    {
      total++;
      TopoDS_Face TopologicalFace = TopoDS::Face (ExpFace.Current());
      TopologicalFace.Orientation (TopAbs_FORWARD) ;
      BRepMesh_IncrementalMesh IM(TopologicalFace, 1);
      TopLoc_Location loc;
      facing = BRep_Tool::Triangulation(TopologicalFace, loc);
      di << "Face " << total << " - ";
      if (facing.IsNull())
        {
	  failed++;
	  di << "******************** FAILED during Triangulation \n";
        }
      else
        {
	  di << facing->NbTriangles() << " Triangles\n";
        }
    }
  di<<"Triangulation of all Faces Completed: \n\n";
  if (failed == 0) return 1;
  di<<"***************************************************\n";
  di<<"*******                                    ********\n";
  di<<"***** Triangulation FAILED for " << failed << " of " << total << " Faces ******\n";
  di<<"*******                                    ********\n";
  di<<"***************************************************\n";
  return 0;
}

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepAlgoAPI_Cut.hxx>
//=======================================================================
//function : OCC822_1
//purpose  :
//=======================================================================
static Standard_Integer OCC822_1 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {

  if(argc != 4) {
    di << "Usage : " << argv[0] << " name1 name2 result\n";
    return 1;
  }

  int index = 1;

  gp_Pnt P1(0, 0, 0);
  gp_Dir D1(0, 0, 1);
  gp_Ax2 A1(P1,D1);

  BRepPrimAPI_MakeCylinder cylMakerIn(A1, 40, 110);
  BRepPrimAPI_MakeCylinder cylMakerOut(A1, 50, 100);
  TopoDS_Shape cylIn = cylMakerIn.Shape();
  TopoDS_Shape cylOut = cylMakerOut.Shape();

  gp_Pnt P2(0, 0, 0);
  gp_Dir D2(0, 0, -1);
  gp_Ax2 A2(P2,D2);

  BRepPrimAPI_MakeCone conMakerIn(A2, 40, 60, 110);
  BRepPrimAPI_MakeCone conMakerOut(A2, 50, 70, 100);
  TopoDS_Shape conIn = conMakerIn.Shape();
  TopoDS_Shape conOut = conMakerOut.Shape();

  di << "All primitives created.....  Creating Boolean\n";

  try
  {
    OCC_CATCH_SIGNALS

    di << "theIn = BRepAlgoAPI_Fuse(cylIn, conIn)\n";
    di << "theOut = BRepAlgoAPI_Fuse(cylOut, conOut)\n";
    di << "theRes = BRepAlgoAPI_Cut(theOut, theIn)\n";
    TopoDS_Shape theIn = BRepAlgoAPI_Fuse(cylIn, conIn).Shape();
    TopoDS_Shape theOut = BRepAlgoAPI_Fuse(cylOut, conOut).Shape();
    TopoDS_Shape theRes = BRepAlgoAPI_Cut(theOut, theIn).Shape();

    if (index < argc) DBRep::Set(argv[index++], theIn);
    if (index < argc) DBRep::Set(argv[index++], theOut);
    if (index < argc) DBRep::Set(argv[index++], theRes);
    di << "Booleans Created !    Triangulating !\n";

    performTriangulation(theRes, di);
  }
  catch ( Standard_Failure )
  {
    di << "*********************************************************\n";
    di << "*****                                              ******\n";
    di << "***** Standard_Failure : Exception in Shoe Function *****\n";
    di << "*****                                              ******\n";
    di << "*********************************************************\n";
    return 1;
  }
  return 0;

}

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>

//=======================================================================
//  OCC822_2
//=======================================================================

static Standard_Integer OCC822_2 (Draw_Interpretor& di,Standard_Integer argc, const char ** argv)
{
  if(argc != 4) {
    di << "Usage : " << argv[0] << " name1 name2 result\n";
    return 1;
  }

  int index = 1;

  gp_Dir xDir(1, 0, 0);
  gp_Dir zDir(0, 0, 1);
  gp_Pnt cen1(0, 0, 0);
  gp_Ax2 cor1(cen1, zDir, xDir);
  BRepPrimAPI_MakeBox boxMaker(cor1, 100, 100, 100);
  TopoDS_Shape box = boxMaker.Shape();
  if (index < argc) DBRep::Set(argv[index++], box);

  BRepPrimAPI_MakeSphere sphereMaker(gp_Pnt(100.0, 50.0, 50.0), 25.0);
  TopoDS_Shape sph = sphereMaker.Shape();
  if (index < argc) DBRep::Set(argv[index++], sph);

  di << "All primitives created.....  Creating Cut Objects\n";

  try
  {
    OCC_CATCH_SIGNALS

    di << "fuse = BRepAlgoAPI_Fuse(box, sph)\n";
    TopoDS_Shape fuse = BRepAlgoAPI_Fuse(box, sph).Shape();

    if (index < argc) DBRep::Set(argv[index++], fuse);
    di << "Object Created !   Now Triangulating !";

    performTriangulation(fuse, di);
  }
  catch ( Standard_Failure )
  {
    di << "*********************************************************\n";
    di << "*****                                              ******\n";
    di << "***** Standard_Failure : Exception in HSP Function ******\n";
    di << "*****                                              ******\n";
    di << "*********************************************************\n";
    return 1;
  }

  return 0;
}

//=======================================================================
//  OCC823
//=======================================================================

static Standard_Integer OCC823 (Draw_Interpretor& di,Standard_Integer argc, const char ** argv)
{
  if(argc != 4) {
    di << "Usage : " << argv[0] << " name1 name2 result\n";
    return 1;
  }

  int index = 1;
  Standard_Real size = 0.001;

  gp_Pnt P1(40, 50, 0);
  gp_Dir D1(100, 0, 0);
  gp_Ax2 A1(P1,D1);
  BRepPrimAPI_MakeCylinder mkCyl1(A1, 20, 100);
  TopoDS_Shape cyl1 = mkCyl1.Shape();
  if (index < argc) DBRep::Set(argv[index++], cyl1);

  gp_Pnt P2(100, 50, size);
  gp_Dir D2(0, size, 80);
  gp_Ax2 A2(P2,D2);
  BRepPrimAPI_MakeCylinder mkCyl2(A2, 20, 80);
  TopoDS_Shape cyl2 = mkCyl2.Shape();
  if (index < argc) DBRep::Set(argv[index++], cyl2);

  di << "All primitives created.....  Creating Boolean\n";

  try
  {
    OCC_CATCH_SIGNALS

    di << "fuse = BRepAlgoAPI_Fuse(cyl2, cyl1)\n";
    TopoDS_Shape fuse = BRepAlgoAPI_Fuse(cyl2, cyl1).Shape();

    if (index < argc) DBRep::Set(argv[index++], fuse);
    di << "Fuse Created !    Triangulating !\n";

    performTriangulation(fuse, di);
  }
  catch (Standard_Failure)
  {
    di << "*********************************************************\n";
    di << "*****                                              ******\n";
    di << "***** Standard_Failure : Exception in TEE Function ******\n";
    di << "*****                                              ******\n";
    di << "*********************************************************\n";
    return 1;
  }
  return 0;
}

//=======================================================================
//  OCC824
//=======================================================================

static Standard_Integer OCC824 (Draw_Interpretor& di,Standard_Integer argc, const char ** argv)
{
  if(argc != 4) {
    di << "Usage : " << argv[0] << " name1 name2 result\n";
    return 1;
  }

  int index = 1;

  gp_Pnt P1(100, 0, 0);
  gp_Dir D1(-1, 0, 0);
  gp_Ax2 A1(P1,D1);
  BRepPrimAPI_MakeCylinder mkCyl(A1, 20, 100);
  TopoDS_Shape cyl = mkCyl.Shape();
  if (index < argc) DBRep::Set(argv[index++], cyl);

  BRepPrimAPI_MakeSphere sphere(P1, 20.0);
  TopoDS_Shape sph = sphere.Shape();
  if (index < argc) DBRep::Set(argv[index++], sph);

  di << "All primitives created.....  Creating Boolean\n";

  try
  {
    OCC_CATCH_SIGNALS

    di << "fuse = BRepAlgoAPI_Fuse(cyl, sph)\n";
    TopoDS_Shape fuse = BRepAlgoAPI_Fuse(cyl, sph).Shape();

    di << "Fuse Created !    Triangulating !\n";
    if (index < argc) DBRep::Set(argv[index++], fuse);

    performTriangulation(fuse, di);
  }
  catch (Standard_Failure)
  {
    di << "*********************************************************\n";
    di << "*****                                              ******\n";
    di << "***** Standard_Failure : Exception in YOU Function ******\n";
    di << "*****                                              ******\n";
    di << "*********************************************************\n";
    return 1;
  }
  return 0;
}

#include <TColgp_Array2OfPnt.hxx>
#include <GeomConvert.hxx>
#include <Geom_BezierSurface.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>

//=======================================================================
//  OCC825
//=======================================================================

static Standard_Integer OCC825 (Draw_Interpretor& di,Standard_Integer argc, const char ** argv)
{
  if(argc != 6) {
    di << "Usage : " << argv[0] << " name1 name2 name3 result1 result2\n";
    return 1;
  }

  int index = 1;

  Standard_Real size = 50;
  TColgp_Array2OfPnt poles(1, 2, 1, 2);

  poles(1, 1).SetCoord(-size, 0, -size);
  poles(1, 2).SetCoord(-size, 0,  size);
  poles(2, 1).SetCoord( size, 0, -size);
  poles(2, 2).SetCoord( size, 0,  size);

  Handle(Geom_BezierSurface) BezSurf = new Geom_BezierSurface(poles);
  Handle(Geom_BSplineSurface) BSpSurf = GeomConvert::SurfaceToBSplineSurface(BezSurf);
  BRepBuilderAPI_MakeFace faceMaker(BSpSurf, Precision::Confusion());
  TopoDS_Face face = faceMaker.Face();

  gp_Pnt pnt(0, size, 0);
  BRepPrimAPI_MakeHalfSpace *hSpace = new BRepPrimAPI_MakeHalfSpace(face,pnt);
  TopoDS_Shape hsp = hSpace->Solid();
  if (index < argc) DBRep::Set(argv[index++], hsp);

  BRepPrimAPI_MakeSphere sphere1(gp_Pnt(0.0, 0.0, 0.0), 25.0);
  TopoDS_Shape sph1 = sphere1.Shape();
  if (index < argc) DBRep::Set(argv[index++], sph1);

  BRepPrimAPI_MakeSphere sphere2(gp_Pnt(0.0, 0.00001, 0.0), 25.0);
  TopoDS_Shape sph2 = sphere2.Shape();
  if (index < argc) DBRep::Set(argv[index++], sph2);

  di << "All primitives created.....  Creating Cut Objects\n";

  try
  {
    OCC_CATCH_SIGNALS

    di << "cut1 = BRepAlgoAPI_Cut(sph1, hsp)\n";
    TopoDS_Shape cut1 = BRepAlgoAPI_Cut(sph1, hsp).Shape();

    if (index < argc) DBRep::Set(argv[index++], cut1);
    di << "CUT 1 Created !   " ;


    di << "cut2 = BRepAlgoAPI_Cut(sph2, hsp)\n";
    TopoDS_Shape cut2 = BRepAlgoAPI_Cut(sph2, hsp).Shape();

    if (index < argc) DBRep::Set(argv[index++], cut2);
    di << "CUT 2 Created !\n\n";

    GProp_GProps G;
    BRepGProp::VolumeProperties(cut1, G);
    di << "CUT 1 Mass = " << G.Mass() << "\n\n";
    BRepGProp::VolumeProperties(cut2, G);
    di << "CUT 2 Mass = " << G.Mass() << "\n\n";

    di << "Trianglating Faces of CUT 1 .....\n";
    performTriangulation(cut1, di);

    di << "Trianglating Faces of CUT 2 .....\n";
    performTriangulation(cut2, di);
  }
  catch (Standard_Failure)
  {
    di << "*********************************************************\n";
    di << "*****                                              ******\n";
    di << "***** Standard_Failure : Exception in HSP Function ******\n";
    di << "*****                                              ******\n";
    di << "*********************************************************\n";
    return 1;
  }

  di << "*************************************************************\n";
  di << " CUT 1 and CUT 2 gives entirely different results during\n";
  di << " mass computation and face triangulation, eventhough the\n";
  di << " two spheres are located more or less at the same position.\n";
  di << "*************************************************************\n";

  return 0;
}

//=======================================================================
//  OCC826
//=======================================================================

static Standard_Integer OCC826 (Draw_Interpretor& di,Standard_Integer argc, const char ** argv)
{
  if(argc != 4) {
    di << "Usage : " << argv[0] << " name1 name2 result\n";
    return 1;
  }

  int index = 1;

  Standard_Real x1 = 181.82808;
  Standard_Real x2 = 202.39390;
  Standard_Real y1 = 31.011970;
  Standard_Real y2 = 123.06856;

  BRepBuilderAPI_MakePolygon W1;
  W1.Add(gp_Pnt(x1, y1, 0));
  W1.Add(gp_Pnt(x2, y1, 0));
  W1.Add(gp_Pnt(x2, y2, 0));
  W1.Add(gp_Pnt(x1, y2, 0));
  W1.Add(gp_Pnt(x1, y1, 0));

  Standard_Boolean myFalse = Standard_False;
  TopoDS_Face F1 = BRepBuilderAPI_MakeFace(W1.Wire(), myFalse);

  gp_Pnt P1(0, 0, 0);
  gp_Dir D1(0, 30, 0);
  gp_Ax1 A1(P1,D1);
  Standard_Real angle1 = 360 * (M_PI / 180.0);
  TopoDS_Shape rev = BRepPrimAPI_MakeRevol(F1, A1, angle1);
  if (index < argc) DBRep::Set(argv[index++], rev);

  BRepPrimAPI_MakeSphere sphere(gp_Pnt(166.373, 77.0402, 96.0555), 23.218586);
  TopoDS_Shape sph = sphere.Shape();
  if (index < argc) DBRep::Set(argv[index++], sph);

  di << "All primitives created.....  Creating Boolean\n";

  try
  {
    OCC_CATCH_SIGNALS

    di << "fuse = BRepAlgoAPI_Fuse(rev, sph)\n";
    TopoDS_Shape fuse = BRepAlgoAPI_Fuse(rev, sph).Shape();

    if (index < argc) DBRep::Set(argv[index++], fuse);
    di << "Fuse Created !   Triangulating !\n";
    performTriangulation(fuse, di);
  }
  catch (Standard_Failure)
  {
    di << "*********************************************************\n";
    di << "*****                                              ******\n";
    di << "***** Standard_Failure : Exception in SPH Function ******\n";
    di << "*****                                              ******\n";
    di << "*********************************************************\n";
    return 1;
  }
  return 0;
}

#include <BRepPrimAPI_MakeTorus.hxx>
//=======================================================================
//  OCC827
//=======================================================================

static Standard_Integer OCC827 (Draw_Interpretor& di,Standard_Integer argc, const char ** argv)
{
  if(argc != 6) {
    di << "Usage : " << argv[0] << " name1 name2 name3 result1 result2\n";
    return 1;
  }

  int index = 1;

  BRepBuilderAPI_MakePolygon W1;
  W1.Add(gp_Pnt(10, 0, 0));
  W1.Add(gp_Pnt(20, 0, 0));
  W1.Add(gp_Pnt(20, 0, 50));
  W1.Add(gp_Pnt(10, 0, 50));
  W1.Add(gp_Pnt(10, 0, 0));

  Standard_Boolean myFalse = Standard_False;
  TopoDS_Face F1 = BRepBuilderAPI_MakeFace(W1.Wire(), myFalse);

  gp_Pnt P1(0, 0, 0);
  gp_Dir D1(0, 0, 30);
  gp_Ax1 A1(P1,D1);
  Standard_Real angle1 = 360 * (M_PI / 180.0);
  TopoDS_Shape rev = BRepPrimAPI_MakeRevol(F1, A1, angle1);
  if (index < argc) DBRep::Set(argv[index++], rev);

  gp_Pnt P2(0, 0, 50);
  gp_Dir D2(0, 0, 30);
  gp_Ax2 A2(P2,D2);
  Standard_Real majRad = 15;
  Standard_Real minRad = 5;
  BRepPrimAPI_MakeTorus Torus1(A2, majRad, minRad);
  TopoDS_Shape tor1 = Torus1.Shape();
  if (index < argc) DBRep::Set(argv[index++], tor1);

  gp_Pnt P3(0, 0, 10);
  gp_Dir D3(0, 0, 30);
  gp_Ax2 A3(P3,D3);
  BRepPrimAPI_MakeTorus Torus2(A3, majRad, minRad);
  TopoDS_Shape tor2 = Torus2.Shape();
  if (index < argc) DBRep::Set(argv[index++], tor2);

  di << "All primitives created.....  Creating Boolean\n";

  try
  {
    OCC_CATCH_SIGNALS

    di << "Fuse1 = BRepAlgoAPI_Fuse(tor1, rev)\n";
    TopoDS_Shape fuse1 = BRepAlgoAPI_Fuse(tor1, rev).Shape();

    if (index < argc) DBRep::Set(argv[index++], fuse1);
    di << "Fuse1 Created !    Creating Fuse 2\n";

    di << "Fuse2 = BRepAlgoAPI_Fuse(tor2, fuse1)\n";
    TopoDS_Shape fuse2 = BRepAlgoAPI_Fuse(tor2, fuse1).Shape();

    if (index < argc) DBRep::Set(argv[index++], fuse2);
    di << "Fuse2 Created !    Triangulating !\n";

    performTriangulation(fuse2, di);
  }
  catch (Standard_Failure)
  {
    di << "*********************************************************\n";
    di << "*****                                              ******\n";
    di << "***** Standard_Failure : Exception in REV Function ******\n";
    di << "*****                                              ******\n";
    di << "*********************************************************\n";
    return 1;
  }
  return 0;
}

//=======================================================================
//  performBlend
//=======================================================================

int performBlend (TopoDS_Shape aShape, Standard_Real rad, TopoDS_Shape& bShape, Draw_Interpretor& di)
{
  Standard_Integer status = 0;
  TopoDS_Shape newShape;
  TopTools_IndexedDataMapOfShapeListOfShape edgemap;
  TopExp::MapShapesAndAncestors(aShape,TopAbs_EDGE,TopAbs_SOLID,edgemap);
  di << "Blending All Edges: No. of Edges: " << edgemap.Extent() << "\n";
  ChFi3d_FilletShape FShape = ChFi3d_Rational;
  BRepFilletAPI_MakeFillet blend(aShape,FShape);
  for(int i = 1; i <= edgemap.Extent(); i++)
    {
      TopoDS_Edge edg = TopoDS::Edge( edgemap.FindKey(i) );
      if(!edg.IsNull()) blend.Add(rad, edg);
    }

  try
    {
    OCC_CATCH_SIGNALS
      blend.Build();
      if(!blend.HasResult() || blend.Shape().IsNull()) {
        status = 1;
      }
    }
  catch ( Standard_Failure )
    {
      status = 1;
    }
  if(status) {
      di<<"*******************************************************\n";
      di<<"******                                          *******\n";
      di<<"****** Blending Failed (Radius = " << rad << ") *******\n";
      di<<"******                                          *******\n";
      di<<"*******************************************************\n";
      return 1;
  } else {
    di<<"Blending successfully performed on all Edges: \n\n";
  }
  bShape = blend.Shape();
  return 0;
}

#include <GC_MakeSegment.hxx>
//=======================================================================
//  OCC828
//=======================================================================

static Standard_Integer OCC828 (Draw_Interpretor& di,Standard_Integer argc, const char ** argv)
{
  if(argc != 2) {
    di << "Usage : " << argv[0] << " shape\n";
  }
  int index = 1;

  Standard_Real slabThick = 111;

  gp_Pnt p11(-27.598139, -7.0408573, 0.0);
  gp_Pnt p12(-28.483755, -17.487625, 0.0);
  gp_Pnt p13(-19.555504, -22.983587, 0.0);
  GC_MakeArcOfCircle arc1(p11, p12, p13);

  gp_Pnt p21(12.125083, -22.983587, 0.0);
  gp_Pnt p22(21.1572, -17.27554, 0.0);
  gp_Pnt p23(19.878168, -6.6677585, 0.0);
  GC_MakeArcOfCircle arc2(p21, p22, p23);

  gp_Pnt p31(3.265825, 13.724955, 0.0);
  gp_Pnt p32(-4.7233953, 17.406338, 0.0);
  gp_Pnt p33(-12.529893, 13.351856, 0.0);
  GC_MakeArcOfCircle arc3(p31, p32, p33);

  GC_MakeSegment ln1(p13, p21);
  GC_MakeSegment ln2(p23, p31);
  GC_MakeSegment ln3(p33, p11);

  TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(arc1.Value());
  TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(arc2.Value());
  TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(arc3.Value());

  TopoDS_Edge e4 = BRepBuilderAPI_MakeEdge(ln1.Value());
  TopoDS_Edge e5 = BRepBuilderAPI_MakeEdge(ln2.Value());
  TopoDS_Edge e6 = BRepBuilderAPI_MakeEdge(ln3.Value());

  BRepBuilderAPI_MakeWire MW;
  MW.Add(e1);
  MW.Add(e4);
  MW.Add(e2);
  MW.Add(e5);
  MW.Add(e3);
  MW.Add(e6);

  if (!MW.IsDone())
    {
      di << "my Wire not done\n";
      return 1;
    }

  TopoDS_Wire W = MW.Wire();
  TopoDS_Face F = BRepBuilderAPI_MakeFace(W);
  if ( F.IsNull())
    {
      di << " Error in Face creation \n";
      return 1;
    }

  try
    {
    OCC_CATCH_SIGNALS
      gp_Dir slabDir(0, 0, 1);
      gp_Vec slabVect(slabDir);
      slabVect *= slabThick;

      BRepPrimAPI_MakePrism slab(F, slabVect, Standard_True);
      if ( ! slab.IsDone() )
        {
	  di << " Error in Slab creation \n";
	  return 1;
        }
      if (index < argc) DBRep::Set(argv[index++], slab.Shape());

//       cout << "Slab Successfully Created !   Now Blending ..." << endl;
//       TopoDS_Shape aShape;
//       int ret = performBlend(slab.Shape(), radius, aShape);
//       if (ret) return 1;
//       if (index < argc) DBRep::Set(argv[index++], aShape);

//       cout << "Blending Successfully Done !   Now Triangulating ..." << endl;
//       performTriangulation(aShape);
    }
  catch ( Standard_Failure )
    {
      di << " Error in Draft Slab \n";
      return 1;
    }
  return 0;
}

void QABugs::Commands_10(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add ("OCC426", "OCC426 shape1 shape2 shape3 shape4 shape5 shape6 shape7", __FILE__, OCC426, group);

  theCommands.Add("isperiodic", "Use : isperiodic surfaceOfRevolution", __FILE__, isPeriodic, group);
  theCommands.Add("OCC486", "Use : OCC486 surf x y z du dv ", __FILE__, OCC486, group);
  theCommands.Add("OCC712", "OCC712 draftAngle slabThick", __FILE__, OCC712, group);
  theCommands.Add("OCC822_1", "OCC822_1 name1 name2 result", __FILE__,OCC822_1, group);
  theCommands.Add("OCC822_2", "OCC822_2 name1 name2 result", __FILE__,OCC822_2, group);
  theCommands.Add("OCC823", "OCC823 name1 name2 result", __FILE__,OCC823, group);
  theCommands.Add("OCC824", "OCC824 name1 name2 result", __FILE__,OCC824, group);
  theCommands.Add("OCC825", "OCC825 name1 name2 name3 name4 name5", __FILE__,OCC825, group);
  theCommands.Add("OCC826", "OCC826 name1 name2 result", __FILE__,OCC826, group);
  theCommands.Add("OCC827", "OCC827 name1 name2 name3 result1 result2", __FILE__,OCC827, group);
  theCommands.Add("OCC828", "OCC828 redius shape result ", __FILE__,OCC828, group);

  return;
}
