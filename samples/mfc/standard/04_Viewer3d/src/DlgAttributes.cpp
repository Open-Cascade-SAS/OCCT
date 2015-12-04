// DlgAttributes.cpp : implementation file
//

#include "stdafx.h"

#include "DlgAttributes.h"

#include "AISDisplayModeApp.h"

#include <AISDialogs.h>
#include <OCC_3dView.h>

#include <Graphic3d_MaterialAspect.hxx>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgAttributes dialog


DlgAttributes::DlgAttributes(CWnd* pParent)
	: CDialog(DlgAttributes::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgAttributes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void DlgAttributes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgAttributes)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgAttributes, CDialog)
	//{{AFX_MSG_MAP(DlgAttributes)
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
	ON_BN_CLICKED(ID_OBJECT_COLOR, OnColor)
	ON_BN_CLICKED(ID_OBJECT_SHADING, OnShading)
	ON_BN_CLICKED(ID_OBJECT_WIREFRAME, OnWireframe)
	ON_COMMAND(ID_OBJECT_TRANSPARENCY, OnTransparency)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_TRANSPARENCY, OnUpdateObjectTransparency)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgAttributes message handlers


void DlgAttributes::OnColor() 
{
	Handle(AIS_InteractiveObject) Current ;
	Quantity_Color CSFColor ;
	COLORREF MSColor ;

	myAISContext->InitCurrent();
	if (myAISContext->MoreCurrent()) {
		Current = myAISContext->Current() ;
		if ( Current->HasColor () ) {
		  CSFColor = Current->Color () ;
		  MSColor = RGB (CSFColor.Red()*255.,
		 						CSFColor.Green()*255.,
								CSFColor.Blue()*255.);
		}
		else {
		  MSColor = RGB (255,255,255) ;
		}
	
		CColorDialog dlgColor(MSColor);
		if (dlgColor.DoModal() == IDOK) {
			MSColor = dlgColor.GetColor();
			CSFColor = Quantity_Color (GetRValue(MSColor)/255.,
									   GetGValue(MSColor)/255.,
									   GetBValue(MSColor)/255.,Quantity_TOC_RGB); 
			for (myAISContext->InitCurrent(); 
				 myAISContext->MoreCurrent ();
				 myAISContext->NextCurrent ())
				myAISContext->SetColor (myAISContext->Current(),
                                     CSFColor.Name());
		}	
	}

TCollection_AsciiString Message ("\
\n\
for (myAISContext->InitCurrent(); myAISContext->MoreCurrent ();  \n\
	myAISContext->NextCurrent ())	    \n\
	myAISContext->SetColor (myAISContext->Current(), CSFColor.Name());  \n\
\n");

	CString text(Message.ToCString());
	(*myCResultDialog).SetTitle(CString("Setting Color"));
	(*myCResultDialog).SetText(text);
}


void DlgAttributes::OnWireframe() 
{
	for(myAISContext->InitCurrent(); myAISContext->MoreCurrent();
	myAISContext->NextCurrent())            
		myAISContext->SetDisplayMode(myAISContext->Current(), 0);	
		 
TCollection_AsciiString Message ("\
\n\
for (myAISContext->InitCurrent(); myAISContext->MoreCurrent ();  \n\
	myAISContext->NextCurrent ())	    \n\
	myAISContext->SetDisplayMode(myAISContext->Current(), 0);  \n\
\n");

	CString text(Message.ToCString());
	(*myCResultDialog).SetTitle(CString("Wireframe"));
	(*myCResultDialog).SetText(text);
}

void DlgAttributes::Set(Handle ( AIS_InteractiveContext ) & acontext, CResultDialog& aResultDialog)
{
	myAISContext = acontext;
	myCResultDialog=&aResultDialog;
}

