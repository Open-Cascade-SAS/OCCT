// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <PXCAFDoc_Datum.ixx>

//=======================================================================
//function : PXCAFDoc_Datum
//purpose  : 
//=======================================================================

PXCAFDoc_Datum::PXCAFDoc_Datum ()
{}

//=======================================================================
//function : PXCAFDoc_Datum
//purpose  : 
//=======================================================================

PXCAFDoc_Datum::PXCAFDoc_Datum 
  (const Handle(PCollection_HAsciiString)& theName,
   const Handle(PCollection_HAsciiString)& theDescr,
   const Handle(PCollection_HAsciiString)& theId)
: myName(theName),
  myDescr(theDescr),
  myId(theId)
{}
    
//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void PXCAFDoc_Datum::Set (const Handle(PCollection_HAsciiString)& theName,
                          const Handle(PCollection_HAsciiString)& theDescr,
                          const Handle(PCollection_HAsciiString)& theId)
{
  myName = theName;
  myDescr = theDescr;
  myId = theId;
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_Datum::GetName () const
{
  return myName;
}

//=======================================================================
//function : GetDescription
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_Datum::GetDescription () const
{
  return myDescr;
}

//=======================================================================
//function : GetIdentification
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_Datum::GetIdentification () const
{
  return myId;
}
