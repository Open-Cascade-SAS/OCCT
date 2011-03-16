// File:  QATelco_MyText.cxx
// Created: Wed Apr 10 10:31:36 2002
// Author:  QA Admin
//    <qa@umnox.nnov.matra-dtv.fr>


#include <QATelco_MyText.ixx>
#include <QATelco_MyText.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_TextAspect.hxx>
#include <AIS_Drawer.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Prs3d_Text.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <SelectMgr_Selection.hxx>
#include <Graphic3d_NameOfFont.hxx>

/*************************************************************************
*
* CONSTRUCTION/DESTRUCTION
*
**************************************************************************/


QATelco_MyText::QATelco_MyText(const TCollection_ExtendedString& aText, const gp_Pnt& aPosition, 
                               const Standard_CString aFont, const Quantity_NameOfColor aColor, const Standard_Real aHeight)
{
  myPosition = aPosition;
  myText = TCollection_ExtendedString(aText);
  myNameOfFont = aFont;
  myNameOfColor = aColor;
  myHeight = aHeight;
}


/*************************************************************************
*
* QUERY METHODS
*
**************************************************************************/

void QATelco_MyText::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                             const Handle(Prs3d_Presentation)& aPresentation,
                             const Standard_Integer aMode ) 
{
  aPresentation->Clear();
  Handle_Prs3d_TextAspect asp = myDrawer->TextAspect();

  asp->SetFont(myNameOfFont);
  asp->SetColor(myNameOfColor);
  asp->SetHeight(myHeight); // I am changing the myHeight value
  Prs3d_Text::Draw(aPresentation, asp, myText, myPosition);
}

void QATelco_MyText::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                      const Standard_Integer aMode)
{
  // This code is here to compute the selection, erase this code if you don't what selection.

  Handle( SelectMgr_EntityOwner ) own = new SelectMgr_EntityOwner(this, 7);

  Handle( Select3D_SensitiveBox ) box = new Select3D_SensitiveBox(own,
    myPosition.X(), 
    myPosition.Y(),
    myPosition.Z(), 
    myPosition.X() + 20,
    myPosition.Y() + 20,
    myPosition.Z() + 20);
  aSelection->Add(box);
}


Standard_Integer QATelco_MyText::NbPossibleSelection() const
{
  return 1;
}

