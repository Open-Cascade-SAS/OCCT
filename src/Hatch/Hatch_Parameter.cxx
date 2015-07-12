// Created on: 1992-08-19
// Created by: Modelistation
// Copyright (c) 1992-1999 Matra Datavision
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


#include <Hatch_Hatcher.hxx>
#include <Hatch_Line.hxx>
#include <Hatch_Parameter.hxx>

//=======================================================================
//function : Hatch_Parameter
//purpose  : 
//=======================================================================
Hatch_Parameter::Hatch_Parameter()
{
}

//=======================================================================
//function : Hatch_Parameter
//purpose  : 
//=======================================================================

Hatch_Parameter::Hatch_Parameter
  (const Standard_Real    Par1,
   const Standard_Boolean Start,
   const Standard_Integer Index,
   const Standard_Real    Par2) :
       myPar1(Par1), myStart(Start), myIndex(Index), myPar2(Par2)
{
}
