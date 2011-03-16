// OCC_BaseChildFrame.h: interface for the OCC_BaseChildFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_BaseChildFrame_H__230F9547_3905_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_BaseChildFrame_H__230F9547_3905_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS OCC_BaseChildFrame : public CMDIChildWnd  
{
	DECLARE_DYNCREATE(OCC_BaseChildFrame)
public:
	OCC_BaseChildFrame();
	virtual ~OCC_BaseChildFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OCC_BaseChildFrame)
	public:
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//Attributes
protected:
	CToolBar m_wndToolBar;

	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_OCC_BaseChildFrame_H__230F9547_3905_11D7_8611_0060B0EE281E__INCLUDED_)
