// File:	Materials_Color.cxx
// Created:	Thu Apr 15 17:48:59 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@phylox>


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
