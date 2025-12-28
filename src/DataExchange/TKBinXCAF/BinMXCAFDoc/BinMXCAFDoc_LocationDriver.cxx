// Created on: 2005-05-17
// Created by: Eugeny NAPALKOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <BinMDataStd.hxx>
#include <BinMXCAFDoc_LocationDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinTools_LocationSet.hxx>
#include <BinTools_ShapeReader.hxx>
#include <BinTools_ShapeWriter.hxx>
#include <Message_Messenger.hxx>
#include <gp_Mat.hxx>
#include <gp_Trsf.hxx>
#include <gp_XYZ.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TopLoc_Datum3D.hxx>
#include <TopLoc_Location.hxx>
#include <XCAFDoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_LocationDriver, BinMDF_ADriver)

// #include <Precision.hxx>
//=================================================================================================

BinMXCAFDoc_LocationDriver::BinMXCAFDoc_LocationDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Location)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMXCAFDoc_LocationDriver::NewEmpty() const
{
  return new XCAFDoc_Location();
}

//=================================================================================================

bool BinMXCAFDoc_LocationDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                       const occ::handle<TDF_Attribute>& theTarget,
                                       BinObjMgt_RRelocationTable&       theRelocTable) const
{
  occ::handle<XCAFDoc_Location> anAtt = occ::down_cast<XCAFDoc_Location>(theTarget);
  TopLoc_Location               aLoc;
  bool                          aRes = Translate(theSource, aLoc, theRelocTable);
  anAtt->Set(aLoc);
  return aRes;
}

//=================================================================================================

void BinMXCAFDoc_LocationDriver::Paste(
  const occ::handle<TDF_Attribute>&                        theSource,
  BinObjMgt_Persistent&                                    theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>& theRelocTable) const
{
  occ::handle<XCAFDoc_Location> anAtt = occ::down_cast<XCAFDoc_Location>(theSource);
  TopLoc_Location               aLoc  = anAtt->Get();
  Translate(aLoc, theTarget, theRelocTable);
}

//=================================================================================================

bool BinMXCAFDoc_LocationDriver::Translate(const BinObjMgt_Persistent& theSource,
                                           TopLoc_Location&            theLoc,
                                           BinObjMgt_RRelocationTable& theMap) const
{
  if (!myNSDriver.IsNull() && myNSDriver->IsQuickPart())
  {
    BinTools_IStream aDirectStream(*(const_cast<BinObjMgt_Persistent*>(&theSource)->GetIStream()));
    BinTools_ShapeReader* aReader = static_cast<BinTools_ShapeReader*>(myNSDriver->ShapeSet(true));
    theLoc                        = *(aReader->ReadLocation(aDirectStream));
    return true;
  }

  int anId = 0;
  theSource >> anId;

  if (anId == 0)
  {
    return true;
  }

  if (!myNSDriver.IsNull() && myNSDriver->IsQuickPart())
  { // read directly from the stream
  }

  int aFileVer = theMap.GetHeaderData()->StorageVersion().IntegerValue();
  if (aFileVer >= TDocStd_FormatVersion_VERSION_6 && myNSDriver.IsNull())
  {
    return false;
  }

  int                         aPower(0);
  occ::handle<TopLoc_Datum3D> aDatum;

  if (aFileVer >= TDocStd_FormatVersion_VERSION_6)
  {
    const TopLoc_Location& aLoc = myNSDriver->GetShapesLocations().Location(anId);
    aPower                      = aLoc.FirstPower();
    aDatum                      = aLoc.FirstDatum();
  }
  else
  {
    theSource >> aPower;

    int aDatumID   = -1;
    int aReadDatum = -1;
    theSource >> aReadDatum;
    theSource >> aDatumID;
    if (aReadDatum != -1)
    {
      if (theMap.IsBound(aDatumID))
      {
        aDatum = occ::down_cast<TopLoc_Datum3D>(theMap.Find(aDatumID));
      }
      else
        return false;
    }
    else
    {
      // read the datum's transformation
      gp_Trsf aTrsf;

      double aScaleFactor;
      theSource >> aScaleFactor;
      aTrsf.SetScaleFactor(aScaleFactor);

      int aForm;
      theSource >> aForm;
      aTrsf.SetForm((gp_TrsfForm)aForm);

      int     R, C;
      gp_Mat& aMat = (gp_Mat&)aTrsf.HVectorialPart();
      for (R = 1; R <= 3; R++)
        for (C = 1; C <= 3; C++)
        {
          double aVal;
          theSource >> aVal;
          aMat.SetValue(R, C, aVal);
        }

      double x, y, z;
      theSource >> x >> y >> z;
      gp_XYZ& aLoc = (gp_XYZ&)aTrsf.TranslationPart();
      aLoc.SetX(x);
      aLoc.SetY(y);
      aLoc.SetZ(z);

      aDatum = new TopLoc_Datum3D(aTrsf);
      theMap.Bind(aDatumID, aDatum);
    }
  }

  //  Get Next Location
  TopLoc_Location aNextLoc;
  Translate(theSource, aNextLoc, theMap);

  //  Calculate the result
  theLoc = aNextLoc * TopLoc_Location(aDatum).Powered(aPower);
  return true;
}

//=================================================================================================

void BinMXCAFDoc_LocationDriver::Translate(
  const TopLoc_Location&                                   theLoc,
  BinObjMgt_Persistent&                                    theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>& theMap) const
{
  if (!myNSDriver.IsNull() && myNSDriver->IsQuickPart())
  { // write directly to the stream
    Standard_OStream*     aDirectStream = theTarget.GetOStream();
    BinTools_ShapeWriter* aWriter = static_cast<BinTools_ShapeWriter*>(myNSDriver->ShapeSet(false));
    BinTools_OStream      aStream(*aDirectStream);
    aWriter->WriteLocation(aStream, theLoc);
    return;
  }
  if (theLoc.IsIdentity())
  {
    theTarget.PutInteger(0);
    return;
  }

  // The location is not identity
  if (myNSDriver.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "NamedShape Driver is NULL\n";
#endif
    return;
  }

  int anId = myNSDriver->GetShapesLocations().Add(theLoc);
  theTarget << anId;

  // In earlier version of this driver a datums from location stored in
  // the relocation table, but now it's not necessary
  // (try to uncomment it if some problems appear)
  /*
  occ::handle<TopLoc_Datum3D> aDatum = theLoc.FirstDatum();

  if(!theMap.Contains(aDatum)) {
    theMap.Add(aDatum);
  }
  */

  Translate(theLoc.NextLocation(), theTarget, theMap);
}
