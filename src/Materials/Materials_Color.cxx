// Created on: 1993-04-15
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
