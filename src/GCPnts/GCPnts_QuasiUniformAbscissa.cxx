// Copyright (c) 1996-1999 Matra Datavision
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

#include <GCPnts_QuasiUniformAbscissa.hxx>

#include <GCPnts_TCurveTypes.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

//=======================================================================
//function : GCPnts_QuasiUniformAbscissa
//purpose  :
//=======================================================================
GCPnts_QuasiUniformAbscissa::GCPnts_QuasiUniformAbscissa()
: myDone (Standard_False),
  myNbPoints (0)
{
  //
} 

//=======================================================================
//function : GCPnts_QuasiUniformAbscissa
//purpose  :
//=======================================================================
GCPnts_QuasiUniformAbscissa::GCPnts_QuasiUniformAbscissa (const Adaptor3d_Curve& theC,
                                                          const Standard_Integer theNbPoints)
: myDone (Standard_False),
  myNbPoints (0)
{
  Initialize (theC, theNbPoints);
}

//=======================================================================
//function : GCPnts_QuasiUniformAbscissa
//purpose  :
//=======================================================================
GCPnts_QuasiUniformAbscissa::GCPnts_QuasiUniformAbscissa (const Adaptor3d_Curve& theC,
                                                          const Standard_Integer theNbPoints,
                                                          const Standard_Real theU1,
                                                          const Standard_Real theU2)
: myDone (Standard_False),
  myNbPoints (0)
{
  Initialize (theC, theNbPoints, theU1, theU2);
}

//=======================================================================
//function : GCPnts_QuasiUniformAbscissa
//purpose  :
//=======================================================================
GCPnts_QuasiUniformAbscissa::GCPnts_QuasiUniformAbscissa (const Adaptor2d_Curve2d& theC,
                                                          const Standard_Integer theNbPoints)
: myDone (Standard_False),
  myNbPoints (0)
{
  Initialize (theC, theNbPoints);
}

//=======================================================================
//function : GCPnts_QuasiUniformAbscissa
//purpose  :
//=======================================================================
GCPnts_QuasiUniformAbscissa::GCPnts_QuasiUniformAbscissa (const Adaptor2d_Curve2d& theC,
                                                          const Standard_Integer theNbPoints,
                                                          const Standard_Real theU1,
                                                          const Standard_Real theU2)
: myDone (Standard_False),
  myNbPoints (0)
{
  Initialize (theC, theNbPoints, theU1, theU2);
}

//=======================================================================
//function : Initialize
//purpose  :
//=======================================================================
void GCPnts_QuasiUniformAbscissa::Initialize (const Adaptor3d_Curve& theC,
                                              const Standard_Integer theNbPoints)
{
  Initialize (theC, theNbPoints, theC.FirstParameter(), theC.LastParameter());
}

//=======================================================================
//function : Initialize
//purpose  :
//=======================================================================
void GCPnts_QuasiUniformAbscissa::Initialize (const Adaptor2d_Curve2d& theC,
                                              const Standard_Integer theNbPoints)
{
  Initialize (theC, theNbPoints, theC.FirstParameter(), theC.LastParameter());
}

//=======================================================================
//function : Initialize
//purpose  :
//=======================================================================
void GCPnts_QuasiUniformAbscissa::Initialize (const Adaptor3d_Curve& theC,
                                              const Standard_Integer theNbPoints,
                                              const Standard_Real theU1,
                                              const Standard_Real theU2)
{
  initialize (theC, theNbPoints, theU1, theU2);
}

//=======================================================================
//function : Initialize
//purpose  :
//=======================================================================
void GCPnts_QuasiUniformAbscissa::Initialize (const Adaptor2d_Curve2d& theC,
                                              const Standard_Integer theNbPoints,
                                              const Standard_Real theU1,
                                              const Standard_Real theU2)
{
  initialize (theC, theNbPoints, theU1, theU2);
}

//=======================================================================
//function : initialize
//purpose  :
//=======================================================================
template<class TheCurve>
void GCPnts_QuasiUniformAbscissa::initialize (const TheCurve& theC,
                                              const Standard_Integer theNbPoints,
                                              const Standard_Real theU1,
                                              const Standard_Real theU2)
{
  if (theC.GetType() != GeomAbs_BezierCurve
   && theC.GetType() != GeomAbs_BSplineCurve)
  {
    GCPnts_UniformAbscissa aUA (theC, theNbPoints, theU1, theU2);
    myDone = aUA.IsDone();
    myNbPoints = aUA.NbPoints();
    myParams = new TColStd_HArray1OfReal (1, myNbPoints);
    for (Standard_Integer aPntIter = 1 ; aPntIter <= myNbPoints; ++aPntIter)
    {
      myParams->SetValue (aPntIter, aUA.Parameter (aPntIter));
    }
    return;
  }

  Standard_ConstructionError_Raise_if (theNbPoints <= 1, "GCPnts_QuasiUniformAbscissa::Initialize(), number of points should be >= 2");

  // evaluate the approximative length of the 3dCurve
  myNbPoints = theNbPoints;
  Standard_Real aLength = 0.0;
  const Standard_Real dU = (theU2 - theU1) / (2 * theNbPoints - 1);

  TColgp_Array1OfPnt2d aLP (1, 2 * theNbPoints); // table Length <-> Param
  typename GCPnts_TCurveTypes<TheCurve>::Point aP1, aP2;
  aP1 = theC.Value (theU1);

  // On additionne toutes les distances
  for (Standard_Integer i = 0; i < 2 * theNbPoints; ++i)
  {
    aP2      = theC.Value (theU1 + i * dU);
    const Standard_Real aDist = aP1.Distance (aP2);
    aLength += aDist;
    aLP(i+1) = gp_Pnt2d (aLength, theU1 + (i * dU));
    aP1      = aP2;
  }

  // On cherche a mettre NbPoints dans la curve.
  // on met les points environ a Length/NbPoints.
  if (IsEqual (aLength, 0.0))
  { //use usual analytical grid
    Standard_Real aStep = (theU2 - theU1) / (theNbPoints - 1);
    myParams = new TColStd_HArray1OfReal (1, theNbPoints);
    myParams->SetValue (1, theU1);
    for (Standard_Integer i = 2; i < theNbPoints; ++i)
    {
      myParams->SetValue (i, theU1 + aStep * (i - 1));
    }
  }
  else
  {
    const Standard_Real aDCorde = aLength / (theNbPoints - 1);
    Standard_Real aCorde  = aDCorde;
    Standard_Integer anIndex = 1;
    myParams = new TColStd_HArray1OfReal (1, theNbPoints);
    myParams->SetValue (1, theU1);
    for (Standard_Integer i = 2; i < theNbPoints; ++i)
    {
      while (aLP (anIndex).X() < aCorde)
      {
        ++anIndex;
      }
      Standard_Real  anAlpha = (aCorde - aLP(anIndex - 1).X()) / (aLP (anIndex).X() - aLP (anIndex-1).X());
      Standard_Real aU = aLP (anIndex - 1).Y() + anAlpha * (aLP (anIndex).Y() - aLP (anIndex-1).Y());
      myParams->SetValue (i, aU);
      aCorde = i * aDCorde;
    }
  }

  myParams->SetValue (theNbPoints, theU2);
  myDone = Standard_True;
}
