// OCC_App.cpp: implementation of the OCC_App class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#include "OCC_App.h"
#include "OCC_BaseDoc.h"
#include <res\OCC_Resource.h>

#include <Standard_Version.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OSD.hxx>

#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// OCC_App

BEGIN_MESSAGE_MAP(OCC_App, CWinApp)
	//{{AFX_MSG_MAP(OCC_App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
  ON_COMMAND(ID_BUTTON_STEREO, &OCC_App::OnStereo)
  ON_UPDATE_COMMAND_UI(ID_BUTTON_STEREO, &OCC_App::OnUpdateStereo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OCC_App construction

OCC_App::OCC_App() : CWinApp()
{
  OSD::SetSignal (Standard_True);
  SampleName = "";
  SetSamplePath (NULL);
  try
  {
    Handle(Aspect_DisplayConnection) aDisplayConnection;
    myGraphicDriver = new OpenGl_GraphicDriver (aDisplayConnection);
  }
  catch(Standard_Failure)
  {
    AfxMessageBox (L"Fatal error during graphic initialization", MB_ICONSTOP);
    ExitProcess (1);
  }
}

void OCC_App::SetSamplePath(LPCTSTR aPath)
{
  wchar_t anAbsoluteExecutableFileName[MAX_PATH + 1];
  HMODULE hModule = GetModuleHandleW (NULL);
  GetModuleFileNameW (hModule, anAbsoluteExecutableFileName, MAX_PATH);

  SamplePath = CString (anAbsoluteExecutableFileName);
  int index = SamplePath.ReverseFind('\\');
  SamplePath.Delete(index+1, SamplePath.GetLength() - index - 1);
  if (aPath == NULL)
    SamplePath += "..";
  else{
    CString aCInitialDir(aPath);
    //SamplePath += "..\\" + aCInitialDir;
  }
}
/////////////////////////////////////////////////////////////////////////////
// CAboutDlgStd dialog used for App About

class CAboutDlgStd : public CDialog
{
public:
  CAboutDlgStd();
  BOOL OnInitDialog();

  // Dialog Data
  //{{AFX_DATA(CAboutDlgStd)
  enum { IDD = IDD_OCC_ABOUTBOX };
  //}}AFX_DATA

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAboutDlgStd)
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL


// Implementation
protected:
  //{{AFX_MSG(CAboutDlgStd)
  // No message handlers
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

public:
  CString ReadmeText;
};

CAboutDlgStd::CAboutDlgStd() : CDialog(CAboutDlgStd::IDD)
, ReadmeText(_T(""))
{
  //{{AFX_DATA_INIT(CAboutDlgStd)
  //}}AFX_DATA_INIT
}

void CAboutDlgStd::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAboutDlgStd)
  //}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_README, ReadmeText);
}

BEGIN_MESSAGE_MAP(CAboutDlgStd, CDialog)
  //{{AFX_MSG_MAP(CAboutDlgStd)
  // No message handlers
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlgStd::OnInitDialog(){
  CWnd* Title = GetDlgItem(IDC_ABOUTBOX_TITLE);

  CString About = "About ";
  CString Sample = "Sample ";
  CString SampleName = ((OCC_App*)AfxGetApp())->GetSampleName();
  CString Cascade = ", Open CASCADE Technology ";
  CString Version = OCC_VERSION_STRING;

  CString strTitle = Sample + SampleName + Cascade + Version;
  CString dlgTitle = About + SampleName;

  Title->SetWindowText(strTitle);
  SetWindowText(dlgTitle);

  CWnd* aReadmeEdit = GetDlgItem(IDC_README);
  CFile aFile;
  CString aHelpFilePath = CString (((OCC_App*)AfxGetApp())->GetInitDataDir()) + L"\\README.txt";
  if(aFile.Open (aHelpFilePath, CFile::modeRead))
  {
    aReadmeEdit->ShowWindow(TRUE);
    UINT aFileLength = (UINT)aFile.GetLength();
    char* buffer = new char[aFileLength];
    aFile.Read(buffer,aFileLength);
    ReadmeText = buffer;
    delete[] buffer;
    ReadmeText.Replace (L"\n", L"\r\n");
    UpdateData (FALSE);
  }
  else
  {
    aReadmeEdit->ShowWindow(FALSE);
  }

  CenterWindow();
  return TRUE;
}

// App command to run the dialog
void OCC_App::OnAppAbout()
{
  CAboutDlgStd aboutDlg;
  aboutDlg.DoModal();
}

const wchar_t* OCC_App::GetSampleName() const
{
  return (const wchar_t* )SampleName;
}

const wchar_t* OCC_App::GetInitDataDir() const
{
  return (const wchar_t* )SamplePath;
}

void OCC_App::SetSampleName (const wchar_t* theName)
{
  SampleName = theName;
}

//=============================================================================
// function: OnStereo
// purpose:
//=============================================================================
void OCC_App::OnStereo()
{
  Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (myGraphicDriver);

  int anAnswer = MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd,
    L"It is required to switch OpenGl context to turn on / off hardware stereo support. "
    L"The document views need to be re-created to change \"GL\" context pixel format. "
    L"This will close all current views and open new one (the model will be kept).\n"
    L"Do you want to continue?", L"Enable/disable hardware stereo support", MB_OKCANCEL | MB_ICONQUESTION);
  if (anAnswer != IDOK)
  {
    return;
  }

  Standard_Boolean& aStereoMode = aDriver->ChangeOptions().contextStereo;

  aStereoMode = !aStereoMode;

  // reset document views
  POSITION aTemplateIt = GetFirstDocTemplatePosition();

  while (aTemplateIt != NULL)
  {
    CDocTemplate* aTemplate = (CDocTemplate*)GetNextDocTemplate (aTemplateIt);

    POSITION aDocumentIt = aTemplate->GetFirstDocPosition();

    while (aDocumentIt != NULL)
    {
      OCC_BaseDoc* aDocument = dynamic_cast<OCC_BaseDoc*> (aTemplate->GetNextDoc (aDocumentIt));
      if (aDocument == NULL)
        continue;

      aDocument->ResetDocumentViews (aTemplate);
    }
  }
}

//=============================================================================
// function: OnUpdateStereo
// purpose:
//=============================================================================
void OCC_App::OnUpdateStereo (CCmdUI* theCmdUI)
{
  Handle(OpenGl_GraphicDriver) aDriver =
    Handle(OpenGl_GraphicDriver)::DownCast (myGraphicDriver);

  theCmdUI->SetCheck (!aDriver.IsNull() && aDriver->Options().contextStereo);
}
