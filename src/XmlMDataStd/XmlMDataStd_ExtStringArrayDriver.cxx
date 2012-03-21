// Created on: 2004-09-27
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


#include <XmlMDataStd_ExtStringArrayDriver.ixx>

#include <TDataStd_ExtStringArray.hxx>

#include <LDOM_MemManager.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlMDataStd.hxx>

IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString, "last")
IMPLEMENT_DOMSTRING (ExtString,       "string")
IMPLEMENT_DOMSTRING (IsDeltaOn,       "delta")
//=======================================================================
//function : XmlMDataStd_ExtStringArrayDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_ExtStringArrayDriver::XmlMDataStd_ExtStringArrayDriver
                        ( const Handle(CDM_MessageDriver)& theMsgDriver )
: XmlMDF_ADriver( theMsgDriver, NULL )
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_ExtStringArrayDriver::NewEmpty() const
{
  return ( new TDataStd_ExtStringArray() );
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_ExtStringArrayDriver::Paste
                                        ( const XmlObjMgt_Persistent&  theSource,
                                          const Handle(TDF_Attribute)& theTarget,
                                          XmlObjMgt_RRelocationTable& ) const
{
  Standard_Integer aFirstInd, aLastInd, ind;
  TCollection_ExtendedString aValue;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for ExtStringArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for ExtStringArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_ExtStringArray) aExtStringArray =
    Handle(TDataStd_ExtStringArray)::DownCast(theTarget);
  aExtStringArray->Init(aFirstInd, aLastInd);
  
  if ( !anElement.hasChildNodes() )
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve array of extended string");
    WriteMessage (aMessageString);
    return Standard_False;
  }

  LDOM_Node aCurNode = anElement.getFirstChild();
  LDOM_Element* aCurElement = (LDOM_Element*)&aCurNode;
  TCollection_ExtendedString aValueStr;
  for (ind = aFirstInd; ind <= aLastInd && *aCurElement != anElement.getLastChild(); ind++)
  {
    XmlObjMgt::GetExtendedString( *aCurElement, aValueStr );
    aExtStringArray->SetValue(ind, aValueStr);
    aCurNode = aCurElement->getNextSibling();
    aCurElement = (LDOM_Element*)&aCurNode;
  }

  XmlObjMgt::GetExtendedString( *aCurElement, aValueStr );
  aExtStringArray->SetValue( aLastInd, aValueStr );

#ifdef DEB
  //cout << "CurDocVersion = " << XmlMDataStd::DocumentVersion() <<endl;
#endif
  Standard_Boolean aDelta(Standard_False);
  
  if(XmlMDataStd::DocumentVersion() > 2) {
    Standard_Integer aDeltaValue;
    if (!anElement.getAttribute(::IsDeltaOn()).GetInteger(aDeltaValue)) 
      {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve the isDelta value"
                                 " for IntegerArray attribute as \"")
        + aDeltaValue + "\"";
	WriteMessage (aMessageString);
	return Standard_False;
      } 
    else
      aDelta = (Standard_Boolean)aDeltaValue;
  }
#ifdef DEB
  else if(XmlMDataStd::DocumentVersion() == -1)
    cout << "Current DocVersion field is not initialized. "  <<endl;
#endif
  aExtStringArray->SetDelta(aDelta);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ExtStringArrayDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                         XmlObjMgt_Persistent&        theTarget,
                                         XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_ExtStringArray) aExtStringArray =
    Handle(TDataStd_ExtStringArray)::DownCast(theSource);

  Standard_Integer aL = aExtStringArray->Lower(), anU = aExtStringArray->Upper();

  XmlObjMgt_Element& anElement = theTarget;

  if (aL != 1) anElement.setAttribute(::FirstIndexString(), aL);
  anElement.setAttribute(::LastIndexString(), anU);
  anElement.setAttribute(::IsDeltaOn(), aExtStringArray->GetDelta()); 

  // store a set of elements with string in each of them
  XmlObjMgt_Document aDoc = anElement.getOwnerDocument().Doc();
  
  for ( Standard_Integer i = aL; i <= anU; i++ )
  {
    TCollection_ExtendedString aValueStr = aExtStringArray->Value( i );
    XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
    XmlObjMgt::SetExtendedString( aCurTarget, aValueStr );
    anElement.appendChild( aCurTarget );
  }
}
