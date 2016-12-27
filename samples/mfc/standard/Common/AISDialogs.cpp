// AISDialogs.cpp : implementation file
//

#include "stdafx.h"

#include "AISDialogs.h"

#define DEFAULT_DEVIATIONCOEFFICIENT 0.001
#define DEFAULT_DCBIG 0.005
#define DEFAULT_DCVBIG 0.01
#define DEFAULT_DCSMALL 0.0002
#define DEFAULT_DCVSMALL 0.00004
#define DEFAULT_COLOR Quantity_NOC_CYAN1
#define DEFAULT_MATERIAL Graphic3d_NOM_PLASTER
#define DEFAULT_BACKGROUNDCOLOR Quantity_NOC_MATRAGRAY
#define DEFAULT_HILIGHTCOLOR Quantity_NOC_YELLOW


/////////////////////////////////////////////////////////////////////////////
// CAISNbrIsosDialog dialog

//CAISNbrIsosDialog::CAISNbrIsosDialog(Handle(AIS_InteractiveContext) CurrentIC,
//									 CWnd* pParent /*=NULL*/)
//	: CDialog(CAISNbrIsosDialog::IDD, pParent)
/*
{
	//{{AFX_DATA_INIT(CAISNbrIsosDialog)
	m_Isosu = 0;
	m_Isosv = 0;
	//}}AFX_DATA_INIT

	myCurrentIC = CurrentIC;
	Handle (Prs3d_Drawer) ICDrawer = myCurrentIC->DefaultDrawer();
    m_Isosu = ICDrawer->UIsoAspect()->Number();
    m_Isosv = ICDrawer->VIsoAspect()->Number();

}


void CAISNbrIsosDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAISNbrIsosDialog)
	DDX_Text(pDX, IDC_EDITAISISOSU, m_Isosu);
	DDX_Text(pDX, IDC_EDITAISISOSV, m_Isosv);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAISNbrIsosDialog, CDialog)
	//{{AFX_MSG_MAP(CAISNbrIsosDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINAISISOSU, OnDeltaposSpinaisisosu)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINAISISOSV, OnDeltaposSpinaisisosv)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAISNbrIsosDialog message handlers

void CAISNbrIsosDialog::OnDeltaposSpinaisisosu(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here

	if ((pNMUpDown->iDelta < 0)) 
		m_Isosu = m_Isosu + 1;
	if ((pNMUpDown->iDelta > 0) && (m_Isosu > 0)) 
		m_Isosu = m_Isosu - 1;

	UpdateIsos ();
	
	*pResult = 0;
}

void CAISNbrIsosDialog::OnDeltaposSpinaisisosv(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here

	if ((pNMUpDown->iDelta < 0)) 
		m_Isosv = m_Isosv + 1;
	if ((pNMUpDown->iDelta > 0) && (m_Isosu > 0)) 
		m_Isosv = m_Isosv - 1;

	UpdateIsos ();
	
	*pResult = 0;
}


#include <AIS_InteractiveObject.hxx>
#include <Prs3d_Drawer.hxx>

void CAISNbrIsosDialog::UpdateIsos ()
{
    UpdateData (false);

	for (myCurrentIC->InitCurrent(); 
	     myCurrentIC->MoreCurrent ();
		 myCurrentIC->NextSelected ())
	{	
		Handle(AIS_InteractiveObject) CurObject;
		Handle(Prs3d_Drawer) CurDrawer;

		
		CurObject = myCurrentIC->Current();
		CurDrawer = CurObject->Attributes();

		CurDrawer->UIsoAspect()->SetNumber(m_Isosu);
        CurDrawer->VIsoAspect()->SetNumber(m_Isosv);

		myCurrentIC->SetLocalAttributes(CurObject, CurDrawer);
	
        myCurrentIC->Redisplay(CurObject);
    }	

}
*/
/////////////////////////////////////////////////////////////////////////////
// CDevCoeffDialog dialog


