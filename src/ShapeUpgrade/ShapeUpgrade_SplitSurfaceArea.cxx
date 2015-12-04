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


#include <GeomAdaptor_Surface.hxx>
#include <ShapeUpgrade_SplitSurfaceArea.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HSequenceOfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_SplitSurfaceArea,ShapeUpgrade_SplitSurface)

//=======================================================================
//function : ShapeUpgrade_SplitSurfaceArea
//purpose  : 
//=======================================================================
ShapeUpgrade_SplitSurfaceArea::ShapeUpgrade_SplitSurfaceArea():
       ShapeUpgrade_SplitSurface()
{
  myNbParts =1;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

 void ShapeUpgrade_SplitSurfaceArea::Compute(const Standard_Boolean /*Segment*/) 
{
  if(myNbParts <= 1)
    return;
  
  GeomAdaptor_Surface ads(mySurface,myUSplitValues->Value(1),myUSplitValues->Value(2),
                          myVSplitValues->Value(1),myVSplitValues->Value(2));
  Standard_Real aKoefU = ads.UResolution(1.);
  Standard_Real aKoefV = ads.VResolution(1.);
  if(aKoefU ==0)
    aKoefU =1.;
  if(aKoefV ==0)
    aKoefV =1.;
  Standard_Real aUSize = fabs(myUSplitValues->Value(2) - myUSplitValues->Value(1))/aKoefU;
  Standard_Real aVSize = fabs(myVSplitValues->Value(2) - myVSplitValues->Value(1))/aKoefV;
  Standard_Real  aNbUV =  aUSize/aVSize;
  Handle(TColStd_HSequenceOfReal) aFirstSplit = (aNbUV <1. ? myVSplitValues : myUSplitValues);
  Handle(TColStd_HSequenceOfReal) aSecondSplit = (aNbUV <1. ? myUSplitValues : myVSplitValues);
  if(aNbUV<1)
    aNbUV = 1./aNbUV;
  
  Standard_Integer nbSplitF = (aNbUV >=  myNbParts ? myNbParts : RealToInt(ceil(sqrt(myNbParts*ceil(aNbUV)))));
  Standard_Integer nbSplitS = (aNbUV >=  myNbParts ? 0  : RealToInt(ceil((Standard_Real)myNbParts/(Standard_Real)nbSplitF)));
  if(nbSplitS ==1)
    nbSplitS++;
  if(!nbSplitF)
    return;
  Standard_Real aStep = (aFirstSplit->Value(2) - aFirstSplit->Value(1))/nbSplitF;
  Standard_Real aPrevPar = aFirstSplit->Value(1);
  Standard_Integer i =1;
  for( ; i < nbSplitF; i++) {
    Standard_Real aNextPar = aPrevPar + aStep;
    aFirstSplit->InsertBefore(i+1,aNextPar);
    aPrevPar = aNextPar;
  }
  
  if(nbSplitS) {
    aStep = (aSecondSplit->Value(2) - aSecondSplit->Value(1))/nbSplitS;
    aPrevPar = aSecondSplit->Value(1);
    for(i =1 ; i < nbSplitS; i++) {
      Standard_Real aNextPar = aPrevPar + aStep;
      aSecondSplit->InsertBefore(i+1,aNextPar);
      aPrevPar = aNextPar;
    }
  }
}

