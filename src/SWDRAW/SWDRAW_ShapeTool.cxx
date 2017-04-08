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

// 25.12.98 pdn renaming
// 02.02.99 cky/rln PRO17746: transmitting 'sketch' command to XSDRAWEUC
// 23.02.99 abv: method ShapeFix::FillFace() removed
// 02.03.99 cky/rln: command edgeregul only accepts tolerance
// 15.06.99 abv/pdn: command comptol added (from S4030)

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepLib.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <SWDRAW_ShapeTool.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <stdio.h>
// + edge, face
// + edgeregul/updtol
// + fillface
static Standard_Integer XSHAPE_edge
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) { di<<"donner un nom de shape\n"; return 1 /* Error */; }
  Standard_CString arg1 = argv[1];
  TopoDS_Shape Shape = DBRep::Get(arg1);
  if (Shape.IsNull()) { di<<arg1<<" inconnu\n"; return 1 /* Error */; }
  Standard_Integer nbe = 0, nbf = 0;  Standard_Real f3d,l3d;

  for (TopExp_Explorer exp(Shape,TopAbs_EDGE); exp.More(); exp.Next()) {
    TopoDS_Edge Edge = TopoDS::Edge (exp.Current());  nbe ++;
    if (BRep_Tool::Degenerated(Edge)) continue;
    Handle(Geom_Curve) curve3d = BRep_Tool::Curve (Edge,f3d,l3d);
    if (curve3d.IsNull()) {
      char nomsh[30];
      nbf ++;
      Sprintf (nomsh,"faultedge_%d",nbf);
      di<<"Edge sans Curve3d, n0 "<<nbe<<"\n";
      DBRep::Set (nomsh,Edge);
    }
  }
  return 0;
}



static Standard_Integer XSHAPE_explorewire
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  char nomsh[30];
  if (argc < 2) { di<<"donner un nom de wire\n"; return 1 /* Error */; }
  Standard_CString arg1 = argv[1];
  TopoDS_Shape Shape = DBRep::Get(arg1);
  if (Shape.IsNull()) { di<<arg1<<" inconnu\n"; return 1 /* Error */; }
  if (Shape.ShapeType() != TopAbs_WIRE) { di<<"Pas un WIRE\n"; return 1 /* Error */; }
  TopoDS_Wire W = TopoDS::Wire (Shape);
  TopoDS_Face F;
  if (argc > 2) {
    Standard_CString arg2 = argv[2];
    TopoDS_Shape aLocalShape = DBRep::Get(arg2);
    F = TopoDS::Face ( aLocalShape );
  }

  Standard_Integer i,num = 0, nbw, nbe = 0;
  TopTools_IndexedMapOfShape map;
  for (TopoDS_Iterator ext(W); ext.More(); ext.Next()) {
    if (ext.Value().ShapeType() != TopAbs_EDGE) continue;
    TopoDS_Edge E = TopoDS::Edge (ext.Value());
    nbe ++;  num = map.Add(E);
  }
  int* nbs = new int[nbe+1];  for (i = 0; i <= nbe; i ++) nbs[i] = 0;

  di<<"TopoDS_Iterator(EDGE)  donne "<<nbe<<" Edges dont "<<num<<" distinctes\n";
  nbe = num;
  nbw = 0;
  for (TopExp_Explorer exe(W.Oriented(TopAbs_FORWARD),TopAbs_EDGE); exe.More(); exe.Next()) nbw ++;
  di<<"TopExp_Explorer(EDGE)  donne "<<nbw<<" Edges\n";
  nbw = 0;
  BRepTools_WireExplorer bwe;
  if (F.IsNull()) bwe.Init(W);
  else bwe.Init (W,F);
  for (; bwe.More(); bwe.Next()) {
    TopoDS_Edge E = TopoDS::Edge (bwe.Current());
    nbw ++;
    num = map.FindIndex(E);
    nbs[num] ++;
  }
  di<<"BRepTools_WireExplorer donne "<<nbw<<" Edges\n";
  di<<"Par rapport a la map, edges sautees par WE en NOWE_num, passees > 1 fois en MULTWE_num\n";
  if (nbs[0] > 0) di<<"NB : Edge n0 0 comptee "<<nbs[0]<<" fois\n";
  for (i = 1; i <= nbe; i ++) {
    if (nbs[i] < 1) {
      di<<"Edge n0 "<<i<<" pas vue par WE\n";
      Sprintf (nomsh,"NOWE_%d",i);
      DBRep::Set (nomsh,map.FindKey(i));
    } else if (nbs[i] > 1) {
      di<<"Edge n0 "<<i<<" vue par WE : "<<nbs[i]<<" fois\n";
      Sprintf (nomsh,"MULT_%d",i);
      DBRep::Set (nomsh,map.FindKey(i));
    }
  }
  delete [] nbs;
  return 0;
}



