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

#ifndef DATAEXCHANGESAMPLES_H
#define DATAEXCHANGESAMPLES_H

#include "BaseSample.h"

#include <STEPControl_StepModelType.hxx>
#include <V3d_View.hxx>

//! Implements Data Exchange samples
class DataExchangeSamples : public BaseSample
{
  DEFINE_STANDARD_RTTI_INLINE(DataExchangeSamples, BaseSample)
public:

  DataExchangeSamples (const TCollection_AsciiString& theSampleSourcePath,
                       const Handle(V3d_View)& theView,
                       const Handle(AIS_InteractiveContext)& theContext)
  : BaseSample (theSampleSourcePath, theContext),
    myStepType (STEPControl_AsIs),
    myView (theView)
  {
    //
  }

  virtual void Process (const TCollection_AsciiString& theSampleName) Standard_OVERRIDE;

  void AppendBottle();
  void SetFileName(TCollection_AsciiString theFileName) { myFileName = theFileName; };
  void SetStepType(STEPControl_StepModelType theStepType) { myStepType = theStepType; };

  static Standard_Boolean IsExportSample(const TCollection_AsciiString& theSampleName) { return theSampleName == "BrepExportSample"
                                                                                             || theSampleName == "StepExportSample"
                                                                                             || theSampleName == "IgesExportSample"
                                                                                             || theSampleName == "StlExportSample"
                                                                                             || theSampleName == "VrmlExportSample"
                                                                                             || theSampleName == "ImageExportSample"; }
  static Standard_Boolean IsImportSample(const TCollection_AsciiString& theSampleName) { return theSampleName == "BrepImportSample"
                                                                                             || theSampleName == "StepImportSample"
                                                                                             || theSampleName == "IgesImportSample"; }
  static Standard_Boolean IsBrepSample  (const TCollection_AsciiString& theSampleName) { return theSampleName == "BrepExportSample" || theSampleName == "BrepImportSample"; }
  static Standard_Boolean IsStepSample  (const TCollection_AsciiString& theSampleName) { return theSampleName == "StepExportSample" || theSampleName == "StepImportSample"; }
  static Standard_Boolean IsIgesSample  (const TCollection_AsciiString& theSampleName) { return theSampleName == "IgesExportSample" || theSampleName == "IgesImportSample"; }
  static Standard_Boolean IsStlSample   (const TCollection_AsciiString& theSampleName) { return theSampleName == "StlExportSample"; }
  static Standard_Boolean IsVrmlSample  (const TCollection_AsciiString& theSampleName) { return theSampleName == "VrmlExportSample"; }
  static Standard_Boolean IsImageSample (const TCollection_AsciiString& theSampleName) { return theSampleName == "ImageExportSample"; }

protected:
  virtual void ExecuteSample (const TCollection_AsciiString& theSampleName) Standard_OVERRIDE;

private:
  TCollection_AsciiString   myFileName;
  STEPControl_StepModelType myStepType;
  Handle(V3d_View)          myView;

private:
  Standard_Boolean CheckFacetedBrep();
  // One function for every sample
  void BrepExportSample();
  void StepExportSample();
  void IgesExportSample();
  void StlExportSample();
  void VrmlExportSample();
  void ImageExportSample();
  void BrepImportSample();
  void StepImportSample();
  void IgesImportSample();
};

#endif  //DATAEXCHANGESAMPLES_H
