// ISession_Direction.cpp: implementation of the ISession_Direction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\GeometryApp.h"
#include "ISession_Direction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

IMPLEMENT_STANDARD_HANDLE(ISession_Direction,AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(ISession_Direction,AIS_InteractiveObject)

#include "Graphic2d_SetOfSegments.hxx"
#include "gp_Vec2d.hxx"
#include "DsgPrs_LengthPresentation.hxx"
#include "Prs3d_ArrowAspect.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ISession_Direction::ISession_Direction()
{

}

ISession_Direction::ISession_Direction(gp_Pnt& aPnt,gp_Dir&   aDir,Standard_Real aLength,Standard_Real anArrowLength)
:myPnt(aPnt),myDir(aDir),myLength(aLength),myArrowLength(anArrowLength)
{}

ISession_Direction::ISession_Direction(gp_Pnt& aPnt,gp_Vec&   aVec,Standard_Real anArrowLength)
:myPnt(aPnt),myDir(aVec),myArrowLength(anArrowLength)
{
  myLength = aVec.Magnitude();
}

ISession_Direction::ISession_Direction(gp_Pnt2d& aPnt2d,
                                       gp_Dir2d& aDir2d,
                                       Standard_Real aLength)
:myPnt(gp_Pnt(aPnt2d.X(),aPnt2d.Y(),0)),myDir(gp_Dir(aDir2d.X(),aDir2d.Y(),0)),myLength(aLength)
{}

ISession_Direction::ISession_Direction(gp_Pnt2d& aPnt2d,
                                       gp_Vec2d&   aVec2d)
:myPnt(gp_Pnt(aPnt2d.X(),aPnt2d.Y(),0)),myDir(gp_Dir(aVec2d.X(),aVec2d.Y(),0))
{
  myLength = aVec2d.Magnitude();
}


ISession_Direction::~ISession_Direction()
{

}

void ISession_Direction::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                             const Handle(Prs3d_Presentation)& aPresentation,
                             const Standard_Integer aMode)
{
    Handle(Prs3d_ArrowAspect) anArrowAspect = myDrawer->ArrowAspect();
    anArrowAspect->SetLength(myArrowLength);
    myDrawer->SetArrowAspect(anArrowAspect);

    gp_Pnt LastPoint = myPnt  ;
    LastPoint.Translate (myLength*gp_Vec(myDir))  ;

    if (myText.Length() == 0)
        DsgPrs_LengthPresentation::Add(aPresentation,myDrawer,myPnt,LastPoint,DsgPrs_AS_LASTAR);
    else
    {
        gp_Pnt OffsetPoint = myPnt;
        OffsetPoint.Translate ( (myLength) *gp_Vec(myDir))  ;
        DsgPrs_LengthPresentation::Add(aPresentation,myDrawer,myText,myPnt,LastPoint, -myDir, OffsetPoint,  DsgPrs_AS_LASTAR);
    }
}


void ISession_Direction::Compute(const Handle(Prs3d_Projector)& aProjector,
                             const Handle(Prs3d_Presentation)& aPresentation) 
 {
 }

void ISession_Direction::Compute(const Handle(PrsMgr_PresentationManager2d)& aPresentationManager,
			     const Handle(Graphic2d_GraphicObject)& aGrObj,
			     const Standard_Integer aMode) 
{
    Handle(Graphic2d_SetOfSegments) segment;
    segment = new Graphic2d_SetOfSegments(aGrObj);

    segment->Add(myPnt.X(),
                 myPnt.Y(),
                 myPnt.X()+myLength*myDir.X(),
                 myPnt.Y()+myLength*myDir.Y());

    gp_Dir2d aFirstDir = gp_Dir2d(myDir.X(),myDir.Y()).Rotated (200*PI180) ;
    segment->Add(myPnt.X()+myLength*myDir.X(),
                 myPnt.Y()+myLength*myDir.Y(),
                 myPnt.X()+myLength*myDir.X()+ myLength*aFirstDir.X()/5 ,
                 myPnt.Y()+myLength*myDir.Y()+ myLength*aFirstDir.Y()/5 );

    gp_Dir2d aSecondDir = gp_Dir2d(myDir.X(),myDir.Y()).Rotated (-200*PI180) ;
    segment->Add(myPnt.X()+myLength*myDir.X(),
                 myPnt.Y()+myLength*myDir.Y(),
                 myPnt.X()+myLength*myDir.X()+ myLength*aSecondDir.X()/5 ,
                 myPnt.Y()+myLength*myDir.Y()+ myLength*aSecondDir.Y()/5 );

}
void ISession_Direction::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
				      const Standard_Integer aMode) 
{ 
}

void ISession_Direction::SetText(TCollection_ExtendedString & aText)
{
    myText = aText;
}
