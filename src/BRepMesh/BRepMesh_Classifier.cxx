// Created on: 2016-07-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_Classifier.hxx>

#include <Precision.hxx>
#include <gp_Pnt2d.hxx>
#include <CSLib_Class2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_Classifier, Standard_Transient)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_Classifier::BRepMesh_Classifier()
{
}

//=======================================================================
//function : Destructor
//purpose  : 
//=======================================================================
BRepMesh_Classifier::~BRepMesh_Classifier()
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
TopAbs_State BRepMesh_Classifier::Perform(const gp_Pnt2d& thePoint) const
{
  Standard_Boolean isOut = Standard_False;
  Standard_Integer aNb   = myTabClass.Length();
  
  for (Standard_Integer i = 0; i < aNb; i++)
  {
    const Standard_Integer aCur = myTabClass(i)->SiDans(thePoint);
    if (aCur == 0)
    {
      // Point is ON, but mark it as OUT
      isOut = Standard_True;
    }
    else
    {
      isOut = myTabOrient(i) ? (aCur == -1) : (aCur == 1);
    }
    
    if (isOut)
    {
      return TopAbs_OUT;
    }
  }

  return TopAbs_IN;
}

//=======================================================================
//function : RegisterWire
//purpose  : 
//=======================================================================
void BRepMesh_Classifier::RegisterWire(
  const NCollection_Sequence<const gp_Pnt2d*>&   theWire,
  const std::pair<Standard_Real, Standard_Real>& theTolUV,
  const std::pair<Standard_Real, Standard_Real>& theRangeU,
  const std::pair<Standard_Real, Standard_Real>& theRangeV)
{
  const Standard_Integer aNbPnts = theWire.Length();
  if (aNbPnts < 2)
  {
    return;
  }

  // Accumulate angle
  TColgp_Array1OfPnt2d aPClass(1, aNbPnts);
  Standard_Real anAngle = 0.0;
  const gp_Pnt2d *p1 = theWire(1), *p2 = theWire(2), *p3;
  aPClass(1) = *p1;
  aPClass(2) = *p2;

  constexpr Standard_Real aAngTol = Precision::Angular();
  constexpr Standard_Real aSqConfusion =
    Precision::PConfusion() * Precision::PConfusion();

  for (Standard_Integer i = 1; i <= aNbPnts; i++)
  { 
    Standard_Integer ii = i + 2;
    if (ii > aNbPnts)
    {
      p3 = &aPClass(ii - aNbPnts);
    }
    else
    {
      p3 = theWire.Value(ii);
      aPClass(ii) = *p3;
    }

    const gp_Vec2d A(*p1,*p2), B(*p2,*p3);
    if (A.SquareMagnitude() > aSqConfusion && 
        B.SquareMagnitude() > aSqConfusion)
    {
      const Standard_Real aCurAngle    = A.Angle(B);
      const Standard_Real aCurAngleAbs = Abs(aCurAngle);
      // Check if vectors are opposite
      if (aCurAngleAbs > aAngTol && (M_PI - aCurAngleAbs) > aAngTol)
      {
        anAngle += aCurAngle;
        p1 = p2;
      }
    }
    p2 = p3;
  }
  // Check for zero angle - treat self intersecting wire as outer
  if (Abs(anAngle) < aAngTol)
    anAngle = 0.0;

  myTabClass.Append(new CSLib_Class2d(
                    aPClass, theTolUV.first, theTolUV.second,
                    theRangeU.first, theRangeV.first,
                    theRangeU.second, theRangeV.second));

  myTabOrient.Append( !(anAngle < 0.0) );
}
