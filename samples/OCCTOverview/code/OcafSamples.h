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

#ifndef OCAFSAMPLES_H
#define OCAFSAMPLES_H

#include "BaseSample.h"
#include "TOcaf_Application.h"

#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>
#include <TDocStd_Document.hxx>

//! Implements OCAF samples
class OcafSamples : public BaseSample
{
  DEFINE_STANDARD_RTTI_INLINE(OcafSamples, BaseSample)
public:

  OcafSamples (const TCollection_AsciiString& theSampleSourcePath,
               const Handle(V3d_Viewer)& theViewer,
               const Handle(AIS_InteractiveContext)& theContext)
  : BaseSample (theSampleSourcePath, theContext),
    myViewer (theViewer)
  {
    //
  }

  virtual void Process (const TCollection_AsciiString& theSampleName) Standard_OVERRIDE;

  void ClearExtra();
  void SetFileName (const TCollection_AsciiString& theFileName) { myFileName = theFileName; };

  static Standard_Boolean IsExportSample (const TCollection_AsciiString& theSampleName);
  static Standard_Boolean IsImportSample (const TCollection_AsciiString& theSampleName);
  static Standard_Boolean IsBinarySample (const TCollection_AsciiString& theSampleName);
  static Standard_Boolean IsXmlSample    (const TCollection_AsciiString& theSampleName);

protected:
  virtual void ExecuteSample (const TCollection_AsciiString& theSampleName) Standard_OVERRIDE;

private:
  // One function for every sample
  void CreateOcafDocument();
  void CreateBoxOcafSample();
  void CreateCylinderOcafSample();
  void ModifyBoxOcafSample();
  void ModifyCylinderOcafSample();
  void UndoOcafSample();
  void RedoOcafSample();
  void DialogOpenOcafSample();
  void DialogSaveBinOcafSample();
  void DialogSaveXmlOcafSample();
  void DisplayPresentation();

private:

  TCollection_AsciiString  myFileName;
  Handle(V3d_Viewer)       myViewer;
  Handle(TDocStd_Document) myOcafDoc;
};

#endif  //OCAFSAMPLES_H
