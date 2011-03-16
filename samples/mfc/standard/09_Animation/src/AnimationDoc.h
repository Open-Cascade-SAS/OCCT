// Animation.h : interface of the CAnimationDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMATIONDOC_H__79840E86_1001_11D2_B8C1_0000F87A77C1__INCLUDED_)
#define AFX_ANIMATIONDOC_H__79840E86_1001_11D2_B8C1_0000F87A77C1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "AIS_ConnectedInteractive.hxx"
#include "Geom_Transformation.hxx"


class CAnimationDoc : public CDocument
{
public:

    void DragEvent      (const Standard_Integer  x       ,
	    			     const Standard_Integer  y       ,
		    		     const Standard_Integer  TheState,
                         const Handle(V3d_View)& aView   );
    void InputEvent     (const Standard_Integer  x       ,
	    			     const Standard_Integer  y       ,
                         const Handle(V3d_View)& aView   );  
    void MoveEvent      (const Standard_Integer  x       ,
                         const Standard_Integer  y       ,
                         const Handle(V3d_View)& aView   ); 
    void ShiftMoveEvent (const Standard_Integer  x       ,
                         const Standard_Integer  y       ,
                         const Handle(V3d_View)& aView   ); 
    void ShiftDragEvent (const Standard_Integer  x       ,
	    				 const Standard_Integer  y       ,
		    			 const Standard_Integer  TheState,
                         const Handle(V3d_View)& aView   ); 
    void ShiftInputEvent(const Standard_Integer  x       ,
	    				 const Standard_Integer  y       ,
                         const Handle(V3d_View)& aView   ); 
    void Popup          (const Standard_Integer  x       ,
		    			 const Standard_Integer  y       ,
                         const Handle(V3d_View)& aView   ); 

protected: // create from serialization only
	CAnimationDoc();
	DECLARE_DYNCREATE(CAnimationDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimationDoc)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAnimationDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAnimationDoc)
	afx_msg void OnShading();
	afx_msg void OnThread();
	afx_msg void OnFileLoadgrid();
	afx_msg void OnUpdateWalkWalkthru(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public :



private:
  Handle_V3d_Viewer myViewer;
  Handle_AIS_InteractiveContext myAISContext;
public :
  	Handle_AIS_InteractiveContext& GetAISContext(){ return myAISContext; };
	Handle_V3d_Viewer GetViewer()  { return myViewer; };



private :
	Handle_AIS_Shape myAisCrankArm     ;
	Handle_AIS_Shape myAisCylinderHead ;
	Handle_AIS_Shape myAisPropeller    ;
	Handle_AIS_Shape myAisPiston       ;
	Handle_AIS_Shape myAisEngineBlock  ;

	Standard_Real     myDeviation;
	Standard_Integer  myAngle;

public :
	void OnMyTimer();
	Standard_Integer myCount;
	Standard_Integer thread;
	double m_Xmin, m_Ymin, m_Zmin, m_Xmax, m_Ymax, m_Zmax;
	BOOL m_bIsGridLoaded;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMATIONDOC_H__79840E86_1001_11D2_B8C1_0000F87A77C1__INCLUDED_)
