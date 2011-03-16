//--------------------------------------------------------------------
//
//  File Name : IGESGraph_Color.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_Color.ixx>


    IGESGraph_Color::IGESGraph_Color ()    {  }


// This class inherits from IGESData_ColorEntity

    void IGESGraph_Color::Init
  (const Standard_Real red,
   const Standard_Real green,
   const Standard_Real blue,
   const Handle(TCollection_HAsciiString)& aColorName)
{
  theRed        = red;
  theGreen      = green;
  theBlue       = blue;
  theColorName  = aColorName;
  InitTypeAndForm(314,0);
}

    void IGESGraph_Color::RGBIntensity
  (Standard_Real& Red, Standard_Real& Green, Standard_Real& Blue) const
{
  Red   = theRed;
  Green = theGreen;
  Blue  = theBlue;
}

    void IGESGraph_Color::CMYIntensity
  (Standard_Real& Cyan, Standard_Real& Magenta, Standard_Real& Yellow) const
{
  Cyan    = 100.0 - theRed;
  Magenta = 100.0 - theGreen;
  Yellow  = 100.0 - theBlue;
}

    void IGESGraph_Color::HLSPercentage
  (Standard_Real& Hue, Standard_Real& Lightness, Standard_Real& Saturation) const
{
  Hue        = ((1.0 / (2.0 * PI)) *
		(ATan(((2 * theRed) - theGreen - theBlue) /
		      (Sqrt(3) * (theGreen - theBlue)))));
  Lightness  = ((1.0 / 3.0) * (theRed + theGreen + theBlue));
  Saturation = (Sqrt((theRed   * theRed  ) +
		     (theGreen * theGreen) +
		     (theBlue  * theBlue ) -
		     (theRed   * theGreen) -
		     (theRed   * theBlue ) -
		     (theBlue  * theGreen)));
}

    Standard_Boolean  IGESGraph_Color::HasColorName () const
{
  return (! theColorName.IsNull());
}

    Handle(TCollection_HAsciiString)  IGESGraph_Color::ColorName () const
{
  return theColorName;
}
