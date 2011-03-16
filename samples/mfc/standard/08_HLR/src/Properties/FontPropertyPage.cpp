// FontPropertyPage.cpp : implementation file
//

#include "stdafx.h"

#include "FontPropertyPage.h"

#include "HLRApp.h"
#include "Aspect_FontMapEntry.hxx"

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFontPropertyPage property page

IMPLEMENT_DYNCREATE(CFontPropertyPage, CPropertyPage)

CFontPropertyPage::CFontPropertyPage() : CPropertyPage(CFontPropertyPage::IDD)
{
	//{{AFX_DATA_INIT(CFontPropertyPage)
	m_FontMapSize       = _T("Computing...");
	m_CurrentEntryValue = _T("Computing...");
	m_CurrentEntryStyle = _T("Computing...");
	m_CurrentEntrySize  = _T("Computing...");
	m_CurrentEntrySlant  = _T("Computing...");
	m_NewEntryValue = _T("");
	m_NewEntrySize = 1.0;
	m_NewEntrySlant = 0.0;
    //}}AFX_DATA_INIT
}

CFontPropertyPage::~CFontPropertyPage()
{
}

BOOL CFontPropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
    m_NewEntry_Type.AddString("DEFAULT");
    m_NewEntry_Type.AddString("COURIER");
    m_NewEntry_Type.AddString("HELVETICA");
    m_NewEntry_Type.AddString("TIMES");
    m_NewEntry_Type.AddString("USERDEFINED");
    m_NewEntry_Type.SetCurSel(1);

	// TODO: Add extra initialization here
    UpdateDisplay(1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CFontPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontPropertyPage)
	DDX_Text   (pDX, IDC_FontMap_STATIC_Size              , m_FontMapSize       );
	DDX_Control(pDX, IDC_FontMap_TAB                      , m_TabCtrl           );
	DDX_Text   (pDX, IDC_FontMap_STATIC_CurrentEntryValue , m_CurrentEntryValue );
	DDX_Text   (pDX, IDC_FontMap_STATIC_CurrentEntryType  , m_CurrentEntryStyle );
	DDX_Text   (pDX, IDC_FontMap_STATIC_CurrentEntrySize  , m_CurrentEntrySize  );
	DDX_Text   (pDX, IDC_FontMap_STATIC_CurrentEntrySlant , m_CurrentEntrySlant );

	DDX_Control(pDX, IDC_FontMap_COMBO_NewEntryType       , m_NewEntry_Type     );
	DDX_Text(pDX, IDC_FontMap_EDIT_NewEntryValue, m_NewEntryValue);
	DDX_Text(pDX, IDC_FontMap_EDIT_NewEntrySize, m_NewEntrySize);
	DDX_Text(pDX, IDC_FontMap_EDIT_NewEntrySlant, m_NewEntrySlant);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFontPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFontPropertyPage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_FontMap_TAB, OnSelchangeFontMapTAB)
	ON_BN_CLICKED(IDC_FontMap_BUTTON_NewEntry_EditFont, OnFontMapBUTTONNewEntryEditFont)
	ON_BN_CLICKED(IDC_FontMap_BUTTON_UpdateCurrent, OnFontMapBUTTONUpdateCurrent)
	ON_CBN_SELCHANGE(IDC_FontMap_COMBO_NewEntryType, OnSelchangeFontMapCOMBONewEntryType)
	ON_BN_CLICKED(IDC_FontMap_BUTTON_NewEntry, OnFontMapBUTTONNewEntry)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontPropertyPage message handlers
