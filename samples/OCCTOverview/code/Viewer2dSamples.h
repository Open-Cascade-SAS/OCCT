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

#ifndef VIEWER2DSAMPLES_H
#define VIEWER2DSAMPLES_H

#include "BaseSample.h"

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>

//! Implements viewer 2D samples.
class Viewer2dSamples : public BaseSample
{
  DEFINE_STANDARD_RTTI_INLINE(Viewer2dSamples, BaseSample)
public:

  Viewer2dSamples(const TCollection_AsciiString& theSampleSourcePath,
                  const Handle(V3d_View)& theView,
                  const Handle(V3d_Viewer)& theViewer,
                  const Handle(AIS_InteractiveContext)& theContext)
  : BaseSample (theSampleSourcePath, theContext),
    myView (theView),
    myViewer (theViewer)
  {}

  void SetFileName (const TCollection_AsciiString& theFileName) { myFileName = theFileName; }
  void ClearExtra();

  static Standard_Boolean IsFileSample  (const TCollection_AsciiString& theSampleName) { return theSampleName == "BackgroungImage2dSample"; }
  static Standard_Boolean IsShadedSample(const TCollection_AsciiString& theSampleName) { return theSampleName == "BackgroungImage2dSample"; }

protected:
  virtual void ExecuteSample (const TCollection_AsciiString& theSampleName) Standard_OVERRIDE;

private:

  // One function for every sample
  void TextView2dSample();
  void MarkerView2dSample();
  void FillAreaView2dSample();
  void LoopOnFaceView2dSample();
  void RectagularLineGrid2dSample();
  void RectagularPointGrid2dSample();
  void CircularLineGrid2dSample();
  void CircularPointGrid2dSample();
  void ClearGrid2dSample();
  void BackgroungImage2dSample();

private:

  TCollection_AsciiString myFileName;
  Handle(V3d_View)        myView;
  Handle(V3d_Viewer)      myViewer;

};

#endif // VIEWER2DSAMPLES_H
