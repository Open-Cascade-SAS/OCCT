// Viewer2dDoc.h : interface of the CViewer2dDoc class
//
/////////////////////////////////////////////////////////////////////////////

#include <AIS_Shape.hxx>
#include <AIS_LocalContext.hxx>
#include <TopoDS_Face.hxx>
#include "OCC_2dDoc.h"


class CViewer2dDoc : public OCC_2dDoc
{
protected: // create from serialization only
	CViewer2dDoc();
	DECLARE_DYNCREATE(CViewer2dDoc)

// Implementation
public:
	virtual ~CViewer2dDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CViewer2dDoc)
	afx_msg void OnBUTTONTestText();
   	afx_msg void OnBUTTONTestMarkers();
	afx_msg void OnBUTTONTestLine();
	afx_msg void OnBUTTONErase();
	afx_msg void OnBUTTONTestFace();
	afx_msg void OnBUTTONTestRect();
	afx_msg void OnBUTTONTestCurve();
	afx_msg void OnBUTTONTestImage();
	afx_msg void OnBUTTONMultipleImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