void DlgAttributes::OnTransparency() 
{
/*
	for (myAISContext->InitCurrent(); myAISContext->MoreCurrent ();
		 myAISContext->NextCurrent ()) {
			
	
		int ival = (int) ((double) myAISContext->Current()->Transparency()*10.) ;
		double rval = ((double) ival)/10.;
		CDialogTransparency Dlg(NULL, rval);
		if(Dlg.DoModal()== IDOK) 
			myAISContext->SetTransparency (myAISContext->Current(),Dlg.m_transvalue);
	
	}	
*/
	/*
	CDialogTransparency DialBox(myAISContext);
	DialBox.DoModal();
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	OCC_3dView *pView = (OCC_3dView *) pChild->GetActiveView();
	pView->Redraw();
	*/
	myAISContext->InitCurrent();
	if(myAISContext->NbCurrents() > 0){
	CDialogTransparency DialBox(myAISContext, AfxGetMainWnd());
	DialBox.DoModal();
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	OCC_3dView *pView = (OCC_3dView *) pChild->GetActiveView();
	pView->Redraw();
	}

TCollection_AsciiString Message ("\
\n\
for (myAISContext->InitCurrent(); myAISContext->MoreCurrent ();  \n\
	myAISContext->NextCurrent ())	    \n\
	myAISContext->SetTransparency (myAISContext->Current(),Dlg.m_transvalue);  \n\
\n");

	CString text(Message.ToCString());
	(*myCResultDialog).SetTitle(CString("Setting Transparency"));
	(*myCResultDialog).SetText(text);
}



void DlgAttributes::OnObjectMaterialAluminium     () { SetMaterial ( Graphic3d_NOM_ALUMINIUM     ) ; }
void DlgAttributes::OnObjectMaterialBrass     () { SetMaterial ( Graphic3d_NOM_BRASS     ) ; }
void DlgAttributes::OnObjectMaterialBronze    () { SetMaterial ( Graphic3d_NOM_BRONZE    ) ; }
void DlgAttributes::OnObjectMaterialChrome    () { SetMaterial ( Graphic3d_NOM_CHROME     ) ; }
void DlgAttributes::OnObjectMaterialCopper    () { SetMaterial ( Graphic3d_NOM_COPPER    ) ; }
void DlgAttributes::OnObjectMaterialGold      () { SetMaterial ( Graphic3d_NOM_GOLD      ) ; }
void DlgAttributes::OnObjectMaterialJade     () { SetMaterial ( Graphic3d_NOM_JADE     ) ; }
void DlgAttributes::OnObjectMaterialMetalized     () { SetMaterial ( Graphic3d_NOM_METALIZED     ) ; }
void DlgAttributes::OnObjectMaterialNeonGNC     () { SetMaterial ( Graphic3d_NOM_NEON_GNC     ) ; }
void DlgAttributes::OnObjectMaterialNeonPHC     () { SetMaterial ( Graphic3d_NOM_NEON_PHC     ) ; }
void DlgAttributes::OnObjectMaterialObsidian     () { SetMaterial ( Graphic3d_NOM_OBSIDIAN     ) ; }
void DlgAttributes::OnObjectMaterialPewter    () { SetMaterial ( Graphic3d_NOM_PEWTER    ) ; }
void DlgAttributes::OnObjectMaterialPlaster   () { SetMaterial ( Graphic3d_NOM_PLASTER   ) ; }
void DlgAttributes::OnObjectMaterialPlastic   () { SetMaterial ( Graphic3d_NOM_PLASTIC   ) ; }
void DlgAttributes::OnObjectMaterialSatin     () { SetMaterial ( Graphic3d_NOM_SATIN     ) ; }
void DlgAttributes::OnObjectMaterialShinyPlastic     () { SetMaterial ( Graphic3d_NOM_SHINY_PLASTIC     ) ; }
void DlgAttributes::OnObjectMaterialSilver    () { SetMaterial ( Graphic3d_NOM_SILVER    ) ; }
void DlgAttributes::OnObjectMaterialSteel    () { SetMaterial ( Graphic3d_NOM_STEEL    ) ; }
void DlgAttributes::OnObjectMaterialStone    () { SetMaterial ( Graphic3d_NOM_STONE    ) ; }
void DlgAttributes::OnObjectMaterialDefault    () { SetMaterial ( Graphic3d_NOM_DEFAULT     ) ; }

