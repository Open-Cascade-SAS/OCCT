// File:      BinMXCAFDoc_DimTolDriver.cxx
// Created:   10.12.08 13:15:38
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

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
