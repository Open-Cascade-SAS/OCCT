// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#ifndef GEOMETRYSAMPLES_H
#define GEOMETRYSAMPLES_H

#include "BaseSample.h"

//! Implements Geometry samples
class GeometrySamples : public BaseSample
{
  DEFINE_STANDARD_RTTI_INLINE(GeometrySamples, BaseSample)
public:

  GeometrySamples (const TCollection_AsciiString& theSampleSourcePath,
                   const Handle(AIS_InteractiveContext)& theContext)
  : BaseSample(theSampleSourcePath, theContext) {}

protected:
  virtual void ExecuteSample (const TCollection_AsciiString& theSampleName) Standard_OVERRIDE;

private:
  void DisplayPnt (const gp_Pnt& thePnt, const TCollection_AsciiString& theText,
                   Aspect_TypeOfMarker theMarker = Aspect_TOM_PLUS,
                   Standard_Real theDistance = 5.0);
  void DisplayPnt (const gp_Pnt2d& thePnt2d, const TCollection_AsciiString& theText,
                   Aspect_TypeOfMarker theMarker = Aspect_TOM_PLUS,
                   Standard_Real theDistance = 5.0);

  // One function for every sample
  void ZeroDimensionObjects3dSample();
  void Vectors3dSample();
  void InfinityLines3dSample();
  void SecondOrderCurves3dSample();
  void PlaneSurfaces3dSample();
  void SecondOrderSurfaces3dSample();
  void ZeroDimensionObjects2dSample();
  void Vectors2dSample();
  void InfinityLines2dSample();
  void SecondOrderCurves2dSample();
  void BarycenterPoint3dSample();
  void RotatedVector3dSample();
  void MirroredLine3dSample();
  void ScaledEllipse3dSample();
  void TransformedCylinder3dSample();
  void TranslatedTorus3dSample();
  void ConjugateObjects3dSample();
  void ProjectionOfPoint3dSample();
  void MinimalDistance3dSample();
  void Intersection3dSample();
  void TranslatedPoint2dSample();
  void RotatedDirection2dSample();
  void MirroredAxis2dSample();
  void TransformedEllipse2dSample();
  void ConjugateObjects2dSample();
  void Tangent2dSample();
  void ProjectionOfPoint2dSample();
  void MinimalDistance2dSample();
  void Intersection2dSample();
  void PointInfo3dSample();
  void EllipseInfo3dSample();
  void PointInfo2dSample();
  void CircleInfo2dSample();
  void FreeStyleCurves3dSample();
  void AnalyticalSurfaces3dSample();
  void FreeStyleSurfaces3dSample();
  void FreeStyleCurves2dSample();
  void TrimmedCurve3dSample();
  void OffsetCurve3dSample();
  void BSplineFromCircle3dSample();
  void TrimmedSurface3dSample();
  void OffsetSurface3dSample();
  void ExtrusionSurface3dSample();
  void RevolutionSurface3dSample();
  void TrimmedCurve2dSample();
  void OffsetCurve2dSample();
  void BoundingBoxOfSurface3dSample();
  void BoundingBoxOfCurves3dSample();
  void BoundingBoxOfCurves2dSample();
  void DumpCircleInfoSample();
  void DumpBSplineCurveInfoSample();
};

#endif  //GEOMETRYSAMPLES_H
