// File:        XmlMDataStd_ReferenceArrayDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <XmlMDataStd_ReferenceArrayDriver.ixx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <LDOM_MemManager.hxx>

#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_ReferenceArray.hxx>

IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")
IMPLEMENT_DOMSTRING (ExtString,        "string")

//=======================================================================
//function : XmlMDataStd_ReferenceArrayDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_ReferenceArrayDriver::XmlMDataStd_ReferenceArrayDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_ReferenceArrayDriver::NewEmpty() const
{
  return new TDataStd_ReferenceArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_ReferenceArrayDriver::Paste(const XmlObjMgt_Persistent&  theSource,
							 const Handle(TDF_Attribute)& theTarget,
							 XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for ReferenceArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for ReferenceArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_ReferenceArray) aReferenceArray = Handle(TDataStd_ReferenceArray)::DownCast(theTarget);
  aReferenceArray->Init(aFirstInd, aLastInd);
  
  if (!anElement.hasChildNodes())
  {
    TCollection_ExtendedString aMessageString = 
      TCollection_ExtendedString("Cannot retrieve a Array of reference");
    WriteMessage (aMessageString);
    return Standard_False;
  }

  LDOM_Node aCurNode = anElement.getFirstChild();
  LDOM_Element* aCurElement = (LDOM_Element*)&aCurNode;
  XmlObjMgt_DOMString aValueStr;
  Standard_Integer i = aFirstInd;
  while (*aCurElement != anElement.getLastChild())
  {
    aValueStr = XmlObjMgt::GetStringValue( *aCurElement );
    if (aValueStr == NULL)
    {
      WriteMessage ("Cannot retrieve reference string from element");
      return Standard_False;
    }
    TCollection_AsciiString anEntry;
    if (XmlObjMgt::GetTagEntryString (aValueStr, anEntry) == Standard_False)
    {
      TCollection_ExtendedString aMessage =
	TCollection_ExtendedString ("Cannot retrieve reference from \"")
	  + aValueStr + '\"';
      WriteMessage (aMessage);
      return Standard_False;
    }
    // Find label by entry
    TDF_Label tLab; // Null label.
    if (anEntry.Length() > 0)
    {
      TDF_Tool::Label(aReferenceArray->Label().Data(), anEntry, tLab, Standard_True);
    }
    aReferenceArray->SetValue(i++, tLab);
    aCurNode = aCurElement->getNextSibling();
    aCurElement = (LDOM_Element*)&aCurNode;
  }

  // Last reference
  aValueStr = XmlObjMgt::GetStringValue( *aCurElement );
  if (aValueStr == NULL)
  {
    WriteMessage ("Cannot retrieve reference string from element");
    return Standard_False;
  }
  TCollection_AsciiString anEntry;
  if (XmlObjMgt::GetTagEntryString (aValueStr, anEntry) == Standard_False)
  {
    TCollection_ExtendedString aMessage =
      TCollection_ExtendedString ("Cannot retrieve reference from \"")
	+ aValueStr + '\"';
    WriteMessage (aMessage);
    return Standard_False;
  }
  // Find label by entry
  TDF_Label tLab; // Null label.
  if (anEntry.Length() > 0)
  {
    TDF_Tool::Label(aReferenceArray->Label().Data(), anEntry, tLab, Standard_True);
  }
  aReferenceArray->SetValue(i, tLab);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ReferenceArrayDriver::Paste(const Handle(TDF_Attribute)& theSource,
					     XmlObjMgt_Persistent&        theTarget,
					     XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_ReferenceArray) aReferenceArray = Handle(TDataStd_ReferenceArray)::DownCast(theSource);
  TDF_Label L = aReferenceArray->Label();
  if (L.IsNull())
  {
    WriteMessage ("Label of a ReferenceArray is Null.");
    return;
  }

  Standard_Integer aL = aReferenceArray->Lower();
  Standard_Integer anU = aReferenceArray->Upper();
  XmlObjMgt_Element& anElement = theTarget;
  anElement.setAttribute(::FirstIndexString(), aL);
  anElement.setAttribute(::LastIndexString(), anU);
  
  XmlObjMgt_Document aDoc = anElement.getOwnerDocument().Doc();
  
  for (Standard_Integer i = aL; i <= anU; i++)
  {
    if (L.IsDescendant(aReferenceArray->Value(i).Root()))
    {
      // Internal reference
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(aReferenceArray->Value(i), anEntry);

      XmlObjMgt_DOMString aDOMString;
      XmlObjMgt::SetTagEntryString (aDOMString, anEntry);
      XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
      XmlObjMgt::SetStringValue (aCurTarget, aDOMString, Standard_True);
      anElement.appendChild( aCurTarget );
    }
  }
}
