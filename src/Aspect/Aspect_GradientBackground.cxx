// File:  Aspect_GradientBackground.cdl
// Created: 21 May 2010
// Author:  PAUL SUPRYATKIN

//Copyright:  Open CASCADE 2010



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

