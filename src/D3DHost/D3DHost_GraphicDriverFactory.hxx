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

#ifndef _D3DHost_GraphicDriverFactory_Header
#define _D3DHost_GraphicDriverFactory_Header

#include <OpenGl_GraphicDriverFactory.hxx>

//! This class for creation of D3DHost_GraphicDriver.
class D3DHost_GraphicDriverFactory : public OpenGl_GraphicDriverFactory
{
  DEFINE_STANDARD_RTTIEXT(D3DHost_GraphicDriverFactory, OpenGl_GraphicDriverFactory)
public:

  //! Empty constructor.
  Standard_EXPORT D3DHost_GraphicDriverFactory();

  //! Creates new empty graphic driver.
  Standard_EXPORT virtual Handle(Graphic3d_GraphicDriver) CreateDriver (const Handle(Aspect_DisplayConnection)& theDisp) Standard_OVERRIDE;

};

#endif //_D3DHost_GraphicDriverFactory_Header
