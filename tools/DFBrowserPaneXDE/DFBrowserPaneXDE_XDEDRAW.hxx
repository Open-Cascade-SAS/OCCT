// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _DFBrowserPaneXDE_XDEDRAW__H
#define _DFBrowserPaneXDE_XDEDRAW__H

#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>

//! \class DFBrowserPaneXDE_XDEDRAW
//! \brief The tool that gives auxiliary methods for XDE elements manipulation
class DFBrowserPaneXDE_XDEDRAW
{
public:
  //! Dumps the attribute information into a string. The similar information is provided in Draw for XDE format
  //! \param theAttribute
  //! \return the GUID value
  Standard_EXPORT static TCollection_AsciiString GetAttributeInfo (Handle(TDF_Attribute) theAttribute);
};

#endif
