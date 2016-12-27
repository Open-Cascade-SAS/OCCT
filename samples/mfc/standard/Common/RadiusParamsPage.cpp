
#include "stdafx.h"
#include "RadiusParamsPage.h"
#include "DimensionDlg.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_RadiusDimension.hxx>
#include <AIS_DiameterDimension.hxx>
#include <ElCLib.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_DYNAMIC(CRadiusParamsPage, CDialog)

//=======================================================================
//function : CRadiusParamsPage
//purpose  :
//=======================================================================

CRadiusParamsPage::CRadiusParamsPage (const Handle(AIS_InteractiveContext)& theAISContext,
                                      const Standard_Boolean isDiameterDimension /* =Standard_False*/,
                                      CWnd* pParent /*=NULL*/)
 : CDialog (CRadiusParamsPage::IDD, pParent)
{
  myAISContext = theAISContext;
  myIsDiameterDimension = isDiameterDimension;
}

//=======================================================================
//function : ~CRadiusParamsPage
//purpose  :
//=======================================================================

CRadiusParamsPage::~CRadiusParamsPage()
{
}

//=======================================================================
//function : DoDataExchange
//purpose  :
//=======================================================================

void CRadiusParamsPage::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRadiusParamsPage, CDialog)
  ON_BN_CLICKED(IDC_BUTTON1, &CRadiusParamsPage::OnBnClickedObjectBtn)
END_MESSAGE_MAP()

//=======================================================================
//function : OnBnClickedObjectBtn
//purpose  :
//=======================================================================

void CRadiusParamsPage::OnBnClickedObjectBtn()
{
  //Build dimension here
  myAISContext->InitSelected();
  if (!myAISContext->MoreSelected() ||
       myAISContext->SelectedShape().ShapeType() != TopAbs_EDGE)
  {
    AfxMessageBox (_T ("Choose the edge and press the button again"), MB_ICONINFORMATION | MB_OK);
    return;
  }

  gp_Circ aCircle;
  Standard_Boolean isAttachPoint = Standard_False;
  Standard_Real aFirstPar = 0, aLastPar = 0;

  TopoDS_Shape aSelShape = myAISContext->SelectedShape();

  if (aSelShape.ShapeType() != TopAbs_EDGE &&
      aSelShape.ShapeType() != TopAbs_FACE &&
      aSelShape.ShapeType() != TopAbs_WIRE)
    return;

  if (aSelShape.ShapeType() == TopAbs_EDGE)
  {
    BRepAdaptor_Curve aCurve (TopoDS::Edge (aSelShape));
    if (aCurve.GetType() != GeomAbs_Circle)
    {
      return;
    }

    aCircle = aCurve.Circle();
    if (aCurve.FirstParameter() != 0 && aCurve.LastParameter() != M_PI * 2)
    {
      isAttachPoint = Standard_True;
      aFirstPar = aCurve.FirstParameter();
      aLastPar = aCurve.LastParameter();
    }
  }

  myAISContext->ClearSelected (Standard_False);
  CDimensionDlg *aDimDlg = (CDimensionDlg*)(this->GetParentOwner());
  // Try to create dimension if it is possible
  Handle(AIS_Dimension) aDim;
  if (myIsDiameterDimension)
  {
    aDim = new AIS_DiameterDimension (aCircle);
    Handle(AIS_DiameterDimension)::DownCast(aDim)->SetFlyout (aDimDlg->GetFlyout());
  }
  else
  {
    aDim = new AIS_RadiusDimension (aCircle);
    Handle(AIS_RadiusDimension)::DownCast(aDim)->SetFlyout (aDimDlg->GetFlyout());
  }

  Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
  anAspect->MakeArrows3d (Standard_False);
  anAspect->MakeText3d (aDimDlg->GetTextType());
  anAspect->TextAspect()->SetHeight (aDimDlg->GetFontHeight());
  anAspect->MakeTextShaded (aDimDlg->IsText3dShaded());
  anAspect->SetCommonColor (aDimDlg->GetDimensionColor());
  anAspect->MakeUnitsDisplayed (aDimDlg->IsUnitsDisplayed());
  if (aDimDlg->IsUnitsDisplayed())
  {
    aDim->SetDisplayUnits (aDimDlg->GetUnits());
  }

  aDim->SetDimensionAspect (anAspect);

  // Display dimension in the neutral point

  myAISContext->Display (aDim, Standard_True);
  myAISContext->Activate (AIS_Shape::SelectionMode (TopAbs_EDGE));
}
