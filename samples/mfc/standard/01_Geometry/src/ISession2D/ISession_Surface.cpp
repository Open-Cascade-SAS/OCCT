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

void ISession_Surface::Compute (const Handle(PrsMgr_PresentationManager3d)& ,
                                const Handle(Prs3d_Presentation)& thePrs,
                                const Standard_Integer theMode)
{
  GeomAdaptor_Surface anAdaptorSurface (mySurface);
  Handle(GeomAdaptor_HSurface) anAdaptorHSurface = new GeomAdaptor_HSurface (mySurface);

  Handle(Prs3d_Drawer) aPoleDrawer = new Prs3d_Drawer();
  aPoleDrawer->SetLineAspect (new Prs3d_LineAspect (Quantity_NOC_YELLOW3, Aspect_TOL_SOLID, 1.0));
  switch (theMode)
  {
    case 2:
    {
	    StdPrs_ShadedSurface::Add (thePrs, anAdaptorSurface, myDrawer);
      break;
    }
    case 1:
    {
      StdPrs_WFPoleSurface::Add (thePrs, anAdaptorSurface, aPoleDrawer);
    }
    case 0:
    {
      StdPrs_WFSurface::Add (thePrs, anAdaptorHSurface, myDrawer);
      break;
    }
  }
}
