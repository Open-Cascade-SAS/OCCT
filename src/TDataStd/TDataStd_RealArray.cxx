// Created on: 1999-06-16
// Created by: Sergey RUIN
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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_DeltaOnModificationOfRealArray.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_RealArray,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_RealArray::GetID() 
{ 
  static Standard_GUID TDataStd_RealArrayID ("2a96b61e-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_RealArrayID; 
}

//=======================================================================
//function : SetAttr
//purpose  : Implements Set functionality
//=======================================================================
static Handle(TDataStd_RealArray) SetAttr(const TDF_Label&       label,
                                          const Standard_Integer lower,
                                          const Standard_Integer upper,
                                          const Standard_Boolean isDelta,
                                          const Standard_GUID&   theGuid) 
{
  Handle(TDataStd_RealArray) A;
  if (!label.FindAttribute (theGuid, A)) 
  {
    A = new TDataStd_RealArray;
    A->Init (lower, upper);
    A->SetDelta(isDelta); 
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  else if (lower != A->Lower() || upper != A->Upper())
  {
    A->Init(lower, upper);
  }
  return A;
}

//=======================================================================
//function : TDataStd_RealArray
//purpose  : Empty Constructor
//=======================================================================

TDataStd_RealArray::TDataStd_RealArray() : myIsDelta(Standard_False)
{}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TDataStd_RealArray::Init(const Standard_Integer lower,
                              const Standard_Integer upper)
{
  Standard_RangeError_Raise_if(upper < lower,"TDataStd_RealArray::Init");  
  Backup(); // jfa 15.01.2003 for LH3D1378
  myValue = new TColStd_HArray1OfReal(lower, upper, 0.);
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_RealArray) TDataStd_RealArray::Set
                                          (const TDF_Label&       label,
                                           const Standard_Integer lower,
                                           const Standard_Integer upper,
                                           const Standard_Boolean isDelta) 
{
  return SetAttr(label, lower, upper, isDelta, GetID());
}

//=======================================================================
//function : Set
//purpose  : Set user defined attribute with specific ID
//=======================================================================

Handle(TDataStd_RealArray) TDataStd_RealArray::Set
                                          (const TDF_Label&       label,
                                           const Standard_GUID&   theGuid,
                                           const Standard_Integer lower,
                                           const Standard_Integer upper,
                                           const Standard_Boolean isDelta) 
{
  return SetAttr(label, lower, upper, isDelta, theGuid);
}
//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void TDataStd_RealArray::SetValue (const Standard_Integer index,
                                   const Standard_Real value) 
{
  // OCC2932 correction
  if(myValue.IsNull()) return;
  if(myValue->Value(index) == value)
    return;
  Backup();
  myValue->SetValue(index, value);
}


//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================

Standard_Real TDataStd_RealArray::Value (const Standard_Integer index) const 
{
  if(myValue.IsNull()) return RealFirst();
  return myValue->Value(index); 
}



//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_RealArray::Lower (void) const 
{ 
  if(myValue.IsNull()) return 0;
  return myValue->Lower(); 
}


//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_RealArray::Upper (void) const 
{ 
  if(myValue.IsNull()) return 0;
  return myValue->Upper(); 
}


//=======================================================================
//function : Length
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_RealArray::Length (void) const 
{
  if(myValue.IsNull()) return 0;
  return myValue->Length(); 
}


//=======================================================================
//function : ChangeArray
//purpose  : If value of <newArray> differs from <myValue>, Backup 
//         : performed and myValue refers to new instance of HArray1OfReal
//         : that holds <newArray>
//=======================================================================

void TDataStd_RealArray::ChangeArray(const Handle(TColStd_HArray1OfReal)& newArray,
                                     const Standard_Boolean isCheckItems) 
{
  Standard_Integer aLower    = newArray->Lower();
  Standard_Integer anUpper   = newArray->Upper();
  Standard_Boolean aDimEqual = Standard_False;
  Standard_Integer i;

  if (Lower() == aLower && Upper() == anUpper ) {
    aDimEqual = Standard_True;
    Standard_Boolean isEqual = Standard_True;
    if(isCheckItems) {
      for(i = aLower; i <= anUpper; i++) {
        if(myValue->Value(i) != newArray->Value(i)) {
          isEqual = Standard_False;
          break;
        }
      }
      if(isEqual)
        return;
    }
  }

  Backup();

  if(myValue.IsNull() || !aDimEqual) 
    myValue = new TColStd_HArray1OfReal(aLower, anUpper);

  for(i = aLower; i <= anUpper; i++) 
    myValue->SetValue(i, newArray->Value(i));
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_RealArray::ID () const { return myID; }

//=======================================================================
//function : SetID
//purpose  :
//=======================================================================

void TDataStd_RealArray::SetID( const Standard_GUID&  theGuid)
{  
  if(myID == theGuid) return;
  Backup();
  myID = theGuid;
}

//=======================================================================
//function : SetID
//purpose  : sets default ID
//=======================================================================

void TDataStd_RealArray::SetID()
{
  Backup();
  myID = GetID();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_RealArray::NewEmpty () const
{  
  return new TDataStd_RealArray(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_RealArray::Restore(const Handle(TDF_Attribute)& With) 
{
  Standard_Integer i, lower, upper;
  Handle(TDataStd_RealArray) anArray = Handle(TDataStd_RealArray)::DownCast(With);
  if(!anArray->myValue.IsNull()) {
    lower = anArray->Lower();
    upper = anArray->Upper();
    myIsDelta = anArray->myIsDelta;
    myValue = new TColStd_HArray1OfReal(lower, upper);
    for(i = lower; i<=upper; i++)
      myValue->SetValue(i, anArray->Value(i)); 
    myID = anArray->ID();
  }
  else
    myValue.Nullify();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_RealArray::Paste (const Handle(TDF_Attribute)& Into,
                                const Handle(TDF_RelocationTable)& ) const
{
  if(!myValue.IsNull()) {    
    Handle(TDataStd_RealArray) anAtt = Handle(TDataStd_RealArray)::DownCast(Into);
    if(!anAtt.IsNull()) {
      anAtt->ChangeArray( myValue, Standard_False );
      anAtt->SetDelta(myIsDelta);
      anAtt->SetID(myID);
    }
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_RealArray::Dump (Standard_OStream& anOS) const
{  
  anOS << "\nRealArray::" << this <<" :";
  if(!myValue.IsNull()) {
    Standard_Integer i, lower, upper;
    lower = myValue->Lower();
    upper = myValue->Upper();
    for(i = lower; i<=upper; i++)
      anOS << " " <<myValue->Value(i);
  }
  anOS << " Delta is " << (myIsDelta ? "ON":"OFF");
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << endl;
  return anOS;
}

//=======================================================================
//function : DeltaOnModification
//purpose  : 
//=======================================================================

Handle(TDF_DeltaOnModification) TDataStd_RealArray::DeltaOnModification
(const Handle(TDF_Attribute)& OldAtt) const
{
  if(myIsDelta)
    return new TDataStd_DeltaOnModificationOfRealArray(Handle(TDataStd_RealArray)::DownCast (OldAtt));
  else return new TDF_DefaultDeltaOnModification(OldAtt);
}
