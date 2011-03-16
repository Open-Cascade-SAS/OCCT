// MarkPropertyPage.cpp : implementation file
//

#include "stdafx.h"

#include "MarkPropertyPage.h"

#include "HLRApp.h"

#include "Aspect_MarkMap.hxx"
#include "Aspect_MarkMapEntry.hxx"
#include "TColQuantity_Array1OfLength.hxx"
#include "TColStd_SequenceOfReal.hxx"
#include "TShort_Array1OfShortReal.hxx"
#include "TColStd_Array1OfBoolean.hxx"

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMarkPropertyPage property page

IMPLEMENT_DYNCREATE(CMarkPropertyPage, CPropertyPage)

CMarkPropertyPage::CMarkPropertyPage() : CPropertyPage(CMarkPropertyPage::IDD)
{
	//{{AFX_DATA_INIT(CMarkPropertyPage)
	m_MarkMapSize       = _T("Computing...");
	m_CurrentEntryStyle = _T("Computing...");
	m_CurrentEntryXValue = _T("Computing...");
	m_CurrentEntryYValue = _T("Computing...");
	m_CurrentEntrySValue = _T("Computing...");
	//}}AFX_DATA_INIT


}
BOOL CMarkPropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

    UpdateDisplay(1);

    UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CMarkPropertyPage::~CMarkPropertyPage()
{
}

void CMarkPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMarkPropertyPage)
	DDX_Control(pDX, IDC_MarkMap_TAB, m_TabCtrl);
	DDX_Text(pDX, IDC_MarkMap_STATIC_Size, m_MarkMapSize);
	DDX_Text(pDX, IDC_MarkMap_STATIC_CurrentEntryStyle, m_CurrentEntryStyle);
	DDX_Text(pDX, IDC_MarkMap_STATIC_CurrentEntryXValue, m_CurrentEntryXValue);
	DDX_Text(pDX, IDC_MarkMap_STATIC_CurrentEntryYValue, m_CurrentEntryYValue);
	DDX_Text(pDX, IDC_MarkMap_STATIC_CurrentEntrySValue, m_CurrentEntrySValue);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMarkPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMarkPropertyPage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MarkMap_TAB, OnSelchangeMarkMapTAB)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMarkPropertyPage message handlers
