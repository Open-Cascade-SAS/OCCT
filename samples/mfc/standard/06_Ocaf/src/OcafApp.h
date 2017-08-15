// OcafApp.h : main header file for the OCAF application
//

#if !defined(AFX_OCAF_H__49324D74_7836_11D4_8D45_00AA00D10994__INCLUDED_)
#define AFX_OCAF_H__49324D74_7836_11D4_8D45_00AA00D10994__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_App.h>
#include "OcafDoc.h"
#include <TOcaf_Application.hxx>


class COcafApp : public OCC_App
{
public:

  COcafApp();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COcafApp)
  public:
  virtual BOOL InitInstance();
  //}}AFX_VIRTUAL
  void OnFileOpen();
public:
  Handle(TOcaf_Application) GetApp() {return myApp;}

private :
  BOOL IsViewExisting(CDocument* pDoc,CRuntimeClass* pViewClass,CView*& pView);

private:

  Handle(TOcaf_Application) myApp;
  CMultiDocTemplate* pDocTemplateForView3d;

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_OCAF_H__49324D74_7836_11D4_8D45_00AA00D10994__INCLUDED_)
