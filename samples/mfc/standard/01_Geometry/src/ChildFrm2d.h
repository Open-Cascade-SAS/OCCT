// ChildFrm.h : interface of the CChildFrame2D class
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_ChildFrame2D_H__338883C5_510A_11D1_A4A8_00C04FB15CA3__INCLUDED_)
#define AFX_ChildFrame2D_H__338883C5_510A_11D1_A4A8_00C04FB15CA3__INCLUDED_

#include <OCC_BaseChildFrame.h>

class CChildFrame2D : public OCC_BaseChildFrame
{
  DECLARE_DYNCREATE(CChildFrame2D)
public:
  CChildFrame2D();

  // Attributes
public:

  // Operations
public:

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CChildFrame2D)
public:
  virtual void ActivateFrame(int nCmdShow = -1);
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CChildFrame2D();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
protected:
  //CToolBar m_wndToolBar;
  //{{AFX_MSG(CChildFrame2D)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
