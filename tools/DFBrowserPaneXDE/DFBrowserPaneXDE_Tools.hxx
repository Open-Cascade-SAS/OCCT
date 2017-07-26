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

#ifndef DFBrowserPaneXDE_Tools_H
#define DFBrowserPaneXDE_Tools_H

#include <Standard.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Application.hxx>

#ifdef _MSC_VER
#pragma warning(disable:4127) // conditional expression is constant
#endif
#include <QMap>
#include <QWidget>
#include <QString>

class DFBrowserPane_AttributePaneAPI;

//! \namespace DFBrowserPaneXDE_Tools
//! The namespace that gives auxiliary methods for XCAFDoc elements manipulation
namespace DFBrowserPaneXDE_Tools
{
  //! Returns true if the application is XDE: application has document where there is XCAFDoc_DocumentTool attribute in
  //! a child label of the root
  //! \param theApplication checked application
  //! \return boolean value 
  Standard_EXPORT bool IsXDEApplication (const Handle(TDocStd_Application)& theApplication);
}

#endif
