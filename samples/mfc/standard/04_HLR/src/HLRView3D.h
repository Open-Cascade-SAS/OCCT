// HLRView.h : interface of the CHLRView3D class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HLRVIEW3D_H__376C7010_0B3D_11D2_8E0A_0800369C8A03_3D_INCLUDED_)
#define AFX_HLRVIEW3D_H__376C7010_0B3D_11D2_8E0A_0800369C8A03_3D_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_3dView.h>

class CHLRView3D : public OCC_3dView
{
protected: // create from serialization only
	CHLRView3D();
	DECLARE_DYNCREATE(CHLRView3D)

// Attributes
public:
	CHLRDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHLRView3D)
	public:
	virtual void OnInitialUpdate(); // CasCade
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHLRView3D();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CHLRView3D)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
// CasCade :
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void DragEvent      (const Standard_Integer  x       ,
	    			     const Standard_Integer  y       ,
		    		     const Standard_Integer  TheState);
    void InputEvent     (const Standard_Integer  x       ,
	    			     const Standard_Integer  y       );  
    void MoveEvent      (const Standard_Integer  x       ,
                         const Standard_Integer  y       ); 
    void MultiMoveEvent (const Standard_Integer  x       ,
                         const Standard_Integer  y       ); 
    void MultiDragEvent (const Standard_Integer  x       ,
	    				 const Standard_Integer  y       ,
		    			 const Standard_Integer  TheState); 
    void MultiInputEvent(const Standard_Integer  x       ,
	    				 const Standard_Integer  y       ); 


};

#ifndef _DEBUG  // debug version in HLRView.cpp
inline CHLRDoc* CHLRView3D::GetDocument()
   { return (CHLRDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HLRVIEW3D_H__376C7010_0B3D_11D2_8E0A_0800369C8A03_3D_INCLUDED_)
