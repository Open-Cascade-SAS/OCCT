// Created on: 1993-03-03
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

#include <PTopLoc_Datum3D.ixx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : PTopLoc_Datum3D
//purpose  : 
//=======================================================================

PTopLoc_Datum3D::PTopLoc_Datum3D(const gp_Trsf& T) :
       myTrsf(T)
{
  // Update 7-05-96 FMA : validity control is not performed during 
  //                      Persistant instance construction
  // check the validity of the transformation
  //if (Abs(1. - myTrsf.VectorialPart().Determinant()) > 1.e-7)
  //Standard_ConstructionError::Raise
  //("PTopLoc_Datum3D::Non rigid transformation");
}


//=======================================================================
//function : Transformation
//purpose  : 
//=======================================================================

gp_Trsf  PTopLoc_Datum3D::Transformation()const 
{
  return myTrsf;
}


