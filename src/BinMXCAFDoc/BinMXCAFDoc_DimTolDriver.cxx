// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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


#include <BinMXCAFDoc_DimTolDriver.ixx>
#include <XCAFDoc_DimTol.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>

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
