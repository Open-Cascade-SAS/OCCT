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
#include <BRepGProp_Vinert.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : Constructor
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert()
{
}

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : 
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert(BRepGProp_Face&     theSurface,
                                   const gp_Pnt&       theLocation,
                                   const Standard_Real theEps)
{
  SetLocation(theLocation);
  Perform(theSurface, theEps);
}

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : 
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert(BRepGProp_Face&     theSurface,
                                   BRepGProp_Domain&   theDomain,
                                   const gp_Pnt&       theLocation,
                                   const Standard_Real theEps)
{
  SetLocation(theLocation);
  Perform(theSurface, theDomain, theEps);
}

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : 
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert(BRepGProp_Face&   theSurface,
                                   BRepGProp_Domain& theDomain,
                                   const gp_Pnt&     theLocation)
{
  SetLocation(theLocation);
  Perform(theSurface, theDomain);
}

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : 
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert(const BRepGProp_Face& theSurface,
                                   const gp_Pnt&         theLocation)
{
  SetLocation(theLocation);
  Perform(theSurface);
}

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : 
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert(BRepGProp_Face&     theSurface,
                                   const gp_Pnt&       theOrigin,
                                   const gp_Pnt&       theLocation,
                                   const Standard_Real theEps)
{
  SetLocation(theLocation);
  Perform(theSurface, theOrigin, theEps);
}

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : 
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert(BRepGProp_Face&     theSurface,
                                   BRepGProp_Domain&   theDomain,
                                   const gp_Pnt&       theOrigin,
                                   const gp_Pnt&       theLocation,
                                   const Standard_Real theEps)
{
  SetLocation(theLocation);
  Perform(theSurface, theDomain, theOrigin, theEps);
}

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : 
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert(const BRepGProp_Face& theSurface,
                                   const gp_Pnt&         theOrigin,
                                   const gp_Pnt&         theLocation)
{
  SetLocation(theLocation);
  Perform(theSurface, theOrigin);
}

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : 
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert(BRepGProp_Face&   theSurface,
                                   BRepGProp_Domain& theDomain,
                                   const gp_Pnt&     theOrigin,
                                   const gp_Pnt&     theLocation)
{
  SetLocation(theLocation);
  Perform(theSurface, theDomain, theOrigin);
}

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : 
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert(BRepGProp_Face&     theSurface,
                                   const gp_Pln&       thePlane,
                                   const gp_Pnt&       theLocation,
                                   const Standard_Real theEps)
{
  SetLocation(theLocation);
  Perform(theSurface, thePlane, theEps);
}

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : 
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert(BRepGProp_Face&     theSurface,
                                   BRepGProp_Domain&   theDomain,
                                   const gp_Pln&       thePlane,
                                   const gp_Pnt&       theLocation,
                                   const Standard_Real theEps)
{
  SetLocation(theLocation);
  Perform(theSurface, theDomain, thePlane, theEps);
}

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : 
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert(const BRepGProp_Face& theSurface,
                                   const gp_Pln&         thePlane,
                                   const gp_Pnt&         theLocation)
{
  SetLocation(theLocation);
  Perform(theSurface, thePlane);
}

//=======================================================================
//function : BRepGProp_Vinert
//purpose  : 
//=======================================================================
BRepGProp_Vinert::BRepGProp_Vinert(BRepGProp_Face&   theSurface,
                                   BRepGProp_Domain& theDomain,
                                   const gp_Pln&     thePlane,
                                   const gp_Pnt&     theLocation)
{
  SetLocation(theLocation);
  Perform(theSurface, theDomain, thePlane);
}

