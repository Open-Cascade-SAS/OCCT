// GeometryDoc.cpp : implementation of the CGeometryDoc class
//

#include "stdafx.h"

#include "GeometryDoc.h"
#include "GeoAlgo_Sol.hxx"
#include "GeometryApp.h"
#include "GeometryView2D.h"
#include "GeometryView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "GeomSources.h"

#define EOL "\r\n"

#define WAIT_A_LITTLE WaitForInput(500)

#define MAX_PARAM 1000 // if a surface parameter is infinite, it is assingned
// this value in order to display the "infinit" object in the viewer.

/////////////////////////////////////////////////////////////////////////////
// CGeometryDoc

IMPLEMENT_DYNCREATE(CGeometryDoc, OCC_3dBaseDoc)

BEGIN_MESSAGE_MAP(CGeometryDoc, OCC_3dBaseDoc)
  //{{AFX_MSG_MAP(CGeometryDoc)
  ON_COMMAND(ID_WINDOW_NEW2D, OnWindowNew2d)
  ON_COMMAND(ID_BUTTON_Test_1, OnBUTTONTest1)
  ON_COMMAND(ID_BUTTON_Test_2, OnBUTTONTest2)
  ON_COMMAND(ID_BUTTON_Test_3, OnBUTTONTest3)
  ON_COMMAND(ID_BUTTON_Test_4, OnBUTTONTest4)
  ON_COMMAND(ID_BUTTON_Test_5, OnBUTTONTest5)
  ON_COMMAND(ID_BUTTON_Test_6, OnBUTTONTest6)
  ON_COMMAND(ID_BUTTON_Test_7, OnBUTTONTest7)
  ON_COMMAND(ID_BUTTON_Test_8, OnBUTTONTest8)
  ON_COMMAND(ID_BUTTON_Test_9, OnBUTTONTest9)
  ON_COMMAND(ID_BUTTON_Test_23, OnBUTTONTest23)
  ON_COMMAND(ID_BUTTON_Test_22, OnBUTTONTest22)
  ON_COMMAND(ID_BUTTON_Test_10, OnBUTTONTest10)
  ON_COMMAND(ID_BUTTON_Test_11, OnBUTTONTest11)
  ON_COMMAND(ID_BUTTON_Test_12, OnBUTTONTest12)
  ON_COMMAND(ID_BUTTON_Test_13, OnBUTTONTest13)
  ON_COMMAND(ID_BUTTON_Test_14, OnBUTTONTest14)
  ON_COMMAND(ID_BUTTON_Test_15, OnBUTTONTest15)
  ON_COMMAND(ID_BUTTON_Test_16, OnBUTTONTest16)
  ON_COMMAND(ID_BUTTON_Test_17, OnBUTTONTest17)
  ON_COMMAND(ID_BUTTON_Test_18, OnBUTTONTest18)
  ON_COMMAND(ID_BUTTON_Test_19, OnBUTTONTest19)
  ON_COMMAND(ID_BUTTON_Test_20, OnBUTTONTest20)
  ON_COMMAND(ID_BUTTON_Test_21, OnBUTTONTest21)
  ON_COMMAND(ID_BUTTON_Test_24, OnBUTTONTest24)
  ON_COMMAND(ID_BUTTON_Test_25, OnBUTTONTest25)
  ON_COMMAND(ID_BUTTON_Test_26, OnBUTTONTest26)
  ON_COMMAND(ID_BUTTON_Test_27, OnBUTTONTest27)
  ON_COMMAND(ID_BUTTON_Test_28, OnBUTTONTest28)
  ON_COMMAND(ID_BUTTON_Test_29, OnBUTTONTest29)
  ON_COMMAND(ID_BUTTON_Test_30, OnBUTTONTest30)
  ON_COMMAND(ID_BUTTON_Test_31, OnBUTTONTest31)
  ON_COMMAND(ID_BUTTON_Test_32, OnBUTTONTest32)
  ON_COMMAND(ID_BUTTON_Test_33, OnBUTTONTest33)
  ON_COMMAND(ID_BUTTON_Test_34, OnBUTTONTest34)
  ON_COMMAND(ID_BUTTON_Test_35, OnBUTTONTest35)
  ON_COMMAND(ID_BUTTON_Test_36, OnBUTTONTest36)
  ON_COMMAND(ID_BUTTON_Test_37, OnBUTTONTest37)
  ON_COMMAND(ID_BUTTON_Test_38, OnBUTTONTest38)
  ON_COMMAND(ID_BUTTON_Test_39, OnBUTTONTest39)
  ON_COMMAND(ID_BUTTON_Test_40, OnBUTTONTest40)
  ON_COMMAND(ID_BUTTON_Test_41, OnBUTTONTest41)
  ON_COMMAND(ID_BUTTON_Test_42, OnBUTTONTest42)
  ON_COMMAND(ID_BUTTON_Test_43, OnBUTTONTest43)
  ON_COMMAND(ID_BUTTON_Test_44, OnBUTTONTest44)
  ON_COMMAND(ID_BUTTON_Test_45, OnBUTTONTest45)
  ON_COMMAND(ID_BUTTON_Test_46, OnBUTTONTest46)
  ON_COMMAND(ID_BUTTON_Test_47, OnBUTTONTest47)
  ON_COMMAND(ID_BUTTON_Test_48, OnBUTTONTest48)
  ON_COMMAND(ID_BUTTON_Test_49, OnBUTTONTest49)
  ON_COMMAND(ID_BUTTON_Test_50, OnBUTTONTest50)
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_1  , OnUpdateBUTTONTest1  )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_2  , OnUpdateBUTTONTest2  )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_3  , OnUpdateBUTTONTest3  )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_4  , OnUpdateBUTTONTest4  )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_5  , OnUpdateBUTTONTest5  )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_6  , OnUpdateBUTTONTest6  )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_7  , OnUpdateBUTTONTest7  )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_8  , OnUpdateBUTTONTest8  )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_9  , OnUpdateBUTTONTest9  )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_10 , OnUpdateBUTTONTest10 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_11 , OnUpdateBUTTONTest11 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_12 , OnUpdateBUTTONTest12 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_13 , OnUpdateBUTTONTest13 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_14 , OnUpdateBUTTONTest14 ) 
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_15 , OnUpdateBUTTONTest15 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_16 , OnUpdateBUTTONTest16 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_17 , OnUpdateBUTTONTest17 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_18 , OnUpdateBUTTONTest18 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_19 , OnUpdateBUTTONTest19 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_20 , OnUpdateBUTTONTest20 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_21 , OnUpdateBUTTONTest21 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_22 , OnUpdateBUTTONTest22 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_23 , OnUpdateBUTTONTest23 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_24 , OnUpdateBUTTONTest24 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_25 , OnUpdateBUTTONTest25 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_26 , OnUpdateBUTTONTest26 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_27 , OnUpdateBUTTONTest27 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_28 , OnUpdateBUTTONTest28 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_29 , OnUpdateBUTTONTest29 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_30 , OnUpdateBUTTONTest30 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_31 , OnUpdateBUTTONTest31 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_32 , OnUpdateBUTTONTest32 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_33 , OnUpdateBUTTONTest33 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_34 , OnUpdateBUTTONTest34 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_35 , OnUpdateBUTTONTest35 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_36 , OnUpdateBUTTONTest36 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_37 , OnUpdateBUTTONTest37 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_38 , OnUpdateBUTTONTest38 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_39 , OnUpdateBUTTONTest39 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_40 , OnUpdateBUTTONTest40 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_41 , OnUpdateBUTTONTest41 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_42 , OnUpdateBUTTONTest42 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_43 , OnUpdateBUTTONTest43 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_44 , OnUpdateBUTTONTest44 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_45 , OnUpdateBUTTONTest45 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_46 , OnUpdateBUTTONTest46 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_47 , OnUpdateBUTTONTest47 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_48 , OnUpdateBUTTONTest48 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_49 , OnUpdateBUTTONTest49 )
  ON_UPDATE_COMMAND_UI(ID_BUTTON_Test_50 , OnUpdateBUTTONTest50 )
  ON_COMMAND(ID_Create_Sol, OnCreateSol)
  ON_COMMAND(ID_BUTTON_Simplify, OnSimplify)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeometryDoc construction/destruction

