// Created on: 1997-04-09
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
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


#include <PDataStd_Real.ixx>


//=======================================================================
//function : PDataStd_Real
//purpose  : 
//=======================================================================

PDataStd_Real::PDataStd_Real () { }



//=======================================================================
//function : PDataStd_Real
//purpose  : 
//=======================================================================

PDataStd_Real::PDataStd_Real (const Standard_Real V, 
			      const Standard_Integer D) : myValue (V), myDimension(D) { }


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void PDataStd_Real::Set (const Standard_Real V) 
{
  myValue = V;
}



//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Real PDataStd_Real::Get () const
{
  return myValue;
}

//=======================================================================
//function : SetDimension
//purpose  : 
//=======================================================================

void PDataStd_Real::SetDimension (const Standard_Integer D)
{
  myDimension = D;
}



//=======================================================================
//function : GetDimension
//purpose  : 
//=======================================================================

Standard_Integer PDataStd_Real::GetDimension () const
{
  return myDimension;
}


