// ColorNewColorRampColorMapDialog.cpp : implementation file
//

#include "stdafx.h"

#include "ColorNewColorRampColorMapDialog.h"

#include "HLRApp.h"
#include "Quantity_Color.hxx"
#include "Aspect_ColorRampColorMap.hxx"

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorNewColorRampColorMapDialog dialog

CColorNewColorRampColorMapDialog::CColorNewColorRampColorMapDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CColorNewColorRampColorMapDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorNewColorRampColorMapDialog)
	m_base_pixel = 0;
	m_dimension = 16;
	m_Red       = 1.0;
	m_Green     = 0.0;
	m_Blue      = 0.0;
	//}}AFX_DATA_INIT
}

void CColorNewColorRampColorMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorNewColorRampColorMapDialog)
	DDX_Control(pDX, IDC_NewColorMap_COMBO_NewEntryColorName, m_ColorList);
	DDX_Text(pDX, IDC_NewColorRamp_EDIT_base_pixel, m_base_pixel);
	DDX_Text(pDX, IDC_NewColorramp_EDIT_dimension, m_dimension);
	DDX_Text(pDX, IDC_NewColorMapRamp_EDIT_ColorRed, m_Red);
	DDX_Text(pDX, IDC_NewColorMapRamp_EDIT_ColorGreen, m_Green);
	DDX_Text(pDX, IDC_NewColorMapRamp_EDIT_ColorBlue, m_Blue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorNewColorRampColorMapDialog, CDialog)
	//{{AFX_MSG_MAP(CColorNewColorRampColorMapDialog)
	ON_BN_CLICKED(IDC_NewColorMapRamp_BUTTON_EditColor, OnColorMapRampBUTTONEditColor)
	ON_EN_CHANGE(IDC_NewColorMapRamp_EDIT_ColorRed, OnChangeColorMapRampEDITColorRed)
	ON_EN_CHANGE(IDC_NewColorMapRamp_EDIT_ColorGreen, OnChangeColorMapRampEDITColorGreen)
	ON_EN_CHANGE(IDC_NewColorMapRamp_EDIT_ColorBlue, OnChangeColorMapRampEDITColorBlue)
	ON_CBN_SELCHANGE(IDC_NewColorMap_COMBO_NewEntryColorName, OnSelchangeColorMapCOMBONewEntryColorName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorNewColorRampColorMapDialog message handlers
BOOL CColorNewColorRampColorMapDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
    for (int i = 0 ;i< 517 ;  i++)
    {
      Standard_CString TheColorName = Quantity_Color::StringName((Quantity_NameOfColor)i);
      // update the CComboBox to add the enumeration possibilities.
      m_ColorList.AddString( TheColorName );
    }
    Quantity_Color aColor(m_Red,m_Green,m_Blue,Quantity_TOC_RGB);
    m_ColorList.SetCurSel(aColor.Name());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CColorNewColorRampColorMapDialog::OnColorMapRampBUTTONEditColor() 
{
    UpdateData(true);
    COLORREF m_clr ;
	m_clr = RGB(m_Red*255,m_Green*255,m_Blue*255);
	CColorDialog dlgColor(m_clr);
	if (dlgColor.DoModal() == IDOK)
	{
		m_clr = dlgColor.GetColor();
		m_Red    = GetRValue(m_clr)/255.;
		m_Green  = GetGValue(m_clr)/255.;
		m_Blue   = GetBValue(m_clr)/255.;
    }
    Quantity_Color aColor(m_Red,m_Green,m_Blue,Quantity_TOC_RGB);
    m_ColorList.SetCurSel(aColor.Name());
    UpdateData(false);
}


void CColorNewColorRampColorMapDialog::OnChangeColorMapRampEDITColorRed() 
{
    UpdateData(true);
    if (m_Red<0) m_Red=0;
    if (m_Red>1) m_Red=1;
    
    Quantity_Color aColor(m_Red,m_Green,m_Blue,Quantity_TOC_RGB);
    m_ColorList.SetCurSel(aColor.Name());
    UpdateData(false);
}

void CColorNewColorRampColorMapDialog::OnChangeColorMapRampEDITColorGreen() 
{
    UpdateData(true);
    if (m_Green<0) m_Green=0;
    if (m_Green>1) m_Green=1;
    Quantity_Color aColor(m_Red,m_Green,m_Blue,Quantity_TOC_RGB);
    m_ColorList.SetCurSel(aColor.Name());
    UpdateData(false);	
}
void CColorNewColorRampColorMapDialog::OnChangeColorMapRampEDITColorBlue() 
{
    UpdateData(true);
    if (m_Blue<0) m_Blue=0;
    if (m_Blue>1) m_Blue=1;
    Quantity_Color aColor(m_Red,m_Green,m_Blue,Quantity_TOC_RGB);
    m_ColorList.SetCurSel(aColor.Name());
    UpdateData(false);	
}

void CColorNewColorRampColorMapDialog::OnSelchangeColorMapCOMBONewEntryColorName() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);	
    int CurSel = m_ColorList.GetCurSel();
    Quantity_NameOfColor SelectedNameOfColor = (Quantity_NameOfColor)CurSel;
    Quantity_Color TheSelectedEquivalentColor(SelectedNameOfColor);

    Quantity_Parameter Red,Green,Blue;
    TheSelectedEquivalentColor.Values(Red,Green,Blue,Quantity_TOC_RGB);
    m_Red         = Red;
    m_Green       = Green;
    m_Blue        = Blue; 
    UpdateData(false);	
}


void CColorNewColorRampColorMapDialog::OnOK() 
{
	UpdateData(true);
    Quantity_Color TheColor(m_Red,m_Green,m_Blue,Quantity_TOC_RGB);
    myColorMap = 
        new Aspect_ColorRampColorMap(m_base_pixel,
	                                 m_dimension,
                                     TheColor );
	CDialog::OnOK();
}
