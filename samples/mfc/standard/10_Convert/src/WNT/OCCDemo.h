// OCCDemo.h : main header file for the OCCDEMO application
//

#if !defined(AFX_OCCDEMO_H__57A6A228_7B4F_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_OCCDEMO_H__57A6A228_7B4F_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include <OCC_App.h>

/////////////////////////////////////////////////////////////////////////////
// COCCDemoApp:
// See OCCDemo.cpp for the implementation of this class
//

class COCCDemoApp : public OCC_App
{
public:

  COCCDemoApp();

  Handle(Graphic3d_GraphicDriver) GetGraphicDriver() const { return myGraphicDriver; }

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COCCDemoApp)
  public:
  virtual BOOL InitInstance();
  //}}AFX_VIRTUAL

// Implementation
  //{{AFX_MSG(COCCDemoApp)
  //afx_msg void OnAppAbout();
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:

  Handle(Graphic3d_GraphicDriver) myGraphicDriver;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OCCDEMO_H__57A6A228_7B4F_11D5_BA4A_0060B0EE18EA__INCLUDED_)
