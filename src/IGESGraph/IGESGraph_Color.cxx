// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESGraph_Color.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_Color,IGESData_ColorEntity)

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
