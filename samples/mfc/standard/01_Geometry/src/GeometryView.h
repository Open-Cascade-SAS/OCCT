// GeometryView.h : interface of the CGeometryView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWERVIEW_H__4EF39FBC_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
#define AFX_VIEWERVIEW_H__4EF39FBC_4EBB_11D1_8D67_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_3dView.h>

class CGeometryView : public OCC_3dView
{
protected: // create from serialization only
public:
	CGeometryView();
	DECLARE_DYNCREATE(CGeometryView)

// Attributes
public:
	CGeometryDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeometryView)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGeometryView();
	void FitAll(Quantity_Coefficient Coef) {   if (Coef != -1) 	myView->FitAll(Coef);
												else myView->FitAll();
												myView->ZFitAll();  };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CGeometryView)
	afx_msg void OnFileExportImage();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnUpdateBUTTONHlrOff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONHlrOn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONPanGlo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONPan(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONZoomProg(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONZoomWin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBUTTONRot(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void DragEvent      (const Standard_Integer  x       ,
	    			     const Standard_Integer  y       ,
		    		     const Standard_Integer  TheState);
    void InputEvent     (const Standard_Integer  x       ,
	    			     const Standard_Integer  y       );  
    void MoveEvent      (const Standard_Integer  x       ,
                         const Standard_Integer  y       ); 
    void ShiftMoveEvent (const Standard_Integer  x       ,
                         const Standard_Integer  y       ); 
    void ShiftDragEvent (const Standard_Integer  x       ,
	    				 const Standard_Integer  y       ,
		    			 const Standard_Integer  TheState); 
    void ShiftInputEvent(const Standard_Integer  x       ,
	    				 const Standard_Integer  y       ); 
    void Popup          (const Standard_Integer  x       ,
		    			 const Standard_Integer  y       ); 

};

#ifndef _DEBUG  // debug version in GeometryView.cpp
inline CGeometryDoc* CGeometryView::GetDocument()
   { return (CGeometryDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWERVIEW_H__4EF39FBC_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
