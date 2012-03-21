// Created on: 1996-09-11
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


#include <TestTopOpeDraw_DrawableSUR.ixx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp.hxx>

//=======================================================================
//function : TestTopOpeDraw_DrawableSUR
//purpose  : 
//=======================================================================

TestTopOpeDraw_DrawableSUR::TestTopOpeDraw_DrawableSUR
(const Handle(Geom_Surface)& S, const Draw_Color& IsoColor) :
 DrawTrSurf_Surface(S,0, 0, Draw_cyan, IsoColor,16,  0.01, 1),
                  //  nu,nv,boundscolor         Disc,Defl, DMode)
 myNormalColor(Draw_blanc)

{
  myText = new Draw_Text3D(Pnt(),"",IsoColor);
}

//=======================================================================
//function : TestTopOpeDraw_DrawableSUR
//purpose  : 
//=======================================================================

TestTopOpeDraw_DrawableSUR::TestTopOpeDraw_DrawableSUR
(const Handle(Geom_Surface)& S, const Draw_Color& IsoColor,
 const Standard_CString Text,const Draw_Color& TextColor) :
 DrawTrSurf_Surface(S,0, 0, Draw_cyan, IsoColor,16,  0.01, 1),
                  //  nu,nv,boundscolor         Disc,Defl, DMode)
 myNormalColor(Draw_blanc)

{
  myText = new Draw_Text3D(Pnt(),Text,TextColor);
}

//=======================================================================
//function : TestTopOpeDraw_DrawableSUR
//purpose  : 
//=======================================================================

TestTopOpeDraw_DrawableSUR::TestTopOpeDraw_DrawableSUR
(const Handle(Geom_Surface)& S,
 const Draw_Color& IsoColor,
 const Draw_Color& BoundColor, 
 const Draw_Color& NormalColor,
 const Standard_CString Text,const Draw_Color& TextColor,
 const Standard_Integer Nu, const Standard_Integer Nv,
 const Standard_Integer Disc, const Standard_Real Defl, const Standard_Integer DMode,
 const Standard_Boolean DispOrigin) :
 DrawTrSurf_Surface(S,Nu,Nv,IsoColor,BoundColor,Disc,Defl,DMode)
{
  myText = new Draw_Text3D(Pnt(),Text,TextColor);
  myNormalColor = NormalColor;
}

//=======================================================================
//function : Pnt2d
//purpose  : 
//=======================================================================

gp_Pnt2d TestTopOpeDraw_DrawableSUR::Pnt2d() const 
{
  const Handle(Geom_Surface)& GS = GetSurface();
  Standard_Real u1,u2,v1,v2; GS->Bounds(u1,u2,v1,v2);
  Standard_Real facpar = 0.20;
  Standard_Real u = u1 + (u2-u1)*facpar;
  Standard_Real v = v1 + (v2-v1)*facpar;
  gp_Pnt2d P(u,v);
  return P;
}

//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

gp_Pnt TestTopOpeDraw_DrawableSUR::Pnt() const
{
  const Handle(Geom_Surface)& GS = GetSurface();
  gp_Pnt2d P2d = Pnt2d();
  gp_Pnt P = GS->Value(P2d.X(),P2d.Y());
  return P;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void  TestTopOpeDraw_DrawableSUR::DrawOn(Draw_Display& dis) const
{
  DrawTrSurf_Surface::DrawOn(dis);
  myText->DrawOn(dis);
  DrawNormale(dis);
}

//=======================================================================
//function : NormalColor
//purpose  : 
//=======================================================================

void  TestTopOpeDraw_DrawableSUR::NormalColor(const Draw_Color& NormalColor)
{
  myNormalColor =  NormalColor;
}

//=======================================================================
//function : DrawNormale
//purpose  : 
//=======================================================================

void  TestTopOpeDraw_DrawableSUR::DrawNormale(Draw_Display& dis) const 
{
  dis.SetColor(myNormalColor);

  // la normale
  gp_Pnt2d P2d = Pnt2d(); Standard_Real u,v; P2d.Coord(u,v);
  gp_Pnt P1,P2; gp_Vec V,V1,V2; 
  const Handle(Geom_Surface)& GS = GetSurface();
  GS->D1(u,v,P1,V1,V2); 
  Standard_Real mag; V = V1.Crossed(V2); mag = V.Magnitude();
  Standard_Real lvec = 1.;
  Handle(Geom_RectangularTrimmedSurface) GRTS;
  GRTS = Handle(Geom_RectangularTrimmedSurface)::DownCast(GS);
  if (!GRTS.IsNull()) {
    Standard_Real u1,u2,v1,v2; GRTS->Bounds(u1,u2,v1,v2);
    gp_Pnt Pmin,Pmax; GRTS->D0(u1,v1,Pmin); GRTS->D0(u2,v2,Pmax);
    lvec = Pmin.Distance(Pmax);
  }
  if (mag > 1.e-10) V.Multiply(lvec/mag);
  else { V.SetCoord(lvec/2.,0,0); cout<<"Null normal"<<endl; }
  P2 = P1; P2.Translate(V);
  dis.Draw(P1,P2);

  // la fleche de la normale
  gp_Pnt2d p1,p2;
  dis.Project(P1,p1);
  dis.Project(P2,p2);
  gp_Vec2d v2d(p1,p2);
  if (v2d.Magnitude() > gp::Resolution()) {
    Standard_Real L = 20 / dis.Zoom();
    Standard_Real H = 10 / dis.Zoom();
    gp_Dir2d d2d(v2d);
    gp_Pnt2d pp;
    pp.SetCoord(p2.X() - L*d2d.X() - H*d2d.Y(), p2.Y() - L*d2d.Y() + H*d2d.X());
    dis.MoveTo(pp);
    dis.DrawTo(p2);
    pp.SetCoord(p2.X() - L*d2d.X() + H*d2d.Y(), p2.Y() - L*d2d.Y() - H*d2d.X());
    dis.DrawTo(pp);
  }

}
