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
#include <AIS_ViewController.hxx>
#include <Aspect_VKeyFlags.hxx>
#include <V3d_View.hxx>
#include <inspector/View_ViewActionType.hxx>
#include <inspector/View_Viewer.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAction>
#include <QMap>
#include <QString>
#include <QToolButton>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

class View_Viewer;

//! \class View_Widget
//! \brief It is a Qt control that visualizes content of OCCT 3D view
//! It creates control and actions of manipulating of this view,
//! emits signal about selection happenning in the view and signal about display mode change and
//! changes application cursor depending on an active action.
class View_Widget : public QWidget
{
  Q_OBJECT

public:
  //! Constructor
  Standard_EXPORT View_Widget (QWidget* theParent,
                               const Handle(AIS_InteractiveContext)& theContext,
                               const bool isFitAllActive);

  //! Destructor
  virtual ~View_Widget() {}

  //! Returns current viewer
  View_Viewer* GetViewer() const { return myViewer; }

  //! Sets default size that is used in sizeHint when the widget is firstly show
  Standard_EXPORT void SetPredefinedSize (int theDefaultWidth, int theDefaultHeight);

  //! Creates V3d view and set Qt control for it
  Standard_EXPORT void Init();

  //! Returns an action for the given action type
  //! \param theActionId an action index
  QAction* ViewAction (const View_ViewActionType theActionId) const { return myViewActions[theActionId]; };

  //! Retuns an action widget if exist. Implemented for fit all widget.
  //! \param theActionId an action index
  QWidget* GetWidget (const View_ViewActionType theActionId) const { return theActionId == View_ViewActionType_FitAllId ? myFitAllAction : 0; };

  //! \returns 0 - AIS_WireFrame, 1 - AIS_Shaded
  Standard_EXPORT int DisplayMode() const;

  //! Sets display mode: 0 - AIS_WireFrame, 1 - AIS_Shaded
  Standard_EXPORT void SetDisplayMode (const int theMode);

  //! Sets enable/disable view and tool bar actions depending on the parameter
  //! \param theIsEnabled boolean value
  Standard_EXPORT void SetEnabledView (const bool theIsEnabled);

  //! Returns true if action is checked. It processes fit all action only.
  //! \param theIsEnabled boolean value
  bool IsActionChecked (const View_ViewActionType theActionId)
    { return theActionId == View_ViewActionType_FitAllId && myFitAllAction->isChecked(); }

  //! Sets checked fit all action. Double click on fit all action set the action checked automatically
  //! \param theIsEnabled boolean value
  void SetActionChecked (const View_ViewActionType theActionId, const bool isChecked)
  { if (theActionId == View_ViewActionType_FitAllId) myFitAllAction->setChecked(isChecked); }

  //! Sets initial camera position
  //! \param theVx direction on Ox
  //! \param theVy direction on Oy
  //! \param theVz direction on Oz
  void SetInitProj (const Standard_Real theVx, const Standard_Real theVy, const Standard_Real theVz)
  { myHasInitProj = Standard_True; myInitVx = theVx; myInitVy = theVy; myInitVz = theVz; }

  //! Returns paint engine for the OpenGL viewer. Avoid default execution of Qt Widget.
  virtual QPaintEngine* paintEngine() const Standard_OVERRIDE { return 0; }

  //! Return the recommended size for view. If default size exists, it returns the default size
  Standard_EXPORT virtual QSize sizeHint() const Standard_OVERRIDE;

  //! Saves state of widget actions
  //! \param theParameters a view instance
  //! \param theItems [out] properties
  //! \param thePrefix peference item prefix
  Standard_EXPORT static void SaveState (View_Widget* theWidget,
                                         QMap<QString, QString>& theItems,
                                         const QString& thePrefix = QString());

  //! Restores state of widget actions
  //! \param theParameters a view instance
  //! \param theKey property key
  //! \param theValue property value
  //! \param thePrefix peference item prefix
  //! \return boolean value whether the property is applied to the tree view
  Standard_EXPORT static bool RestoreState (View_Widget* theWidget,
                                            const QString& theKey, const QString& theValue,
                                            const QString& thePrefix = QString());

signals:

  //! Sends a signal about selection change if the left mouse button is pressed and current action does not process it
  void selectionChanged();

  //! Sends a signal about display mode change
  void displayModeClicked();

  //! Sends a signal about checked state is changed
  //! \param theActionId an action index
  //! \param theState the checked state
  void checkedStateChanged (const int theActionId, bool theState);

public slots:

  //! Fits all the V3d view and redraw view
  void OnFitAll() { myViewer->GetView()->FitAll(); }

  //! Updates states of widget actions
  //! 
  //! - if the state is checked, uncheck all other actions
  Standard_EXPORT void onCheckedStateChanged (bool isOn);

protected:

  //! Avoids Qt standard execution of this method, redraw V3d view
  //! \param an event
  virtual void paintEvent (QPaintEvent* theEvent) Standard_OVERRIDE;

  //! Avoids Qt standard execution of this method, do mustBeResized for V3d view, Init view if it is the first call
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

  //! Empty: template to create popup menu
  //! \param theX a horizontal position of mouse event
  //! \param theX a vertical position of mouse event
  void popup (const Standard_Integer theX, const Standard_Integer theY) { (void)theX; (void)theY; }

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

private:
  //! Converts Qt modifier key to Aspect key flag
  //! \param theModifierId the event modifier
  static Aspect_VKeyFlags keyFlag (const int theModifierId);

  //! Converts Qt button key to Aspect key mouse
  //! \param theButtonId the event button
  static Aspect_VKeyMouse keyMouse (const int theButtonId);

private:

  View_Viewer* myViewer; //!< connector to context, V3d viewer and V3d View
  AIS_ViewController* myController; //!< controller to process view actions

  QToolButton* myFitAllAction; //!< widget for fit all, processed double click to perform action automatically
  QMap<View_ViewActionType, QAction*> myViewActions; //!< tool bar view actions

  Standard_Boolean myFirst; //!< flag to Init view by the first resize/paint call
  Standard_Integer myDefaultWidth; //!< default width for the first sizeHint
  Standard_Integer myDefaultHeight; //!< default height for the first sizeHint
  Standard_Boolean myViewIsEnabled; //!< flag if the view and tool bar view actions are enabled/disabled

  Standard_Boolean myHasInitProj; //!< is initial camera position defined
  Standard_Real myInitVx; //!< initial camera position on X
  Standard_Real myInitVy; //!< initial camera position on Y
  Standard_Real myInitVz; //!< initial camera position on Z
};

#endif
