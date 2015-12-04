// OcafDoc.h : interface of the COcafDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCAFDOC_H__49324D7C_7836_11D4_8D45_00AA00D10994__INCLUDED_)
#define AFX_OCAFDOC_H__49324D7C_7836_11D4_8D45_00AA00D10994__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_3dDoc.h>
#include <OCC_3dView.h>

class COcafDoc : public OCC_3dDoc
{
protected: // create from serialization only
	COcafDoc();
	DECLARE_DYNCREATE(COcafDoc)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COcafDoc)
	public:
	virtual void OnCloseDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL COcafDoc::OnNewDocument();

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COcafDoc();
    virtual void Popup (const Standard_Integer  x       ,
		    			const Standard_Integer  y       ,
                        const Handle(V3d_View)& aView   ); 

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(COcafDoc)
	afx_msg void OnCreatebox();
	afx_msg void OnEditRedo();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnModify();
	afx_msg void OnUpdateModify(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnCut();
	afx_msg void OnObjectDelete();
	afx_msg void OnUpdateObjectDelete(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnCreatecyl();
	afx_msg void OnDfbr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public :
void ActivateFrame(CRuntimeClass* pViewClass, int nCmdShow = SW_RESTORE  );
  Handle(TDocStd_Document) GetOcafDoc() {return myOcafDoc;};
	CString PathName;

private:
  Handle(TDocStd_Document) myOcafDoc;

public :
	void EraseAll();

private:
	void DisplayPrs();
	void Fit3DViews();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OCAFDOC_H__49324D7C_7836_11D4_8D45_00AA00D10994__INCLUDED_)
