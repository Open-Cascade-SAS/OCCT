// Created on: 2001-09-19
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <TCollection_AsciiString.hxx>
#include <XmlLDrivers_NamespaceDef.hxx>

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
