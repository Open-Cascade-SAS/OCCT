// Copyright (c) 1995-1999 Matra Datavision
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

// pmn -> modified 17/01/1996 : utilisation de Curve() et SetCurve()

#include <Law_Interpol.ixx>
#include <Precision.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Law_Interpolate.hxx>

#ifdef DEB
#ifdef DRAW

// Pour le dessin.
#include <Draw_Appli.hxx>
#include <Draw_Display.hxx>
#include <Draw.hxx>
#include <Draw_Segment3D.hxx>
#include <Draw_Segment2D.hxx>
#include <Draw_Marker2D.hxx>
#include <Draw_ColorKind.hxx>
#include <Draw_MarkerShape.hxx>

static void Law_draw1dcurve(const Handle(Law_BSpline)& bs,
                            const gp_Vec2d&            tra,
                            const Standard_Real        scal)
{
  TColStd_Array1OfReal pol(1,bs->NbPoles());
  bs->Poles(pol);
  TColStd_Array1OfReal knots(1,bs->NbKnots());
  bs->Knots(knots);
  TColStd_Array1OfInteger  mults(1,bs->NbKnots());
  bs->Multiplicities(mults);
  Standard_Integer deg = bs->Degree();
  Standard_Integer nbk = knots.Length();
  
  Handle(Draw_Marker2D) mar;
  gp_Pnt2d pp(knots(1),scal*bs->Value(knots(1)));
  pp.Translate(tra);
  gp_Pnt2d qq;
  mar = new Draw_Marker2D(pp,Draw_Square,Draw_cyan);
  dout<<mar;
  Handle(Draw_Segment2D) seg;
  for(Standard_Integer i = 1; i < nbk; i++){
    Standard_Real f = knots(i);
    Standard_Real l = knots(i+1);
    for (Standard_Integer iu = 1; iu <= 30; iu++){
      Standard_Real uu = iu/30.;
      uu = f+uu*(l-f);
      qq.SetCoord(uu,scal*bs->Value(uu));
      qq.Translate(tra);
      seg = new Draw_Segment2D(pp,qq,Draw_jaune);
      dout<<seg;
      pp = qq;
    }
    mar = new Draw_Marker2D(pp,Draw_Square,Draw_cyan);
    dout<<mar;
  }
}

static void Law_draw1dcurve(const TColStd_Array1OfReal&    pol,
                            const TColStd_Array1OfReal&    knots,
                            const TColStd_Array1OfInteger& mults,
                            const Standard_Integer         deg, 
                            const gp_Vec2d&                tra,
                            const Standard_Real            scal)
{
  Handle(Law_BSpline) bs = new Law_BSpline(pol,knots,mults,deg);
  Law_draw1dcurve(bs,tra,scal);
}

static Standard_Boolean Affich = 0;

#endif
#endif

//=======================================================================
//function : Law_Interpol
//purpose  : 
//=======================================================================

