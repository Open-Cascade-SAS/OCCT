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

#include <DrawTrSurf_Surface.ixx>
#include <GeomTools_SurfaceSet.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <Precision.hxx>

Standard_Real DrawTrSurf_SurfaceLimit = 400;


//=======================================================================
//function : DrawTrSurf_Surface
//purpose  : 
//=======================================================================

DrawTrSurf_Surface::DrawTrSurf_Surface (const Handle(Geom_Surface)& S) 
: DrawTrSurf_Drawable (16, 0.01, 1) 
{
  surf = S;
  boundsLook = Draw_jaune;
  isosLook = Draw_bleu;
  nbUIsos = 1;
  nbVIsos = 1;
}




//=======================================================================
//function : DrawTrSurf_Surface
//purpose  : 
//=======================================================================

DrawTrSurf_Surface::DrawTrSurf_Surface 
  (const Handle(Geom_Surface)& S, const Standard_Integer Nu, 
   const Standard_Integer Nv,
   const Draw_Color& BoundsColor, const Draw_Color& IsosColor,
   const Standard_Integer Discret, const Standard_Real Deflection,
   const Standard_Integer DrawMode) 
:  DrawTrSurf_Drawable (Discret, Deflection, DrawMode) 
{
  surf = S;
  boundsLook = BoundsColor;
  isosLook = IsosColor;
  nbUIsos = Abs(Nu);
  nbVIsos = Abs(Nv);
}



//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawTrSurf_Surface::DrawOn (Draw_Display& dis) const 
{
  DrawOn(dis,Standard_True);
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawTrSurf_Surface::DrawOn (Draw_Display& dis,
				 const Standard_Boolean Iso) const 
{
  Standard_Real UFirst, ULast, VFirst, VLast;
  surf->Bounds (UFirst, ULast, VFirst, VLast);
  
  Standard_Boolean UfirstInf = Precision::IsNegativeInfinite(UFirst);
  Standard_Boolean UlastInf  = Precision::IsPositiveInfinite(ULast);
  Standard_Boolean VfirstInf = Precision::IsNegativeInfinite(VFirst);
  Standard_Boolean VlastInf  = Precision::IsPositiveInfinite(VLast);
  
  if (UfirstInf || UlastInf) {
    gp_Pnt P1,P2;
    Standard_Real v;
    if (VfirstInf && VlastInf) 
      v = 0;
    else if (VfirstInf)
      v = VLast;
    else if (VlastInf)
      v = VFirst;
    else
      v = (VFirst + VLast) / 2;
    
    Standard_Real delta = 1.;

    if (UfirstInf && UlastInf) {
      do {
	delta *= 2;
	UFirst = - delta;
	ULast  =   delta;
	surf->D0(UFirst,v,P1);
	surf->D0(ULast,v,P2);
      } while (P1.Distance(P2) < DrawTrSurf_SurfaceLimit);
    }
    else if (UfirstInf) {
      surf->D0(ULast,v,P2);
      do {
	delta *= 2;
	UFirst = ULast - delta;
	surf->D0(UFirst,v,P1);
      } while (P1.Distance(P2) < DrawTrSurf_SurfaceLimit);
    }
    else if (UlastInf) {
      surf->D0(UFirst,v,P1);
      do {
	delta *= 2;
	ULast = UFirst + delta;
	surf->D0(ULast,v,P2);
      } while (P1.Distance(P2) < DrawTrSurf_SurfaceLimit);
    }
  }

  if (VfirstInf || VlastInf) {
    gp_Pnt P1,P2;
    Standard_Real u = (UFirst + ULast) /2 ;

    Standard_Real delta = 1.;

    if (VfirstInf && VlastInf) {
      do {
	delta *= 2;
	VFirst = - delta;
	VLast  =   delta;
	surf->D0(u,VFirst,P1);
	surf->D0(u,VLast,P2);
      } while (P1.Distance(P2) < DrawTrSurf_SurfaceLimit);
    }
    else if (VfirstInf) {
      surf->D0(u,VLast,P2);
      do {
	delta *= 2;
	VFirst = VLast - delta;
	surf->D0(u,VFirst,P1);
      } while (P1.Distance(P2) < DrawTrSurf_SurfaceLimit);
    }
    else if (VlastInf) {
      surf->D0(u,VFirst,P1);
      do {
	delta *= 2;
	VLast = VFirst + delta;
	surf->D0(u,VLast,P2);
      } while (P1.Distance(P2) < DrawTrSurf_SurfaceLimit);
    }
  }


  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface();
  HS->ChangeSurface().Load(surf,UFirst,ULast,VFirst,VLast);

  Adaptor3d_IsoCurve C(HS);
  
  if (Iso) {
    dis.SetColor(isosLook);
    Standard_Integer i, j;
    
    Standard_Real Du = (ULast - UFirst) / (nbUIsos + 1);
    Standard_Real U = UFirst;
    for (i = 1; i <= nbUIsos; i++) {
      U += Du;
      DrawIsoCurveOn(C,GeomAbs_IsoU,U,VFirst,VLast,dis);
    }
    
    Standard_Real Dv = (VLast - VFirst) / (nbVIsos + 1);
    Standard_Real V = VFirst;
    for (j = 1; j <= nbVIsos; j++) {
      V += Dv;
      DrawIsoCurveOn(C,GeomAbs_IsoV,V,UFirst,ULast,dis);
    }
  }

  // draw bounds
  dis.SetColor(boundsLook);
  if (!UfirstInf) DrawIsoCurveOn(C,GeomAbs_IsoU,UFirst,VFirst,VLast,dis);
  if (!UlastInf)  DrawIsoCurveOn(C,GeomAbs_IsoU,ULast ,VFirst,VLast,dis);
  if (!VfirstInf) DrawIsoCurveOn(C,GeomAbs_IsoV,VFirst,UFirst,ULast,dis);
  if (!VlastInf)  DrawIsoCurveOn(C,GeomAbs_IsoV,VLast ,UFirst,ULast,dis);

  // draw marker
  DrawIsoCurveOn(C,GeomAbs_IsoU,
		 UFirst+(ULast-UFirst)/10.,
		 VFirst, VFirst + (VLast-VFirst)/10.,
		 dis);
}



//=======================================================================
//function : ShowIsos
//purpose  : 
//=======================================================================

void DrawTrSurf_Surface::ShowIsos ( const Standard_Integer Nu, 
				    const Standard_Integer Nv) 
{
  nbUIsos = Abs(Nu);
  nbVIsos = Abs(Nv);
}


//=======================================================================
//function : ClearIsos
//purpose  : 
//=======================================================================

void DrawTrSurf_Surface::ClearIsos () 
{
  nbUIsos = 0;
  nbVIsos = 0;
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D)  DrawTrSurf_Surface::Copy() const 
{
  Handle(DrawTrSurf_Surface) DS = new DrawTrSurf_Surface
    (Handle(Geom_Surface)::DownCast(surf->Copy()),
     nbUIsos,nbVIsos,boundsLook,isosLook,
     GetDiscretisation(),GetDeflection(),GetDrawMode());
     
  return DS;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  DrawTrSurf_Surface::Dump(Standard_OStream& S)const 
{
  GeomTools_SurfaceSet::PrintSurface(surf,S);
}


//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void  DrawTrSurf_Surface::Whatis(Draw_Interpretor& S)const 
{
  S << "a surface";
}
