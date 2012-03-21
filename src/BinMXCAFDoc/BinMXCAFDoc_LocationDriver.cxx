// Created on: 2005-05-17
// Created by: Eugeny NAPALKOV
// Copyright (c) 2005-2012 OPEN CASCADE SAS
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


#include <BinMXCAFDoc_LocationDriver.ixx>
#include <XCAFDoc_Location.hxx>

#include <TopLoc_Datum3D.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Trsf.hxx>
#include <gp_Mat.hxx>
#include <gp_XYZ.hxx>
//#include <Precision.hxx>
#include <BinMDataStd.hxx>
#include <BinTools_LocationSet.hxx>


//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_LocationDriver::BinMXCAFDoc_LocationDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Location)->Name())
     , myLocations(0) {
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_LocationDriver::NewEmpty() const {
  return new XCAFDoc_Location();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_LocationDriver::Paste(const BinObjMgt_Persistent& theSource,
						   const Handle(TDF_Attribute)& theTarget,
						   BinObjMgt_RRelocationTable& theRelocTable) const
{
  Handle(XCAFDoc_Location) anAtt = Handle(XCAFDoc_Location)::DownCast(theTarget);
  TopLoc_Location aLoc;
  Standard_Boolean aRes = Translate(theSource, aLoc, theRelocTable);
  anAtt->Set(aLoc);
  return aRes;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_LocationDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                       BinObjMgt_Persistent& theTarget,
                                       BinObjMgt_SRelocationTable& theRelocTable) const
{
  Handle(XCAFDoc_Location) anAtt = Handle(XCAFDoc_Location)::DownCast(theSource);
  TopLoc_Location aLoc = anAtt->Get();
  Translate(aLoc, theTarget, theRelocTable);
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_LocationDriver::Translate(const BinObjMgt_Persistent& theSource,
						       TopLoc_Location& theLoc,
						       BinObjMgt_RRelocationTable& theMap) const
{
  Standard_Integer anId = 0;
  theSource >> anId;
  
  if(anId == 0)
  {
    return Standard_True;
  }
  
  Standard_Integer aFileVer = BinMDataStd::DocumentVersion();
  if( aFileVer > 5 && myLocations == 0 )
  {
    return Standard_False;
  }
  
  Standard_Integer aPower;
  Handle(TopLoc_Datum3D) aDatum;
  
  if( aFileVer > 5 )
  {
    const TopLoc_Location& aLoc = myLocations->Location(anId);
    aPower = aLoc.FirstPower();
    aDatum = aLoc.FirstDatum();
  } else {
    theSource >> aPower;

    Standard_Integer aDatumID = -1;
    Standard_Integer aReadDatum = -1;
    theSource >> aReadDatum;
    theSource >> aDatumID;
    if(aReadDatum != -1) {
      if(theMap.IsBound(aDatumID)) {
        aDatum = Handle(TopLoc_Datum3D)::DownCast(theMap.Find(aDatumID));
      } else
        return Standard_False;
    } else {
      // read the datum's trasformation
      gp_Trsf aTrsf;

      Standard_Real aScaleFactor;
      theSource >> aScaleFactor;
      aTrsf._CSFDB_Setgp_Trsfscale(aScaleFactor);

      Standard_Integer aForm;
      theSource >> aForm;
      aTrsf._CSFDB_Setgp_Trsfshape((gp_TrsfForm)aForm);

      Standard_Integer R, C;
      gp_Mat& aMat = (gp_Mat&)aTrsf._CSFDB_Getgp_Trsfmatrix();
      for(R = 1; R <= 3; R++)
        for(C = 1; C <= 3; C++) {
          Standard_Real aVal;
          theSource >> aVal;
          aMat.SetValue(R, C, aVal);
        }

      Standard_Real x, y, z;
      theSource >> x >> y >> z;
      gp_XYZ& aLoc = (gp_XYZ&)aTrsf._CSFDB_Getgp_Trsfloc();
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

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_LocationDriver::Translate(const TopLoc_Location& theLoc,
                                           BinObjMgt_Persistent& theTarget,
                                           BinObjMgt_SRelocationTable& theMap) const
{
  if(theLoc.IsIdentity()) 
  {
    theTarget.PutInteger(0);
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
  
  Standard_Integer anId = myLocations->Add(theLoc);
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

