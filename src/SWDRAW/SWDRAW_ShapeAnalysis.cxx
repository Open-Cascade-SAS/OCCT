// Created on: 1999-03-09
// Created by: data exchange team
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

// sln 19.11.2001. Bug 2: Correction of output of 'statshape' draw function.

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepGProp.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_FreeBoundData.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeAnalysis_FreeBoundsProperties.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_FreeBounds.hxx>
#include <SWDRAW.hxx>
#include <SWDRAW_ShapeAnalysis.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>

#include <stdio.h>
static Standard_Integer tolerance
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) { di<<"myshape  : analyses a shape\n"
		    <<  "myshape [mode] valmin : sub-shapes over valmin\n"
		      <<"myshape [mode] valmin valmax : between valmin and valmax\n"
			<<"myshape [mode] 0 valmax : below valmax\n"
			  <<"  more : all shapes(D)  v vertices  e edges  f faces  c combined(faces)\n";
		  return (argc < 2 ? 0 : 1 /* Error */);  }
  Standard_CString arg1 = argv[1];
  TopoDS_Shape Shape = DBRep::Get(arg1);
  if (Shape.IsNull()) { di<<"Shape unknown : "<<arg1<<"\n"; return 1 /* Error */; }

  ShapeAnalysis_ShapeTolerance sat;
  if (argc == 2) {
    sat.InitTolerance();
    sat.AddTolerance (Shape);
    di<<"Tolerance MAX="<<sat.GlobalTolerance(1)<<" AVG="<<sat.GlobalTolerance(0)<<" MIN="<<sat.GlobalTolerance(-1)<<"\n";
    sat.InitTolerance();
    sat.AddTolerance (Shape,TopAbs_FACE);
    di<<"FACE    : MAX="<<sat.GlobalTolerance(1)<<" AVG="<<sat.GlobalTolerance(0)<<" MIN="<<sat.GlobalTolerance(-1)<<"\n";
    sat.InitTolerance();
    sat.AddTolerance (Shape,TopAbs_EDGE);
    di<<"EDGE    : MAX="<<sat.GlobalTolerance(1)<<" AVG="<<sat.GlobalTolerance(0)<<" MIN="<<sat.GlobalTolerance(-1)<<"\n";
    sat.InitTolerance();
    sat.AddTolerance (Shape,TopAbs_VERTEX);
    di<<"VERTEX  : MAX="<<sat.GlobalTolerance(1)<<" AVG="<<sat.GlobalTolerance(0)<<" MIN="<<sat.GlobalTolerance(-1)<<"\n";
  } else {
//  Analyse plus fouillee
    Standard_Integer nextarg = 2;
    char opt = argv[2][0];
    TopAbs_ShapeEnum type = TopAbs_SHAPE;
    if (opt == 'a')  nextarg = 3;
    else if (opt == 'c') { nextarg = 3; type = TopAbs_SHELL;  }
    else if (opt == 'e') { nextarg = 3; type = TopAbs_EDGE;   }
    else if (opt == 'f') { nextarg = 3; type = TopAbs_FACE;   }
    else if (opt == 'v') { nextarg = 3; type = TopAbs_VERTEX; }
    if (nextarg < 3) opt = 'a';

    Standard_Real tol1= 0., tol2= 0.;
    if (nextarg < argc)   tol1 = Draw::Atof (argv[nextarg]);
    if (nextarg < argc-1) tol2 = Draw::Atof (argv[nextarg+1]);
//    Standard_Real tol = Draw::Atof (argv[2]);
    Handle(TopTools_HSequenceOfShape) list = sat.InTolerance (Shape,tol1,tol2,type);
    Standard_Integer i, nb = list->Length();
    switch (type) {
    case TopAbs_VERTEX : di<<"Analysing Vertices gives "; break;
    case TopAbs_EDGE   : di<<"Analysing Edges gives "; break;
    case TopAbs_FACE   : di<<"Analysing Faces gives "; break;
    case TopAbs_SHELL  : di<<"Analysing Shells,Faces+content gives "; break;
      default : di<<"Analysing all sub-shapes gives ";
    }
    if (tol1 == 0) di<<nb<<" Shapes below tol="<<tol2;
    else if (tol2 == 0) di<<nb<<" Shapes over tol="<<tol1;
    else di<<nb<<" Shapes between tol1="<<tol1<<" and tol2="<<tol2;
    if (nb == 1) di<<" , named tol_1";
    if (nb > 1) di<<" , named tol_1 to tol_"<<nb;
    di<<"\n";
    char nomsh[30];
    for (i = 1; i <= nb; i ++) {
      Sprintf (nomsh,"tol_%d",i);
      DBRep::Set (nomsh,list->Value(i));
    }
  }
  return 0;
}

static Standard_Integer projface
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) { di<<"Give FACE name and X Y [Z]\n"; return 1 /* Error */; }
  Standard_CString arg1 = argv[1];
  TopoDS_Shape Shape = DBRep::Get(arg1);
  if (Shape.IsNull()) { di<<"Shape unknown : "<<arg1<<"\n"; return 1 /* Error */; }
  if (Shape.ShapeType() != TopAbs_FACE) { di<<"Not a face\n"; return 1 /* Error */; }
  TopoDS_Face F = TopoDS::Face (Shape);
  Handle(Geom_Surface) thesurf = BRep_Tool::Surface (F);  // pas locface
