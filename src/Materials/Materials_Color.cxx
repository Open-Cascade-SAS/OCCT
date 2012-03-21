// Created on: 1993-04-15
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
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



#include <Materials_Color.ixx>

Materials_Color::Materials_Color()
{}

Materials_Color::Materials_Color(const Quantity_Color& acolor)
{
  thecolor = acolor;
}

void Materials_Color::Color(const Quantity_Color& acolor)
{
  thecolor = acolor;
}

Quantity_Color Materials_Color::Color() const
{
  return thecolor;
}

void Materials_Color::Color(const Quantity_TypeOfColor aTypeOfColor,
			    Standard_Real& Reel1,
			    Standard_Real& Reel2,
			    Standard_Real& Reel3) const
{
  Quantity_Color aQColor = Color();
  aQColor.Values(Reel1,Reel2, Reel3, aTypeOfColor);
}

void Materials_Color::Color255(const Quantity_TypeOfColor aTypeOfColor,
			       Standard_Real& Reel1,
			       Standard_Real& Reel2,
			       Standard_Real& Reel3) const
{
  Quantity_Color aQColor = Color();
  aQColor.Values(Reel1,Reel2, Reel3, aTypeOfColor);
  Reel1=Reel1*255.0;
  Reel2=Reel2*255.0;
  Reel3=Reel3*255.0;
}

void Materials_Color::SetColor(const Quantity_TypeOfColor aTypeOfColor,
			       const Standard_Real Reel1,
			       const Standard_Real Reel2,
			       const Standard_Real Reel3) 
{
  Quantity_Color aQColor(Reel1,Reel2, Reel3, aTypeOfColor);
  Color(aQColor);
}

void Materials_Color::SetColor255(const Quantity_TypeOfColor aTypeOfColor,
				  const Standard_Real Reel1,
				  const Standard_Real Reel2,
				  const Standard_Real Reel3) 
{
  Quantity_Color aQColor(Reel1/255.0,Reel2/255.0, Reel3/255.0, aTypeOfColor);
  Color(aQColor);
}
