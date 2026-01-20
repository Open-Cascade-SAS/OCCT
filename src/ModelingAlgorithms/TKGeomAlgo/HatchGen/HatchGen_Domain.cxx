// Created on: 1993-11-10
// Created by: Jean Marc LACHAUME
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

#include <HatchGen_Domain.hxx>
#include <HatchGen_PointOnHatching.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>

//=================================================================================================

HatchGen_Domain::HatchGen_Domain()
    : myHasFirstPoint(false),
      myHasSecondPoint(false)
{
}

//=================================================================================================

HatchGen_Domain::HatchGen_Domain(const HatchGen_PointOnHatching& P1,
                                 const HatchGen_PointOnHatching& P2)
    : myHasFirstPoint(true),
      myFirstPoint(P1),
      myHasSecondPoint(true),
      mySecondPoint(P2)
{
}

//=================================================================================================

HatchGen_Domain::HatchGen_Domain(const HatchGen_PointOnHatching& P, const bool First)
{
  if (First)
  {
    myHasFirstPoint  = true;
    myHasSecondPoint = false;
    myFirstPoint     = P;
  }
  else
  {
    myHasFirstPoint  = false;
    myHasSecondPoint = true;
    mySecondPoint    = P;
  }
}

//=================================================================================================

void HatchGen_Domain::Dump(const int Index) const
{
  std::cout << "=== Domain ";
  if (Index > 0)
  {
    std::cout << "# " << std::setw(3) << Index << " ";
  }
  else
  {
    std::cout << "======";
  }
  std::cout << "=============================" << std::endl;

  if (myHasFirstPoint)
  {
    myFirstPoint.Dump(1);
  }
  else
  {
    std::cout << "    Has not a first point" << std::endl;
  }

  if (myHasSecondPoint)
  {
    mySecondPoint.Dump(2);
  }
  else
  {
    std::cout << "    Has not a second point" << std::endl;
  }

  std::cout << "==============================================" << std::endl;
}
