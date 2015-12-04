// Created on: 1999-05-06
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade_SplitSurfaceAngle.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HSequenceOfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_SplitSurfaceAngle,ShapeUpgrade_SplitSurface)

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
  Standard_Real U1 = 0.,U2 = 0.;
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
   