//  On y va
  Standard_Real X,Y,Z,U,V;
  X = U = Draw::Atof (argv[2]);
  Y = V = Draw::Atof (argv[3]);
  if (argc > 4) {
    Z = Draw::Atof (argv[4]);
    gp_Pnt P3D (X,Y,Z);
    di<<" Point 3D X = "<<X<<"  Y = "<<Y<<"  Z = "<<Z<<"\n";
    Standard_Real uf, ul, vf, vl;
    thesurf->Bounds(uf, ul, vf, vl);

    if (Precision::IsInfinite(uf)) uf = -1000;
    if (Precision::IsInfinite(ul)) ul = 1000;
    if (Precision::IsInfinite(vf)) vf = -1000;
    if (Precision::IsInfinite(vl)) vl = 1000;
    Standard_Real du = Abs(ul-uf)/10;  Standard_Real dv = Abs(vl-vf)/10;

    GeomAPI_ProjectPointOnSurf proj(P3D, thesurf, uf-du, ul+du, vf-dv, vl+dv);
    Standard_Integer sol, nPSurf = proj.NbPoints();
    di<<" Found "<<nPSurf<<" Points\n";
    for (sol = 1; sol <= nPSurf; sol ++) {
      di<<"n0 "<<sol<<" Distance "<<proj.Distance(sol);
      proj.Parameters(sol, U,V);
      di<<"  U = "<<U<<"  V = "<<V<<"\n";
//  reprojection
      P3D = thesurf->Value (U,V);
      di<<"  => reproj  X = "<<P3D.X()<<"  Y = "<<P3D.Y()<<"  Z = "<<P3D.Z()<<"\n";
    }
//    Que donne ShapeTool ?
    P3D.SetCoord (X,Y,Z);
    Handle(ShapeAnalysis_Surface) su = new ShapeAnalysis_Surface(thesurf);
    gp_Pnt2d suval = su->ValueOfUV (P3D,BRep_Tool::Tolerance(F));
    suval.Coord(U,V);
    di<<"**  ShapeAnalysis_Surface gives  U = "<<U<<"  V = "<<V<<"\n";
    P3D = thesurf->Value(U,V);
      di<<"  => reproj  X = "<<P3D.X()<<"  Y = "<<P3D.Y()<<"  Z = "<<P3D.Z()<<"\n";

  } else {
    di<<" Point UV  U = "<<U<<"  V = "<<V<<"\n";
    gp_Pnt P3D = thesurf->Value(U,V);
      di<<"  =>   proj  X = "<<P3D.X()<<"  Y = "<<P3D.Y()<<"  Z = "<<P3D.Z()<<"\n";
  }
  return 0;
}

static Standard_Integer projcurve
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
//  admet une EDGE ou une CURVE
  if (argc < 5) { di<<"Give : EDGE X Y Z  or  CURVE3D X Y Z  or  CURVE3D first last X Y Z\n";  return 1 /* Error */; }
  const char * arg1 = argv[1];
  Standard_Integer i0 = 0;
  TopoDS_Shape Shape = DBRep::Get(arg1);

  Handle(Geom_Curve) C;  Standard_Real cf,cl;
  if (!Shape.IsNull()) {
    if (Shape.ShapeType() == TopAbs_EDGE) {
      TopoDS_Edge E = TopoDS::Edge(Shape);
      C = BRep_Tool::Curve (E,cf,cl);
      di<<"Edge "<<arg1<<" Params from "<<cf<<" to "<<cl<<"\n";
    } else {
      di<<"SHAPE "<<arg1<<" Not an EDGE\n"; return 1 /* Error */;
    }
  } else {
    const char* a1 = arg1;
    C = DrawTrSurf::GetCurve (a1);
    if (C.IsNull())
      {  di<<arg1<<" neither EDGE nor CURVE 3D\n"; return 1 /* Error */;  }
    cf = C->FirstParameter();  cl = C->LastParameter();
    if (argc >= 7) { cf = Draw::Atof (argv[2]);  cl = Draw::Atof (argv[3]); i0 = 2; }
    di<<"Curve 3D "<<arg1<<" Params from "<<cf<<" to "<<cl<<"\n";
  }

  Standard_Real X,Y,Z;
  X = Draw::Atof (argv[2+i0]);
  Y = Draw::Atof (argv[3+i0]);
  Z = Draw::Atof (argv[4+i0]);
  di<<"Precision (BRepBuilderAPI) : "<<BRepBuilderAPI::Precision()
    <<"  Projection : "<<X<<"  "<<Y<<"  "<<Z<<"\n";

  gp_Pnt P3D (X,Y,Z);
  gp_Pnt res; Standard_Real param,dist;

  dist = ShapeAnalysis_Curve().Project (C,P3D,BRepBuilderAPI::Precision(),res,param, cf,cl);
  res.Coord(X,Y,Z);
  di<<"Result : "<<X<<"  "<<Y<<"  "<<Z<<"\nParam = "<<param<<"  Gap = "<<dist<<"\n";
  return 0;
}
static Standard_Integer projpcurve
(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  //  admet une EDGE ou une CURVE
  if (argc < 7)
  {
    di << "Give : projpcurve edge face tol X Y Z [start_param]\n";
    return 1;
  }

  TopoDS_Edge aEdge = TopoDS::Edge(DBRep::Get(argv[1]));
  if (aEdge.IsNull())
  {
    di << "SHAPE " << argv[1] << " is not an EDGE\n"; return 1 /* Error */;
  }
  TopoDS_Face aFace = TopoDS::Face(DBRep::Get(argv[2]));
  if (aFace.IsNull())
  {
    di << "SHAPE " << argv[2] << " is not a FACE\n"; return 1 /* Error */;
  }

  Standard_Real aTol = Draw::Atof(argv[3]);


  Standard_Real X = Draw::Atof(argv[4]);
  Standard_Real Y = Draw::Atof(argv[5]);
  Standard_Real Z = Draw::Atof(argv[6]);

  gp_Pnt aP3D(X, Y, Z);

  Standard_Boolean IsStartPoint = Standard_False;
  Standard_Real startpar = 0.;
  if (argc > 7)
  {
    startpar = Draw::Atof(argv[7]);
    IsStartPoint = Standard_True;
  }

  Adaptor3d_CurveOnSurface aCOnS =
    Adaptor3d_CurveOnSurface(new BRepAdaptor_HCurve2d(BRepAdaptor_Curve2d(aEdge, aFace)),
    new BRepAdaptor_HSurface(BRepAdaptor_Surface(aFace, Standard_False)));

  gp_Pnt aPnt;
  Standard_Real aParam;
  ShapeAnalysis_Curve aTool;
  Standard_Real aDist = RealLast();
  if (IsStartPoint)
  {
    aDist = aTool.NextProject(startpar, aCOnS, aP3D, aTol, aPnt, aParam);
  }
  else
  {
    aDist = aTool.Project(aCOnS, aP3D, aTol, aPnt, aParam, Standard_False);
  }

  di << "Point:" << "\n" << aPnt.X() << " " << aPnt.Y() << " " << aPnt.Z() << "\n";
  di << "Param: " << aParam << "\n";
  di << "Dist: " << aDist << "\n";
  return 0;
}

