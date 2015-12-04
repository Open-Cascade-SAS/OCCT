// ShadingModelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Viewer3dApp.h"
#include "ShadingModelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShadingModelDlg dialog


CShadingModelDlg::CShadingModelDlg(Handle(V3d_View) Current_V3d_View, CWnd* pParent /*=NULL*/)
	: CDialog(CShadingModelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShadingModelDlg)
	myCurrent_V3d_View=Current_V3d_View;
	//}}AFX_DATA_INIT
}


void CShadingModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShadingModelDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShadingModelDlg, CDialog)
	//{{AFX_MSG_MAP(CShadingModelDlg)
	ON_BN_CLICKED(IDC_SHADINGMODEL_COLOR,   OnShadingmodelColor)
	ON_BN_CLICKED(IDC_SHADINGMODEL_FLAT,    OnShadingmodelFlat)
	ON_BN_CLICKED(IDC_SHADINGMODEL_GOURAUD, OnShadingmodelGouraud)
	ON_BN_CLICKED(IDC_SHADINGMODEL_PHONG,   OnShadingmodelPhong)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShadingModelDlg message handlers

void CShadingModelDlg::OnShadingmodelColor() 
{
	myCurrent_V3d_View->SetShadingModel(V3d_COLOR);
	myCurrent_V3d_View->Update();
}

void CShadingModelDlg::OnShadingmodelFlat() 
{
	myCurrent_V3d_View->SetShadingModel(V3d_FLAT);
	myCurrent_V3d_View->Update();	
}

void CShadingModelDlg::OnShadingmodelGouraud() 
{
	myCurrent_V3d_View->SetShadingModel(V3d_GOURAUD);
	myCurrent_V3d_View->Update();	
}

void CShadingModelDlg::OnShadingmodelPhong()
{
	myCurrent_V3d_View->SetShadingModel(V3d_PHONG);
	myCurrent_V3d_View->Update();
}