//CDevCoeffDialog::CDevCoeffDialog(Handle(AIS_InteractiveContext) CurrentIC,
//								 CWnd* pParent /*=NULL*/)
//	: CDialog(CDevCoeffDialog::IDD, pParent)
/*
{
	//{{AFX_DATA_INIT(CDevCoeffDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	myCurrentIC = CurrentIC;
}


void CDevCoeffDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDevCoeffDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDevCoeffDialog, CDialog)
	//{{AFX_MSG_MAP(CDevCoeffDialog)
	ON_BN_CLICKED(IDC_DC_BIG, OnDcBig)
	ON_BN_CLICKED(IDC_DC_DEFAULT, OnDcDefault)
	ON_BN_CLICKED(IDC_DC_SMALL, OnDcSmall)
	ON_BN_DOUBLECLICKED(IDC_DC_BIG, OnDoubleclickedDcBig)
	ON_BN_DOUBLECLICKED(IDC_DC_DEFAULT, OnDoubleclickedDcDefault)
	ON_BN_DOUBLECLICKED(IDC_DC_SMALL, OnDoubleclickedDcSmall)
	ON_BN_CLICKED(IDC_DC_VBIG, OnDcVbig)
	ON_BN_DOUBLECLICKED(IDC_DC_VBIG, OnDoubleclickedDcVbig)
	ON_BN_CLICKED(IDC_DC_VSMALL, OnDcVsmall)
	ON_BN_DOUBLECLICKED(IDC_DC_VSMALL, OnDoubleclickedDcVsmall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDevCoeffDialog message handlers



void CDevCoeffDialog::OnDcDefault() 
{
	// TODO: Add your control notification handler code here
	myCurrentIC->SetDeviationCoefficient (DEFAULT_DEVIATIONCOEFFICIENT);		
}

void CDevCoeffDialog::OnDcSmall() 
{
	// TODO: Add your control notification handler code here
	myCurrentIC->SetDeviationCoefficient (DEFAULT_DCSMALL);	
}

void CDevCoeffDialog::OnDcVsmall() 
{
	// TODO: Add your control notification handler code here
	myCurrentIC->SetDeviationCoefficient (DEFAULT_DCVSMALL);		
}


void CDevCoeffDialog::OnDcBig() 
{
	// TODO: Add your control notification handler code here
	myCurrentIC->SetDeviationCoefficient (DEFAULT_DCBIG);	
}

void CDevCoeffDialog::OnDcVbig() 
{
	// TODO: Add your control notification handler code here
	myCurrentIC->SetDeviationCoefficient (DEFAULT_DCVBIG);		
}



void CDevCoeffDialog::OnDoubleclickedDcDefault() 
{
	// TODO: Add your control notification handler code here
	myCurrentIC->SetDeviationCoefficient (DEFAULT_DEVIATIONCOEFFICIENT);		
	RedisplaySelected();	
}

void CDevCoeffDialog::OnDoubleclickedDcSmall() 
{
	// TODO: Add your control notification handler code here
	myCurrentIC->SetDeviationCoefficient (DEFAULT_DCSMALL);	
	RedisplaySelected();	
}

void CDevCoeffDialog::OnDoubleclickedDcVsmall() 
{
	// TODO: Add your control notification handler code here
	myCurrentIC->SetDeviationCoefficient (DEFAULT_DCVSMALL);	
	RedisplaySelected();	
}

void CDevCoeffDialog::OnDoubleclickedDcBig() 
{
	// TODO: Add your control notification handler code here
	myCurrentIC->SetDeviationCoefficient (DEFAULT_DCBIG);	
	RedisplaySelected();	
}

void CDevCoeffDialog::OnDoubleclickedDcVbig() 
{
	// TODO: Add your control notification handler code here
	myCurrentIC->SetDeviationCoefficient (DEFAULT_DCVBIG);	
	RedisplaySelected();	
}



void CDevCoeffDialog::RedisplaySelected ()
{
	for (myCurrentIC->InitCurrent(); 
	     myCurrentIC->MoreCurrent ();
		 myCurrentIC->NextSelected ())
	{	
	    myCurrentIC->Redisplay(myCurrentIC->Current());
    }		
}

*/

/////////////////////////////////////////////////////////////////////////////
// CDialogMaterial dialog


CDialogMaterial::CDialogMaterial(Handle(AIS_InteractiveContext) CurrentIC,
								 CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMaterial::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogMaterial)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	myCurrentIC = CurrentIC;