static Standard_Integer anaface
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) { di<<"donner un nom de face\n"; return 1 /* Error */; }
  Standard_CString arg1 = argv[1];
  TopoDS_Shape Shape = DBRep::Get(arg1);
  if (Shape.IsNull()) { di<<arg1<<" inconnu\n"; return 1 /* Error */; }
//  if (Shape.ShapeType() != TopAbs_FACE) { di<<"Pas une FACE\n"; return 1 /* Error */; }

//  On regarde les wires, sont-ils bien fermes
  Standard_Integer nbw = 0, nbe = 0;
  Handle(Geom_Surface) surface;
  TopoDS_Face Face;
  if (Shape.ShapeType() == TopAbs_FACE) {
    Face = TopoDS::Face(Shape);
    surface = BRep_Tool::Surface (Face);  // pas locface
//    TopLoc_Location locface;
    di<<"Face, surface type = "<<surface->DynamicType()->Name()<<" Tol="<<BRep_Tool::Tolerance(Face);
    if (Face.Orientation() == TopAbs_REVERSED) {
      di<<" (REV)";
//      Face.Orientation(TopAbs_FORWARD);
    }
    di<<"\n";
  }
  else { di<<"Analyse Wires, Ignore Faces\n"; }
  Standard_Boolean iasurf = !surface.IsNull();
