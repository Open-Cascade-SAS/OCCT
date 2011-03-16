// File:        XmlLDrivers_NamespaceDef.cxx
// Created:     Wed Sep 19 15:18:44 2001
// Author:      Alexander GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlLDrivers_NamespaceDef.ixx>

//=======================================================================
//function : XmlLDrivers_NamespaceDef
//purpose  : Empty constructor
//=======================================================================

XmlLDrivers_NamespaceDef::XmlLDrivers_NamespaceDef ()
 {}

//=======================================================================
//function : XmlLDrivers_NamespaceDef
//purpose  : Constructor
//=======================================================================

XmlLDrivers_NamespaceDef::XmlLDrivers_NamespaceDef
                                (const TCollection_AsciiString& thePrefix,
                                 const TCollection_AsciiString& theURI)
        : myPrefix (thePrefix), myURI (theURI)
{}

//=======================================================================
//function : Prefix
//purpose  : Query
//=======================================================================

const TCollection_AsciiString& XmlLDrivers_NamespaceDef::Prefix () const
{
  return myPrefix;
}

//=======================================================================
//function : URI
//purpose  : Query
//=======================================================================

const TCollection_AsciiString& XmlLDrivers_NamespaceDef::URI () const
{
  return myURI;
}
