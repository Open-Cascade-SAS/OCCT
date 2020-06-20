// AnimationView.h : interface of the CAnimationView3D class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMATIONVIEW3D_H__79840E88_1001_11D2_B8C1_0000F87A77C1_3D_INCLUDED_)
#define AFX_ANIMATIONVIEW3D_H__79840E88_1001_11D2_B8C1_0000F87A77C1_3D_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AnimationDoc.h"
#include "..\..\Common\res\OCC_Resource.h"

#include <AIS_AnimationTimer.hxx>
#include <AIS_ViewController.hxx>

class AIS_RubberBand;

enum View3D_CurrentAction { 
  CurrentAction3d_Nothing,
  CurrentAction3d_DynamicZooming,
  CurrentAction3d_WindowZooming,
  CurrentAction3d_DynamicPanning,
  CurrentAction3d_GlobalPanning,
  CurrentAction3d_DynamicRotation,
  CurrentAction3d_Fly,
  CurrentAction3d_Turn
};

class CAnimationView3D : public CView, public AIS_ViewController
{
protected: // create from serialization only
	CAnimationView3D();
	DECLARE_DYNCREATE(CAnimationView3D)

// Attributes
public:
	CAnimationDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimationView3D)
	public:
	void FitAll() {   if ( !myView.IsNull() ) myView->FitAll();  myView->ZFitAll(); };
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate(); // CasCade
	protected:

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAnimationView3D();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	double m_dAngle;
	BOOL m_bShift;

// Generated message map functions
protected:
	//{{AFX_MSG(CAnimationView3D)
	afx_msg void OnBUTTONAxo();
	afx_msg void OnBUTTONBack();
	afx_msg void OnBUTTONBottom();
	afx_msg void OnBUTTONFront();
	afx_msg void OnBUTTONHlrOff();
	afx_msg void OnBUTTONHlrOn();
	afx_msg void OnBUTTONLeft();
	afx_msg void OnBUTTONPan();
	afx_msg void OnBUTTONPanGlo();
	afx_msg void OnBUTTONReset();
	afx_msg void OnBUTTONRight();
	afx_msg void OnBUTTONRot();
	afx_msg void OnBUTTONTop();
	afx_msg void OnBUTTONZoomAll();
    afx_msg void OnFileExportImage();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBUTTONZoomProg();
	afx_msg void OnBUTTONZoomWin();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnUpdateBUTTONHlrOff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONHlrOn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONPanGlo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONPan(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONZoomProg(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONZoomWin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONRot(CCmdUI* pCmdUI);

	afx_msg void OnChangeBackground();

	afx_msg void OnBUTTONFly();
	afx_msg void OnBUTTONTurn();
	afx_msg void OnUpdateBUTTONFly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONTurn(CCmdUI* pCmdUI);
	//}}AFX_MSG
	
public :
	afx_msg void OnStop();
	afx_msg void OnRestart();
protected:

	DECLARE_MESSAGE_MAP()
private:
	Handle(V3d_View)     myView;
public:
	Handle(V3d_View)&    GetView() { return myView;};

  //! Request view redrawing.
  void update3dView();

  //! Flush events and redraw view.
  void redraw3dView();

protected:

  //! Handle view redraw.
  virtual void handleViewRedraw (const Handle(AIS_InteractiveContext)& theCtx,
                                 const Handle(V3d_View)& theView) Standard_OVERRIDE;

protected:

  //! Setup mouse gestures.
  void defineMouseGestures();

  //! Get current action.
  View3D_CurrentAction getCurrentAction() const { return myCurrentMode; }

  //! Set current action.
  void setCurrentAction (View3D_CurrentAction theAction)
  {
    myCurrentMode = theAction;
    defineMouseGestures();
  }

private:
  AIS_AnimationTimer   myAnimTimer;
  AIS_MouseGestureMap  myDefaultGestures;
  Graphic3d_Vec2i      myClickPos;
  Standard_Real        myCurZoom;
  Standard_Boolean     myHlrModeIsOn;
  Standard_Boolean     myIsTurnStarted;
  unsigned int         myUpdateRequests; //!< counter for unhandled update requests

  View3D_CurrentAction myCurrentMode;
};

#ifndef _DEBUG  // debug version in AnimationView.cpp
inline CAnimationDoc* CAnimationView3D::GetDocument()
   { return (CAnimationDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMATIONVIEW3D_H__79840E88_1001_11D2_B8C1_0000F87A77C1_3D_INCLUDED_)
