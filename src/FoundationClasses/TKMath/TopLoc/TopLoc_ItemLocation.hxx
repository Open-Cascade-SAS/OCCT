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

#ifndef _TopLoc_ItemLocation_HeaderFile
#define _TopLoc_ItemLocation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <gp_Trsf.hxx>
class TopLoc_Datum3D;

//! An ItemLocation is an elementary coordinate system
//! in a Location.
//!
//! The  ItemLocation     contains :
//!
//! * The elementary Datum.
//!
//! * The exponent of the elementary Datum.
//!
//! * The transformation associated to the composition.
class TopLoc_ItemLocation
{
public:
  DEFINE_STANDARD_ALLOC

  //! Sets the elementary Datum to <D>
  //! Sets the exponent to <P>
  Standard_EXPORT TopLoc_ItemLocation(const Handle(TopLoc_Datum3D)& D, const Standard_Integer P);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  friend class TopLoc_Location;
  friend class TopLoc_SListOfItemLocation;

protected:
private:
  Handle(TopLoc_Datum3D) myDatum;
  Standard_Integer       myPower;
  gp_Trsf                myTrsf;
};

#endif // _TopLoc_ItemLocation_HeaderFile