void DlgAttributes::SetMaterial(Graphic3d_NameOfMaterial Material) 
{
  for (myAISContext->InitCurrent();myAISContext->MoreCurrent ();myAISContext->NextCurrent ())
		myAISContext->SetMaterial (myAISContext->Current(),
         (Graphic3d_NameOfMaterial)(Material));

  TCollection_AsciiString aString;
  TCollection_AsciiString Message1 ("\
\n\
for (myAISContext->InitCurrent(); myAISContext->MoreCurrent ();  \n\
	myAISContext->NextCurrent ())	    \n\
	myAISContext->SetMaterial (myAISContext->Current(), "); 

	TCollection_AsciiString Message2(");  \n\
\n");
  switch (Material){
	case 0:
		aString = "Graphic3d_NOM_BRASS";
		break;
	case 1:
		aString = "Graphic3d_NOM_BRONZE";
		break;
	case 2:
		aString = "Graphic3d_NOM_COPPER";
		break;
	case 3:
		aString = "Graphic3d_NOM_GOLD";
		break;
	case 4:
		aString = "Graphic3d_NOM_PEWTER";
		break;
	case 5:
		aString = "Graphic3d_NOM_PLASTER";
		break;
	case 6:
		aString = "Graphic3d_NOM_PLASTIC";
		break;
	case 7:
		aString = "Graphic3d_NOM_SILVER";
		break;
	case 8:
		aString = "Graphic3d_NOM_STEEL";
		break;
	case 9:
		aString = "Graphic3d_NOM_STONE";
		break;
	case 10:
		aString = "Graphic3d_NOM_SHINY_PLASTIC";
		break;
	case 11:
		aString = "Graphic3d_NOM_SATIN";
		break;
	case 12:
		aString = "Graphic3d_NOM_METALIZED";
		break;
	case 13:
		aString = "Graphic3d_NOM_NEON_GNC";
		break;
	case 14:
		aString = "Graphic3d_NOM_CHROME";
		break;
	case 15:
		aString = "Graphic3d_NOM_ALUMINIUM";
		break;
	case 16:
		aString = "Graphic3d_NOM_OBSIDIAN";
		break;
	case 17:
		aString = "Graphic3d_NOM_NEON_PHC";
		break;
	case 18:
		aString = "Graphic3d_NOM_JADE";
		break;
	case 19:
		aString = "Graphic3d_NOM_DEFAULT";
		break;
	default:
		break;
  }

	Message1 = Message1 +aString +Message2;
	//Graphic3d_MaterialAspect aAspect (Material);
	//Standard_CString aString = aAspect.MaterialName();

	CString text(Message1.ToCString());
	(*myCResultDialog).SetTitle(CString("Setting Material"));
	(*myCResultDialog).SetText(text);

}

void DlgAttributes::OnUpdateObjectTransparency(CCmdUI* pCmdUI) 
{
  bool OneOrMoreInShading = false;
  for (myAISContext->InitCurrent();myAISContext->MoreCurrent ();myAISContext->NextCurrent ())
    if (myAISContext->IsDisplayed(myAISContext->Current(),1)) OneOrMoreInShading=true;
	pCmdUI->Enable (OneOrMoreInShading);	
}



void DlgAttributes::OnShading() 
{
	for(myAISContext->InitCurrent(); myAISContext->MoreCurrent();
	myAISContext->NextCurrent())            
		myAISContext->SetDisplayMode(myAISContext->Current(), 1);	

		 
TCollection_AsciiString Message ("\
\n\
for (myAISContext->InitCurrent(); myAISContext->MoreCurrent ();  \n\
	myAISContext->NextCurrent ())	    \n\
	myAISContext->SetDisplayMode(myAISContext->Current(), 1);  \n\
\n");

	CString text(Message.ToCString());
	(*myCResultDialog).SetTitle(CString("Shading"));
	(*myCResultDialog).SetText(text);
}
