// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _UnitsMethods_LengthUnit_HeaderFile
#define _UnitsMethods_LengthUnit_HeaderFile

//! The Enumeration describes possible values for length units
enum UnitsMethods_LengthUnit
{
  UnitsMethods_LengthUnit_Undefined  = 0,  // 1.
  UnitsMethods_LengthUnit_Inch       = 1,  // 25.4
  UnitsMethods_LengthUnit_Millimeter = 2,  // 1.
  UnitsMethods_LengthUnit_Foot       = 4,  // 304.8
  UnitsMethods_LengthUnit_Mile       = 5,  // 1609344.
  UnitsMethods_LengthUnit_Meter      = 6,  // 1000.
  UnitsMethods_LengthUnit_Kilometer  = 7,  // 1000000.
  UnitsMethods_LengthUnit_Mil        = 8,  // (0.001 inch) // 0.0254
  UnitsMethods_LengthUnit_Micron     = 9,  // 0.001
  UnitsMethods_LengthUnit_Centimeter = 10, // 10.
  UnitsMethods_LengthUnit_Microinch  = 11  // 0.0000254
};

#endif // _UnitsMethods_LengthUnit
