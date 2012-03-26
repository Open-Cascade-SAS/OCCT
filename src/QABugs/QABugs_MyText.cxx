// Created on: 2002-04-10
// Created by: QA Admin
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <QABugs_MyText.ixx>
#include <QABugs_MyText.hxx>
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


QABugs_MyText::QABugs_MyText(const TCollection_ExtendedString& aText, const gp_Pnt& aPosition, 
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

void QABugs_MyText::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
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

void QABugs_MyText::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
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


Standard_Integer QABugs_MyText::NbPossibleSelection() const
{
  return 1;
}

