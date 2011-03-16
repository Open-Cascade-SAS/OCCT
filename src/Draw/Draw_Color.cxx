// Copyright: 	Matra-Datavision 1991
// File:	Draw_Color.cxx
// Created:	Wed Apr 24 16:52:41 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Draw_Color.ixx>

Draw_Color::Draw_Color () :
       myKind(Draw_blanc)
{
}

Draw_Color::Draw_Color (const Draw_ColorKind c) :
       myKind(c)
{
}

Draw_ColorKind Draw_Color::ID () const
{
  return myKind;
}
