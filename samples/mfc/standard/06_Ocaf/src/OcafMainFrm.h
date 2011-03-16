// MainFrm.h : interface of the OcafMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__49324D78_7836_11D4_8D45_00AA00D10994__INCLUDED_)
#define AFX_MAINFRM_H__49324D78_7836_11D4_8D45_00AA00D10994__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_MainFrame.h>

class OcafMainFrame : public OCC_MainFrame
{
	DECLARE_DYNAMIC(OcafMainFrame)
public:
	OcafMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OcafMainFrame)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~OcafMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:  // control bar embedded members
	CToolBar   	m_UndoRedoBar;
	CToolBar   	m_ActionsBar;

// Generated message map functions
protected:
	//{{AFX_MSG(OcafMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private :
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__49324D78_7836_11D4_8D45_00AA00D10994__INCLUDED_)
