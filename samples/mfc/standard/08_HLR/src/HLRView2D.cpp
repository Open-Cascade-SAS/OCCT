// HLRView.cpp : implementation of the CHLRView2D class
//

#include "stdafx.h"

#include "HLRView2D.h"

#include "HLRApp.h"
#include "HLRDoc.h"
#include "resource2d\RectangularGrid.h"
#include "resource2d\CircularGrid.h"

#define ValZWMin 1

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// the key for multi selection :
#define MULTISELECTIONKEY MK_SHIFT

// the key for shortcut ( use to activate dynamic rotation, panning )
#define CASCADESHORTCUTKEY MK_CONTROL

// define in witch case you want to display the popup
#define POPUPONBUTTONDOWN

/////////////////////////////////////////////////////////////////////////////
// CHLRView2D

IMPLEMENT_DYNCREATE(CHLRView2D, OCC_2dView)

BEGIN_MESSAGE_MAP(CHLRView2D, OCC_2dView)
	//{{AFX_MSG_MAP(CHLRView2D)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHLRView2D construction/destruction

CHLRView2D::CHLRView2D()
{
}

CHLRView2D::~CHLRView2D()
{
}

CHLRDoc* CHLRView2D::GetDocument() // non-debug version is inline
{
	//ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(OCC_2dDoc)));
	return (CHLRDoc*)m_pDocument;
}


void CHLRView2D::OnInitialUpdate()
{

	Handle(WNT_Window) aWNTWindow;
	aWNTWindow = new WNT_Window(((CHLRApp*)AfxGetApp())->GetGraphicDevice(),GetSafeHwnd());	  
//    aWNTWindow->SetBackground(Quantity_NOC_BLACK);

	Handle(WNT_WDriver)  aDriver= new WNT_WDriver(aWNTWindow);
	myV2dView = new V2d_View(aDriver, GetDocument()->GetViewer2D(),0,0,50);

    // initialyse the grids dialogs
    TheRectangularGridDialog.Create(CRectangularGrid::IDD, NULL);
    TheCircularGridDialog.Create(CCircularGrid::IDD, NULL);
    TheRectangularGridDialog.SetViewer (GetDocument()->GetViewer2D());
    TheCircularGridDialog.SetViewer (GetDocument()->GetViewer2D());

	Standard_Integer w=100 , h=100 ;   /* Debug Matrox                         */
	aWNTWindow->Size (w,h) ;           /* Keeps me unsatisfied (rlb).....      */
	                                   /* Resize is not supposed to be done on */
	                                   /* Matrox                               */
	                                   /* I suspect another problem elsewhere  */
	::PostMessage ( GetSafeHwnd () , WM_SIZE , SIZE_RESTORED , w + h*65536 ) ;

}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::DragEvent2D(const Standard_Integer  x        ,
						  const Standard_Integer  y        ,
						  const Standard_Integer  TheState )
{
}


//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::InputEvent2D(const Standard_Integer  x     ,
						   const Standard_Integer  y     ) 
{
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::MoveEvent2D(const Standard_Integer  x       ,
						  const Standard_Integer  y       ) 
{
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::MultiMoveEvent2D(const Standard_Integer  x       ,
							   const Standard_Integer  y      ) 
{
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::MultiDragEvent2D(const Standard_Integer  x        ,
							   const Standard_Integer  y        ,
							   const Standard_Integer  TheState) 
{
}


//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::MultiInputEvent2D(const Standard_Integer  x       ,
								const Standard_Integer  y       ) 
{
}


#ifdef _DEBUG
void CHLRView2D::AssertValid() const
{
	 OCC_2dView::AssertValid();
}

void CHLRView2D::Dump(CDumpContext& dc) const
{
	 OCC_2dView::Dump(dc);
}

#endif