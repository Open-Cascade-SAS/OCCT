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

#ifndef TOPOLOGYSAMPLES_H
#define TOPOLOGYSAMPLES_H

#include "BaseSample.h"

#include <AIS_InteractiveContext.hxx>

//! Implements Topology samples
class TopologySamples : public BaseSample
{
  DEFINE_STANDARD_RTTI_INLINE(TopologySamples, BaseSample)
public:

  TopologySamples (const TCollection_AsciiString& theSampleSourcePath,
                   const Handle(AIS_InteractiveContext)& theContext)
  : BaseSample(theSampleSourcePath, theContext)
  {}

protected:
  virtual void ExecuteSample (const TCollection_AsciiString& theSampleName) Standard_OVERRIDE;

private:
  // One function for every sample
  void Vertex3dSample();
  void Edge3dSample();
  void Face3dSample();
  void Wire3dSample();
  void Shell3dSample();
  void Solid3dSample();
  void Edge2dSample();
  void Box3dSample();
  void Cylinder3dSample();
  void Revolution3dSample();
  void TopologyIterator3dSample();
  void TopologyExplorer3dSample();
  void AssessToCurve3dSample();
  void AssessToCompositeCurve3dSample();
  void AssessToSurface3dSample();
  void Common3dSample();
  void Cut3dSample();
  void Fuse3dSample();
  void Section3dSample();
  void Splitter3dSample();
  void Defeaturing3dSample();
  void Fillet3dSample();
  void Chamfer3dSample();
  void Offset3dSample();
  void Evolved3dSample();
  void Copy3dSample();
  void Transform3dSample();
  void ConvertToNurbs3dSample();
  void SewContiguousFaces3dSample();
  void CheckValidity3dSample();
  void ComputeLinearProperties3dSample();
  void ComputeSurfaceProperties3dSample();
  void ComputeVolumeProperties3dSample();
};

#endif  //TOPOLOGYSAMPLES_H
