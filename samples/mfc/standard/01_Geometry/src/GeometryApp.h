// GeometryApp.h : main header file for the GEOMETRY application
//

#if !defined(AFX_VIEWER_H__4EF39FB2_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
#define AFX_VIEWER_H__4EF39FB2_4EBB_11D1_8D67_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_App.h>
#include <GeometryDoc.h>

class CGeometryApp : public OCC_App
{
public:
  CGeometryApp();
  ~CGeometryApp();
  // =========================================
  CFrameWnd*  CreateView2D(CGeometryDoc* pDoc);
  // =========================================
  // =========================================

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGeometryApp)
public:
  virtual BOOL InitInstance();
  //}}AFX_VIRTUAL
private :
  BOOL IsViewExisting(CDocument* pDoc,CRuntimeClass* pViewClass,CView*& pView); 	 
  CMultiDocTemplate* pDocTemplateForView3d;
  CMultiDocTemplate* pDocTemplateForView2d;

};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_VIEWER_H__4EF39FB2_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