void CFontPropertyPage::UpdateDisplay(int CurrentSelectionIndex)
{     
    // Update the size of the Width map
    TCollection_AsciiString SizeMessage(myFontMap->Size());
    m_FontMapSize = _T(SizeMessage.ToCString());

    // clear the Tab Ctrl
    m_TabCtrl.DeleteAllItems();
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;
    for(int i =1;i<=myFontMap->Size();i++) 
    {
      Aspect_FontMapEntry aFontMapEntry = myFontMap->Entry(i);
      TCollection_AsciiString EntryNumerMessage(aFontMapEntry.Index());
      TabCtrlItem.pszText = (LPSTR) EntryNumerMessage.ToCString();
	  m_TabCtrl.InsertItem( aFontMapEntry.Index(), &TabCtrlItem );
    }
    m_TabCtrl.SetCurSel(CurrentSelectionIndex-1);
    // update the current Entry informations
    Aspect_FontMapEntry aFontMapEntry = myFontMap->Entry(CurrentSelectionIndex);

    Aspect_FontStyle TheType =  aFontMapEntry.Type();
    m_CurrentEntryValue = (TheType.Value()        == NULL?" ":TheType.Value());

    TCollection_AsciiString Message;
    switch (TheType.Style())
    {
      case Aspect_TOF_DEFAULT     : Message = "Aspect_TOF_DEFAULT";     break;
      case Aspect_TOF_COURIER     : Message = "Aspect_TOF_COURIER";     break;
      case Aspect_TOF_HELVETICA   : Message = "Aspect_TOF_HELVETICA";   break;
      case Aspect_TOF_TIMES       : Message = "Aspect_TOF_TIMES";       break;
      case Aspect_TOF_USERDEFINED : Message = "Aspect_TOF_USERDEFINED"; break;
      default:   Message = "Underknown";
    }
	m_CurrentEntryStyle = _T(Message.ToCString());

    Message = TheType.Size ();
	m_CurrentEntrySize = _T(Message.ToCString());

    Message = TheType.Slant ();
	m_CurrentEntrySlant = _T(Message.ToCString());

    // update The New Entry :
    switch (TheType.Style())
    {
      case Aspect_TOF_DEFAULT     : m_NewEntry_Type.SetCurSel(0); break;
      case Aspect_TOF_COURIER     : m_NewEntry_Type.SetCurSel(1); break;
      case Aspect_TOF_HELVETICA   : m_NewEntry_Type.SetCurSel(2); break;
      case Aspect_TOF_TIMES       : m_NewEntry_Type.SetCurSel(3); break;
      case Aspect_TOF_USERDEFINED : m_NewEntry_Type.SetCurSel(4); break;
      default: ;
    }

   m_NewEntryValue = (TheType.Value()        == NULL?" ":TheType.Value());
   m_NewEntrySize = TheType.Size ();	
   m_NewEntrySlant = TheType.Slant ();
   if (TheType.Style()==Aspect_TOF_USERDEFINED) 
   {
        GetDlgItem(IDC_FontMap_EDIT_NewEntrySize)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FontMap_EDIT_NewEntrySlant)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FontMap_BUTTON_NewEntry_EditFont)->ShowWindow(SW_SHOW);
   }
   else 
   {
        GetDlgItem(IDC_FontMap_EDIT_NewEntrySize)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_FontMap_EDIT_NewEntrySlant)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_FontMap_BUTTON_NewEntry_EditFont)->ShowWindow(SW_HIDE);
   }
   UpdateData(false);
}

void CFontPropertyPage::OnSelchangeFontMapTAB(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
    UpdateDisplay( m_TabCtrl.GetCurSel()+1);
	*pResult = 0;
}

void CFontPropertyPage::OnSelchangeFontMapCOMBONewEntryType() 
{
  UpdateData(true);
  // update the New Entry Value message
  if (m_NewEntry_Type.GetCurSel() == Aspect_TOF_USERDEFINED)
  {
    // allow the user to select a font from the common CFontDialog
    GetDlgItem(IDC_FontMap_BUTTON_NewEntry_EditFont)->ShowWindow(SW_SHOW);
    m_NewEntryValue = "Please Select a Font";
    // remove the posiblility to update while a font was selected
    GetDlgItem(IDC_FontMap_BUTTON_UpdateCurrent)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_FontMap_BUTTON_NewEntry)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_FontMap_EDIT_NewEntrySize)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_FontMap_EDIT_NewEntrySlant)->ShowWindow(SW_HIDE);
  }
  else
  {
    GetDlgItem(IDC_FontMap_EDIT_NewEntrySize)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_FontMap_EDIT_NewEntrySlant)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_FontMap_BUTTON_NewEntry_EditFont)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_FontMap_BUTTON_UpdateCurrent)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_FontMap_BUTTON_NewEntry)->ShowWindow(SW_SHOW);
    Aspect_FontStyle TheType((Aspect_TypeOfFont)m_NewEntry_Type.GetCurSel(),m_NewEntrySize);
    m_NewEntryValue = (TheType.Value()        == NULL?" ":TheType.Value());
  }
  UpdateData(false);
	
}

