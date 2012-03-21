// Created on: 1995-04-21
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <StdSelect_TextProjector2d.ixx>
#include <Aspect_WindowDriver.hxx>


//==================================================
// Function: 
// Purpose :
//==================================================

StdSelect_TextProjector2d::
StdSelect_TextProjector2d(const Handle(V2d_View)& aView):
myview(aView)
{
}

//==================================================
// Function: 
// Purpose :
//==================================================

void StdSelect_TextProjector2d::
Convert(const gp_Pnt2d& aPointIn, gp_Pnt2d& aPointOut) const 
{
  aPointOut = aPointIn.Transformed(mytrsf);
}

//==================================================
// Function: 
// Purpose :
//==================================================

void StdSelect_TextProjector2d
::Convert(const TCollection_ExtendedString& aText, 
	  const Standard_Real XPos, 
	  const Standard_Real YPos, 
	  gp_Pnt2d& MinPoint, 
	  gp_Pnt2d& MaxPoint, 
	  const Standard_Integer afont) const 
{
  Standard_ShortReal myL,myH;
  MinPoint = gp_Pnt2d(XPos,YPos).Transformed(mytrsf);
  myview->Driver()->TextSize(aText,myL,myH,afont);
  Standard_Real myWinL=myview->Convert(myL);
  Standard_Real myWinH=myview->Convert(myH);

  MaxPoint.SetCoord(XPos+myWinL,YPos+myWinH);
  MaxPoint.Transform(mytrsf);
}



