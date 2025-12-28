// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

// modified     13.04.2009 Sergey Zaritchny

#include <BinMDataXtd_PatternStdDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataXtd_PatternStd.hxx>
#include <TDF_Attribute.hxx>
#include <TNaming_NamedShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataXtd_PatternStdDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataXtd_PatternStdDriver::BinMDataXtd_PatternStdDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, nullptr)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataXtd_PatternStdDriver::NewEmpty() const
{
  return (new TDataXtd_PatternStd());
}

//=================================================================================================

bool BinMDataXtd_PatternStdDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                         const occ::handle<TDF_Attribute>& theTarget,
                                         BinObjMgt_RRelocationTable&       theRelocTable) const
{
  occ::handle<TDataXtd_PatternStd> aP = occ::down_cast<TDataXtd_PatternStd>(theTarget);

  // signature
  int signature;
  if (!(theSource >> signature))
    return false;
  if (signature == 0)
    return true;
  aP->Signature(signature);

  // reversed flags
  int revFlags;
  if (!(theSource >> revFlags))
    return false;
  aP->Axis1Reversed((revFlags & 1) != 0);
  aP->Axis2Reversed((revFlags & 2) != 0);

  occ::handle<TNaming_NamedShape> TNS;
  int                             aNb;

  if (signature == 5) // mirror
  {
    if (!(theSource >> aNb))
      return false;
    if (theRelocTable.IsBound(aNb))
      TNS = occ::down_cast<TNaming_NamedShape>(theRelocTable.Find(aNb));
    else
    {
      TNS = new TNaming_NamedShape;
      theRelocTable.Bind(aNb, TNS);
    }
    aP->Mirror(TNS);
  }
  else
  {
    occ::handle<TDataStd_Real>    TReal;
    occ::handle<TDataStd_Integer> TInt;

    // axis 1
    if (!(theSource >> aNb))
      return false;
    if (theRelocTable.IsBound(aNb))
      TNS = occ::down_cast<TNaming_NamedShape>(theRelocTable.Find(aNb));
    else
    {
      TNS = new TNaming_NamedShape;
      theRelocTable.Bind(aNb, TNS);
    }
    aP->Axis1(TNS);

    // value 1
    if (!(theSource >> aNb))
      return false;
    if (theRelocTable.IsBound(aNb))
      TReal = occ::down_cast<TDataStd_Real>(theRelocTable.Find(aNb));
    else
    {
      TReal = new TDataStd_Real;
      theRelocTable.Bind(aNb, TReal);
    }
    aP->Value1(TReal);

    // number of instances 1
    if (!(theSource >> aNb))
      return false;
    if (theRelocTable.IsBound(aNb))
      TInt = occ::down_cast<TDataStd_Integer>(theRelocTable.Find(aNb));
    else
    {
      TInt = new TDataStd_Integer;
      theRelocTable.Bind(aNb, TInt);
    }
    aP->NbInstances1(TInt);

    if (signature > 2)
    {
      // axis 2
      if (!(theSource >> aNb))
        return false;
      if (theRelocTable.IsBound(aNb))
        TNS = occ::down_cast<TNaming_NamedShape>(theRelocTable.Find(aNb));
      else
      {
        TNS = new TNaming_NamedShape;
        theRelocTable.Bind(aNb, TNS);
      }
      aP->Axis2(TNS);

      // real value 2
      if (!(theSource >> aNb))
        return false;
      if (theRelocTable.IsBound(aNb))
        TReal = occ::down_cast<TDataStd_Real>(theRelocTable.Find(aNb));
      else
      {
        TReal = new TDataStd_Real;
        theRelocTable.Bind(aNb, TReal);
      }
      aP->Value2(TReal);

      // number of instances 2
      if (!(theSource >> aNb))
        return false;
      if (theRelocTable.IsBound(aNb))
        TInt = occ::down_cast<TDataStd_Integer>(theRelocTable.Find(aNb));
      else
      {
        TInt = new TDataStd_Integer;
        theRelocTable.Bind(aNb, TInt);
      }
      aP->NbInstances2(TInt);
    }
  }

  return true;
}

//=================================================================================================

void BinMDataXtd_PatternStdDriver::Paste(
  const occ::handle<TDF_Attribute>&                        theSource,
  BinObjMgt_Persistent&                                    theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>& theRelocTable) const
{
  occ::handle<TDataXtd_PatternStd> aP = occ::down_cast<TDataXtd_PatternStd>(theSource);

  // signature
  int signature = aP->Signature();
  if (signature < 1 || signature > 5)
    signature = 0;
  theTarget << signature;
  if (signature == 0)
    return;

  // reversed flags
  int revFlags = 0;
  if (aP->Axis1Reversed())
    revFlags |= 1;
  if (aP->Axis2Reversed())
    revFlags |= 2;
  theTarget << revFlags;

  int aNb;
  if (signature == 5) // mirror
  {
    occ::handle<TNaming_NamedShape> Plane = aP->Mirror();
    aNb                                   = theRelocTable.Add(Plane);
    theTarget << aNb;
  }
  else
  {
    // axis 1
    occ::handle<TNaming_NamedShape> Axis = aP->Axis1();
    aNb                                  = theRelocTable.Add(Axis);
    theTarget << aNb;

    // real value 1
    occ::handle<TDataStd_Real> Value = aP->Value1();
    aNb                              = theRelocTable.Add(Value);
    theTarget << aNb;

    // number of instances 1
    occ::handle<TDataStd_Integer> NbInstances = aP->NbInstances1();
    aNb                                       = theRelocTable.Add(NbInstances);
    theTarget << aNb;

    if (signature > 2)
    {
      // axis 2
      Axis = aP->Axis2();
      aNb  = theRelocTable.Add(Axis);
      theTarget << aNb;

      // real value 2
      Value = aP->Value2();
      aNb   = theRelocTable.Add(Value);
      theTarget << aNb;

      // number of instances 2
      NbInstances = aP->NbInstances2();
      aNb         = theRelocTable.Add(NbInstances);
      theTarget << aNb;
    }
  }
}
