// Created on: 2001-09-11
// Created by: Julia DOROVSKIKH
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

#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDocStd_FormatVersion.hxx>
#include <TopLoc_Datum3D.hxx>
#include <TopTools_LocationSet.hxx>
#include <XCAFDoc_Location.hxx>
#include <XmlMXCAFDoc_LocationDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_DOMString.hxx>
#include <XmlObjMgt_GP.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_LocationDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(DatumString, "datum")
IMPLEMENT_DOMSTRING(LocationString, "location")
IMPLEMENT_DOMSTRING(PowerString, "power")
IMPLEMENT_DOMSTRING(TrsfString, "trsf")
IMPLEMENT_DOMSTRING(LocIdString, "locId")

//=================================================================================================

XmlMXCAFDoc_LocationDriver::XmlMXCAFDoc_LocationDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, "xcaf", "Location"),
      myLocations(nullptr)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMXCAFDoc_LocationDriver::NewEmpty() const
{
  return (new XCAFDoc_Location());
}

//=================================================================================================

bool XmlMXCAFDoc_LocationDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                       const occ::handle<TDF_Attribute>& theTarget,
                                       XmlObjMgt_RRelocationTable&       theRelocTable) const
{
  TopLoc_Location aLoc;
  Translate(theSource.Element(), aLoc, theRelocTable);

  occ::handle<XCAFDoc_Location> aT = occ::down_cast<XCAFDoc_Location>(theTarget);
  aT->Set(aLoc);

  return true;
}

//=================================================================================================

void XmlMXCAFDoc_LocationDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                       XmlObjMgt_Persistent&             theTarget,
                                       XmlObjMgt_SRelocationTable&       theRelocTable) const
{
  occ::handle<XCAFDoc_Location> aS     = occ::down_cast<XCAFDoc_Location>(theSource);
  XmlObjMgt_Element             anElem = theTarget.Element();
  Translate(aS->Get(), anElem, theRelocTable);
}

//=======================================================================
// function : Translate
// purpose  : .. from Transient to Persistent
//=======================================================================
void XmlMXCAFDoc_LocationDriver::Translate(const TopLoc_Location&      theLoc,
                                           XmlObjMgt_Element&          theParent,
                                           XmlObjMgt_SRelocationTable& theMap) const
{
  if (theLoc.IsIdentity())
  {
    return;
  }

  // The location is not identity
  if (myLocations == nullptr)
  {
#ifdef OCCT_DEBUG
    std::cout << "Pointer to LocationSet is NULL\n";
#endif
    return;
  }

  //  Create Location element
  XmlObjMgt_Document aDoc     = theParent.getOwnerDocument();
  XmlObjMgt_Element  aLocElem = aDoc.createElement(::LocationString());

  int anId = myLocations->Add(theLoc);

  aLocElem.setAttribute(::LocIdString(), anId);
  theParent.appendChild(aLocElem);

  // In earlier version of this driver a datums from location stored in
  // the relocation table, but now it's not necessary
  // (try to uncomment it if some problems appear)
  /*
  occ::handle<TopLoc_Datum3D> aDatum = theLoc.FirstDatum();

  if(!theMap.Contains(aDatum)) {
    theMap.Add(aDatum);
  }
  */

  //  Add next Location from the list
  Translate(theLoc.NextLocation(), aLocElem, theMap);
}

//=======================================================================
// function : Translate
// purpose  : .. from Persistent to Transient
//=======================================================================
bool XmlMXCAFDoc_LocationDriver::Translate(const XmlObjMgt_Element&    theParent,
                                           TopLoc_Location&            theLoc,
                                           XmlObjMgt_RRelocationTable& theMap) const
{
  XmlObjMgt_Element aLocElem = XmlObjMgt::FindChildByName(theParent, ::LocationString());
  if (aLocElem == nullptr)
    return false;

  int aFileVer = theMap.GetHeaderData()->StorageVersion().IntegerValue();
  if (aFileVer >= TDocStd_FormatVersion_VERSION_6 && myLocations == nullptr)
  {
    return false;
  }

  int                         aPower;
  occ::handle<TopLoc_Datum3D> aDatum;

  if (aFileVer >= TDocStd_FormatVersion_VERSION_6)
  {
    //  Get Location ID
    int anId;
    aLocElem.getAttribute(::LocIdString()).GetInteger(anId);

    const TopLoc_Location& aLoc = myLocations->Location(anId);
    aPower                      = aLoc.FirstPower();
    aDatum                      = aLoc.FirstDatum();
  }
  else
  {
    //  Get Power
    aLocElem.getAttribute(::PowerString()).GetInteger(aPower);

    //  get datum
    XmlObjMgt_Persistent aPD(aLocElem, ::DatumString());
    if (aPD.Id() <= 0)
    {
      int aDatumID;
      aLocElem.getAttribute(::DatumString()).GetInteger(aDatumID);
      if (aDatumID > 0 && theMap.IsBound(aDatumID))
        aDatum = occ::down_cast<TopLoc_Datum3D>(theMap.Find(aDatumID));
      else
        return false;
    }
    else
    {
      gp_Trsf aTrsf;
      XmlObjMgt_GP::Translate(aPD.Element().getAttribute(::TrsfString()), aTrsf);
      aDatum = new TopLoc_Datum3D(aTrsf);
      theMap.Bind(aPD.Id(), aDatum);
    }
  }

  //  Get Next Location
  TopLoc_Location aNextLoc;
  Translate(aLocElem, aNextLoc, theMap);

  //  Calculate the result
  theLoc = aNextLoc * TopLoc_Location(aDatum).Powered(aPower);
  return true;
}
