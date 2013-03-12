// HLRApp.h : main header file for the HLR application
//

#if !defined(AFX_HLR_H__376C7004_0B3D_11D2_8E0A_0800369C8A03__INCLUDED_)
#define AFX_HLR_H__376C7004_0B3D_11D2_8E0A_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_App.h>
#include "HLRDoc.h"
//#include "CutAndPasteSession.h"

class CHLRApp : public OCC_App
{
public:
  CHLRApp();
  ~CHLRApp();
  virtual BOOL InitInstance();

  // =========================================
  CFrameWnd*  CreateView2D(CHLRDoc* pDoc);
  // =========================================
  CFrameWnd*  CreateView3D(CHLRDoc* pDoc);
  // =========================================

private :
  BOOL IsViewExisting(CDocument* pDoc,CRuntimeClass* pViewClass,CView*& pView);
  CMultiDocTemplate* pDocTemplateForView3d;
  CMultiDocTemplate* pDocTemplateForView2d;
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_HLR_H__376C7004_0B3D_11D2_8E0A_0800369C8A03__INCLUDED_)
