// Viewer3dApp.h : main header file for the Viewer3d application
//

#if !defined(AFX_VIEWER3DAPP_H__3045338E_3E75_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_VIEWER3DAPP_H__3045338E_3E75_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_App.h>

class CViewer3dApp : public OCC_App
{
public:

  CViewer3dApp();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CViewer3dApp)
  public:
  virtual BOOL InitInstance();
  //}}AFX_VIRTUAL

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWER3DAPP_H__3045338E_3E75_11D7_8611_0060B0EE281E__INCLUDED_)
