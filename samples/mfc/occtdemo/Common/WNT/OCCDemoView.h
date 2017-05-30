// OCCDemoView.h : interface of the COCCDemoView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCCDEMOVIEW_H__57A6A230_7B4F_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_OCCDEMOVIEW_H__57A6A230_7B4F_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum View3D_CurrentAction { 
  CurAction3d_Nothing,
  CurAction3d_DynamicZooming,
  CurAction3d_WindowZooming,
  CurAction3d_DynamicPanning,
  CurAction3d_GlobalPanning,
  CurAction3d_DynamicRotation
};

class COCCDemoView : public CView
{
protected: // create from serialization only
	COCCDemoView();
	DECLARE_DYNCREATE(COCCDemoView)

// Attributes
public:
	COCCDemoDoc* GetDocument();

// Operations
public:
  void InitButtons();
  void Reset();
  void FitAll() {   myView->FitAll();  myView->ZFitAll();  };
  void GetViewCenter(Standard_Real& Xc, Standard_Real& Yc);
  void SetViewCenter(const Standard_Real Xc, const Standard_Real Yc);
  void GetViewEye(Standard_Real& X, Standard_Real& Y, Standard_Real& Z);
  void SetViewEye(const Standard_Real X,const Standard_Real Y,const Standard_Real Z);
  Standard_Real GetViewScale();
  void SetViewScale(const Standard_Real Coef);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COCCDemoView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COCCDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(COCCDemoView)
	afx_msg void OnBUTTONAxo();
	afx_msg void OnBUTTONBack();
	afx_msg void OnBUTTONBottom();
	afx_msg void OnBUTTONFront();
	afx_msg void OnBUTTONHlrOn();
	afx_msg void OnBUTTONLeft();
	afx_msg void OnBUTTONPan();
	afx_msg void OnBUTTONPanGlo();
	afx_msg void OnBUTTONReset();
	afx_msg void OnBUTTONRight();
	afx_msg void OnBUTTONRot();
	afx_msg void OnBUTTONTop();
	afx_msg void OnBUTTONZoomAll();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBUTTONZoomProg();
	afx_msg void OnBUTTONZoomWin();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnUpdateBUTTONPanGlo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONPan(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONZoomProg(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONZoomWin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONRot(CCmdUI* pCmdUI);
	afx_msg void OnBUTTONWire();
	afx_msg void OnBUTTONShade();
	afx_msg void OnUpdateBUTTONHlrOn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONShade(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONWire(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  void RedrawVisMode();

private:
  enum VisMode { VIS_WIREFRAME, VIS_SHADE, VIS_HLR };
  Handle(V3d_View)     myView;
  Handle(Graphic3d_GraphicDriver) myGraphicDriver;
  View3D_CurrentAction myCurrentMode;
  VisMode              myVisMode;
  Standard_Integer     myXmin;
  Standard_Integer     myYmin;  
  Standard_Integer     myXmax;
  Standard_Integer     myYmax;
  Standard_Real        myCurZoom;

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

#ifndef _DEBUG  // debug version in OCCDemoView.cpp
inline COCCDemoDoc* COCCDemoView::GetDocument()
   { return (COCCDemoDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OCCDEMOVIEW_H__57A6A230_7B4F_11D5_BA4A_0060B0EE18EA__INCLUDED_)