//:sw  if (!Face.IsNull()) STW.SetFace (Face);
//:sw  else STW.SetPrecision (BRepBuilderAPI::Precision());

  for (TopExp_Explorer exp(Shape,TopAbs_WIRE); exp.More(); exp.Next()) {
    TopoDS_Wire Wire = TopoDS::Wire   (exp.Current());  nbw ++;  nbe = 0;
    di<<"WIRE "<<nbw;
    if (Wire.Orientation() == TopAbs_FORWARD) di<<" (FWD)\n";
    else di<<" (REV)\n";
    gp_Pnt   fin,debut;
    gp_Pnt2d finuv,debuv;  gp_XY baseuv;
    TopoDS_Vertex fv,lv;
    Standard_Real maxp3d = 0., maxuv = 0., maxvtx = 0.;
    Standard_Real df3d= 0.,dl3d= 0.,dvtx= 0.,duv= 0.;
    Standard_Boolean iaw2d = iasurf;
    Standard_Real u1= 0.,u2= 0.,v1= 0.,v2= 0.,umin= 0.,umax= 0.,vmin= 0.,vmax= 0., totcross= 0.;

//:sw    STW.ClearWire();  STW.Load (Wire);
//    if (STW.Reorder()) di<<"Wire reordered to explore\n";
//    for (TopExp_Explorer exe(Wire.Oriented(TopAbs_FORWARD),TopAbs_EDGE); exe.More(); exe.Next()) {
//      TopoDS_Edge Edge = TopoDS::Edge (exe.Current());  nbe ++;
//:sw    for (nbe = 1; nbe <= STW.NbEdges(); nbe ++) {
//:sw      TopoDS_Edge Edge = STW.Edge(nbe);
    TopoDS_Iterator it (Wire); //:sw: ShapeTool_Wire -> TopoDS_Iterator (it is the same)
    for (; it.More(); it.Next()) {
      TopoDS_Edge Edge = TopoDS::Edge (it.Value());

      di<<"Wire "<<nbw<<", Edge "<<nbe;
      if (Edge.Orientation() == TopAbs_FORWARD) di<<" (FWD";
      else di<<" (REV";
      di<<" , Tol= "<<BRep_Tool::Tolerance (Edge)<<" )\n";
      Standard_Real f3d,l3d,f2d,l2d;
      Handle(Geom_Curve) curve3d = BRep_Tool::Curve (Edge,f3d,l3d);
      Handle(Geom2d_Curve) curve2d;
      if (iasurf) curve2d = BRep_Tool::CurveOnSurface(Edge,Face,f2d,l2d);
      Standard_Boolean ia2d = !curve2d.IsNull(); iaw2d |= ia2d;
      if (!ia2d) di<<"--  No PCurve\n";
      if (curve3d.IsNull()) di<<"-- no Curve 3d\n";

//      On va tacher de calculer les positions et les comparer
      gp_Pnt2d fuv,luv;
      if (Edge.Orientation() == TopAbs_FORWARD)
      {
        TopExp::Vertices (Edge, fv, lv);
        if (ia2d) BRep_Tool::UVPoints (Edge, Face, fuv, luv);
      }
      else
      {
        TopExp::Vertices (Edge, lv, fv);
        if (ia2d) BRep_Tool::UVPoints (Edge, Face, luv, fuv);
      }
      gp_Pnt fp = BRep_Tool::Pnt (fv);
      gp_Pnt lp = BRep_Tool::Pnt (lv);
      gp_Pnt fxyz, lxyz;
      if (ia2d)
      {
	surface->D0 (fuv.X(),fuv.Y(),fxyz);
	surface->D0 (luv.X(),luv.Y(),lxyz);
	df3d = fp.Distance    (fxyz); maxp3d = Max (maxp3d,df3d);
	dl3d = lp.Distance    (lxyz); maxp3d = Max (maxp3d,dl3d);
        if (nbe > 1)
        {
          duv = finuv.Distance (fuv);
          maxuv = Max (maxuv, duv);
        }
// et les min-max
	u1 = Min (fuv.X(),luv.X());  u2 = Max (fuv.X(),luv.X());
	v1 = Min (fuv.Y(),luv.Y());  v2 = Max (fuv.Y(),luv.Y());
	if (nbe == 1) { umin = u1; umax = u2; vmin = v1; vmax = v2; }
	else { umin = Min(umin,u1); umax = Max(umax,u2); vmin = Min(vmin,v1); vmax = Max(vmax,v2); }
// et la classification directe
	if (nbe == 1) { baseuv = fuv.XY(); totcross = 0; }
	else {
	  gp_XY buv1 = fuv.XY() - baseuv;
	  gp_XY buv2 = luv.XY() - baseuv;
	  totcross += buv2.Crossed(buv1);
	}
      }

//  Resultats ...
      if (nbe == 1)  {  debut = fxyz; debuv = fuv;  }
      else {
	duv  = finuv.Distance (fuv);  maxuv  = Max (maxuv, duv);
	dvtx = fin.Distance   (fxyz); maxvtx = Max (maxvtx,dvtx);
	di<<"   Fin("<<nbe-1<<")-Debut("<<nbe<<"): DISTANCE="<<dvtx;
	if (ia2d) di<<" DeltaUV="<<duv;
	di<<" Tol(Fin)="<<BRep_Tool::Tolerance(lv)<<"\n";
      }
      fin = lxyz;  finuv = luv;

      di<<"-- Deb : VTX="<<fp.X()<<" , "<<fp.Y()<<" , "<<fp.Z()
	<<"\n         XYZ="<<fxyz.X()<<" , "<<fxyz.Y()<<" , "<<fxyz.Z()
	<<"\n         UV="<<fuv.X()<<" , "<<fuv.Y()<<"  -- D.UV/3D="<<df3d<<"\n";
      di<<"-- Fin : VTX="<<lp.X()<<" , "<<lp.Y()<<" , "<<lp.Z()
	<<"\n         XYZ="<<lxyz.X()<<" , "<<lxyz.Y()<<" , "<<lxyz.Z()
	<<"\n         UV="<<luv.X()<<" , "<<luv.Y()<<"  -- D.UV/3D="<<dl3d<<"\n";
    }
    dvtx = fin.Distance   (debut); maxvtx = Max (maxvtx,dvtx);
    if (iaw2d)  {  duv  = finuv.Distance (debuv); maxuv  = Max (maxuv, duv);  }
    di<<"   Fin("<<nbe<<")-Debut(1): DISTANCE="<<dvtx;
    if (iaw2d) di <<" DeltaUV="<<duv;
    di<<" Tol(Fin)="<<BRep_Tool::Tolerance(lv)<<"\n";

    di<<"   Wire "<<nbw<<" Max :  Dist.Vertex="<<maxvtx;
    if (iaw2d) di<<"  Ecart UV/3D="<<maxp3d<<"  DeltaUV="<<maxuv;
    di<<"\n";
//  Min Max
    if (iaw2d) {
      di<<"TotCross="<<totcross;
      di<<"  UMin-Max:"<<umin<<" , "<<umax<<"  VMin-Max:"<<vmin<<" , "<<vmax<<"\n";
      Standard_Real difu = umax - umin, difv = vmax - vmin;
      GProp_GProps G;
      BRepGProp::SurfaceProperties(Face,G);
      if (G.Mass() > 0) di<<"GProps:Mass Out\n";
      else di<<"GProps:Mass In\n";
///  return (G.Mass() > 0);
      BRepTopAdaptor_FClass2d fcl (Face,BRep_Tool::Tolerance(Face));
      if (fcl.PerformInfinitePoint () == TopAbs_OUT) di<<"Classifier Infinite : Out\n";
      else di<<"Classifier Infinite : In\n";
      gp_Pnt2d pcl;
      pcl.SetCoord(umin-difu,vmin-difv);
      if (fcl.Perform (pcl) == TopAbs_OUT) di<<"Classifier UMin-VMin : Out\n";
      pcl.SetCoord(umin-difu,vmax+difv);
      if (fcl.Perform (pcl) == TopAbs_OUT) di<<"Classifier UMin-VMax : Out\n";
      pcl.SetCoord(umax+difu,vmin-difv);
      if (fcl.Perform (pcl) == TopAbs_OUT) di<<"Classifier UMax-VMin : Out\n";
      pcl.SetCoord(umax+difu,vmax+difv);
      if (fcl.Perform (pcl) == TopAbs_OUT) di<<"Classifier UMax-VMax : Out\n";
    }
  }
  if (ShapeAnalysis::IsOuterBound (Face)) di<<"ShapeAnalysis: Outer Bound\n";
  else di<<"ShapeAnalysis: Not Outer Bound\n";
  di<<" Total "<<nbw<<" Wire(s)\n";
  return 0;
}

