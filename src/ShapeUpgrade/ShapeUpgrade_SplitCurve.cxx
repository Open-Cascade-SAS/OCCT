// Created on: 1998-03-12
// Created by: Pierre BARRAS
// Copyright (c) 1998-1999 Matra Datavision
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

//    gka 30.04.99 S4137: re-worked

#include <Geom_BSplineCurve.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade.hxx>
#include <ShapeUpgrade_SplitCurve.hxx>
#include <Standard_Type.hxx>
#include <TColGeom_HArray1OfCurve.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_SplitCurve,Standard_Transient)

//=======================================================================
//function : ShapeUpgrade_SplitCurve
//purpose  : 
//=======================================================================
ShapeUpgrade_SplitCurve::ShapeUpgrade_SplitCurve() : myStatus(0)
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

 void ShapeUpgrade_SplitCurve::Init(const Standard_Real First,
				    const Standard_Real Last) 
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
//  if (ShapeUpgrade::Debug()) cout << "SplitCurve::Init"<<endl;

  myNbCurves = 1;
 // mySplitValues.Clear();
  mySplitValues = new TColStd_HSequenceOfReal;
  mySplitValues->Append(First);
  mySplitValues->Append(Last);
}

//=======================================================================
//function : SetSplitValues
//purpose  : 
//=======================================================================

 void ShapeUpgrade_SplitCurve::SetSplitValues (const Handle(TColStd_HSequenceOfReal)& SplitValues)
{
  Standard_Real precision = Precision::PConfusion();
  if(SplitValues.IsNull()) return;
  if(SplitValues->Length()==0) return;
    
  Standard_Real First = mySplitValues->Value(1), 
  Last = mySplitValues->Value(mySplitValues->Length()); 
  Standard_Integer i =1;
  Standard_Integer len = SplitValues->Length();
   for( Standard_Integer k = 2;k <= mySplitValues->Length();k++) {
     Last = mySplitValues->Value(k);
     for(; i <=  len; i++) {
       if( (First + precision) >= SplitValues->Value(i)) continue;
       if((Last - precision) <= SplitValues->Value(i)) break;
       mySplitValues->InsertBefore(k++,SplitValues->Value(i));
     }
     First = Last;
   }
}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

 void ShapeUpgrade_SplitCurve::Build(const Standard_Boolean /*Segment*/) 
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
}


//=======================================================================
//function : GlobalKnots
//purpose  : 
//=======================================================================

const Handle(TColStd_HSequenceOfReal)& ShapeUpgrade_SplitCurve::SplitValues() const
{
  return mySplitValues;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void ShapeUpgrade_SplitCurve::Perform(const Standard_Boolean Segment)
{
  Compute();
  //if ( ! mySplitValues.IsNull() ) 
  //  SetSplitValues(mySplitValues);
  Build(Segment);

}
//=======================================================================
//function : Compute
//purpose  : 
//===================================================================
void ShapeUpgrade_SplitCurve::Compute()  
{  
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
}
//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Standard_Boolean ShapeUpgrade_SplitCurve::Status(const ShapeExtend_Status status) const
{
  return ShapeExtend::DecodeStatus (myStatus, status);
}
