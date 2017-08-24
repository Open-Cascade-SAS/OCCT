// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef View_View_H
#define View_View_H

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <inspector/View_ViewActionType.hxx>
#include <inspector/View_Viewer.hxx>

#ifdef _MSC_VER
  #pragma warning(disable : 4127 4718) // conditional expression is constant, recursive call has no side effects
#endif
#include <QAction>
#include <QMap>
#include <QString>
#include <QWidget>

class View_Viewer;

class QRubberBand;

//! \class View_Widget
//! \brief It is a Qt control that visualizes content of OCCT 3D view
//! It creates control and actions of manipulating of this view,
//! emits signal about selection happenning in the view and signal about display mode change and
//! changes application cursor depending on an active action.
class View_Widget : public QWidget
{
  Q_OBJECT
protected:

  //! Enumeration defines manipulating actions of the widget
  enum View_CurrentAction3d
  {
    View_CurrentAction3d_Nothing, //!< Empty action
    View_CurrentAction3d_DynamicZooming, //!< Zoom action
    View_CurrentAction3d_WindowZooming, //!< Zoom action using rectangle
    View_CurrentAction3d_DynamicPanning, //!< Panning action
    View_CurrentAction3d_DynamicRotation //!< Rotation action
  };

  //! Enumeration defines cursor kind
  enum View_CursorMode
  {
    View_CursorMode_DefaultCursor, //!< default Qt cursor
    View_CursorMode_HandCursor, //!< hand cursor
    View_CursorMode_PanCursor, //!< panning cursor
    View_CursorMode_ZoomCursor, //!< zoom cursor
    View_CursorMode_RotationCursor //!< onRotate cursor
  };

  //! Enumeration defines drag mode
  enum View_DragMode
  {
    View_DragMode_ButtonDown, // theState == -1  button down
    View_DragMode_ButtonMove, // theState ==  0  move
    View_DragMode_ButtonUp // theState ==  1  button up
  };

public:

  //! Constructor
  Standard_EXPORT View_Widget (QWidget* theParent);

  //! Destructor
  virtual ~View_Widget() {}

  //! Returns current viewer
  View_Viewer* GetViewer() const { return myViewer; }

  //! Sets default size that is used in sizeHint when the widget is firstly show
  Standard_EXPORT void SetPredefinedSize (int theDefaultWidth, int theDefaultHeight);

  //! Creates V3d view and set Qt control for it
  Standard_EXPORT void Init();

  //! Returns an action for the given action type
  //! \param theActionId an action indes
  QAction* GetViewAction (const View_ViewActionType theActionId) const { return myViewActions[theActionId]; };

  //! \returns 0 - AIS_WireFrame, 1 - AIS_Shaded
  Standard_EXPORT int GetDisplayMode() const;

  //! Enable/disable view and tool bar actions depending on the parameter
  //! \param theIsEnabled boolean value
  Standard_EXPORT void SetEnabledView (const bool theIsEnabled);

  //! Get paint engine for the OpenGL viewer. Avoid default execution of Qt Widget.
  virtual QPaintEngine* paintEngine() const Standard_OVERRIDE { return 0; }

  //! Recommended size for view. If default size exists, it returns the default size
  Standard_EXPORT virtual QSize sizeHint() const Standard_OVERRIDE;

signals:

  //! Sends a signal about selection change if the left mouse button is pressed and current action does not process it
  void selectionChanged();

  //! Sends a signal about display mode change
  void displayModeClicked();

public slots:

  //! Fits all the V3d view and redraw view
  void OnFitAll() { myViewer->GetView()->FitAll(); }

  //! Stores state about fitting all to use it by the mouse move
  void OnFitArea() { myCurrentMode = View_CurrentAction3d_WindowZooming; }

  //! Stores state about zoom to use it by the mouse move
  void OnZoom() { myCurrentMode = View_CurrentAction3d_DynamicZooming; }

  //! Stores state about pan to use it by the mouse move
  void OnPan() { myCurrentMode = View_CurrentAction3d_DynamicPanning; }

