// OCC_3dView.h: interface for the OCC_3dView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_3DVIEW_H__1F4065AE_39C4_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_3DVIEW_H__1F4065AE_39C4_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCC_BaseView.h"
#include "OCC_3dDoc.h"
#include "OCC_StereoConfigDlg.h"
#include <Standard_Macro.hxx>

enum CurAction3d { 
  CurAction3d_Nothing,
  CurAction3d_DynamicZooming,
  CurAction3d_WindowZooming,
  CurAction3d_DynamicPanning,
  CurAction3d_GlobalPanning,
  CurAction3d_DynamicRotation
};

class AFX_EXT_CLASS OCC_3dView : public OCC_BaseView  
{
	DECLARE_DYNCREATE(OCC_3dView)
public:
	OCC_3dView();
	virtual ~OCC_3dView();

	OCC_3dDoc* GetDocument();
	void FitAll() {   if ( !myView.IsNull() ) myView->FitAll();  myView->ZFitAll(); };
	void Redraw() {   if ( !myView.IsNull() ) myView->Redraw(); };

	void SetZoom ( const Standard_Real& Coef  ) {   myView->SetZoom ( Coef  );  };

  Handle(V3d_View)& GetView() { return myView; }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OCC_3dView)
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(OCC_3dView)
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
	afx_msg void OnModifyChangeBackground();
  afx_msg void OnStereoConfigButton();
  afx_msg void OnUpdateStereoConfigButton (CCmdUI* theCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

  Handle(V3d_View) myView;
  CurAction3d      myCurrentMode;
  Standard_Integer myWidth;
  Standard_Integer myHeight;
  Standard_Boolean myHlrModeIsOn;

private:

  //! Persistent non blocking stereo configuration dialog
  OCC_StereoConfigDlg* m_pStereoDlg;
};

#ifndef _DEBUG  // debug version in OCC_3dView.cpp
inline OCC_3dDoc* OCC_3dView::GetDocument()
   { return (OCC_3dDoc*)m_pDocument; }
#endif

#endif // !defined(AFX_OCC_3DVIEW_H__1F4065AE_39C4_11D7_8611_0060B0EE281E__INCLUDED_)
