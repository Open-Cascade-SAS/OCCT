// OCCDemoDoc.h : interface of the COCCDemoDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCCDEMODOC_H__57A6A22E_7B4F_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_OCCDEMODOC_H__57A6A22E_7B4F_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ResultDialog.h"
class OCCDemo_Presentation;

class COCCDemoDoc : public CDocument
{
public:
  Handle(V3d_Viewer) GetViewer() const { return myViewer; };
  Handle(AIS_InteractiveContext) GetAISContext() const { return myAISContext; };
  CResultDialog* GetResultDialog () {return &myCResultDialog;}
  Standard_CString GetDataDir() {return myDataDir;}

protected: // create from serialization only
	COCCDemoDoc();
	DECLARE_DYNCREATE(COCCDemoDoc)

// Attributes
public:
  BOOL& IsShowResult() {return myShowResult;}

// Operations
public:
  void Start();
  static void Fit();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COCCDemoDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COCCDemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  void InitViewButtons();
  void DoSample();

// Generated message map functions
protected:
	//{{AFX_MSG(COCCDemoDoc)
	afx_msg void OnBUTTONNext();
	afx_msg void OnBUTTONStart();
	afx_msg void OnBUTTONRepeat();
	afx_msg void OnBUTTONPrev();
	afx_msg void OnBUTTONEnd();
	afx_msg void OnUpdateBUTTONNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONPrev(CCmdUI* pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnBUTTONShowResult();
	afx_msg void OnUpdateBUTTONShowResult(CCmdUI* pCmdUI);
	afx_msg void OnDumpView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  Handle(V3d_Viewer) myViewer;
  Handle(AIS_InteractiveContext) myAISContext;
  OCCDemo_Presentation *myPresentation;
  CResultDialog myCResultDialog;
  BOOL myShowResult;
  char myDataDir[8]; // for "..\Data\0"
  char myLastPath[MAX_PATH]; // directory of lastly saved file in DumpView()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OCCDEMODOC_H__57A6A22E_7B4F_11D5_BA4A_0060B0EE18EA__INCLUDED_)
