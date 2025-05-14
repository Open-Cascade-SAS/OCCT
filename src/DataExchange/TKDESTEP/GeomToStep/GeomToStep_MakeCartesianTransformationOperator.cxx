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

#include <GeomToStep_MakeCartesianTransformationOperator.hxx>

#include <GeomToStep_MakeCartesianPoint.hxx>
#include <GeomToStep_MakeDirection.hxx>
#include <gp_Trsf.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>

//=============================================================================

GeomToStep_MakeCartesianTransformationOperator::GeomToStep_MakeCartesianTransformationOperator(
  const gp_Trsf&          theTrsf,
  const StepData_Factors& theLocalFactors)
{
  gp_Trsf anInvTrsf = theTrsf;
  anInvTrsf.Invert();
  Standard_Real aScale   = anInvTrsf.ScaleFactor();
  gp_Mat        anInvMat = anInvTrsf.HVectorialPart();

  gp_Mat aMat         = theTrsf.HVectorialPart();
  gp_XYZ aTranslation = anInvTrsf.TranslationPart().Multiplied(aMat);
  aTranslation.Reverse();

  GeomToStep_MakeCartesianPoint aMkPoint(aTranslation, theLocalFactors.LengthFactor());
  GeomToStep_MakeDirection      aMkDir1(anInvMat.Row(1));
  GeomToStep_MakeDirection      aMkDir2(anInvMat.Row(2));
  GeomToStep_MakeDirection      aMkDir3(anInvMat.Row(3));

  myTrsfOp = new StepGeom_CartesianTransformationOperator3d();
  myTrsfOp->SetName(new TCollection_HAsciiString(""));
  myTrsfOp->SetAxis1(aMkDir1.Value());
  myTrsfOp->SetAxis2(aMkDir2.Value());
  myTrsfOp->SetAxis3(aMkDir3.Value());
  myTrsfOp->SetLocalOrigin(aMkPoint.Value());
  myTrsfOp->SetScale(aScale);

  done = Standard_True;
}
