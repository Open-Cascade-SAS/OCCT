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


#include <gp_Pnt.hxx>
#include <IntSurf_PntOn2S.hxx>

IntSurf_PntOn2S::IntSurf_PntOn2S ()
  : pt(0,0,0),u1(0),v1(0),u2(0),v2(0)
{}

void IntSurf_PntOn2S::SetValue (const gp_Pnt& Pt,
				const Standard_Boolean OnFirst,
				const Standard_Real U,
				const Standard_Real V) {

  pt = Pt;
  if (OnFirst) {
    u1 = U;
    v1 = V;
  }
  else {
    u2 = U;
    v2 = V;
  }
}


void IntSurf_PntOn2S::SetValue (const Standard_Boolean OnFirst,
				const Standard_Real U,
				const Standard_Real V) {

  if (OnFirst) {
    u1 = U;
    v1 = V;
  }
  else {
    u2 = U;
    v2 = V;
  }
}

gp_Pnt2d IntSurf_PntOn2S::ValueOnSurface(const Standard_Boolean OnFirst) const
{
  gp_Pnt2d PointOnSurf;
  if (OnFirst)
    PointOnSurf.SetCoord(u1,v1);
  else
    PointOnSurf.SetCoord(u2,v2);
  return PointOnSurf;
}

void IntSurf_PntOn2S::ParametersOnSurface(const Standard_Boolean OnFirst,
                                          Standard_Real& U,
                                          Standard_Real& V) const
{
  if (OnFirst) {
    U = u1;
    V = v1;
  }
  else {
    U = u2;
    V = v2;
  }
}

Standard_Boolean IntSurf_PntOn2S::IsSame( const IntSurf_PntOn2S& theOterPoint,
                                          const Standard_Real theTol3D,
                                          const Standard_Real theTol2D) const
{
  if(pt.SquareDistance(theOterPoint.Value()) > theTol3D*theTol3D)
    return Standard_False;

  if(theTol2D < 0.0)
  {//We need not compare 2D-coordinates of the points
    return Standard_True;
  }

  Standard_Real aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  theOterPoint.Parameters(aU1, aV1, aU2, aV2);

  gp_Pnt2d aP1(u1, v1), aP2(aU1, aV1);

  if(!aP1.IsEqual(aP2, theTol2D))
    return Standard_False;

  aP1.SetCoord(u2, v2);
  aP2.SetCoord(aU2, aV2);

  if(!aP1.IsEqual(aP2, theTol2D))
    return Standard_False;

  return Standard_True;
}
