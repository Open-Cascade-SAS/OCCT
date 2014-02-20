// Created on: 1997-04-09
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
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

#include <PDataStd_Integer.ixx>


//=======================================================================
//function : PDataStd_Integer
//purpose  : 
//=======================================================================

PDataStd_Integer::PDataStd_Integer () : myValue(0) {}


//=======================================================================
//function : PDataStd_Integer
//purpose  : 
//=======================================================================

PDataStd_Integer::PDataStd_Integer (const Standard_Integer V) : myValue (V) { }



//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void PDataStd_Integer::Set (const Standard_Integer V) 
{
  myValue = V;
}


//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Integer PDataStd_Integer::Get () const
{
  return myValue;
}

