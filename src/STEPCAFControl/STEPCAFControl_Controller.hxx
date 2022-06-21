// Created on: 2000-10-05
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _STEPCAFControl_Controller_HeaderFile
#define _STEPCAFControl_Controller_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <STEPControl_Controller.hxx>


class STEPCAFControl_Controller;
DEFINE_STANDARD_HANDLE(STEPCAFControl_Controller, STEPControl_Controller)

//! Extends Controller from STEPControl in order to provide
//! ActorWrite adapted for writing assemblies from DECAF
//! Note that ActorRead from STEPControl is used for reading
//! (inherited automatically)
class STEPCAFControl_Controller : public STEPControl_Controller
{

public:

  
  //! Initializes the use of STEP Norm (the first time)
  Standard_EXPORT STEPCAFControl_Controller();
  
  //! Standard Initialisation. It creates a Controller for STEP-XCAF
  //! and records it to various names, available to select it later
  //! Returns True when done, False if could not be done
  Standard_EXPORT static Standard_Boolean Init();




  DEFINE_STANDARD_RTTIEXT(STEPCAFControl_Controller,STEPControl_Controller)

protected:




private:




};







#endif // _STEPCAFControl_Controller_HeaderFile
