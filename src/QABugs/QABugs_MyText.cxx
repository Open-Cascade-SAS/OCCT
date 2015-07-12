// Created on: 2002-04-10
// Created by: QA Admin
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Font_NameOfFont.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>
#include <QABugs_MyText.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>

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

void QABugs_MyText::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                             const Handle(Prs3d_Presentation)& aPresentation,
                             const Standard_Integer /*aMode*/ ) 
{
  aPresentation->Clear();
  Handle(Prs3d_TextAspect) asp = myDrawer->TextAspect();

  asp->SetFont(myNameOfFont);
  asp->SetColor(myNameOfColor);
  asp->SetHeight(myHeight); // I am changing the myHeight value
  Prs3d_Text::Draw(aPresentation, asp, myText, myPosition);
}

void QABugs_MyText::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                      const Standard_Integer /*aMode*/)
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
