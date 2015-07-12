// Created on: 1995-02-08
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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


#include <GeomInt_ParameterAndOrientation.hxx>

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


