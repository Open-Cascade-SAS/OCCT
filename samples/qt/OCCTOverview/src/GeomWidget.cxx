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

#include "GeomWidget.h"

#include <Standard_WarningsDisable.hxx>
#include <QFrame>
#include <QBoxLayout>
#include <QTextEdit>
#include <QStackedLayout>
#include <QToolBar>
#include <QStackedWidget>
#include <Standard_WarningsRestore.hxx>

GeomWidget::GeomWidget (DocumentCommon* theDocument3d,
                        DocumentCommon* theDocument2d,
                        QWidget* theParent)
: QWidget (theParent),
  myDocument3d(theDocument3d),
  myDocument2d(theDocument2d)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  aMainLayout->setContentsMargins(0, 0, 0, 0);

  my2dVidget = new QWidget;
  QVBoxLayout* a2dLayout = new QVBoxLayout(my2dVidget);
  a2dLayout->setContentsMargins(0, 0, 0, 0);
  a2dLayout->setSpacing(0);
  myView2d = new View(myDocument2d->getContext(), false, my2dVidget);
  QToolBar* aToolBar2d = new QToolBar;
  aToolBar2d->addActions(myView2d->getViewActions());
  a2dLayout->addWidget(aToolBar2d);
  a2dLayout->addWidget(myView2d);

  my3dVidget = new QWidget;
  QVBoxLayout* a3dLayout = new QVBoxLayout(my3dVidget);
  a3dLayout->setContentsMargins(0, 0, 0, 0);
  a3dLayout->setSpacing(0);
  myView3d = new View(myDocument3d->getContext(), true, my3dVidget);
  QToolBar* aToolBar3d = new QToolBar;
  aToolBar3d->addActions(myView3d->getViewActions());
  aToolBar3d->addSeparator();
  aToolBar3d->addActions(myView3d->getRaytraceActions());
  a3dLayout->addWidget(aToolBar3d);
  a3dLayout->addWidget(myView3d);

  myStackWidget = new QStackedWidget(this);
  aMainLayout->addWidget(myStackWidget);
  myStackWidget->addWidget(my2dVidget);
  myStackWidget->addWidget(my3dVidget);

  FitAll();
}

void GeomWidget::FitAll()
{
  if (myDocument2d->IsEmpty())
    Show3d();
  else
    Show2d();
}

void GeomWidget::Show3d()
{
  myView3d->axo();
  myView3d->fitAll();
  QAction* aShadingAction = myView3d->getViewAction(ViewAction_Shading);
  aShadingAction->trigger();
  aShadingAction->setChecked(true);
  QAction* aHlrOffAction = myView3d->getViewAction(ViewAction_HlrOff);
  aHlrOffAction->trigger();
  aHlrOffAction->setChecked(true);
  myStackWidget->setCurrentWidget(my3dVidget);
  setStatusTip("Mouse buttons: Right-Zoom, Middle-Pan, Left-Rotate");
}

void GeomWidget::Show2d()
{
  myView2d->fitAll();
  myStackWidget->setCurrentWidget(my2dVidget);
  setStatusTip("Mouse buttons: Right-Zoom, Middle-Pan");
}
