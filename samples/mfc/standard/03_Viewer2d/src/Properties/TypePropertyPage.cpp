// TypePropertyPage.cpp : implementation file
//

#include "stdafx.h"

#include "TypePropertyPage.h"

#include "Aspect_TypeMap.hxx"
#include "Aspect_TypeMapEntry.hxx"
#include "TColQuantity_Array1OfLength.hxx"
#include "TColStd_SequenceOfReal.hxx"

/////////////////////////////////////////////////////////////////////////////
// CTypePropertyPage property page

IMPLEMENT_DYNCREATE(CTypePropertyPage, CPropertyPage)

CTypePropertyPage::CTypePropertyPage() : CPropertyPage(CTypePropertyPage::IDD)
{
	//{{AFX_DATA_INIT(CTypePropertyPage)
	m_TypeMapSize       = _T("Computing...");
	m_CurrentEntryStyle = _T("Computing...");
	m_CurrentEntryValue = _T("Computing...");
    m_NewEntryValue     = _T("");
	//}}AFX_DATA_INIT


}
BOOL CTypePropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

    m_NewEntryControl.AddString("SOLID");
    m_NewEntryControl.AddString("DASH");
    m_NewEntryControl.AddString("DOT");
    m_NewEntryControl.AddString("DOTDASH");
    m_NewEntryControl.AddString("USERDEFINED");
    m_NewEntryControl.SetCurSel(1);

    UpdateDisplay(1);

    UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CTypePropertyPage::~CTypePropertyPage()
{
}

void CTypePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTypePropertyPage)
	DDX_Control(pDX, IDC_TypeMap_TAB, m_TabCtrl);
	DDX_Control(pDX, IDC_TypeMap_COMBO_NewEntryStyle, m_NewEntryControl);
	DDX_Control(pDX, IDC_TypeMap_EDIT_NewEntryValue, m_NewEntryValueControl);
	DDX_Text(pDX, IDC_TypeMap_STATIC_Size, m_TypeMapSize);
	DDX_Text(pDX, IDC_TypeMap_STATIC_CurrentEntryStyle, m_CurrentEntryStyle);
	DDX_Text(pDX, IDC_TypeMap_STATIC_CurrentEntryValue, m_CurrentEntryValue);
	DDX_Text(pDX, IDC_TypeMap_EDIT_NewEntryValue, m_NewEntryValue);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTypePropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTypePropertyPage)
	ON_BN_CLICKED(IDC_FontMap_BUTTON_NewEntry, OnFontMapBUTTONNewEntry)
	ON_BN_CLICKED(IDC_FontMap_BUTTON_UpdateCurrent, OnFontMapBUTTONUpdateCurrent)
	ON_EN_CHANGE(IDC_TypeMap_EDIT_NewEntryValue, OnChangeTypeMapEDITNewEntryValue)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TypeMap_TAB, OnSelchangeTypeMapTAB)
	ON_CBN_SELCHANGE(IDC_TypeMap_COMBO_NewEntryStyle, OnSelchangeTypeMapCOMBONewEntryStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTypePropertyPage message handlers
void CTypePropertyPage::UpdateDisplay(int CurrentSelectionIndex)
{
    TCollection_AsciiString SizeMessage(myTypeMap->Size());
    m_TypeMapSize = _T(SizeMessage.ToCString());
	
    m_TabCtrl.DeleteAllItems();

	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;

    for(int i =1;i<=myTypeMap->Size();i++) 
    {
      Aspect_TypeMapEntry aTypeMapEntry = myTypeMap->Entry(i);
      TCollection_AsciiString EntryNumerMessage(aTypeMapEntry.Index());
      TabCtrlItem.pszText = (LPSTR) EntryNumerMessage.ToCString();
	  m_TabCtrl.InsertItem( aTypeMapEntry.Index(), &TabCtrlItem );
    }
    m_TabCtrl.SetCurSel(CurrentSelectionIndex-1);

    // update The Current Selected entry
    Aspect_TypeMapEntry aTypeMapEntry = myTypeMap->Entry(CurrentSelectionIndex);
    if (aTypeMapEntry. IsAllocated () ) 
      {
        Aspect_LineStyle TypeMapEntryType = aTypeMapEntry.Type() ;

        switch (TypeMapEntryType.Style()) {
          case Aspect_TOL_SOLID       :  m_CurrentEntryStyle = _T("SOLID");       break;
          case Aspect_TOL_DASH        :  m_CurrentEntryStyle = _T("DASH");        break;
          case Aspect_TOL_DOT         :  m_CurrentEntryStyle = _T("DOT");         break;
          case Aspect_TOL_DOTDASH     :  m_CurrentEntryStyle = _T("DOTDASH");     break;
          case Aspect_TOL_USERDEFINED :  m_CurrentEntryStyle = _T("USERDEFINED"); break;
          default : m_CurrentEntryStyle = _T("Underknown");
        }
        int NbValue = TypeMapEntryType.Length();
        TCollection_AsciiString Message = BuildValuesAscii(TypeMapEntryType.Values());

	    m_CurrentEntryValue = _T(Message.ToCString());

        m_NewEntryControl.SetCurSel(TypeMapEntryType.Style());
        m_NewEntryValue = _T(Message.ToCString());  
        if (TypeMapEntryType.Style() == Aspect_TOL_USERDEFINED)
          m_NewEntryValueControl.SetReadOnly( false );
        else
          m_NewEntryValueControl.SetReadOnly( true );
      }
      else 
      {
	    m_CurrentEntryStyle = _T("Not Allocated");
	    m_CurrentEntryValue = _T("");
        m_NewEntryValue     = _T("No value");  
        GetDlgItem(IDC_FontMap_BUTTON_UpdateCurrent)->ShowWindow(SW_HIDE);
      }
}

void CTypePropertyPage::OnSelchangeTypeMapTAB(NMHDR* pNMHDR, LRESULT* pResult) 
{
    UpdateDisplay(m_TabCtrl.GetCurSel()+1);	
    UpdateData(false);
	*pResult = 0;
}
void CTypePropertyPage::OnSelchangeTypeMapCOMBONewEntryStyle() 
{
   UpdateData(true);
   // the Type Map entry change in the edit part
   if (m_NewEntryControl.GetCurSel() == Aspect_TOL_USERDEFINED)
     m_NewEntryValueControl.SetReadOnly( false );
   else
   {
      m_NewEntryValueControl.SetReadOnly( true );
    
    // create a dummy map to extract the default values
      Aspect_TypeMapEntry aTypeMapEntry(99,(Aspect_TypeOfLine)m_NewEntryControl.GetCurSel());
      Aspect_LineStyle TypeMapEntryType = aTypeMapEntry.Type() ;
      TCollection_AsciiString Message = BuildValuesAscii(TypeMapEntryType.Values());

      m_NewEntryValue = _T(Message.ToCString());
   }
   UpdateData(false);
}

void CTypePropertyPage::OnChangeTypeMapEDITNewEntryValue() 
{
   UpdateData(true);
   Handle(TColQuantity_HArray1OfLength) anArray;
   Standard_Boolean IsDone = ExtractValues(TCollection_AsciiString((char *)(LPCSTR)m_NewEntryValue),anArray);
   if (IsDone)
   {
        GetDlgItem(IDC_FontMap_BUTTON_UpdateCurrent)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_FontMap_BUTTON_NewEntry)->ShowWindow(SW_SHOW);
   }
   else
   {
        GetDlgItem(IDC_FontMap_BUTTON_UpdateCurrent)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FontMap_BUTTON_NewEntry)->ShowWindow(SW_HIDE);
    }
}

