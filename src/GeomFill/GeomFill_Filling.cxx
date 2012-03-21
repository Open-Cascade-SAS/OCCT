// Created on: 1993-09-28
// Created by: Bruno DUMORTIER
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


#include <GeomFill_Filling.ixx>

#include <gp_Pnt.hxx>

//=======================================================================
//function : GeomFill_Filling
//purpose  : 
//=======================================================================

GeomFill_Filling::GeomFill_Filling()
{
}


//=======================================================================
//function : NbUPoles
//purpose  : 
//=======================================================================

Standard_Integer GeomFill_Filling::NbUPoles() const
{
  return myPoles->ColLength();
}

//=======================================================================
//function : NbVPoles
//purpose  : 
//=======================================================================

Standard_Integer GeomFill_Filling::NbVPoles() const
{
  return myPoles->RowLength();
}

//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  GeomFill_Filling::Poles(TColgp_Array2OfPnt& Poles)const 
{
  Poles = myPoles->ChangeArray2();
}

//=======================================================================
//function : isRational
//purpose  : 
//=======================================================================

Standard_Boolean  GeomFill_Filling::isRational()const 
{
  return IsRational;
}


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void  GeomFill_Filling::Weights(TColStd_Array2OfReal& Weights)const 
{
  Weights = myWeights->ChangeArray2();
}


