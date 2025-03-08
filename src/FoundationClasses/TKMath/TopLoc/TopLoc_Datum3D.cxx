// Created on: 1991-01-25
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

#include <Standard_ConstructionError.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Type.hxx>
#include <Standard_Dump.hxx>
#include <TopLoc_Datum3D.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopLoc_Datum3D, Standard_Transient)

//=================================================================================================

TopLoc_Datum3D::TopLoc_Datum3D() {}

//=================================================================================================

TopLoc_Datum3D::TopLoc_Datum3D(const gp_Trsf& T)
    : myTrsf(T)
{
}

//=================================================================================================

void TopLoc_Datum3D::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myTrsf)
}

//=================================================================================================

void TopLoc_Datum3D::ShallowDump(Standard_OStream& S) const
{
  S << " TopLoc_Datum3D " << (void*)this << std::endl;
  Standard_Integer i;
  gp_Trsf          T = myTrsf;
  for (i = 1; i <= 3; i++)
  {
    S << "  ( " << std::setw(10) << T.Value(i, 1);
    S << "," << std::setw(10) << T.Value(i, 2);
    S << "," << std::setw(10) << T.Value(i, 3);
    S << "," << std::setw(10) << T.Value(i, 4);
    S << ")\n";
  }
  S << std::endl;
}
