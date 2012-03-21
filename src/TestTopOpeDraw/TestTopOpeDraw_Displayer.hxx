// Created on: 1996-09-09
// Created by: Jean Yves LEBEY
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


#ifndef _TestTopOpeDraw_Displayer_HeaderFile
#define _TestTopOpeDraw_Displayer_HeaderFile

#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopAbs_Orientation.hxx>
#include <Draw_Color.hxx>
#include <gp_Pnt.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <TopOpeBRepTool_define.hxx>

class TestTopOpeDraw_Displayer {

 public:

  Standard_EXPORT TestTopOpeDraw_Displayer();
  Standard_EXPORT TestTopOpeDraw_Displayer(const TCollection_AsciiString&,const TopoDS_Shape&);
  Standard_EXPORT TestTopOpeDraw_Displayer(const TCollection_AsciiString&,const TCollection_AsciiString&,const TopoDS_Shape&);
  Standard_EXPORT void DisplayShape(const TCollection_AsciiString&,const TopoDS_Shape&);
  Standard_EXPORT void DisplayShape(const TCollection_AsciiString&,const TCollection_AsciiString&,const TopoDS_Shape&);
  Standard_EXPORT void InitDisplayer();
  void SetShape(const TCollection_AsciiString&,const TopoDS_Shape&);

  TCollection_AsciiString& NameDBRep() { return myNameDBRep; }
  TCollection_AsciiString& NameDisplay() { return myNameDisplay; }
  void DisplayGeometry(const Standard_Boolean b) { myDisplayGeometry = b; }
  Standard_Boolean DisplayGeometry(void) { return myDisplayGeometry; }
  void DisplayNameWithGeometry(const Standard_Boolean b) { myDisplayNameWithGeometry = b; }
  void DisplayNameWithOrientation(const Standard_Boolean b) { myDisplayNameWithOrientation = b; }
  void NbIsos(const Standard_Integer n) { mySnbisos = n; }
  void NbIsosDef(const Standard_Boolean b) { mySnbisosdef = b; }
  void Discret(const Standard_Integer n) { mySdiscret = n; }
  void DiscretDef(const Standard_Boolean b) { mySdiscretdef = b; }
  void SetTol(const Standard_Real Tol) { myTol = Tol;}
  void GetTol(Standard_Real Tol) { Tol = myTol;}
  Standard_Boolean TolIs(void) {return myTolIs;}
  void TolIs(Standard_Boolean TolIs) { myTolIs = TolIs;}
  void SetPar(const Standard_Real Par) { myPar = Par; myParIs = Standard_True;}
  void GetPar(Standard_Real Par) { Par = myPar;}
  Standard_Boolean ParIs(void) {return myParIs;}
  void SetColor(const Draw_Color Col) { myCol = Col; myColIs = Standard_True;}
  void SetNameColor(const Draw_Color Col) { myNameColor = Col; myNameColorIs = Standard_True;}
  void AllColors(const Draw_Color Col) { SetColor(Col); SetNameColor(Col); }

  protected: 
  TCollection_AsciiString myNameDBRep;
  TCollection_AsciiString myNameDisplay;
  Standard_Boolean myDisplayGeometry;
  Standard_Boolean myDisplayNameWithGeometry;
  Standard_Boolean myDisplayNameWithOrientation;
  Draw_Color myNameColor; Standard_Boolean myNameColorIs;
  Draw_Color myCol; Standard_Boolean myColIs;

  private:
  // Shape
  void DisplayShapePrivate();
  TopoDS_Shape myShape;
  Standard_Real mySsize;
  Standard_Integer mySnbisos;
  Standard_Boolean mySnbisosdef;
  Standard_Integer mySdiscret;
  Standard_Boolean mySdiscretdef;
  Standard_Real myTol; Standard_Boolean myTolIs;
  Standard_Real myPar; Standard_Boolean myParIs;
};

class TestTopOpeDraw_C3DDisplayer : public TestTopOpeDraw_Displayer {
public:
  TestTopOpeDraw_C3DDisplayer();
  TestTopOpeDraw_C3DDisplayer(const Handle(Geom_Curve)& C);
  void InitC3DDisplayer();
  void DisplayC3D(const Handle(Geom_Curve)& C);
private:
  Standard_Integer myCDdiscret;
  Standard_Real myCDdeflect;
  Standard_Integer myCDdrawmod;
  Standard_Boolean myCDdisplayorigin;
};

class TestTopOpeDraw_C2DDisplayer : public TestTopOpeDraw_Displayer {
public:
  TestTopOpeDraw_C2DDisplayer();
  TestTopOpeDraw_C2DDisplayer(const Handle(Geom2d_Curve)& C);
  void InitC2DDisplayer();
  void DisplayC2D(const Handle(Geom2d_Curve)& C);
  void DisplayC2D(const TCollection_AsciiString&,const TCollection_AsciiString&,const Handle(Geom2d_Curve)& C);
private:
  Standard_Integer myC2Ddiscret;
  Standard_Boolean myC2Ddisplayorigin;
  Standard_Boolean myC2Ddisplaycurvradius;
  Standard_Real myC2Dradiusmax;
  Standard_Real myC2Dradiusratio;
};

class TestTopOpeDraw_SurfaceDisplayer : public TestTopOpeDraw_Displayer {
public:
  TestTopOpeDraw_SurfaceDisplayer();
  TestTopOpeDraw_SurfaceDisplayer(const Handle(Geom_Surface)& S);
  void InitSurfaceDisplayer();
  void DisplaySurface(const Handle(Geom_Surface)& S);
private:
  Draw_Color mySDBoundColor,mySDIsoColor,mySDNormalColor;
  Standard_Integer mySDnu,mySDnv;
  Standard_Integer mySDdiscret;
  Standard_Real mySDdeflect;
  Standard_Integer mySDdrawmod;
  Standard_Boolean mySDdisplayorigin;
};

class TestTopOpeDraw_P3DDisplayer : public TestTopOpeDraw_Displayer {
public:
  TestTopOpeDraw_P3DDisplayer();
  TestTopOpeDraw_P3DDisplayer(const TCollection_AsciiString&,const gp_Pnt& P);
  TestTopOpeDraw_P3DDisplayer(const TCollection_AsciiString&,const Standard_Real x, const Standard_Real y, const Standard_Real z);
  void DisplayP3D(const TCollection_AsciiString&,const Standard_Real x, const Standard_Real y, const Standard_Real z);
  void DisplayP3D(const TCollection_AsciiString&,const gp_Pnt& P);
};

class TestTopOpeDraw_P2DDisplayer : public TestTopOpeDraw_Displayer {
public:
  TestTopOpeDraw_P2DDisplayer();
  TestTopOpeDraw_P2DDisplayer(const TCollection_AsciiString&,const gp_Pnt2d& P);
  TestTopOpeDraw_P2DDisplayer(const TCollection_AsciiString&,const Standard_Real x, const Standard_Real y);
  void DisplayP2D(const TCollection_AsciiString&,const Standard_Real x, const Standard_Real y);
  void DisplayP2D(const TCollection_AsciiString&,const gp_Pnt2d& P);
};

#endif
