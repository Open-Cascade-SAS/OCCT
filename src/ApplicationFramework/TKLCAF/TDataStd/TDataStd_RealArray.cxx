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

#include <TDataStd_RealArray.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_DeltaOnModificationOfRealArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_RealArray, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_RealArray::GetID()
{
  static Standard_GUID TDataStd_RealArrayID("2a96b61e-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_RealArrayID;
}

//=======================================================================
// function : SetAttr
// purpose  : Implements Set functionality
//=======================================================================
static occ::handle<TDataStd_RealArray> SetAttr(const TDF_Label&       label,
                                          const int lower,
                                          const int upper,
                                          const bool isDelta,
                                          const Standard_GUID&   theGuid)
{
  occ::handle<TDataStd_RealArray> A;
  if (!label.FindAttribute(theGuid, A))
  {
    A = new TDataStd_RealArray;
    A->Init(lower, upper);
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

//=================================================================================================

TDataStd_RealArray::TDataStd_RealArray()
    : myIsDelta(false),
      myID(GetID())
{
}

//=================================================================================================

void TDataStd_RealArray::Init(const int lower, const int upper)
{
  Standard_RangeError_Raise_if(upper < lower, "TDataStd_RealArray::Init");
  Backup(); // jfa 15.01.2003 for LH3D1378
  myValue = new NCollection_HArray1<double>(lower, upper, 0.);
}

//=================================================================================================

occ::handle<TDataStd_RealArray> TDataStd_RealArray::Set(const TDF_Label&       label,
                                                   const int lower,
                                                   const int upper,
                                                   const bool isDelta)
{
  return SetAttr(label, lower, upper, isDelta, GetID());
}

//=======================================================================
// function : Set
// purpose  : Set user defined attribute with specific ID
//=======================================================================

occ::handle<TDataStd_RealArray> TDataStd_RealArray::Set(const TDF_Label&       label,
                                                   const Standard_GUID&   theGuid,
                                                   const int lower,
                                                   const int upper,
                                                   const bool isDelta)
{
  return SetAttr(label, lower, upper, isDelta, theGuid);
}

//=================================================================================================

void TDataStd_RealArray::SetValue(const int index, const double value)
{
  // OCC2932 correction
  if (myValue.IsNull())
    return;
  if (myValue->Value(index) == value)
    return;
  Backup();
  myValue->SetValue(index, value);
}

//=================================================================================================

double TDataStd_RealArray::Value(const int index) const
{
  if (myValue.IsNull())
    return RealFirst();
  return myValue->Value(index);
}

//=================================================================================================

int TDataStd_RealArray::Lower(void) const
{
  if (myValue.IsNull())
    return 0;
  return myValue->Lower();
}

//=================================================================================================

int TDataStd_RealArray::Upper(void) const
{
  if (myValue.IsNull())
    return 0;
  return myValue->Upper();
}

//=================================================================================================

int TDataStd_RealArray::Length(void) const
{
  if (myValue.IsNull())
    return 0;
  return myValue->Length();
}

//=======================================================================
// function : ChangeArray
// purpose  : If value of <newArray> differs from <myValue>, Backup
//         : performed and myValue refers to new instance of HArray1OfReal
//         : that holds <newArray>
//=======================================================================

void TDataStd_RealArray::ChangeArray(const occ::handle<NCollection_HArray1<double>>& newArray,
                                     const bool               isCheckItems)
{
  int aLower    = newArray->Lower();
  int anUpper   = newArray->Upper();
  bool aDimEqual = false;
  int i;

  if (!myValue.IsNull())
  {
    if (Lower() == aLower && Upper() == anUpper)
    {
      aDimEqual                = true;
      bool isEqual = true;
      if (isCheckItems)
      {
        for (i = aLower; i <= anUpper; i++)
        {
          if (myValue->Value(i) != newArray->Value(i))
          {
            isEqual = false;
            break;
          }
        }
        if (isEqual)
          return;
      }
    }
  }

  Backup();

  if (myValue.IsNull() || !aDimEqual)
    myValue = new NCollection_HArray1<double>(aLower, anUpper);

  for (i = aLower; i <= anUpper; i++)
    myValue->SetValue(i, newArray->Value(i));
}

//=================================================================================================

const Standard_GUID& TDataStd_RealArray::ID() const
{
  return myID;
}

//=================================================================================================

void TDataStd_RealArray::SetID(const Standard_GUID& theGuid)
{
  if (myID == theGuid)
    return;
  Backup();
  myID = theGuid;
}

//=================================================================================================

void TDataStd_RealArray::SetID()
{
  Backup();
  myID = GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataStd_RealArray::NewEmpty() const
{
  return new TDataStd_RealArray();
}

//=================================================================================================

void TDataStd_RealArray::Restore(const occ::handle<TDF_Attribute>& With)
{
  int           i, lower, upper;
  occ::handle<TDataStd_RealArray> anArray = occ::down_cast<TDataStd_RealArray>(With);
  if (!anArray->myValue.IsNull())
  {
    lower     = anArray->Lower();
    upper     = anArray->Upper();
    myIsDelta = anArray->myIsDelta;
    myValue   = new NCollection_HArray1<double>(lower, upper);
    for (i = lower; i <= upper; i++)
      myValue->SetValue(i, anArray->Value(i));
    myID = anArray->ID();
  }
  else
    myValue.Nullify();
}

//=================================================================================================

void TDataStd_RealArray::Paste(const occ::handle<TDF_Attribute>& Into,
                               const occ::handle<TDF_RelocationTable>&) const
{
  if (!myValue.IsNull())
  {
    occ::handle<TDataStd_RealArray> anAtt = occ::down_cast<TDataStd_RealArray>(Into);
    if (!anAtt.IsNull())
    {
      anAtt->ChangeArray(myValue, false);
      anAtt->SetDelta(myIsDelta);
      anAtt->SetID(myID);
    }
  }
}

//=================================================================================================

Standard_OStream& TDataStd_RealArray::Dump(Standard_OStream& anOS) const
{
  anOS << "\nRealArray::" << this << " :";
  if (!myValue.IsNull())
  {
    int i, lower, upper;
    lower = myValue->Lower();
    upper = myValue->Upper();
    for (i = lower; i <= upper; i++)
      anOS << " " << myValue->Value(i);
  }
  anOS << " Delta is " << (myIsDelta ? "ON" : "OFF");
  char sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << std::endl;
  return anOS;
}

//=================================================================================================

occ::handle<TDF_DeltaOnModification> TDataStd_RealArray::DeltaOnModification(
  const occ::handle<TDF_Attribute>& OldAtt) const
{
  if (myIsDelta)
    return new TDataStd_DeltaOnModificationOfRealArray(
      occ::down_cast<TDataStd_RealArray>(OldAtt));
  else
    return new TDF_DefaultDeltaOnModification(OldAtt);
}

//=================================================================================================

void TDataStd_RealArray::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  if (!myValue.IsNull())
  {
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myValue->Lower())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myValue->Upper())

    for (NCollection_Array1<double>::Iterator aValueIt(myValue->Array1()); aValueIt.More();
         aValueIt.Next())
    {
      const double& aValue = aValueIt.Value();
      OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aValue)
    }
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsDelta)
}
