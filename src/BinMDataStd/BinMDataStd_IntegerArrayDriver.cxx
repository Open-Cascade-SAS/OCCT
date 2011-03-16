// File:      BinMDataStd_IntegerArrayDriver.cxx
// Created:   31.10.02 19:43:57
// Author:    Michael SAZONOV
// Copyright: Open CASCADE 2002

#include <BinMDataStd_IntegerArrayDriver.ixx>
#include <TDataStd_IntegerArray.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <BinMDataStd.hxx>

//=======================================================================
//function : BinMDataStd_IntegerArrayDriver
//purpose  : Constructor
//=======================================================================

BinMDataStd_IntegerArrayDriver::BinMDataStd_IntegerArrayDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_IntegerArray)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_IntegerArrayDriver::NewEmpty() const
{
  return new TDataStd_IntegerArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_IntegerArrayDriver::Paste
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

  Handle(TDataStd_IntegerArray) anAtt =
    Handle(TDataStd_IntegerArray)::DownCast(theTarget);
  anAtt->Init(aFirstInd, aLastInd);
  TColStd_Array1OfInteger& aTargetArray = anAtt->Array()->ChangeArray1();
  if(!theSource.GetIntArray (&aTargetArray(aFirstInd), aLength))
    return Standard_False;
  Standard_Boolean aDelta(Standard_False);
#ifdef DEB
  //cout << "CurDocVersion = " << BinMDataStd::DocumentVersion() <<endl;
#endif
  if(BinMDataStd::DocumentVersion() > 2) {
    Standard_Byte aDeltaValue;
    if (! (theSource >> aDeltaValue))
      return Standard_False;
    else
      aDelta = (Standard_Boolean)aDeltaValue;
  }
#ifdef DEB
  else if(BinMDataStd::DocumentVersion() == -1)
    cout << "Current DocVersion field is not initialized. "  <<endl;
#endif
  anAtt->SetDelta(aDelta);
  return Standard_True; 
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_IntegerArrayDriver::Paste
                                (const Handle(TDF_Attribute)& theSource,
                                 BinObjMgt_Persistent&        theTarget,
                                 BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_IntegerArray) anAtt =
    Handle(TDataStd_IntegerArray)::DownCast(theSource);
  const TColStd_Array1OfInteger& aSourceArray = anAtt->Array()->Array1();
  const Standard_Integer aFirstInd = aSourceArray.Lower();
  const Standard_Integer aLastInd  = aSourceArray.Upper();
  const Standard_Integer aLength   = aLastInd - aFirstInd + 1;
  theTarget << aFirstInd << aLastInd;
  Standard_Integer *aPtr = (Standard_Integer *) &aSourceArray(aFirstInd);
  theTarget.PutIntArray (aPtr, aLength);
  theTarget << (Standard_Byte)anAtt->GetDelta(); 
}
