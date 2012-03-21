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


#include <BinMDataStd_ReferenceListDriver.ixx>
#include <TDataStd_ReferenceList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

//=======================================================================
//function : BinMDataStd_ReferenceListDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_ReferenceListDriver::BinMDataStd_ReferenceListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_ReferenceList)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_ReferenceListDriver::NewEmpty() const
{
  return new TDataStd_ReferenceList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_ReferenceListDriver::Paste(const BinObjMgt_Persistent&  theSource,
							const Handle(TDF_Attribute)& theTarget,
							BinObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd;
  if (! (theSource >> aFirstInd >> aLastInd))
    return Standard_False;
  const Standard_Integer aLength = aLastInd - aFirstInd + 1;
  if (aLength <= 0)
    return Standard_False;

  Handle(TDataStd_ReferenceList) anAtt = Handle(TDataStd_ReferenceList)::DownCast(theTarget);
  for (Standard_Integer i = aFirstInd; i <= aLastInd; i++)
  {
    TCollection_AsciiString entry;
    if ( !(theSource >> entry) )
      return Standard_False;
    TDF_Label L;
    TDF_Tool::Label(anAtt->Label().Data(), entry, L, Standard_True);
    if (!L.IsNull())
      anAtt->Append(L);
  }

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_ReferenceListDriver::Paste(const Handle(TDF_Attribute)& theSource,
					    BinObjMgt_Persistent&        theTarget,
					    BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_ReferenceList) anAtt = Handle(TDataStd_ReferenceList)::DownCast(theSource);
  if (anAtt->IsEmpty())
    return;
  Standard_Integer aFirstInd = 1, aLastInd = anAtt->Extent(), i = aFirstInd;
  theTarget << aFirstInd << aLastInd;
  TDF_ListIteratorOfLabelList itr(anAtt->List());
  for (; itr.More(); itr.Next(), i++)
  {
    TDF_Label L = itr.Value();
    if (!L.IsNull())
    {
      TCollection_AsciiString entry;
      TDF_Tool::Entry(L, entry);
      theTarget << entry;
    }
  }
}
