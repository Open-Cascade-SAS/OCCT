// Created on: 1994-10-04
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRepTest.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Curve2d.hxx>

#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>

#include <MAT_Bisector.hxx>
#include <MAT_Zone.hxx>
#include <MAT_Graph.hxx>
#include <MAT_Arc.hxx>
#include <MAT_BasicElt.hxx>
#include <MAT_Node.hxx>
#include <MAT_Side.hxx>

#include <Bisector_Bisec.hxx>
#include <Bisector_BisecAna.hxx>
#include <Bisector_Curve.hxx>
#include <Precision.hxx>

#include <BRepMAT2d_Explorer.hxx>
#include <BRepMAT2d_BisectingLocus.hxx>
#include <BRepMAT2d_LinkTopoBilo.hxx>

#include <gp_Parab2d.hxx>
#include <gp_Hypr2d.hxx>

#include <DBRep.hxx>
#include <TopoDS.hxx>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

static BRepMAT2d_BisectingLocus  MapBiLo;
static BRepMAT2d_Explorer        anExplo;
static BRepMAT2d_LinkTopoBilo    TopoBilo;
static MAT_Side                  SideOfMat = MAT_Left;
static Standard_Boolean          LinkComputed;

static void DrawCurve(const Handle(Geom2d_Curve)& aCurve,
		      const Standard_Integer      Indice);

