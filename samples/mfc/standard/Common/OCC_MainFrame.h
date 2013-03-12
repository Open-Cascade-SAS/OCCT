// OCC_MainFrame.h: interface for the OCC_MainFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_MAINFRAME_H__B7D6F369_38DA_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_MAINFRAME_H__B7D6F369_38DA_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCC_App.h>

enum OCC_MainFrm{
	without_AIS_TB,
	with_AIS_TB
};

class Standard_EXPORT OCC_MainFrame : public CMDIFrameWnd  
{
	DECLARE_DYNAMIC(OCC_MainFrame)
public:
	OCC_MainFrame(BOOL withAISToolBar = false);
	virtual ~OCC_MainFrame();

	void SetStatusMessage(const CString & message);

// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommonMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL


// Implementation

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
     void DockControlBarLeftOf(CToolBar* Bar,CToolBar* LeftOf);

	
	
// Attributes
protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBar*    m_AISToolBar;
	BOOL	 m_withAISToolBar;


// Generated message map functions
protected:
	//{{AFX_MSG(OCC_MainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_OCC_MAINFRAME_H__B7D6F369_38DA_11D7_8611_0060B0EE281E__INCLUDED_)
