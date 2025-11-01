// OCC_BaseView.h: interface for the OCC_BaseView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_BASEVIEW_H__2E048CCA_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_BASEVIEW_H__2E048CCA_38F9_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdafx.h>

#include "OCC_BaseDoc.h"
#include "AIS_ViewController.hxx"

enum CurAction3d
{
  CurAction3d_Nothing,
  CurAction3d_DynamicZooming,
  CurAction3d_WindowZooming,
  CurAction3d_DynamicPanning,
  CurAction3d_GlobalPanning,
  CurAction3d_DynamicRotation
};

class Standard_EXPORT OCC_BaseView  : public CView, public AIS_ViewController
{
  
public:
  
	OCC_BaseView();
	virtual ~OCC_BaseView();

  //! Return the view.
  const Handle(V3d_View)& GetView() const { return myView; }

  void FitAll() {   if ( !myView.IsNull() ) myView->FitAll();  myView->ZFitAll(); };
  void Redraw() {   if ( !myView.IsNull() ) myView->Redraw(); };

  void SetZoom ( const Standard_Real& Coef  ) {   myView->SetZoom ( Coef  );  };

  //! Return the document.
  OCC_BaseDoc* GetDocument();

  //! Request view redrawing.
  void update3dView();

  //! Flush events and redraw view.
  void redraw3dView();

protected:

  //! Handle view redraw.
  virtual void handleViewRedraw (const Handle(AIS_InteractiveContext)& theCtx,
                                 const Handle(V3d_View)& theView) Standard_OVERRIDE;

  //! Callback called by handleMoveTo() on Selection in 3D Viewer.
  virtual void OnSelectionChanged (const Handle(AIS_InteractiveContext)& theCtx,
                                   const Handle(V3d_View)& theView) Standard_OVERRIDE;

  //! Return interactive context.
  virtual const Handle(AIS_InteractiveContext)& GetAISContext() const;

protected:

  //! Setup mouse gestures.
  void defineMouseGestures();

  //! Get current action.
  CurAction3d getCurrentAction() const { return myCurrentMode; }

  //! Set current action.
  void setCurrentAction (CurAction3d theAction)
  {
    myCurrentMode = theAction;
    defineMouseGestures();
  }

public:

  virtual BOOL PreCreateWindow (CREATESTRUCT& cs) Standard_OVERRIDE;
  virtual void PostNcDestroy() Standard_OVERRIDE;

protected:

  Handle(V3d_View)    myView;
  AIS_MouseGestureMap myDefaultGestures;
  Graphic3d_Vec2i     myClickPos;
  Standard_Real       myCurZoom;
  unsigned int        myUpdateRequests; //!< counter for unhandled update requests

private:

  CurAction3d         myCurrentMode;

// message map functions
protected:
  DECLARE_MESSAGE_MAP()
public:
  virtual void OnInitialUpdate() Standard_OVERRIDE;
  virtual void OnDraw (CDC* /*pDC*/) Standard_OVERRIDE;
  afx_msg void OnSize (UINT nType, int cx, int cy);
  afx_msg BOOL OnMouseWheel (UINT theFlags, short theDelta, CPoint thePoint);
  afx_msg void OnMouseMove (UINT theFlags, CPoint thePoint);
  afx_msg void OnMouseLeave();
  afx_msg void OnLButtonDown (UINT theFlags, CPoint thePoint);
  afx_msg void OnLButtonUp (UINT theFlags, CPoint thePoint);
  afx_msg void OnMButtonDown (UINT theFlags, CPoint thePoint);
  afx_msg void OnMButtonUp (UINT theFlags, CPoint thePoint);
  afx_msg void OnRButtonDown (UINT theFlags, CPoint thePoint);
  afx_msg void OnRButtonUp (UINT theFlags, CPoint thePoint);
};

#endif // !defined(AFX_OCC_BASEVIEW_H__2E048CCA_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
