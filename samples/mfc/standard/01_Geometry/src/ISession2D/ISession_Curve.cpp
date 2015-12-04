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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ISession_Curve::ISession_Curve(Handle(Geom_Curve)& aCurve)
:AIS_InteractiveObject(),myCurve(aCurve)
{

}

ISession_Curve::~ISession_Curve()
{

}
void ISession_Curve::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                             const Handle(Prs3d_Presentation)& aPresentation,
                             const Standard_Integer aMode)
{
    GeomAdaptor_Curve anAdaptorCurve(myCurve);
    Handle(Prs3d_Drawer) aDrawer = new Prs3d_Drawer();
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

void ISession_Curve::Compute(const Handle(Prs3d_Projector)& /*aProjector*/,
                             const Handle(Prs3d_Presentation)& /*aPresentation*/) 
 {

 }

void ISession_Curve::ComputeSelection(const Handle(SelectMgr_Selection)& /*aSelection*/,
				      const Standard_Integer /*aMode*/) 
{ 
}


