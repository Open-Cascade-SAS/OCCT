// AngleParamsVerticesPage.cpp : implementation file
//

#include "stdafx.h"
#include "AngleParamsVerticesPage.h"
#include "DimensionDlg.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_LengthDimension.hxx>
#include <AIS_AngleDimension.hxx>
#include <BRep_Tool.hxx>
#include <GC_MakePlane.hxx>
#include <Prs3d_DimensionAspect.hxx>

// CAngleParamsVerticesPage dialog

IMPLEMENT_DYNAMIC(CAngleParamsVerticesPage, CDialog)

//=======================================================================
//function : CAngleParamsVerticesPage
//purpose  :
//=======================================================================

CAngleParamsVerticesPage::CAngleParamsVerticesPage (Handle(AIS_InteractiveContext) theAISContext,
                                                    CWnd* pParent /*=NULL*/)
: CDialog(CAngleParamsVerticesPage::IDD, pParent)
{
  myAISContext = theAISContext;
}

//=======================================================================
//function : ~CAngleParamsVerticesPage
//purpose  :
//=======================================================================

CAngleParamsVerticesPage::~CAngleParamsVerticesPage()
{
}

//=======================================================================
//function : DoDataExchange
//purpose  :
//=======================================================================

void CAngleParamsVerticesPage::DoDataExchange (CDataExchange* pDX)
{
  CDialog::DoDataExchange (pDX);
}


BEGIN_MESSAGE_MAP(CAngleParamsVerticesPage, CDialog)
  ON_BN_CLICKED(IDC_BUTTON1, &CAngleParamsVerticesPage::OnBnClickedVertex1Btn)
  ON_BN_CLICKED(IDC_BUTTON3, &CAngleParamsVerticesPage::OnBnClickedVertex2Btn)
  ON_BN_CLICKED(IDC_BUTTON4, &CAngleParamsVerticesPage::OnBnClickedVertex3Btn)
END_MESSAGE_MAP()


//=======================================================================
//function : OnBnClickedVertex1Btn
//purpose  :
//=======================================================================

void CAngleParamsVerticesPage::OnBnClickedVertex1Btn()
{
  myAISContext->Activate (AIS_Shape::SelectionMode (TopAbs_VERTEX));

  // Now it's ok, edge selection mode is activated
  // Check if some vertex is selected
  myAISContext->InitSelected();
  if (!myAISContext->MoreSelected() ||
       myAISContext->SelectedShape().ShapeType() != TopAbs_VERTEX)
  {
    AfxMessageBox (_T ("Choose the vertex and press the button again"),
                       MB_ICONINFORMATION | MB_OK);
    return;
  }

  myFirstVertex = TopoDS::Vertex (myAISContext->SelectedShape());
  myAISContext->ClearSelected (Standard_True);
}

//=======================================================================
//function : OnBnClickedVertex2Btn
//purpose  :
//=======================================================================

void CAngleParamsVerticesPage::OnBnClickedVertex2Btn()
{
  myAISContext->InitSelected();
  if (!myAISContext->MoreSelected() ||
       myAISContext->SelectedShape().ShapeType() != TopAbs_VERTEX)
  {
    AfxMessageBox ( _T("Choose the vertex and press the button again"), MB_ICONINFORMATION | MB_OK);
    return;
  }

  mySecondVertex = TopoDS::Vertex (myAISContext->SelectedShape());

  myAISContext->ClearSelected (Standard_True);
}

//=======================================================================
//function : OnBnClickedVertex3Btn
//purpose  :
//=======================================================================

void CAngleParamsVerticesPage::OnBnClickedVertex3Btn()
{
  myAISContext->InitSelected();
  if (!myAISContext->MoreSelected())
  {
    AfxMessageBox (_T ("Choose the vertex and press the button again"), MB_ICONINFORMATION | MB_OK);
    return;
  }

  myThirdVertex = TopoDS::Vertex (myAISContext->SelectedShape());
  myAISContext->ClearSelected (Standard_False);

  //Build dimension here
  TopoDS_Edge anEdge12 = BRepBuilderAPI_MakeEdge (myFirstVertex, mySecondVertex);
  TopoDS_Edge anEdge23 = BRepBuilderAPI_MakeEdge (mySecondVertex, myThirdVertex);

  CDimensionDlg *aDimDlg = (CDimensionDlg*)(GetParentOwner());

  gp_Pnt aP1 = BRep_Tool::Pnt (myFirstVertex),
         aP2 = BRep_Tool::Pnt (mySecondVertex),
         aP3 = BRep_Tool::Pnt (myThirdVertex);
  GC_MakePlane aPlaneBuilder (aP1,aP2,aP3);

  Handle(Geom_Plane) aPlane = aPlaneBuilder.Value();
  Handle(AIS_AngleDimension) anAngleDim = new AIS_AngleDimension (aP1,aP2,aP3);
  Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
  anAspect->MakeArrows3d (Standard_False);
  anAspect->MakeText3d (aDimDlg->GetTextType());
  anAspect->TextAspect()->SetHeight (aDimDlg->GetFontHeight());
  anAspect->MakeTextShaded (aDimDlg->IsText3dShaded());
  anAspect->SetCommonColor (aDimDlg->GetDimensionColor());
  anAspect->MakeUnitsDisplayed (aDimDlg->IsUnitsDisplayed());
  if (aDimDlg->IsUnitsDisplayed())
  {
    anAngleDim->SetDisplayUnits (aDimDlg->GetUnits());
    if ((anAngleDim->GetDisplayUnits().IsEqual (TCollection_AsciiString ("deg"))))
    {
      // No units - for degree is special symbol that is enabled by default
      anAspect->MakeUnitsDisplayed (Standard_False);
    }
    else // radians - no special symbol
    {
      anAngleDim->SetDisplaySpecialSymbol (AIS_DSS_No);
    }
  }
  anAngleDim->SetDimensionAspect (anAspect);
  myAISContext->Display (anAngleDim, Standard_True);
  myAISContext->Activate (AIS_Shape::SelectionMode (TopAbs_VERTEX));
}
