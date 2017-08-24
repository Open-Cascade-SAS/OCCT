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

#ifndef DFBrowserPaneXDE_ATTRIBUTEPANECREATOR_H
#define DFBrowserPaneXDE_ATTRIBUTEPANECREATOR_H

#include <inspector/DFBrowserPane_AttributePaneCreatorAPI.hxx>

#include <Standard.hxx>
#include <Standard_Macro.hxx>

class DFBrowserPane_AttributePaneAPI;

//! \class DFBrowserPaneXDE_AttributePaneCreator
//! \brief This class can creates attribute pane for XDE attribute name.
//! It creates the following panes:
//! - custom panes for XCAFDoc attributes own panes
//! - extended panes (modified short information) for several types of attribute (use AttributeCommonPane)
//! - common panes described in DFBrowserPane library
class DFBrowserPaneXDE_AttributePaneCreator : public DFBrowserPane_AttributePaneCreatorAPI
{
public:

  //! Constructor
  Standard_EXPORT DFBrowserPaneXDE_AttributePaneCreator (DFBrowserPane_AttributePaneCreatorAPI* theStandardPaneCreator);

  //! Destructor
  virtual ~DFBrowserPaneXDE_AttributePaneCreator() Standard_OVERRIDE {}

  //! Creates attribute panes for XCAFDoc attributes, extended and common panes from DFBrowserPane library
  //! \param theAttributeName a type of attribute
  //! \return an attribute pane if it can be created for this type
  Standard_EXPORT virtual DFBrowserPane_AttributePaneAPI* CreateAttributePane
    (const Standard_CString& theAttributeName) Standard_OVERRIDE;

protected:

  //! Cretates pane for XCAFDoc attribute name
  //! \param theAttributeName a type of attribute
  //! \return an attribute pane if it can be created for this type
  DFBrowserPane_AttributePaneAPI* createXDEPane (const Standard_CString& theAttributeName);

private:

  DFBrowserPane_AttributePaneCreatorAPI* myStandardPaneCreator; //! pane creator for panes from DFBrowserPane library
};

#endif 
