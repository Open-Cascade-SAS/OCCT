// ColorPropertyPage.cpp : implementation file
//

#include "stdafx.h"

#include "ColorPropertyPage.h"

#include "HLRApp.h"
#include "ColorNewColorCubeColorMapDialog.h"
#include "ColorNewColorRampColorMapDialog.h"
#include "Aspect_ColorMapEntry.hxx"
#include "V2d_DefaultMap.hxx"
#include "Aspect_GenericColorMap.hxx"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorPropertyPage property page

IMPLEMENT_DYNCREATE(CColorPropertyPage, CPropertyPage)

CColorPropertyPage::CColorPropertyPage()
 : CPropertyPage(CColorPropertyPage::IDD)
{
    //{{AFX_DATA_INIT(CColorPropertyPage)
	m_ColorMapSize        = _T("Computing...");
	m_ColorMapType        = _T("Computing...");
	m_CurrentEntryRed     = _T("9.999");
	m_CurrentEntryGreen   = _T("9.999");
	m_CurrentEntryBlue    = _T("9.999");
	m_NearsetColorName    = _T("Computing...");
	m_NewEntryRed         = 9.999;
	m_NewEntryBlue        = 9.999;
	m_NewEntryGreen       = 9.999;
	//}}AFX_DATA_INIT
}