void CTypePropertyPage::OnFontMapBUTTONNewEntry() 
{
   UpdateData(true);
   int NewEntry;

   if (m_NewEntryControl.GetCurSel() == Aspect_TOL_USERDEFINED)
   {
     Handle(TColQuantity_HArray1OfLength) anArray;
     if (ExtractValues(TCollection_AsciiString((char *)(LPCSTR)m_NewEntryValue),anArray))
     {
       Aspect_LineStyle aLineStyle(anArray->Array1());
      NewEntry = myTypeMap->AddEntry( aLineStyle );
     }
     else Standard_Failure::Raise(" The String is not Valid ");
   }
   else
   {
      Aspect_LineStyle aLineStyle((Aspect_TypeOfLine)m_NewEntryControl.GetCurSel());
      NewEntry = myTypeMap->AddEntry( aLineStyle );
   }

   SetModified(true);
   UpdateDisplay(NewEntry);
   UpdateData(false);
}

void CTypePropertyPage::OnFontMapBUTTONUpdateCurrent() 
{
   UpdateData(true);
   Aspect_TypeMapEntry aTypeMapEntry = myTypeMap->Entry( m_TabCtrl.GetCurSel()+1);

   if (m_NewEntryControl.GetCurSel() == Aspect_TOL_USERDEFINED)
   {
     Handle(TColQuantity_HArray1OfLength) anArray;
     if (ExtractValues(TCollection_AsciiString((char *)(LPCSTR)m_NewEntryValue),anArray))
     {
       Aspect_LineStyle aLineStyle(anArray->Array1());
       aTypeMapEntry.SetType( aLineStyle );
     }
     else Standard_Failure::Raise(" The String is not Valid ");
   }
   else
   {
     Aspect_LineStyle aLineStyle((Aspect_TypeOfLine)m_NewEntryControl.GetCurSel());
     aTypeMapEntry.SetType( aLineStyle );
   }

   myTypeMap->AddEntry(aTypeMapEntry); // in fact just update
   
   SetModified(true);
   UpdateDisplay(m_TabCtrl.GetCurSel()+1);	
   UpdateData(false);
}

BOOL CTypePropertyPage::OnApply() 
{
    myViewer->SetTypeMap(myTypeMap);
    myViewer->Update();
	return CPropertyPage::OnApply();
}

Standard_Boolean  CTypePropertyPage::ExtractValues(TCollection_AsciiString aMessage,
                                                   Handle(TColQuantity_HArray1OfLength)& anArray) // out 
{
  TColStd_SequenceOfReal aSequenceOfReal;
  Standard_Integer CurrentStartValue=1;
  bool NotFinish = true;
  while (NotFinish)
  {
    CurrentStartValue = aMessage.SearchFromEnd(";");
    if ( CurrentStartValue == aMessage.Length()) return Standard_False;
    if (CurrentStartValue != -1)
    { 
      TCollection_AsciiString aNewMessage = aMessage.Split(CurrentStartValue);
      aMessage.Remove(aMessage.Length());
      if (aNewMessage.IsRealValue())
        aSequenceOfReal.Append(aNewMessage.RealValue());
      else  return Standard_False;
    }
    else
    {
      if (aMessage.IsRealValue())
        aSequenceOfReal.Append(aMessage.RealValue());
      else  return Standard_False;
      NotFinish = false;
    }
  }

  anArray = new TColQuantity_HArray1OfLength(1,aSequenceOfReal.Length());
  for (int i=1;i<=aSequenceOfReal.Length();i++)
    anArray->SetValue(i,aSequenceOfReal(aSequenceOfReal.Length()-i+1));

  return Standard_True;
}

TCollection_AsciiString CTypePropertyPage::BuildValuesAscii(const TColQuantity_Array1OfLength& anArray)
{
  TCollection_AsciiString Message;
  int NbValue = anArray.Length();
  for (int j=1;j<NbValue;j++)
   {
     Message += anArray(j);
     Message += " ; ";
   }
  Quantity_Length Length =  anArray(NbValue);
  if (Length > Precision::Confusion()) Message += Length;
  else Message += "No value";
  return Message;
}
