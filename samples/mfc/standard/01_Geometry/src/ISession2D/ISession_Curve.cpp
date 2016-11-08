// ISession_Curve.cpp: implementation of the ISession_Curve class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\GeometryApp.h"
#include "ISession_Curve.h"
#include <StdPrs_PoleCurve.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ISession_Curve,AIS_InteractiveObject)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

void ISession_Curve::Compute (const Handle(PrsMgr_PresentationManager3d)& ,
                              const Handle(Prs3d_Presentation)& thePrs,
                              const Standard_Integer theMode)
{
  GeomAdaptor_Curve anAdaptorCurve (myCurve);
  switch (theMode)
  {
    case 1:
    {
      Handle(Prs3d_Drawer) aPoleDrawer = new Prs3d_Drawer();
      aPoleDrawer->SetLineAspect (new Prs3d_LineAspect (Quantity_NOC_RED, Aspect_TOL_SOLID, 1.0));
      StdPrs_PoleCurve::Add (thePrs, anAdaptorCurve, aPoleDrawer);
    }
    case 0:
    {
      StdPrs_Curve::Add (thePrs, anAdaptorCurve, myDrawer);
      break;
    }
  }
}
