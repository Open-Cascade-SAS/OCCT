// Created on: 2010-05-21
// Created by: PAUL SUPRYATKIN
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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
}


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
