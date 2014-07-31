// ResultDialog.cpp : implementation file
//

#include "stdafx.h"
#include "OCCDemo.h"
#include "ResultDialog.h"
#include "MainFrm.h"
#include "OCCDemoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// minimum distance between buttons, button and dialog border
#define d 5

/////////////////////////////////////////////////////////////////////////////
// CResultDialog dialog


CResultDialog::CResultDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CResultDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResultDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CResultDialog::Empty()
{
  CRichEditCtrl *pEd = (CRichEditCtrl *) GetDlgItem (IDC_RICHEDIT_ResultDialog);
  pEd->Clear();
}

void CResultDialog::SetText(LPCSTR aText)
{
  CRichEditCtrl *pEd = (CRichEditCtrl *) GetDlgItem (IDC_RICHEDIT_ResultDialog);
	CString aCStr(aText);
  pEd->SetWindowText(aCStr);
}

void CResultDialog::SetTitle(LPCSTR aTitle)
{
  CString aCStr(aTitle);
  SetWindowText(aCStr);
}

void CResultDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResultDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResultDialog, CDialog)
	//{{AFX_MSG_MAP(CResultDialog)
	ON_BN_CLICKED(IDC_CopySelectionToClipboard, OnCopySelectionToClipboard)
	ON_BN_CLICKED(IDC_CopyAllToClipboard, OnCopyAllToClipboard)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultDialog message handlers

void CResultDialog::OnCopySelectionToClipboard() 
{
	CRichEditCtrl *pEd = (CRichEditCtrl *) GetDlgItem (IDC_RICHEDIT_ResultDialog);
	pEd->Copy( );
}

void CResultDialog::OnCopyAllToClipboard() 
{
  CRichEditCtrl *pEd = (CRichEditCtrl *) GetDlgItem (IDC_RICHEDIT_ResultDialog);
  long nStartChar, nEndChar;
  pEd->GetSel (nStartChar, nEndChar);
  pEd->SetSel (0, -1);
  pEd->Copy();
  pEd->SetSel (nStartChar, nEndChar);
}

void CResultDialog::OnCancel() 
{
  CMainFrame* cFrame = (CMainFrame*) AfxGetApp()->GetMainWnd();
  COCCDemoDoc* aDoc = (COCCDemoDoc*) cFrame->GetActiveDocument();
  aDoc->IsShowResult() = FALSE;
	
	CDialog::OnCancel();
}

void CResultDialog::Initialize()
{
  // Set Fixed Width Font for rich edit control
  CRichEditCtrl *pEd = (CRichEditCtrl *) GetDlgItem (IDC_RICHEDIT_ResultDialog);
  CFont aFixedWidthFont;
  VERIFY(aFixedWidthFont.CreateFont(   
    15,                        // nHeight
    5,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_NORMAL,                 // nWeight   
    FALSE,                     // bItalic
    FALSE,                     // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    FF_MODERN,                 // Fix width fonts are FF_MODERN
    L"Courier New"));          // lpszFacename
  pEd->SetFont(&aFixedWidthFont, false);

  // save button's width and height, so they are calculated only once
  RECT rect;
  CWnd *pBCopy = (CWnd *) GetDlgItem (IDC_CopySelectionToClipboard);
  pBCopy->GetWindowRect(&rect);
  bw = rect.right - rect.left; // a button's width (they are all equal)
  bh = rect.bottom - rect.top; // a button's height
}


// OnSize is a message handler of WM_SIZE messge, 
// it is reimplemented in order to reposition the buttons
// (keep them always in the bottom part of the dialog)
// and resize the rich edit controls as user resized THIS dialog.
void CResultDialog::OnSize(UINT nType, int cx, int cy) 
{ 
  // call the base class handler
  CDialog::OnSize(nType, cx, cy);
  
  //resizes internal GUI controls (rich edit control and 3 buttons)
  //according to given values of this dialog width (cx) and height (cy).
  //the method is called from OnSize message handler and 
  //from InitInstance of OCCDemo to do initial resize
  CWnd *pEd = (CWnd *) GetDlgItem (IDC_RICHEDIT_ResultDialog);
  CWnd *pBCopy = (CWnd *) GetDlgItem (IDC_CopySelectionToClipboard);
  CWnd *pBCopyAll = (CWnd *) GetDlgItem (IDC_CopyAllToClipboard);
  CWnd *pBClose = (CWnd *) GetDlgItem (IDCANCEL);

  if (pEd != NULL) // it is NULL when dialog is being created and OnSize is called
  {
    int butY = cy-bh-d; // y coordinate of buttons' top left corner

    pEd->SetWindowPos(&wndTop, 0, 0, cx, butY-4*d, SWP_NOMOVE);
    pBCopy->SetWindowPos(&wndTop, d, butY, 0, 0, SWP_NOSIZE); 
    pBCopyAll->SetWindowPos(&wndTop, d+bw+d, butY, 0, 0, SWP_NOSIZE);
    pBClose->SetWindowPos(&wndTop, cx-bw-d, butY, 0, 0, SWP_NOSIZE);
  }

  RedrawWindow();
}

LRESULT CResultDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
  if (message != WM_SIZING)
    return CDialog::WindowProc(message, wParam, lParam);

  LPRECT pRect = (LPRECT) lParam;
  int cx = pRect->right - pRect->left;
  int cy = pRect->bottom - pRect->top;

  int minCx = 3*bw+5*d;
  int minCy = 2*d+bh+200;

  if (cx < minCx || cy < minCy)
  {
    switch (wParam)
    {
    case WMSZ_BOTTOM:
      pRect->bottom = pRect->top + minCy;
      break;
    case WMSZ_TOP:
      pRect->top = pRect->bottom - minCy;
      break;
    case WMSZ_LEFT:
      pRect->left = pRect->right - minCx;
      break;
    case WMSZ_RIGHT:
      pRect->right = pRect->left + minCx;
      break;
    case WMSZ_TOPLEFT:
      if (cx < minCx) pRect->left = pRect->right - minCx;
      if (cy < minCy) pRect->top = pRect->bottom - minCy;
      break;
    case WMSZ_BOTTOMRIGHT:
      if (cx < minCx) pRect->right = pRect->left + minCx;
      if (cy < minCy) pRect->bottom = pRect->top + minCy;
      break;
    case WMSZ_TOPRIGHT:
      if (cx < minCx) pRect->right = pRect->left + minCx;
      if (cy < minCy) pRect->top = pRect->bottom - minCy;
      break;
    case WMSZ_BOTTOMLEFT:
      if (cx < minCx) pRect->left = pRect->right - minCx;
      if (cy < minCy) pRect->bottom = pRect->top + minCy;
      break;
    }
  }

  return CDialog::WindowProc(message, wParam, lParam);
}

