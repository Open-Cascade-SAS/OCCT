// Copyright (c) 2024 OPEN CASCADE SAS
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

#include <DE_ShapeFixParameters.hxx>

//=================================================================================================

DE_ShapeFixParameters DE_ShapeFixParameters::GetDefaultReadingParamsIGES()
{
  DE_ShapeFixParameters aShapeFixParameters;
  aShapeFixParameters.DetalizationLevel   = TopAbs_EDGE;
  aShapeFixParameters.CreateOpenSolidMode = DE_ShapeFixParameters::FixMode::Fix;
  aShapeFixParameters.FixTailMode         = DE_ShapeFixParameters::FixMode::FixOrNot;
  aShapeFixParameters.MaxTailAngle        = DE_ShapeFixParameters::FixMode::FixOrNot;
  return aShapeFixParameters;
}

//=================================================================================================

DE_ShapeFixParameters DE_ShapeFixParameters::GetDefaultWritingParamsIGES()
{
  DE_ShapeFixParameters aShapeFixParameters;
  aShapeFixParameters.DetalizationLevel   = TopAbs_EDGE;
  aShapeFixParameters.CreateOpenSolidMode = DE_ShapeFixParameters::FixMode::Fix;
  aShapeFixParameters.FixTailMode         = DE_ShapeFixParameters::FixMode::FixOrNot;
  aShapeFixParameters.MaxTailAngle        = DE_ShapeFixParameters::FixMode::FixOrNot;
  return aShapeFixParameters;
}

//=================================================================================================

DE_ShapeFixParameters DE_ShapeFixParameters::GetDefaultReadingParamsSTEP()
{
  return DE_ShapeFixParameters();
}

//=================================================================================================

DE_ShapeFixParameters DE_ShapeFixParameters::GetDefaultWritingParamsSTEP()
{
  return DE_ShapeFixParameters();
}
