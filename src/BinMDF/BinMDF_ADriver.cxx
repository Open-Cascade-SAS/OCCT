// File:      BinMDF_ADriver.cxx
// Created:   29.10.02 19:26:53
// Author:    Michael SAZONOV
// Copyright: Open CASCADE 2002

#include <BinMDF_ADriver.ixx>

//=======================================================================
//function : BinMDF_ADriver
//purpose  : Constructor
//=======================================================================

BinMDF_ADriver::BinMDF_ADriver (const Handle(CDM_MessageDriver)& theMsgDriver,
                                const Standard_CString           theName)
     : myMessageDriver (theMsgDriver)
{
  if (theName)
    myTypeName = theName;
}
//=======================================================================
//function : WriteMessage
//purpose  :
//=======================================================================

void BinMDF_ADriver::WriteMessage
                              (const TCollection_ExtendedString& aMessage) const
{
  myMessageDriver -> Write (aMessage.ToExtString());
}
