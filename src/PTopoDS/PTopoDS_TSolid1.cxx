// Created on: 1993-03-09
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <PTopoDS_TSolid1.ixx>

//=======================================================================
//function : PTopoDS_TSolid1
//purpose  : 
//=======================================================================

PTopoDS_TSolid1::PTopoDS_TSolid1() 
{
}



//=======================================================================
//function : ShapeType 
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum PTopoDS_TSolid1::ShapeType() const
{
  return TopAbs_SOLID;
}
