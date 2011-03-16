#include <ISession_Curve.ixx>
#include <GeomAdaptor_Curve.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Drawer.hxx>
#include <StdPrs_PoleCurve.hxx>
#include <Prs3d_LineAspect.hxx>
#include <StdPrs_Curve.hxx>



ISession_Curve::ISession_Curve(const Handle(Geom_Curve)& aCurve)
     :AIS_InteractiveObject(),myCurve(aCurve)
{
}

 void ISession_Curve::Compute(const Handle(PrsMgr_PresentationManager3d)& ,const Handle(Prs3d_Presentation)& aPresentation,const Standard_Integer aMode) 
{
  GeomAdaptor_Curve anAdaptorCurve(myCurve);
  Handle(AIS_Drawer) aDrawer = new AIS_Drawer();
  aDrawer->LineAspect()->SetColor(Quantity_NOC_RED);
  
  switch (aMode)
  {
    case 1 :
      StdPrs_PoleCurve::Add(aPresentation, anAdaptorCurve,aDrawer);
    case 0 :
      StdPrs_Curve::Add( aPresentation, anAdaptorCurve ,myDrawer);
      break;
  }
}

 void ISession_Curve::Compute(const Handle(Prs3d_Projector)& ,const Handle(Prs3d_Presentation)& ) 
{
}

 void ISession_Curve::ComputeSelection(const Handle(SelectMgr_Selection)& ,const Standard_Integer ) 
{
}