  //! Stores state about onRotate to use it by the mouse move
  void OnRotate() { myCurrentMode = View_CurrentAction3d_DynamicRotation; }

  //! Updates states of tool actions:
  //! - if the action is display mode, it changes an icon for action(wireframe or shading)
  //! - if the state is checked, uncheck all other actions
  //! \param isOn boolean value about check
  Standard_EXPORT void OnUpdateToggled (bool isOn);

protected:

  //! Avoid Qt standard execution of this method, redraw V3d view
  //! \param an event
  virtual void paintEvent (QPaintEvent* theEvent) Standard_OVERRIDE;

  //! Avoid Qt standard execution of this method, do mustBeResized for V3d view, Init view if it is the first call
  //! \param an event
  virtual void resizeEvent (QResizeEvent* theEvent) Standard_OVERRIDE;

  //! Left, Middle, Right button processing
  //! \param an event
  virtual void mousePressEvent (QMouseEvent* theEvent) Standard_OVERRIDE;

  //! Left, Middle, Right button processing
  //! \param an event
  virtual void mouseReleaseEvent (QMouseEvent* theEvent) Standard_OVERRIDE;

  //! Left, Middle, Right button processing
  //! \param an event
  virtual void mouseMoveEvent (QMouseEvent* theEvent) Standard_OVERRIDE;

protected:

  //! Creates view actions and fills an internal map
  void initViewActions();

  //! Creates cursors and fills an internal map
  void initCursors();

  //! Sets widget cursor by the action type
  //! \param theMode an active action mode
  void activateCursor (const View_CurrentAction3d theMode);

  //! Activates cursor of the active operation, perform drag, onRotate depending on mode,
  //! stores the point position in xmin/xmax and ymin/ymax
  //! \param theFlags an event buttons and modifiers
  //! \param thePoint a clicked point
  void processLeftButtonDown (const int theFlags, const QPoint thePoint);

  //! Activates cursor of the active operation and performs dynamic pan if it is active
  //! \param theFlags an event buttons and modifiers
  //! \param thePoint a clicked point
  void processMiddleButtonDown (const int theFlags, const QPoint thePoint);

  //! Activates cursor of the active operation, build popup menu
  //! \param theFlags an event buttons and modifiers
  //! \param thePoint a clicked point
  void processRightButtonDown (const int theFlags, const QPoint thePoint);

  //! Performs the active operation or performs Input/Drag event processing, emits selection changed signal
  //! \param theFlags an event buttons and modifiers
  //! \param thePoint a clicked point
  void processLeftButtonUp (const int  theFlags, const QPoint thePoint);

  //! Changes the active operation to None
  //! \param theFlags an event buttons and modifiers
  //! \param thePoint a clicked point
  void processMiddleButtonUp (const int  theFlags, const QPoint thePoint);

  //! Calls popup menu build and changes the active operation to None
  //! \param theFlags an event buttons and modifiers
  //! \param thePoint a clicked point
  void processRightButtonUp (const int  theFlags, const QPoint thePoint);

  //! Performs active operation or draws rectangle of zoom
  //! \param theFlags an event buttons and modifiers
  //! \param thePoint a clicked point
  void processMouseMove (const int  theFlags, const QPoint thePoint);

  //! Performs selection: store clicked point by botton down, call Select by button up
  //! Emits signal about selection changed
  //! \param theX a horizontal position of mouse event
  //! \param theX a vertical position of mouse event
  //! \param theState a mouse button state: down, move or up
  void processDragEvent (const Standard_Integer theX, const Standard_Integer theY, const View_DragMode& theState);

  //! Performs selection in context without parameter, it means the selection of picked object
  //! \param theX a horizontal position of mouse event
  //! \param theX a vertical position of mouse event
  void processInputEvent (const Standard_Integer theX, const Standard_Integer theY);

  //! Calls MoveTo of the context for the parameter coordinates
  //! \param theX a horizontal position of mouse event
  //! \param theX a vertical position of mouse event
  void processMoveEvent (const Standard_Integer theX, const Standard_Integer theY);

