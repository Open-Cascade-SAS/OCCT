// Created by: CKY / Contract Toubro-Larsen
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

//--------------------------------------------------------------------
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
  Hue        = ((1.0 / (2.0 * M_PI)) *
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