BOOL CColorPropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
    for (int i = 0 ;i< 517 ;  i++)
    {
      Standard_CString TheColorName = Quantity_Color::StringName((Quantity_NameOfColor)i);
      // update the CComboBox to add the enumeration possibilities.
      m_NewEntryColorNameCtrl.AddString( TheColorName );
    }
    m_NewEntryColorNameCtrl.SetCurSel(1);
	// TODO: Add extra initialization here
    UpdateDisplay(1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CColorPropertyPage::~CColorPropertyPage()
{
}

void CColorPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPropertyPage)
	DDX_Control(pDX, IDC_ColorMap_COMBO_NewEntryColorName   , m_NewEntryColorNameCtrl );
   	DDX_Control(pDX, IDC_ColorMap_TAB                       , m_TabCtrl               );
	DDX_Text   (pDX, IDC_ColorMap_STATIC_Size               , m_ColorMapSize          );
	DDX_Text   (pDX, IDC_ColorMap_STATIC_Type               , m_ColorMapType          );
	DDX_Text   (pDX, IDC_ColorMap_STATIC_EntryColorRed      , m_CurrentEntryRed       );
	DDX_Text   (pDX, IDC_ColorMap_STATIC_EntryColorGreen    , m_CurrentEntryGreen     );
	DDX_Text   (pDX, IDC_ColorMap_STATIC_EntryColorBlue     , m_CurrentEntryBlue      );
	DDX_Text   (pDX, IDC_ColorMap_STATIC_EntryColorName     , m_NearsetColorName      );
	DDX_Text   (pDX, IDC_ColorMap_EDIT_NewEntryColorRed     , m_NewEntryRed           );
	DDX_Text   (pDX, IDC_ColorMap_EDIT_NewEntryColorBlue    , m_NewEntryBlue          );
	DDX_Text   (pDX, IDC_ColorMap_EDIT_NewEntryColorGreen   , m_NewEntryGreen         );
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CColorPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CColorPropertyPage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_ColorMap_TAB, OnSelchangeColorMapTAB)
	ON_CBN_SELCHANGE(IDC_ColorMap_COMBO_NewEntryColorName, OnSelchangeColorMapCOMBONewEntryColorName)
	ON_BN_CLICKED(IDC_ColorMap_BUTTON_UpdateCurrentEntry, OnColorMapBUTTONUpdateCurrentEntry)
	ON_BN_CLICKED(IDC_ColorMap_BUTTON_NewColorCubeColorMap, OnColorMapBUTTONNewColorCubeColorMap)
	ON_BN_CLICKED(IDC_ColorMap_BUTTON_NewGenericColorMap, OnColorMapBUTTONNewGenericColorMap)
	ON_BN_CLICKED(IDC_ColorMap_BUTTON_NewColorRampColorMap, OnColorMapBUTTONNewColorRampColorMap)
	ON_BN_CLICKED(IDC_ColorMap_BUTTON_NewEntry_EditColor, OnColorMapBUTTONNewEntryEditColor)
	ON_BN_CLICKED(IDC_ColorMap_BUTTON_AddNewEntry, OnColorMapBUTTONAddNewEntry)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CColorPropertyPage::UpdateDisplay(int CurrentSelectionIndex)
{
    Aspect_TypeOfColorMap aColorMapType = myColorMap->Type();
    switch (aColorMapType) {
      case Aspect_TOC_Generic   : m_ColorMapType = _T("Generic");  ; break;
      case Aspect_TOC_ColorCube : m_ColorMapType = _T("ColorCube"); break;
      case Aspect_TOC_ColorRamp : m_ColorMapType = _T("ColorRamp"); break;
      default : cout<<" Underknown";
    }

    int ShowOrHide;
    if (aColorMapType == Aspect_TOC_Generic)
    {
      ShowOrHide = SW_SHOW;
      GetDlgItem(IDC_ColorMap_STATIC_NewEntryNotavailable)->ShowWindow(SW_HIDE);
    }
    else
    {  
      ShowOrHide = SW_HIDE;
      GetDlgItem(IDC_ColorMap_STATIC_NewEntryNotavailable)->ShowWindow(SW_SHOW);
    }

    GetDlgItem(IDC_ColorMap_EDIT_NewEntryColorRed)->ShowWindow(ShowOrHide);
    GetDlgItem(IDC_ColorMap_EDIT_NewEntryColorGreen)->ShowWindow(ShowOrHide);
    GetDlgItem(IDC_ColorMap_EDIT_NewEntryColorBlue)->ShowWindow(ShowOrHide);
    GetDlgItem(IDC_ColorMap_COMBO_NewEntryColorName)->ShowWindow(ShowOrHide);
    GetDlgItem(IDC_ColorMap_BUTTON_NewEntry_EditColor)->ShowWindow(ShowOrHide);
    GetDlgItem(IDC_ColorMap_BUTTON_UpdateCurrentEntry)->ShowWindow(ShowOrHide);
    GetDlgItem(IDC_ColorMap_BUTTON_AddNewEntry)->ShowWindow(ShowOrHide);
    GetDlgItem(IDC_ColorMap_STATIC_NewEntryColorNameStatic)->ShowWindow(ShowOrHide);

    TCollection_AsciiString SizeMessage(myColorMap->Size());
    m_ColorMapSize = _T(SizeMessage.ToCString());
    // clear the Tab Ctrl
    m_TabCtrl.DeleteAllItems();

	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;

    for(int i =0;i<myColorMap->Size();i++) // not <=ColorMapSize, I've enter a CSR 
    {
      Aspect_ColorMapEntry aColorMapEntry = myColorMap->FindEntry(i);
      TCollection_AsciiString EntryNumerMessage(aColorMapEntry.Index());
      TabCtrlItem.pszText = (LPSTR) EntryNumerMessage.ToCString();
	  m_TabCtrl.InsertItem( aColorMapEntry.Index(), &TabCtrlItem );
    }
    m_TabCtrl.SetCurSel(CurrentSelectionIndex);

    // update The Current Selected entry
      Aspect_ColorMapEntry aColorMapEntry = myColorMap->FindEntry(CurrentSelectionIndex);
      if (aColorMapEntry. IsAllocated () ) 
      {
        Quantity_Color TheColor =  aColorMapEntry.Color();
        Quantity_Parameter Red,Green,Blue;
        TheColor.Values(Red,Green,Blue,Quantity_TOC_RGB);
        TCollection_AsciiString Message;
        Message = Red;	
        m_CurrentEntryRed     = _T(Message.ToCString());
        m_NewEntryRed         = Red;
        Message = Green; 
        m_CurrentEntryGreen   = _T(Message.ToCString());
        m_NewEntryGreen       = Green;
        Message = Blue;  
        m_CurrentEntryBlue    = _T(Message.ToCString());
        m_NewEntryBlue        = Blue;

        Standard_CString StringName = Quantity_Color::StringName(TheColor.Name());
        m_NearsetColorName = StringName;
        cout<<"StringName : "<<TheColor.Name()<<endl;
        m_NewEntryColorNameCtrl.SetCurSel(TheColor.Name());
       if (aColorMapType == Aspect_TOC_Generic) 
         if (m_TabCtrl.GetCurSel() == 0)
           GetDlgItem(IDC_ColorMap_BUTTON_UpdateCurrentEntry)->ShowWindow(SW_HIDE);
         else
           GetDlgItem(IDC_ColorMap_BUTTON_UpdateCurrentEntry)->ShowWindow(SW_SHOW);
      }
      else 
       {
	      m_NearsetColorName = _T("Not Allocated");       
          m_CurrentEntryRed     = _T("9.999");
          m_NewEntryRed         = 9.999;
          m_CurrentEntryGreen   =  _T("9.999");
          m_NewEntryGreen       = 9.999;
          m_CurrentEntryBlue    =  _T("9.999");
          m_NewEntryBlue        = 9.999;
          GetDlgItem(IDC_ColorMap_BUTTON_UpdateCurrentEntry)->ShowWindow(SW_HIDE);
       }

    UpdateData(false);
}
void CColorPropertyPage::OnColorMapBUTTONNewGenericColorMap() 
{
	// TODO: Add your control notification handler code here
	myColorMap = V2d_DefaultMap::ColorMap();
    SetModified(true);
    UpdateDisplay(1);	
}

