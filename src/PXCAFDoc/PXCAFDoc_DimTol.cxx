// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PXCAFDoc_DimTol.ixx>

//=======================================================================
//function : PXCAFDoc_DimTol
//purpose  : 
//=======================================================================

PXCAFDoc_DimTol::PXCAFDoc_DimTol () : myKind(0)
{}

//=======================================================================
//function : PXCAFDoc_DimTol
//purpose  : 
//=======================================================================

PXCAFDoc_DimTol::PXCAFDoc_DimTol 
  (const Standard_Integer theKind,
   const Handle(PColStd_HArray1OfReal)& theVal,
   const Handle(PCollection_HAsciiString)& theName,
   const Handle(PCollection_HAsciiString)& theDescr)
: myKind(theKind),
  myVal(theVal),
  myName(theName),
  myDescr(theDescr)
{}
    
//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void PXCAFDoc_DimTol::Set (const Standard_Integer theKind,
                           const Handle(PColStd_HArray1OfReal)& theVal,
                           const Handle(PCollection_HAsciiString)& theName,
                           const Handle(PCollection_HAsciiString)& theDescr)
{
  myKind = theKind;
  myVal = theVal;
  myName = theName;
  myDescr = theDescr;
}

//=======================================================================
//function : GetKind
//purpose  : 
//=======================================================================

Standard_Integer PXCAFDoc_DimTol::GetKind () const
{
  return myKind;
}

//=======================================================================
//function : GetVal
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal) PXCAFDoc_DimTol::GetVal () const
{
  return myVal;
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_DimTol::GetName () const
{
  return myName;
}

//=======================================================================
//function : GetDescription
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_DimTol::GetDescription () const
{
  return myDescr;
}
