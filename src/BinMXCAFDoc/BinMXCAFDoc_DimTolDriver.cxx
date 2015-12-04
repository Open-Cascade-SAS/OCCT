// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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


#include <BinMXCAFDoc_DimTolDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_DimTol.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_DimTolDriver,BinMDF_ADriver)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BinMXCAFDoc_DimTolDriver::BinMXCAFDoc_DimTolDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
: BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_DimTol)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_DimTolDriver::NewEmpty() const
{
  return new XCAFDoc_DimTol();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_DimTolDriver::Paste(const BinObjMgt_Persistent& theSource,
                                                 const Handle(TDF_Attribute)& theTarget,
                                                 BinObjMgt_RRelocationTable& /*theRelocTable*/) const 
{
  Handle(XCAFDoc_DimTol) anAtt = Handle(XCAFDoc_DimTol)::DownCast(theTarget);
  Standard_Integer aKind, aFirstInd, aLastInd;
  TCollection_AsciiString aName, aDescr;
  if ( !(theSource >> aKind >> aName >> aDescr >> aFirstInd >> aLastInd) )
    return Standard_False;

  Handle(TColStd_HArray1OfReal) aHArr;
  const Standard_Integer aLength = aLastInd - aFirstInd + 1;
  if (aLength > 0 ) {
    aHArr = new TColStd_HArray1OfReal( aFirstInd, aLastInd );

    TColStd_Array1OfReal& aTargetArray = aHArr->ChangeArray1();
    if(!theSource.GetRealArray (&aTargetArray(aFirstInd), aLength))
      return Standard_False;
  }
  anAtt->Set(aKind, aHArr,
             new TCollection_HAsciiString( aName ),
             new TCollection_HAsciiString( aDescr ));
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMXCAFDoc_DimTolDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                     BinObjMgt_Persistent& theTarget,
                                     BinObjMgt_SRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_DimTol) anAtt = Handle(XCAFDoc_DimTol)::DownCast(theSource);
  theTarget << anAtt->GetKind();
  if ( !anAtt->GetName().IsNull() )
    theTarget << anAtt->GetName()->String();
  else
    theTarget << TCollection_AsciiString("");
  if ( !anAtt->GetDescription().IsNull() )
    theTarget << anAtt->GetDescription()->String();
  else
    theTarget << TCollection_AsciiString("");
  
  Handle(TColStd_HArray1OfReal) aHArr = anAtt->GetVal();
  Standard_Integer aFirstInd = 1, aLastInd = 0;
  if ( !aHArr.IsNull() ) {
    aFirstInd = aHArr->Lower();
    aLastInd = aHArr->Upper();
  }
  theTarget << aFirstInd << aLastInd;
  if ( !aHArr.IsNull() ) {
    const Standard_Integer aLength   = aLastInd - aFirstInd + 1;
    const TColStd_Array1OfReal& anArr = aHArr->Array1();
    Standard_Real *aPtr = (Standard_Real *) &anArr(aFirstInd);
    theTarget.PutRealArray (aPtr, aLength);
  }
}
