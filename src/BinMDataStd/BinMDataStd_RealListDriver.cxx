// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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
