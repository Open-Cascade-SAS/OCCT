#include <ISession_Surface.ixx>
#include <AIS_InteractiveObject.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <AIS_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <StdPrs_ShadedSurface.hxx>
#include <StdPrs_WFPoleSurface.hxx>
#include <StdPrs_WFSurface.hxx>


ISession_Surface::ISession_Surface()
{
}

ISession_Surface::ISession_Surface(const Handle(Geom_Surface)& aSurface)
     :AIS_InteractiveObject(),mySurface(aSurface)
{
}

 void ISession_Surface::Compute(const Handle(PrsMgr_PresentationManager3d)& ,const Handle(Prs3d_Presentation)& aPresentation,const Standard_Integer aMode) 
{
  GeomAdaptor_Surface anAdaptorSurface(mySurface);
  Handle(GeomAdaptor_HSurface) anAdaptorHSurface = new GeomAdaptor_HSurface(mySurface);
  
  Handle(AIS_Drawer) aDrawer = new AIS_Drawer();
  aDrawer->LineAspect()->SetColor(Quantity_NOC_YELLOW3);
  
  switch (aMode)
  {
    case 2:
      StdPrs_ShadedSurface::Add(aPresentation,anAdaptorSurface,myDrawer);
      break;
    case 1 :
      StdPrs_WFPoleSurface::Add(aPresentation,anAdaptorSurface,aDrawer);
    case 0 :
      StdPrs_WFSurface::Add(aPresentation,anAdaptorHSurface,myDrawer);
      break;
  }
}

 void ISession_Surface::Compute(const Handle(Prs3d_Projector)& ,const Handle(Prs3d_Presentation)& ) 
{
}

 void ISession_Surface::ComputeSelection(const Handle(SelectMgr_Selection)& ,const Standard_Integer ) 
{
}

