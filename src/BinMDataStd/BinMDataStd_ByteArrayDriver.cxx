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


#include <BinMDataStd_ByteArrayDriver.ixx>
#include <TDataStd_ByteArray.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <BinMDataStd.hxx>
//=======================================================================
//function : BinMDataStd_ByteArrayDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_ByteArrayDriver::BinMDataStd_ByteArrayDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_ByteArray)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_ByteArrayDriver::NewEmpty() const
{
  return new TDataStd_ByteArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_ByteArrayDriver::Paste(const BinObjMgt_Persistent&  theSource,
						    const Handle(TDF_Attribute)& theTarget,
						    BinObjMgt_RRelocationTable&  ) const
{
#ifdef DEB
//  cout << "ByteArrayDriver::Retrieve: " << TypeName() << endl;
#endif
  Standard_Integer aFirstInd, aLastInd;
  if (! (theSource >> aFirstInd >> aLastInd))
    return Standard_False;
  if (aLastInd < aFirstInd)
    return Standard_False;

  TColStd_Array1OfByte aTargetArray(aFirstInd, aLastInd);
  theSource.GetByteArray (&aTargetArray(aFirstInd), aTargetArray.Length());

  Handle(TDataStd_ByteArray) anAtt = Handle(TDataStd_ByteArray)::DownCast(theTarget);
  Handle(TColStd_HArray1OfByte) bytes = new TColStd_HArray1OfByte(aFirstInd, aLastInd);
  for (Standard_Integer i = aFirstInd; i <= aLastInd; i++)
  {
    bytes->SetValue(i, aTargetArray.Value(i));
  }
  anAtt->ChangeArray(bytes);
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
void BinMDataStd_ByteArrayDriver::Paste(const Handle(TDF_Attribute)& theSource,
					BinObjMgt_Persistent&        theTarget,
					BinObjMgt_SRelocationTable&  ) const
{
#ifdef DEB
//  cout << "ByteArrayDriver::Store: " << TypeName() << endl;
#endif
  Handle(TDataStd_ByteArray) anAtt = Handle(TDataStd_ByteArray)::DownCast(theSource);
  const Standard_Integer aFirstInd = anAtt->Lower();
  const Standard_Integer aLastInd  = anAtt->Upper();
  if (aLastInd < aFirstInd)
    return;
  theTarget << aFirstInd << aLastInd;

  const Handle(TColStd_HArray1OfByte)& bytes = anAtt->InternalArray();
  Standard_Integer lower = bytes->Lower(), i = lower, upper = bytes->Upper();
  TColStd_Array1OfByte aSourceArray(lower, upper);
  for (; i <= upper; i++)
  {
    aSourceArray.SetValue(i, bytes->Value(i));
  }
  Standard_Byte *aPtr = (Standard_Byte *) &aSourceArray(lower);
  theTarget.PutByteArray(aPtr, bytes->Length());
  theTarget << (Standard_Byte)anAtt->GetDelta();
}
