// Created on: 1996-04-10
// Created by: Jean-Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#include <Draw_Appli.hxx>
#include <Draw.hxx>
#include <Draw_Drawable3D.hxx>
#include <TestTopOpeDraw_DrawableSHA.hxx>
#include <TestTopOpeDraw_DrawableC3D.hxx>
#include <TestTopOpeDraw_DrawableMesure.hxx>
#include <DrawTrSurf_Curve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <Geom_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TestTopOpeTools_Mesure.hxx>

// pour sauver un DrawableSHA

//=======================================================================
//function : stestTopOpe
//purpose  : 
//=======================================================================

static Standard_Boolean stestTopOpe(const Handle(Draw_Drawable3D)& D) 
{
  Standard_Boolean res = D->IsInstance(STANDARD_TYPE(TestTopOpeDraw_DrawableSHA));
  return res;
}


//=======================================================================
//function : ssaveTopOpe
//purpose  : 
//=======================================================================

static void ssaveTopOpe (const Handle(Draw_Drawable3D)& D, ostream& OS)
{  
  Handle(TestTopOpeDraw_DrawableSHA) N;
  N = Handle(TestTopOpeDraw_DrawableSHA)::DownCast(D);
  if ( !N.IsNull()) {
    BRep_Builder B;
    BRepTools_ShapeSet S(B);
    S.Add(N->Shape());
    S.Write(OS);
    S.Write(N->Shape(),OS);
  }
  else cout <<"TestTopOpeDraw::ssaveTopOpe : error" << endl; 
}

static Standard_Real    size    = 100.;
static Standard_Integer nbIsos  = 2;
static Standard_Integer discret = 30;
//=======================================================================
//function : srestoreTopOpe
//purpose  : 
//=======================================================================

static Handle(Draw_Drawable3D) srestoreTopOpe (istream& IS)
{  
  BRep_Builder B;
  BRepTools_ShapeSet S(B);
  S.Read(IS);
  TopoDS_Shape theShape;
  S.Read(theShape,IS);
  Handle(DBRep_DrawableShape) N;
  N = new DBRep_DrawableShape(theShape,
			      Draw_vert,
			      Draw_jaune,
			      Draw_rouge,
			      Draw_bleu,
			      size,
			      nbIsos,
			      discret);
  return N;
}


//=======================================================================
//function : ssrTopOpe
//purpose  : 
//=======================================================================

static Draw_SaveAndRestore ssrTopOpe
("TestTopOpeDraw_DrawableSHA",
 stestTopOpe,ssaveTopOpe,srestoreTopOpe);

// pour sauver une DrawableC3D

//=======================================================================
//function : ctestTopOpe
//purpose  : 
//=======================================================================

static Standard_Boolean ctestTopOpe(const Handle(Draw_Drawable3D)& D) 
{
  Standard_Boolean res = D->IsInstance(STANDARD_TYPE(TestTopOpeDraw_DrawableC3D));
  return res;
}


//=======================================================================
//function : csaveTopOpe
//purpose  : 
//=======================================================================

static void csaveTopOpe (const Handle(Draw_Drawable3D)& D, ostream& OS)
{


  Handle(DrawTrSurf_Curve) N;
  N = Handle(DrawTrSurf_Curve)::DownCast(D);
  if ( !N.IsNull()) {
    Handle(Geom_Curve) GC;
    GC = N->GetCurve();
    BRep_Builder BB;
    TopoDS_Edge E;
    BB.MakeEdge(E, GC, 1.0e-7);
    BRep_Builder B;
    BRepTools_ShapeSet S(B);
    S.Add(E);
    S.Write(OS);
    S.Write(E,OS);
  }
  else cout <<"TestTopOpeDraw::csaveTopOpe : error" << endl; 
}

//=======================================================================
//function : crestoreTopOpe
//purpose  : 
//=======================================================================

static Handle(Draw_Drawable3D) crestoreTopOpe (istream& IS)
{  
  BRep_Builder B;
  BRepTools_ShapeSet S(B);
  S.Read(IS);
  TopoDS_Shape theShape;
  S.Read(theShape,IS);
  TopoDS_Edge& E = TopoDS::Edge(theShape);
  Standard_Real f, l;
  Handle(Geom_Curve) C3d = BRep_Tool::Curve(E, f, l);
  Handle(TestTopOpeDraw_DrawableC3D) N;
  N = new TestTopOpeDraw_DrawableC3D(C3d,Draw_blanc);
  return N;
}


//=======================================================================
//function : csrTopOpe
//purpose  : 
//=======================================================================

static Draw_SaveAndRestore csrTopOpe
("TestTopOpeDraw_DrawableC3D",
 ctestTopOpe,csaveTopOpe,crestoreTopOpe);


// pour sauver une DrawableMesure

//=======================================================================
//function : mtestTopOpe
//purpose  : 
//=======================================================================

static Standard_Boolean mtestTopOpe(const Handle(Draw_Drawable3D)& D) 
{
  Standard_Boolean res = D->IsInstance(STANDARD_TYPE(TestTopOpeDraw_DrawableMesure));
  return res;
}


//=======================================================================
//function : msaveTopOpe
//purpose  : 
//=======================================================================

static void msaveTopOpe (const Handle(Draw_Drawable3D)& D, ostream& OS)
{  
  Handle(DrawTrSurf_Curve) N;
  N = Handle(DrawTrSurf_Curve)::DownCast(D);
  if ( !N.IsNull()) {
    Handle(Geom_BSplineCurve) BS;
    BS = Handle(Geom_BSplineCurve)::DownCast(N->GetCurve());
    BRep_Builder BB;
    TopoDS_Edge E;
    BB.MakeEdge(E, BS, 1.0e-7);
    BRep_Builder B;
    BRepTools_ShapeSet S(B);
    S.Add(E);
    S.Write(OS);
    S.Write(E,OS);
  }
  else cout <<"TestTopOpeDraw::msaveTopOpe : error" << endl; 
}

//=======================================================================
//function : mrestoreTopOpe
//purpose  : 
//=======================================================================

static Handle(Draw_Drawable3D) mrestoreTopOpe (istream& IS)
{  
  BRep_Builder B;
  BRepTools_ShapeSet S(B);
  S.Read(IS);
  TopoDS_Shape theShape;
  S.Read(theShape,IS);
  TopoDS_Edge& E = TopoDS::Edge(theShape);
  Standard_Integer NbPoles;
  Standard_Real f, l;
  Handle(Geom_Curve) C3d = BRep_Tool::Curve(E, f, l);
  Handle(Geom_BSplineCurve) BS;
  BS = Handle(Geom_BSplineCurve)::DownCast(C3d);
  NbPoles = BS->NbPoles();
  TColgp_HArray1OfPnt* Pnt = new TColgp_HArray1OfPnt(1, NbPoles);
  BS->Poles(Pnt->ChangeArray1());
  TestTopOpeTools_Mesure Mes(Pnt);
  Handle(TestTopOpeDraw_DrawableMesure) N;
  N = new TestTopOpeDraw_DrawableMesure(Mes, Draw_blanc, Draw_rose);
  return N;
}


//=======================================================================
//function : msrTopOpe
//purpose  : 
//=======================================================================

static Draw_SaveAndRestore msrTopOpe
("TestTopOpeDraw_DrawableMesure",
 mtestTopOpe,msaveTopOpe,mrestoreTopOpe);
