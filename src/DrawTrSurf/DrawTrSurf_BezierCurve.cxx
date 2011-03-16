#include <DrawTrSurf_BezierCurve.ixx>
#include <Geom_BezierCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

   DrawTrSurf_BezierCurve::DrawTrSurf_BezierCurve (
   const Handle(Geom_BezierCurve)& C) 
   : DrawTrSurf_Curve (C, Draw_vert, 16, 0.05, 1) {

      drawPoles = Standard_True;
      polesLook = Draw_rouge;
   }



   DrawTrSurf_BezierCurve::DrawTrSurf_BezierCurve (
   const Handle(Geom_BezierCurve)& C, const Draw_Color& CurvColor,
   const Draw_Color& PolesColor, const Standard_Boolean ShowPoles, 
   const Standard_Integer Discret,const Standard_Real Deflection,
   const Standard_Integer DrawMode ) : 
   DrawTrSurf_Curve (C, CurvColor, Discret, Deflection, DrawMode) {

      drawPoles = ShowPoles;
      polesLook = PolesColor;
  }


   void DrawTrSurf_BezierCurve::DrawOn (Draw_Display& dis) const {


    Handle(Geom_BezierCurve) C = Handle(Geom_BezierCurve)::DownCast(curv);

    if (drawPoles) {
      Standard_Integer NbPoles = C->NbPoles();
      dis.SetColor(polesLook);
      TColgp_Array1OfPnt CPoles (1, NbPoles);
      C->Poles (CPoles);
      dis.MoveTo(CPoles(1));
      for (Standard_Integer i = 2; i <= NbPoles; i++) {
        dis.DrawTo(CPoles(i));
      }
    }

    DrawTrSurf_Curve::DrawOn(dis);
  }



   void DrawTrSurf_BezierCurve::ShowPoles () { drawPoles = Standard_True; }


   void DrawTrSurf_BezierCurve::ClearPoles () { drawPoles = Standard_False; }


   void DrawTrSurf_BezierCurve::FindPole (
   const Standard_Real X, const Standard_Real Y, const Draw_Display& D,
   const Standard_Real XPrec, Standard_Integer& Index) const {
    
     Handle(Geom_BezierCurve) bz = Handle(Geom_BezierCurve)::DownCast(curv);
     gp_Pnt2d p1(X/D.Zoom(),Y/D.Zoom());
     Standard_Real Prec = XPrec / D.Zoom();
     Index++;
     Standard_Integer NbPoles = bz->NbPoles();
     while (Index <= NbPoles) {
       if (D.Project(bz->Pole(Index)).Distance(p1) <= Prec)
	 return;
       Index++;
     }
     Index = 0;
   }


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D)  DrawTrSurf_BezierCurve::Copy()const 
{
  Handle(DrawTrSurf_BezierCurve) DC = new DrawTrSurf_BezierCurve
    (Handle(Geom_BezierCurve)::DownCast(curv->Copy()),
     look,polesLook,
     drawPoles,
     GetDiscretisation(),GetDeflection(),GetDrawMode());
     
  return DC;
}





