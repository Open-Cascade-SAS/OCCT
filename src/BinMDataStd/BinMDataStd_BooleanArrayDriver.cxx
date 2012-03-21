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
