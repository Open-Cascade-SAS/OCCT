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

#ifndef _XCAFDimTolObjects_DimensionType_HeaderFile
#define _XCAFDimTolObjects_DimensionType_HeaderFile

//! Defines types of dimension
enum XCAFDimTolObjects_DimensionType
{
  XCAFDimTolObjects_DimensionType_Location_None,
  XCAFDimTolObjects_DimensionType_Location_CurvedDistance,
  XCAFDimTolObjects_DimensionType_Location_LinearDistance,
  XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToOuter,
  XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToInner,
  XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToCenter,
  XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToOuter,
  XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToInner,
  XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToCenter,
  XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToOuter,
  XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToInner,
  XCAFDimTolObjects_DimensionType_Location_Angular,
  XCAFDimTolObjects_DimensionType_Location_Oriented,
  XCAFDimTolObjects_DimensionType_Location_WithPath,
  XCAFDimTolObjects_DimensionType_Size_CurveLength,
  XCAFDimTolObjects_DimensionType_Size_Diameter,
  XCAFDimTolObjects_DimensionType_Size_SphericalDiameter,
  XCAFDimTolObjects_DimensionType_Size_Radius,
  XCAFDimTolObjects_DimensionType_Size_SphericalRadius,
  XCAFDimTolObjects_DimensionType_Size_ToroidalMinorDiameter,
  XCAFDimTolObjects_DimensionType_Size_ToroidalMajorDiameter,
  XCAFDimTolObjects_DimensionType_Size_ToroidalMinorRadius,
  XCAFDimTolObjects_DimensionType_Size_ToroidalMajorRadius,
  XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorDiameter,
  XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorDiameter,
  XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorRadius,
  XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorRadius,
  XCAFDimTolObjects_DimensionType_Size_Thickness,
  XCAFDimTolObjects_DimensionType_Size_Angular,
  XCAFDimTolObjects_DimensionType_Size_WithPath,
  XCAFDimTolObjects_DimensionType_CommonLabel,
  XCAFDimTolObjects_DimensionType_DimensionPresentation
};

#endif // _XCAFDimTolObjects_DimensionType_HeaderFile
