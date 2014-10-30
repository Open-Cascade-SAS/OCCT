// OCC_3dChildFrame.h: interface for the OCC_3dChildFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_3DCHILDFRAME_H__84879CFC_7EE3_11D7_8632_0060B0EE281E__INCLUDED_)
#define AFX_OCC_3DCHILDFRAME_H__84879CFC_7EE3_11D7_8632_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCC_BaseChildFrame.h"
#include <Standard_Macro.hxx>

class AFX_EXT_CLASS OCC_3dChildFrame : public OCC_BaseChildFrame  
{
	DECLARE_DYNCREATE(OCC_3dChildFrame)
public:
	OCC_3dChildFrame();
	virtual ~OCC_3dChildFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OCC_3dChildFrame)
	//}}AFX_VIRTUAL

// Generated message map functions
	//{{AFX_MSG(OCC_3dChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

//Attributes
protected:
	CToolBar m_RTToolBar;
};

#endif // !defined(AFX_OCC_3DCHILDFRAME_H__84879CFC_7EE3_11D7_8632_0060B0EE281E__INCLUDED_)
