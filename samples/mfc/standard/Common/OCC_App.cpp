// OCC_App.cpp: implementation of the OCC_App class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#include "OCC_App.h"

#include <res\OCC_Resource.h>

#include <Standard_Version.hxx>
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
    myGraphicDriver = Graphic3d::InitGraphicDriver (aDisplayConnection);
  }
  catch(Standard_Failure)
  {
    AfxMessageBox ("Fatal error during graphic initialization", MB_ICONSTOP);
    ExitProcess (1);
  }
}

void OCC_App::SetSamplePath(LPCTSTR aPath)
{
  char AbsoluteExecutableFileName[MAX_PATH+1];
  HMODULE hModule = GetModuleHandle(NULL);
  GetModuleFileName(hModule, AbsoluteExecutableFileName, MAX_PATH);

  SamplePath = CString(AbsoluteExecutableFileName);
  int index = SamplePath.ReverseFind('\\');
  SamplePath.Delete(index+1, SamplePath.GetLength() - index - 1);
  if (aPath == NULL)
    SamplePath += "..";
  else{
    CString aCInitialDir(aPath);
    SamplePath += "..\\" + aCInitialDir;
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
  if(SampleName.Find("Viewer2d")==-1)
  {
    CWnd* aReadmeEdit = GetDlgItem(IDC_README);
    aReadmeEdit->ShowWindow(FALSE);
  }
  else
  {
    CFile aFile;
    if(aFile.Open("..//..//..//README.txt",CFile::modeRead))
    {
      UINT aFileLength = (UINT)aFile.GetLength();
      char* buffer=new char[aFileLength];
      aFile.Read(buffer,aFileLength);
      ReadmeText.SetString(buffer);
      ReadmeText.SetAt(aFileLength,'\0');
      ReadmeText.Replace("\n","\r\n");
      UpdateData(FALSE);
    }
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

LPCTSTR OCC_App::GetSampleName()
{
  return SampleName;
}

LPCTSTR OCC_App::GetInitDataDir()
{
  return (LPCTSTR) SamplePath;
}

void OCC_App::SetSampleName(LPCTSTR Name)
{
  SampleName = Name;
}
