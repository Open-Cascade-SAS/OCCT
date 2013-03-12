// AnimationApp.h : main header file for the Animation application
//

#if !defined(AFX_ANIMATION_H__79840E7E_1001_11D2_B8C1_0000F87A77C1__INCLUDED_)
#define AFX_ANIMATION_H__79840E7E_1001_11D2_B8C1_0000F87A77C1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_App.h>

class CAnimationApp : public OCC_App
{
public:

  CAnimationApp();

//{{AFX_MSG(CAnimationApp)
  afx_msg void OnAppAbout();
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAnimationApp)
  public:
  virtual BOOL InitInstance();
  //}}AFX_VIRTUAL
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMATION_H__79840E7E_1001_11D2_B8C1_0000F87A77C1__INCLUDED_)
