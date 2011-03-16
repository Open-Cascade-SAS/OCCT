// ISession_Direction.cpp: implementation of the ISession_Direction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ISession_Direction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

IMPLEMENT_STANDARD_HANDLE(ISession_Direction,AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(ISession_Direction,AIS_InteractiveObject)

#include "DsgPrs_LengthPresentation.hxx"
#include "Prs3d_ArrowAspect.hxx"
#include "AIS_Drawer.hxx"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ISession_Direction::ISession_Direction(gp_Pnt& aPnt,gp_Pnt& aPnt2)
:myStartPnt(aPnt),myEndPnt(aPnt2)
{}

ISession_Direction::ISession_Direction(gp_Pnt& aPnt,gp_Vec& aVec)
:myStartPnt(aPnt)
{
  myEndPnt = myStartPnt.Translated(aVec);
}


void ISession_Direction::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                             const Handle(Prs3d_Presentation)& aPresentation,
                             const Standard_Integer aMode)
{
    Handle(Prs3d_ArrowAspect) anArrowAspect = myDrawer->ArrowAspect();
    anArrowAspect->SetLength(myStartPnt.Distance(myEndPnt));
    myDrawer->SetArrowAspect(anArrowAspect);

    DsgPrs_LengthPresentation::Add(aPresentation,myDrawer,
		                           myStartPnt,myEndPnt,
								   DsgPrs_AS_LASTAR);
}


