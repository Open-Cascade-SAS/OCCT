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

#include <ShapeUpgrade_SplitSurfaceArea.ixx>
#include <TColStd_HSequenceOfReal.hxx>
#include <GeomAdaptor_Surface.hxx>

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

