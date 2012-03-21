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

#include <Bnd_Sphere.hxx>

Bnd_Sphere::Bnd_Sphere()
  : myCenter (0., 0., 0.),
    myRadius (0.),
    myIsValid (Standard_False),
    myU (0),
    myV (0)
{}

Bnd_Sphere::Bnd_Sphere( const gp_XYZ& theCenter, const Standard_Real theRadius,
                        const Standard_Integer theU, const Standard_Integer theV )
  : myCenter (theCenter),
    myRadius (theRadius),
    myIsValid (Standard_False),
    myU (theU),
    myV (theV)
{}

void Bnd_Sphere::SquareDistances( const gp_XYZ& theXYZ,
                                  Standard_Real& theMin, Standard_Real& theMax ) const
{
  theMax = ( theXYZ - myCenter ).SquareModulus();
  theMin = ( theMax - myRadius <0 ? 0.0 : theMax - myRadius * myRadius );
  theMax += myRadius * myRadius;
}

void Bnd_Sphere::Distances( const gp_XYZ& theXYZ,
                            Standard_Real& theMin, Standard_Real& theMax ) const
{
  theMax = ( theXYZ - myCenter ).Modulus();
  theMin = ( theMax - myRadius <0 ? 0.0 : theMax - myRadius );
  theMax += myRadius;
}

Standard_Boolean Bnd_Sphere::Project(const gp_XYZ& theNode, gp_XYZ& theProjNode, Standard_Real& theDist, Standard_Boolean& theInside) const
{ 
  theProjNode = myCenter;
  theDist = ( theNode - theProjNode ).Modulus();
  theInside = Standard_True;
  return Standard_True;
}

Standard_Real Bnd_Sphere::Distance(const gp_XYZ& theNode) const
{
  return ( theNode - myCenter ).Modulus();
}

Standard_Real Bnd_Sphere::SquareDistance(const gp_XYZ& theNode) const
{
  return ( theNode - myCenter ).SquareModulus();
}

void Bnd_Sphere::Add( const Bnd_Sphere& theOther)
{
  if ( myRadius < 0.0 )
  {
    // not initialised yet
    *this = theOther;
    return;
  }

  const Standard_Real aDist = (myCenter - theOther.myCenter).Modulus();
  if ( myRadius + aDist <= theOther.myRadius )
  {
    // the other sphere is larger and encloses this
    *this = theOther;
    return;
  }

  if ( theOther.myRadius + aDist <= myRadius )
    return; // this sphere encloses other

  // expansion
  const Standard_Real dfR = ( aDist + myRadius + theOther.myRadius ) * 0.5;
  const Standard_Real aParamOnDiam = ( dfR - myRadius ) / aDist;
  myCenter = myCenter * ( 1.0 - aParamOnDiam ) + theOther.myCenter * aParamOnDiam;
  myRadius = dfR;
  myIsValid = Standard_False;
}

Standard_Boolean Bnd_Sphere::IsOut( const Bnd_Sphere& theOther ) const
{ 
  return (myCenter - theOther.myCenter).SquareModulus() > (myRadius + theOther.myRadius) * (myRadius + theOther.myRadius); 
}

Standard_Boolean Bnd_Sphere::IsOut( const gp_XYZ& theXYZ,
                                    Standard_Real& theMaxDist) const
{
  Standard_Real aCurMinDist, aCurMaxDist;
  Distances( theXYZ, aCurMinDist, aCurMaxDist );
  if ( aCurMinDist > theMaxDist )
    return Standard_True;
  if( myIsValid && aCurMaxDist < theMaxDist )
    theMaxDist = aCurMaxDist;
  return Standard_False;
}

Standard_Real Bnd_Sphere::SquareExtent() const
{ 
  return 4 * myRadius * myRadius; 
}
