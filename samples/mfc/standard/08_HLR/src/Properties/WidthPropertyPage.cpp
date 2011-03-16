// WidthPropertyPage.cpp : implementation file
//

#include "stdafx.h"

#include "WidthPropertyPage.h"

#include "HLRApp.h"
#include "Aspect_WidthMapEntry.hxx"
#include "Aspect_LineStyle.hxx"

#ifdef _DEBUG
// #define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWidthPropertyPage property page

IMPLEMENT_DYNCREATE(CWidthPropertyPage, CPropertyPage)

CWidthPropertyPage::CWidthPropertyPage() : CPropertyPage(CWidthPropertyPage::IDD)
{
	//{{AFX_DATA_INIT(CWidthPropertyPage)
	m_WidthMapSize  = _T("Computing...");
    m_EntryType     = _T("Computing...");
	m_EntryWidth    = _T("Computing...");
	m_NewEntryWidth = 1;
	//}}AFX_DATA_INIT
}

CWidthPropertyPage::~CWidthPropertyPage()
{
}

void CWidthPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWidthPropertyPage)
	DDX_Text    (pDX, IDC_WidthMap_STATIC_Size,        m_WidthMapSize);
	DDX_Control (pDX, IDC_WidthMap_TAB,                m_TabCtrl);
	DDX_Text    (pDX, IDC_WidthMap_STATIC_EntryType,   m_EntryType);
	DDX_Text    (pDX, IDC_WidthMap_STATIC_EntryWidth,  m_EntryWidth);
	DDX_Control (pDX, IDC_WidthMap_COMBO_NewEntryType, m_NewEntryType);
	DDX_Control (pDX, IDC_WidthMap_EDIT_NewEntryWidth, m_NewEntryWidthControl);
	DDX_Text    (pDX, IDC_WidthMap_EDIT_NewEntryWidth, m_NewEntryWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWidthPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWidthPropertyPage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_WidthMap_TAB, OnSelchangeDialogWidthTAB)
	ON_BN_CLICKED(IDC_WidthMap_BUTTON_AddNewEntry, OnWidthMapBUTTONAddNewEntry)
	ON_CBN_SELCHANGE(IDC_WidthMap_COMBO_NewEntryType, OnSelchangeWidthMapCOMBONewEntryType)
	ON_BN_CLICKED(IDC_WidthMap_BUTTON_UpdateCurrentEntry, OnWidthMapBUTTONUpdateCurrentEntry)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWidthPropertyPage message handlers

BOOL CWidthPropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

    // update the CComboBox to add the enumeration possibilities.
    m_NewEntryType.AddString( "THIN" );
    m_NewEntryType.AddString( "MEDIUM" );
    m_NewEntryType.AddString( "THICK" );
    m_NewEntryType.AddString( "VERYTHICK" );
    m_NewEntryType.AddString( "USERDEFINED" );
    m_NewEntryType.SetCurSel(0);
	
    UpdateDisplay(1);
    UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CWidthPropertyPage::UpdateDisplay(int CurrentSelectionIndex)
{     
    // Update the size of the Width map
    TCollection_AsciiString SizeMessage(myWidthMap->Size());
    m_WidthMapSize = _T(SizeMessage.ToCString());

    // clear the Tab Ctrl
    m_TabCtrl.DeleteAllItems();
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;
    for(int i =1;i<=myWidthMap->Size();i++) 
    {
    Aspect_WidthMapEntry aWidthMapEntry = myWidthMap->Entry(i);

      TCollection_AsciiString EntryNumerMessage(aWidthMapEntry.Index());
      TabCtrlItem.pszText = (LPSTR) EntryNumerMessage.ToCString();
	  m_TabCtrl.InsertItem( aWidthMapEntry.Index(), &TabCtrlItem );
    }
    m_TabCtrl.SetCurSel(CurrentSelectionIndex-1);
    // update the current Entry informations
    Aspect_WidthMapEntry aWidthMapEntry = myWidthMap->Entry(CurrentSelectionIndex);
    
    // update the current entry 
    switch (aWidthMapEntry.Type()) {
        case Aspect_WOL_THIN        : m_EntryType   = _T("Aspect_WOL_THIN")        ; break;
        case Aspect_WOL_MEDIUM      : m_EntryType   = _T("Aspect_WOL_MEDIUM")      ; break;
        case Aspect_WOL_THICK       : m_EntryType   = _T("Aspect_WOL_THICK")       ; break;
        case Aspect_WOL_VERYTHICK   : m_EntryType   = _T("Aspect_WOL_VERYTHICK")   ; break;
        case Aspect_WOL_USERDEFINED : m_EntryType   = _T("Aspect_WOL_USERDEFINED") ; break;
        default : cout<<" Underknown";
    }

    TCollection_AsciiString WidthMessage(aWidthMapEntry.Width());
    m_EntryWidth = _T(WidthMessage.ToCString());
 
    // update the edit / new part of the 
    // also update the edit part
    m_NewEntryType.SetCurSel(aWidthMapEntry.Type());
    m_NewEntryWidth = aWidthMapEntry.Width();
    if (aWidthMapEntry.Type() == Aspect_WOL_USERDEFINED)
      m_NewEntryWidthControl.SetReadOnly( false);
    else
      m_NewEntryWidthControl.SetReadOnly( true);

    if (CurrentSelectionIndex == 1) // the item 1 is not editable ( in the map by default )
      GetDlgItem(IDC_WidthMap_BUTTON_UpdateCurrentEntry)->ShowWindow(SW_HIDE);
    else
      GetDlgItem(IDC_WidthMap_BUTTON_UpdateCurrentEntry)->ShowWindow(SW_SHOW);
}