static Standard_Integer XSHAPE_statshape(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) { di<<"ShapeName; + options : prefix for particular cases : what to get\n"; return 1 /* Error */; }
  Standard_CString arg1 = argv[1];
  Standard_CString arg2 = NULL;
  Standard_CString arg3 = NULL;
  if (argc > 2) arg2 = argv[2];
  if (argc > 3) arg3 = argv[3];
  TopoDS_Shape Shape = DBRep::Get(arg1);
  if (Shape.IsNull()) { di<<"Shape unknown : "<<arg1<<"\n"; return 1 /* Error */; }
  char nompart[100];
  Standard_Integer nb;

  ShapeAnalysis_ShapeContents analyzer;
  if(argc > 3) {
    analyzer.ModifyBigSplineMode()=(strstr("bigspl",arg3)!=NULL);
    analyzer.ModifyIndirectMode()=(strstr("indsur",arg3)!=NULL);
    analyzer.ModifyOffestSurfaceMode()=(strstr("ofsur",arg3)!=NULL);
    analyzer.ModifyTrimmed3dMode()=(strstr("trc3d",arg3)!=NULL);
    analyzer.ModifyOffsetCurveMode()=(strstr("ofcur",arg3)!=NULL);
    analyzer.ModifyTrimmed2dMode()=(strstr("trc2d",arg3)!=NULL);
  }
  analyzer.Perform(Shape);
  
  di<<"Count    Item\n-----    ----\n";
  nb = analyzer.NbEdges();
  if(nb>0) di<<nb<<"	 EDGE (Oriented)\n";
  nb = analyzer.NbSharedEdges();
  if(nb>0) di<<nb<<"	 EDGE (Shared)\n";
  nb = analyzer.NbFreeEdges();
  if(nb>0) di<<nb<<"	 EDGE (Free)\n";
  nb = analyzer.NbFaces();
  if(nb>0) di<<nb<<"	 FACE\n";
  nb = analyzer.NbFreeFaces();
  if(nb>0) di<<nb<<"	 FACE (Free)\n";
  nb = analyzer.NbFreeWires();
  if(nb>0) di<<nb<<"	 WIRE (Free)\n";
  nb = analyzer.NbShells();
  if(nb>0) di<<nb<<"	 SHELL\n";
  nb = analyzer.NbSolids();
  if(nb>0) di<<nb<<"	 SOLID\n";
  nb = analyzer.NbVertices();
  if(nb>0) di<<nb<<"	 VERTEX (Oriented)\n";
  nb = analyzer.NbSharedVertices();
  if(nb>0) di<<nb<<"	 VERTEX (Shared)\n";
  nb = analyzer.NbWires();
  if(nb>0) di<<nb<<"	 WIRE\n";
  nb = analyzer.NbFaceWithSevWires();
  if(nb>0) di<<nb<<"	Face with more than one wire\n";
  nb = analyzer.NbNoPCurve();
  if(nb>0) di<<nb<<"	No pcurve\n";
  nb = analyzer.NbSolidsWithVoids();
  if(nb>0) di<<nb<<"	SOLID with voids\n";
  nb = analyzer.NbWireWitnSeam();
  if(nb>0) di<<nb<<"	Wire(s) with one seam edge\n";
  nb = analyzer.NbWireWithSevSeams();
  if(nb>0) di<<nb<<"	Wire(s) with several seam edges\n";
  nb = analyzer.NbBigSplines();
  if(nb>0) di<<nb<<"	bigspl : BSpline > 8192 poles\n";
  nb = analyzer.NbBezierSurf();
  if(nb>0) di<<nb<<"	bezsur : BezierSurface\n";
  nb = analyzer.NbBSplibeSurf();
  if(nb>0) di<<nb<<"	bspsur : BSplineSurface\n";
  nb = analyzer.NbC0Curves();
  if(nb>0) di<<nb<<"	c0curv : Curve Only C0\n";
  nb = analyzer.NbC0Surfaces();
  if(nb>0) di<<nb<<"	c0surf : Surface Only C0\n";
  nb = analyzer.NbIndirectSurf();
  if(nb>0) di<<nb<<"	indsur : Indirect Surface\n";
  nb = analyzer.NbOffsetCurves();
  if(nb>0) di<<nb<<"	ofcur  : Offset Curve(s)\n";
  nb = analyzer.NbOffsetSurf();
  if(nb>0) di<<nb<<"	ofsur  : Offset Surface\n";
  nb = analyzer.NbTrimmedCurve2d();
  if(nb>0) di<<nb<<"	trc2d  : Trimmed Curve2d\n";
  nb = analyzer.NbTrimmedCurve3d();
  if(nb>0) di<<nb<<"	trc3d  : Trimmed Curve3d\n";
  nb = analyzer.NbTrimSurf();
  if(nb>0) di<<nb<<"	trimsu : RectangularTrimmedSurface\n";
  
  if(arg3 ==NULL) return 0;
  
  Handle(TopTools_HSequenceOfShape) sec;
  if(analyzer.ModifyBigSplineMode()) {
    sec = analyzer.BigSplineSec();
    for(Standard_Integer i = 1; i <= sec->Length(); i++) {
      Sprintf(nompart,"%s_bigspl_%d",arg2,i);
      DBRep::Set (nompart,sec->Value(i));
    }
  }
  if(analyzer.ModifyIndirectMode()) {
    sec = analyzer.IndirectSec();
    for(Standard_Integer i = 1; i <= sec->Length(); i++) {
      Sprintf(nompart,"%s_indsur_%d",arg2,i);
      DBRep::Set (nompart,sec->Value(i));
    }
  }
  if(analyzer.ModifyOffestSurfaceMode()) {
    sec = analyzer.OffsetSurfaceSec();
    for(Standard_Integer i = 1; i <= sec->Length(); i++) {
      Sprintf(nompart,"%s_ofsur_%d",arg2,i);
      DBRep::Set (nompart,sec->Value(i));
    }
  }
  if(analyzer.ModifyTrimmed3dMode()) {
    sec = analyzer.Trimmed3dSec();
    for(Standard_Integer i = 1; i <= sec->Length(); i++) {
      Sprintf(nompart,"%s_trc3d_%d",arg2,i);
      DBRep::Set (nompart,sec->Value(i));
    }
  }
  if(analyzer.ModifyOffsetCurveMode()) {
    sec = analyzer.OffsetCurveSec();
    for(Standard_Integer i = 1; i <= sec->Length(); i++) {
      Sprintf(nompart,"%s_ofcur_%d",arg2,i);
      DBRep::Set (nompart,sec->Value(i));
    }
  }
  if(analyzer.ModifyTrimmed2dMode()) {
    sec = analyzer.Trimmed2dSec();
    for(Standard_Integer i = 1; i <= sec->Length(); i++) {
      Sprintf(nompart,"%s_trc2d_%d",arg2,i);
      DBRep::Set (nompart,sec->Value(i));
    }
  }
  return 0;
}
    
