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
  const Handle(Message_Messenger)& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Location)->Name())
{
}

//=================================================================================================

Handle(TDF_Attribute) BinMXCAFDoc_LocationDriver::NewEmpty() const
{
  return new XCAFDoc_Location();
}

//=================================================================================================

Standard_Boolean BinMXCAFDoc_LocationDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                   const Handle(TDF_Attribute)& theTarget,
                                                   BinObjMgt_RRelocationTable&  theRelocTable) const
{
  Handle(XCAFDoc_Location) anAtt = Handle(XCAFDoc_Location)::DownCast(theTarget);
  TopLoc_Location          aLoc;
  Standard_Boolean         aRes = Translate(theSource, aLoc, theRelocTable);
  anAtt->Set(aLoc);
  return aRes;
}

//=================================================================================================

void BinMXCAFDoc_LocationDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                       BinObjMgt_Persistent&        theTarget,
                                       BinObjMgt_SRelocationTable&  theRelocTable) const
{
  Handle(XCAFDoc_Location) anAtt = Handle(XCAFDoc_Location)::DownCast(theSource);
  TopLoc_Location          aLoc  = anAtt->Get();
  Translate(aLoc, theTarget, theRelocTable);
}

//=================================================================================================

Standard_Boolean BinMXCAFDoc_LocationDriver::Translate(const BinObjMgt_Persistent& theSource,
                                                       TopLoc_Location&            theLoc,
                                                       BinObjMgt_RRelocationTable& theMap) const
{
  if (!myNSDriver.IsNull() && myNSDriver->IsQuickPart())
  {
    BinTools_IStream aDirectStream(*(const_cast<BinObjMgt_Persistent*>(&theSource)->GetIStream()));
    BinTools_ShapeReader* aReader =
      static_cast<BinTools_ShapeReader*>(myNSDriver->ShapeSet(Standard_True));
    theLoc = *(aReader->ReadLocation(aDirectStream));
    return Standard_True;
  }

  Standard_Integer anId = 0;
  theSource >> anId;

  if (anId == 0)
  {
    return Standard_True;
  }

  if (!myNSDriver.IsNull() && myNSDriver->IsQuickPart())
  { // read directly from the stream
  }

  Standard_Integer aFileVer = theMap.GetHeaderData()->StorageVersion().IntegerValue();
  if (aFileVer >= TDocStd_FormatVersion_VERSION_6 && myNSDriver.IsNull())
  {
    return Standard_False;
  }

  Standard_Integer       aPower(0);
  Handle(TopLoc_Datum3D) aDatum;

  if (aFileVer >= TDocStd_FormatVersion_VERSION_6)
  {
    const TopLoc_Location& aLoc = myNSDriver->GetShapesLocations().Location(anId);
    aPower                      = aLoc.FirstPower();
    aDatum                      = aLoc.FirstDatum();
  }
  else
  {
    theSource >> aPower;

    Standard_Integer aDatumID   = -1;
    Standard_Integer aReadDatum = -1;
    theSource >> aReadDatum;
    theSource >> aDatumID;
    if (aReadDatum != -1)
    {
      if (theMap.IsBound(aDatumID))
      {
        aDatum = Handle(TopLoc_Datum3D)::DownCast(theMap.Find(aDatumID));
      }
      else
        return Standard_False;
    }
    else
    {
      // read the datum's transformation
      gp_Trsf aTrsf;

      Standard_Real aScaleFactor;
      theSource >> aScaleFactor;
      aTrsf.SetScaleFactor(aScaleFactor);

      Standard_Integer aForm;
      theSource >> aForm;
      aTrsf.SetForm((gp_TrsfForm)aForm);

      Standard_Integer R, C;
      gp_Mat&          aMat = (gp_Mat&)aTrsf.HVectorialPart();
      for (R = 1; R <= 3; R++)
        for (C = 1; C <= 3; C++)
        {
          Standard_Real aVal;
          theSource >> aVal;
          aMat.SetValue(R, C, aVal);
        }

      Standard_Real x, y, z;
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
  return Standard_True;
}

//=================================================================================================

void BinMXCAFDoc_LocationDriver::Translate(const TopLoc_Location&      theLoc,
                                           BinObjMgt_Persistent&       theTarget,
                                           BinObjMgt_SRelocationTable& theMap) const
{
  if (!myNSDriver.IsNull() && myNSDriver->IsQuickPart())
  { // write directly to the stream
    Standard_OStream*     aDirectStream = theTarget.GetOStream();
    BinTools_ShapeWriter* aWriter =
      static_cast<BinTools_ShapeWriter*>(myNSDriver->ShapeSet(Standard_False));
    BinTools_OStream aStream(*aDirectStream);
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

  Standard_Integer anId = myNSDriver->GetShapesLocations().Add(theLoc);
  theTarget << anId;

  // In earlier version of this driver a datums from location stored in
  // the relocation table, but now it's not necessary
  // (try to uncomment it if some problems appear)
  /*
  Handle(TopLoc_Datum3D) aDatum = theLoc.FirstDatum();

  if(!theMap.Contains(aDatum)) {
    theMap.Add(aDatum);
  }
  */

  Translate(theLoc.NextLocation(), theTarget, theMap);
}
