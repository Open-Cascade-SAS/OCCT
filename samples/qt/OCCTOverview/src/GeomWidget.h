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

#ifndef GEOMWIDGET_H
#define GEOMWIDGET_H

#include "View.h"
#include "DocumentCommon.h"

#include <Standard_WarningsDisable.hxx>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

class ApplicationCommon;
class QStackedWidget;

//! Qt widget for organize 3D & 2D documents
class GeomWidget : public QWidget
{
  Q_OBJECT
public:
  GeomWidget(DocumentCommon* theDocument3d,
             DocumentCommon* theDocument2d,
             QWidget* theParent = nullptr);

  void FitAll();

  Handle(V3d_View) Get3dView() { return myView3d->getView(); }

  Handle(V3d_View) Get2dView() { return myView2d->getView(); }

  void Show3d();
  void Show2d();

private:
  View*    myView3d;
  View*    myView2d;

  QWidget* my3dVidget;
  QWidget* my2dVidget;
  QStackedWidget* myStackWidget;

  DocumentCommon* myDocument3d;
  DocumentCommon* myDocument2d;
};

#endif //GEOMWIDGET_H
