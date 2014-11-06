// ImportExportApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "ImportExportApp.h"

#include "OCC_MainFrame.h"
#include "OCC_3dChildFrame.h"
#include "ImportExportDoc.h"
#include <OCC_3dView.h>
#include <res/resource.h>

BEGIN_MESSAGE_MAP(CImportExportApp, OCC_App)
	//{{AFX_MSG_MAP(CSerializeApp)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportExportApp construction

CImportExportApp::CImportExportApp() : OCC_App()
{
  // Set the local system units
  try
  {
    UnitsAPI::SetLocalSystem (UnitsAPI_MDTV);
  }
  catch (Standard_Failure)
  {
    AfxMessageBox (L"Fatal Error in units initialisation");
  }

  SampleName = "ImportExport"; //for about dialog
  SetSamplePath (L"..\\..\\05_ImportExport");
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CImportExportApp object

CImportExportApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CImportExportApp initialization

BOOL CImportExportApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	// Modified by CasCade :
	SetRegistryKey(_T("Local CasCade Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
    pDocTemplate = new CMultiDocTemplate(
		IDR_3DTYPE,
		RUNTIME_CLASS(CImportExportDoc),
		RUNTIME_CLASS(OCC_3dChildFrame), 
		RUNTIME_CLASS(OCC_3dView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	OCC_MainFrame* pMainFrame = new OCC_MainFrame(with_AIS_TB);
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

CDocument* CImportExportApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	CFile cf;
	
	if (!cf.Open(lpszFileName,CFile::modeReadWrite)){ 
		AfxMessageBox (L"File not found!");
		return NULL;
	}
	cf.Close();
	return CWinApp::OpenDocumentFile(lpszFileName); 
}

void CImportExportApp::OnFileOpen() 
{
  CFileDialog dlg(TRUE,
				  NULL,
				  NULL,
				  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
				  NULL, 
				  NULL );
  

	CString initdir;
	initdir.GetEnvironmentVariable (L"CSF_OCCTDataPath");

	dlg.m_ofn.lpstrInitialDir = initdir;

	CString strFilter;
	CString strDefault;

	POSITION pos = GetFirstDocTemplatePosition();

	CDocTemplate* pTemplate = GetNextDocTemplate(pos);
	CString strFilterExt, strFilterName;
	if (pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt) &&
		!strFilterExt.IsEmpty() &&
		pTemplate->GetDocString(strFilterName, CDocTemplate::filterName) &&
		!strFilterName.IsEmpty()) {
		// add to filter
		strFilter += strFilterName;
		ASSERT(!strFilter.IsEmpty());  // must have a file type name
		strFilter += L'\0';  // next string please
		strFilter += L'*';
		strFilter += strFilterExt;
		strFilter += L'\0';  // next string please
		dlg.m_ofn.nMaxCustFilter++;		
	}
	// append the "*.*" all files filter
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
	strFilter += allFilter;
	strFilter += L'\0';   // next string please
	strFilter += L"*.*";
	strFilter += L'\0';   // last string
	dlg.m_ofn.nMaxCustFilter++;
	dlg.m_ofn.lpstrFilter = strFilter;

  if (dlg.DoModal() == IDOK) 
  {
	AfxGetApp()->OpenDocumentFile(dlg.GetPathName());
  }	
}
