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

#ifndef DOCUMENT_COMMON_OVERVIEW_H
#define DOCUMENT_COMMON_OVERVIEW_H

#include "CommonSample.h"

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QList>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>

class ApplicationCommonWindow;

//! Implements visualization of samples content
class DocumentCommon : public QObject
{
  Q_OBJECT
public:

  DocumentCommon(ApplicationCommonWindow* );
  ~DocumentCommon() { }

  const Handle(AIS_InteractiveContext)& getContext() { return myContext; }

  const Handle(V3d_Viewer)& getViewer() { return myViewer; }

  void setViewer (const Handle(V3d_Viewer)& theViewer) { myViewer = theViewer; }

  void SetObjects(const NCollection_Vector<Handle(AIS_InteractiveObject)>& theObjects,
                  Standard_Boolean theDisplayShaded = Standard_False);
  void Clear();
  bool IsEmpty() const { return myContextIsEmpty; }

signals:
  void selectionChanged();
  void sendCloseDocument( DocumentCommon* );

private:
  Handle(V3d_Viewer) Viewer (const Standard_ExtString theName,
                             const Standard_CString theDomain,
                             const Standard_Real theViewSize,
                             const V3d_TypeOfOrientation theViewProj,
                             const Standard_Boolean theComputedMode,
                             const Standard_Boolean theDefaultComputedMode );

private:
  Handle(V3d_Viewer)             myViewer;
  Handle(AIS_InteractiveContext) myContext;
  bool myContextIsEmpty;
};

#endif
