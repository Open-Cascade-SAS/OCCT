#include <ISession_Text.ixx>
#include <AIS_InteractiveObject.hxx>
#include <Prs3d_Text.hxx>
#include <Graphic2d_Text.hxx>



ISession_Text::ISession_Text()
{
}

ISession_Text::ISession_Text(const TCollection_AsciiString& aText,const Standard_Real anX,const Standard_Real anY,const Standard_Real aZ,const Aspect_TypeOfText aType,const Quantity_PlaneAngle anAngle,const Standard_Real aSlant,const Standard_Integer aColorIndex,const Standard_Integer aFontIndex,const Quantity_Factor aScale)
     : AIS_InteractiveObject(),MyText(aText),MyX(anX),MyY(anY),MyZ(aZ),
       MyTypeOfText(aType),MyAngle(anAngle),MySlant(aSlant),
       MyColorIndex(aColorIndex),MyFontIndex(aFontIndex),
       MyScale(aScale),MyWidth(0),MyHeight(0)
{
}

ISession_Text::ISession_Text(const TCollection_AsciiString& aText,const gp_Pnt& aPoint,const Aspect_TypeOfText aType,const Quantity_PlaneAngle anAngle,const Standard_Real aSlant,const Standard_Integer aColorIndex,const Standard_Integer aFontIndex,const Quantity_Factor aScale)
     :AIS_InteractiveObject(),MyText(aText),MyX(aPoint.X()),MyY(aPoint.Y()),MyZ(aPoint.Z()),
       MyTypeOfText(aType),MyAngle(anAngle),MySlant(aSlant),
       MyColorIndex(aColorIndex),MyFontIndex(aFontIndex),
       MyScale(aScale),MyWidth(0),MyHeight(0)
{
}

 void ISession_Text::Compute(const Handle(PrsMgr_PresentationManager3d)& ,const Handle(Prs3d_Presentation)& aPresentation,const Standard_Integer ) 
{
  Prs3d_Text::Draw(aPresentation,myDrawer,MyText,gp_Pnt(MyX,MyY,MyZ));
}

 void ISession_Text::Compute(const Handle(Prs3d_Projector)& ,const Handle(Prs3d_Presentation)& ) 
{
}

 void ISession_Text::Compute(const Handle(PrsMgr_PresentationManager2d)& ,const Handle(Graphic2d_GraphicObject)& aGrObj,const Standard_Integer ) 
{
  Handle(Graphic2d_Text) text;
  text = new Graphic2d_Text(aGrObj, MyText, MyX, MyY, MyAngle, MyTypeOfText, MyScale);
  text->SetFontIndex(MyFontIndex);
 
  text->SetColorIndex(MyColorIndex);
 
  text->SetSlant(MySlant);
  text->SetUnderline(Standard_False);
  text->SetZoomable(Standard_True);
  aGrObj->Display();
  Quantity_Length anXoffset,anYoffset;
  text->TextSize(MyWidth, MyHeight,anXoffset,anYoffset);
}

 void ISession_Text::ComputeSelection(const Handle(SelectMgr_Selection)& ,const Standard_Integer ) 
{
}

