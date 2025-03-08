// Created on: 1991-01-21
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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

#include <TopLoc_Datum3D.hxx>
#include <Standard_Dump.hxx>
#include <TopLoc_ItemLocation.hxx>
#include <TopLoc_Location.hxx>

//=================================================================================================

TopLoc_ItemLocation::TopLoc_ItemLocation(const Handle(TopLoc_Datum3D)& D, const Standard_Integer P)
    : myDatum(D),
      myPower(P),
      myTrsf(D->Transformation().Powered(P))
{
}

//=================================================================================================

void TopLoc_ItemLocation::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, TopLoc_ItemLocation)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myTrsf)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myDatum.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPower)
}
