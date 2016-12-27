// ModelClippingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Viewer3dApp.h"
#include "ModelClippingDlg.h"
#include "Viewer3dDoc.h"
#include "offsetdlg.h"	// Added by ClassView

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EOL "\n"

// =======================================================================
// function : CModelClippingDlg
// purpose  :
// =======================================================================
CModelClippingDlg::CModelClippingDlg (Handle(V3d_View)            theView,
                                      Handle(AIS_Shape)           theShape,
                                      Handle(Graphic3d_ClipPlane) theClippingPlane,
                                      CViewer3dDoc*               theDoc,
                                      CWnd*                       theParent)
: CDialog(CModelClippingDlg::IDD, theParent),
  m_ModelClipping_Z (0.0),
  myModelClipping_Z (0.0),
  m_ModelClippingONOFF (FALSE),
  myView (theView),
  myShape (theShape),
  myClippingPlane (theClippingPlane),
  myDoc (theDoc)
{}

// =======================================================================
// function : DoDataExchange
// purpose  :
// =======================================================================
void CModelClippingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelClippingDlg)
	DDX_Control(pDX, IDC_SLIDER_MODELCLIPPING_Z, m_ModelClippingZSlidCtrl);
	DDX_Text(pDX, IDC_EDIT_MODELCLIPPING_Z, m_ModelClipping_Z);
	DDX_Check(pDX, IDC_CHECK_MODELCLIPPINGONOFF, m_ModelClippingONOFF);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelClippingDlg, CDialog)
	//{{AFX_MSG_MAP(CModelClippingDlg)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_MODELCLIPPING_Z, OnChangeEditModelclippingZ)
	ON_BN_CLICKED(IDC_CHECK_MODELCLIPPINGONOFF, OnCheckModelclippingonoff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =======================================================================
// function : OnHScroll
// purpose  :
// =======================================================================
void CModelClippingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  UpdateData(TRUE);

  m_ModelClipping_Z = m_ModelClippingZSlidCtrl.GetPos();

  UpdateData(FALSE);

  // Setting the ZClipping depth at m_ZClippingDepth value
  gp_Pln aPlane (gp_Pnt (-m_ModelClipping_Z, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));

  myClippingPlane->SetEquation (aPlane);

  gp_Trsf myTrsf;
  myTrsf.SetTranslation (gp_Pnt (m_ModelClipping_Z, 0.0, 0.0), gp_Pnt(myModelClipping_Z, 0.0, 0.0));

  // transform presentation shape
  if (m_ModelClippingONOFF)
  {
    myDoc->GetAISContext()->SetLocation (myShape, TopLoc_Location (myTrsf));
    myDoc->GetAISContext()->Redisplay (myShape, Standard_True);
    myView->Update();
  }
  else
  {
    myShape->SetLocalTransformation (myTrsf);
  }

  TCollection_AsciiString aMessage (
    EOL "gp_Pln aPlane (gp_Pnt (-m_ModelClipping_Z, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));"
    EOL
    EOL "myClippingPlane->SetEquation (aPlane);\n" );

  // Update The Result Message Dialog
  myDoc->UpdateResultMessageDlg ("Change clipping plane", aMessage);

  CDialog::OnHScroll (nSBCode, nPos, pScrollBar);
}

// =======================================================================
// function : OnInitDialog
// purpose  :
// =======================================================================
BOOL CModelClippingDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  const Graphic3d_ClipPlane::Equation aPlaneEquation = myClippingPlane->GetEquation();

  // m_ModelClipping_Z = D (plane coefficient)
  m_ModelClipping_Z = aPlaneEquation[3];
  m_ModelClippingZSlidCtrl.SetRange (-750, 750, TRUE);
  m_ModelClippingZSlidCtrl.SetPos ((int)floor (m_ModelClipping_Z));

  m_ModelClippingONOFF = myClippingPlane->IsOn();

  if (m_ModelClippingONOFF)
  {
    // register and activate clipping plane
    Standard_Boolean toAddPlane = Standard_True;
    Handle(Graphic3d_SequenceOfHClipPlane) aPlanes = myView->ClipPlanes();
    if (!aPlanes.IsNull())
    {
      for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*aPlanes); aPlaneIt.More(); aPlaneIt.Next())
      {
        if (aPlaneIt.Value() == myClippingPlane)
        {
          toAddPlane = Standard_False;
          break;
        }
      }
    }

    if (toAddPlane)
    {
      myView->AddClipPlane (myClippingPlane);
    }

    myClippingPlane->SetOn (Standard_True);

    myDoc->GetAISContext()->Display (myShape, Standard_True);
  }

  UpdateData (FALSE);

  return TRUE;
}