static Standard_Integer XSHAPE_comptoledge
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if ( argc <2 ) {
    di << "Use:\n"
            "> comptol shape [nbpoints=371] [prefix]\n\n" 
	    "Computes real tolerance of edges in the shape \n"
            "as maximal deviation of 3d curve and pcurves.\n"
	    "Deviation is computed by nbpoints sample points (default is 371).\n"
	    "Gives the max, min and average value on all edges in the shape\n"
	    "If prefix is defined, edges with maximal real tolerance and\n"
	    "relation (and corresponding faces) will be saved with such names\n\n"; 
    return 0; 
  }

  TopoDS_Shape shape = DBRep::Get(argv[1]);
  if (shape.IsNull()) { di << "Shape name \"" << argv[1] << "\" is invalid\n"; return 1; }
  Standard_Integer nbpnts = 371;
  Standard_CString prefix = 0;
  if ( argc >2 ) {
    if ( IsDigit(argv[2][0]) ) {
      nbpnts = Draw::Atoi(argv[2]);
      if ( nbpnts <2 ) nbpnts = 2;
      if ( argc >3 ) prefix = argv[3];
    }
    else prefix = argv[2];
  }
  
  Standard_Integer num = 0;
  Standard_Real max=0, min=0, ave=0.;
  Standard_Real relmax=0, relmin=0, relave=0.;
  ShapeAnalysis_Edge sae;
  TopoDS_Edge edmax, edmaxrel;
  for ( TopExp_Explorer exp(shape,TopAbs_EDGE); exp.More(); exp.Next() ) {
    Standard_Real tol;
    TopoDS_Edge edge = TopoDS::Edge(exp.Current());
    sae.CheckSameParameter ( edge, tol, nbpnts );
    Standard_Real t = BRep_Tool::Tolerance(edge);
    Standard_Real rel = tol / ( t > Precision::Confusion() ? t : Precision::Confusion() );
    ave += tol;
    relave += rel;
    if ( ! num ) { max = min = tol; relmax = relmin = rel; edmax = edmaxrel = edge; }
    else {
      if ( max < tol ) { max = tol; edmax = edge; }
      if ( min > tol ) min = tol;
      if ( relmax < rel ) { relmax = rel; edmaxrel = edge; }
      if ( relmin > rel ) relmin = rel;
    }
    num++;
  }
  if ( ! num ) { di << "No edges found in the shape\n"; return 1; }
  
  di << "Edges tolerance computed by " << nbpnts << " points: \n"
          "MAX=" << max << " AVG=" << ave/num << " MIN=" << min << "\n"; 
  di << "Relation real tolerance / tolerance set in edge\n"
          "MAX=" << relmax << " AVG=" << relave/num << " MIN=" << relmin << "\n"; 
  if ( prefix && prefix[0] ) {
    char name[21];
    Sprintf ( name, "%.10s_edge_tol", prefix );
    DBRep::Set (name,edmax);
    di << "Edge with max tolerance saved to " << name;
    if ( edmax.IsSame ( edmaxrel ) ) di << "\n";
    else {
      Sprintf ( name, "%.10s_edge_rel", prefix );
      DBRep::Set (name,edmaxrel);
      di << "; edge with max relation saved to " << name << "\n";
    }
    Standard_Integer num1=0;
    for ( TopExp_Explorer fac(shape,TopAbs_FACE); fac.More(); fac.Next() ) {
      TopoDS_Face face = TopoDS::Face ( fac.Current() );
      for ( TopExp_Explorer ed(face,TopAbs_EDGE); ed.More(); ed.Next() ) {
	TopoDS_Edge edge = TopoDS::Edge ( ed.Current() );
	if ( edge.IsSame ( edmax ) || edge.IsSame ( edmaxrel ) ) {
	  if ( ! num1 ) di << "Concerned faces saved to shapes ";
	  Sprintf ( name, "%.10s_%d", prefix, num1+1 );
	  DBRep::Set (name,face);
	  //cout << ( num1 ? ", " : "" ) << name;
	  if (num1 == 0) {
	    di << "" << name;
	  } else {
	    di << ", " << name;
	  }
	  num1++;
	  break;
	}
      }
    }
    if ( num1 >0 ) di << "\n";
  }
  return 0;
}


//=======================================================================
//function : freebounds
//purpose  : 
//=======================================================================
static Standard_Integer freebounds (Draw_Interpretor& di,
				    Standard_Integer n, const char** a)
{
  if ((n < 3) || (n > 5)) return 1;
  TopoDS_Shape shape = DBRep::Get(a[1]);
  if (shape.IsNull()) return 1;
  Standard_Real toler = Draw::Atof (a[2]);
  Standard_Boolean splitclosed = Standard_False, splitopen = Standard_False;
  if ( n > 3) splitclosed = Draw::Atoi (a[3]) != 0;
  if ( n > 4) splitopen   = Draw::Atoi (a[4]) != 0;

  ShapeAnalysis_FreeBounds F;
  if (toler <= 0)
    F = ShapeAnalysis_FreeBounds (shape, splitclosed, splitopen);
  else
    F = ShapeAnalysis_FreeBounds (shape, toler,  splitclosed, splitopen);

  char name[100];
  TopoDS_Shape wires = F.GetClosedWires();
  Sprintf (name, "%s_c", a[1]);
  DBRep::Set (name, wires);
  di << name << " - closed wires\n";
  wires = F.GetOpenWires();
  Sprintf (name, "%s_o", a[1]);
  DBRep::Set (name, wires);
  di << name << " - open wires\n";

  return 0;
}


//=======================================================================
//function : PrintProps
//purpose  : auxilary for FreeBoundsProps
//=======================================================================
static void PrintProps(Standard_Integer i,
		       const Handle(ShapeAnalysis_FreeBoundData)& fbd,
		       Draw_Interpretor& di)
{
  char str[100];

  Standard_Real area = fbd->Area();
  Standard_Real perimeter = fbd->Perimeter();
  Standard_Real ratio = fbd->Ratio();
  Standard_Real width = fbd->Width();
  Standard_Integer notch = fbd->NbNotches();
  Sprintf(str," %d\t%12.5f\t%12.5f\t%12.5f\t%12.5f\t%d", i, area, perimeter, ratio, width, notch);
  di<<str<<"\n";
}


