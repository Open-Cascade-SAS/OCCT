// ISession_Text.cpp: implementation of the ISession_Text class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\GeometryApp.h"
#include "ISession_Text.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif
IMPLEMENT_STANDARD_HANDLE(ISession_Text,AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(ISession_Text,AIS_InteractiveObject)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ISession_Text::ISession_Text()
{

}

ISession_Text::ISession_Text
                 (const TCollection_AsciiString& aText, 
                  const Standard_Real            anX ,        // = 0
                  const Standard_Real            anY ,        // = 0
                  const Standard_Real            aZ  ,        // = 0
			      const Quantity_PlaneAngle      anAngle,     // = 0.0
			      const Standard_Real            aslant,      // = 0.0
			      const Standard_Integer         aColorIndex, // = 0
			      const Standard_Integer         aFontIndex,  // = 1
			      const Quantity_Factor          aScale)      // = 1
                  :AIS_InteractiveObject(),MyText(aText),MyX(anX),MyY(anY),MyZ(aZ),
                  MyAngle(anAngle),MySlant(aslant),MyFontIndex(aFontIndex),
                  MyColorIndex(aColorIndex),MyScale(aScale),MyWidth(0),MyHeight(0)
{

}

ISession_Text::ISession_Text
                 (const TCollection_AsciiString& aText, 
                  gp_Pnt&                        aPoint,
			      const Quantity_PlaneAngle      anAngle,     // = 0.0
			      const Standard_Real            aslant,      // = 0.0
			      const Standard_Integer         aColorIndex, // = 0
			      const Standard_Integer         aFontIndex,  // = 1
			      const Quantity_Factor          aScale)      // = 1
                  :AIS_InteractiveObject(),MyText(aText),MyX(aPoint.X()),MyY(aPoint.Y()),MyZ(aPoint.Z()),
                  MyAngle(anAngle),MySlant(aslant),MyFontIndex(aFontIndex),
                  MyColorIndex(aColorIndex),MyScale(aScale),MyWidth(0),MyHeight(0)
{

}

ISession_Text::~ISession_Text()
{

}

void ISession_Text::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                            const Handle(Prs3d_Presentation)& aPresentation,
                            const Standard_Integer /*aMode*/)
{
    Prs3d_Text::Draw(aPresentation,myDrawer,MyText,gp_Pnt(  MyX ,MyY,MyZ ));
}

void ISession_Text::Compute(const Handle(Prs3d_Projector)& /*aProjector*/,
                            const Handle(Prs3d_Presentation)& /*aPresentation*/) 
 {
 }

void ISession_Text::ComputeSelection(const Handle(SelectMgr_Selection)& /*aSelection*/, 
				     const Standard_Integer /*unMode*/)
{
}

