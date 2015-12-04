// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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


#include <gp_Ax1.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_PatternStd.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataXtd_PatternStd,TDataXtd_Pattern)

//=======================================================================
//function : GetPatternID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataXtd_PatternStd::GetPatternID() 
{
  static Standard_GUID TDataXtd_PatternStdID("2a96b61b-ec8b-11d0-bee7-080009dc3333");
  return TDataXtd_PatternStdID;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataXtd_PatternStd) TDataXtd_PatternStd::Set (const TDF_Label& L)
{ 
  Handle(TDataXtd_PatternStd) A; 
  if (!L.FindAttribute(TDataXtd_Pattern::GetID(), A)) {
    A = new TDataXtd_PatternStd(); 
    L.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : TDataXtd_PatternStd
//purpose  : 
//=======================================================================

TDataXtd_PatternStd::TDataXtd_PatternStd()
     : mySignature      (0),
       myAxis1Reversed  (Standard_False),
       myAxis2Reversed  (Standard_False)
{}

//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::Signature(const Standard_Integer signature) 
{
  // OCC2932 correction
  if(mySignature == signature) return;

  Backup();
  mySignature = signature;
}

//=======================================================================
//function : Axis1
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::Axis1(const Handle(TNaming_NamedShape)& Axis1) 
{
  // OCC2932 correction
  if(!myAxis1.IsNull())
    if(myAxis1->Get() == Axis1->Get())
      return;

  Backup();
  myAxis1 = Axis1;
}

//=======================================================================
//function : Axis2
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::Axis2(const Handle(TNaming_NamedShape)& Axis2) 
{
  // OCC2932 correction
  if(!myAxis2.IsNull())
    if(myAxis2->Get() == Axis2->Get())
      return;


  Backup();
  myAxis2 = Axis2;
}

//=======================================================================
//function : Axis1Reversed
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::Axis1Reversed(const Standard_Boolean Axis1Reversed) 
{
  // OCC2932 correction
  if(myAxis1Reversed == Axis1Reversed) return;

  Backup();
  myAxis1Reversed = Axis1Reversed;
}

//=======================================================================
//function : Axis2Reversed
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::Axis2Reversed(const Standard_Boolean Axis2Reversed) 
{
  // OCC2932 correction
  if(myAxis2Reversed == Axis2Reversed) return;

  Backup();
  myAxis2Reversed = Axis2Reversed;
}

//=======================================================================
//function : Value1
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::Value1(const Handle(TDataStd_Real)& value)
{
  // OCC2932 correction
  if(!myValue1.IsNull())
    if(myValue1->Get() == value->Get())
      return;

  Backup();
  myValue1 = value;
}


//=======================================================================
//function : Value2
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::Value2(const Handle(TDataStd_Real)& value)
{
  // OCC2932 correction
  if(!myValue2.IsNull())
    if(myValue2->Get() == value->Get())
      return;

  Backup();
  myValue2 = value;
}


//=======================================================================
//function : NbInstances1
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::NbInstances1(const Handle(TDataStd_Integer)& NbInstances1) 
{
  // OCC2932 correction
  if(!myNb1.IsNull())
    if(myNb1->Get() == NbInstances1->Get())
      return;

  Backup();
  myNb1 = NbInstances1;
}

//=======================================================================
//function : NbInstances2
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::NbInstances2(const Handle(TDataStd_Integer)& NbInstances2) 
{
  // OCC2932 correction
  if(!myNb2.IsNull())
    if(myNb2->Get() == NbInstances2->Get())
      return;

  Backup();
  myNb2 = NbInstances2;
}

//=======================================================================
//function : Mirror
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::Mirror(const Handle(TNaming_NamedShape)& plane)
{
  // OCC2932 correction
  if(!myMirror.IsNull()) {
    if(myMirror->Get() == plane->Get())
      return;
  }

  Backup();
  myMirror = plane;
}

//=======================================================================
//function : NbTrsfs
//purpose  : 
//=======================================================================

Standard_Integer TDataXtd_PatternStd::NbTrsfs() const
{
  Standard_Integer nb = 1;
  if (mySignature < 5) {
    if (!myNb1.IsNull()) nb = myNb1->Get();
    if (!myNb2.IsNull()) nb = nb*myNb2->Get();
    nb--;
  }
  return nb;
}


//=======================================================================
//function : ComputeTrsfs
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::ComputeTrsfs(TDataXtd_Array1OfTrsf& Trsfs) const
{
  Standard_Integer nb = 0;
  gp_Trsf trsf;

  if (mySignature < 5) {

    // recover direction and step
    gp_Ax1 axis1;
    TDataXtd_Geometry::Axis(myAxis1, axis1);
    if (myAxis1Reversed) axis1.Reverse();
    
    Standard_Real value1 = myValue1->Get();
    
    for (Standard_Integer i=2; i<=myNb1->Get(); i++) {
      if (mySignature != 2) {
	gp_Vec vec(axis1.Direction());
	vec *= (value1*(i-1));
	trsf.SetTranslation(vec);
      }
      else {
	trsf.SetRotation(axis1, value1*(i-1));
      }
      Trsfs(++nb) = trsf;
    }
    
    if (mySignature == 3 || mySignature == 4) {
      // recover direction and step
      gp_Ax1 axis2;
      TDataXtd_Geometry::Axis(myAxis2, axis2);
      if (myAxis2Reversed) axis2.Reverse();
      
      Standard_Real value2 = myValue2->Get();
      
      for (Standard_Integer j=2; j<=myNb2->Get(); j++) {
	gp_Trsf trsf2;
	if (mySignature ==3) {
	  gp_Vec vec(axis2.Direction());
	  vec *= (value2*(j-1));
	  trsf2.SetTranslation(vec);
	}
	else {
	  trsf2.SetRotation(axis2, value2*(j-1));
	}
	
	Trsfs(++nb) = trsf2;	
	for (Standard_Integer i=2; i<=myNb1->Get(); i++) {
	  trsf = trsf2;
	  trsf.Multiply(Trsfs(i-1));
	  Trsfs(++nb) = trsf;
	}
      }
    }  
  }
  else {
    gp_Pln pln;
    TDataXtd_Geometry::Plane(myMirror, pln);
    trsf.SetMirror(pln.Position().Ax2());
    Trsfs(++nb) = trsf;
  }
}

//=======================================================================
//function : PatternID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataXtd_PatternStd::PatternID() const
{
  return GetPatternID ();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::Restore(const Handle(TDF_Attribute)& With) 
{
  Handle(TDataXtd_PatternStd) PatternStd = Handle(TDataXtd_PatternStd)::DownCast(With);

  mySignature = PatternStd->Signature();
  myAxis1Reversed = PatternStd->Axis1Reversed();
  myAxis2Reversed = PatternStd->Axis2Reversed();
  
  myAxis1 = PatternStd->Axis1();
  myAxis2 = PatternStd->Axis2();
  myValue1 = PatternStd->Value1();
  myValue2 = PatternStd->Value2();
  myNb1 = PatternStd->NbInstances1();
  myNb2 = PatternStd->NbInstances2();
  myMirror = PatternStd->Mirror();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataXtd_PatternStd::NewEmpty() const
{
  return new TDataXtd_PatternStd();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::Paste(const Handle(TDF_Attribute)& Into,
				const Handle(TDF_RelocationTable)& RT) const
{
  Handle(TDataXtd_PatternStd) intof = Handle(TDataXtd_PatternStd)::DownCast(Into);

  intof->Signature(mySignature);
  intof->Axis1Reversed(myAxis1Reversed);
  intof->Axis2Reversed(myAxis2Reversed);
    
  if (mySignature < 5) {
    Handle(TNaming_NamedShape) axis;
    Handle(TDataStd_Real) value;
    Handle(TDataStd_Integer) nb;

    RT->HasRelocation(myAxis1, axis);
    intof->Axis1(axis);
    RT->HasRelocation(myValue1, value);
    intof->Value1(value);
    RT->HasRelocation(myNb1, nb);
    intof->NbInstances1(nb);

    if (mySignature > 2) {
      RT->HasRelocation(myAxis2, axis);
      intof->Axis2(axis);    
      RT->HasRelocation(myValue2, value);
      intof->Value2(value);
      RT->HasRelocation(myNb2, nb);
      intof->NbInstances2(nb);
    }
  }
  else {
    Handle(TNaming_NamedShape) plane;
    RT->HasRelocation(myMirror, plane);
    intof->Mirror(plane);
  }
}

//=======================================================================
//function : References
//purpose  : 
//=======================================================================

void TDataXtd_PatternStd::References(const Handle(TDF_DataSet)& aDataSet) const
{
  if (mySignature < 5) {
    aDataSet->AddAttribute(myAxis1);
    aDataSet->AddAttribute(myValue1);
    aDataSet->AddAttribute(myNb1);
    if (mySignature > 2) {
      aDataSet->AddAttribute(myAxis2);
      aDataSet->AddAttribute(myValue2);
      aDataSet->AddAttribute(myNb2);
    }
  }
  else {
    aDataSet->AddAttribute(myMirror);
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataXtd_PatternStd::Dump(Standard_OStream& anOS) const
{
  anOS << "TDataXtd_PatternStd";
  return anOS;
}



