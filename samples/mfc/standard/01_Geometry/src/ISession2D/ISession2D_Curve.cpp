#include "stdafx.h"

#include <ISession2D_Curve.h>
#include "..\\GeometryApp.h"
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Geom2d_BSplineCurve.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ISession2D_Curve,AIS_InteractiveObject)

ISession2D_Curve::ISession2D_Curve(const Handle(Geom2d_Curve) aGeom2dCurve,
                                   const Aspect_TypeOfLine aTypeOfLine,
                                   const Aspect_WidthOfLine aWidthOfLine,
                                   const Standard_Integer aColorIndex)
{
  myGeom2dCurve = aGeom2dCurve;
  myTypeOfLine  = aTypeOfLine ;
  myWidthOfLine = aWidthOfLine;
  myColorIndex  = aColorIndex ;
  myDisplayPole = Standard_True;
  myDisplayCurbure = Standard_False;
  myDiscretisation = 20;
  myradiusmax = 10;
  myradiusratio = 1;
}

void ISession2D_Curve::Compute(const Handle(PrsMgr_PresentationManager3d)& ,
                               const Handle(Prs3d_Presentation)& thePrs,
                               const Standard_Integer )
{
  Handle(Graphic3d_Group) aPrsGroup = Prs3d_Root::CurrentGroup (thePrs);
  aPrsGroup->SetGroupPrimitivesAspect (myDrawer->LineAspect()->Aspect());
  aPrsGroup->SetGroupPrimitivesAspect (myDrawer->PointAspect()->Aspect());

  Geom2dAdaptor_Curve anAdaptor(myGeom2dCurve);
  GCPnts_QuasiUniformDeflection anEdgeDistrib(anAdaptor,1.e-2);
  if(anEdgeDistrib.IsDone())
  {
    Handle(Graphic3d_ArrayOfPolylines) aCurve =
      new Graphic3d_ArrayOfPolylines(anEdgeDistrib.NbPoints()); 
    for(Standard_Integer i=1;i<=anEdgeDistrib.NbPoints();++i)
      aCurve->AddVertex(anEdgeDistrib.Value(i));

    aPrsGroup->AddPrimitiveArray (aCurve);
  }

  if (myDisplayPole) 
  {
    if (anAdaptor.GetType() == GeomAbs_BezierCurve  )
    {
      Handle(Geom2d_BezierCurve) aBezier = anAdaptor.Bezier();
      Handle(Graphic3d_ArrayOfPolylines) anArrayOfVertex = new Graphic3d_ArrayOfPolylines(aBezier->NbPoles());
      for(int i=1;i<=aBezier->NbPoles();i++)
      {
        gp_Pnt2d CurrentPoint = aBezier->Pole(i);
        anArrayOfVertex->AddVertex(CurrentPoint.X(),CurrentPoint.Y(),0.);
      }
      aPrsGroup->AddPrimitiveArray (anArrayOfVertex);
    }

    if (anAdaptor.GetType() == GeomAbs_BSplineCurve  )
    {
      Handle(Geom2d_BSplineCurve) aBSpline = anAdaptor.BSpline();

      Handle(Graphic3d_ArrayOfPolylines) anArrayOfVertex = 
        new Graphic3d_ArrayOfPolylines(aBSpline->NbPoles());

      for(int i=1;i<=aBSpline->NbPoles();i++)
      {
        gp_Pnt2d CurrentPoint = aBSpline->Pole(i);
        anArrayOfVertex->AddVertex(CurrentPoint.X(),CurrentPoint.Y(),0.);
      }
      aPrsGroup->AddPrimitiveArray (anArrayOfVertex);
    }
  }

  if (myDisplayCurbure && (anAdaptor.GetType() != GeomAbs_Line))
  {
    Standard_Integer ii;
    Standard_Integer intrv, nbintv = anAdaptor.NbIntervals(GeomAbs_CN);
    TColStd_Array1OfReal TI(1,nbintv+1);
    anAdaptor.Intervals(TI,GeomAbs_CN);
    Standard_Real Resolution = 1.0e-9, Curvature;
    Geom2dLProp_CLProps2d LProp(myGeom2dCurve, 2, Resolution);
    gp_Pnt2d P1, P2;    

    for (intrv = 1; intrv <= nbintv; intrv++) 
    {
      Standard_Real t = TI(intrv);
      Standard_Real step = (TI(intrv+1) - t) / GetDiscretisation();
      Standard_Real LRad, ratio;
      for (ii = 1; ii <= myDiscretisation; ii++) 
      {	 
        LProp.SetParameter(t);
        if (LProp.IsTangentDefined()) 
        {
          Curvature = Abs(LProp.Curvature());
          if ( Curvature >  Resolution) 
          {
            myGeom2dCurve->D0(t, P1);
            LRad = 1./Curvature;
            ratio = ( (  LRad > myradiusmax) ? myradiusmax/LRad : 1 );
            ratio *= myradiusratio;
            LProp.CentreOfCurvature(P2);
            gp_Vec2d V(P1, P2);
            gp_Pnt2d P3 = P1.Translated(ratio*V);
            Handle(Graphic3d_ArrayOfPolylines) aSegment = new Graphic3d_ArrayOfPolylines(2);
            aSegment->AddVertex(P1.X(),P1.Y(),0.);
            aSegment->AddVertex(P3.X(),P3.Y(),0.);
            aPrsGroup->AddPrimitiveArray (aSegment);
          }
        }
        t += step;
      }
    }
  }
}
