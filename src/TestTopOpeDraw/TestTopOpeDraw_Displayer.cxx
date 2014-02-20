// Created on: 1996-09-09
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#include <TestTopOpeDraw_Displayer.hxx>

#include <TestTopOpeDraw_DrawableSHA.hxx>
#include <TestTopOpeDraw_DrawableP3D.hxx>
#include <TestTopOpeDraw_DrawableP2D.hxx>
#include <TestTopOpeDraw_DrawableC3D.hxx>
#include <TestTopOpeDraw_DrawableC2D.hxx>
#include <TestTopOpeDraw_DrawableSUR.hxx>
#include <TestTopOpeDraw_TTOT.hxx>

#include <Precision.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <DBRep.hxx>

#ifdef WNT
Standard_IMPORT Draw_Viewer dout;
#endif

//=======================================================================
//function : TestTopOpeDraw_Displayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_Displayer::TestTopOpeDraw_Displayer()
{ 
  InitDisplayer();
}

//=======================================================================
//function : TestTopOpeDraw_Displayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_Displayer::TestTopOpeDraw_Displayer(const TCollection_AsciiString& namedbrep,const TopoDS_Shape& S)
{
  InitDisplayer();
  DisplayShape(namedbrep,S);
}

//=======================================================================
//function : TestTopOpeDraw_Displayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_Displayer::TestTopOpeDraw_Displayer
(const TCollection_AsciiString& namedbrep,const TCollection_AsciiString& namedisp,const TopoDS_Shape& S)
{
  InitDisplayer();
  DisplayShape(namedbrep,namedisp,S);
}

//=======================================================================
//function : DisplayShape
//purpose  : 
//=======================================================================

void TestTopOpeDraw_Displayer::DisplayShape(const TCollection_AsciiString& namedbrep, const TopoDS_Shape& S)
{
  if (S.IsNull()) return;
  SetShape(namedbrep,S);
  DisplayShapePrivate();
}

//=======================================================================
//function : DisplayShape
//purpose  : 
//=======================================================================

void TestTopOpeDraw_Displayer::DisplayShape
(const TCollection_AsciiString& namedbrep,const TCollection_AsciiString& namedisp,const TopoDS_Shape& S)
{
  if (S.IsNull()) return;
  SetShape(namedbrep,S);
  myNameDisplay = namedisp;
  DisplayShapePrivate();
}

//=======================================================================
//function : DisplayShapePrivate
//purpose  : 
//=======================================================================

void TestTopOpeDraw_Displayer::DisplayShapePrivate()
{
  if (myShape.IsNull()) return; 
  TopAbs_ShapeEnum t = myShape.ShapeType();

  myNameDisplay.Prepend(" ");
  if ( ! myNameColorIs ) myNameColor = TestTopOpeDraw_TTOT::ShapeColor(myShape);
  
  if (myDisplayNameWithOrientation) TestTopOpeDraw_TTOT::CatOrientation(myShape,myNameDisplay);
  if (myDisplayNameWithGeometry) TestTopOpeDraw_TTOT::CatGeometry(myShape,myNameDisplay);
  Draw_Color ConnCol = Draw_jaune;
  if (t == TopAbs_VERTEX) ConnCol = myNameColor;
  Draw_Color EdgeCol = Draw_rouge;
  if (myColIs) {
    ConnCol = EdgeCol = myCol;
  }
  Handle(TestTopOpeDraw_DrawableSHA) D;
  Standard_Integer nbisos = mySnbisos;
  Standard_Boolean nbisosdef = mySnbisosdef;
  Standard_Integer discret = mySdiscret;
  Standard_Boolean discretdef = mySdiscretdef;
  Standard_CString csn = myNameDBRep.ToCString(); 
  Handle(DBRep_DrawableShape) DBS = Handle(DBRep_DrawableShape)::DownCast(Draw::Get(csn));
  if (!DBS.IsNull() && !nbisosdef) {
    nbisos = DBS->NbIsos();
  }
  if (!DBS.IsNull() && !discretdef) {
    discret = DBS->Discret();
  }
  D = new TestTopOpeDraw_DrawableSHA
    (myShape,
     Draw_vert,ConnCol,EdgeCol,Draw_bleu,//free,shared,other,isos
     mySsize,nbisos,discret,
     myNameDisplay.ToCString(),myNameColor,
     myDisplayGeometry);
  
  if (myTol != 0.0 ) D->SetTol(myTol);
  if (myPar != -1.0 ) D->SetPar(myPar);
  
  char* pname = (char *)myNameDBRep.ToCString();
  Draw::Set(pname,Handle(DBRep_DrawableShape)::DownCast(D));
}

