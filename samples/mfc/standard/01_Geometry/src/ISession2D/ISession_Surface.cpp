// ISession_Surface.cpp: implementation of the ISession_Surface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\GeometryApp.h"
#include "ISession_Surface.h"
#include <GeomAdaptor_HSurface.hxx>
#include <StdPrs_ShadedSurface.hxx>
#include <StdPrs_WFPoleSurface.hxx>
#include <StdPrs_WFSurface.hxx>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

IMPLEMENT_STANDARD_RTTIEXT(ISession_Surface,AIS_InteractiveObject)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ISession_Surface::ISession_Surface(const Handle(Geom_Surface)& aSurface)
:AIS_InteractiveObject(),mySurface(aSurface)
{
}

ISession_Surface::~ISession_Surface()
{

}
void ISession_Surface::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                             const Handle(Prs3d_Presentation)& aPresentation,
                             const Standard_Integer aMode)
{

    GeomAdaptor_Surface anAdaptorSurface(mySurface);
    Handle(GeomAdaptor_HSurface) anAdaptorHSurface = new GeomAdaptor_HSurface(mySurface);

    Handle(Prs3d_Drawer) aDrawer = new Prs3d_Drawer();
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

void ISession_Surface::Compute(const Handle(Prs3d_Projector)& /*aProjector*/,
                               const Handle(Prs3d_Presentation)& /*aPresentation*/) 
{
}

void ISession_Surface::ComputeSelection(const Handle(SelectMgr_Selection)& /*aSelection*/,
				        const Standard_Integer /*aMode*/) 
{ 
}