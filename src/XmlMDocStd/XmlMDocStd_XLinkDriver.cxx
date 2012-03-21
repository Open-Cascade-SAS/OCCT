// Created on: 2001-09-04
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <XmlMDocStd_XLinkDriver.ixx>

#include <XmlObjMgt.hxx>

#include <TDocStd_XLink.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_DOMSTRING (DocEntryString, "documentEntry")

//=======================================================================
//function : XmlMDocStd_XLinkDriver
//purpose  : Constructor
//=======================================================================
XmlMDocStd_XLinkDriver::XmlMDocStd_XLinkDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDocStd_XLinkDriver::NewEmpty() const
{
  return (new TDocStd_XLink());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDocStd_XLinkDriver::Paste
                (const XmlObjMgt_Persistent&  theSource,
                 const Handle(TDF_Attribute)& theTarget,
                 XmlObjMgt_RRelocationTable&  ) const
{
  XmlObjMgt_DOMString anXPath = XmlObjMgt::GetStringValue (theSource);

  if (anXPath == NULL)
  {
    WriteMessage ("XLink: Cannot retrieve reference string from element");
    return Standard_False;
  }

  TCollection_AsciiString anEntry;
  if (XmlObjMgt::GetTagEntryString (anXPath, anEntry) == Standard_False)
  {
    TCollection_ExtendedString aMessage =
      TCollection_ExtendedString ("Cannot retrieve XLink reference from \"")
        + anXPath + '\"';
    WriteMessage (aMessage);
    return Standard_False;
  }

  Handle(TDocStd_XLink) aRef = Handle(TDocStd_XLink)::DownCast(theTarget);

  // set referenced label
  aRef->LabelEntry(anEntry);

  // document entry
  aRef->DocumentEntry(theSource.Element().getAttribute(::DocEntryString()));

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//           <label tag='1'>     <This is label entry 0:4:1>
//           ...
//           <label tag='8'>     <This is label entry 0:4:1:8>
//
//           <TDocStd_XLink id="621"> /document/label/label[@tag="4"]/label[@tag="1"]
//           </TDocStd_XLink>    <This is reference to label 0:4:1>
//=======================================================================
void XmlMDocStd_XLinkDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDocStd_XLink) aRef = Handle(TDocStd_XLink)::DownCast(theSource);
  if (!aRef.IsNull())
  {
    // reference
    TCollection_AsciiString anEntry = aRef->LabelEntry();
    XmlObjMgt_DOMString aDOMString;
    XmlObjMgt::SetTagEntryString (aDOMString, anEntry);
    XmlObjMgt::SetStringValue (theTarget, aDOMString);

    // document entry
    theTarget.Element().setAttribute(::DocEntryString(),
                                      aRef->DocumentEntry().ToCString());
  }
}
