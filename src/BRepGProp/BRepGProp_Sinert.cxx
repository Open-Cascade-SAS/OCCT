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


#include <BRepGProp_Domain.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepGProp_Gauss.hxx>
#include <BRepGProp_Sinert.hxx>
#include <gp_Pnt.hxx>

//=======================================================================
//function : BRepGProp_Sinert
//purpose  : Constructor
//=======================================================================
BRepGProp_Sinert::BRepGProp_Sinert()
{
}

//=======================================================================
//function : BRepGProp_Sinert
//purpose  : Constructor
//=======================================================================
BRepGProp_Sinert::BRepGProp_Sinert (const BRepGProp_Face& theSurface,
                                    const gp_Pnt&         theLocation)
{
  SetLocation(theLocation);
  Perform(theSurface);
}

//=======================================================================
//function : BRepGProp_Sinert
//purpose  : Constructor
//=======================================================================
BRepGProp_Sinert::BRepGProp_Sinert(BRepGProp_Face&   theSurface,
                                   BRepGProp_Domain& theDomain,
                                   const gp_Pnt&     theLocation)
{
  SetLocation(theLocation);
  Perform(theSurface, theDomain);
}

//=======================================================================
//function : BRepGProp_Sinert
//purpose  : Constructor
//=======================================================================
BRepGProp_Sinert::BRepGProp_Sinert(BRepGProp_Face&     theSurface,
                                   const gp_Pnt&       theLocation,
                                   const Standard_Real theEps)
{
  SetLocation(theLocation);
  Perform(theSurface, theEps);
}

//=======================================================================
//function : BRepGProp_Sinert
//purpose  : Constructor
//=======================================================================
BRepGProp_Sinert::BRepGProp_Sinert(BRepGProp_Face&     theSurface,
                                   BRepGProp_Domain&   theDomain,
                                   const gp_Pnt&       theLocation,
                                   const Standard_Real theEps)
{
  SetLocation(theLocation);
  Perform(theSurface, theDomain, theEps);
}

//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================
void BRepGProp_Sinert::SetLocation(const gp_Pnt& theLocation)
{
  loc = theLocation;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepGProp_Sinert::Perform(const BRepGProp_Face& theSurface)
{
  myEpsilon = 1.0;

  BRepGProp_Gauss aGauss(BRepGProp_Gauss::Sinert);
  aGauss.Compute(theSurface, loc, dim, g, inertia);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepGProp_Sinert::Perform(BRepGProp_Face&   theSurface,
                               BRepGProp_Domain& theDomain)
{
  myEpsilon = 1.0;

  BRepGProp_Gauss aGauss(BRepGProp_Gauss::Sinert);
  aGauss.Compute(theSurface, theDomain, loc, dim, g, inertia);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
Standard_Real BRepGProp_Sinert::Perform(BRepGProp_Face&     theSurface,
                                        const Standard_Real theEps)
{
  BRepGProp_Domain anEmptyDomian;
  return Perform(theSurface, anEmptyDomian, theEps);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
Standard_Real BRepGProp_Sinert::Perform(BRepGProp_Face&     theSurface,
                                        BRepGProp_Domain&   theDomain,
                                        const Standard_Real theEps)
{
  BRepGProp_Gauss  aGauss(BRepGProp_Gauss::Sinert);
  return myEpsilon = aGauss.Compute(theSurface, theDomain, loc, theEps, dim, g, inertia);
}

//=======================================================================
//function : GetEpsilon
//purpose  : 
//=======================================================================
Standard_Real BRepGProp_Sinert::GetEpsilon()
{
  return myEpsilon;
}
