// LengthParamsEdgePage.cpp : implementation file
//

#include "stdafx.h"
#include "LengthParamsEdgePage.h"
#include "DimensionDlg.h"

#include <Standard_Macro.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_LengthDimension.hxx>
#include <GC_MakePlane.hxx>
#include <TopExp.hxx>

// CLengthParamsEdgePage dialog

IMPLEMENT_DYNAMIC(CLengthParamsEdgePage, CDialog)

//=======================================================================
//function : CLengthParamsEdgePage
//purpose  :
//=======================================================================

CLengthParamsEdgePage::CLengthParamsEdgePage (Handle(AIS_InteractiveContext) theContext,CWnd* pParent /*=NULL*/)
: CDialog (CLengthParamsEdgePage::IDD, pParent)
{
  myAISContext = theContext;
}

//=======================================================================
//function : ~CLengthParamsEdgePage
//purpose  :
//=======================================================================

CLengthParamsEdgePage::~CLengthParamsEdgePage()
{
}

//=======================================================================
//function : DoDataExchange
//purpose  :
//=======================================================================

void CLengthParamsEdgePage::DoDataExchange (CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP (CLengthParamsEdgePage, CDialog)
  ON_BN_CLICKED (IDC_ChooseEdgeBtn, &CLengthParamsEdgePage::OnBnClickedChooseEdgeBtn)
END_MESSAGE_MAP()

//=======================================================================
//function : GetButton
//purpose  :
//=======================================================================

CButton* CLengthParamsEdgePage::GetButton()
{
  return (CButton*)GetDlgItem (IDC_ChooseEdgeBtn);
}

//=======================================================================
//function : OnBnClickedChooseEdgeBtn
//purpose  :
//=======================================================================

void CLengthParamsEdgePage::OnBnClickedChooseEdgeBtn()
{
  myAISContext->InitSelected();

  if (!myAISContext->MoreSelected() ||
       myAISContext->SelectedShape().ShapeType() != TopAbs_EDGE)
  {
    AfxMessageBox ( _T("Choose the edge and press the button again"), MB_ICONINFORMATION | MB_OK);
    return;
  }

  TopoDS_Shape aSelShape = myAISContext->SelectedShape();
  const TopoDS_Edge& anEdge = TopoDS::Edge (aSelShape);

  myAISContext->ClearSelected (Standard_False);
  TopoDS_Vertex aFirstVertex, aSecondVertex;
  TopExp::Vertices (TopoDS::Edge (anEdge), aFirstVertex, aSecondVertex);

  gp_Pnt aP1 = BRep_Tool::Pnt (aFirstVertex);
  gp_Pnt aP2 = BRep_Tool::Pnt (aSecondVertex);
  gp_Pnt aP3 (aP2.X() + 10, aP2.Y() + 10, aP2.Z() + 10);

  GC_MakePlane aMkPlane (aP1,aP2,aP3);
  Handle(Geom_Plane) aPlane = aMkPlane.Value();

  CDimensionDlg *aDimDlg = (CDimensionDlg*)(GetParentOwner());

  Handle(AIS_LengthDimension) aLenDim = new AIS_LengthDimension (TopoDS::Edge (anEdge), aPlane->Pln());
  Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
  anAspect->MakeArrows3d (Standard_False);
  anAspect->MakeText3d (aDimDlg->GetTextType());
  anAspect->TextAspect()->SetHeight (aDimDlg->GetFontHeight());
  anAspect->MakeTextShaded (aDimDlg->IsText3dShaded());
  anAspect->SetCommonColor (aDimDlg->GetDimensionColor());
  anAspect->MakeUnitsDisplayed (aDimDlg->IsUnitsDisplayed());
  if (aDimDlg->IsUnitsDisplayed())
  {
    aLenDim->SetDisplayUnits (aDimDlg->GetUnits());
  }

  aLenDim->SetDimensionAspect (anAspect);
  aLenDim->SetFlyout (aDimDlg->GetFlyout());

  myAISContext->Display (aLenDim, Standard_True);
  myAISContext->Activate (AIS_Shape::SelectionMode (TopAbs_EDGE));
}
