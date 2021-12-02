// ImportExportApp.h : main header file for the IMPORTEXPORT application
//

#if !defined(AFX_IMPORTEXPORT_H__88A21474_3B23_11D2_8E1E_0800369C8A03__INCLUDED_)
#define AFX_IMPORTEXPORT_H__88A21474_3B23_11D2_8E1E_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_App.h>

class CImportExportApp : public OCC_App
{
public:

  CImportExportApp();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CImportExportApp)
  public:
  virtual BOOL InitInstance();
  virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
  //}}AFX_VIRTUAL

protected:

  //{{AFX_MSG(CSerializeApp)
  afx_msg void OnFileOpen();
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_IMPORTEXPORT_H__88A21474_3B23_11D2_8E1E_0800369C8A03__INCLUDED_)
