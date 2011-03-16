// File:        BinMDataStd_BooleanArrayDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <BinMDataStd_BooleanArrayDriver.ixx>
#include <TDataStd_BooleanArray.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <TColStd_Array1OfInteger.hxx>

//=======================================================================
//function : BinMDataStd_BooleanArrayDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_BooleanArrayDriver::BinMDataStd_BooleanArrayDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_BooleanArray)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_BooleanArrayDriver::NewEmpty() const
{
  return new TDataStd_BooleanArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_BooleanArrayDriver::Paste(const BinObjMgt_Persistent&  theSource,
						       const Handle(TDF_Attribute)& theTarget,
						       BinObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd;
  if (! (theSource >> aFirstInd >> aLastInd))
    return Standard_False;
  if (aLastInd < aFirstInd)
    return Standard_False;

  TColStd_Array1OfByte aTargetArray(0, (aLastInd - aFirstInd + 1) >> 3);
  theSource.GetByteArray (&aTargetArray(0), aTargetArray.Length());

  Handle(TDataStd_BooleanArray) anAtt = Handle(TDataStd_BooleanArray)::DownCast(theTarget);
  anAtt->Init(aFirstInd, aLastInd);
  Handle(TColStd_HArray1OfByte) bytes = new TColStd_HArray1OfByte(aTargetArray.Lower(), aTargetArray.Upper());
  Standard_Integer lower = bytes->Lower(), i = lower, upper = bytes->Upper();
  for (; i <= upper; i++)
  {
    bytes->SetValue(i, aTargetArray.Value(i));
  }
  anAtt->SetInternalArray(bytes);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_BooleanArrayDriver::Paste(const Handle(TDF_Attribute)& theSource,
					   BinObjMgt_Persistent&        theTarget,
					   BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_BooleanArray) anAtt = Handle(TDataStd_BooleanArray)::DownCast(theSource);
  const Standard_Integer aFirstInd = anAtt->Lower();
  const Standard_Integer aLastInd  = anAtt->Upper();
  if (aLastInd < aFirstInd)
    return;
  theTarget << aFirstInd << aLastInd;

  const Handle(TColStd_HArray1OfByte)& bytes = anAtt->InternalArray();
  TColStd_Array1OfByte aSourceArray(bytes->Lower(), bytes->Upper());
  Standard_Integer lower = bytes->Lower(), i = lower, upper = bytes->Upper();
  for (; i <= upper; i++)
  {
    aSourceArray.SetValue(i, bytes->Value(i));
  }
  Standard_Byte *aPtr = (Standard_Byte *) &aSourceArray(lower);
  theTarget.PutByteArray(aPtr, upper - lower + 1);
}
