// File:        BinMDataStd_RealListDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <BinMDataStd_RealListDriver.ixx>
#include <TDataStd_RealList.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>

//=======================================================================
//function : BinMDataStd_RealListDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_RealListDriver::BinMDataStd_RealListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_RealList)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_RealListDriver::NewEmpty() const
{
  return new TDataStd_RealList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_RealListDriver::Paste(const BinObjMgt_Persistent&  theSource,
						   const Handle(TDF_Attribute)& theTarget,
						   BinObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aIndex, aFirstInd, aLastInd;
  if (! (theSource >> aFirstInd >> aLastInd))
    return Standard_False;
  const Standard_Integer aLength = aLastInd - aFirstInd + 1;
  if (aLength <= 0)
    return Standard_False;

  TColStd_Array1OfReal aTargetArray(aFirstInd, aLastInd);
  theSource.GetRealArray (&aTargetArray(aFirstInd), aLength);

  Handle(TDataStd_RealList) anAtt = Handle(TDataStd_RealList)::DownCast(theTarget);
  for (aIndex = aFirstInd; aIndex <= aLastInd; aIndex++)
  {
    anAtt->Append(aTargetArray.Value(aIndex));
  }
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_RealListDriver::Paste(const Handle(TDF_Attribute)& theSource,
				       BinObjMgt_Persistent&        theTarget,
				       BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_RealList) anAtt = Handle(TDataStd_RealList)::DownCast(theSource);
  const Standard_Integer aFirstInd = 1;
  const Standard_Integer aLastInd  = anAtt->Extent();
  const Standard_Integer aLength   = aLastInd - aFirstInd + 1;
  if (aLength <= 0)
    return;
  theTarget << aFirstInd << aLastInd;
  TColStd_Array1OfReal aSourceArray(aFirstInd, aLastInd);
  if (aLastInd >= 1)
  {
    TColStd_ListIteratorOfListOfReal itr(anAtt->List());
    for (Standard_Integer i = 1; itr.More(); itr.Next(), i++)
    {
      aSourceArray.SetValue(i, itr.Value());
    }
    Standard_Real *aPtr = (Standard_Real *) &aSourceArray(aFirstInd);
    theTarget.PutRealArray(aPtr, aLength);
  }
}
