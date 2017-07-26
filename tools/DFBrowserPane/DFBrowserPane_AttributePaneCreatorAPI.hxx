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

#ifndef DFBrowserPane_AttributePaneCreatorAPI_H
#define DFBrowserPane_AttributePaneCreatorAPI_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

class DFBrowserPane_AttributePaneAPI;

//! \class DFBrowserPane_AttributePaneCreatorAPI
//! \brief An interface to create attribute panes by attribute name.
class DFBrowserPane_AttributePaneCreatorAPI
{
public:

  //! Constructor
  DFBrowserPane_AttributePaneCreatorAPI() {}

  //! Destructor
  virtual ~DFBrowserPane_AttributePaneCreatorAPI() {}

  //! Creates attribute pane for attribute types
  //! \param theAttributeName a standard type of attribute
  //! \return an attribute pane if it can be created for this type
  virtual DFBrowserPane_AttributePaneAPI* CreateAttributePane (const Standard_CString& theAttributeName) = 0;
};

#endif 
