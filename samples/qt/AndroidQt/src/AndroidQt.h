// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef ANDROIDQT_H
#define ANDROIDQT_H

#include <OpenGl_Context.hxx>

// workaround broken definitions in Qt
#define GLdouble GLdouble

#include <QtQuick/qquickwindow.h>
#include <QtQuick/QQuickItem>

#undef GLdouble

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>

#include <QMutex>

#include <AndroidQt_TouchParameters.h>

//! QML item with embedded OCCT viewer.
class AndroidQt : public QQuickItem
{
  Q_OBJECT

public:
  //! Default constructor.
  AndroidQt();

  //! Display shape from file.
  Q_INVOKABLE bool ReadShapeFromFile (QString theFilePath);

  //! Handle touch event.
  Q_INVOKABLE void InitTouch   (const double theX,
                                const double theY);
  
  //! Handle touch event.
  Q_INVOKABLE void UpdateTouch (const double theX,
                                const double theY);

public slots:

  //! Handle OpenGL context creation and window resize events.
  void sync();
  
  //! Redraw OCCT viewer and handle pending viewer events in rendering thread.
  void paint();

private slots:

  //! Handle window change event.
  void handleWindowChanged (QQuickWindow* theWin);

private:

  //! (Re-)initialize viewer on OpenGL context change.
  bool initViewer();

  //! Close viewer
  void release();

private:

  Handle(V3d_Viewer)             myViewer;       //!< 3D viewer
  Handle(V3d_View)               myView;         //!< 3D view
  Handle(AIS_InteractiveContext) myContext;      //!< interactive context

  QMutex                         myMutex;        //!< mutex for interconnection with rendering thread
  QSize                          myViewportSize; //!< QML item size
  AndroidQt_TouchParameters      myTouchPoint;   //!< cached state of touch event
  bool                           myFitAllAction; //!< queued viewer FitALL event

};

#endif // ANDROIDQT_H
