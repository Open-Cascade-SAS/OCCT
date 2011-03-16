// OCC_2dViewRD.h: interface for the OCC_2dViewRD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_2DVIEWRD_H__AD650F81_8D03_11D7_8637_0060B0EE281E__INCLUDED_)
#define AFX_OCC_2DVIEWRD_H__AD650F81_8D03_11D7_8637_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCC_2dView.h"

class OCC_2dViewRD : public OCC_2dView  
{
public:
	OCC_2dDoc* GetDocument();
	OCC_2dViewRD();
	virtual ~OCC_2dViewRD();



// Generated message map functions
protected:
	//{{AFX_MSG(OCC_2dView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
    afx_msg void OnFileExportImage();
   	afx_msg void OnBUTTONGridRectLines();
	afx_msg void OnBUTTONGridRectPoints();
	afx_msg void OnBUTTONGridCircLines();
	afx_msg void OnBUTTONGridCircPoints();
	afx_msg void OnBUTTONGridValues();
	afx_msg void OnBUTTONGridCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_OCC_2DVIEWRD_H__AD650F81_8D03_11D7_8637_0060B0EE281E__INCLUDED_)