//=======================================================================
//function : InitDisplayer
//purpose  : 
//=======================================================================

void TestTopOpeDraw_Displayer::InitDisplayer()
{
  myDisplayGeometry = Standard_False;
  myDisplayNameWithGeometry = Standard_False;
  myDisplayNameWithOrientation = Standard_False;

  myShape.Nullify();
  NameDBRep().Copy(""); 
  NameDisplay().Copy("");

  mySsize    = 100.;
  mySnbisos  = DBRep::NbIsos();
  mySnbisosdef = Standard_False;
  mySdiscret = DBRep::Discretisation();
  mySdiscretdef = Standard_False;
  myTol = 0.0;
  myTolIs = Standard_False;
  myPar = -1.0;
  myParIs = Standard_False;
  myCol = Draw_blanc;
  myColIs = Standard_False;
  myNameColor = Draw_blanc;
  myNameColorIs = Standard_False;
}

//=======================================================================
//function : SetShape
//purpose  : 
//=======================================================================

void TestTopOpeDraw_Displayer::SetShape(const TCollection_AsciiString& namedbrep,const TopoDS_Shape& S)
{
  myShape = S;
  myNameDBRep = namedbrep;
  myNameDisplay = namedbrep;
  if (myDisplayNameWithOrientation) TestTopOpeDraw_TTOT::CatOrientation(myShape,myNameDisplay);
  if (myDisplayNameWithGeometry) TestTopOpeDraw_TTOT::CatGeometry(myShape,myNameDisplay);
}

//=======================================================================
//function : TestTopOpeDraw_P3DDisplayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_P3DDisplayer::TestTopOpeDraw_P3DDisplayer()
{}

//=======================================================================
//function : TestTopOpeDraw_P3DDisplayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_P3DDisplayer::TestTopOpeDraw_P3DDisplayer(const TCollection_AsciiString& namedbrep, const gp_Pnt& P)
{
  DisplayP3D(namedbrep,P);
}

//=======================================================================
//function : TestTopOpeDraw_P3DDisplayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_P3DDisplayer::TestTopOpeDraw_P3DDisplayer
(const TCollection_AsciiString& namedbrep,const Standard_Real x, const Standard_Real y, const Standard_Real z)
{
  DisplayP3D(namedbrep,x,y,z);
}

//=======================================================================
//function : DisplayP3D
//purpose  : 
//=======================================================================

void TestTopOpeDraw_P3DDisplayer::DisplayP3D
(const TCollection_AsciiString& namedbrep,const Standard_Real x, const Standard_Real y, const Standard_Real z)
{
  gp_Pnt P(x,y,z);
  DisplayP3D(namedbrep,P);
}

//=======================================================================
//function : DisplayP3D
//purpose  : 
//=======================================================================

void TestTopOpeDraw_P3DDisplayer::DisplayP3D
(const TCollection_AsciiString& namedbrep,const gp_Pnt& P)
{
  TCollection_AsciiString namedisp(" "); namedisp += namedbrep;
  Handle(TestTopOpeDraw_DrawableP3D) D = 
    new TestTopOpeDraw_DrawableP3D(P,Draw_Square,
				   Draw_Color(Draw_rouge),
				   namedisp.ToCString(),
				   Draw_Color(Draw_cyan),1);
  char* pname = (char *)namedbrep.ToCString();
  Draw::Set(pname,D);
  dout<<D;
}

