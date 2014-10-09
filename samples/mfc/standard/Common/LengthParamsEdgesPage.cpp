// LenghtParamsEdgesPage.cpp : implementation file
//

#include "stdafx.h"
#include "LengthParamsEdgesPage.h"
#include "DimensionDlg.h"
#include <AIS_InteractiveContext.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_LengthDimension.hxx>
#include <AIS_AngleDimension.hxx>
#include <GC_MakePlane.hxx>
// CLengthParamsEdgesPage dialog

//=======================================================================
//function : CLengthParamsEdgesPage
//purpose  :
//=======================================================================

CLengthParamsEdgesPage::CLengthParamsEdgesPage (Handle(AIS_InteractiveContext) theAISContext,
                                                bool isAngleDimension /*= false*/,
                                                CWnd* pParent /*=NULL*/)
: CDialog(CLengthParamsEdgesPage::IDD, pParent)
{
  myAISContext = theAISContext;
  myIsAngleDimension = isAngleDimension;
}

//=======================================================================
//function : ~CLengthParamsEdgesPage
//purpose  :
//=======================================================================

CLengthParamsEdgesPage::~CLengthParamsEdgesPage()
{
}

//=======================================================================
//function : DoDataExchange
//purpose  :
//=======================================================================

void CLengthParamsEdgesPage::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLengthParamsEdgesPage, CDialog)
  ON_BN_CLICKED(IDC_BUTTON1, &CLengthParamsEdgesPage::OnBnClickedEdge1Btn)
  ON_BN_CLICKED(IDC_BUTTON3, &CLengthParamsEdgesPage::OnBnClickedEdge2Btn)
END_MESSAGE_MAP()


//=======================================================================
//function : OnBnClickedEdge1Btn
//purpose  :
//=======================================================================

void CLengthParamsEdgesPage::OnBnClickedEdge1Btn()
{
  // Open local context and choose the edge for length dimensions
  if (!myAISContext->HasOpenedContext())
  {
    myAISContext->OpenLocalContext();
    myAISContext->ActivateStandardMode (TopAbs_EDGE);
    AfxMessageBox (_T("Local context was not opened. Choose the edge and press the button again"),
                     MB_ICONINFORMATION | MB_OK);
    return;
  }

  // Now it's ok, local context is opened and edge selection mode is activated
  // Check if some edge is selected
  myAISContext->LocalContext()->InitSelected();
  if (!myAISContext->LocalContext()->MoreSelected())
  {
    AfxMessageBox(_T("Choose the edge and press the button again"),
                    MB_ICONINFORMATION | MB_OK);
    return;
  }

  // Workaround for AIS_LocalContext::SelectedShape()
  myFirstEdge = TopoDS::Edge (CDimensionDlg::SelectedShape());
  //myFirstEdge =  TopoDS::Edge (myAISContext->LocalContext()->SelectedShape());

  myAISContext->LocalContext()->ClearSelected();
}

//=======================================================================
//function : OnBnClickedEdge2Btn
//purpose  :
//=======================================================================

void CLengthParamsEdgesPage::OnBnClickedEdge2Btn()
{
  myAISContext->LocalContext()->InitSelected();
  if (!myAISContext->LocalContext()->MoreSelected())
  {
    AfxMessageBox (_T("Choose the edge and press the button again"),
       MB_ICONINFORMATION | MB_OK);
    return;
  }

  // Workaround for AIS_LocalContext::SelectedShape()
  mySecondEdge = TopoDS::Edge (CDimensionDlg::SelectedShape());
  //mySecondEdge = TopoDS::Edge (myAISContext->LocalContext()->SelectedShape());

  myAISContext->LocalContext()->ClearSelected();

  // Build plane through three points
  BRepAdaptor_Curve aCurve1 (myFirstEdge);
  BRepAdaptor_Curve aCurve2 (mySecondEdge);

  gp_Pnt aP1=aCurve1.Value (0.1);
  gp_Pnt aP2=aCurve1.Value (0.9);
  gp_Pnt aP3=aCurve2.Value (0.5);

  GC_MakePlane aMkPlane (aP1,aP2,aP3);

  Handle(Geom_Plane) aPlane = aMkPlane.Value();

  CDimensionDlg *aDimDlg = (CDimensionDlg*)(GetParentOwner());

  myAISContext->CloseAllContexts();

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
    Handle(AIS_AngleDimension) anAngleDim = new AIS_AngleDimension (myFirstEdge, mySecondEdge);
    anAngleDim->SetDimensionAspect (anAspect);
    anAngleDim->DimensionAspect()->MakeUnitsDisplayed (aDimDlg->IsUnitsDisplayed());
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
    myAISContext->Display (anAngleDim);
  }
  else
  {
    Handle(AIS_LengthDimension) aLenDim = new AIS_LengthDimension (myFirstEdge, mySecondEdge, aPlane->Pln());
    aLenDim->SetDimensionAspect (anAspect);
    aLenDim->SetFlyout (aDimDlg->GetFlyout());
    if (aDimDlg->IsUnitsDisplayed())
    {
      aLenDim->SetDisplayUnits (aDimDlg->GetUnits());
    }

    myAISContext->Display (aLenDim);
  }

  myAISContext->OpenLocalContext();
  myAISContext->ActivateStandardMode (TopAbs_EDGE);
}
