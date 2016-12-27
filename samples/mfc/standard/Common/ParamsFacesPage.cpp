// ParamsFacesPage.cpp : implementation file
//

#include "stdafx.h"
#include "ParamsFacesPage.h"
#include "DimensionDlg.h"
#include <AIS_InteractiveContext.hxx>
#include <AIS_LengthDimension.hxx>
#include <AIS_AngleDimension.hxx>

// CParamsFacesPage dialog

IMPLEMENT_DYNAMIC(CParamsFacesPage, CDialog)

CParamsFacesPage::CParamsFacesPage (Handle(AIS_InteractiveContext) theAISContext,
                                    bool isAngleDimension /*= false*/,
                                    CWnd* pParent         /*=NULL*/)
 : CDialog(CParamsFacesPage::IDD, pParent),
   myAISContext (theAISContext),
   myIsAngleDimension (isAngleDimension)
{
}

CParamsFacesPage::~CParamsFacesPage()
{
}

void CParamsFacesPage::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamsFacesPage, CDialog)
  ON_BN_CLICKED(IDC_FacesBtn1, &CParamsFacesPage::OnBnClickedFacesbtn1)
  ON_BN_CLICKED(IDC_FacesBtn2, &CParamsFacesPage::OnBnClickedFacesbtn2)
END_MESSAGE_MAP()


// CParamsFacesPage message handlers

void CParamsFacesPage::OnBnClickedFacesbtn1()
{
  // Check if face is selected
  myAISContext->InitSelected();
  if (!myAISContext->MoreSelected() ||
       myAISContext->SelectedShape().ShapeType() != TopAbs_FACE)
  {
    AfxMessageBox(_T("Choose the face and press the button again"),
                    MB_ICONINFORMATION | MB_OK);
    return;
  }

  myFirstFace = TopoDS::Face (myAISContext->SelectedShape());

  myAISContext->ClearSelected (Standard_True);
}

void CParamsFacesPage::OnBnClickedFacesbtn2()
{
  const Standard_Integer aSelectionMode = AIS_Shape::SelectionMode (TopAbs_FACE);

  // Check if face is selected
  myAISContext->InitSelected();
  if (!myAISContext->MoreSelected() ||
       myAISContext->SelectedShape().ShapeType() != TopAbs_FACE)
  {
    AfxMessageBox(_T("Choose the face and press the button again"),
                    MB_ICONINFORMATION | MB_OK);
    return;
  }

  mySecondFace = TopoDS::Face (myAISContext->SelectedShape());
  myAISContext->ClearSelected (Standard_False);

  CDimensionDlg *aDimDlg = (CDimensionDlg*)(GetParentOwner());

  myAISContext->Deactivate (aSelectionMode);

  Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
  anAspect->MakeArrows3d (Standard_False);
  anAspect->MakeText3d (aDimDlg->GetTextType());
  anAspect->TextAspect()->SetHeight (aDimDlg->GetFontHeight());
  anAspect->MakeTextShaded (aDimDlg->IsText3dShaded());
  anAspect->SetCommonColor (aDimDlg->GetDimensionColor());
  anAspect->MakeUnitsDisplayed (aDimDlg->IsUnitsDisplayed());
  if (myIsAngleDimension)
  {
    // Build an angle dimension between two non-parallel edges
    Handle(AIS_AngleDimension) anAngleDim = new AIS_AngleDimension (myFirstFace, mySecondFace);
    anAngleDim->SetDimensionAspect (anAspect);

    if (aDimDlg->IsUnitsDisplayed())
    {
      anAngleDim->SetDisplayUnits (aDimDlg->GetUnits ());
      if ((anAngleDim->GetDisplayUnits().IsEqual (TCollection_AsciiString ("deg"))))
      {
        anAngleDim->DimensionAspect()->MakeUnitsDisplayed (Standard_False);
      }
      else
      {
        anAngleDim->SetDisplaySpecialSymbol (AIS_DSS_No);
      }
    }

    anAngleDim->SetFlyout (aDimDlg->GetFlyout());
    myAISContext->Display (anAngleDim, Standard_True);
  }
  else
  {
    Handle(AIS_LengthDimension) aLenDim = new AIS_LengthDimension (myFirstFace, mySecondFace);
    aLenDim->SetDimensionAspect (anAspect);

    if (aLenDim->DimensionAspect()->IsUnitsDisplayed())
    {
      aLenDim->SetFlyout (aDimDlg->GetFlyout());
      aLenDim->SetDisplayUnits (aDimDlg->GetUnits());
    }

    myAISContext->Display (aLenDim, Standard_True);
  }

  myAISContext->Activate (aSelectionMode);
}
