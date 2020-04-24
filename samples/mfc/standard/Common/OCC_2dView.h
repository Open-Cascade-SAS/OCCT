// OCC_2dView.h: interface for the OCC_2dView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_2dVIEW_H__2E048CC9_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_2dVIEW_H__2E048CC9_38F9_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCC_BaseView.h"
#include "OCC_2dDoc.h"
#include "Resource2d/RectangularGrid.h"
#include "Resource2d/CircularGrid.h"

class Standard_EXPORT OCC_2dView : public OCC_BaseView  
{
  DECLARE_DYNCREATE(OCC_2dView)

public:

  OCC_2dView();

  virtual ~OCC_2dView();

  OCC_2dDoc* GetDocument();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(OCC_2dView)
protected:
  virtual void OnInitialUpdate(); // called first time after construct
    //}}AFX_VIRTUAL

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
  //{{AFX_MSG(OCC_2dView)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
  afx_msg void OnFileExportImage();
  afx_msg void OnBUTTONGridRectLines();
  afx_msg void OnBUTTONGridRectPoints();
  afx_msg void OnBUTTONGridCircLines();
  afx_msg void OnBUTTONGridCircPoints();
  afx_msg void OnBUTTONGridValues();
  afx_msg void OnUpdateBUTTONGridValues(CCmdUI* pCmdUI);
  afx_msg void OnBUTTONGridCancel();
  afx_msg void OnUpdateBUTTONGridCancel(CCmdUI* pCmdUI);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnBUTTONFitAll();
  afx_msg void OnBUTTONGlobPanning();
  afx_msg void OnBUTTONPanning();
  afx_msg void OnBUTTONZoomProg();
  afx_msg void OnBUTTONZoomWin();
  afx_msg void OnUpdateBUTTON2DGlobPanning(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTON2DPanning(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTON2DZoomProg(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTON2DZoomWin(CCmdUI* pCmdUI);
  afx_msg void OnChangeBackground();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

protected:
  CRectangularGrid TheRectangularGridDialog;
  CCircularGrid    TheCircularGridDialog;
};

#ifndef _DEBUG  // debug version in 2DDisplayView.cpp
inline OCC_2dDoc* OCC_2dView::GetDocument()
   { return (OCC_2dDoc*)m_pDocument; }
#endif

#endif // !defined(AFX_OCC_2dVIEW_H__2E048CC9_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
