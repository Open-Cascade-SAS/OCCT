// Created on: 1998-04-17
// Created by: Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _NLPlate_HPG0Constraint_HeaderFile
#define _NLPlate_HPG0Constraint_HeaderFile

#include <Standard.hxx>

#include <gp_XYZ.hxx>
#include <NLPlate_HGPPConstraint.hxx>
#include <Standard_Integer.hxx>
class gp_XY;

//! define a PinPoint G0 Constraint used to load a Non Linear
//! Plate
class NLPlate_HPG0Constraint : public NLPlate_HGPPConstraint
{

public:
  Standard_EXPORT NLPlate_HPG0Constraint(const gp_XY& UV, const gp_XYZ& Value);

  Standard_EXPORT virtual void SetUVFreeSliding(const bool UVFree) override;

  Standard_EXPORT virtual void SetIncrementalLoadAllowed(const bool ILA) override;

  Standard_EXPORT virtual bool UVFreeSliding() const override;

  Standard_EXPORT virtual bool IncrementalLoadAllowed() const override;

  Standard_EXPORT virtual int ActiveOrder() const override;

  Standard_EXPORT virtual bool IsG0() const override;

  Standard_EXPORT virtual const gp_XYZ& G0Target() const override;

  DEFINE_STANDARD_RTTIEXT(NLPlate_HPG0Constraint, NLPlate_HGPPConstraint)

private:
  gp_XYZ myXYZTarget;
  bool   UVIsFree;
  bool   IncrementalLoadingAllowed;
};

#endif // _NLPlate_HPG0Constraint_HeaderFile