//==========================================================================
//function : topoLoad
//           loading of a face in the explorer.
//==========================================================================
static Standard_Integer topoload (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{ 
  if (argc < 2) return 1;
  
  TopoDS_Shape C1 = DBRep::Get (argv[1],TopAbs_FACE);

  if (C1.IsNull()) return 1;

  anExplo.Perform (TopoDS::Face(C1));
  return 0;
}

//==========================================================================
//function : drawcont
//           visualization of the contour defined by the explorer.
//==========================================================================
static Standard_Integer drawcont(Draw_Interpretor& , Standard_Integer , const char**)
{
  Handle(Geom2d_TrimmedCurve) C;

  for (Standard_Integer j = 1; j <= anExplo.NumberOfContours(); j ++) {
    for (anExplo.Init(j);anExplo.More();anExplo.Next()) {
      DrawCurve(anExplo.Value(),1);
    }
  }
  return 0;
}

//==========================================================================
//function : mat
//           calculate the map of locations bisector on the contour defined by 
//           the explorer.
//==========================================================================
static Standard_Integer mat(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  GeomAbs_JoinType theJoinType = GeomAbs_Arc;
  if (n  >= 2 && strcmp(a[1], "i") == 0)
    theJoinType = GeomAbs_Intersection;

  Standard_Boolean IsOpenResult = Standard_False;
  if (n == 3 && strcmp(a[2], "o") == 0)
    IsOpenResult = Standard_True;
  
  MapBiLo.Compute(anExplo, 1, SideOfMat, theJoinType, IsOpenResult);
  LinkComputed = Standard_False;

  return 0;
}

//============================================================================
//function : zone
//           construction and display of the proximity zone associated to the
//           base elements defined by the edge or the vertex.
//============================================================================
static Standard_Integer zone(Draw_Interpretor& , Standard_Integer argc , const char** argv)
{
  if (argc < 2) return 1;

  TopoDS_Shape S = DBRep::Get (argv[1],TopAbs_EDGE);
  if (S.IsNull()) {
    S = DBRep::Get (argv[1],TopAbs_VERTEX);
  }

  if (!LinkComputed) {
    TopoBilo.Perform(anExplo,MapBiLo);
    LinkComputed = Standard_True;
  }

  Standard_Boolean Reverse;
  Handle(MAT_Zone) TheZone = new MAT_Zone();

  for (TopoBilo.Init(S); TopoBilo.More(); TopoBilo.Next()) {
    const Handle(MAT_BasicElt)& BE = TopoBilo.Value();
    TheZone->Perform(BE);
    for (Standard_Integer i=1; i <= TheZone->NumberOfArcs(); i++) {
      DrawCurve(MapBiLo.GeomBis(TheZone->ArcOnFrontier(i),Reverse).Value(),2);
    }
  }
  return 0;
}


//==========================================================================
//function : side
//           side = left  => calculation to the left of the contour.
//           side = right => calculation to the right of the contour.
//==========================================================================

static Standard_Integer side(Draw_Interpretor& , Standard_Integer, const char** argv)
{
  if(!strcmp(argv[1],"right"))
    SideOfMat = MAT_Right;
  else
    SideOfMat = MAT_Left;
  
  return 0;
}

//==========================================================================
//function : result
//           Complete display of the calculated map.
//==========================================================================
static Standard_Integer result(Draw_Interpretor& , Standard_Integer, const char**)
{
  Standard_Integer i,NbArcs=0;
  Standard_Boolean Rev;
  
  NbArcs = MapBiLo.Graph()->NumberOfArcs();

  for (i=1; i <= NbArcs;i++) {
    DrawCurve(MapBiLo.GeomBis(MapBiLo.Graph()->Arc(i),Rev).Value(),3);
  }
  return 0;
}

//==========================================================================
//function : DrawCurve
//           Display of curve <aCurve> of Geom2d in a color defined by <Indice>.
//  Indice = 1 yellow,
//  Indice = 2 blue,
//  Indice = 3 red,
//  Indice = 4 green.
//==========================================================================
void DrawCurve(const Handle(Geom2d_Curve)& aCurve,
	       const Standard_Integer      Indice)
{  
  Handle(Standard_Type)      type = aCurve->DynamicType();
  Handle(Geom2d_Curve)       curve,CurveDraw;
  Handle(DrawTrSurf_Curve2d) dr;
  Draw_Color                 Couleur;

  if (type == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    curve = Handle(Geom2d_TrimmedCurve)::DownCast (aCurve)->BasisCurve();
    type = curve->DynamicType();    
    if (type == STANDARD_TYPE(Bisector_BisecAna)) {
      curve =Handle(Bisector_BisecAna)::DownCast (curve)->Geom2dCurve(); 
      type = curve->DynamicType(); 
    }
    // PB of representation of semi_infinite curves.
    gp_Parab2d gpParabola;
    gp_Hypr2d  gpHyperbola;
    Standard_Real Focus;
    Standard_Real Limit = 50000.;
    Standard_Real delta = 400;

    // PB of representation of semi_infinite curves.
    if (aCurve->LastParameter() == Precision::Infinite()) {
      
      if (type == STANDARD_TYPE(Geom2d_Parabola)) {
	gpParabola = Handle(Geom2d_Parabola)::DownCast(curve)->Parab2d();
	Focus = gpParabola.Focal();
	Standard_Real Val1 = Sqrt(Limit*Focus);
	Standard_Real Val2 = Sqrt(Limit*Limit);
	              delta= (Val1 <= Val2 ? Val1:Val2);
      }
      else if (type == STANDARD_TYPE(Geom2d_Hyperbola)) {
	gpHyperbola = Handle(Geom2d_Hyperbola)::DownCast(curve)->Hypr2d();
	Standard_Real Majr  = gpHyperbola.MajorRadius();
	Standard_Real Minr  = gpHyperbola.MinorRadius();
	Standard_Real Valu1 = Limit/Majr;
	Standard_Real Valu2 = Limit/Minr;
	Standard_Real Val1  = Log(Valu1+Sqrt(Valu1*Valu1-1));
	Standard_Real Val2  = Log(Valu2+Sqrt(Valu2*Valu2+1));
	              delta  = (Val1 <= Val2 ? Val1:Val2);
      }
      if (aCurve->FirstParameter() == -Precision::Infinite())
	CurveDraw = new Geom2d_TrimmedCurve(aCurve, -delta, delta);
      else
	CurveDraw = new Geom2d_TrimmedCurve(aCurve,
					    aCurve->FirstParameter(),
					    aCurve->FirstParameter() + delta);
    }
    else {
      CurveDraw = aCurve;
    }
    // end PB.
  }
  else {
    CurveDraw = aCurve;
  }

  if      (Indice == 1) Couleur = Draw_jaune;
  else if (Indice == 2) Couleur = Draw_bleu;
  else if (Indice == 3) Couleur = Draw_rouge;
  else if (Indice == 4) Couleur = Draw_vert;

  Standard_Integer Discret = 50;

  if (type == STANDARD_TYPE(Geom2d_Circle))
    dr = new DrawTrSurf_Curve2d(CurveDraw,Couleur,30,Standard_False);
  else if (type  == STANDARD_TYPE(Geom2d_Line))
    dr = new DrawTrSurf_Curve2d(CurveDraw,Couleur,2,Standard_False);
  else
    dr = new DrawTrSurf_Curve2d(CurveDraw,Couleur,Discret,Standard_False);

  dout << dr;
  dout.Flush();
}

//==========================================================================
//function BRepTest:: MatCommands
//==========================================================================

void BRepTest::MatCommands (Draw_Interpretor& theCommands)
{
  theCommands.Add("topoload","load face",__FILE__,topoload);
  theCommands.Add("drawcont","display current contour",__FILE__,drawcont);
  theCommands.Add("mat","computes the mat: mat [a/i [o]]",__FILE__,mat);
  theCommands.Add("side","side left/right",__FILE__,side);
  theCommands.Add("result","result",__FILE__,result);
  theCommands.Add("zone","zone edge or vertex",__FILE__,zone);
}