void CWidthPropertyPage::OnSelchangeDialogWidthTAB(NMHDR* pNMHDR, LRESULT* pResult) 
{
    UpdateDisplay( m_TabCtrl.GetCurSel()+1);
    UpdateData(false);
	*pResult = 0;
}

void CWidthPropertyPage::OnSelchangeWidthMapCOMBONewEntryType() 
{
   UpdateData(true);
   // the Width Map entry change in the edit part
   if (m_NewEntryType.GetCurSel() == Aspect_WOL_USERDEFINED)
     m_NewEntryWidthControl.SetReadOnly( false);
   else
   {
      m_NewEntryWidthControl.SetReadOnly( true);
     Aspect_WidthMapEntry aTmpWidthMapEntry (99,(Aspect_WidthOfLine)m_NewEntryType.GetCurSel());
     m_NewEntryWidth = aTmpWidthMapEntry.Width();
   }
   UpdateData(false);
}

void CWidthPropertyPage::OnWidthMapBUTTONAddNewEntry() 
{
   UpdateData(true);
   int NewEntry;
   if (m_NewEntryType.GetCurSel() == Aspect_WOL_USERDEFINED)
     NewEntry =myWidthMap->AddEntry(m_NewEntryWidth);
   else
     NewEntry =myWidthMap->AddEntry( (Aspect_WidthOfLine)m_NewEntryType.GetCurSel());
   SetModified(true);
   UpdateDisplay(NewEntry);
   UpdateData(false);
}

void CWidthPropertyPage::OnWidthMapBUTTONUpdateCurrentEntry() 
{
   UpdateData(true);

   Aspect_WidthMapEntry aWidthMapEntry = myWidthMap->Entry( m_TabCtrl.GetCurSel()+1);

   if (m_NewEntryType.GetCurSel() == Aspect_WOL_USERDEFINED)
      aWidthMapEntry.SetWidth(m_NewEntryWidth );
   else
      aWidthMapEntry.SetType((Aspect_WidthOfLine)m_NewEntryType.GetCurSel());

    myWidthMap->AddEntry(aWidthMapEntry); // in fact just update
    SetModified(true);
    UpdateDisplay(m_TabCtrl.GetCurSel()+1);	
    UpdateData(false);
}

BOOL CWidthPropertyPage::OnApply() 
{
    //AfxGetMainWnd()->SendMessage(WM_USERAPPLY_InMapProperties);
    myViewer->SetWidthMap(myWidthMap);
    myViewer->Update();
	return CPropertyPage::OnApply();
}