void CColorPropertyPage::OnColorMapBUTTONNewColorCubeColorMap() 
{
  CColorNewColorCubeColorMapDialog aDlg(NULL);
  if (aDlg.DoModal() == IDOK)
  {
   myColorMap = aDlg.ColorMap(); 
   SetModified(true);
   UpdateDisplay(1);	
  }
}

void CColorPropertyPage::OnColorMapBUTTONNewColorRampColorMap() 
{
  CColorNewColorRampColorMapDialog aDlg(NULL);
  if (aDlg.DoModal() == IDOK)
  {
   myColorMap = aDlg.ColorMap(); 
   SetModified(true);
   UpdateDisplay(1);	
  }
}

/////////////////////////////////////////////////////////////////////////////
// CColorPropertyPage message handlers

void CColorPropertyPage::OnSelchangeColorMapTAB(NMHDR* pNMHDR, LRESULT* pResult) 
{
    UpdateDisplay(m_TabCtrl.GetCurSel());	
	*pResult = 0;
}

void CColorPropertyPage::OnSelchangeColorMapCOMBONewEntryColorName() 
{
    UpdateData(true);	
    int CurSel = m_NewEntryColorNameCtrl.GetCurSel();
    Quantity_NameOfColor SelectedNameOfColor = (Quantity_NameOfColor)CurSel;
    Quantity_Color TheSelectedEquivalentColor(SelectedNameOfColor);

    Quantity_Parameter Red,Green,Blue;
    TheSelectedEquivalentColor.Values(Red,Green,Blue,Quantity_TOC_RGB);
    m_NewEntryRed         = Red;
    m_NewEntryGreen       = Green;
    m_NewEntryBlue        = Blue; 
    UpdateData(false);	
}

void CColorPropertyPage::OnColorMapBUTTONNewEntryEditColor() 
{
	int CurrentSelectedItem = m_TabCtrl.GetCurSel();

    COLORREF m_clr ;
	m_clr = RGB(m_NewEntryRed*255,m_NewEntryGreen*255,m_NewEntryBlue*255);
	CColorDialog dlgColor(m_clr);
	if (dlgColor.DoModal() == IDOK)
	{
        SetModified(TRUE);
		m_clr = dlgColor.GetColor();
		m_NewEntryRed    = GetRValue(m_clr)/255.;
		m_NewEntryGreen  = GetGValue(m_clr)/255.;
		m_NewEntryBlue   = GetBValue(m_clr)/255.;
    }
    Quantity_Color aColor(m_NewEntryRed,m_NewEntryGreen,m_NewEntryBlue,Quantity_TOC_RGB);
    m_NewEntryColorNameCtrl.SetCurSel(aColor.Name());
    UpdateData(false);
}

void CColorPropertyPage::OnColorMapBUTTONUpdateCurrentEntry() 
{
    UpdateData(true);
    Handle(Aspect_GenericColorMap) aGenericColorMap = Handle(Aspect_GenericColorMap)::DownCast(myColorMap);
    if (aGenericColorMap.IsNull()) 
       Standard_Failure::Raise(" couldn't update a none Generic Color Map");

    Aspect_ColorMapEntry aColorMapEntry = myColorMap->FindEntry( m_TabCtrl.GetCurSel());
    Quantity_Color aColor(m_NewEntryRed,m_NewEntryGreen,m_NewEntryBlue,Quantity_TOC_RGB);
    aColorMapEntry.SetColor(aColor);
    aGenericColorMap->AddEntry(aColorMapEntry); // in fact just update
    SetModified(true);
    UpdateData(true);
    UpdateDisplay(m_TabCtrl.GetCurSel());	
}

void CColorPropertyPage::OnColorMapBUTTONAddNewEntry() 
{
   UpdateData(true);
   Quantity_Color aColor(m_NewEntryRed,m_NewEntryGreen,m_NewEntryBlue,Quantity_TOC_RGB);
    Handle(Aspect_GenericColorMap) aGenericColorMap = Handle(Aspect_GenericColorMap)::DownCast(myColorMap);
    if (aGenericColorMap.IsNull()) 
       Standard_Failure::Raise(" couldn't update a none Generic Color Map");

   int NewEntry = aGenericColorMap->AddEntry(aColor);
   SetModified(true);
   UpdateDisplay(NewEntry);
   UpdateData(false);
}

BOOL CColorPropertyPage::OnApply() 
{
    myViewer->SetColorMap(myColorMap);
    myViewer->Update();
	return CPropertyPage::OnApply();
}
