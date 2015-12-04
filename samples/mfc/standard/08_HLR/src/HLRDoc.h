// HLRDoc.h : interface of the CHLRDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HLRDOC_H__376C700E_0B3D_11D2_8E0A_0800369C8A03__INCLUDED_)
#define AFX_HLRDOC_H__376C700E_0B3D_11D2_8E0A_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SelectionDialog.h"
#include <OCC_3dDoc.h>

class CHLRDoc : public OCC_3dBaseDoc
{
public:

protected: // create from serialization only
  CHLRDoc();
  DECLARE_DYNCREATE(CHLRDoc)
  // Attributes
public:

  // Implementation
public:
  virtual ~CHLRDoc();
  void Fit();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif
protected:

  // Generated message map functions
protected:
  //{{AFX_MSG(CHLRDoc)
  afx_msg void OnWindowNew3d();
  afx_msg void OnWindowNew2d();
  afx_msg void OnFileImportBrep();
  afx_msg void OnBUTTONHLRDialog();
  afx_msg void OnObjectErase();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

public :
  void ActivateFrame(CRuntimeClass* pViewClass, int nCmdShow = SW_RESTORE  );

private:
  Handle(V3d_Viewer) my2DViewer;
  Handle(AIS_InteractiveContext) myInteractiveContext2D;
public :
  Handle(V3d_Viewer) GetViewer2D()  { return my2DViewer; };
  Handle(AIS_InteractiveContext)& GetInteractiveContext2D(){ return myInteractiveContext2D; };
  void FitAll2DViews(Standard_Boolean UpdateViewer=Standard_False);

public :
  CSelectionDialog* myCSelectionDialog;
  bool myCSelectionDialogIsCreated;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HLRDOC_H__376C700E_0B3D_11D2_8E0A_0800369C8A03__INCLUDED_)
