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

#ifndef _HelixGeom_BuilderHelix_HeaderFile
#define _HelixGeom_BuilderHelix_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax2.hxx>
#include <HelixGeom_BuilderHelixGen.hxx>
class gp_Ax2;

//! Upper level class for geometrical algorithm of building
//! helix curves using arbitrary axis
class HelixGeom_BuilderHelix : public HelixGeom_BuilderHelixGen
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT HelixGeom_BuilderHelix();
  Standard_EXPORT virtual ~HelixGeom_BuilderHelix();

  //! Sets coordinate axes for helix
  Standard_EXPORT void SetPosition(const gp_Ax2& aAx2);

  //! Gets coordinate axes for helix
  Standard_EXPORT const gp_Ax2& Position() const;

  //! Performs calculations
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;

protected:
  gp_Ax2 myPosition;

private:
};

#endif // _HelixGeom_BuilderHelix_HeaderFile
