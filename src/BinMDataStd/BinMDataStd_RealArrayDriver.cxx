// File:      BinMDataStd_RealArrayDriver.cxx
// Created:   31.10.02 20:42:59
// Author:    Michael SAZONOV
// Copyright: Open CASCADE 2002

#include <BinMDataStd_RealArrayDriver.ixx>
#include <TDataStd_RealArray.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <BinMDataStd.hxx>

//=======================================================================
//function : BinMDataStd_RealArrayDriver
//purpose  : Constructor
//=======================================================================

BinMDataStd_RealArrayDriver::BinMDataStd_RealArrayDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_RealArray)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_RealArrayDriver::NewEmpty() const
{
  return new TDataStd_RealArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_RealArrayDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd;
  if (! (theSource >> aFirstInd >> aLastInd))
    return Standard_False;
  const Standard_Integer aLength = aLastInd - aFirstInd + 1;
  if (aLength <= 0)
    return Standard_False;

  Handle(TDataStd_RealArray) anAtt =
    Handle(TDataStd_RealArray)::DownCast(theTarget);
  anAtt->Init(aFirstInd, aLastInd);
  TColStd_Array1OfReal& aTargetArray = anAtt->Array()->ChangeArray1();
  if(!theSource.GetRealArray (&aTargetArray(aFirstInd), aLength))
    return Standard_False;
#ifdef DEB
  //cout << "CurDocVersion = " << BinMDataStd::DocumentVersion() <<endl;
#endif
  Standard_Boolean aDelta(Standard_False);
  if(BinMDataStd::DocumentVersion() > 2) {
    Standard_Byte aDeltaValue;
    if (! (theSource >> aDeltaValue))
      return Standard_False;
    else
      aDelta = (Standard_Boolean)aDeltaValue;
  }
  anAtt->SetDelta(aDelta);
  return Standard_True; 
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_RealArrayDriver::Paste
                                (const Handle(TDF_Attribute)& theSource,
                                 BinObjMgt_Persistent&        theTarget,
                                 BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_RealArray) anAtt =
    Handle(TDataStd_RealArray)::DownCast(theSource);
  const TColStd_Array1OfReal& aSourceArray = anAtt->Array()->Array1();
  const Standard_Integer aFirstInd = aSourceArray.Lower();
  const Standard_Integer aLastInd  = aSourceArray.Upper();
  const Standard_Integer aLength   = aLastInd - aFirstInd + 1;
  theTarget << aFirstInd << aLastInd;
  Standard_Real *aPtr = (Standard_Real *) &aSourceArray(aFirstInd);
  theTarget.PutRealArray (aPtr, aLength);
  theTarget << (Standard_Byte)anAtt->GetDelta(); 
}
