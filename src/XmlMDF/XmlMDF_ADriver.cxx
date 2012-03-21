// Created on: 2001-07-09
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


#include <XmlMDF_ADriver.ixx>

//=======================================================================
//function : XmlMDF_ADriver
//purpose  : Constructor
//=======================================================================

XmlMDF_ADriver::XmlMDF_ADriver (const Handle(CDM_MessageDriver)& theMsgDriver,
                                const Standard_CString           theNS,
                                const Standard_CString           theName)
     : myMessageDriver (theMsgDriver)
{
  if (theNS != NULL)
    if (theNS[0] != '\0') {
      myTypeName = theNS;
      myTypeName += ':';
    }
  if (theName != NULL)
    myTypeName += theName;
}

//=======================================================================
//function : VersionNumber
//purpose  : default version number from which the driver is available
//=======================================================================

Standard_Integer XmlMDF_ADriver::VersionNumber () const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) XmlMDF_ADriver::SourceType () const
{
  return NewEmpty() -> DynamicType();
}

//=======================================================================
//function : TypeName
//purpose  : 
//=======================================================================

const TCollection_AsciiString& XmlMDF_ADriver::TypeName () const
{
  const Standard_CString aString = myTypeName.ToCString();
  if (myTypeName.Length() == 0 || aString [myTypeName.Length() - 1] == ':')
    (TCollection_AsciiString&)myTypeName += SourceType() -> Name();
  return myTypeName;
}

//=======================================================================
//function : WriteMessage
//purpose  : 
//=======================================================================

void XmlMDF_ADriver::WriteMessage
                              (const TCollection_ExtendedString& aMessage) const
{
  myMessageDriver -> Write (aMessage.ToExtString());
}
