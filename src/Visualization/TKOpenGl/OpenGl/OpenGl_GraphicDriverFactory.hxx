// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _OpenGl_GraphicDriverFactory_Header
#define _OpenGl_GraphicDriverFactory_Header

#include <Graphic3d_GraphicDriverFactory.hxx>
#include <OpenGl_Caps.hxx>

//! This class for creation of OpenGl_GraphicDriver.
class OpenGl_GraphicDriverFactory : public Graphic3d_GraphicDriverFactory
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_GraphicDriverFactory, Graphic3d_GraphicDriverFactory)
public:
  //! Empty constructor.
  Standard_EXPORT OpenGl_GraphicDriverFactory();

  //! Creates new empty graphic driver.
  Standard_EXPORT virtual Handle(Graphic3d_GraphicDriver) CreateDriver(
    const Handle(Aspect_DisplayConnection)& theDisp) Standard_OVERRIDE;

  //! Return default driver options.
  const Handle(OpenGl_Caps)& DefaultOptions() const { return myDefaultCaps; }

  //! Set default driver options.
  void SetDefaultOptions(const Handle(OpenGl_Caps)& theOptions) { myDefaultCaps = theOptions; }

protected:
  Handle(OpenGl_Caps) myDefaultCaps;
};

#endif //_OpenGl_GraphicDriverFactory_Header
