// Animation.h : interface of the CAnimationDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMATIONDOC_H__79840E86_1001_11D2_B8C1_0000F87A77C1__INCLUDED_)
#define AFX_ANIMATIONDOC_H__79840E86_1001_11D2_B8C1_0000F87A77C1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OCC_BaseDoc.h"
#include "AIS_ConnectedInteractive.hxx"
#include "Geom_Transformation.hxx"


class CAnimationDoc : public OCC_BaseDoc
{
public:

  void DragEvent (const Standard_Integer x,
                  const Standard_Integer y,
                  const Standard_Integer TheState,
                  const Handle(V3d_View)& aView);

  void InputEvent (const Standard_Integer x,
                   const Standard_Integer y,
                   const Handle(V3d_View)& aView);

  void MoveEvent (const Standard_Integer x,
                  const Standard_Integer y,
                  const Handle(V3d_View)& aView);

  void ShiftMoveEvent (const Standard_Integer x,
                       const Standard_Integer y,
                       const Handle(V3d_View)& aView);

  void ShiftDragEvent (const Standard_Integer x,
                       const Standard_Integer y,
                       const Standard_Integer TheState,
                       const Handle(V3d_View)& aView);

  void ShiftInputEvent (const Standard_Integer x,
                        const Standard_Integer y,
                        const Handle(V3d_View)& aView);

  void Popup (const Standard_Integer x,
              const Standard_Integer y,
              const Handle(V3d_View)& aView);

protected: // create from serialization only
  CAnimationDoc();
  DECLARE_DYNCREATE(CAnimationDoc)

// Implementation
public:
  virtual ~CAnimationDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
  //{{AFX_MSG(CAnimationDoc)
  afx_msg void OnShading();
  afx_msg void OnThread();
  afx_msg void OnFileLoadgrid();
  afx_msg void OnUpdateWalkWalkthru(CCmdUI* pCmdUI);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  Handle(AIS_Shape) myAisCrankArm     ;
  Handle(AIS_Shape) myAisCylinderHead ;
  Handle(AIS_Shape) myAisPropeller    ;
  Handle(AIS_Shape) myAisPiston       ;
  Handle(AIS_Shape) myAisEngineBlock  ;

  Standard_Real     myDeviation;
  Standard_Integer  myAngle;

public:
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
