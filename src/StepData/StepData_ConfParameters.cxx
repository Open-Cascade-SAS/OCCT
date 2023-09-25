// Copyright (c) 2023 OPEN CASCADE SAS
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

#include <Interface_Static.hxx>

#include <StepData_ConfParameters.hxx>

//=======================================================================
// function : StepData_ConfParameters
// purpose  :
//=======================================================================
StepData_ConfParameters::StepData_ConfParameters()
{}

//=======================================================================
// function : InitParameters
// purpose  :
//=======================================================================
void StepData_ConfParameters::InitFromStatic()
{
  ReadBSplineContinuity = (StepData_ConfParameters::ReadMode_BSplineContinuity)Interface_Static::IVal("read.iges.bspline.continuity");
  ReadPrecisionMode = (StepData_ConfParameters::ReadMode_Precision)Interface_Static::IVal("read.precision.mode");
  ReadPrecisionVal = Interface_Static::RVal("read.precision.val");
  ReadMaxPrecisionMode = (StepData_ConfParameters::ReadMode_MaxPrecision)Interface_Static::IVal("read.maxprecision.mode");
  ReadMaxPrecisionVal = Interface_Static::RVal("read.maxprecision.val");
  ReadSameParamMode = Interface_Static::IVal("read.stdsameparameter.mode") == 1;
  ReadSurfaceCurveMode = (StepData_ConfParameters::ReadMode_SurfaceCurve)Interface_Static::IVal("read.surfacecurve.mode");
  EncodeRegAngle = Interface_Static::RVal("read.encoderegularity.angle") * 180.0 / M_PI;
  AngleUnit = (StepData_ConfParameters::AngleUnitMode)Interface_Static::IVal("step.angleunit.mode");

  ReadResourceName = Interface_Static::CVal("read.step.resource.name");
  ReadSequence = Interface_Static::CVal("read.step.sequence");
  ReadProductMode = Interface_Static::IVal("read.step.product.mode") == 1;
  ReadProductContext = (StepData_ConfParameters::ReadMode_ProductContext)Interface_Static::IVal("read.step.product.context");
  ReadShapeRepr = (StepData_ConfParameters::ReadMode_ShapeRepr)Interface_Static::IVal("read.step.shape.repr");
  ReadTessellated = (StepData_ConfParameters::RWMode_Tessellated)Interface_Static::IVal("read.step.tessellated");
  ReadAssemblyLevel = (StepData_ConfParameters::ReadMode_AssemblyLevel)Interface_Static::IVal("read.step.assembly.level");
  ReadRelationship = Interface_Static::IVal("read.step.shape.relationship") == 1;
  ReadShapeAspect = Interface_Static::IVal("read.step.shape.aspect") == 1;
  ReadConstrRelation = Interface_Static::IVal("read.step.constructivegeom.relationship") == 1;
  ReadSubshapeNames = Interface_Static::IVal("read.stepcaf.subshapes.name") == 1;
  ReadCodePage = (Resource_FormatType)Interface_Static::IVal("read.step.codepage");
  ReadNonmanifold = Interface_Static::IVal("read.step.nonmanifold") == 1;
  ReadIdeas = Interface_Static::IVal("read.step.ideas") == 1;
  ReadAllShapes = Interface_Static::IVal("read.step.all.shapes") == 1;
  ReadRootTransformation = Interface_Static::IVal("read.step.root.transformation") == 1;
  ReadColor = Interface_Static::IVal("read.color") == 1;
  ReadName = Interface_Static::IVal("read.name") == 1;
  ReadLayer = Interface_Static::IVal("read.layer") == 1;
  ReadProps = Interface_Static::IVal("read.props") == 1;

  WritePrecisionMode = (StepData_ConfParameters::WriteMode_PrecisionMode)Interface_Static::IVal("write.precision.mode");
  WritePrecisionVal = Interface_Static::RVal("write.precision.val");
  WriteAssembly = (StepData_ConfParameters::WriteMode_Assembly)Interface_Static::IVal("write.step.assembly");
  WriteSchema = (StepData_ConfParameters::WriteMode_StepSchema)Interface_Static::IVal("write.step.schema");
  WriteTessellated = (StepData_ConfParameters::RWMode_Tessellated)Interface_Static::IVal("write.step.tessellated");
  WriteProductName = Interface_Static::CVal("write.step.product.name");
  WriteSurfaceCurMode = Interface_Static::IVal("write.surfacecurve.mode") == 1;
  WriteUnit = (UnitsMethods_LengthUnit)Interface_Static::IVal("write.step.unit");
  WriteResourceName = Interface_Static::CVal("write.step.resource.name");
  WriteSequence = Interface_Static::CVal("write.step.sequence");
  WriteVertexMode = (StepData_ConfParameters::WriteMode_VertexMode)Interface_Static::IVal("write.step.vertex.mode");
  WriteSubshapeNames = Interface_Static::IVal("write.stepcaf.subshapes.name") == 1;
  WriteColor = Interface_Static::IVal("write.color") == 1;
  WriteNonmanifold = Interface_Static::IVal("write.step.nonmanifold") == 1;
  WriteName = Interface_Static::IVal("write.name") == 1;
  WriteLayer = Interface_Static::IVal("write.layer") == 1;
  WriteProps = Interface_Static::IVal("write.props") == 1;
  WriteModelType = (STEPControl_StepModelType)Interface_Static::IVal("write.model.type");
}

//=======================================================================
// function : ResetParameters
// purpose  :
//=======================================================================
void StepData_ConfParameters::Reset()
{
  StepData_ConfParameters aParameters;
  *this = aParameters;
}
