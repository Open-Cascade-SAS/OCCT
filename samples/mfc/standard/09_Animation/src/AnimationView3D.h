// AnimationView.h : interface of the CAnimationView3D class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMATIONVIEW3D_H__79840E88_1001_11D2_B8C1_0000F87A77C1_3D_INCLUDED_)
#define AFX_ANIMATIONVIEW3D_H__79840E88_1001_11D2_B8C1_0000F87A77C1_3D_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Tune.h"
#include "AnimationDoc.h"
#include "..\..\Common\res\OCC_Resource.h"

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

class CAnimationView3D : public CView
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
	void SetDimensions ();
	void ReloadData();
	CTune m_Tune;
  void SetFocal (double theFocus, double theAngle);
	void Fly (int x , int y);
	void Turn (int x , int y);
	void Roll (int x , int y);
	void Twist (int x , int y);

protected:
	double m_dAngle;
	BOOL m_bShift;
	int m_cx , m_cy ;
	int m_curx , m_cury ;

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
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnSensitivity();
	afx_msg void OnBUTTONFly();
	afx_msg void OnBUTTONTurn();
	afx_msg void OnUpdateBUTTONFly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONTurn(CCmdUI* pCmdUI);
	afx_msg void OnViewDisplaystatus();
	afx_msg void OnUpdateViewDisplaystatus(CCmdUI* pCmdUI);
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
	void                DisplayTuneDialog();
private:
  Standard_Integer     myXmin;
  Standard_Integer     myYmin;
  Standard_Integer     myXmax;
  Standard_Integer     myYmax;
  Standard_Real        myCurZoom;
  Standard_Boolean     myHlrModeIsOn;

  View3D_CurrentAction myCurrentMode;
  double               m_Atx  , m_Aty  , m_Atz  ;
  double               m_Eyex , m_Eyey , m_Eyez ;
  double               m_FlySens ;
  double               m_TurnSens ;
  double               m_Focus ;

private:
	enum LineStyle { Solid, Dot, ShortDash, LongDash, Default };
	CPen*  m_Pen;
    virtual void DrawRectangle (const Standard_Integer  MinX  ,
    					        const Standard_Integer  MinY  ,
                                const Standard_Integer  MaxX  ,
				    	        const Standard_Integer  MaxY  ,
					            const Standard_Boolean  Draw  ,
                                const LineStyle aLineStyle = Default  );
};

#ifndef _DEBUG  // debug version in AnimationView.cpp
inline CAnimationDoc* CAnimationView3D::GetDocument()
   { return (CAnimationDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMATIONVIEW3D_H__79840E88_1001_11D2_B8C1_0000F87A77C1_3D_INCLUDED_)
