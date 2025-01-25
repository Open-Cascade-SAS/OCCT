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

#include <BRepClass_FaceClassifier.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRepClass_FaceExplorer.hxx>
#include <BRepTools.hxx>
#include <Extrema_ExtPS.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <TopoDS_Face.hxx>

//=================================================================================================

BRepClass_FaceClassifier::BRepClass_FaceClassifier() {}

//=================================================================================================

BRepClass_FaceClassifier::BRepClass_FaceClassifier(BRepClass_FaceExplorer& F,
                                                   const gp_Pnt2d&         P,
                                                   const Standard_Real     Tol)
    : BRepClass_FClassifier(F, P, Tol)
{
}

//=================================================================================================

BRepClass_FaceClassifier::BRepClass_FaceClassifier(const TopoDS_Face&     theF,
                                                   const gp_Pnt&          theP,
                                                   const Standard_Real    theTol,
                                                   const Standard_Boolean theUseBndBox,
                                                   const Standard_Real    theGapCheckTol)
{
  Perform(theF, theP, theTol, theUseBndBox, theGapCheckTol);
}

//=================================================================================================

BRepClass_FaceClassifier::BRepClass_FaceClassifier(const TopoDS_Face&     theF,
                                                   const gp_Pnt2d&        theP,
                                                   const Standard_Real    theTol,
                                                   const Standard_Boolean theUseBndBox,
                                                   const Standard_Real    theGapCheckTol)
{
  Perform(theF, theP, theTol, theUseBndBox, theGapCheckTol);
}

//=================================================================================================

void BRepClass_FaceClassifier::Perform(const TopoDS_Face&     theF,
                                       const gp_Pnt2d&        theP,
                                       const Standard_Real    theTol,
                                       const Standard_Boolean theUseBndBox,
                                       const Standard_Real    theGapCheckTol)
{
  BRepClass_FaceExplorer aFex(theF);
  aFex.SetMaxTolerance(theGapCheckTol);
  aFex.SetUseBndBox(theUseBndBox);
  BRepClass_FClassifier::Perform(aFex, theP, theTol);
}

//=================================================================================================

void BRepClass_FaceClassifier::Perform(const TopoDS_Face&     theF,
                                       const gp_Pnt&          theP,
                                       const Standard_Real    theTol,
                                       const Standard_Boolean theUseBndBox,
                                       const Standard_Real    theGapCheckTol)
{
  Standard_Integer aNbExt, aIndice, i;
  Standard_Real    aU1, aU2, aV1, aV2, aMaxDist, aD;
  gp_Pnt2d         aPuv;
  Extrema_ExtPS    aExtrema;
  //
  aMaxDist = RealLast();
  aIndice  = 0;
  //
  BRepAdaptor_Surface aSurf(theF, Standard_False);
  BRepTools::UVBounds(theF, aU1, aU2, aV1, aV2);
  aExtrema.Initialize(aSurf, aU1, aU2, aV1, aV2, theTol, theTol);
  //
  // modified by NIZNHY-PKV Wed Aug 13 11:28:47 2008f
  rejected = Standard_True;
  // modified by NIZNHY-PKV Wed Aug 13 11:28:49 2008t
  aExtrema.Perform(theP);
  if (!aExtrema.IsDone())
  {
    return;
  }
  //
  aNbExt = aExtrema.NbExt();
  if (!aNbExt)
  {
    return;
  }
  //
  for (i = 1; i <= aNbExt; ++i)
  {
    aD = aExtrema.SquareDistance(i);
    if (aD < aMaxDist)
    {
      aMaxDist = aD;
      aIndice  = i;
    }
  }
  //
  if (aIndice)
  {
    aExtrema.Point(aIndice).Parameter(aU1, aU2);
    aPuv.SetCoord(aU1, aU2);
    Perform(theF, aPuv, theTol, theUseBndBox, theGapCheckTol);
  }
}