//=======================================================================
//function : TestTopOpeDraw_P2DDisplayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_P2DDisplayer::TestTopOpeDraw_P2DDisplayer()
{
  SetColor(Draw_Color(Draw_rouge));
  SetNameColor(Draw_Color(Draw_cyan));
}

//=======================================================================
//function : TestTopOpeDraw_P2DDisplayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_P2DDisplayer::TestTopOpeDraw_P2DDisplayer
(const TCollection_AsciiString& namedbrep, const gp_Pnt2d& P)
{
  DisplayP2D(namedbrep,P);
}

//=======================================================================
//function : TestTopOpeDraw_P2DDisplayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_P2DDisplayer::TestTopOpeDraw_P2DDisplayer
(const TCollection_AsciiString& namedbrep,const Standard_Real x, const Standard_Real y)
{
  DisplayP2D(namedbrep,x,y);
}

//=======================================================================
//function : DisplayP2D
//purpose  : 
//=======================================================================

void TestTopOpeDraw_P2DDisplayer::DisplayP2D
(const TCollection_AsciiString& namedbrep,const Standard_Real x, const Standard_Real y)
{
  gp_Pnt2d P(x,y);
  DisplayP2D(namedbrep,P);
}

//=======================================================================
//function : DisplayP2D
//purpose  : 
//=======================================================================

void TestTopOpeDraw_P2DDisplayer::DisplayP2D
(const TCollection_AsciiString& namedbrep,const gp_Pnt2d& P)
{
  TCollection_AsciiString namedisp(" "); namedisp += namedbrep;
  Handle(TestTopOpeDraw_DrawableP2D) D;
  D = new TestTopOpeDraw_DrawableP2D(P,
				     Draw_Square,myCol,
				     namedisp.ToCString(),myNameColor,
				     1);
  char* pname = (char *)namedbrep.ToCString();
  Draw::Set(pname,D);
  dout<<D;
}

//=======================================================================
//function : TestTopOpeDraw_C3DDisplayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_C3DDisplayer::TestTopOpeDraw_C3DDisplayer()
{
  InitC3DDisplayer();
}

//=======================================================================
//function : TestTopOpeDraw_C3DDisplayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_C3DDisplayer::TestTopOpeDraw_C3DDisplayer(const Handle(Geom_Curve& C))
{
  InitC3DDisplayer();
  DisplayC3D(C);
}

//=======================================================================
//function : InitC3DDisplayer
//purpose  : 
//=======================================================================

void TestTopOpeDraw_C3DDisplayer::InitC3DDisplayer()
{
  myCDdiscret = 16;
  myCDdeflect = 0.01;
  myCDdrawmod = 1;
  myCDdisplayorigin = Standard_True;
}

//=======================================================================
//function : DisplayC3D
//purpose  : 
//=======================================================================

void TestTopOpeDraw_C3DDisplayer::DisplayC3D(const Handle(Geom_Curve& C))
{
  if (C.IsNull()) return;
  Draw_ColorKind col = TestTopOpeDraw_TTOT::GeometryColor(TopOpeBRepDS_CURVE);
  Handle(TestTopOpeDraw_DrawableC3D) D;
  D = new TestTopOpeDraw_DrawableC3D
    (C,col,
     "",col,
     myCDdiscret,myCDdeflect,myCDdrawmod,myCDdisplayorigin);
  dout<<D;
}

//=======================================================================
//function : TestTopOpeDraw_C2DDisplayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_C2DDisplayer::TestTopOpeDraw_C2DDisplayer()
{
  InitC2DDisplayer();
}

