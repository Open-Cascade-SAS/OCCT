// ISession_Direction.cpp: implementation of the ISession_Direction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ISession_Direction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

IMPLEMENT_STANDARD_RTTIEXT(ISession_Direction,AIS_InteractiveObject)

#include "DsgPrs_LengthPresentation.hxx"
#include "Prs3d_ArrowAspect.hxx"
#include "Prs3d_Drawer.hxx"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ISession_Direction::ISession_Direction(const gp_Pnt& aPnt,const gp_Pnt& aPnt2)
:myStartPnt(aPnt),myEndPnt(aPnt2)
{}

ISession_Direction::ISession_Direction(const gp_Pnt& aPnt,const gp_Vec& aVec)
:myStartPnt(aPnt)
{
  myEndPnt = myStartPnt.Translated(aVec);
}


void ISession_Direction::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                                 const Handle(Prs3d_Presentation)& aPresentation,
                                 const Standard_Integer /*aMode*/)
{
  // Set style for arrow
  Handle(Prs3d_ArrowAspect) anArrowAspect = myDrawer->ArrowAspect();

  // Draw Line
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments (2);
  aPrims->AddVertex (myStartPnt);
  aPrims->AddVertex (myEndPnt);
  Prs3d_Root::CurrentGroup (aPresentation)->SetPrimitivesAspect (myDrawer->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup (aPresentation)->AddPrimitiveArray (aPrims);
  // Draw arrow
  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation),
                     myEndPnt,
                     gp_Dir (gp_Vec(myStartPnt, myEndPnt)),
                     anArrowAspect->Angle(),
                     anArrowAspect->Length());
}
