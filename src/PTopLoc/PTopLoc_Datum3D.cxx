// Created on: 1993-03-03
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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


