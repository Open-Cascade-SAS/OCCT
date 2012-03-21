// Created on: 1995-02-08
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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



#include <GeomInt_ParameterAndOrientation.ixx>


//=======================================================================
//function : GeomInt_ParameterAndOrientation
//purpose  : 
//=======================================================================
GeomInt_ParameterAndOrientation::GeomInt_ParameterAndOrientation() :
  prm(0.0),or1(TopAbs_FORWARD),or2(TopAbs_FORWARD)
{}



//=======================================================================
//function : GeomInt_ParameterAndOrientation
//purpose  : 
//=======================================================================
  GeomInt_ParameterAndOrientation::GeomInt_ParameterAndOrientation
   (const Standard_Real P,
    const TopAbs_Orientation Or1,
    const TopAbs_Orientation Or2) : prm(P), or1(Or1), or2(Or2)
{}



//=======================================================================
//function : SetOrientation1
//purpose  : 
//=======================================================================
  void GeomInt_ParameterAndOrientation::SetOrientation1
   (const TopAbs_Orientation Or1)
{
  or1 = Or1;
}


//=======================================================================
//function : SetOrientation2
//purpose  : 
//=======================================================================
  void GeomInt_ParameterAndOrientation::SetOrientation2
   (const TopAbs_Orientation Or2)
{
  or2 = Or2;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================
  Standard_Real GeomInt_ParameterAndOrientation::Parameter () const
{
  return prm;
}


//=======================================================================
//function : Orientation1
//purpose  : 
//=======================================================================
  TopAbs_Orientation GeomInt_ParameterAndOrientation::Orientation1 () const
{
  return or1;
}


//=======================================================================
//function : Orientation2
//purpose  : 
//=======================================================================
  TopAbs_Orientation GeomInt_ParameterAndOrientation::Orientation2 () const
{
  return or2;
}


