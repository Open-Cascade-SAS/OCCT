// Created on: 2013-11-11
// Created by: Anastasia BORISOVA
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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
