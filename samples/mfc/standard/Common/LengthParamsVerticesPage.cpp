// LengthParamsVerticesPage.cpp : implementation file
//

#include "stdafx.h"
#include "LengthParamsVerticesPage.h"
#include "DimensionDlg.h"
#include <Standard_Macro.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_LengthDimension.hxx>
#include <GC_MakePlane.hxx>


// CLengthParamsVerticesPage dialog

IMPLEMENT_DYNAMIC(CLengthParamsVerticesPage, CDialog)

//=======================================================================
//function : CLengthParamsVerticesPage
//purpose  :
//=======================================================================

CLengthParamsVerticesPage::CLengthParamsVerticesPage (Handle(AIS_InteractiveContext) theAISContext, CWnd* pParent /*=NULL*/)
: CDialog (CLengthParamsVerticesPage::IDD, pParent)
{
 myAISContext = theAISContext;
}

//=======================================================================
//function : ~CLengthParamsVerticesPage
//purpose  :
//=======================================================================

CLengthParamsVerticesPage::~CLengthParamsVerticesPage()
{
}

//=======================================================================
//function : DoDataExchange
//purpose  :
//=======================================================================

void CLengthParamsVerticesPage::DoDataExchange (CDataExchange* pDX)
{
  CDialog::DoDataExchange (pDX);
}


BEGIN_MESSAGE_MAP(CLengthParamsVerticesPage, CDialog)
  ON_BN_CLICKED(IDC_BUTTON1, &CLengthParamsVerticesPage::OnBnClickedVertex1Btn)
  ON_BN_CLICKED(IDC_BUTTON2, &CLengthParamsVerticesPage::OnBnClickedVertex2Btn)
END_MESSAGE_MAP()


//=======================================================================
//function : OnBnClickedVertex1Btn
//purpose  :
//=======================================================================

void CLengthParamsVerticesPage::OnBnClickedVertex1Btn()
{
   // Open local context and choose the edge for length dimensions
  if (!myAISContext->HasOpenedContext())
  {
    myAISContext->OpenLocalContext();
    myAISContext->ActivateStandardMode(TopAbs_VERTEX);
    AfxMessageBox(_T("Local context was not opened. Choose the vertices and press the button again"),
                    MB_ICONINFORMATION | MB_OK);
    return;
  }

  // Now it's ok, local context is opened and edge selection mode is activated
  // Check if some edge is selected
  myAISContext->LocalContext()->InitSelected();
  if (!myAISContext->LocalContext()->MoreSelected())
  {
    AfxMessageBox (_T ("Choose the vertex and press the button again"), MB_ICONINFORMATION | MB_OK);
    return;
  }

  myFirstVertex = TopoDS::Vertex (myAISContext->LocalContext()->SelectedShape());

  myAISContext->LocalContext()->ClearSelected();
}

//=======================================================================
//function : OnBnClickedVertex2Btn
//purpose  :
//=======================================================================

void CLengthParamsVerticesPage::OnBnClickedVertex2Btn()
{
  myAISContext->LocalContext()->InitSelected();
  if (!myAISContext->LocalContext()->MoreSelected())
  {
    AfxMessageBox (_T ("Choose the vertex and press the button again"), MB_ICONINFORMATION | MB_OK);
    return;
  }

  mySecondVertex = TopoDS::Vertex (myAISContext->LocalContext()->SelectedShape());
  myAISContext->LocalContext()->ClearSelected();

  //Build dimension here
  gp_Pnt aP1=BRep_Tool::Pnt (myFirstVertex);
  gp_Pnt aP2=BRep_Tool::Pnt (mySecondVertex);
  gp_Pnt aP3 (aP2.X() + 10, aP2.Y() + 10, aP2.Z() + 10);

  GC_MakePlane aMkPlane (aP1,aP2,aP3);
  Handle(Geom_Plane) aPlane = aMkPlane.Value();

  CDimensionDlg *aDimDlg = (CDimensionDlg*)(this->GetParentOwner());

  Handle(AIS_LengthDimension) aLenDim = new AIS_LengthDimension (aP1, aP2, aPlane->Pln());

  Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
  anAspect->MakeArrows3d (Standard_False);
  anAspect->MakeText3d (aDimDlg->GetTextType());
  anAspect->TextAspect()->SetHeight (aDimDlg->GetFontHeight());
  anAspect->MakeTextShaded (aDimDlg->IsText3dShaded());
  anAspect->MakeUnitsDisplayed (aDimDlg->IsUnitsDisplayed());
  if (aDimDlg->IsUnitsDisplayed())
  {
    aLenDim->SetDisplayUnits (aDimDlg->GetUnits ());
  }

  aLenDim->SetDimensionAspect (anAspect);
  aLenDim->SetFlyout (aDimDlg->GetFlyout());

  myAISContext->CloseAllContexts();
  myAISContext->Display (aLenDim);
  myAISContext->OpenLocalContext();
  myAISContext->ActivateStandardMode (TopAbs_VERTEX);
}

//=======================================================================
//function : getFirstVertex
//purpose  :
//=======================================================================

const TopoDS_Vertex& CLengthParamsVerticesPage::getFirstVertex() const
{
  return myFirstVertex;
}

//=======================================================================
//function : getSecondVertex
//purpose  :
//=======================================================================

const TopoDS_Vertex& CLengthParamsVerticesPage::getSecondVertex() const
{
  return mySecondVertex;
}
