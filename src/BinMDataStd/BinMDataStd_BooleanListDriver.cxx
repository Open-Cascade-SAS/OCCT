// File:        BinMDataStd_BooleanListDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <BinMDataStd_BooleanListDriver.ixx>
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ListIteratorOfListOfByte.hxx>
#include <TColStd_Array1OfByte.hxx>

//=======================================================================
//function : BinMDataStd_BooleanListDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_BooleanListDriver::BinMDataStd_BooleanListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_BooleanList)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_BooleanListDriver::NewEmpty() const
{
  return new TDataStd_BooleanList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_BooleanListDriver::Paste(const BinObjMgt_Persistent&  theSource,
						      const Handle(TDF_Attribute)& theTarget,
						      BinObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aIndex, aFirstInd, aLastInd;
  if (! (theSource >> aFirstInd >> aLastInd))
    return Standard_False;
  const Standard_Integer aLength = aLastInd - aFirstInd + 1;
  if (aLength <= 0)
    return Standard_False;

  TColStd_Array1OfByte aTargetArray(aFirstInd, aLastInd);
  theSource.GetByteArray (&aTargetArray(aFirstInd), aLength);

  Handle(TDataStd_BooleanList) anAtt = Handle(TDataStd_BooleanList)::DownCast(theTarget);
  for (aIndex = aFirstInd; aIndex <= aLastInd; aIndex++)
  {
    anAtt->Append(aTargetArray.Value(aIndex) ? Standard_True : Standard_False);
  }
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_BooleanListDriver::Paste(const Handle(TDF_Attribute)& theSource,
					  BinObjMgt_Persistent&        theTarget,
					  BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_BooleanList) anAtt = Handle(TDataStd_BooleanList)::DownCast(theSource);
  const Standard_Integer aFirstInd = 1;
  const Standard_Integer aLastInd  = anAtt->Extent();
  const Standard_Integer aLength   = aLastInd - aFirstInd + 1;
  if (aLength <= 0)
    return;
  theTarget << aFirstInd << aLastInd;
  TColStd_Array1OfByte aSourceArray(aFirstInd, aLastInd);
  if (aLastInd >= 1)
  {
    TDataStd_ListIteratorOfListOfByte itr(anAtt->List());
    for (Standard_Integer i = 1; itr.More(); itr.Next(), i++)
    {
      aSourceArray.SetValue(i, itr.Value());
    }
    Standard_Byte *aPtr = (Standard_Byte *) &aSourceArray(aFirstInd);
    theTarget.PutByteArray(aPtr, aLength);
  }
}
