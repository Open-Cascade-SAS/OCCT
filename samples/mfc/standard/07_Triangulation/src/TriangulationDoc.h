// TriangulationDoc.h : interface of the CTopologyTriangulationDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIANGULATIONDOC_H__3045338B_3E75_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_TRIANGULATIONDOC_H__3045338B_3E75_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_3dDoc.h>
//#include "OCCDemo_Presentation.h"
class OCCDemo_Presentation;

class CTriangulationDoc : public OCC_3dDoc
{

protected: // create from serialization only
	CTriangulationDoc();
	DECLARE_DYNCREATE(CTriangulationDoc)
	void InitViewButtons();
	void DoSample();
public:
	virtual ~CTriangulationDoc();
	void Start();
	static void Fit(); 
	Standard_CString GetDataDir() {return myDataDir;}
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CTriangulationDoc)
	afx_msg void OnTriangu();
	afx_msg void OnClear();
	afx_msg void OnVisu();
	afx_msg void OnBUTTONNext();
	afx_msg void OnBUTTONStart();
	afx_msg void OnBUTTONRepeat();
	afx_msg void OnBUTTONPrev();
	afx_msg void OnBUTTONEnd();
	afx_msg void OnUpdateBUTTONNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONPrev(CCmdUI* pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnDumpView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	OCCDemo_Presentation *myPresentation;
	char myDataDir[5]; // for "Data\0"
	char myLastPath[MAX_PATH]; // directory of lastly saved file in DumpView()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIANGULATIONDOC_H__3045338B_3E75_11D7_8611_0060B0EE281E__INCLUDED_)
