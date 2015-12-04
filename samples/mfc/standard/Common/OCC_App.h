// OCC_BaseApp.h: interface for the OCC_BaseApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_APP_H__425A6447_38ED_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_APP_H__425A6447_38ED_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "res\OCC_Resource.h"
#include <Standard_Macro.hxx>
#include <Graphic3d_GraphicDriver.hxx>

class AFX_EXT_CLASS OCC_App : public CWinApp 
{
public:
  void SetSampleName (const wchar_t* theName);
  virtual const wchar_t* GetSampleName() const;
	const wchar_t* GetInitDataDir() const;
  void SetSamplePath (const wchar_t* thePath);
	OCC_App();
  Handle(Graphic3d_GraphicDriver) GetGraphicDriver() const
  {
    return myGraphicDriver;
  }

// Implementation
  //{{AFX_MSG(OCC_BaseApp)
  afx_msg void OnAppAbout();
  afx_msg void OnStereo();
  afx_msg void OnUpdateStereo (CCmdUI* theCmdUI);
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
protected:
  Handle(Graphic3d_GraphicDriver) myGraphicDriver;
public:
	CString SampleName;
	CString SamplePath;
};

#endif // !defined(AFX_OCC_BASEAPP_H__425A6447_38ED_11D7_8611_0060B0EE281E__INCLUDED_)
