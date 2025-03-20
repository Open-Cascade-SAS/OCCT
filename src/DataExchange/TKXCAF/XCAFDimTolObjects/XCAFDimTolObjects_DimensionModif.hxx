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

#ifndef _XCAFDimTolObjects_DimensionModif_HeaderFile
#define _XCAFDimTolObjects_DimensionModif_HeaderFile

//! Defines modifirs
enum XCAFDimTolObjects_DimensionModif
{
  XCAFDimTolObjects_DimensionModif_ControlledRadius,
  XCAFDimTolObjects_DimensionModif_Square,
  XCAFDimTolObjects_DimensionModif_StatisticalTolerance,
  XCAFDimTolObjects_DimensionModif_ContinuousFeature,
  XCAFDimTolObjects_DimensionModif_TwoPointSize,
  XCAFDimTolObjects_DimensionModif_LocalSizeDefinedBySphere,
  XCAFDimTolObjects_DimensionModif_LeastSquaresAssociationCriterion,
  XCAFDimTolObjects_DimensionModif_MaximumInscribedAssociation,
  XCAFDimTolObjects_DimensionModif_MinimumCircumscribedAssociation,
  XCAFDimTolObjects_DimensionModif_CircumferenceDiameter,
  XCAFDimTolObjects_DimensionModif_AreaDiameter,
  XCAFDimTolObjects_DimensionModif_VolumeDiameter,
  XCAFDimTolObjects_DimensionModif_MaximumSize,
  XCAFDimTolObjects_DimensionModif_MinimumSize,
  XCAFDimTolObjects_DimensionModif_AverageSize,
  XCAFDimTolObjects_DimensionModif_MedianSize,
  XCAFDimTolObjects_DimensionModif_MidRangeSize,
  XCAFDimTolObjects_DimensionModif_RangeOfSizes,
  XCAFDimTolObjects_DimensionModif_AnyRestrictedPortionOfFeature,
  XCAFDimTolObjects_DimensionModif_AnyCrossSection,
  XCAFDimTolObjects_DimensionModif_SpecificFixedCrossSection,
  XCAFDimTolObjects_DimensionModif_CommonTolerance,
  XCAFDimTolObjects_DimensionModif_FreeStateCondition,
  XCAFDimTolObjects_DimensionModif_Between
};

#endif // _XCAFDimTolObjects_DimensionModif_HeaderFile
