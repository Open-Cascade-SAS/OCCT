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

#include <DEIGES_Parameters.hxx>

//=================================================================================================

void DEIGES_Parameters::InitFromStatic()
{
  ReadBSplineContinuity =
    (ReadMode_BSplineContinuity)Interface_Static::IVal("read.iges.bspline.continuity");
  ReadPrecisionMode    = (ReadMode_Precision)Interface_Static::IVal("read.precision.mode");
  ReadPrecisionVal     = Interface_Static::RVal("read.precision.val");
  ReadMaxPrecisionMode = (ReadMode_MaxPrecision)Interface_Static::IVal("read.maxprecision.mode");
  ReadMaxPrecisionVal  = Interface_Static::RVal("read.maxprecision.val");
  ReadSameParamMode    = Interface_Static::IVal("read.stdsameparameter.mode") == 1;
  ReadSurfaceCurveMode = (ReadMode_SurfaceCurve)Interface_Static::IVal("read.surfacecurve.mode");
  EncodeRegAngle       = Interface_Static::RVal("read.encoderegularity.angle");

  ReadApproxd1       = Interface_Static::IVal("read.bspline.approxd1.mode") == 1;
  ReadFaultyEntities = Interface_Static::IVal("read.fau_lty.entities") == 1;
  ReadOnlyVisible    = Interface_Static::IVal("read.onlyvisible") == 1;
  ReadColor          = Interface_Static::IVal("read.color") == 1;
  ReadName           = Interface_Static::IVal("read.name") == 1;
  ReadLayer          = Interface_Static::IVal("read.layer") == 1;

  WriteBRepMode = (WriteMode_BRep)Interface_Static::IVal("write.brep.mode");
  WriteConvertSurfaceMode =
    (WriteMode_ConvertSurface)Interface_Static::IVal("write.convertsurface.mode");
  WriteHeaderAuthor   = Interface_Static::CVal("write.header.author");
  WriteHeaderCompany  = Interface_Static::CVal("write.header.company");
  WriteHeaderProduct  = Interface_Static::CVal("write.header.product");
  WriteHeaderReciever = Interface_Static::CVal("write.header.receiver");
  WritePrecisionMode  = (WriteMode_PrecisionMode)Interface_Static::IVal("write.precision.mode");
  WritePrecisionVal   = Interface_Static::RVal("write.precision.val");
  WritePlaneMode      = (WriteMode_PlaneMode)Interface_Static::IVal("write.plane.mode");
  WriteOffsetMode     = Interface_Static::IVal("write.offset") == 1;
  WriteColor          = Interface_Static::IVal("write.color") == 1;
  WriteName           = Interface_Static::IVal("write.name") == 1;
  WriteLayer          = Interface_Static::IVal("write.layer") == 1;
}

//=================================================================================================

void DEIGES_Parameters::Reset()
{
  *this = DEIGES_Parameters();
}

//=================================================================================================

DE_ShapeFixParameters DEIGES_Parameters::GetDefaultShapeFixParameters()
{
  DE_ShapeFixParameters aShapeFixParameters;
  aShapeFixParameters.DetalizationLevel   = TopAbs_EDGE;
  aShapeFixParameters.CreateOpenSolidMode = DE_ShapeFixParameters::FixMode::Fix;
  aShapeFixParameters.FixTailMode         = DE_ShapeFixParameters::FixMode::FixOrNot;
  aShapeFixParameters.MaxTailAngle        = DE_ShapeFixParameters::FixMode::FixOrNot;
  return aShapeFixParameters;
}
