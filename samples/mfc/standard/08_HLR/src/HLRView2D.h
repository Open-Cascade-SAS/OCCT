// HLRView.h : interface of the CHLRView2D class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HLRVIEW2D_H__376C7013_0B3D_11D2_8E0A_0800369C8A03_2D_INCLUDED_)
#define AFX_HLRVIEW2D_H__376C7013_0B3D_11D2_8E0A_0800369C8A03_2D_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "HLRDoc.h"
#include <OCC_2dView.h>
#include "Resource2d/RectangularGrid.h"
#include "Resource2d/CircularGrid.h"


class CHLRView2D : public OCC_2dView
{
protected: // create from serialization only
	CHLRView2D();
	DECLARE_DYNCREATE(CHLRView2D)


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHLRView2D)
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
    //}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHLRView2D();
	CHLRDoc* GetDocument();
    virtual void DragEvent2D       (const Standard_Integer  x       ,
	    			        const Standard_Integer  y       ,
		    		        const Standard_Integer  TheState);
    virtual void InputEvent2D      (const Standard_Integer  x       ,
	    			        const Standard_Integer  y       );  
    virtual void MoveEvent2D       (const Standard_Integer  x       ,
                            const Standard_Integer  y       ); 
    virtual void MultiMoveEvent2D  (const Standard_Integer  x       ,
                            const Standard_Integer  y       ); 
    virtual void MultiDragEvent2D  (const Standard_Integer  x       ,
	    				    const Standard_Integer  y       ,
		    			    const Standard_Integer  TheState); 
    virtual void MultiInputEvent2D (const Standard_Integer  x       ,
	    				    const Standard_Integer  y       ); 
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CHLRView2D)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HLRVIEW2D_H__376C7013_0B3D_11D2_8E0A_0800369C8A03_2D_INCLUDED_)