/*
	for (myCurrentIC->InitCurrent();myCurrentIC->MoreCurrent ();myCurrentIC->NextCurrent ()){
		for(int i = ID_OBJECT_MATERIAL_BRASS; i <= ID_OBJECT_MATERIAL_DEFAULT; i++){
			if (myCurrentIC->Current()->Material() - (i - ID_OBJECT_MATERIAL_BRASS) == 0) 
				//GotoDlgCtrl(GetDlgItem(i));
		}
	}
*/

}


void CDialogMaterial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogMaterial)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogMaterial, CDialog)
	//{{AFX_MSG_MAP(CDialogMaterial)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_ALUMINIUM, OnObjectMaterialAluminium)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_BRASS, OnObjectMaterialBrass)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_BRONZE, OnObjectMaterialBronze)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_CHROME, OnObjectMaterialChrome)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_COPPER, OnObjectMaterialCopper)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_GOLD, OnObjectMaterialGold)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_JADE, OnObjectMaterialJade)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_METALIZED, OnObjectMaterialMetalized)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_NEON_GNC, OnObjectMaterialNeonGNC)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_NEON_PHC, OnObjectMaterialNeonPHC)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_OBSIDIAN, OnObjectMaterialObsidian)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_PEWTER, OnObjectMaterialPewter)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_PLASTER, OnObjectMaterialPlaster)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_PLASTIC, OnObjectMaterialPlastic)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_SATIN, OnObjectMaterialSatin)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_SHINY_PLASTIC, OnObjectMaterialShinyPlastic)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_SILVER, OnObjectMaterialSilver)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_STEEL, OnObjectMaterialSteel)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_STONE, OnObjectMaterialStone)
	ON_BN_CLICKED(ID_OBJECT_MATERIAL_DEFAULT, OnObjectMaterialDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogMaterial message handlers
/*
void CDialogMaterial::OnMaterial(UINT nID) 
{
	SetMaterial ((Graphic3d_NameOfMaterial)(nID-ID_OBJECT_MATERIAL_BRASS));
}
*/
void CDialogMaterial::SetMaterial(Graphic3d_NameOfMaterial Material) 
{
  Standard_Real aTransparency;
  for (myCurrentIC->InitSelected();myCurrentIC->MoreSelected ();myCurrentIC->NextSelected ()){
    aTransparency = myCurrentIC->SelectedInteractive()->Transparency();
    myCurrentIC->SetMaterial (myCurrentIC->SelectedInteractive(), (Graphic3d_NameOfMaterial)(Material), Standard_False);
    myCurrentIC->SetTransparency (myCurrentIC->SelectedInteractive(), aTransparency, Standard_False);
  }
  myCurrentIC->UpdateCurrentViewer();
}

void CDialogMaterial::OnObjectMaterialAluminium     () { SetMaterial ( Graphic3d_NOM_ALUMINIUM     ) ; }
void CDialogMaterial::OnObjectMaterialBrass     () { SetMaterial ( Graphic3d_NOM_BRASS     ) ; }
void CDialogMaterial::OnObjectMaterialBronze    () { SetMaterial ( Graphic3d_NOM_BRONZE    ) ; }
void CDialogMaterial::OnObjectMaterialChrome    () { SetMaterial ( Graphic3d_NOM_CHROME     ) ; }
void CDialogMaterial::OnObjectMaterialCopper    () { SetMaterial ( Graphic3d_NOM_COPPER    ) ; }
void CDialogMaterial::OnObjectMaterialGold      () { SetMaterial ( Graphic3d_NOM_GOLD      ) ; }
void CDialogMaterial::OnObjectMaterialJade     () { SetMaterial ( Graphic3d_NOM_JADE     ) ; }
void CDialogMaterial::OnObjectMaterialMetalized     () { SetMaterial ( Graphic3d_NOM_METALIZED     ) ; }
void CDialogMaterial::OnObjectMaterialNeonGNC     () { SetMaterial ( Graphic3d_NOM_NEON_GNC     ) ; }
void CDialogMaterial::OnObjectMaterialNeonPHC     () { SetMaterial ( Graphic3d_NOM_NEON_PHC     ) ; }
void CDialogMaterial::OnObjectMaterialObsidian     () { SetMaterial ( Graphic3d_NOM_OBSIDIAN     ) ; }
void CDialogMaterial::OnObjectMaterialPewter    () { SetMaterial ( Graphic3d_NOM_PEWTER    ) ; }
void CDialogMaterial::OnObjectMaterialPlaster   () { SetMaterial ( Graphic3d_NOM_PLASTER   ) ; }
void CDialogMaterial::OnObjectMaterialPlastic   () { SetMaterial ( Graphic3d_NOM_PLASTIC   ) ; }
void CDialogMaterial::OnObjectMaterialSatin     () { SetMaterial ( Graphic3d_NOM_SATIN     ) ; }
void CDialogMaterial::OnObjectMaterialShinyPlastic     () { SetMaterial ( Graphic3d_NOM_SHINY_PLASTIC     ) ; }
void CDialogMaterial::OnObjectMaterialSilver    () { SetMaterial ( Graphic3d_NOM_SILVER    ) ; }
void CDialogMaterial::OnObjectMaterialSteel    () { SetMaterial ( Graphic3d_NOM_STEEL    ) ; }
void CDialogMaterial::OnObjectMaterialStone    () { SetMaterial ( Graphic3d_NOM_STONE    ) ; }
void CDialogMaterial::OnObjectMaterialDefault    () { SetMaterial ( Graphic3d_NOM_DEFAULT     ) ; }


/////////////////////////////////////////////////////////////////////////////
// CDialogTransparency dialog

CDialogTransparency::CDialogTransparency(Handle(AIS_InteractiveContext) CurrentIC,
										 CWnd* pParent /*=NULL*/)
	: CDialog(CDialogTransparency::IDD, pParent)
{
	
	//{{AFX_DATA_INIT(CDialogTransparency)
	m_TransValue = 0;
	//}}AFX_DATA_INIT

	myCurrentIC = CurrentIC;

	Standard_Real temp = 10;
	Standard_Real t;

	for (myCurrentIC->InitSelected();
	     myCurrentIC->MoreSelected ();
		 myCurrentIC->NextSelected ())
	{	
		t = CurrentIC->SelectedInteractive()->Transparency();
		if (temp > t)
			temp = t;
	    //myCurrentIC->SetTransparency (myCurrentIC->Current(), m_TransValue);
    }	

	m_TransValue = int (temp * 10);
	if (Abs(m_TransValue - temp * 10) > 0.01)
		m_TransValue = int (temp * 10) + 1;

	for (myCurrentIC->InitSelected();
	     myCurrentIC->MoreSelected ();
		 myCurrentIC->NextSelected ())
	{
	    myCurrentIC->SetTransparency (myCurrentIC->SelectedInteractive(), temp, Standard_False);
    }

  myCurrentIC->UpdateCurrentViewer();
}


void CDialogTransparency::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogTransparency)
	DDX_Text(pDX, IDC_EDITAISTRANSP, m_TransValue);
	DDV_MinMaxInt(pDX, m_TransValue, 0, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogTransparency, CDialog)
	//{{AFX_MSG_MAP(CDialogTransparency)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINAISTRANSP, OnDeltaposSpinaistransp)
	ON_EN_CHANGE(IDC_EDITAISTRANSP, OnChangeEditaistransp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogTransparency message handlers

void CDialogTransparency::OnDeltaposSpinaistransp(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if ((pNMUpDown->iDelta < 0) && (m_TransValue < 10)) 
		m_TransValue = m_TransValue + 1;
	if ((pNMUpDown->iDelta > 0) && (m_TransValue > 0)) 
		m_TransValue = m_TransValue - 1;

    UpdateData (false);

	for (myCurrentIC->InitSelected();
	     myCurrentIC->MoreSelected ();
		 myCurrentIC->NextSelected())
	{
	    myCurrentIC->SetTransparency (myCurrentIC->SelectedInteractive(), m_TransValue/10.0, Standard_False);
    }
  myCurrentIC->UpdateCurrentViewer();
	*pResult = 0;

}


void CDialogTransparency::OnChangeEditaistransp() 
{
	// TODO: Add your control notification handler code here
	int temp = m_TransValue;
    if (UpdateData (true)){
		for (myCurrentIC->InitSelected();
			 myCurrentIC->MoreSelected ();
			 myCurrentIC->NextSelected())
		{
			myCurrentIC->SetTransparency (myCurrentIC->SelectedInteractive(), m_TransValue/10.0, Standard_False);
		}
    myCurrentIC->UpdateCurrentViewer();
	}
	else{
		m_TransValue = temp;
	    UpdateData (false);
	}

}
