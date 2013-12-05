#if !defined(AFX_SelectionDialog_H__0307BDF3_AF53_11D1_8DAE_0800369C8A03__INCLUDED_)
#define AFX_SelectionDialog_H__0307BDF3_AF53_11D1_8DAE_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelectionDialog.h : header file
//
#include "resource.h"
#include <ISession2D/ISession2D_Shape.h>
#include "SelectionDialog.h"

class CHLRDoc;
/////////////////////////////////////////////////////////////////////////////
// CSelectionDialog dialog

class CSelectionDialog : public CDialog
{
  // Construction
public:
  // standard constructor
  CSelectionDialog (CHLRDoc* aDoc,CWnd* pParent = NULL);

  void SetTitle (const CString& aTitle);

  void OnDisplay (bool isFit);

  const Handle(ISession2D_Shape) DiplayableShape() { return myDisplayableShape; }

  // Updates in dialog view and main 2d nad 3d views shapes for which HLR presentations are going to be displayed in 2d view.
  void UpdateViews();

  // Dialog Data
  //{{AFX_DATA(CSelectionDialog)
  enum { IDD = IDD_SelectionDialog };
  int m_Algo;
  int m_DisplayMode;
  int m_NbIsos;
  BOOL m_DrawHiddenLine;
  BOOL m_HlrModeIsOn;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CSelectionDialog)
protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  //}}AFX_VIRTUAL

  // Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CSelectionDialog)
  virtual BOOL OnInitDialog();
  afx_msg void OnGetSelectedShapes();
  afx_msg void OnDisplayDefault();
  afx_msg void OnVIsoParametrics();
  afx_msg void OnVApparentContour();
  afx_msg void OnVSewingEdges();
  afx_msg void OnVsharpEdges();
  afx_msg void OnVsmoothEdges();
  afx_msg void OnHsharpEdges();
  afx_msg void OnHsmoothEdges();
  afx_msg void OnHSewingEdges();
  afx_msg void OnHIsoParametrics();
  afx_msg void OnHApparentContour();
  afx_msg void OnChangeEDITNBIsos();
  afx_msg void OnAlgo();
  afx_msg void OnPolyAlgo();
  afx_msg void OnUpdate2D();
  afx_msg void OnTopView();
  afx_msg void OnBottomView();
  afx_msg void OnLeftView();
  afx_msg void OnRightView();
  afx_msg void OnFrontView();
  afx_msg void OnBackView();
  afx_msg void OnAxoView();
  virtual void OnOK();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnDrawHiddenLine();
  afx_msg void OnHlrMode();
  afx_msg void OnPaint();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private :
  void UpdateProjector();
  void ShowHideButton(Standard_Boolean EnableButton=Standard_True);
  void Apply();
  CHLRDoc* myDoc;

  Handle(AIS_InteractiveContext) myInteractiveContext;
  Handle(V3d_Viewer) myActiveViewer;
  Handle(V3d_View)   myActiveView;
  Handle(AIS_Trihedron)          myTrihedron;
  Handle(ISession2D_Shape)       myDisplayableShape;

  Standard_Integer myPosMaxX;
  Standard_Integer myPosMinX;
  Standard_Integer myBoxX;

  Standard_Integer myPosMinY;
  Standard_Integer myPosMaxY;
  Standard_Integer myBoxY;

  Standard_Integer   myXmax;
  Standard_Integer   myYmax;

protected:
  CBitmapButton TopView;
  CBitmapButton BottomView;
  CBitmapButton LeftView;
  CBitmapButton RightView;
  CBitmapButton FrontView;
  CBitmapButton BackView;
  CBitmapButton AxoView;
  bool myIsDisplayed;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SelectionDialog_H__0307BDF3_AF53_11D1_8DAE_0800369C8A03__INCLUDED_)