  //! Empty: template to process mouse move with multi selection key pressed
  //! \param theX a horizontal position of mouse event
  //! \param theX a vertical position of mouse event
  void processMoveMultiEvent (const Standard_Integer theX, const Standard_Integer theY)
  { (void)theX; (void)theY; }

  //! Performs selection: store clicked point by botton down, call ShiftSelect by button move
  //! Emits signal about selection changed
  //! \param theX a horizontal position of mouse event
  //! \param theX a vertical position of mouse event
  //! \param theState a mouse button state: down, move or up
  void processDragMultiEvent (const Standard_Integer theX, const Standard_Integer theY, const View_DragMode& theState);

  //! Performs shift selection in context without parameter, it means the selection of picked object
  //! \param theX a horizontal position of mouse event
  //! \param theX a vertical position of mouse event
  void processInputMultiEvent (const Standard_Integer theX, const Standard_Integer theY);

  //! Empty: template to create popup menu
  //! \param theX a horizontal position of mouse event
  //! \param theX a vertical position of mouse event
  void popup (const Standard_Integer theX, const Standard_Integer theY) { (void)theX; (void)theY; }

  //! Draws Qt rectangle for the given area (e.g. for panning operation)
  //! \param theMinX a minimal X coordinate
  //! \param theMinY a minimal Y coordinate
  //! \param theMinZ a minimal Z coordinate
  //! \param theMaxX a maximum X coordinate
  //! \param theMaxY a maximum Y coordinate
  //! \param theMaxZ a maximum Z coordinate
  //! \param theToDraw state whether the rectangle should be visualized or hidden
  void drawRectangle (const Standard_Integer theMinX, const Standard_Integer theMinY, const Standard_Integer theMaxX,
                      const Standard_Integer theMaxY, const Standard_Boolean theToDraw);
private:

  //! Creates action and stores it in a map of actions
  //! \param theActionId an identifier of action in internal map
  //! \param theIcon an icon name and place according to qrc resource file, e.g. ":/icons/view_fitall.png"
  //! \param theText an action text
  //! \param theToolBar a tool bar action text
  //! \param theStatusBar a status bar action text
  void createAction (const View_ViewActionType theActionId, const QString& theIcon, const QString& theText,
                     const char* theSlot, const bool isCheckable = false,
                     const QString& theToolBar = QString(), const QString& theStatusBar = QString());

  //! Sets active action cursor for application
  //! \param theMode a cursor mode
  void setActiveCursor (const View_CursorMode& theMode);

private:

  View_Viewer* myViewer; //!< connector to context, V3d viewer and V3d View
  QMap<View_ViewActionType, QAction*> myViewActions; //!< tool bar view actions
  QMap<View_CursorMode, QCursor> myCursors; //!< possible cursors for view actions

  View_CurrentAction3d myCurrentMode; //!< an active action mode for viewer
  bool myFirst; //!< flag to Init view by the first resize/paint call
  int myDefaultWidth; //!< default width for the first sizeHint
  int myDefaultHeight; //!< default height for the first sizeHint
  bool myViewIsEnabled; //!< flag if the view and tool bar view actions are enabled/disabled
  Standard_Integer myXmin; //!< cached X minimal position by mouse press event
  Standard_Integer myYmin; //!< cached Y minimal position by mouse press event
  Standard_Integer myXmax; //!< cached X maximum position by mouse press event
  Standard_Integer myYmax; //!< cached Y maximum position by mouse press event
  Standard_Integer myDragButtonDownX; //!< cached X button down by drag event
  Standard_Integer myDragButtonDownY; //!< cached Y button down by drag event
  Standard_Integer myDragMultiButtonDownX; //!< cached X button down by multi drag event
  Standard_Integer myDragMultiButtonDownY; //!< cached Y button down by multi drag event
  Standard_Boolean myIsRectVisible; //!< true if rectangle is visible now
  QRubberBand* myRectBand; //!< selection rectangle rubber band
};

#endif
