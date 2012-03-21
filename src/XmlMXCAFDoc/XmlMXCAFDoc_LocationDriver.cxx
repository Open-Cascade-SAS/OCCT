// Created on: 2001-09-11
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <XmlMXCAFDoc_LocationDriver.ixx>

#include <XCAFDoc_Location.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_DOMString.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_GP.hxx>
#include <TopLoc_Datum3D.hxx>

#include <XmlMNaming.hxx>
#include <TopTools_LocationSet.hxx>

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
#ifdef DEB
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
        aDatum = (Handle(TopLoc_Datum3D)&) theMap.Find (aDatumID);
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
