// Created on: 2015-08-06
// Created by: Ilya Novikov
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDimTolObjects_GeomToleranceType_HeaderFile
#define _XCAFDimTolObjects_GeomToleranceType_HeaderFile

//! Defines types of geom tolerance
enum XCAFDimTolObjects_GeomToleranceType
{
  XCAFDimTolObjects_GeomToleranceType_None,
  XCAFDimTolObjects_GeomToleranceType_Angularity,
  XCAFDimTolObjects_GeomToleranceType_CircularRunout,
  XCAFDimTolObjects_GeomToleranceType_CircularityOrRoundness,
  XCAFDimTolObjects_GeomToleranceType_Coaxiality,
  XCAFDimTolObjects_GeomToleranceType_Concentricity,
  XCAFDimTolObjects_GeomToleranceType_Cylindricity,
  XCAFDimTolObjects_GeomToleranceType_Flatness,
  XCAFDimTolObjects_GeomToleranceType_Parallelism,
  XCAFDimTolObjects_GeomToleranceType_Perpendicularity,
  XCAFDimTolObjects_GeomToleranceType_Position,
  XCAFDimTolObjects_GeomToleranceType_ProfileOfLine,
  XCAFDimTolObjects_GeomToleranceType_ProfileOfSurface,
  XCAFDimTolObjects_GeomToleranceType_Straightness,
  XCAFDimTolObjects_GeomToleranceType_Symmetry,
  XCAFDimTolObjects_GeomToleranceType_TotalRunout
};

#endif // _XCAFDimTolObjects_GeomToleranceType_HeaderFile