//=======================================================================
//function : TestTopOpeDraw_C2DDisplayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_C2DDisplayer::TestTopOpeDraw_C2DDisplayer
(const Handle(Geom2d_Curve& C))
{
  InitC2DDisplayer();
  DisplayC2D(C);
}

//=======================================================================
//function : InitC2DDisplayer
//purpose  : 
//=======================================================================

void TestTopOpeDraw_C2DDisplayer::InitC2DDisplayer()
{
  myC2Ddiscret = 16;
  myC2Ddisplayorigin = Standard_True;
  myC2Ddisplaycurvradius = Standard_False;
  myC2Dradiusmax = 1.e3;
  myC2Dradiusratio = 0.1;
  Draw_ColorKind col = TestTopOpeDraw_TTOT::GeometryColor(TopOpeBRepDS_CURVE);
  SetColor(Draw_Color(col));
  SetNameColor(Draw_Color(col));
}

//=======================================================================
//function : DisplayC2D
//purpose  : 
//=======================================================================

void TestTopOpeDraw_C2DDisplayer::DisplayC2D(const Handle(Geom2d_Curve& C))
{
  if (C.IsNull()) return;
  Handle(TestTopOpeDraw_DrawableC2D) D = new TestTopOpeDraw_DrawableC2D
    (C,myCol,"",myNameColor,
     myC2Ddiscret,myC2Ddisplayorigin,
     myC2Ddisplaycurvradius,myC2Dradiusmax,myC2Dradiusratio);
  dout<<D;
}

//=======================================================================
//function : DisplayC2D
//purpose  : 
//=======================================================================

void TestTopOpeDraw_C2DDisplayer::DisplayC2D
(const TCollection_AsciiString& namedbrep,const TCollection_AsciiString& namedisp,const Handle(Geom2d_Curve& C))
{
  Handle(TestTopOpeDraw_DrawableC2D) D = new TestTopOpeDraw_DrawableC2D
    (C,myCol,namedisp.ToCString(),myNameColor,
     myC2Ddiscret,myC2Ddisplayorigin,
     myC2Ddisplaycurvradius,myC2Dradiusmax,myC2Dradiusratio);
  char* pname = (char *)namedbrep.ToCString();
  Draw::Set(pname,D);
}

//=======================================================================
//function : TestTopOpeDraw_SurfaceDisplayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_SurfaceDisplayer::TestTopOpeDraw_SurfaceDisplayer()
{
  InitSurfaceDisplayer();
}

//=======================================================================
//function : TestTopOpeDraw_SurfaceDisplayer
//purpose  : 
//=======================================================================

TestTopOpeDraw_SurfaceDisplayer::TestTopOpeDraw_SurfaceDisplayer
(const Handle(Geom_Surface& S))
{
  InitSurfaceDisplayer();
  DisplaySurface(S);
}

//=======================================================================
//function : InitDisplayer
//purpose  : 
//=======================================================================

void TestTopOpeDraw_SurfaceDisplayer::InitSurfaceDisplayer()
{
  mySDBoundColor = Draw_vert;
  mySDIsoColor = Draw_bleu;
  mySDNormalColor = Draw_blanc;
  mySDnu = 0;
  mySDnv = 0;
  mySDdiscret = 16;
  mySDdeflect = 0.01;
  mySDdrawmod = 1;
  mySDdisplayorigin = Standard_True;
}

//=======================================================================
//function : DisplaySurface
//purpose  : 
//=======================================================================

void TestTopOpeDraw_SurfaceDisplayer::DisplaySurface(const Handle(Geom_Surface& S))
{
  if (S.IsNull()) return;
  Handle(TestTopOpeDraw_DrawableSUR) D;
  D = new TestTopOpeDraw_DrawableSUR
    (S,mySDIsoColor,mySDBoundColor,mySDNormalColor,
     "",myNameColor,
     mySDnu,mySDnv,mySDdiscret,mySDdeflect,mySDdrawmod,mySDdisplayorigin);
  dout<<D;
}