CGeometryDoc::CGeometryDoc()
: OCC_3dBaseDoc()
{
  FitMode = false;
  AfxInitRichEdit();

  myAISContext->DefaultDrawer()->UIsoAspect()->SetNumber(11);
  myAISContext->DefaultDrawer()->VIsoAspect()->SetNumber(11);

  Handle(Graphic3d_GraphicDriver) aGraphicDriver = ((OCC_App*)AfxGetApp())->GetGraphicDriver();
  myViewer2D = new V3d_Viewer (aGraphicDriver);
  myViewer2D->SetCircularGridValues(0,0,1,8,0);
  myViewer2D->SetRectangularGridValues(0,0,1,1,0);

  // Set view projection
  myViewer2D->SetDefaultViewProj(V3d_Zpos);
  myAISContext2D = new AIS_InteractiveContext(myViewer2D);	
  myCResultDialog.Create(CResultDialog::IDD,NULL);

  RECT dlgrect;
  myCResultDialog.GetWindowRect(&dlgrect);
  LONG width = dlgrect.right-dlgrect.left;
  LONG height = dlgrect.bottom-dlgrect.top;
  RECT MainWndRect;
  AfxGetApp()->m_pMainWnd->GetWindowRect(&MainWndRect);
  LONG left = MainWndRect.left+3;
  LONG top = MainWndRect.top + 138;
  myCResultDialog.MoveWindow(left,top,width,height);

  ((CGeometryApp*)AfxGetApp())->CreateView2D(this);	
  Minimize2D();
  Put3DOnTop();
}

CGeometryDoc::~CGeometryDoc()
{
}

BOOL CGeometryDoc::OnNewDocument()
{
  if (!CDocument::OnNewDocument())
    return FALSE;

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)
  // compute a graphic device --> the same for all Views

  return TRUE;
}

void CGeometryDoc::OnWindowNew2d()
{
  ((CGeometryApp*)AfxGetApp())->CreateView2D(this);
}

/////////////////////////////////////////////////////////////////////////////
// CGeometryDoc serialization

void CGeometryDoc::Serialize(CArchive& ar)
{
  if (ar.IsStoring())
  {
    // TODO: add storing code here
  }
  else
  {
    // TODO: add loading code here
  }
}

/////////////////////////////////////////////////////////////////////////////
// CGeometryDoc diagnostics

#ifdef _DEBUG
void CGeometryDoc::AssertValid() const
{
  CDocument::AssertValid();
}

void CGeometryDoc::Dump(CDumpContext& dc) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGeometryDoc commands

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::DragEvent2D(const Standard_Integer /*x*/,
                               const Standard_Integer /*y*/,
                               const Standard_Integer /*TheState*/,
                               const Handle(V3d_View)& /*aView*/)
{
}


