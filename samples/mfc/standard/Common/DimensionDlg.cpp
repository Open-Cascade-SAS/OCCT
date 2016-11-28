// CDimensionDlg.cpp : implementation file
//

#include "stdafx.h"

#include "DimensionDlg.h"
#include "LengthParamsEdgePage.h"
#include "LengthParamsVerticesPage.h"
#include "LengthParamsEdgesPage.h"
#include "AngleParamsVerticesPage.h"
#include "RadiusParamsPage.h"
#include "ParamsFacesPage.h"
#include <Standard_Macro.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <AIS_LocalContext.hxx>
#include <Quantity_Color.hxx>

BEGIN_MESSAGE_MAP(CDimensionDlg, CDialog)
  ON_BN_CLICKED(IDOK, &CDimensionDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_DimLength, &CDimensionDlg::OnBnClickedDimLength)
  ON_BN_CLICKED(IDC_DimAngle, &CDimensionDlg::OnBnClickedDimAngle)
  ON_BN_CLICKED(IDC_DimRadius, &CDimensionDlg::OnBnClickedDimRadius)
  ON_NOTIFY(TCN_SELCHANGE, IDC_LengthTab, &CDimensionDlg::OnTcnSelChangeLengthTab)
  ON_NOTIFY(TCN_SELCHANGING, IDC_LengthTab, &CDimensionDlg::OnTcnSelChangingLengthTab)
  ON_WM_DESTROY()
  ON_NOTIFY(TCN_SELCHANGE, IDC_AngleTab, &CDimensionDlg::OnTcnSelChangeAngleTab)
  ON_NOTIFY(TCN_SELCHANGING, IDC_AngleTab, &CDimensionDlg::OnTcnSelChangingAngleTab)
  ON_BN_CLICKED(IDC_DimDiameter, &CDimensionDlg::OnBnClickedDimDiameter)
  ON_BN_CLICKED(IDC_2DText, &CDimensionDlg::OnBnClicked2dText)
  ON_BN_CLICKED(IDC_3DText, &CDimensionDlg::OnBnClicked3dText)
  ON_BN_CLICKED(IDC_DimensionColor, &CDimensionDlg::OnBnClickedDimensionColor)
  ON_WM_CLOSE()
END_MESSAGE_MAP()

//=======================================================================
//function : CDimensionDlg
//purpose  :
//=======================================================================

CDimensionDlg::CDimensionDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CDimensionDlg::IDD, pParent),
  mySelectedDimType(0),
  myFontSize (10),
  myDimensionColor (Quantity_NOC_LAWNGREEN)
{
}

//=======================================================================
//function : CDimensionDlg
//purpose  :
//=======================================================================

CDimensionDlg::CDimensionDlg (Handle(AIS_InteractiveContext) &theAISContext,
               CWnd* pParent)
: CDialog(CDimensionDlg::IDD, pParent),
  mySelectedDimType(0),
  myFontSize (10),
  myDimensionColor (Quantity_NOC_LAWNGREEN)
{
  myAISContext = theAISContext;
}

//=======================================================================
//function : ~CDimensionDlg
//purpose  :
//=======================================================================

CDimensionDlg::~CDimensionDlg()
{
}

//=======================================================================
//function : SetContext
//purpose  :
//=======================================================================

void CDimensionDlg::SetContext (const Handle(AIS_InteractiveContext) theContext)
{
  myAISContext = theContext;
}

//=======================================================================
//function : OnInitDialog
//purpose  : Initialization of dialog fields and parameters
//=======================================================================