#include "WNT_FontMapEntry.hxx"
void CFontPropertyPage::OnFontMapBUTTONNewEntryEditFont() 
{
    Handle(WNT_FontMapEntry) anEntry = new WNT_FontMapEntry((char* const)(LPCTSTR )m_NewEntryValue);
    Standard_Address anAddress = anEntry->LogFont();
	LOGFONT* lf = (LOGFONT*)(anAddress );
	CFontDialog dlg(lf);
	if (dlg.DoModal() == IDOK)
	{
      TCHAR buff[ 255 ];
      wsprintf ( buff, "%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%s",
      lf->lfHeight, lf->lfWidth, lf->lfEscapement, lf->lfOrientation, lf->lfWeight, lf->lfItalic,
      lf->lfUnderline, lf->lfStrikeOut, lf->lfCharSet, lf->lfOutPrecision, lf->lfClipPrecision, lf->lfQuality,
      lf->lfPitchAndFamily, lf->lfFaceName);

      GetDlgItem(IDC_FontMap_BUTTON_UpdateCurrent)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_FontMap_BUTTON_NewEntry)->ShowWindow(SW_SHOW);
      Aspect_FontStyle TheType(buff);
      m_NewEntryValue = (TheType.Value()        == NULL?" ":TheType.Value());
    }
  UpdateData(false);
}

void CFontPropertyPage::OnFontMapBUTTONUpdateCurrent() 
{
	// The Entry to modify :
    Aspect_FontMapEntry aFontMapEntry = myFontMap->Entry( m_TabCtrl.GetCurSel()+1);

   UpdateData(true);
   Aspect_TypeOfFont aTypeOfFont = (Aspect_TypeOfFont)m_NewEntry_Type.GetCurSel();

  if (aTypeOfFont == Aspect_TOF_USERDEFINED)
  {
    Aspect_FontStyle TheType((Standard_CString)(LPCTSTR )m_NewEntryValue);
    aFontMapEntry.SetType(TheType);
  }
  else
  {
    Aspect_FontStyle TheType((Aspect_TypeOfFont)m_NewEntry_Type.GetCurSel(),m_NewEntrySize,m_NewEntrySlant);
    aFontMapEntry.SetType(TheType);
  }
  myFontMap->AddEntry(aFontMapEntry); // in fact just update
  SetModified(true);
  UpdateDisplay(m_TabCtrl.GetCurSel()+1);	
  UpdateData(false);
}


void CFontPropertyPage::OnFontMapBUTTONNewEntry() 
{
	// The Entry to modify :
    Aspect_FontMapEntry aFontMapEntry;

   UpdateData(true);
   Aspect_TypeOfFont aTypeOfFont = (Aspect_TypeOfFont)m_NewEntry_Type.GetCurSel();

  if (aTypeOfFont == Aspect_TOF_USERDEFINED)
  {
    Aspect_FontStyle TheType((Standard_CString)(LPCTSTR )m_NewEntryValue);
    aFontMapEntry.SetType(TheType);
  }
  else
  {
    Aspect_FontStyle TheType((Aspect_TypeOfFont)m_NewEntry_Type.GetCurSel(),m_NewEntrySize,m_NewEntrySlant);
    aFontMapEntry.SetType(TheType);
  }
  myFontMap->AddEntry(aFontMapEntry); // in fact just update
  SetModified(true);
  UpdateDisplay(m_TabCtrl.GetCurSel()+1);	
  UpdateData(false);
}

BOOL CFontPropertyPage::OnApply() 
{
    myViewer->SetFontMap(myFontMap);
    myViewer->Update();

	return CPropertyPage::OnApply();
}
