// OffsetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\res\resource.h"
#include "offsetdlg.h"
#include "Viewer3dView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FactorMinValue 0
#define FactorMaxValue 20

#define UnitsMinValue 0
#define UnitsMaxValue 20


/////////////////////////////////////////////////////////////////////////////
// COffsetDlg dialog


COffsetDlg::COffsetDlg(CViewer3dDoc* theDoc, CWnd* pParent /*=NULL*/)
	: CDialog(COffsetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COffsetDlg)
	m_Factor = 1.;
	m_Units = 0.;
	//}}AFX_DATA_INIT
	myDoc = theDoc;
}


void COffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COffsetDlg)
	DDX_Control(pDX, IDC_SLIDER_OFFSETUNITS, m_UnitsSlidCtrl);
	DDX_Control(pDX, IDC_SLIDER_OFFSETFACTOR, m_FactorSlidCtrl);
	DDX_Text(pDX, IDC_EDIT_OFFSETFACTOR, m_Factor);
	DDV_MinMaxDouble(pDX, m_Factor, FactorMinValue, FactorMaxValue);
	DDX_Text(pDX, IDC_EDIT_OFFSETUNITS, m_Units);
	DDV_MinMaxDouble(pDX, m_Units, UnitsMinValue, UnitsMaxValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COffsetDlg, CDialog)
	//{{AFX_MSG_MAP(COffsetDlg)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_OFFSETFACTOR, OnChangeEditOffsetFactor)
	ON_EN_CHANGE(IDC_EDIT_OFFSETUNITS, OnChangeEditOffsetUnits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COffsetDlg message handlers

BOOL COffsetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

// Initializing the ComboBox
	SetOffsets(m_Factor,m_Units);

// Setting the m_FactorSlidCtrl position at floor(m_Factor) value (because slider position is an integer)
	m_FactorSlidCtrl.SetRange(FactorMinValue, FactorMaxValue, TRUE);
	m_FactorSlidCtrl.SetPos( (int) floor(m_Factor));

// Setting the m_UnitsSlidCtrl position at floor(m_Units) value (because slider position is an integer)
	m_UnitsSlidCtrl.SetRange(UnitsMinValue, UnitsMaxValue, TRUE);
	m_UnitsSlidCtrl.SetPos( (int) floor(m_Units));

	UpdateData(FALSE);
			
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COffsetDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{

// Setting the m_Factor value at m_FactorSlidCtrl position
	m_Factor = m_FactorSlidCtrl.GetPos();
// Setting the m_Units value at m_UnitsSlidCtrl position
	m_Units = m_UnitsSlidCtrl.GetPos();

	UpdateData(FALSE);


	SetOffsets(m_Factor,m_Units);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void COffsetDlg::OnChangeEditOffsetFactor() 
{
	UpdateData();

	m_FactorSlidCtrl.SetPos(int(m_Units));

	SetOffsets(m_Factor,m_Units);
}

void COffsetDlg::OnChangeEditOffsetUnits() 
{
	UpdateData();

	m_UnitsSlidCtrl.SetPos(int(m_Units));

	SetOffsets(m_Factor,m_Units);
}

void COffsetDlg::SetOffsets(Standard_Real theFactor, Standard_Real theUnits)
{
	Handle(AIS_InteractiveContext) aAISContext = myDoc->GetAISContext();

	Handle(AIS_Shape) aOverlappedBox = myDoc->GetOverlappedBox();
	Handle(AIS_Shape) aOverlappedCylinder = myDoc->GetOverlappedCylinder();
	Handle(AIS_Shape) aOverlappedSphere = myDoc->GetOverlappedSphere();
	Handle(AIS_Shape) aBox = myDoc->GetBox();
	Handle(User_Cylinder) aCylinder = myDoc->GetCylinder();
	Handle(AIS_Shape) aSphere = myDoc->GetSphere();


	myDoc->SetDialogTitle("Set offsets");

	TCollection_AsciiString Message;

	Standard_Boolean OneOrMoreCurrentIsDisplayed=false;
        Standard_ShortReal aFactor = (Standard_ShortReal)theFactor;
        Standard_ShortReal aUnits = (Standard_ShortReal)theUnits;
	
	for (aAISContext->InitSelected(); aAISContext->MoreSelected(); aAISContext->NextSelected())
		if (aAISContext->IsDisplayed(aAISContext->SelectedInteractive())) OneOrMoreCurrentIsDisplayed=true;

	if(OneOrMoreCurrentIsDisplayed){
		// A small trick to avoid complier error (C2668).
		const Handle(AIS_InteractiveObject)& anIOOverlappedBox = aOverlappedBox;
		const Handle(AIS_InteractiveObject)& anIOBox = aBox;
		if((aAISContext->IsSelected(anIOOverlappedBox) || aAISContext->IsSelected(anIOBox))
				&& aAISContext->IsDisplayed(aOverlappedBox)){
			aOverlappedBox->SetPolygonOffsets(Aspect_POM_Fill,aFactor,aUnits);
			Message = "Box2 -> SetPolygonOffsets(Aspect_POM_Fill, Factor, Units);\n";
			//myDoc -> AddTextInDialog(Message);
		}

		// A small trick to avoid complier error (C2668).
		const Handle(AIS_InteractiveObject)& anIOOverlappedCylinder = aOverlappedCylinder;
		const Handle(AIS_InteractiveObject)& anIOCylinder = aCylinder;
		if((aAISContext->IsSelected(anIOOverlappedCylinder) || aAISContext->IsSelected(anIOCylinder))
				&& aAISContext->IsDisplayed(aOverlappedCylinder)){
			aOverlappedCylinder->SetPolygonOffsets(Aspect_POM_Fill,aFactor,aUnits);
			Message = Message + "Cylinder2 -> SetPolygonOffsets(Aspect_POM_Fill, Factor, Units);\n";
			//myDoc -> AddTextInDialog(Message);
		}

		// A small trick to avoid complier error (C2668).
		const Handle(AIS_InteractiveObject)& anIOOverlappedSphere = aOverlappedSphere;
		const Handle(AIS_InteractiveObject)& anIOSphere = aSphere;
		if((aAISContext->IsSelected(anIOOverlappedSphere) || aAISContext->IsSelected(anIOSphere))
				&& aAISContext->IsDisplayed(aOverlappedSphere)){
			aOverlappedSphere->SetPolygonOffsets(Aspect_POM_Fill,aFactor,aUnits);
			Message = Message + "Sphere2 -> SetPolygonOffsets(Aspect_POM_Fill, Factor, Units);\n";
			//myDoc -> AddTextInDialog(Message);
		}	
	}
	else{
		if(aAISContext->IsDisplayed(aOverlappedBox)){
			aOverlappedBox->SetPolygonOffsets(Aspect_POM_Fill,aFactor,aUnits);
			Message = Message + "Box2 -> SetPolygonOffsets(Aspect_POM_Fill, Factor, Units);\n";
			//myDoc -> AddTextInDialog(Message);
		}

		if(aAISContext->IsDisplayed(aOverlappedCylinder)){
			aOverlappedCylinder->SetPolygonOffsets(Aspect_POM_Fill,aFactor,aUnits);
			Message = Message + "Cylinder2 -> SetPolygonOffsets(Aspect_POM_Fill, Factor, Units);\n";
			//myDoc -> AddTextInDialog(Message);
		}

		if(aAISContext->IsDisplayed(aOverlappedSphere)){
			aOverlappedSphere->SetPolygonOffsets(Aspect_POM_Fill,aFactor,aUnits);
			Message = Message + "Sphere2 -> SetPolygonOffsets(Aspect_POM_Fill, Factor, Units);\n";
			//myDoc -> AddTextInDialog(Message);
		}
	}
	
	CString aTextInDialog = myDoc -> GetDialogText();
  if (aTextInDialog != Message.ToCString())
		myDoc -> UpdateResultMessageDlg("Set offsets", Message);
	aAISContext->UpdateCurrentViewer();
}

void COffsetDlg::UpdateValues()
{
	Standard_Integer aOffsetMode;
	Standard_ShortReal aBoxFactor = 0;
	Standard_ShortReal aCylFactor = 0;
	Standard_ShortReal aSphereFactor = 0;
	Standard_ShortReal aCylUnits = 0;
	Standard_ShortReal aSphereUnits = 0;
	Standard_ShortReal aBoxUnits = 0;

	Handle(AIS_InteractiveContext) aAISContext = myDoc->GetAISContext();

	Handle(AIS_Shape) aOverlappedBox = myDoc->GetOverlappedBox();
	Handle(AIS_Shape) aOverlappedCylinder = myDoc->GetOverlappedCylinder();
	Handle(AIS_Shape) aOverlappedSphere = myDoc->GetOverlappedSphere();
	Handle(AIS_Shape) aBox = myDoc->GetBox();
	Handle(User_Cylinder) aCylinder = myDoc->GetCylinder();
	Handle(AIS_Shape) aSphere = myDoc->GetSphere();

	BOOL IsOverlappedCylinderDisplayed = aAISContext->IsDisplayed(aOverlappedCylinder);
	BOOL IsOverlappedSphereDisplayed = aAISContext->IsDisplayed(aOverlappedSphere);
	BOOL IsOverlappedBoxDisplayed = aAISContext->IsDisplayed(aOverlappedBox);

	const Handle(AIS_InteractiveObject)& anIOOverlappedCylinder = aOverlappedCylinder;
	const Handle(AIS_InteractiveObject)& anIOOverlappedSphere = aOverlappedSphere;
	const Handle(AIS_InteractiveObject)& anIOOverlappedBox = aOverlappedBox;
	BOOL IsOverlappedCylinderCurrent = aAISContext->IsSelected(anIOOverlappedCylinder);
	BOOL IsOverlappedSphereCurrent = aAISContext->IsSelected(anIOOverlappedSphere);
	BOOL IsOverlappedBoxCurrent = aAISContext->IsSelected(anIOOverlappedBox);


	/*BOOL IsAnyOverlappedObjectCurrent = 
		IsOverlappedCylinderCurrent || IsOverlappedSphereCurrent || IsOverlappedBoxCurrent;*/

	const Handle(AIS_InteractiveObject)& anIOCylinder = aCylinder;
	const Handle(AIS_InteractiveObject)& anIOSphere = aSphere;
	const Handle(AIS_InteractiveObject)& anIOBox = aBox;
	BOOL IsCylinderCurrent = aAISContext->IsSelected(anIOCylinder);
	BOOL IsSphereCurrent = aAISContext->IsSelected(anIOSphere);
	BOOL IsBoxCurrent = aAISContext->IsSelected(anIOBox);

	BOOL IsAnyObjectCurrent = 
		IsOverlappedCylinderCurrent || IsOverlappedSphereCurrent || IsOverlappedBoxCurrent
		|| IsCylinderCurrent || IsSphereCurrent || IsBoxCurrent;

	BOOL IsAnyOverlappedObjectDisplayed = 
		IsOverlappedCylinderDisplayed || IsOverlappedSphereDisplayed || IsOverlappedBoxDisplayed;

	Standard_Boolean OneOrMoreCurrentIsDisplayed=false;
	for (aAISContext->InitSelected();aAISContext->MoreSelected ();aAISContext->NextSelected ())
		if (aAISContext->IsDisplayed(aAISContext->SelectedInteractive())) OneOrMoreCurrentIsDisplayed=true;

	if(OneOrMoreCurrentIsDisplayed){
		if(IsOverlappedBoxDisplayed && (IsBoxCurrent || IsOverlappedBoxCurrent)){
			aOverlappedBox->PolygonOffsets(aOffsetMode,aBoxFactor,aBoxUnits);
		}

		if(IsOverlappedCylinderDisplayed && (IsCylinderCurrent || IsOverlappedCylinderCurrent)){
			aOverlappedCylinder->PolygonOffsets(aOffsetMode,aCylFactor,aCylUnits);
		}

		if(IsOverlappedSphereDisplayed && (IsSphereCurrent || IsOverlappedSphereCurrent)){
			aOverlappedSphere->PolygonOffsets(aOffsetMode,aSphereFactor,aSphereUnits);
		}

	}
	else{
	if(IsOverlappedBoxDisplayed){
			aOverlappedBox->PolygonOffsets(aOffsetMode,aBoxFactor,aBoxUnits);
		}

		if(IsOverlappedCylinderDisplayed){
			aOverlappedCylinder->PolygonOffsets(aOffsetMode,aCylFactor,aCylUnits);
		}

		if(IsOverlappedSphereDisplayed){
			aOverlappedSphere->PolygonOffsets(aOffsetMode,aSphereFactor,aSphereUnits);
		}
	}

	Standard_Real Max = aBoxFactor;
	if (Max < aCylFactor) Max = aCylFactor;
	if (Max < aSphereFactor) Max = aSphereFactor;

	m_Factor = Max;

	Max = aBoxUnits;
	if (Max < aCylUnits) Max = aCylUnits;
	if (Max < aSphereUnits) Max = aSphereUnits;

	m_Units = Max;

	m_FactorSlidCtrl.SetPos( (int) floor(m_Factor));


	m_UnitsSlidCtrl.SetPos( (int) floor(m_Units));
	UpdateData(FALSE);

	if(		(IsOverlappedBoxCurrent || (IsBoxCurrent && IsOverlappedBoxDisplayed)) ||
			(IsOverlappedCylinderCurrent || (IsCylinderCurrent && IsOverlappedCylinderDisplayed)) ||
			(IsOverlappedSphereCurrent || (IsSphereCurrent && IsOverlappedSphereDisplayed))  ||
			(!IsAnyObjectCurrent && IsAnyOverlappedObjectDisplayed)
	)
	{
		m_FactorSlidCtrl.EnableWindow();
		m_UnitsSlidCtrl.EnableWindow();
		GetDlgItem(IDC_EDIT_OFFSETFACTOR)->EnableWindow();
		GetDlgItem(IDC_EDIT_OFFSETUNITS)->EnableWindow();
	}
	else{
		m_FactorSlidCtrl.EnableWindow(false);
		m_UnitsSlidCtrl.EnableWindow(false);
		GetDlgItem(IDC_EDIT_OFFSETFACTOR)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_OFFSETUNITS)->EnableWindow(false);
	}

}