BOOL CDimensionDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  myLengthParams = (CTabCtrl*) GetDlgItem (IDC_LengthTab);
  myAngleParams = (CTabCtrl*) GetDlgItem (IDC_AngleTab);
  myRadiusParams = (CTabCtrl*) GetDlgItem (IDC_RadiusTab);
  myDiameterParams = (CTabCtrl*) GetDlgItem (IDC_DiameterTab);

  CreateLengthParamsTab();
  CreateAngleParamsTab();
  CreateRadiusParamsTab();
  CreateDiameterParamsTab( );

  myLengthParams->ShowWindow (SW_SHOW);
  myAngleParams->ShowWindow (SW_HIDE);
  myRadiusParams->ShowWindow (SW_HIDE);
  myDiameterParams->ShowWindow (SW_HIDE);

  // Setting default values
  ((CSliderCtrl*)GetDlgItem(IDC_Flyout))->SetRange (-30,30,true);
  ((CSliderCtrl*)GetDlgItem(IDC_Flyout))->SetPos (15);
  UpdateUnitsListForLength ();
  ((CComboBox*)GetDlgItem (IDC_DisplayUnits))->SetCurSel (1);
  CheckRadioButton (IDC_2DText, IDC_3DText, IDC_2DText);
  SetTextModeControlsVisible (false);
  CComboBox* aCombo =(CComboBox* )GetDlgItem (IDC_FontSize);
  aCombo->SelectString (0, L"10");

  UpdateData (FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

//=======================================================================
//function : DoDataExchange
//purpose  : Updating of dialog data if it's needed
//=======================================================================

void CDimensionDlg::DoDataExchange (CDataExchange* pDX)
{
  CDialog::DoDataExchange (pDX);

  DDX_Radio (pDX, IDC_DimLength, mySelectedDimType);
}

//=======================================================================
//function : OnBnClickedOk
//purpose  : Reset all local contexts and close the dimension dialog
//=======================================================================

void CDimensionDlg::OnBnClickedOk()
{
 OnOK();
}

//=======================================================================
//function : GetFlyout
//purpose  : Only for length dimensions! Gets flyout value
//=======================================================================

const Standard_Real CDimensionDlg::GetFlyout() const 
{
  return ((CSliderCtrl*)GetDlgItem(IDC_Flyout))->GetPos();
}

//=======================================================================
//function : CreateLengthParamsTab
//purpose  : Fill tab control for length dimensions
//=======================================================================

void CDimensionDlg::CreateLengthParamsTab()
{
  TC_ITEM aTabItem;
  aTabItem.mask = TCIF_TEXT;
  aTabItem.pszText = L"Edge";
  myLengthParams->InsertItem (0, &aTabItem);
  aTabItem.pszText = L"Vertices";
  myLengthParams->InsertItem (1, &aTabItem);
  aTabItem.pszText = L"Parallel edges";
  myLengthParams->InsertItem (2, &aTabItem);
  aTabItem.pszText = L"Parallel faces";
  myLengthParams->InsertItem (3, &aTabItem);

  CLengthParamsEdgePage *aPage1 = new CLengthParamsEdgePage (myAISContext);
  aTabItem.mask = TCIF_PARAM;
  aTabItem.lParam = (LPARAM)aPage1;
  myLengthParams->SetItem (0, &aTabItem);
  VERIFY (aPage1->Create (CLengthParamsEdgePage::IDD,myLengthParams));
  aPage1->SetWindowPos (NULL,10,30,0,0,SWP_NOSIZE | SWP_NOZORDER);
  aPage1->ShowWindow (SW_SHOW);

  CLengthParamsVerticesPage *aPage2 = new CLengthParamsVerticesPage (myAISContext);
  aTabItem.mask = TCIF_PARAM;
  aTabItem.lParam = (LPARAM)aPage2;
  myLengthParams->SetItem (1, &aTabItem);
  VERIFY (aPage2->Create (CLengthParamsVerticesPage::IDD,myLengthParams));
  aPage2->SetWindowPos (NULL,10,30,0,0,SWP_NOSIZE | SWP_NOZORDER);
  aPage2->ShowWindow (SW_HIDE);

  CLengthParamsEdgesPage *aPage3 = new CLengthParamsEdgesPage (myAISContext);
  aTabItem.mask = TCIF_PARAM;
  aTabItem.lParam = (LPARAM)aPage3;
  myLengthParams->SetItem (2, &aTabItem);
  VERIFY (aPage3->Create (CLengthParamsEdgesPage::IDD,myLengthParams));
  aPage3->SetWindowPos (NULL,10,30,0,0,SWP_NOSIZE | SWP_NOZORDER);
  aPage3->ShowWindow (SW_HIDE);

  CParamsFacesPage *aPage4 = new CParamsFacesPage (myAISContext);
  aTabItem.mask = TCIF_PARAM;
  aTabItem.lParam = (LPARAM)aPage4;
  myLengthParams->SetItem (3, &aTabItem);
  VERIFY (aPage4->Create (CParamsFacesPage::IDD,myLengthParams));
  aPage4->SetWindowPos (NULL,10,30,0,0,SWP_NOSIZE | SWP_NOZORDER);
  aPage4->ShowWindow (SW_HIDE);
}

//=======================================================================
//function : CreateAngleParamsTab
//purpose  : Fill tab control for angle dimensions
//=======================================================================

void CDimensionDlg::CreateAngleParamsTab()
{
  TC_ITEM aTabItem;
  aTabItem.mask = TCIF_TEXT;
  aTabItem.pszText = L"Two edges";
  myAngleParams->InsertItem (0, &aTabItem);
  aTabItem.pszText = L"Three vertices";
  myAngleParams->InsertItem (1, &aTabItem);
  aTabItem.pszText = L"Two faces";
  myAngleParams->InsertItem (2, &aTabItem);

  CLengthParamsEdgesPage *aPage1 = new CLengthParamsEdgesPage (myAISContext, true);
  aTabItem.mask = TCIF_PARAM;
  aTabItem.lParam = (LPARAM)aPage1;
  myAngleParams->SetItem (0, &aTabItem);
  VERIFY (aPage1->Create (CLengthParamsEdgesPage::IDD,myAngleParams));
  aPage1->SetWindowPos (NULL,10,30,0,0,SWP_NOSIZE | SWP_NOZORDER);
  aPage1->ShowWindow (SW_SHOW);

  CAngleParamsVerticesPage *aPage2 = new CAngleParamsVerticesPage (myAISContext);
  aTabItem.mask = TCIF_PARAM;
  aTabItem.lParam = (LPARAM)aPage2;
  myAngleParams->SetItem (1, &aTabItem);
  VERIFY (aPage2->Create (CAngleParamsVerticesPage::IDD,myAngleParams));
  aPage2->SetWindowPos (NULL,10,30,0,0,SWP_NOSIZE | SWP_NOZORDER);
  aPage2->ShowWindow (SW_HIDE);

  CParamsFacesPage *aPage3 = new CParamsFacesPage (myAISContext, true);
  aTabItem.mask = TCIF_PARAM;
  aTabItem.lParam = (LPARAM)aPage3;
  myAngleParams->SetItem (2, &aTabItem);
  VERIFY (aPage3->Create (CParamsFacesPage::IDD,myAngleParams));
  aPage3->SetWindowPos (NULL,10,30,0,0,SWP_NOSIZE | SWP_NOZORDER);
  aPage3->ShowWindow (SW_HIDE);
}

//=======================================================================
//function : CreateRadiusParamsTab
//purpose  : Fill tab control for radius dimensions
//=======================================================================

void CDimensionDlg::CreateRadiusParamsTab()
{
  TC_ITEM aTabItem;
  aTabItem.mask = TCIF_TEXT;
  aTabItem.pszText = L"Circle or arc";
  myRadiusParams->InsertItem (0, &aTabItem);
  CRadiusParamsPage *aPage1 = new CRadiusParamsPage (myAISContext);
  aTabItem.mask = TCIF_PARAM;
  aTabItem.lParam = (LPARAM)aPage1;
  myRadiusParams->SetItem (0, &aTabItem);
  VERIFY (aPage1->Create (CRadiusParamsPage::IDD,myRadiusParams));
  aPage1->SetWindowPos (NULL,10,30,0,0,SWP_NOSIZE | SWP_NOZORDER);
  aPage1->ShowWindow (SW_SHOW);
}

//=======================================================================
//function : CreateDiameterParamsTab
//purpose  : Fill tab control for diameter dimensions
//=======================================================================

void CDimensionDlg::CreateDiameterParamsTab()
{
  TC_ITEM aTabItem;
  aTabItem.mask = TCIF_TEXT;
  aTabItem.pszText = L"Circle or arc";
  myDiameterParams->InsertItem (0, &aTabItem);
  CRadiusParamsPage *aPage1 = new CRadiusParamsPage (myAISContext,Standard_True);
  aTabItem.mask = TCIF_PARAM;
  aTabItem.lParam = (LPARAM)aPage1;
  myRadiusParams->SetItem (0, &aTabItem);
  VERIFY (aPage1->Create (CRadiusParamsPage::IDD,myDiameterParams));
  aPage1->SetWindowPos (NULL,10,30,0,0,SWP_NOSIZE | SWP_NOZORDER);
  aPage1->ShowWindow (SW_SHOW);
}

//=======================================================================
//function : UpdateStandardModeForAngle
//purpose  : 
//=======================================================================

void CDimensionDlg::UpdateStandardModeForAngle()
{
  int aTabNum = ((CTabCtrl*) GetDlgItem (IDC_AngleTab))->GetCurSel();
  TopAbs_ShapeEnum aMode;

  if (aTabNum == 1)
  {
    aMode = TopAbs_VERTEX;
  }
  else if (aTabNum == 2)
  {
    aMode = TopAbs_FACE;
  }
  else
  {
   aMode = TopAbs_EDGE;
  }

  myAISContext->Activate (AIS_Shape::SelectionMode (aMode));
}

//=======================================================================
//function : UpdateStandardModeForLength
//purpose  : 
//=======================================================================

void CDimensionDlg::UpdateStandardModeForLength()
{
  int aTabNum = ((CTabCtrl*) GetDlgItem (IDC_LengthTab))->GetCurSel();

  TopAbs_ShapeEnum aMode;

  if (aTabNum == 1)
  {
    aMode = TopAbs_VERTEX;
  }
  else if (aTabNum == 3)
  {
    aMode = TopAbs_FACE;
  }
  else
  {
   aMode = TopAbs_EDGE;
  }
  myAISContext->Activate (AIS_Shape::SelectionMode (aMode));
}

//=======================================================================
//function : UpdateStandardModeForLength
//purpose  : 
//=======================================================================

void CDimensionDlg::UpdateStandardMode()
{
  int aCurIndex = GetCheckedRadioButton (IDC_DimLength, IDC_DimDiameter);
  switch (aCurIndex)
  {
  case IDC_DimLength:
    UpdateStandardModeForLength();
    break;
  case IDC_DimAngle:
    UpdateStandardModeForAngle();
    break;
  case IDC_DimRadius:
  case IDC_DimDiameter:
    {
      myAISContext->Activate (AIS_Shape::SelectionMode (TopAbs_EDGE));
    }
    break;
  }
}

//=======================================================================
//function : OnBnClickedDimLength
//purpose  : it is called when <Length> radio button is chosen
//=======================================================================

void CDimensionDlg::OnBnClickedDimLength()
{
  // Update parameters
  UpdateStandardModeForLength ();
  GetDlgItem (IDC_LengthTab)->ShowWindow (SW_SHOW);
  GetDlgItem (IDC_AngleTab)->ShowWindow (SW_HIDE);
  GetDlgItem (IDC_RadiusTab)->ShowWindow (SW_HIDE);
  GetDlgItem (IDC_DiameterTab)->ShowWindow (SW_HIDE);

  UpdateUnitsListForLength ();
  ((CSliderCtrl*)GetDlgItem(IDC_Flyout))->SetPos (15);
}

//=======================================================================
//function : OnBnClickedDimAngle
//purpose  : it is called when <Angle> radio button is chosen
//=======================================================================

void CDimensionDlg::OnBnClickedDimAngle()
{
  // Update parameters
  UpdateStandardModeForAngle();
  GetDlgItem (IDC_LengthTab)->ShowWindow (SW_HIDE);
  GetDlgItem (IDC_AngleTab)->ShowWindow (SW_SHOW);
  GetDlgItem (IDC_RadiusTab)->ShowWindow (SW_HIDE);
  GetDlgItem (IDC_DiameterTab)->ShowWindow (SW_HIDE);

  UpdateUnitsListForAngle();
  ((CSliderCtrl*)GetDlgItem(IDC_Flyout))->SetPos (15);
}

//=======================================================================
//function : OnBnClickedDimDiameter
//purpose  : it is called when <Diameter> radio button is chosen
//=======================================================================

void CDimensionDlg::OnBnClickedDimDiameter()
{
  // Update parameters
  myAISContext->Activate (AIS_Shape::SelectionMode (TopAbs_EDGE));

  GetDlgItem (IDC_LengthTab)->ShowWindow (SW_HIDE);
  GetDlgItem (IDC_AngleTab)->ShowWindow (SW_HIDE);
  GetDlgItem (IDC_RadiusTab)->ShowWindow (SW_HIDE);
  GetDlgItem (IDC_DiameterTab)->ShowWindow (SW_SHOW);

  UpdateUnitsListForLength();
  ((CSliderCtrl*)GetDlgItem(IDC_Flyout))->SetPos (0);
}

//=======================================================================
//function : OnBnClickedDimRadius
//purpose  : it is called when <Radius> radio button is chosen
//=======================================================================

void CDimensionDlg::OnBnClickedDimRadius()
{
  // Update parameters
  myAISContext->Activate (AIS_Shape::SelectionMode (TopAbs_EDGE));
  GetDlgItem (IDC_LengthTab)->ShowWindow (SW_HIDE);
  GetDlgItem (IDC_AngleTab)->ShowWindow (SW_HIDE);
  GetDlgItem (IDC_RadiusTab)->ShowWindow (SW_SHOW);
  GetDlgItem (IDC_DiameterTab)->ShowWindow (SW_HIDE);

  UpdateUnitsListForLength();
  ((CSliderCtrl*)GetDlgItem(IDC_Flyout))->SetPos (0);
}

//=======================================================================
//function : OnTcnSelChangeLengthTab
//purpose  : it is called when in Length tab control current tab was changed
//=======================================================================

void CDimensionDlg::OnTcnSelChangeLengthTab (NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
  // Show this chosen tab page
  int aTabNum = ((CTabCtrl*) GetDlgItem (IDC_LengthTab))->GetCurSel();
  TC_ITEM anItem;
  anItem.mask = TCIF_PARAM;
  ((CTabCtrl*) GetDlgItem (IDC_LengthTab))->GetItem (aTabNum, &anItem);
  ASSERT(anItem.lParam);
  CWnd *aWnd = (CWnd*)anItem.lParam;
  aWnd->ShowWindow (SW_SHOW);
  UpdateStandardModeForLength();
  *pResult = 0;
}

//=======================================================================
//function : OnTcnSelChangingLengthTab
//purpose  : it is called when in Length tab control current tab
//           is changing
//           It is used to hide the current tab here to prevent collisions.
//=======================================================================

void CDimensionDlg::OnTcnSelChangingLengthTab (NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
  // Hide current tab page
  int aTabNum =  ((CTabCtrl*) GetDlgItem (IDC_LengthTab))->GetCurSel();
  TC_ITEM anItem;
  anItem.mask = TCIF_PARAM;
   ((CTabCtrl*) GetDlgItem (IDC_LengthTab))->GetItem (aTabNum, &anItem);
  ASSERT (anItem.lParam);
  CWnd *aWnd = (CWnd*)anItem.lParam;
  aWnd->ShowWindow (SW_HIDE);
  *pResult = 0;
}

//=======================================================================
//function : OnTcnSelChangeAngleTab
//purpose  : it is called when in Angle tab control current tab was changed
//=======================================================================

void CDimensionDlg::OnTcnSelChangeAngleTab (NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
  int aTabNum = ((CTabCtrl*) GetDlgItem (IDC_AngleTab))->GetCurSel();
  TC_ITEM anItem;
  anItem.mask = TCIF_PARAM;
  ((CTabCtrl*) GetDlgItem (IDC_AngleTab))->GetItem (aTabNum, &anItem);
  ASSERT (anItem.lParam);
  CWnd *aWnd = (CWnd*)anItem.lParam;
  aWnd->ShowWindow (SW_SHOW);
  UpdateStandardModeForAngle();
  *pResult = 0;
}

//=======================================================================
//function : OnTcnSelChangingAngleTab
//purpose  : it is called when in Angle tab control current tab
//           is changing
//           It is used to hide the current tab here to prevent collisions.
//=======================================================================

void CDimensionDlg::OnTcnSelChangingAngleTab (NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
  int aTabNum = ((CTabCtrl*) GetDlgItem (IDC_AngleTab))->GetCurSel();
  TC_ITEM anItem;
  anItem.mask = TCIF_PARAM;
  ((CTabCtrl*) GetDlgItem (IDC_AngleTab))->GetItem (aTabNum, &anItem);
  ASSERT (anItem.lParam);
  CWnd *aWnd = (CWnd*)anItem.lParam;
  aWnd->ShowWindow (SW_HIDE);
  *pResult = 0;
}

//=======================================================================
//function : DeactivateAllStandardModes
//purpose  : 
//=======================================================================

void CDimensionDlg::DeactivateAllStandardModes()
{
  myAISContext->Deactivate();
}

//=======================================================================
//function : OnDestroy
//purpose  : 
//=======================================================================

void CDimensionDlg::OnDestroy()
{
  CWnd *aWnd;
  TC_ITEM anItem;
  anItem.mask = TCIF_PARAM;

  // Destroy length tab
  for (int i = 3; i >= 0; --i)
  {
    ((CTabCtrl*) GetDlgItem (IDC_LengthTab))->GetItem (i, &anItem);
    ASSERT (anItem.lParam);
    aWnd  = (CWnd*) anItem.lParam;
    aWnd->DestroyWindow();
    delete aWnd;
  }
  // Destroy angle tab
  for (int i = 2; i >= 0; --i)
  {
    ((CTabCtrl*) GetDlgItem (IDC_AngleTab))->GetItem (i, &anItem);
    ASSERT(anItem.lParam);
    aWnd  = (CWnd*) anItem.lParam;
    aWnd->DestroyWindow();
    delete aWnd;
  }

  // Destroy radius tab
    ((CTabCtrl*) GetDlgItem (IDC_RadiusTab))->GetItem (0, &anItem);
    ASSERT(anItem.lParam);
    aWnd  = (CWnd*) anItem.lParam;
    aWnd->DestroyWindow();
    delete aWnd;

  // Destroy diameter tab
  ((CTabCtrl*) GetDlgItem (IDC_DiameterTab))->GetItem (0, &anItem);
  ASSERT(anItem.lParam);
  aWnd  = (CWnd*) anItem.lParam;
  aWnd->DestroyWindow();
  delete aWnd;

  CDialog::OnDestroy();
}

//=======================================================================
//function : GetTextType
//purpose  : Returns true if 3d text is to be used
//           and false in the case of 2d text.
//=======================================================================

const Standard_Boolean CDimensionDlg::GetTextType() const
{
  CButton* a3DButton = (CButton*)GetDlgItem (IDC_3DText);
  return a3DButton->GetCheck() != 0;
}

//=======================================================================
//function : GetFontHeight
//purpose  : Returns font height
//=======================================================================

const Standard_Real CDimensionDlg::GetFontHeight() const
{
  CComboBox *aComboBox = (CComboBox*)GetDlgItem (IDC_FontSize);
  CString aStr;
  aComboBox->GetWindowText (aStr);
  return _wtof (aStr);
}

//=======================================================================
//function : IsText3dShaded
//purpose  : Only for 3d text; returns true if shaded 3d text is to be used
//=======================================================================

const Standard_Boolean CDimensionDlg::IsText3dShaded() const
{
  CComboBox *aComboBox = (CComboBox*)GetDlgItem (IDC_TextDisplayMode);
  int aCurIndex = aComboBox->GetCurSel();
  return aCurIndex == 0 ? Standard_False : Standard_True;
}

//=======================================================================
//function : SetTextModeControlsVisible
//purpose  : for the dialog updating
//=======================================================================

void CDimensionDlg::SetTextModeControlsVisible (bool isVisible)
{
  GetDlgItem (IDC_TextDisplayMode)->ShowWindow (isVisible ? SW_SHOW : SW_HIDE);
  GetDlgItem (IDC_TextDisplayModeStatic)->ShowWindow (isVisible ? SW_SHOW : SW_HIDE);
}

//=======================================================================
//function : OnBnClicked2dText
//purpose  : for the dialog updating when 2d text radio button was chosen
//=======================================================================

void CDimensionDlg::OnBnClicked2dText()
{
  SetTextModeControlsVisible (false);
}

//=======================================================================
//function : OnBnClicked3dText
//purpose  : for the dialog updating when 3d text radio button was chosen
//=======================================================================

void CDimensionDlg::OnBnClicked3dText()
{
  SetTextModeControlsVisible (true);
}

//=======================================================================
//function : UpdateUnitsListForLength
//purpose  : for the dialog updating when 3d text radio button was chosen
//=======================================================================

void CDimensionDlg::UpdateUnitsListForLength()
{
  CComboBox *aCombo = (CComboBox*)GetDlgItem (IDC_DisplayUnits);
  aCombo->ResetContent();
  aCombo->AddString (L"No");
  aCombo->AddString (L"m");
  aCombo->AddString (L"mm");
  aCombo->AddString (L"in");
  aCombo->SetCurSel (1);
}

//=======================================================================
//function : UpdateUnitsListForAngle
//purpose  : for the dialog updating when 3d text radio button was chosen
//=======================================================================

void CDimensionDlg::UpdateUnitsListForAngle()
{
  CComboBox *aCombo = (CComboBox*)GetDlgItem (IDC_DisplayUnits);
  aCombo->ResetContent();
  aCombo->AddString (L"No");
  aCombo->AddString (L"deg");
  aCombo->AddString (L"rad");
  aCombo->SetCurSel (1);
}

//=======================================================================
//function : IsUnitsDisplayed
//purpose  : returns true if the units is to be displayed
//=======================================================================

const Standard_Boolean CDimensionDlg::IsUnitsDisplayed() const
{
  CString aStr;
  GetDlgItem (IDC_DisplayUnits)->GetWindowText (aStr);
  return !aStr.IsEmpty() && aStr != "No";
}

//=======================================================================
//function : GetUnits
//purpose  : returns display quantity units for current dimension
//=======================================================================

const TCollection_AsciiString CDimensionDlg::GetUnits() const
{
  if (!IsUnitsDisplayed())
    return TCollection_AsciiString();
  CString aStr;
  GetDlgItem (IDC_DisplayUnits)->GetWindowText (aStr);
  return TCollection_AsciiString ((const wchar_t* )aStr);
}

//=======================================================================
//function : OnBnClickedDimensionColor
//purpose  : returns display quantity units for current dimension
//=======================================================================

void CDimensionDlg::OnBnClickedDimensionColor()
{
  Standard_Real aR;
  Standard_Real aG;
  Standard_Real aB;
  myDimensionColor.Values (aR,aG,aB, Quantity_TOC_RGB);
  COLORREF aColor = RGB (aR*255, aG*255, aB*255);

  CColorDialog aDlgColor (aColor);
  if (aDlgColor.DoModal() == IDOK)
  {
    aColor = aDlgColor.GetColor();
    aR = GetRValue(aColor) / 255.0;
    aG = GetGValue(aColor) / 255.0;
    aB = GetBValue(aColor) / 255.0;
    myDimensionColor = Quantity_Color (aR, aG, aB, Quantity_TOC_RGB);
  }
}

//=======================================================================
//function : GetDimensionColor
//purpose  : returns current dimension color
//=======================================================================

const Quantity_Color CDimensionDlg::GetDimensionColor() const
{
  return myDimensionColor;
}

void CDimensionDlg::OnClose()
{
  CDialog::OnClose();
}
