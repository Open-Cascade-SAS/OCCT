#include <ISession2D_Curve.ixx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <GGraphic2d_SetOfCurves.hxx>
#include <Graphic2d_Array1OfVertex.hxx>
#include <Graphic2d_Vertex.hxx>
#include <Graphic2d_Polyline.hxx>
#include <Graphic2d_Segment.hxx>
#include <gp_Pnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>


ISession2D_Curve::ISession2D_Curve(const Handle(Geom2d_Curve)& aGeom2dCurve,const Aspect_TypeOfLine aTypeOfLine,const Aspect_WidthOfLine aWidthOfLine,const Standard_Integer aColorIndex)
     :AIS_InteractiveObject()
{
  myGeom2dCurve = aGeom2dCurve;
  myTypeOfLine = aTypeOfLine;
  myWidthOfLine = aWidthOfLine;
  myColorIndex = aColorIndex;
  myDisplayPole = Standard_True;
  myDisplayCurbure = Standard_False;
  myDiscretisation = 20;
  myradiusmax = 10;
  myradiusratio = 1;
}

 void ISession2D_Curve::Compute(const Handle(PrsMgr_PresentationManager2d)& ,const Handle(Graphic2d_GraphicObject)& aGrObj,const Standard_Integer ) 
{
  Handle(GGraphic2d_SetOfCurves) segment;
  segment = new GGraphic2d_SetOfCurves(aGrObj);
  
  segment->Add(myGeom2dCurve);
  
  segment->SetColorIndex (myColorIndex);
  segment->SetWidthIndex (myWidthOfLine + 1);
  segment->SetTypeIndex  (myTypeOfLine + 1);

  Geom2dAdaptor_Curve anAdaptor(myGeom2dCurve);
  if (myDisplayPole)
    {
      if (anAdaptor.GetType() == GeomAbs_BezierCurve)
	{
	  Handle(Geom2d_BezierCurve) aBezier = anAdaptor.Bezier();
	  Graphic2d_Array1OfVertex anArrayOfVertex(1,aBezier->NbPoles());
	  for (int i=1;i<=aBezier->NbPoles();i++)
	    {
	      gp_Pnt2d CurrentPoint = aBezier->Pole(i);
	      Graphic2d_Vertex aVertex(CurrentPoint.X(),CurrentPoint.Y());
	      anArrayOfVertex(i) = aVertex;
	    }
	  Handle(Graphic2d_Polyline) aPolyline = new Graphic2d_Polyline(aGrObj, anArrayOfVertex);
	}

      if (anAdaptor.GetType() == GeomAbs_BSplineCurve)
	{
	  Handle(Geom2d_BSplineCurve) aBSpline = anAdaptor.BSpline();
	  Graphic2d_Array1OfVertex anArrayOfVertex(1,aBSpline->NbPoles());
	  for (int i=1;i<=aBSpline->NbPoles();i++)
	    {
	      gp_Pnt2d CurrentPoint = aBSpline->Pole(i);
	      Graphic2d_Vertex aVertex(CurrentPoint.X(),CurrentPoint.Y());
	      anArrayOfVertex(i) = aVertex;
	    }
	  Handle(Graphic2d_Polyline) aPolyline = new Graphic2d_Polyline(aGrObj,anArrayOfVertex);
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

      for (intrv=1; intrv<=nbintv; intrv++)
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
		  if (Curvature > Resolution)
		    {
		      myGeom2dCurve->D0(t, P1);
		      LRad = 1./Curvature;
		      ratio = ((LRad>myradiusmax)?myradiusmax/LRad:1);
		      ratio *= myradiusratio;
		      LProp.CentreOfCurvature(P2);
		      gp_Vec2d V(P1,P2);
		      gp_Pnt2d P3 = P1.Translated(ratio*V);
		      Handle(Graphic2d_Segment) aSegment = new Graphic2d_Segment(aGrObj,P1.X(),P1.Y(),P3.X(),P3.Y());
		    }
		}
	      t += step;
	    }
	}
    }
}

 void ISession2D_Curve::ComputeSelection(const Handle(SelectMgr_Selection)& ,const Standard_Integer ) 
{
}

