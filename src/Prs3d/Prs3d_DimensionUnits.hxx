// Created on: 2013-11-11
// Created by: Anastasia BORISOVA
// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _Prs3d_DimensionUnits_Header
#define _Prs3d_DimensionUnits_Header

#include <TCollection_AsciiString.hxx>

//! This class provides units for two dimension groups:
//! - lengthes (length, radius, diameter)
//! - angles
class Prs3d_DimensionUnits
{
public:

  //! Default constructor. Sets meters as default length units
  //! and radians as default angle units.
  Prs3d_DimensionUnits() 
    : myLengthUnits ("m"),
      myAngleUnits ("rad") 
  {}

  Prs3d_DimensionUnits (const Prs3d_DimensionUnits& theUnits)
    : myLengthUnits (theUnits.GetLengthUnits()),
      myAngleUnits (theUnits.GetAngleUnits())
  {}

  //! Sets angle units
  Standard_EXPORT void SetAngleUnits (const TCollection_AsciiString& theUnits);

  //! @return angle units
  const TCollection_AsciiString& GetAngleUnits() const { return myAngleUnits; }

  //! Sets length units
  Standard_EXPORT void SetLengthUnits (const TCollection_AsciiString& theUnits);

  //! @return length units
  const TCollection_AsciiString& GetLengthUnits() const { return myLengthUnits; }

private:

  TCollection_AsciiString myLengthUnits;
  TCollection_AsciiString myAngleUnits;
};
#endif
