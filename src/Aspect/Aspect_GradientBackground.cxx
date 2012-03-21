// Created on: 2010-05-21
// Created by: PAUL SUPRYATKIN
// Copyright (c) 2010-2012 OPEN CASCADE SAS
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





#include <Aspect_GradientBackground.hxx>


Aspect_GradientBackground::Aspect_GradientBackground () {

  Quantity_Color Black (Quantity_NOC_BLACK);

  SetColor( Black );
  MyColor2 = Black;
  MyGradientMethod = Aspect_GFM_NONE;   

}

Aspect_GradientBackground::Aspect_GradientBackground( const Quantity_Color& AColor1,
                                                      const Quantity_Color& AColor2,
                                                      const Aspect_GradientFillMethod AMethod ) 
{
  SetColor( AColor1 );
  MyColor2 = AColor2;
  MyGradientMethod = AMethod;
};


void Aspect_GradientBackground::SetColors( const Quantity_Color& AColor1,
                                           const Quantity_Color& AColor2,
                                           const Aspect_GradientFillMethod AMethod ) 
{
  SetColor( AColor1 );
  MyColor2 = AColor2;
  MyGradientMethod = AMethod; 
}

void Aspect_GradientBackground::Colors ( Quantity_Color& AColor1, Quantity_Color& AColor2) const 
{
  AColor1 = Color();
  AColor2 = MyColor2;
}

Aspect_GradientFillMethod Aspect_GradientBackground::BgGradientFillMethod( ) const
{
  return MyGradientMethod;
}
