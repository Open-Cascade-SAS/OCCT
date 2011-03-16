// File:      XmlMDF_ADriver.cxx
// Created:   Mon Jul  9 12:29:49 MSK DST 2001
// Author:    Julia DOROVSKIKH
// Copyright: Matra Datavision 2001

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