//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::InputEvent2D(const Standard_Integer /*x*/,
                                const Standard_Integer /*y*/,
                                const Handle(V3d_View)& /*aView*/)
{
  myAISContext2D->Select(Standard_True);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::MoveEvent2D(const Standard_Integer x,
                               const Standard_Integer y,
                               const Handle(V3d_View)& aView)
{
  if(aView->Viewer()->Grid()->IsActive())
  {
    Standard_Real aGridX=0,aGridY=0,aGridZ=0;
    aView->ConvertToGrid(x,y,aGridX,aGridY,aGridZ);
    //View is not updated automatically in ConvertToGrid
    aView->Update();
  }
  this->myAISContext2D->MoveTo (x, y, aView, Standard_True);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::ShiftMoveEvent2D(const Standard_Integer /*x*/,
                                    const Standard_Integer /*y*/,
                                    const Handle(V3d_View)& /*aView*/)
{
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::ShiftDragEvent2D(const Standard_Integer /*x*/,
                                    const Standard_Integer /*y*/,
                                    const Standard_Integer /*TheState*/,
                                    const Handle(V3d_View)& /*aView*/)
{
}


//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::ShiftInputEvent2D(const Standard_Integer /*x*/,
                                     const Standard_Integer /*y*/,
                                     const Handle(V3d_View)& /*aView*/)
{
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void  CGeometryDoc::Popup2D(const Standard_Integer x,
                            const Standard_Integer y,
                            const Handle(V3d_View)& aView)
{
  CMenu menu;
  VERIFY(menu.LoadMenu(IDR_Popup3D));
  CMenu* pPopup; 

  pPopup = menu.GetSubMenu(0);

  ASSERT(pPopup != NULL);

  POINT winCoord = { x , y };
  Handle(WNT_Window) aWNTWindow=
    Handle(WNT_Window)::DownCast(aView->Window());
  ClientToScreen ( (HWND)(aWNTWindow->HWindow()),&winCoord);
  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON , winCoord.x, winCoord.y , 
    AfxGetMainWnd());
}


//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void  CGeometryDoc::Popup (const Standard_Integer theMouseX,
                           const Standard_Integer theMouseY,
                           const Handle(V3d_View)& theView) 
{ 
  Standard_Integer PopupMenuNumber=0;
  GetAISContext()->InitSelected();
  if (GetAISContext()->MoreSelected())
  {
    PopupMenuNumber=1;
  }

  CMenu menu;
  VERIFY(menu.LoadMenu(IDR_Popup3D));
  CMenu* pPopup = menu.GetSubMenu(PopupMenuNumber);
  ASSERT(pPopup != NULL);

  POINT winCoord = { theMouseX , theMouseY };
  Handle(WNT_Window) aWNTWindow = Handle(WNT_Window)::DownCast(theView->Window());
  ClientToScreen ( (HWND)(aWNTWindow->HWindow()), &winCoord );

  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON ,
                         winCoord.x, winCoord.y , 
                         AfxGetMainWnd());
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::InputEvent (const Standard_Integer /*theMouseX*/,
                                const Standard_Integer /*theMouseY*/,
                                const Handle(V3d_View)& /*theView*/)
{
  myAISContext->Select (Standard_True);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::Put2DOnTop(bool isMax)
{
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
    if(pCurrentView->IsKindOf( RUNTIME_CLASS( CGeometryView2D ) ) )
    {
      ASSERT_VALID(pCurrentView);
      CFrameWnd* pParentFrm = pCurrentView->GetParentFrame();
      ASSERT(pParentFrm != (CFrameWnd *)NULL);
      // simply make the frame window visible
      if(isMax)
      {
        pParentFrm->ActivateFrame(SW_SHOWMAXIMIZED);
      }
      else
      {
        pParentFrm->ActivateFrame(SW_SHOW);
      }
    }
  }
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::Minimize2D()
{
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
    if(pCurrentView->IsKindOf( RUNTIME_CLASS( CGeometryView2D ) ) )
    {
      ASSERT_VALID(pCurrentView);
      CFrameWnd* pParentFrm = pCurrentView->GetParentFrame();
      ASSERT(pParentFrm != (CFrameWnd *)NULL);
      // simply make the frame window visible
      pParentFrm->ActivateFrame(SW_HIDE);
    }
  }
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::Fit2DViews()
{
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
    if(pCurrentView->IsKindOf( RUNTIME_CLASS( CGeometryView2D ) ) )
    {
      ASSERT_VALID(pCurrentView);
      CGeometryView2D* aCGeometryView2D = (CGeometryView2D*)pCurrentView;
      aCGeometryView2D->FitAll();
    }
  }
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::Put3DOnTop(bool isMax)
{
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
    if(pCurrentView->IsKindOf( RUNTIME_CLASS( CGeometryView ) ) )
    {
      ASSERT_VALID(pCurrentView);
      CFrameWnd* pParentFrm = pCurrentView->GetParentFrame();
      ASSERT(pParentFrm != (CFrameWnd *)NULL);
      // simply make the frame window visible
      if(isMax)
      {
        pParentFrm->ActivateFrame(SW_SHOWMAXIMIZED);
      }
      else
      {
        pParentFrm->ActivateFrame(SW_SHOW);
      }
    }
  }
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::Minimize3D()
{
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
    if(pCurrentView->IsKindOf( RUNTIME_CLASS( CGeometryView ) ) )
    {
      ASSERT_VALID(pCurrentView);
      CFrameWnd* pParentFrm = pCurrentView->GetParentFrame();
      ASSERT(pParentFrm != (CFrameWnd *)NULL);
      // simply make the frame window visible
      pParentFrm->ActivateFrame(SW_HIDE);
    }
  }
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::Fit3DViews(Standard_Real Coef)
{
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
    if(pCurrentView->IsKindOf( RUNTIME_CLASS( CGeometryView ) ) )
    {
      ASSERT_VALID(pCurrentView);
      CGeometryView* aCGeometryView = (CGeometryView*)pCurrentView;
      aCGeometryView->FitAll(Coef);
    }
  }
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryDoc::Set3DViewsZoom(const Standard_Real& Coef)
{
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
    if(pCurrentView->IsKindOf( RUNTIME_CLASS( CGeometryView ) ) )
    {
      ASSERT_VALID(pCurrentView);
      CGeometryView* aCGeometryView = (CGeometryView*)pCurrentView;
      aCGeometryView->SetZoom( Coef );
    }
  }
}

//-----------------------------------------------------------------------------------------
//                                Buttons event handlers
//-----------------------------------------------------------------------------------------
void CGeometryDoc::OnBUTTONTest1() 
{   Current = 1;
GeomSources::gpTest1(this);      }
void CGeometryDoc::OnBUTTONTest2() 
{   Current = 2;
GeomSources::gpTest2(this);      }
void CGeometryDoc::OnBUTTONTest3() 
{   Current = 3;
GeomSources::gpTest3(this);      }
void CGeometryDoc::OnBUTTONTest4() 
{   Current = 4;
GeomSources::gpTest4(this);      }
void CGeometryDoc::OnBUTTONTest5() 
{   Current = 5;
GeomSources::gpTest5(this);	   }
void CGeometryDoc::OnBUTTONTest6() 
{   Current = 6;
GeomSources::gpTest6(this);      }
void CGeometryDoc::OnBUTTONTest7() 
{   Current = 7;
GeomSources::gpTest7(this);      }
void CGeometryDoc::OnBUTTONTest8() 
{   Current = 8;
GeomSources::gpTest8(this);      }
void CGeometryDoc::OnBUTTONTest9() 
{   Current = 9;
GeomSources::gpTest9(this);	   }
void CGeometryDoc::OnBUTTONTest10() 
{   Current = 10;
GeomSources::gpTest10(this);	}
void CGeometryDoc::OnBUTTONTest11() 
{   Current = 11;
GeomSources::gpTest11(this);	}
void CGeometryDoc::OnBUTTONTest12() 
{   Current = 12;
GeomSources::gpTest12(this);	}
void CGeometryDoc::OnBUTTONTest13() 
{   Current = 13;
GeomSources::gpTest13(this);	}
void CGeometryDoc::OnBUTTONTest14() 
{   Current = 14;
GeomSources::gpTest14(this);	}
void CGeometryDoc::OnBUTTONTest15() 
{   Current = 15;
GeomSources::gpTest15(this);	}
void CGeometryDoc::OnBUTTONTest16() 
{   Current = 16;
GeomSources::gpTest16(this);	}
void CGeometryDoc::OnBUTTONTest17() 
{   Current = 17;
GeomSources::gpTest17(this);	}
void CGeometryDoc::OnBUTTONTest18() 
{   Current = 18;
GeomSources::gpTest18(this);	}
void CGeometryDoc::OnBUTTONTest19() 
{   Current = 19;
GeomSources::gpTest19(this);	}
void CGeometryDoc::OnBUTTONTest20() 
{   Current = 20;
GeomSources::gpTest20(this);	}
void CGeometryDoc::OnBUTTONTest21() 
{   Current = 21;
GeomSources::gpTest21(this);	}
void CGeometryDoc::OnBUTTONTest22() 
{   Current = 22;
GeomSources::gpTest22(this);	}
void CGeometryDoc::OnBUTTONTest23() 
{   Current = 23;
GeomSources::gpTest23(this);	}
void CGeometryDoc::OnBUTTONTest24() 
{   Current = 24;
GeomSources::gpTest24(this);	}
void CGeometryDoc::OnBUTTONTest25() 
{   Current = 25;
GeomSources::gpTest25(this);	}
void CGeometryDoc::OnBUTTONTest26() 
{   Current = 26;
GeomSources::gpTest26(this);	}
void CGeometryDoc::OnBUTTONTest27() 
{   Current = 27;
GeomSources::gpTest27(this);	}
void CGeometryDoc::OnBUTTONTest28() 
{   Current = 28;
GeomSources::gpTest28(this);	}
void CGeometryDoc::OnBUTTONTest29() 
{   Current = 29;
GeomSources::gpTest29(this);	}
void CGeometryDoc::OnBUTTONTest30() 
{   Current = 30;
GeomSources::gpTest30(this);	}
void CGeometryDoc::OnBUTTONTest31() 
{   Current = 31;
GeomSources::gpTest31(this);	}
void CGeometryDoc::OnBUTTONTest32() 
{   Current = 32;
GeomSources::gpTest32(this);  }
void CGeometryDoc::OnBUTTONTest33() 
{   Current = 33;
GeomSources::gpTest33(this);	}
void CGeometryDoc::OnBUTTONTest34() 
{   Current = 34;
GeomSources::gpTest34(this);	}
void CGeometryDoc::OnBUTTONTest35() 
{   Current = 35;
GeomSources::gpTest35(this);	}
void CGeometryDoc::OnBUTTONTest36() 
{   Current = 36;
GeomSources::gpTest36(this);	}

void CGeometryDoc::OnBUTTONTest37() 
{   Current = 37;
GeomSources::gpTest37(this);	}
void CGeometryDoc::OnBUTTONTest38() 
{   Current = 38;
GeomSources::gpTest38(this);	}
void CGeometryDoc::OnBUTTONTest39() 
{   Current = 39;
GeomSources::gpTest39(this);	}
void CGeometryDoc::OnBUTTONTest40() 
{   Current = 40;
GeomSources::gpTest40(this);	}
void CGeometryDoc::OnBUTTONTest41() 
{   Current = 41;
GeomSources::gpTest41(this);	}
void CGeometryDoc::OnBUTTONTest42() 
{   Current = 42;
GeomSources::gpTest42(this);	}
void CGeometryDoc::OnBUTTONTest43() 
{   Current = 43;
GeomSources::gpTest43(this);	}
void CGeometryDoc::OnBUTTONTest44() 
{   Current = 44;
GeomSources::gpTest44(this);	}
void CGeometryDoc::OnBUTTONTest45() 
{   Current = 45;
GeomSources::gpTest45(this);	}
void CGeometryDoc::OnBUTTONTest46() 
{   Current = 46;
GeomSources::gpTest46(this);	}
void CGeometryDoc::OnBUTTONTest47() 
{   Current = 47;
GeomSources::gpTest47(this);	}
void CGeometryDoc::OnBUTTONTest48() 
{   Current = 48;
GeomSources::gpTest48(this);	}
void CGeometryDoc::OnBUTTONTest49() 
{   Current = 49;
GeomSources::gpTest49(this);	}
void CGeometryDoc::OnBUTTONTest50() 
{   Current = 50;
GeomSources::gpTest50(this);	}

void CGeometryDoc::OnUpdateBUTTONTest1(CCmdUI* pCmdUI) 
{
  if (Current == 1)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest2(CCmdUI* pCmdUI) 
{
  if (Current == 2)
    pCmdUI->SetCheck(true);
  else 
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest3(CCmdUI* pCmdUI) 
{
  if (Current == 3)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest4(CCmdUI* pCmdUI) 
{
  if (Current == 4)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest5(CCmdUI* pCmdUI) 
{
  if (Current == 5)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest6(CCmdUI* pCmdUI) 
{
  if (Current == 6)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest7(CCmdUI* pCmdUI) 
{
  if (Current == 7)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest8(CCmdUI* pCmdUI) 
{
  if (Current == 8)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest9(CCmdUI* pCmdUI) 
{
  if (Current == 9)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest10(CCmdUI* pCmdUI) 
{
  if (Current == 10)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest11(CCmdUI* pCmdUI) 
{
  if (Current == 11)
    pCmdUI->SetCheck(true);
  else 
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest12(CCmdUI* pCmdUI) 
{
  if (Current == 12)
    pCmdUI->SetCheck(true);
  else pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest13(CCmdUI* pCmdUI) 
{
  if (Current == 13)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest14(CCmdUI* pCmdUI) 
{
  if (Current == 14)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest15(CCmdUI* pCmdUI)
{
  if(Current == 15)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest16(CCmdUI* pCmdUI)
{
  if (Current == 16)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest17(CCmdUI* pCmdUI)
{
  if (Current == 17)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest18(CCmdUI* pCmdUI)
{
  if (Current == 18)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest19(CCmdUI* pCmdUI)
{
  if (Current == 19)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest20(CCmdUI* pCmdUI)
{
  if (Current == 20)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest21(CCmdUI* pCmdUI)
{
  if (Current == 21)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest22(CCmdUI* pCmdUI)
{
  if (Current == 22)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest23(CCmdUI* pCmdUI)
{
  if (Current == 23)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest24(CCmdUI* pCmdUI)
{
  if (Current == 24)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest25(CCmdUI* pCmdUI)
{
  if (Current == 25)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest26(CCmdUI* pCmdUI)
{
  if (Current == 26)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest27(CCmdUI* pCmdUI)
{
  if (Current == 27)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest28(CCmdUI* pCmdUI)
{
  if (Current == 28)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest29(CCmdUI* pCmdUI)
{
  if (Current == 29)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest30(CCmdUI* pCmdUI)
{
  if (Current == 30)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest31(CCmdUI* pCmdUI)
{
  if (Current == 31)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest32(CCmdUI* pCmdUI)
{
  if (Current == 32)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest33(CCmdUI* pCmdUI)
{
  if (Current == 33)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest34(CCmdUI* pCmdUI)
{
  if (Current == 34)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest35(CCmdUI* pCmdUI)
{
  if (Current == 35)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest36(CCmdUI* pCmdUI)
{
  if (Current == 36)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest37(CCmdUI* pCmdUI)
{
  if (Current == 37)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest38(CCmdUI* pCmdUI)
{
  if (Current == 38)
    pCmdUI->SetCheck(true);
else
pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest39(CCmdUI* pCmdUI)
{
  if (Current == 39)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest40(CCmdUI* pCmdUI)
{
  if (Current == 40)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest41(CCmdUI* pCmdUI)
{
  if (Current == 41)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest42(CCmdUI* pCmdUI)
{
  if (Current == 42)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest43(CCmdUI* pCmdUI)
{
  if (Current == 43)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest44(CCmdUI* pCmdUI)
{
  if (Current == 44)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest45(CCmdUI* pCmdUI)
{
  if (Current == 45)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest46(CCmdUI* pCmdUI)
{
  if (Current == 46)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest47(CCmdUI* pCmdUI)
{
  if (Current == 47)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest48(CCmdUI* pCmdUI)
{
  if (Current == 48)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest49(CCmdUI* pCmdUI)
{
  if (Current == 49)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnUpdateBUTTONTest50(CCmdUI* pCmdUI)
{
  if (Current == 50)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

void CGeometryDoc::OnCloseDocument() 
{
  // TODO: Add your specialized code here and/or call the base class
  CDocument::OnCloseDocument();
}

void CGeometryDoc::OnCreateSol() 
{
  // TODO: Add your command handler code here
  // Creation d'un sol
  CFileDialog dlg (TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                   L"Points Files (*.dat)|*.dat; |All Files (*.*)|*.*||", NULL);

  CString anOCCTDataPathValue;
  anOCCTDataPathValue.GetEnvironmentVariable(L"CSF_OCCTDataPath");
  CString initdir = (anOCCTDataPathValue + L"\\occ\\SurfaceFromPoints");

  dlg.m_ofn.lpstrInitialDir = initdir;

  if (dlg.DoModal() == IDOK) 
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    CString filename = dlg.GetPathName();

    filebuf fic;
    istream in(&fic);  
    if (!fic.open (filename, ios::in))
      MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : Unable to open file", L"CasCade Error", MB_ICONERROR);

    TColgp_SequenceOfXYZ seqOfXYZ;
    gp_XYZ pntXYZ;
    Standard_Integer nbPnt=0;
    Standard_Real x,y,z;
    BRep_Builder B;
    TopoDS_Compound C;
    B.MakeCompound(C);
    while (!in.fail()|| !in.eof())
    {
      if (in >> x && in >> y && in >> z){
        pntXYZ.SetX(x);
        pntXYZ.SetY(y);
        pntXYZ.SetZ(z);
        nbPnt++;
        seqOfXYZ.Append(pntXYZ);
        BRepBuilderAPI_MakeVertex V(gp_Pnt(x, y, z));
        B.Add(C,V.Vertex());	    
      }
    }
    fic.close();
    Handle(AIS_Shape) anAISCompound = new AIS_Shape(C);
    myAISContext->Display(anAISCompound, Standard_False);
    Fit();
    Sleep(1000);
    GeoAlgo_Sol sol;
    sol.Build(seqOfXYZ);

    if (sol.IsDone() == Standard_True)
    {
      Handle(Geom_BSplineSurface) GeomSol = sol.Surface();
      TopoDS_Face aface = BRepBuilderAPI_MakeFace(GeomSol, Precision::Confusion());
      if (!BRepAlgo::IsValid(aface))
        MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : The plate surface is not valid!", L"CasCade Error", MB_ICONERROR);
      Handle(AIS_Shape) anAISShape=new AIS_Shape(aface);
      myAISContext->Display(anAISShape, Standard_False);
      Fit();
    }   
    else
      MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : Computation has failed", L"CasCade Error", MB_ICONERROR);
  }
}

/*********************************************************************************************
**********************  S I M P L I F Y  *****************************************************
*********************************************************************************************/

//================================================================
// Function : fixParam
// Purpose  : assings a finite value to theParam if it intinite
//            (equal to +- Precision::Infinite())
//================================================================
static Standard_Boolean fixParam(Standard_Real& theParam)
{
  Standard_Boolean aResult = Standard_False;
  if (Precision::IsNegativeInfinite(theParam))
  {
    theParam = -MAX_PARAM;
    aResult = Standard_True;
  }
  if (Precision::IsPositiveInfinite(theParam))
  {
    theParam = MAX_PARAM;
    aResult = Standard_True;
  }
  return aResult;
}


void CGeometryDoc::OnSimplify() 
{
  CString anOCCTDataPathValue;
  anOCCTDataPathValue.GetEnvironmentVariable(L"CSF_OCCTDataPath");
  CString initfile = (anOCCTDataPathValue + L"\\occ\\shell1.brep");

  std::filebuf aFileBuf;
  std::istream aStream (&aFileBuf);
  if (!aFileBuf.open (initfile, ios::in))
  {
    initfile += L" was not found. The sample can not be shown.";
    myCResultDialog.SetText (initfile);
    return;
  }

  TopoDS_Shape aShape;
  BRep_Builder aBld;
  BRepTools::Read (aShape, aStream, aBld);
  if (aShape.IsNull())
  {
    initfile += L" is invalid file. The sample can not be shown.";
    myCResultDialog.SetText(initfile);
    return;
  }
  myAISContext->SetDisplayMode(AIS_Shaded, Standard_True);
  simplify(aShape);
}

void CGeometryDoc::simplify(const TopoDS_Shape& aShape)
{
    myCResultDialog.SetTitle("Simplify Face");
    myCResultDialog.SetText("  TopoDS_Shape aShape;\n"
    "\n"
    "  // initialize aShape\n"
    "  //aShape = ...\n"
    "\n"
    "  // define parameter triangulation\n"
    "  Standard_Real aDeflection = 0.1;\n"
    "  \n"
    "  // removes all the triangulations of the faces ,\n"
    "  //and all the polygons on the triangulations of the edges\n"
    "  BRepTools::Clean(aShape);\n"
    "  // adds a triangulation of the shape aShape with the deflection aDeflection\n"
    "  BRepMesh::Mesh(aShape,aDeflection);\n"
    "\n"
    "  Standard_Integer aIndex = 1, nbNodes = 0;\n"
    "  \n"
    "  // define two sequence of points\n"
    "  TColgp_SequenceOfPnt aPoints, aPoints1;\n"
    " \n"
    "  // triangulation\n"
    "  for(TopExp_Explorer aExpFace(aShape,TopAbs_FACE); aExpFace.More(); aExpFace.Next())\n"
    "  {  \n"
    "    TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());\n"
    "    TopLoc_Location aLocation;\n"
    "\n"
    "    // takes the triangulation of the face aFace\n"
    "    Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace,aLocation);\n"
    "\n"
    "    if(!aTr.IsNull())\n"
    "    { \n"
    "      // takes the array of nodes for this triangulation\n"
    "      const TColgp_Array1OfPnt& aNodes = aTr->Nodes();    \n"
    "      nbNodes = aNodes.Length();\n"
    "\n"
    "      for( Standard_Integer i = 1; i <= nbNodes; i++)\n"
    "      {\n"
    "        // create seguence of node points in absolute coordinate system\n"
    "        gp_Pnt aPnt = aNodes(i).Transformed(aLocation);\n"
    "        aPoints.Append(aPnt);\n"
    "        \n"
    "      }\n"
    "    }\n"
    "  }\n"
    " \n"
    "  // remove double points\n"
    "  nbNodes = aPoints.Length();\n"
    "  for( Standard_Integer i = 1; i <= nbNodes; i++)\n"
    "  {\n"
    "    gp_Pnt aPi = aPoints(i);\n"
    "    for( Standard_Integer j = i + 1; j < nbNodes; j++)\n"
    "    {\n"
    "      gp_Pnt aPj = aPoints(j);\n"
    "      if(!aPi.IsEqual(aPj,0.9))\n"
    "        aIndex++;\n"
    "    }\n"
    "    if(aIndex == j - 1)\n"
    "      aPoints1.Append(aPi);\n"
    "\n"
    "    aIndex = i + 1;\n"
    "  }\n"
    "\n"
    "  // find max point\n"
    "  aIndex = 0;\n"
    "  gp_Pnt aPntMax = aPoints1(1);\n"
    "  nbNodes = aPoints1.Length();\n"
    "  for(i = 2; i <= nbNodes; i++)\n"
    "  {\n"
    "    if(aPoints1(i).X() > aPntMax.X())\n"
    "    {\n"
    "      aIndex = i;\n"
    "      aPntMax = aPoints1(aIndex);      \n"
    "    } \n"
    "  }\n"
    "\n"
    "  // clear seguence\n"
    "  aPoints.Clear();\n"
    "\n"
    "  Standard_Integer nbLeftNodes = nbNodes;\n"
    "\n"
    "  // ascending sort - fill aPoints with ascending \n"
    "  // by X coordinate points from aPoints1\n"
    "  for(i = 1; i < nbNodes; i++)\n"
    "  {\n"
    "    Standard_Real aMin = aPntMax.X();\n"
    "    aIndex = 1;\n"
    "    for( Standard_Integer j = 1; j <= nbLeftNodes; j++)\n"
    "    {\n"
    "      if(aPoints1(j).X() < aMin)\n"
    "      {\n"
    "        aMin = aPoints1(j).X();\n"
    "        aIndex = j;\n"
    "      } \n"
    "    }\n"
    "    aPoints.Append(aPoints1(aIndex));\n"
    "    aPoints1.Remove(aIndex);\n"
    "    nbLeftNodes = aPoints1.Length();\n"
    "  }\n"
    "  aPoints.Append(aPntMax);\n"
    "\n"
    "  // define parameters GeomPlate_BuildPlateSurface\n"
    "  Standard_Integer Degree = 3;\n"
    "  Standard_Integer NbPtsOnCur = 10;\n"
    "  Standard_Integer NbIter = 3;\n"
    "  Standard_Integer Order = 0;\n"
    "  Standard_Integer MaxSeg = 9;\n"
    "  Standard_Integer MaxDegree = 5;\n"
    "  Standard_Real dmax, anApproxTol = 0.001;\n"
    "  Standard_Real aConstrTol = Precision::Confusion();\n"
    "  \n"
    "  // define object BuildPlateSurface\n"
    "  GeomPlate_BuildPlateSurface BPSurf(Degree,NbPtsOnCur,NbIter);\n"
    "  \n"
    "  // add point constraints to GeomPlate_BuildPlateSurface object\n"
    "  nbNodes = aPoints.Length();\n"
    "  for (i = 1; i <= nbNodes; i++)\n"
    "    BPSurf.Add(new GeomPlate_PointConstraint(aPoints(i), Order, aConstrTol));\n"
    "\n"
    "  BPSurf.Perform();\n"
    "\n"
    "  // make PlateSurface\n"
    "  Handle(GeomPlate_Surface) PSurf;\n"
    "  Handle(Geom_Surface) aSurf;\n"
    "  \n"
    "  if (BPSurf.IsDone())\n"
    "  {\n"
    "    PSurf = BPSurf.Surface();\n"
    "\n"
    "    // define parameter approximation\n"
    "    dmax = Max(0.01,10*BPSurf.G0Error());\n"
    "\n"
    "    // make approximation\n"
    "    GeomPlate_MakeApprox Mapp(PSurf,anApproxTol, MaxSeg,MaxDegree,dmax);\n"
    "    aSurf = Mapp.Surface();\n"
    "  }\n"
    "  else \n"
    "    return;\n"
    "\n"
    "  ShapeAnalysis_FreeBounds aFreeBounds(aShape, Standard_False, Standard_True);\n"
    "  TopoDS_Compound aClosedWires = aFreeBounds.GetClosedWires();\n"
    "  TopTools_IndexedMapOfShape aWires;\n"
    "  TopExp::MapShapes(aClosedWires, TopAbs_WIRE, aWires);\n"
    "  TopoDS_Wire aWire;\n"
    "  Standard_Integer nbWires = aWires.Extent();\n"
    "  if (nbWires) \n"
    "    aWire = TopoDS::Wire(aWires(1));\n"
    "  else \n"
    "    return;\n"
    "\n"
    "  BRep_Builder B;\n"
    "  TopoDS_Face aFace;\n"
    "  B.MakeFace(aFace, aSurf, Precision::Confusion());\n"
    "  B.Add(aFace, aWire);\n"
    "  Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape(aFace);\n"
    "  sfs->Perform();\n"
    "  TopoDS_Shape aFixedFace = sfs->Shape();\n"
    "  if (aFixedFace.IsNull()) \n"
    "    return;\n");

    // define parameter triangulation
    Standard_Real aDeflection = 0.1;

    // removes all the triangulations of the faces ,
    //and all the polygons on the triangulations of the edges
    BRepTools::Clean(aShape);
    // adds a triangulation of the shape aShape with the deflection aDeflection
    BRepMesh_IncrementalMesh(aShape,aDeflection);

    Standard_Integer aIndex = 1, nbNodes = 0;

    // define two sequence of points
    TColgp_SequenceOfPnt aPoints, aPoints1;

    // triangulation
    for(TopExp_Explorer aExpFace(aShape,TopAbs_FACE); aExpFace.More(); aExpFace.Next())
    {  
      TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());
      TopLoc_Location aLocation;

      // takes the triangulation of the face aFace
      Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace,aLocation);

      if(!aTr.IsNull())
      { 
        // takes the array of nodes for this triangulation
        const TColgp_Array1OfPnt& aNodes = aTr->Nodes();    
        nbNodes = aNodes.Length();

        for( Standard_Integer i = 1; i <= nbNodes; i++)
        {
          // create seguence of node points in absolute coordinate system
          gp_Pnt aPnt = aNodes(i).Transformed(aLocation);
          aPoints.Append(aPnt);

        }
      }
    }

    // remove double points
    nbNodes = aPoints.Length();
    Standard_Integer i;
    for( i = 1; i <= nbNodes; i++)
    {
      gp_Pnt aPi = aPoints(i);
      Standard_Integer j;
      for( j = i + 1; j < nbNodes; j++)
      {
        gp_Pnt aPj = aPoints(j);
        if(!aPi.IsEqual(aPj,0.9))
          aIndex++;
      }
      if(aIndex == j - 1)
        aPoints1.Append(aPi);

      aIndex = i + 1;
    }

    // find max point
    aIndex = 0;
    gp_Pnt aPntMax = aPoints1(1);
    nbNodes = aPoints1.Length();
    for(i = 2; i <= nbNodes; i++)
    {
      if(aPoints1(i).X() > aPntMax.X())
      {
        aIndex = i;
        aPntMax = aPoints1(aIndex);
      } 
    }

    // clear seguence
    aPoints.Clear();

    Standard_Integer nbLeftNodes = nbNodes;

    // ascending sort - fill aPoints with ascending 
    // by X coordinate points from aPoints1
    for(i = 1; i < nbNodes; i++)
    {
      Standard_Real aMin = aPntMax.X();
      aIndex = 1;
      for( Standard_Integer j = 1; j <= nbLeftNodes; j++)
      {
        if(aPoints1(j).X() < aMin)
        {
          aMin = aPoints1(j).X();
          aIndex = j;
        } 
      }
      aPoints.Append(aPoints1(aIndex));
      aPoints1.Remove(aIndex);
      nbLeftNodes = aPoints1.Length();
    }
    aPoints.Append(aPntMax);

    // define parameters GeomPlate_BuildPlateSurface
    Standard_Integer Degree = 3;
    Standard_Integer NbPtsOnCur = 10;
    Standard_Integer NbIter = 3;
    Standard_Integer Order = 0;
    Standard_Integer MaxSeg = 9;
    Standard_Integer MaxDegree = 5;
    Standard_Real dmax, anApproxTol = 0.001;
    Standard_Real aConstrTol = Precision::Confusion();

    // define object BuildPlateSurface
    GeomPlate_BuildPlateSurface BPSurf(Degree,NbPtsOnCur,NbIter);

    // add point constraints to GeomPlate_BuildPlateSurface object
    nbNodes = aPoints.Length();
    for (i = 1; i <= nbNodes; i++)
      BPSurf.Add(new GeomPlate_PointConstraint(aPoints(i), Order, aConstrTol));

    BPSurf.Perform();

    // make PlateSurface
    Handle(GeomPlate_Surface) PSurf;
    Handle(Geom_Surface) aSurf;

    if (BPSurf.IsDone())
    {
      PSurf = BPSurf.Surface();

      // define parameter approximation
      dmax = Max(0.01,10*BPSurf.G0Error());

      // make approximation
      GeomPlate_MakeApprox Mapp(PSurf,anApproxTol, MaxSeg,MaxDegree,dmax);
      aSurf = Mapp.Surface();
    }
    else 
      return;

    ShapeAnalysis_FreeBounds aFreeBounds(aShape, Standard_False, Standard_True);
    TopoDS_Compound aClosedWires = aFreeBounds.GetClosedWires();
    TopTools_IndexedMapOfShape aWires;
    TopExp::MapShapes(aClosedWires, TopAbs_WIRE, aWires);
    TopoDS_Wire aWire;
    Standard_Integer nbWires = aWires.Extent();
    if (nbWires) 
      aWire = TopoDS::Wire(aWires(1));
    else 
      return;

    BRep_Builder B;
    TopoDS_Face aFace;
    B.MakeFace(aFace, aSurf, Precision::Confusion());
    B.Add(aFace, aWire);
    Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape(aFace);
    sfs->Perform();
    TopoDS_Shape aFixedFace = sfs->Shape();
    if (aFixedFace.IsNull()) 
      return;

    // output surface, make it half transparent
    Handle(AIS_InteractiveObject) aSurfIO = drawSurface(
      aSurf, Quantity_NOC_LEMONCHIFFON3, Standard_False);
    aSurfIO->SetTransparency(0.5);
    myAISContext->Display(aSurfIO,Standard_False);
    Fit();

    if(WAIT_A_LITTLE) return;

    // output points
    for(i = 1; i <= nbNodes; i++)
      drawPoint(aPoints(i));

    if(WAIT_A_LITTLE) return;

    // output resulting face
    drawShape(aFixedFace);
}

Handle(AIS_InteractiveObject) CGeometryDoc::drawSurface
                                  (const Handle(Geom_Surface)& theSurface,
                                   const Quantity_Color& theColor,
                                   const Standard_Boolean toDisplay)
{
  Standard_Real u1, u2, v1, v2;
  theSurface->Bounds(u1,u2,v1,v2);
  fixParam(u1);
  fixParam(u2);
  fixParam(v1);
  fixParam(v2);

  Handle(AIS_Shape) aGraphicSurface = 
    new AIS_Shape(BRepBuilderAPI_MakeFace (theSurface, u1, u2, v1, v2, Precision::Confusion()));

  myAISContext->SetMaterial(aGraphicSurface, Graphic3d_NOM_PLASTIC, toDisplay);
  myAISContext->SetColor(aGraphicSurface, theColor, toDisplay);
  if (toDisplay)
  {
    if (FitMode)
    {
      myAISContext->Display (aGraphicSurface, Standard_False);
      Fit();
    }
    else
      myAISContext->Display (aGraphicSurface, Standard_True);
  }

  return aGraphicSurface;
}

Standard_Boolean CGeometryDoc::WaitForInput (unsigned long aMilliSeconds)
{
  //::WaitForSingleObject(::CreateEvent (NULL, FALSE, FALSE, NULL), aMilliSeconds);
  if (::MsgWaitForMultipleObjects(0, NULL, FALSE, aMilliSeconds,
    QS_KEY | QS_MOUSEBUTTON) != WAIT_TIMEOUT)
  {
    MSG msg;
    if (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))
    {
      if (msg.message == WM_KEYUP)
      {
        ::PeekMessage (&msg, NULL, 0, 0, PM_REMOVE);
        return WaitForInput (aMilliSeconds);
      }
      else
        return Standard_True;
    }
  }
  return Standard_False;
}

Handle(AIS_Point) CGeometryDoc::drawPoint
                                  (const gp_Pnt& aPnt,
                                   const Quantity_Color& theColor,
                                   const Standard_Boolean toDisplay)
{
  Handle(AIS_Point) aGraphicPoint = new AIS_Point (new Geom_CartesianPoint(aPnt));

  myAISContext->SetColor (aGraphicPoint, theColor, toDisplay);
  if (toDisplay)
  {
  myAISContext->Display (aGraphicPoint, Standard_True);
    //COCCDemoDoc::Fit();
  }

  return aGraphicPoint;
}

Handle(AIS_Shape) CGeometryDoc::drawShape
         (const TopoDS_Shape& theShape,
          const Graphic3d_NameOfMaterial theMaterial,
          const Standard_Boolean toDisplay)
{
  Handle(AIS_Shape) aGraphicShape = new AIS_Shape(theShape);

  myAISContext->SetMaterial(aGraphicShape, theMaterial, toDisplay);
  if (toDisplay)
  {
    if (FitMode)
    {
      myAISContext->Display (aGraphicShape, Standard_False);
      Fit();
    }
    else
      myAISContext->Display (aGraphicShape, Standard_True);
  }

  return aGraphicShape;
}