void CMarkPropertyPage::UpdateDisplay(int CurrentSelectionIndex)
{
    TCollection_AsciiString SizeMessage(myMarkMap->Size());
    m_MarkMapSize = _T(SizeMessage.ToCString());
	
    m_TabCtrl.DeleteAllItems();

	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;

    for(int i =1;i<=myMarkMap->Size();i++) 
    {
      Aspect_MarkMapEntry aMarkMapEntry = myMarkMap->Entry(i);
      TCollection_AsciiString EntryNumerMessage(aMarkMapEntry.Index());
      TabCtrlItem.pszText = (LPSTR) EntryNumerMessage.ToCString();
	  m_TabCtrl.InsertItem( aMarkMapEntry.Index(), &TabCtrlItem );
    }
    m_TabCtrl.SetCurSel(CurrentSelectionIndex-1);

    // update The Current Selected entry
    Aspect_MarkMapEntry aMarkMapEntry = myMarkMap->Entry(CurrentSelectionIndex);
    if (aMarkMapEntry. IsAllocated () ) 
      {
        Aspect_MarkerStyle aMarkerStyle =  aMarkMapEntry.Style() ;
        switch (aMarkerStyle.Type())  {
          case Aspect_TOM_POINT       : m_CurrentEntryStyle = _T("Aspect_TOM_POINT");       break;
          case Aspect_TOM_PLUS        : m_CurrentEntryStyle = _T("Aspect_TOM_PLUS");        break;
          case Aspect_TOM_STAR        : m_CurrentEntryStyle = _T("Aspect_TOM_STAR");        break;
          case Aspect_TOM_O           : m_CurrentEntryStyle = _T("Aspect_TOM_O");           break;
          case Aspect_TOM_X           : m_CurrentEntryStyle = _T("Aspect_TOM_X");           break;
          case Aspect_TOM_O_POINT     : m_CurrentEntryStyle = _T("Aspect_TOM_O_POINT");     break;
          case Aspect_TOM_O_PLUS      : m_CurrentEntryStyle = _T("Aspect_TOM_O_PLUS");      break;
          case Aspect_TOM_O_STAR      : m_CurrentEntryStyle = _T("Aspect_TOM_O_STAR");      break;
          case Aspect_TOM_O_X         : m_CurrentEntryStyle = _T("Aspect_TOM_O_X");         break;
          case Aspect_TOM_BALL        : m_CurrentEntryStyle = _T("Aspect_TOM_BALL");        break;
          case Aspect_TOM_RING1       : m_CurrentEntryStyle = _T("Aspect_TOM_RING1");       break;
          case Aspect_TOM_RING2       : m_CurrentEntryStyle = _T("Aspect_TOM_RING2");       break;
          case Aspect_TOM_RING3       : m_CurrentEntryStyle = _T("Aspect_TOM_RING3");       break;
          case Aspect_TOM_USERDEFINED : m_CurrentEntryStyle = _T("Aspect_TOM_USERDEFINED"); break;
          default : m_CurrentEntryStyle = _T("Underknown");
        }

        cout<<" aMarkerStyle  Length: "<<aMarkerStyle.Length()<<endl;

        const TShort_Array1OfShortReal& aMarkerStyleXValues = aMarkerStyle.XValues() ;
        const TShort_Array1OfShortReal& aMarkerStyleYValues= aMarkerStyle.YValues() ;
        const TColStd_Array1OfBoolean&  aMarkerStyleSValues= aMarkerStyle.SValues() ;

        TCollection_AsciiString MessageX =BuildValuesAscii(aMarkerStyleXValues);
        TCollection_AsciiString MessageY =BuildValuesAscii(aMarkerStyleYValues);
        TCollection_AsciiString MessageS =BuildValuesAscii(aMarkerStyleSValues);

        m_CurrentEntryXValue = _T(MessageX.ToCString());
	    m_CurrentEntryYValue = _T(MessageY.ToCString());
	    m_CurrentEntrySValue = _T(MessageS.ToCString());

      }
      else 
      {
	    m_CurrentEntryStyle = _T("Not Allocated");
	    m_CurrentEntryXValue = _T("");
	    m_CurrentEntryYValue = _T("");
	    m_CurrentEntrySValue = _T("");
        GetDlgItem(IDC_FontMap_BUTTON_UpdateCurrent)->ShowWindow(SW_HIDE);
      }
}

void CMarkPropertyPage::OnSelchangeMarkMapTAB(NMHDR* pNMHDR, LRESULT* pResult) 
{
    UpdateDisplay(m_TabCtrl.GetCurSel()+1);	
    UpdateData(false);
	*pResult = 0;
}

BOOL CMarkPropertyPage::OnApply() 
{
//    myViewer->SetMarkMap(myMarkMap);
//    myViewer->Update();
	return CPropertyPage::OnApply();
}

TCollection_AsciiString CMarkPropertyPage::BuildValuesAscii(const TShort_Array1OfShortReal& anArray)
{
  TCollection_AsciiString Message;
  for(int i=1;i<=anArray.Length();i++)
   {
     Message += anArray.Value(i);
     Message += "\t";
   }
  return Message;
}

TCollection_AsciiString CMarkPropertyPage::BuildValuesAscii(const TColStd_Array1OfBoolean& anArray)
{
  TCollection_AsciiString Message;
  for(int i=1;i<=anArray.Length();i++)
   {
     if (anArray(i)) Message += "True"; else Message += "False";
     Message += "\t";    
   }
  return Message;
}
