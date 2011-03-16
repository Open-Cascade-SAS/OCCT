// OCC_2dChildFrame.h: interface for the OCC_2dChildFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_2DCHILDFRAME_H__84879CFB_7EE3_11D7_8632_0060B0EE281E__INCLUDED_)
#define AFX_OCC_2DCHILDFRAME_H__84879CFB_7EE3_11D7_8632_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCC_BaseChildFrame.h"

class AFX_EXT_CLASS OCC_2dChildFrame : public OCC_BaseChildFrame  
{
	DECLARE_DYNCREATE(OCC_2dChildFrame)
public:
	OCC_2dChildFrame();
	virtual ~OCC_2dChildFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OCC_2dChildFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Generated message map functions
	//{{AFX_MSG(OCC_2dChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_OCC_2DCHILDFRAME_H__84879CFB_7EE3_11D7_8632_0060B0EE281E__INCLUDED_)