//=======================================================================
//function : FreeBoundsProps
//purpose  : 
//=======================================================================
static Standard_Integer FreeBoundsProps(Draw_Interpretor& di,
					Standard_Integer n, const char** a)
{
  if ( (n < 2)||(n > 5) ) {
    di<<"Usage : freeprops shapename [tolerance [splitclosed [splitopen]]]\n";
    return 1;
  }

  TopoDS_Shape source = DBRep::Get(a[1]);
  if (source.IsNull()) {
    di<<"Error : unknown shape "<<a[1]<<"\n";
    return 1;
  }

  Standard_Real toler = 0.;
  Standard_Boolean splitclosed = Standard_False, splitopen = Standard_False;
  if (n > 2) toler =  Draw::Atof(a[2]);
  if (n > 3) splitclosed = Draw::Atoi(a[3]) != 0;
  if (n > 4) splitopen   = Draw::Atoi(a[4]) != 0;
  ShapeAnalysis_FreeBoundsProperties analyzer;
  if (toler > 0)
    analyzer.Init(source, toler, splitclosed, splitopen);
  else
    analyzer.Init(source, splitclosed, splitopen);
  analyzer.Perform();
  TopoDS_Compound closed, open;
  BRep_Builder B;

  Standard_Integer nb = analyzer.NbClosedFreeBounds();
  di<<"\n";
  di<<" \tArea mm2\tLength mm\tRatio L/W\tWidth mm\tNb noth\n";
  B.MakeCompound(closed);
  if (nb) {
    di<<"Closed bounds properties\n";
    for (Standard_Integer i=1; i <= nb; i++) {
      Handle(ShapeAnalysis_FreeBoundData) fbd = analyzer.ClosedFreeBound(i);
      PrintProps(i, fbd, di);
      B.Add(closed,fbd->FreeBound());
    }
  }

  nb = analyzer.NbOpenFreeBounds();
  B.MakeCompound(open);
  if (nb) {
    di<<"Open bounds properties\n";
    for (Standard_Integer i=1; i <= nb; i++) {
      Handle(ShapeAnalysis_FreeBoundData) fbd = analyzer.OpenFreeBound(i);
      PrintProps(i, fbd, di);
      B.Add(open,fbd->FreeBound());
    }
  }

  char name[100];
  Sprintf (name, "%s_c",a[1]);
  di << name << " - closed wires,  ";
  DBRep::Set(name, closed);
  Sprintf (name, "%s_o",a[1]);
  di << name << " - closed wires \n";
  DBRep::Set(name, open);
  return 0;
}


//=======================================================================
//function : closefreebounds
//purpose  : 
//=======================================================================
static Standard_Integer closefreebounds (Draw_Interpretor& di,
					 Standard_Integer n, const char** a)
{
  if ((n < 4) || (n > 6)) return 1;
  TopoDS_Shape shape = DBRep::Get(a[1]);
  if (shape.IsNull()) return 1;
  Standard_Real sewtoler = Draw::Atof (a[2]), closetoler = Draw::Atof (a[3]);
  Standard_Boolean splitclosed = Standard_False, splitopen = Standard_False;
  if ( n > 4) splitclosed = Draw::Atoi (a[3]) != 0;
  if ( n > 5) splitopen   = Draw::Atoi (a[4]) != 0;

  ShapeFix_FreeBounds F;
  if (sewtoler <= 0)
    F = ShapeFix_FreeBounds (shape, closetoler, splitclosed, splitopen);
  else
    F = ShapeFix_FreeBounds (shape, sewtoler, closetoler, splitclosed, splitopen);

  char name[100];
  TopoDS_Shape wires = F.GetClosedWires();
  Sprintf (name, "%s_c", a[1]);
  DBRep::Set (name, wires);
  di << name << " - closed wires\n";
  wires = F.GetOpenWires();
  Sprintf (name, "%s_o", a[1]);
  DBRep::Set (name, wires);
  di << name << " - open wires\n";

  return 0;
}


//=======================================================================
//function : MyVISEDG
//purpose  : 
//=======================================================================
static Standard_Integer MyVISEDG (Draw_Interpretor& /*di*/,
			       Standard_Integer n, const char** a)
{
  if (n >4) return 1;
  TopoDS_Shape MaListe = DBRep::Get(a[1]);
  if (MaListe.IsNull()) return 1;
  TopoDS_Compound TheList = TopoDS::Compound(MaListe);
  if (TheList.IsNull()) return 1;
  Standard_Real toler = 0.001;
  int create = 0;
  if ( n >= 3) toler = Draw::Atof(a[2]);
  if (n == 4 && !strcmp(a[3],"C")) create = 1;
  ShapeAnalysis_FreeBounds F(TheList,toler);
  //
  //
  //
  char name[100];
  char num[5];
  if (!create)
    {
      TopoDS_Compound Wires = F.GetClosedWires();
      TopoDS_Iterator S(Wires);
      Standard_Integer iwire = 0;
      while (S.More())
	{
	  Sprintf (num,"%d",iwire);
	  name[0] = 'w';
	  name[1] = '\0';
	  strncat(name,num,strlen(num));
	  name[strlen(name)] = '\0';
	  DBRep::Set(name,S.Value());
	  S.Next();
	  iwire++;
	}
      iwire = 0;
      TopoDS_Compound Edges = F.GetOpenWires();
      S.Initialize(Edges);
      iwire = 0;
      while (S.More())
	{
	  Sprintf (num,"%d",iwire);
	  name[0] = 'E';
	  name[1] = '\0';
	  strncat(name,num,strlen(num));
	  name[strlen(name)] = '\0';
	  DBRep::Set(name,S.Value());
	  S.Next();
	  iwire++;
	}
    }
  else
    {
    }
  return 0;
}

static Standard_Integer getareacontour (Draw_Interpretor& di,
					 Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  TopoDS_Shape shape = DBRep::Get(a[1]);
  if (shape.IsNull()) {
    di<<"Shape is not defined\n";
    return 1;
  }
  if(shape.ShapeType() != TopAbs_WIRE) {
    di<<"invalid type of argument\n";
    return 1;
  }
  //Handle(ShapeExtend_WireData) asewd = new ShapeExtend_WireData(TopoDS::Wire(shape));
  Standard_Real anArea = ShapeAnalysis::ContourArea(TopoDS::Wire(shape));
  di<<"Area = "<<anArea<<"\n";
  return 0;
}


