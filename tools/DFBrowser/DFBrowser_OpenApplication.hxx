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

#ifndef DFBrowser_OpenApplication_H
#define DFBrowser_OpenApplication_H

#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDocStd_Application.hxx>

//! \namespace DFBrowser_OpenApplication
namespace DFBrowser_OpenApplication
{

  //! Open the application by the name.
  //! \param theFileName a name of the file initialized the application
  //! \param isSTEPFile an output parameter, true if the file name is a STEP file
  //! \return an opened application
  Standard_EXPORT Handle(TDocStd_Application) OpenApplication (const TCollection_AsciiString& theFileName,
                                                               bool& isSTEPFile);

  //! Creates a new application if the name contains "stp" or "step" extension. The application is "BinOcaf",
  //! STEP reader transfers the file into the application
  //! \param theFileName a name of the file initialized the application
  //! \return a new application
  Standard_EXPORT Handle(TDocStd_Application) CreateApplicationBySTEPFile (const TCollection_AsciiString& theFileName);
}

#endif
