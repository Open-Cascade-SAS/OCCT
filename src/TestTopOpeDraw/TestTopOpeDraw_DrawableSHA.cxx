// Created on: 1994-10-24
// Created by: Jean Yves LEBEY
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw_Segment3D.hxx>
#include <Draw_Text3D.hxx>
#include <Geom_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TestTopOpeDraw_DrawableC3D.hxx>
#include <TestTopOpeDraw_DrawableP3D.hxx>
#include <TestTopOpeDraw_DrawableSHA.hxx>
#include <TestTopOpeDraw_DrawableSUR.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

Draw_Color DBRep_ColorOrientation (const TopAbs_Orientation);

//=======================================================================
//function : TestTopOpeDraw_DrawableSHA
//purpose  : 
//=======================================================================

TestTopOpeDraw_DrawableSHA::TestTopOpeDraw_DrawableSHA
(const TopoDS_Shape& aShape,
 const Draw_Color& FreeCol,
 const Draw_Color& ConnCol,
 const Draw_Color& EdgeCol,
 const Draw_Color& IsosCol,
 const Standard_Real size,
 const Standard_Integer nbisos,
 const Standard_Integer discret,
 const Standard_CString Text,
 const Draw_Color& TextColor,
 const Standard_Boolean DisplayGeometry) :
 DBRep_DrawableShape(aShape,FreeCol,ConnCol,EdgeCol,IsosCol,size,nbisos,discret),
 myTol(0.0), myPar(-1.0)
{
  myDisplayGeometry = DisplayGeometry;
  myText = new Draw_Text3D(Pnt(),Text,TextColor);
  myTextColor = TextColor;
}


//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

gp_Pnt TestTopOpeDraw_DrawableSHA::Pnt() const
{
  gp_Pnt P(0,0,0);
  Standard_Real u1,u2,v1,v2;
  TopExp_Explorer ex;

  TopoDS_Shape S = Shape();
  TopAbs_ShapeEnum T = S.ShapeType();
  Standard_Real facpar = 0.;

  while ( T == TopAbs_COMPOUND ) {
    TopoDS_Iterator ti(S);
    if ( ti.More() ) {
      S = ti.Value(); T = S.ShapeType();
    }
    else {
      break;
    }
  }
  // si S final = compound --> P = 0 0 0 

  switch (T) {
    
  case TopAbs_VERTEX : 
    P = BRep_Tool::Pnt(TopoDS::Vertex(S)); 
    break;

  case TopAbs_WIRE : 
    T = TopAbs_EDGE; 
    ex.Init(S,T); S = ex.Current();
    facpar = 0.33;
    // no break, in order to execute EDGE case

  case TopAbs_EDGE : 
    {
    BRepAdaptor_Curve CU(TopoDS::Edge(S));
    u1 = CU.FirstParameter(); u2 = CU.LastParameter();
    if (facpar == 0.) facpar = 0.20;
    if (myPar != -1.0) facpar = myPar;
    Standard_Real p = u1 + (u2-u1)*facpar;
    P = CU.Value(p);
    }
    break;

  case TopAbs_SHELL : case TopAbs_SOLID : case TopAbs_COMPSOLID :
    T = TopAbs_FACE; 
    ex.Init(S,T); S = ex.Current();
    facpar = 0.33;
    // no break, in order to execute FACE case

  case TopAbs_FACE : 
    {
    BRepAdaptor_Surface SU(TopoDS::Face(S));
    BRepTools::UVBounds(TopoDS::Face(S),u1,u2,v1,v2);
    if (facpar == 0.) facpar = 0.20;
    Standard_Real u = u1 + (u2-u1)*facpar;
    Standard_Real v = v1 + (v2-v1)*facpar;
    P = SU.Value(u,v);
    }
    break;
  default:
    break;
  }

  return P;
}

//=======================================================================
//function : SetDisplayGeometry
//purpose  : 
//=======================================================================

void TestTopOpeDraw_DrawableSHA::SetDisplayGeometry(const Standard_Boolean b)
{
  myDisplayGeometry = b;
}

//=======================================================================
//function : SetTol
//purpose  : 
//=======================================================================

void TestTopOpeDraw_DrawableSHA::SetTol(const Standard_Real t)
{
  myTol = t;
  if(myTol != 0.0) 
    myDM3d = new Draw_Marker3D(Pnt(), Draw_CircleZoom , Draw_rouge, myTol);
}


//=======================================================================
//function : SetPar
//purpose  : 
//=======================================================================

void TestTopOpeDraw_DrawableSHA::SetPar(const Standard_Real t)
{
  myPar = t;
  myText->SetPnt(Pnt());
}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void TestTopOpeDraw_DrawableSHA::DrawOn(Draw_Display& dis) const
{
  if(myDM3d.IsNull()) 
    DBRep_DrawableShape::DrawOn(dis);
  else 
    myDM3d->DrawOn(dis);
  myText->DrawOn(dis);
  if (myDisplayGeometry) DisplayGeometry(dis);
}

//=======================================================================
//function : DisplayGeometry
//purpose  : 
//=======================================================================

void TestTopOpeDraw_DrawableSHA::DisplayGeometry(Draw_Display& dis) const 
{
  const TopoDS_Shape& aShape = Shape();
  const TopAbs_ShapeEnum t = aShape.ShapeType();

  if ( t == TopAbs_FACE ) {
    const TopoDS_Face& F = TopoDS::Face(aShape);
    TopLoc_Location loc;
    Handle(Geom_Surface) GS = BRep_Tool::Surface(F,loc);
    if ( GS.IsNull() ) return;
    Standard_Real u1,u2,v1,v2; BRepTools::UVBounds(F,u1,u2,v1,v2);
    GS = Handle(Geom_Surface)::DownCast(GS->Transformed(loc.Transformation()));
    Handle(Geom_RectangularTrimmedSurface) GRTS;
    GRTS = new Geom_RectangularTrimmedSurface(GS,u1,u2,v1,v2);
    Handle(TestTopOpeDraw_DrawableSUR) D;
    D = new TestTopOpeDraw_DrawableSUR(GRTS,Draw_bleu);
    Draw_Color norcol = DBRep_ColorOrientation(aShape.Orientation());
    D->NormalColor(norcol);
    D->DrawNormale(dis);
  }

  else if ( t == TopAbs_EDGE ) {
    const TopoDS_Edge& E = TopoDS::Edge(aShape);
    TopLoc_Location loc; Standard_Real f,l;
    Handle(Geom_Curve) GC = BRep_Tool::Curve(E,loc,f,l);
    if ( GC.IsNull() ) return;    
    GC = Handle(Geom_Curve)::DownCast(GC->Transformed(loc.Transformation()));
    Handle(Geom_TrimmedCurve) GTC = new Geom_TrimmedCurve(GC,f,l);
    Handle(TestTopOpeDraw_DrawableC3D) D;
    D = new TestTopOpeDraw_DrawableC3D(GTC,myTextColor);
    D->DrawOn(dis);
  }

  else if ( t == TopAbs_VERTEX ) {
    const TopoDS_Vertex& V = TopoDS::Vertex(aShape);
    gp_Pnt P = BRep_Tool::Pnt(V);
    Handle(TestTopOpeDraw_DrawableP3D) D;
    D = new TestTopOpeDraw_DrawableP3D(P,myTextColor);
    D->DrawOn(dis);
  }

}
