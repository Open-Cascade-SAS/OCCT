// ModelingApp.h : main header file for the TOPOLOGYTRANSFORMATIONS application
//

#if !defined(AFX_MODELINGAPP_H__30453388_3E75_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_MODELINGAPP_H__30453388_3E75_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_App.h>

class CModelingApp : public OCC_App
{
public:

  CModelingApp();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CModelingApp)
  public:
  virtual BOOL InitInstance();
  //}}AFX_VIRTUAL

private:

  CToolBar *m_pToolBar2;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELINGAPP_H__30453388_3E75_11D7_8611_0060B0EE281E__INCLUDED_)
