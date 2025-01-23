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

#include <DESTEP_Parameters.hxx>

//=================================================================================================

DESTEP_Parameters::DESTEP_Parameters() {}

//=================================================================================================

void DESTEP_Parameters::InitFromStatic()
{
  ReadBSplineContinuity = (DESTEP_Parameters::ReadMode_BSplineContinuity)Interface_Static::IVal(
    "read.iges.bspline.continuity");
  ReadPrecisionMode =
    (DESTEP_Parameters::ReadMode_Precision)Interface_Static::IVal("read.precision.mode");
  ReadPrecisionVal = Interface_Static::RVal("read.precision.val");
  ReadMaxPrecisionMode =
    (DESTEP_Parameters::ReadMode_MaxPrecision)Interface_Static::IVal("read.maxprecision.mode");
  ReadMaxPrecisionVal = Interface_Static::RVal("read.maxprecision.val");
  ReadSameParamMode   = Interface_Static::IVal("read.stdsameparameter.mode") == 1;
  ReadSurfaceCurveMode =
    (DESTEP_Parameters::ReadMode_SurfaceCurve)Interface_Static::IVal("read.surfacecurve.mode");
  EncodeRegAngle = Interface_Static::RVal("read.encoderegularity.angle") * 180.0 / M_PI;
  AngleUnit      = (DESTEP_Parameters::AngleUnitMode)Interface_Static::IVal("step.angleunit.mode");

  ReadProductMode = Interface_Static::IVal("read.step.product.mode") == 1;
  ReadProductContext =
    (DESTEP_Parameters::ReadMode_ProductContext)Interface_Static::IVal("read.step.product.context");
  ReadShapeRepr =
    (DESTEP_Parameters::ReadMode_ShapeRepr)Interface_Static::IVal("read.step.shape.repr");
  ReadTessellated =
    (DESTEP_Parameters::RWMode_Tessellated)Interface_Static::IVal("read.step.tessellated");
  ReadAssemblyLevel =
    (DESTEP_Parameters::ReadMode_AssemblyLevel)Interface_Static::IVal("read.step.assembly.level");
  ReadRelationship       = Interface_Static::IVal("read.step.shape.relationship") == 1;
  ReadShapeAspect        = Interface_Static::IVal("read.step.shape.aspect") == 1;
  ReadConstrRelation     = Interface_Static::IVal("read.step.constructivegeom.relationship") == 1;
  ReadSubshapeNames      = Interface_Static::IVal("read.stepcaf.subshapes.name") == 1;
  ReadCodePage           = (Resource_FormatType)Interface_Static::IVal("read.step.codepage");
  ReadNonmanifold        = Interface_Static::IVal("read.step.nonmanifold") == 1;
  ReadIdeas              = Interface_Static::IVal("read.step.ideas") == 1;
  ReadAllShapes          = Interface_Static::IVal("read.step.all.shapes") == 1;
  ReadRootTransformation = Interface_Static::IVal("read.step.root.transformation") == 1;
  ReadColor              = Interface_Static::IVal("read.color") == 1;
  ReadName               = Interface_Static::IVal("read.name") == 1;
  ReadLayer              = Interface_Static::IVal("read.layer") == 1;
  ReadProps              = Interface_Static::IVal("read.props") == 1;
  ReadMetadata           = Interface_Static::IVal("read.metadata") == 1;

  WritePrecisionMode =
    (DESTEP_Parameters::WriteMode_PrecisionMode)Interface_Static::IVal("write.precision.mode");
  WritePrecisionVal = Interface_Static::RVal("write.precision.val");
  WriteAssembly =
    (DESTEP_Parameters::WriteMode_Assembly)Interface_Static::IVal("write.step.assembly");
  WriteSchema =
    (DESTEP_Parameters::WriteMode_StepSchema)Interface_Static::IVal("write.step.schema");
  WriteTessellated =
    (DESTEP_Parameters::RWMode_Tessellated)Interface_Static::IVal("write.step.tessellated");
  WriteProductName    = Interface_Static::CVal("write.step.product.name");
  WriteSurfaceCurMode = Interface_Static::IVal("write.surfacecurve.mode") == 1;
  WriteUnit           = (UnitsMethods_LengthUnit)Interface_Static::IVal("write.step.unit");
  WriteVertexMode =
    (DESTEP_Parameters::WriteMode_VertexMode)Interface_Static::IVal("write.step.vertex.mode");
  WriteSubshapeNames = Interface_Static::IVal("write.stepcaf.subshapes.name") == 1;
  WriteColor         = Interface_Static::IVal("write.color") == 1;
  WriteNonmanifold   = Interface_Static::IVal("write.step.nonmanifold") == 1;
  WriteName          = Interface_Static::IVal("write.name") == 1;
  WriteLayer         = Interface_Static::IVal("write.layer") == 1;
  WriteProps         = Interface_Static::IVal("write.props") == 1;
  WriteModelType     = (STEPControl_StepModelType)Interface_Static::IVal("write.model.type");
}

//=================================================================================================

void DESTEP_Parameters::Reset()
{
  DESTEP_Parameters aParameters;
  *this = aParameters;
}

//=================================================================================================

DE_ShapeFixParameters DESTEP_Parameters::GetDefaultShapeFixParameters()
{
  return DE_ShapeFixParameters();
}
