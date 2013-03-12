// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__4EF39FB6_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
#define AFX_MAINFRM_H__4EF39FB6_4EBB_11D1_8D67_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMainFrame : public CMDIFrameWnd
{
  DECLARE_DYNAMIC(CMainFrame)
public:
  CMainFrame();

  // Attributes
public:

  // Operations
public:

  // Overrides
  // ClassWizard generated virtual function overrides
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

  // Implementation
public:
  virtual ~CMainFrame();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
  CStatusBar  m_wndStatusBar;
  CToolBar    m_wndToolBar;
  CToolBar    m_wndToolBar2;

  // Generated message map functions
protected:
  //{{AFX_MSG(CMainFrame)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__4EF39FB6_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
