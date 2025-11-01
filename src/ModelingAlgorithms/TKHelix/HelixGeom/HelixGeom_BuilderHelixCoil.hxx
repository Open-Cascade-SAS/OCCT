// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _HelixGeom_BuilderHelixCoil_HeaderFile
#define _HelixGeom_BuilderHelixCoil_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <HelixGeom_BuilderHelixGen.hxx>

//! Implementation of algorithm for building helix coil with
//! axis OZ
class HelixGeom_BuilderHelixCoil : public HelixGeom_BuilderHelixGen
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT HelixGeom_BuilderHelixCoil();
  Standard_EXPORT virtual ~HelixGeom_BuilderHelixCoil();

  //! Performs calculations
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;

protected:
private:
};

#endif // _HelixGeom_BuilderHelixCoil_HeaderFile
