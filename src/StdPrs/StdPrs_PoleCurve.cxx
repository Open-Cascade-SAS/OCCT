// File:	StdPrs_PoleCurve.cxx
// Created:	Mon Jul 24 15:11:50 1995
// Author:	Modelistation
//		<model@metrox>


#include <StdPrs_PoleCurve.ixx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void StdPrs_PoleCurve::Add (const Handle (Prs3d_Presentation)& aPresentation,
			    const Adaptor3d_Curve&               aCurve,
			    const Handle (Prs3d_Drawer)&       aDrawer) 
{
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(aDrawer->LineAspect()->Aspect());

  GeomAbs_CurveType CType = aCurve.GetType();
  if (CType == GeomAbs_BezierCurve || CType == GeomAbs_BSplineCurve) {
    Standard_Real x,y,z;
    Standard_Integer i, Nb;
    if (CType == GeomAbs_BezierCurve) {
      Handle(Geom_BezierCurve) Bz = aCurve.Bezier();
      Nb = Bz->NbPoles();
      Graphic3d_Array1OfVertex VertexArray(1, Nb);
      for (i = 1; i <= Nb; i++) { 
	(Bz->Pole(i)).Coord(x,y,z);
	VertexArray(i).SetCoord(x,y,z);
      }
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VertexArray);
    }
    else if (CType == GeomAbs_BSplineCurve) {
      Handle(Geom_BSplineCurve) Bs = aCurve.BSpline();
      Nb = Bs->NbPoles();
      Graphic3d_Array1OfVertex VertexArray(1, Nb);
      for (i = 1; i <= Nb; i++) { 
	(Bs->Pole(i)).Coord(x,y,z);
	VertexArray(i).SetCoord(x,y,z);
      }
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VertexArray);
    }
  }
  
  if (aDrawer->LineArrowDraw()) {
    gp_Pnt Location;
    gp_Vec Direction;
    aCurve.D1(aCurve.LastParameter(),Location,Direction);
    Prs3d_Arrow::Draw (aPresentation,
		       Location,
		       gp_Dir(Direction),
		       aDrawer->ArrowAspect()->Angle(),
		       aDrawer->ArrowAspect()->Length());
  }
}
     

//=======================================================================
//function : Match
//purpose  : 
//=======================================================================

Standard_Boolean StdPrs_PoleCurve::Match(const Quantity_Length        X,
					 const Quantity_Length        Y,
					 const Quantity_Length        Z,
					 const Quantity_Length        aDistance,
					 const Adaptor3d_Curve&         aCurve,
					 const Handle (Prs3d_Drawer)& aDrawer) 
{
  GeomAbs_CurveType CType = aCurve.GetType();
  Standard_Integer i, Nb = 0;
  Standard_Real x,y,z;
  if (CType == GeomAbs_BezierCurve) {
    Handle(Geom_BezierCurve) Bz = aCurve.Bezier();
    Nb = Bz->NbPoles();
    for (i = 1; i <= Nb; i++) { 
      Bz->Pole(i).Coord(x,y,z);
      if ( Abs(X-x) +Abs(Y-y)+Abs(Z-z) <= aDistance) return Standard_True;
    }
    return Standard_False;
  }
  else if (CType == GeomAbs_BSplineCurve) {
    Handle(Geom_BSplineCurve) Bs = aCurve.BSpline();
    Nb = Bs->NbPoles();
    for (i = 1; i <= Nb; i++) { 
      Bs->Pole(i).Coord(x,y,z);
      if ( Abs(X-x) +Abs(Y-y)+Abs(Z-z) <= aDistance) return Standard_True;
    }
    return Standard_False;
  }
  return Standard_False;
}

//=======================================================================
//function : Pick
//purpose  : 
//=======================================================================

Standard_Integer StdPrs_PoleCurve::Pick
             (const Quantity_Length       X,
	      const Quantity_Length       Y,
	      const Quantity_Length       Z,
	      const Quantity_Length       aDistance,
	      const Adaptor3d_Curve&        aCurve,
	      const Handle(Prs3d_Drawer)& aDrawer) 
{
  Standard_Real x, y, z, DistMin = RealLast();
  Standard_Integer num = 0, i, Nb = 0;
  Standard_Real dist;
  GeomAbs_CurveType CType = aCurve.GetType();

  if (CType == GeomAbs_BezierCurve) {
    Handle(Geom_BezierCurve) Bz = aCurve.Bezier();
    Nb = Bz->NbPoles();
    for (i = 1; i <= Nb; i++) { 
      Bz->Pole(i).Coord(x,y,z);
      dist = Abs(X-x) +Abs(Y-y)+Abs(Z-z);
      if ( dist <= aDistance) {
	if (dist < DistMin) {
	  DistMin = dist;
	  num = i;
	}
      }
    }
  }
  else if (CType == GeomAbs_BSplineCurve) {
    Handle(Geom_BSplineCurve) Bs = aCurve.BSpline();
    Nb = Bs->NbPoles();
    for (i = 1; i <= Nb; i++) { 
      Bs->Pole(i).Coord(x,y,z);
      dist = Abs(X-x) +Abs(Y-y)+Abs(Z-z);
      if (dist <= aDistance) {
	if (dist < DistMin) {
	  DistMin = dist;
	  num = i;
	}
      }
    }
  }

  return num;
}



     



