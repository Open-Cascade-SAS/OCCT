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

#include <Bnd_Sphere.hxx>

Bnd_Sphere::Bnd_Sphere()
    : myCenter(0., 0., 0.),
      myRadius(0.),
      myIsValid(false),
      myU(0),
      myV(0)
{
}

Bnd_Sphere::Bnd_Sphere(const gp_XYZ& theCenter,
                       const double  theRadius,
                       const int     theU,
                       const int     theV)
    : myCenter(theCenter),
      myRadius(theRadius),
      myIsValid(false),
      myU(theU),
      myV(theV)
{
}

void Bnd_Sphere::SquareDistances(const gp_XYZ& theXYZ, double& theMin, double& theMax) const
{
  theMax              = (theXYZ - myCenter).SquareModulus();
  const double aRadSq = myRadius * myRadius;
  theMin              = (theMax < aRadSq ? 0.0 : theMax - aRadSq);
  theMax += aRadSq;
}

void Bnd_Sphere::Distances(const gp_XYZ& theXYZ, double& theMin, double& theMax) const
{
  theMax = (theXYZ - myCenter).Modulus();
  theMin = (theMax - myRadius < 0 ? 0.0 : theMax - myRadius);
  theMax += myRadius;
}

bool Bnd_Sphere::Project(const gp_XYZ& theNode,
                         gp_XYZ&       theProjNode,
                         double&       theDist,
                         bool&         theInside) const
{
  theProjNode = myCenter;
  theDist     = (theNode - theProjNode).Modulus();
  theInside   = true;
  return true;
}

double Bnd_Sphere::Distance(const gp_XYZ& theNode) const
{
  return (theNode - myCenter).Modulus();
}

double Bnd_Sphere::SquareDistance(const gp_XYZ& theNode) const
{
  return (theNode - myCenter).SquareModulus();
}

void Bnd_Sphere::Add(const Bnd_Sphere& theOther)
{
  if (myRadius < 0.0)
  {
    // not initialised yet
    *this = theOther;
    return;
  }

  const double aDist = (myCenter - theOther.myCenter).Modulus();
  if (myRadius + aDist <= theOther.myRadius)
  {
    // the other sphere is larger and encloses this
    *this = theOther;
    return;
  }

  if (theOther.myRadius + aDist <= myRadius)
    return; // this sphere encloses other

  // expansion
  const double dfR          = (aDist + myRadius + theOther.myRadius) * 0.5;
  const double aParamOnDiam = (dfR - myRadius) / aDist;
  myCenter                  = myCenter * (1.0 - aParamOnDiam) + theOther.myCenter * aParamOnDiam;
  myRadius                  = dfR;
  myIsValid                 = false;
}

bool Bnd_Sphere::IsOut(const Bnd_Sphere& theOther) const
{
  return (myCenter - theOther.myCenter).SquareModulus()
         > (myRadius + theOther.myRadius) * (myRadius + theOther.myRadius);
}

bool Bnd_Sphere::IsOut(const gp_XYZ& theXYZ, double& theMaxDist) const
{
  double aCurMinDist, aCurMaxDist;
  Distances(theXYZ, aCurMinDist, aCurMaxDist);
  if (aCurMinDist > theMaxDist)
    return true;
  if (myIsValid && aCurMaxDist < theMaxDist)
    theMaxDist = aCurMaxDist;
  return false;
}

double Bnd_Sphere::SquareExtent() const
{
  return 4 * myRadius * myRadius;
}
