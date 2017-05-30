// GeometryView.h : interface of the CGeometryView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWERVIEW_H__4EF39FBC_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
#define AFX_VIEWERVIEW_H__4EF39FBC_4EBB_11D1_8D67_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_3dView.h>

class CGeometryView : public OCC_3dView
{
protected: // create from serialization only
public:
  CGeometryView();
  DECLARE_DYNCREATE(CGeometryView)

  // Attributes
public:
  CGeometryDoc* GetDocument();

  // Operations
public:

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGeometryView)
public:
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CGeometryView();
  void FitAll(Standard_Real Coef)
  {
    if (Coef != -1)
      myView->FitAll(Coef);
    else myView->FitAll();
    myView->ZFitAll();
  };

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
  //{{AFX_MSG(CGeometryView)
  afx_msg void OnFileExportImage();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GeometryView.cpp
inline CGeometryDoc* CGeometryView::GetDocument()
   { return (CGeometryDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWERVIEW_H__4EF39FBC_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