static Standard_Integer checkselfintersection
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) 
  { 
      di<<"Call please \"checkselfintersection wire [face]\"\n";  
      return 1; 
  }

  // Get wire.
  const char* arg1 = argv[1];
  TopoDS_Shape wire = DBRep::Get(arg1);
  if (wire.IsNull() || wire.ShapeType() != TopAbs_WIRE)
  { 
      di<<"A null shape or not a wire is used.\n";  
      return 2; 
  }

  // Get face if the user provided us with a face.
  TopoDS_Shape face;
  if (argc > 2)
  {
      const char* arg2 = argv[2];
      face = DBRep::Get(arg2);
      if (face.IsNull() || face.ShapeType() != TopAbs_FACE)
      { 
          di<<"A null shape or not a face is used.\n";  
          return 3; 
      }
  }

  // If the face is null, make a plane inside the wire.
  if (face.IsNull())
  {
      BRepBuilderAPI_MakeFace mkFace(TopoDS::Wire(wire), true);
      if (mkFace.IsDone())
          face = mkFace.Face();
      else
      { 
          di<<"Can't make a face for the wire. Provide please a face for analysis.\n";  
          return 4; 
      }
  }

  ShapeAnalysis_Wire analyser(TopoDS::Wire(wire), TopoDS::Face(face), Precision::Confusion());
  Standard_Boolean result = analyser.CheckSelfIntersection();

  if (result == Standard_True)
      di<<"A self-intersecting wire.\n";
  else
      di<<"Not self-intersecting wire.\n";
  return 0;
}

static Standard_Integer checkedge(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) 
  { 
    di<<"Call please \"checkedge edge [face]\"\n";  
    return 1;
  }

  // Get edge.
  const char* arg1 = argv[1];
  TopoDS_Shape edge = DBRep::Get(arg1);
  if (edge.IsNull() || edge.ShapeType() != TopAbs_EDGE)
  { 
    di<<"A null shape or not an edge is used.\n";  
    return 2;
  }

  // Get face.
  TopoDS_Shape face;
  if (argc == 3)
  {
    const char* arg2 = argv[2];
    face = DBRep::Get(arg2);
    if (face.IsNull() || face.ShapeType() != TopAbs_FACE)
    { 
      di<<"A null shape or not a face is used.\n";  
      return 3;
    }
  }

  // Analysis of the edge.
  ShapeAnalysis_Edge analyser;
  Standard_Boolean isOk(Standard_True);

  // Curve 3D.
  if (analyser.HasCurve3d(TopoDS::Edge(edge)))
  {
    // Check vertices.
    if (analyser.CheckVerticesWithCurve3d(TopoDS::Edge(edge)))
    {
      isOk = Standard_False;
      di<<"Vertices of the edge don't coincide with start/end points of 3d-curve (using tolerance of the vertices).\n";
    }
  }
  else
  {
    isOk = Standard_False;
    di<<"Edge doesn't have a 3d-curve\n";
  }

  if (!face.IsNull())
  {
      // Curve 2D.
      if (analyser.HasPCurve(TopoDS::Edge(edge), TopoDS::Face(face)))
      {
        // Check vertices.
        if (analyser.CheckVerticesWithPCurve(TopoDS::Edge(edge), TopoDS::Face(face)))
        {
          isOk = Standard_False;
          di<<"Vertices of the edge don't coincide with start/end points of 2d-curve (using tolerance of the vertices).\n";
        }
      }
      else
      {
        isOk = Standard_False;
        di<<"Edge doesn't have a 2d-curve on this face\n";
      }
  }

  if (isOk)
    di<<"Edge seems OK.\n";

  return 0;
}

//=======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================

 void SWDRAW_ShapeAnalysis::InitCommands(Draw_Interpretor& theCommands) 
{
  static Standard_Integer initactor = 0;
  if (initactor)
  {
    return;
  }
  initactor = 1;

  Standard_CString g = SWDRAW::GroupName();

  theCommands.Add ("tolerance","shape [tolmin tolmax:real]", __FILE__,tolerance,g);
  theCommands.Add ("projface","nom_face X Y [Z]", __FILE__,projface,g);
  theCommands.Add ("projcurve","nom_edge | curve3d | curve3d first last + X Y Z",
		   __FILE__,projcurve,g);
  theCommands.Add("projpcurve", "edge face tol x y z [start_param]",
    __FILE__, projpcurve, g);
  theCommands.Add("anaface", "nomface", __FILE__, anaface, g);
  theCommands.Add ("statshape","shape [particul] : stats/particularites",
		   __FILE__,XSHAPE_statshape,g);
  theCommands.Add ("comptol","shape [nbpoints]",__FILE__,XSHAPE_comptoledge,g);
  theCommands.Add("freebounds",
		  "shp toler [splitclosed [splitopen]] - free bounds; toler <= 0 for shells (no sewing call)",
		  __FILE__, freebounds, g);

  const char* groupold="DE: old";
  theCommands.Add("fbprops",
		  "shp [toler [splitclosed [splitopen]]] - free bounds properties; toler <= 0 or not specified - for shells (no sewing call)",
		  __FILE__, FreeBoundsProps, groupold);
  theCommands.Add("fbclose",
		  "shp sewtoler closetoler [splitclosed [splitopen]] - closes free bounds; use sewtoler <= 0 for shells (no sewing call)",
		  __FILE__, closefreebounds, groupold);
  theCommands.Add("K_VISEDG",
                  "K_VISEDG Visu of free edge of a compound of faces.",
                  __FILE__, MyVISEDG, groupold);
    
  theCommands.Add("getareacontour","wire ",__FILE__, getareacontour, groupold);
  theCommands.Add ("checkselfintersection","wire [face]", __FILE__,checkselfintersection,g);
  theCommands.Add ("checkedge","edge [face]", __FILE__,checkedge,g);
}
