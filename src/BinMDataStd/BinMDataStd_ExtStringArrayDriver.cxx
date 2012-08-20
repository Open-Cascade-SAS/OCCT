// Created on: 2004-08-24
// Created by: Pavel TELKOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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


#include <BinMDataStd_ExtStringArrayDriver.ixx>
#include <TDataStd_ExtStringArray.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <TColStd_Array1OfExtendedString.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>
#include <BinMDataStd.hxx>
//=======================================================================
//function : BinMDataStd_ExtStringArrayDriver
//purpose  : Constructor
//=======================================================================

BinMDataStd_ExtStringArrayDriver::BinMDataStd_ExtStringArrayDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_ExtStringArray)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_ExtStringArrayDriver::NewEmpty() const
{
  return new TDataStd_ExtStringArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_ExtStringArrayDriver::Paste
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

  Handle(TDataStd_ExtStringArray) anAtt =
    Handle(TDataStd_ExtStringArray)::DownCast(theTarget);
  anAtt->Init(aFirstInd, aLastInd);
  TColStd_Array1OfExtendedString& aTargetArray = anAtt->Array()->ChangeArray1();
  Standard_Boolean ok = Standard_True;
  for (Standard_Integer i = aFirstInd; i <= aLastInd; i ++)
  {
    TCollection_ExtendedString aStr;
    if ( !(theSource >> aStr) )
    {
      ok = Standard_False;
      break;
    }
    aTargetArray.SetValue( i, aStr );
  }

  if(ok) {
//#ifdef DEB
//  cout << "CurDocVersion = " << BinMDataStd::DocumentVersion() <<endl;
//#endif
    Standard_Boolean aDelta(Standard_False);
    if(BinMDataStd::DocumentVersion() > 2) {
      Standard_Byte aDeltaValue;
	  if (! (theSource >> aDeltaValue)) {
//#ifdef DEB
//      cout <<"DeltaValue = " << (Standard_Integer)aDeltaValue <<endl;
//#endif
	    return Standard_False;
	  }
      else
	aDelta = (Standard_Boolean)aDeltaValue;
	}
    anAtt->SetDelta(aDelta);
  }
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_ExtStringArrayDriver::Paste
                                (const Handle(TDF_Attribute)& theSource,
                                 BinObjMgt_Persistent&        theTarget,
                                 BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_ExtStringArray) anAtt =
    Handle(TDataStd_ExtStringArray)::DownCast(theSource);
  const TColStd_Array1OfExtendedString& aSourceArray = anAtt->Array()->Array1();
  const Standard_Integer aFirstInd = aSourceArray.Lower();
  const Standard_Integer aLastInd  = aSourceArray.Upper();
  theTarget << aFirstInd << aLastInd;
  for (Standard_Integer i = aFirstInd; i <= aLastInd; i ++)
    theTarget << anAtt->Value( i );

  theTarget << (Standard_Byte)anAtt->GetDelta();
}
