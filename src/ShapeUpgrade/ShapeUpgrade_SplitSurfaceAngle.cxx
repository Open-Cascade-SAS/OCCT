// Created on: 1999-05-06
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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



#include <ShapeUpgrade_SplitSurfaceAngle.ixx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <ShapeExtend.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Precision.hxx>

//=======================================================================
//function : ShapeUpgrade_SplitSurfaceAngle
//purpose  : 
//=======================================================================

ShapeUpgrade_SplitSurfaceAngle::ShapeUpgrade_SplitSurfaceAngle (const Standard_Real MaxAngle)
{
  myMaxAngle = MaxAngle;
}

//=======================================================================
//function : SetMaxAngle
//purpose  : 
//=======================================================================

void ShapeUpgrade_SplitSurfaceAngle::SetMaxAngle (const Standard_Real MaxAngle)
{
  myMaxAngle = MaxAngle;
}
     
//=======================================================================
//function : MaxAngle
//purpose  : 
//=======================================================================

double ShapeUpgrade_SplitSurfaceAngle::MaxAngle () const
{
  return myMaxAngle;
}
     
//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void ShapeUpgrade_SplitSurfaceAngle::Compute(const Standard_Boolean /*Segment*/)
{
  Handle(Geom_Surface) S;
  Standard_Real U1,U2;
  Standard_Boolean isRect = Standard_False;
  if(mySurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))){
    Handle(Geom_RectangularTrimmedSurface) rts = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(mySurface);
    isRect = Standard_True;
    Standard_Real V1,V2;
    rts->Bounds(U1,U2,V1,V2);
    S = rts->BasisSurface();
  }
  else if (mySurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    Handle(Geom_OffsetSurface) ofs = 
      Handle(Geom_OffsetSurface)::DownCast(mySurface);
    S = ofs->BasisSurface();
  }
  else 
    S = mySurface;
  
  if(S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))||
     S->IsKind(STANDARD_TYPE(Geom_ConicalSurface))||
     S->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))||
     S->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))||
     S->IsKind(STANDARD_TYPE(Geom_SphericalSurface))) {
    
    Standard_Real UFirst = myUSplitValues->Sequence().First();
    Standard_Real ULast  = myUSplitValues->Sequence().Last();
    Standard_Real maxAngle = myMaxAngle; //maximal u length of segment
    Standard_Real uLength = ULast-UFirst;
    Standard_Integer nbSegments = Standard_Integer((uLength-Precision::Angular())/maxAngle)+1;
    if(nbSegments==1)
      if(!isRect || !(uLength < maxAngle) || !((U2-U1) < maxAngle))
	myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE2);
    Standard_Real segAngle = uLength/nbSegments;
    Standard_Real currAngle = segAngle+UFirst;
    Handle(TColStd_HSequenceOfReal) splitValues = new TColStd_HSequenceOfReal;
    for( Standard_Integer i = 1; i < nbSegments; i++, currAngle+=segAngle)
      splitValues->Append(currAngle);
    SetUSplitValues ( splitValues );
  }
}
   