// =======================================================================
// function : OnChangeEditModelclippingZ
// purpose  :
// =======================================================================
void CModelClippingDlg::OnChangeEditModelclippingZ()
{
  UpdateData (TRUE);

  m_ModelClippingZSlidCtrl.SetPos ((int)floor (m_ModelClipping_Z));

  // Change clipping plane
  gp_Pln aPlane (gp_Pnt (-m_ModelClipping_Z, 0.0, 0.0), gp_Dir (1.0, 0.0, 0.0));

  myClippingPlane->SetEquation (aPlane);

  // transform presentation shape
  gp_Trsf myTrsf;
  myTrsf.SetTranslation ( gp_Pnt (m_ModelClipping_Z, 0.0, 0.0), gp_Pnt (myModelClipping_Z, 0.0, 0.0));

  // transform presentation shape
  if (m_ModelClippingONOFF)
  {
    myDoc->GetAISContext()->SetLocation (myShape, TopLoc_Location (myTrsf));
    myDoc->GetAISContext()->Redisplay (myShape, Standard_False);
    myView->Update();
  }
  else
  {
    myShape->SetLocalTransformation (myTrsf);
  }

  myModelClipping_Z = m_ModelClipping_Z;

  TCollection_AsciiString aMessage (
    EOL "gp_Pln aPlane (gp_Pnt (-m_ModelClipping_Z, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));"
    EOL
    EOL "myClippingPlane->SetEquation (aPlane);\n" );

  // Update The Result Message Dialog
  myDoc->UpdateResultMessageDlg ("Change clipping plane", aMessage);
}

// =======================================================================
// function : OnCheckModelclippingonoff
// purpose  :
// =======================================================================
void CModelClippingDlg::OnCheckModelclippingonoff()
{
  UpdateData(TRUE);

  if (m_ModelClippingONOFF)
  {
    // register and activate clipping plane
    Standard_Boolean toAddPlane = Standard_True;
    Handle(Graphic3d_SequenceOfHClipPlane) aPlanes = myView->ClipPlanes();
    if (!aPlanes.IsNull())
    {
      for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*aPlanes); aPlaneIt.More(); aPlaneIt.Next())
      {
        if (aPlaneIt.Value() == myClippingPlane)
        {
          toAddPlane = Standard_False;
          break;
        }
      }
    }

    if (toAddPlane)
    {
      myView->AddClipPlane (myClippingPlane);
    }

    myClippingPlane->SetOn (Standard_True);

    myDoc->GetAISContext()->Display (myShape, Standard_False);
  }
  else
  {
    // deactivate clipping plane
    myClippingPlane->SetOn (Standard_False);

    myDoc->GetAISContext()->Remove (myShape, Standard_False);
  }

  myView->Update();

  TCollection_AsciiString aMessage (
    EOL "if (...)"
    EOL "{"
    EOL "  // register and activate clipping plane"
    EOL "  if (!myView->ClipPlanes()->Contains (myClippingPlane))"
    EOL "  {"
    EOL "    myView->AddClipPlane (myClippingPlane);"
    EOL "  }"
    EOL
    EOL "  myClippingPlane->SetOn (Standard_True);"
    EOL "}"
    EOL "else"
    EOL "{"
    EOL "  // deactivate clipping plane"
    EOL "  myClippingPlane->SetOn (Standard_False);"
    EOL "}" );

  myDoc->UpdateResultMessageDlg ("Switch clipping on/off", aMessage);
}

// =======================================================================
// function : OnCancel
// purpose  :
// =======================================================================
void CModelClippingDlg::OnCancel() 
{
  UpdateData(TRUE);

  if (m_ModelClippingONOFF)
  {
    // remove and deactivate clipping plane
    myView->RemoveClipPlane (myClippingPlane);

    myClippingPlane->SetOn (Standard_False);
  }

  m_ModelClippingONOFF=FALSE;

  if (!myShape.IsNull())
  {
    myDoc->GetAISContext()->Remove (myShape, Standard_False);
  }

  myView->Update();

  CDialog::OnCancel();
}

// =======================================================================
// function : OnOK
// purpose  :
// =======================================================================
void CModelClippingDlg::OnOK() 
{
  if (!myShape.IsNull())
  {
    myDoc->GetAISContext()->Remove (myShape, Standard_True);
  }

  CDialog::OnOK();
}
