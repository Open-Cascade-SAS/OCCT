// Created on: 1998-04-10
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


#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <NLPlate_HGPPConstraint.hxx>
#include <Plate_D1.hxx>
#include <Plate_D2.hxx>
#include <Plate_D3.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(NLPlate_HGPPConstraint,Standard_Transient)

void NLPlate_HGPPConstraint::SetUVFreeSliding(const Standard_Boolean) 
{
}

 void NLPlate_HGPPConstraint::SetIncrementalLoadAllowed(const Standard_Boolean ) 
{
}

 void NLPlate_HGPPConstraint::SetActiveOrder(const Standard_Integer ActiveOrder) 
{
  myActiveOrder = ActiveOrder;
}

 void NLPlate_HGPPConstraint::SetUV(const gp_XY& UV) 
{
  myUV = UV;
}
 void NLPlate_HGPPConstraint::SetOrientation(const Standard_Integer /*Orient*/) 
{
}

 void NLPlate_HGPPConstraint::SetG0Criterion(const Standard_Real /*TolDist*/) 
{
}

 void NLPlate_HGPPConstraint::SetG1Criterion(const Standard_Real /*TolAng*/) 
{
}

 void NLPlate_HGPPConstraint::SetG2Criterion(const Standard_Real /*TolCurv*/) 
{
}

 void NLPlate_HGPPConstraint::SetG3Criterion(const Standard_Real /*TolG3*/) 
{
}

 Standard_Boolean NLPlate_HGPPConstraint::UVFreeSliding() const
{
  return Standard_False;
}

 Standard_Boolean NLPlate_HGPPConstraint::IncrementalLoadAllowed() const
{
  return Standard_False;
}

const gp_XY& NLPlate_HGPPConstraint::UV() const
{
  return myUV;
}

static const gp_XYZ XYZnull(0.,0.,0.);
const gp_XYZ& NLPlate_HGPPConstraint::G0Target() const
{
  return XYZnull;
  //cette methode ne devrait pas etre appelee
}

static const Plate_D1 D1null(XYZnull,XYZnull);
const Plate_D1& NLPlate_HGPPConstraint::G1Target() const
{
  return D1null;
  //cette methode ne devrait pas etre appelee
}

static const Plate_D2 D2null(XYZnull,XYZnull,XYZnull);
const Plate_D2& NLPlate_HGPPConstraint::G2Target() const
{
  return D2null;
  //cette methode ne devrait pas etre appelee
}

static const Plate_D3 D3null(XYZnull,XYZnull,XYZnull,XYZnull);
const Plate_D3& NLPlate_HGPPConstraint::G3Target() const
{
  return D3null;
  //cette methode ne devrait pas etre applee
}

 Standard_Integer NLPlate_HGPPConstraint::Orientation() 
{
  return 0;
}
 Standard_Real NLPlate_HGPPConstraint::G0Criterion() const
{
  return 0.;
}

 Standard_Real NLPlate_HGPPConstraint::G1Criterion() const
{
  return 0.;
}

 Standard_Real NLPlate_HGPPConstraint::G2Criterion() const
{
  return 0.;
}

 Standard_Real NLPlate_HGPPConstraint::G3Criterion() const
{
  return 0.;
}


