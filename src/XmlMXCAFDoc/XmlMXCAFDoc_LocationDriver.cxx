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


#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TopLoc_Datum3D.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_LocationSet.hxx>
#include <XCAFDoc_Location.hxx>
#include <XmlMNaming.hxx>
#include <XmlMXCAFDoc_LocationDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_DOMString.hxx>
#include <XmlObjMgt_GP.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_LocationDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (DatumString,    "datum")
IMPLEMENT_DOMSTRING (LocationString, "location")
IMPLEMENT_DOMSTRING (PowerString,    "power")
IMPLEMENT_DOMSTRING (TrsfString,     "trsf")
IMPLEMENT_DOMSTRING (LocIdString,    "locId")

//=======================================================================
//function : XmlMXCAFDoc_LocationDriver
//purpose  : Constructor
//=======================================================================
XmlMXCAFDoc_LocationDriver::XmlMXCAFDoc_LocationDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "Location")
      , myLocations(0)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_LocationDriver::NewEmpty() const
{
  return (new XCAFDoc_Location());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean XmlMXCAFDoc_LocationDriver::Paste
                              (const XmlObjMgt_Persistent&  theSource,
                               const Handle(TDF_Attribute)& theTarget,
                               XmlObjMgt_RRelocationTable&  theRelocTable) const
{
  TopLoc_Location aLoc;
  Translate (theSource.Element(), aLoc, theRelocTable);

  Handle(XCAFDoc_Location) aT = Handle(XCAFDoc_Location)::DownCast (theTarget);
  aT->Set(aLoc);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_LocationDriver::Paste
                              (const Handle(TDF_Attribute)& theSource,
                               XmlObjMgt_Persistent&        theTarget,
                               XmlObjMgt_SRelocationTable&  theRelocTable) const
{
  Handle(XCAFDoc_Location) aS = Handle(XCAFDoc_Location)::DownCast(theSource);
  XmlObjMgt_Element anElem = theTarget.Element();
  Translate (aS->Get(), anElem, theRelocTable);
}

//=======================================================================
//function : Translate
//purpose  : .. from Transient to Persistent
//=======================================================================
void XmlMXCAFDoc_LocationDriver::Translate (const TopLoc_Location&      theLoc,
                                     XmlObjMgt_Element&          theParent,
                                     XmlObjMgt_SRelocationTable& theMap) const
{
  if (theLoc.IsIdentity())
  {
    return;
  }
  
  // The location is not identity  
  if( myLocations == 0 )
  {
#ifdef OCCT_DEBUG
    cout<<"Pointer to LocationSet is NULL\n";
#endif
    return;
  }
  
  //  Create Location element
  XmlObjMgt_Document aDoc = theParent.getOwnerDocument();
  XmlObjMgt_Element aLocElem = aDoc.createElement(::LocationString());
  
  Standard_Integer anId = myLocations->Add(theLoc);
  
  aLocElem.setAttribute (::LocIdString(), anId);
  theParent.appendChild (aLocElem);

  
  // In earlier version of this driver a datums from location stored in 
  // the relocation table, but now it's not necessary
  // (try to uncomment it if some problems appear)
  /*
  Handle(TopLoc_Datum3D) aDatum = theLoc.FirstDatum();
  
  if(!theMap.Contains(aDatum)) {
    theMap.Add(aDatum);
  }
  */

  //  Add next Location from the list
  Translate (theLoc.NextLocation(), aLocElem, theMap);
}

//=======================================================================
//function : Translate
//purpose  : .. from Persistent to Transient
//=======================================================================
Standard_Boolean XmlMXCAFDoc_LocationDriver::Translate
                                        (const XmlObjMgt_Element&    theParent,
                                         TopLoc_Location&            theLoc,
                                         XmlObjMgt_RRelocationTable& theMap) const
{
  XmlObjMgt_Element aLocElem =
    XmlObjMgt::FindChildByName (theParent, ::LocationString());
  if (aLocElem == NULL)
    return Standard_False;
  
  Standard_Integer aFileVer = XmlMNaming::DocumentVersion();
  if( aFileVer > 5 && myLocations == 0 )
  {
    return Standard_False;
  }
  
  Standard_Integer aPower;
  Handle(TopLoc_Datum3D) aDatum;
  
  if( aFileVer > 5 )
  {
    //  Get Location ID
    Standard_Integer anId;
    aLocElem.getAttribute (::LocIdString()).GetInteger (anId);
    
    const TopLoc_Location& aLoc = myLocations->Location(anId);
    aPower = aLoc.FirstPower();
    aDatum = aLoc.FirstDatum();
  } else
  {
    //  Get Power
    aLocElem.getAttribute (::PowerString()).GetInteger (aPower);

    //  get datum
    XmlObjMgt_Persistent aPD (aLocElem, ::DatumString());
    if (aPD.Id() <= 0) {
      Standard_Integer aDatumID;
      aLocElem.getAttribute (::DatumString()).GetInteger (aDatumID);
      if (aDatumID > 0 && theMap.IsBound (aDatumID))
        aDatum = Handle(TopLoc_Datum3D)::DownCast (theMap.Find (aDatumID));
      else
        return Standard_False;
    }else{
      gp_Trsf aTrsf;
      XmlObjMgt_GP::Translate (aPD.Element().getAttribute(::TrsfString()), aTrsf);
      aDatum = new TopLoc_Datum3D (aTrsf);
      theMap.Bind (aPD.Id(), aDatum);
    }
  }
  
  //  Get Next Location
  TopLoc_Location aNextLoc;
  Translate (aLocElem, aNextLoc, theMap);
  
  //  Calculate the result
  theLoc = aNextLoc * TopLoc_Location (aDatum).Powered (aPower);
  return Standard_True;
}