Law_Interpol::Law_Interpol()
{
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void Law_Interpol::Set(const TColgp_Array1OfPnt2d& ParAndRad,
		       const Standard_Boolean Periodic)
{
  Standard_Integer l = ParAndRad.Lower();
  Standard_Integer nbp = ParAndRad.Length();

  Handle(TColStd_HArray1OfReal) par = new TColStd_HArray1OfReal(1,nbp);
  Handle(TColStd_HArray1OfReal) rad;
  if(Periodic) rad = new TColStd_HArray1OfReal(1,nbp - 1);
  else rad = new TColStd_HArray1OfReal(1,nbp);
  Standard_Real x,y;
  Standard_Integer i;
  for(i = 1; i <= nbp; i++){
    ParAndRad(l + i - 1).Coord(x,y);
    par->SetValue(i,x);
    if(!Periodic || i != nbp) rad->SetValue(i,y);
  }
  Law_Interpolate inter(rad,par,Periodic,Precision::Confusion());
  inter.Perform();
  SetCurve(inter.Curve()); 
#ifdef DEB
#ifdef DRAW
  if (Affich) {
    gp_Vec2d veve(0.,0.);
    Law_draw1dcurve(Curve(),veve,1.);
  }
#endif
#endif
}

//=======================================================================
//function : SetInRelative
//purpose  : 
//=======================================================================

void Law_Interpol::SetInRelative(const TColgp_Array1OfPnt2d& ParAndRad,
				 const Standard_Real Ud,
				 const Standard_Real Uf,
				 const Standard_Boolean Periodic)
{
  Standard_Integer l = ParAndRad.Lower(), u = ParAndRad.Upper();
  Standard_Real wd = ParAndRad(l).X(),wf = ParAndRad(u).X();
  Standard_Integer nbp = ParAndRad.Length();
  Handle(TColStd_HArray1OfReal) par = new TColStd_HArray1OfReal(1,nbp);
  Handle(TColStd_HArray1OfReal) rad;
  if(Periodic) rad = new TColStd_HArray1OfReal(1,nbp - 1);
  else rad = new TColStd_HArray1OfReal(1,nbp);
  Standard_Real x,y;
  Standard_Integer i;
  for(i = 1; i <= nbp; i++){
    ParAndRad(l + i - 1).Coord(x,y);
    par->SetValue(i,(Uf*(x-wd)+Ud*(wf-x))/(wf-wd));
    if(!Periodic || i != nbp) rad->SetValue(i,y);
  }
  Law_Interpolate inter(rad,par,Periodic,Precision::Confusion());
  inter.Perform();
  SetCurve(inter.Curve()); 
#ifdef DEB
#ifdef DRAW
  if (Affich) {
    gp_Vec2d veve(0.,0.);
    Law_draw1dcurve(Curve(),veve,1.);
  }
#endif
#endif
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void Law_Interpol::Set(const TColgp_Array1OfPnt2d& ParAndRad,
		       const Standard_Real Dd,
		       const Standard_Real Df,
		       const Standard_Boolean Periodic)
{
  Standard_Integer l = ParAndRad.Lower();
  Standard_Integer nbp = ParAndRad.Length();

  Handle(TColStd_HArray1OfReal) par = new TColStd_HArray1OfReal(1,nbp);
  Handle(TColStd_HArray1OfReal) rad;
  if(Periodic) rad = new TColStd_HArray1OfReal(1,nbp - 1);
  else rad = new TColStd_HArray1OfReal(1,nbp);
  Standard_Real x,y;
  Standard_Integer i;
  for(i = 1; i <= nbp; i++){
    ParAndRad(l + i - 1).Coord(x,y);
    par->SetValue(i,x);
    if(!Periodic || i != nbp) rad->SetValue(i,y);
  }
  Law_Interpolate inter(rad,par,Periodic,Precision::Confusion());
  inter.Load(Dd,Df);
  inter.Perform();
  SetCurve(inter.Curve()); 
}

//=======================================================================
//function : SetInRelative
//purpose  : 
//=======================================================================

void Law_Interpol::SetInRelative(const TColgp_Array1OfPnt2d& ParAndRad,
				 const Standard_Real Ud,
				 const Standard_Real Uf,
				 const Standard_Real Dd,
				 const Standard_Real Df,
				 const Standard_Boolean Periodic)
{
  Standard_Integer l = ParAndRad.Lower(), u = ParAndRad.Upper();
  Standard_Real wd = ParAndRad(l).X(),wf = ParAndRad(u).X();
  Standard_Integer nbp = ParAndRad.Length();
  Handle(TColStd_HArray1OfReal) par = new TColStd_HArray1OfReal(1,nbp);
  Handle(TColStd_HArray1OfReal) rad;
  if(Periodic) rad = new TColStd_HArray1OfReal(1,nbp - 1);
  else rad = new TColStd_HArray1OfReal(1,nbp);
  Standard_Real x,y;
  Standard_Integer i;
  for(i = 1; i <= nbp; i++){
    ParAndRad(l + i - 1).Coord(x,y);
    par->SetValue(i,(Uf*(x-wd)+Ud*(wf-x))/(wf-wd));
    if(!Periodic || i != nbp) rad->SetValue(i,y);
  }
  Law_Interpolate inter(rad,par,Periodic,Precision::Confusion());
  inter.Load(Dd,Df);
  inter.Perform();
  SetCurve(inter.Curve()); 
}
