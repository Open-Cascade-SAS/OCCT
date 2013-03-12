// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_ChildFrame_H__338883C5_510A_11D1_A4A8_00C04FB15CA3__INCLUDED_)
#define AFX_ChildFrame_H__338883C5_510A_11D1_A4A8_00C04FB15CA3__INCLUDED_

#include <OCC_BaseChildFrame.h>

class CChildFrame : public OCC_BaseChildFrame
{
  DECLARE_DYNCREATE(CChildFrame)
public:
  CChildFrame();

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CChildFrame)
public:
  virtual void ActivateFrame(int nCmdShow = -1);
  virtual BOOL DestroyWindow();
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CChildFrame();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(CChildFrame)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif// MainFrm.cpp : implementation of the CMainFrame class
