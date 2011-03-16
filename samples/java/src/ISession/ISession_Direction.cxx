#include <ISession_Direction.ixx>
#include <Prs3d_ArrowAspect.hxx>
#include <gp_Pnt.hxx>
#include <DsgPrs_LengthPresentation.hxx>
#include <DsgPrs.hxx>
#include <Graphic2d_SetOfSegments.hxx>
#include <gp_Dir2d.hxx>
#include <AIS_Drawer.hxx>

ISession_Direction::ISession_Direction()
{
}

ISession_Direction::ISession_Direction(const gp_Pnt& aPnt,const gp_Dir& aDir,const Standard_Real aLength,const Standard_Real anArrowLength)
     :myPnt(aPnt),myDir(aDir),myLength(aLength),myArrowLength(anArrowLength)
{
}

ISession_Direction::ISession_Direction(const gp_Pnt& aPnt,const gp_Vec& aVec,const Standard_Real aLength)
     :myPnt(aPnt),myDir(aVec),myArrowLength(aLength)
{
  myLength = aVec.Magnitude();
}

ISession_Direction::ISession_Direction(const gp_Pnt2d& aPnt2d,const gp_Dir2d& aDir2d,const Standard_Real aLength)
     :myPnt(gp_Pnt(aPnt2d.X(),aPnt2d.Y(),0)),myDir(gp_Dir(aDir2d.X(),aDir2d.Y(),0)),myLength(aLength)
{
}

ISession_Direction::ISession_Direction(const gp_Pnt2d& aPnt2d,const gp_Vec2d& aVec2d)
     :myPnt(gp_Pnt(aPnt2d.X(),aPnt2d.Y(),0)),myDir(gp_Dir(aVec2d.X(),aVec2d.Y(),0))
{
  myLength = aVec2d.Magnitude();
}

 void ISession_Direction::SetText(const TCollection_ExtendedString& aText) 
{
  myText = aText;
}

 void ISession_Direction::Compute(const Handle(PrsMgr_PresentationManager3d)& ,const Handle(Prs3d_Presentation)& aPresentation,const Standard_Integer ) 
{
  Handle(Prs3d_ArrowAspect) anArrowAspect = myDrawer->ArrowAspect();
  anArrowAspect->SetLength(myArrowLength);
  myDrawer->SetArrowAspect(anArrowAspect);
  
  gp_Pnt LastPoint = myPnt;
  LastPoint.Translate (myLength*gp_Vec(myDir));

  if (myText.Length() == 0)
    DsgPrs_LengthPresentation::Add(aPresentation,myDrawer,myPnt,LastPoint,DsgPrs_AS_LASTAR);
  else
  {
    gp_Pnt OffsetPoint = myPnt;
    OffsetPoint.Translate ((myLength) *gp_Vec(myDir));
    DsgPrs_LengthPresentation::Add(aPresentation,myDrawer,myText,myPnt,LastPoint, -myDir, OffsetPoint, DsgPrs_AS_LASTAR);
  }
}

 void ISession_Direction::Compute(const Handle(Prs3d_Projector)& ,const Handle(Prs3d_Presentation)& ) 
{
}

 void ISession_Direction::Compute(const Handle(PrsMgr_PresentationManager2d)& ,const Handle(Graphic2d_GraphicObject)& aGrObj,const Standard_Integer ) 
{
  Handle(Graphic2d_SetOfSegments) segment;
  segment = new Graphic2d_SetOfSegments(aGrObj);
  
  segment->Add(myPnt.X(),
	       myPnt.Y(),
	       myPnt.X()+myLength*myDir.X(),
	       myPnt.Y()+myLength*myDir.Y());
  
  gp_Dir2d aFirstDir = gp_Dir2d(myDir.X(),myDir.Y()).Rotated (200*PI180);
  segment->Add(myPnt.X()+myLength*myDir.X(),
	       myPnt.Y()+myLength*myDir.Y(),
	       myPnt.X()+myLength*myDir.X()+ myLength*aFirstDir.X()/5,
	       myPnt.Y()+myLength*myDir.Y()+ myLength*aFirstDir.Y()/5);
  
  gp_Dir2d aSecondDir = gp_Dir2d(myDir.X(),myDir.Y()).Rotated (-200*PI180);
  segment->Add(myPnt.X()+myLength*myDir.X(),
	       myPnt.Y()+myLength*myDir.Y(),
	       myPnt.X()+myLength*myDir.X()+ myLength*aSecondDir.X()/5,
	       myPnt.Y()+myLength*myDir.Y()+ myLength*aSecondDir.Y()/5);
}

 void ISession_Direction::ComputeSelection(const Handle(SelectMgr_Selection)& ,const Standard_Integer ) 
{
}