static Standard_Integer XSHAPE_ssolid
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) { di<<"Give new solid name + shell name\n"; return 1 /* Error */; }
  Standard_CString arg1 = argv[1];
  TopoDS_Shape Shape = DBRep::Get(arg1);
  if (Shape.IsNull()) { di<<"Shape unknown : "<<arg1<<"\n"; return 1 /* Error */; }
  TopAbs_ShapeEnum shen = Shape.ShapeType();
  if (shen == TopAbs_SOLID) {
    di<<" Already a Solide ! nothing done\n";
    return 0;
  }
  if (shen != TopAbs_SHELL) {
    di<<" Not a Shell\n";  return 1 /* Error */;
  }
  if (!Shape.Free ()) {
    di<<"Shape non Free -> Freeing\n";
    Shape.Free(Standard_True);
  }
  TopoDS_Shell sh = TopoDS::Shell (Shape);
  TopoDS_Solid solid;
  BRep_Builder B;
  B.MakeSolid (solid);
  B.Add (solid,sh);
//   Pas encore fini : il faut une bonne orientation
  BRepClass3d_SolidClassifier bsc3d (solid);
  bsc3d.PerformInfinitePoint(BRepBuilderAPI::Precision());
  if (bsc3d.State() == TopAbs_IN) {
//         Ensuite, inverser C-A-D REPRENDRE LES SHELLS
//         (l inversion du solide n est pas bien prise en compte)
    di<<"NB : Shell to be reversed\n";
    TopoDS_Solid soli2;
    B.MakeSolid (soli2);    // on recommence
    sh.Reverse();
    B.Add (soli2,sh);
    solid = soli2;
  }
  DBRep::Set(argv[2],solid);
  return 0; // Done
}

static Standard_Integer samerange (Draw_Interpretor& di,  Standard_Integer argc, const char** argv)  
{
  if ( argc ==2 ) {
    TopoDS_Shape Shape = DBRep::Get(argv[1]);
    if (Shape.IsNull()) { di<<"Shape unknown: "<<argv[2]<<"\n"; return 1; }
  
    for ( TopExp_Explorer exp(Shape,TopAbs_EDGE); exp.More(); exp.Next() ) {
      TopoDS_Edge edge = TopoDS::Edge ( exp.Current() );
      BRepLib::SameRange ( edge, Precision::PConfusion() );
    }
  }
  else if ( argc == 7 ) {
    Handle(Geom2d_Curve) C = DrawTrSurf::GetCurve2d(argv[2]);
    if (C.IsNull()) { di<<"Curve unknown: "<<argv[2]<<"\n"; return 1; }
  
    Standard_Real oldFirst = Draw::Atof(argv[3]);
    Standard_Real oldLast = Draw::Atof(argv[4]);
    Standard_Real current_first = Draw::Atof(argv[5]);
    Standard_Real current_last = Draw::Atof(argv[6]);
    Standard_Real Tol = Precision::PConfusion();
    Handle(Geom2d_Curve) NewC2d;
    GeomLib::SameRange(Tol, C,  oldFirst,oldLast,
		       current_first, current_last, NewC2d);
    DrawTrSurf::Set(argv[1],NewC2d);
  }
  else {
    di << "Apply BRepLib::SameRange() to shape or GeomLib::SameRange() to pcurve:\n";
    di << "> samerange shape\n";
    di << "or\n";
    di << "> samerange newcurve curve2d first last newfirst newlast\n";
  }
  
  return 0;
}

//  ########################################
//  ##            DECLARATIONS            ##
//  ########################################

void  SWDRAW_ShapeTool::InitCommands (Draw_Interpretor& theCommands)
{
  static int initactor = 0;
  if (initactor)
  {
    return;
  }
  initactor = 1;

  const char* g;
  g = "DE: old";

  theCommands.Add ("anaedges","nom shape",
		   __FILE__,XSHAPE_edge,g);
  theCommands.Add ("expwire","nom wire [nom face]",
		   __FILE__,XSHAPE_explorewire,g);

  theCommands.Add ("ssolid","nom shell + nouveau nom solid",
		   __FILE__,XSHAPE_ssolid,g);

  theCommands.Add ("samerange","{ shape | result curve2d first last newfirst newlast }",
		   __FILE__,samerange,g);
}