//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================
void BRepGProp_Vinert::SetLocation(const gp_Pnt& theLocation)
{
  loc = theLocation;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
Standard_Real BRepGProp_Vinert::Perform(BRepGProp_Face&     theSurface,
                                        const Standard_Real theEps)
{
  BRepGProp_Domain anEmptyDomain;
  return Perform(theSurface, anEmptyDomain, theEps);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Real BRepGProp_Vinert::Perform(BRepGProp_Face&     theSurface,
                                        BRepGProp_Domain&   theDomain,
                                        const Standard_Real theEps)
{
  const Standard_Real aCoeff[] = {0.0, 0.0, 0.0};
  BRepGProp_Gauss     aGauss(BRepGProp_Gauss::Vinert);

  return myEpsilon =
    aGauss.Compute(theSurface, theDomain, loc, theEps,
                   aCoeff, Standard_True, dim, g, inertia);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepGProp_Vinert::Perform(const BRepGProp_Face& theSurface)
{
  const Standard_Real aCoeff[] = {0.0, 0.0, 0.0};
  BRepGProp_Gauss     aGauss(BRepGProp_Gauss::Vinert);

  myEpsilon = 1.0;
  aGauss.Compute(theSurface, loc, aCoeff, Standard_True, dim, g, inertia);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepGProp_Vinert::Perform(BRepGProp_Face&   theSurface,
                               BRepGProp_Domain& theDomain)
{
  const Standard_Real aCoeff[] = {0.0, 0.0, 0.0};
  BRepGProp_Gauss     aGauss(BRepGProp_Gauss::Vinert);

  myEpsilon = 1.0;
  aGauss.Compute(theSurface, theDomain, loc, aCoeff, Standard_True, dim, g, inertia);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
Standard_Real BRepGProp_Vinert::Perform(BRepGProp_Face&     theSurface,
                                        const gp_Pnt&       theOrigin,
                                        const Standard_Real theEps)
{
  BRepGProp_Domain anEmptyDomain;
  return Perform(theSurface, anEmptyDomain, theOrigin, theEps);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
Standard_Real BRepGProp_Vinert::Perform(BRepGProp_Face&     theSurface,
                                        BRepGProp_Domain&   theDomain,
                                        const gp_Pnt&       theOrigin,
                                        const Standard_Real theEps)
{
  const Standard_Real aCoeff[] =
  {
    theOrigin.X() - loc.X(),
    theOrigin.Y() - loc.Y(),
    theOrigin.Z() - loc.Z()
  };

  BRepGProp_Gauss  aGauss(BRepGProp_Gauss::Vinert);

  return myEpsilon =
    aGauss.Compute(theSurface, theDomain, loc, theEps,
                   aCoeff, Standard_True, dim, g, inertia);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepGProp_Vinert::Perform(const BRepGProp_Face& theSurface,
                               const gp_Pnt&         theOrigin)
{
  BRepGProp_Gauss     aGauss(BRepGProp_Gauss::Vinert);
  const Standard_Real aCoeff[] =
  {
    theOrigin.X() - loc.X(),
    theOrigin.Y() - loc.Y(),
    theOrigin.Z() - loc.Z()
  };

  myEpsilon = 1.0;
  aGauss.Compute(theSurface, loc, aCoeff, Standard_True, dim, g, inertia);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepGProp_Vinert::Perform(BRepGProp_Face&   theSurface,
                               BRepGProp_Domain& theDomain,
                               const gp_Pnt&     theOrigin)
{
  BRepGProp_Gauss     aGauss(BRepGProp_Gauss::Vinert);
  const Standard_Real aCoeff[] =
  {
    theOrigin.X() - loc.X(),
    theOrigin.Y() - loc.Y(),
    theOrigin.Z() - loc.Z()
  };

  myEpsilon = 1.0;
  aGauss.Compute(theSurface, theDomain, loc, aCoeff, Standard_True, dim, g, inertia);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Real BRepGProp_Vinert::Perform(BRepGProp_Face&     theSurface,
                                        const gp_Pln&       thePlane,
                                        const Standard_Real theEps)
{
  BRepGProp_Domain anEmptyDomain;
  return Perform(theSurface, anEmptyDomain, thePlane, theEps);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
Standard_Real BRepGProp_Vinert::Perform(BRepGProp_Face&     theSurface,
                                        BRepGProp_Domain&   theDomain,
                                        const gp_Pln&       thePlane,
                                        const Standard_Real theEps)
{
  Standard_Real aCoeff[4];
  thePlane.Coefficients(aCoeff[0], aCoeff[1], aCoeff[2], aCoeff[3]);
  aCoeff[3] = aCoeff[3] - aCoeff[0] * loc.X()
                        - aCoeff[1] * loc.Y()
                        - aCoeff[2] * loc.Z();

  BRepGProp_Gauss  aGauss(BRepGProp_Gauss::Vinert);

  return myEpsilon =
    aGauss.Compute(theSurface, theDomain, loc, theEps,
                   aCoeff, Standard_False, dim, g, inertia);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepGProp_Vinert::Perform(const BRepGProp_Face& theSurface,
                               const gp_Pln&         thePlane)
{
  BRepGProp_Gauss aGauss(BRepGProp_Gauss::Vinert);
  Standard_Real   aCoeff[4];

  thePlane.Coefficients  (aCoeff[0],
                          aCoeff[1],
                          aCoeff[2],
                          aCoeff[3]);

  aCoeff[3] = aCoeff[3] - aCoeff[0] * loc.X()
                        - aCoeff[1] * loc.Y()
                        - aCoeff[2] * loc.Z();

  myEpsilon = 1.0;
  aGauss.Compute(theSurface, loc, aCoeff, Standard_False, dim, g, inertia);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepGProp_Vinert::Perform(BRepGProp_Face&   theSurface,
                               BRepGProp_Domain& theDomain,
                               const gp_Pln&     thePlane)
{
  BRepGProp_Gauss aGauss(BRepGProp_Gauss::Vinert);
  Standard_Real   aCoeff[4];

  thePlane.Coefficients  (aCoeff[0],
                          aCoeff[1],
                          aCoeff[2],
                          aCoeff[3]);

  aCoeff[3] = aCoeff[3] - aCoeff[0] * loc.X()
                        - aCoeff[1] * loc.Y()
                        - aCoeff[2] * loc.Z();

  myEpsilon = 1.0;
  aGauss.Compute(theSurface, theDomain, loc, aCoeff, Standard_False, dim, g, inertia);
}

//=======================================================================
//function : GetEpsilon
//purpose  : 
//=======================================================================
Standard_Real BRepGProp_Vinert::GetEpsilon()
{
  return myEpsilon;
}
