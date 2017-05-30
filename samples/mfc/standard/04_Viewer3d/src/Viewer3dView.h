// Viewer3dView.h : interface of the CViewer3dView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWERVIEW_H__4EF39FBC_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
#define AFX_VIEWERVIEW_H__4EF39FBC_4EBB_11D1_8D67_0800369C8A03__INCLUDED_

#include <Viewer3dDoc.h>

#include <V3d_DirectionalLight.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_SpotLight.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#include <Graphic3d_ClipPlane.hxx>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
enum View3D_CurrentAction { 
  CurAction3d_Nothing,
  CurAction3d_DynamicZooming,
  CurAction3d_WindowZooming,
  CurAction3d_DynamicPanning,
  CurAction3d_GlobalPanning,
  CurAction3d_DynamicRotation,
  CurAction3d_BeginSpotLight,
  CurAction3d_TargetSpotLight,
  CurAction3d_EndSpotLight,
  CurAction3d_BeginPositionalLight,
  CurAction3d_BeginDirectionalLight,
  CurAction3d_EndDirectionalLight
};

class CViewer3dView : public CView
{
protected: // create from serialization only
	CViewer3dView();
	DECLARE_DYNCREATE(CViewer3dView)

public:
  CViewer3dDoc* GetDocument();
  void Redraw() {   myView->Redraw(); };
  void InitButtons();
  void Reset();
  void GetViewAt (Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ) const;
  void SetViewAt (const Standard_Real theX, const Standard_Real theY, const Standard_Real theZ);
  void GetViewEye (Standard_Real& X, Standard_Real& Y, Standard_Real& Z);
  void SetViewEye (const Standard_Real X,const Standard_Real Y,const Standard_Real Z);
  Standard_Real GetViewScale();
  void SetViewScale (const Standard_Real Coef);
  void FitAll() {   myView->FitAll();  myView->ZFitAll();  };


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewer3dView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CViewer3dView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

  int scaleX;
  int scaleY;
  int scaleZ;

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CViewer3dView)
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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBUTTONZoomProg();
	afx_msg void OnBUTTONZoomWin();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
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
	afx_msg void OnModifyChangeBackground();
	afx_msg void OnDirectionalLight();
	afx_msg void OnSpotLight();
	afx_msg void OnPositionalLight();
	afx_msg void OnAmbientLight();
	afx_msg void OnScale();
	afx_msg void OnShadingmodel();
	afx_msg void OnAntialiasingonoff();
	afx_msg void OnClearLights();
	afx_msg void OnModelclipping();
	afx_msg void OnOptionsTrihedronStaticTrihedron();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	enum VisMode { VIS_WIREFRAME, VIS_SHADE, VIS_HLR };
	VisMode                        myVisMode;
	
  Handle(V3d_View)                myView;
  Handle(Graphic3d_GraphicDriver) myGraphicDriver;
  View3D_CurrentAction           myCurrentMode;
  Standard_Integer               myXmin;
  Standard_Integer               myYmin;
  Standard_Integer               myXmax;
  Standard_Integer               myYmax;

  Standard_Integer               NbActiveLights;
  Standard_Boolean               myHlrModeIsOn;
  Standard_Real                  myCurZoom;
  Handle(V3d_AmbientLight)       myCurrent_AmbientLight;
  Handle(V3d_SpotLight)          myCurrent_SpotLight;
  Handle(V3d_PositionalLight)    myCurrent_PositionalLight;
  Handle(V3d_DirectionalLight)   myCurrent_DirectionalLight;
  Handle(Graphic3d_ClipPlane)    myClippingPlane;
  Handle(AIS_Shape)              myShape;

private:
	enum LineStyle { Solid, Dot, ShortDash, LongDash, Default };
	CPen*  m_Pen;
    virtual void DrawRectangle (const Standard_Integer  MinX  ,
    					        const Standard_Integer  MinY  ,
                                const Standard_Integer  MaxX  ,
				    	        const Standard_Integer  MaxY  ,
					            const Standard_Boolean  Draw  ,
                                const LineStyle aLineStyle = Default  );
	UINT myAxisKey;
	UINT myScaleDirection;
	void RedrawVisMode();

};

#ifndef _DEBUG  // debug version in Viewer3dView.cpp
inline CViewer3dDoc* CViewer3dView::GetDocument()
   { return (CViewer3dDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWERVIEW_H__4EF39FBC_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
