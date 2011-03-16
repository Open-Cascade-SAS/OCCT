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


/////////////////////////////////////////////////////////////////////////////
// CModelClippingDlg dialog


CModelClippingDlg::CModelClippingDlg(Handle(V3d_View) aView, Handle(V3d_Plane) aPlane, 
					       Handle(AIS_Shape) aShape, CViewer3dDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CModelClippingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelClippingDlg)
	m_ModelClipping_Z = 0.0;
	myView=aView;
    myPlane=aPlane;
	myShape=aShape;
	myDoc=pDoc;
	m_ModelClippingONOFF = FALSE;
	//}}AFX_DATA_INIT
}


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

/////////////////////////////////////////////////////////////////////////////
// CModelClippingDlg message handlers

void CModelClippingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData(TRUE);

	m_ModelClipping_Z = m_ModelClippingZSlidCtrl.GetPos();

	UpdateData(FALSE);

// Setting the ZClipping depth at m_ZClippingDepth value
	gp_Pln  clipPln(gp_Pnt(-m_ModelClipping_Z,0,0),gp_Dir(1,0,0));
	Standard_Real A,B,C,D;
	clipPln.Coefficients(A,B,C,D);
	myPlane->SetPlane(A,B,C,D);
	if(m_ModelClippingONOFF)
	    myView->SetPlaneOn(myPlane);
	gp_Trsf myTrsf;
	myTrsf.SetTranslation(gp_Pnt(m_ModelClipping_Z,0,0), gp_Pnt(myModelClipping_Z,0,0));
	myDoc->GetAISContext()->SetLocation(myShape,TopLoc_Location(myTrsf)) ;
	myDoc->GetAISContext()->Redisplay(myShape);
	myView->Update();

  TCollection_AsciiString Message("\
gp_Pln  clipPln(gp_Pnt(-m_ModelClippingZSlidCtrl.GetPos(),0,0),gp_Dir(1,0,0));\n\
Standard_Real A,B,C,D;\n\
clipPln.Coefficients(A,B,C,D);\n\
myPlane->SetPlane(A,B,C,D);\n\
myView->SetPlaneOn(myPlane); \n\
  ");

  // Update The Result Message Dialog
	myDoc->UpdateResultMessageDlg("SetPlaneOn",Message);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CModelClippingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	Standard_Real A,B,C,D;
	myPlane->Plane(A,B,C,D);
    m_ModelClipping_Z = D;
	m_ModelClippingZSlidCtrl.SetRange(-750, 750, TRUE);
	m_ModelClippingZSlidCtrl.SetPos( (int) floor(m_ModelClipping_Z));

	Handle(V3d_Plane) thePlane;
	for( myView->InitActivePlanes() ; 
		myView->MoreActivePlanes() ; myView->NextActivePlanes() ) {
	  thePlane = myView->ActivePlane() ;
	  if( thePlane == myPlane ) m_ModelClippingONOFF = TRUE;
	}
    if(m_ModelClippingONOFF)
		if(!myShape.IsNull())
			myDoc->GetAISContext()->Display(myShape);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
} //V3d_Plane.hxx

void CModelClippingDlg::OnChangeEditModelclippingZ() 
{
	UpdateData(TRUE);

// Setting the m_ZClippingDepthSlidCtrl position at floor(m_ZClippingDepth) value (because slider position is an integer)
	m_ModelClippingZSlidCtrl.SetPos( (int) floor(m_ModelClipping_Z));
// Setting the ZClipping depth at m_ZClippingDepth value
	gp_Pln  clipPln(gp_Pnt(-m_ModelClipping_Z,0,0),gp_Dir(1,0,0));
	Standard_Real A,B,C,D;
	clipPln.Coefficients(A,B,C,D);
	myPlane->SetPlane(A,B,C,D);
	if(m_ModelClippingONOFF)
		myView->SetPlaneOn(myPlane); 
	gp_Trsf myTrsf;
	myTrsf.SetTranslation(gp_Pnt(m_ModelClipping_Z,0,0), gp_Pnt(myModelClipping_Z,0,0));
	myDoc->GetAISContext()->SetLocation(myShape,TopLoc_Location(myTrsf)) ;
	myDoc->GetAISContext()->Redisplay(myShape);
	myView->Update();

	myModelClipping_Z = m_ModelClipping_Z;

  TCollection_AsciiString Message("\
gp_Pln  clipPln(gp_Pnt(-m_ModelClipping_Z,0,0),gp_Dir(1,0,0));\n\
Standard_Real A,B,C,D;\n\
clipPln.Coefficients(A,B,C,D);\n\
myPlane->SetPlane(A,B,C,D);\n\
myView->SetPlaneOn(myPlane); \n\
  ");

  // Update The Result Message Dialog
	myDoc->UpdateResultMessageDlg("SetPlaneOn",Message);
}

void CModelClippingDlg::OnCheckModelclippingonoff() 
{
	UpdateData(TRUE);

//activate the plane
	if(m_ModelClippingONOFF)
	{
//activate the plane
		myView->SetPlaneOn(myPlane);
		myDoc->GetAISContext()->Display(myShape);
	}
	else
//deactivate the plane
	{
		myView->SetPlaneOff(myPlane);
		myDoc->GetAISContext()->Erase(myShape);
	}

	myView->Update();

  TCollection_AsciiString Message("\
gp_Pln  clipPln(gp_Pnt(-m_ModelClippingZSlidCtrl.GetPos(),0,0),gp_Dir(1,0,0));\n\
Standard_Real A,B,C,D;\n\
clipPln.Coefficients(A,B,C,D);\n\
myPlane->SetPlane(A,B,C,D);\n\
if(m_ModelClippingONOFF) \n\
	myView->SetPlaneOn(myPlane); \n\
else \n\
	myView->SetPlaneOff(myPlane); \n\
  ");

  // Update The Result Message Dialog
	myDoc->UpdateResultMessageDlg("SetPlaneOn",Message);
}

void CModelClippingDlg::OnCancel() 
{
	UpdateData(TRUE);
	if(m_ModelClippingONOFF)
//deactivate the plane
		myView->SetPlaneOff(myPlane);

	m_ModelClippingONOFF=FALSE;

	if(!myShape.IsNull())
		myDoc->GetAISContext()->Erase(myShape);

	myView->Update();
	
	CDialog::OnCancel();
}

void CModelClippingDlg::OnOK() 
{
	if(!myShape.IsNull())
		myDoc->GetAISContext()->Erase(myShape);
	
	CDialog::OnOK();
}
