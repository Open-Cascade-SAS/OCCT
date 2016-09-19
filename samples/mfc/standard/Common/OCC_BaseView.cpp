// OCC_BaseView.cpp: implementation of the OCC_BaseView class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include "OCC_BaseView.h"

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
OCC_BaseView::OCC_BaseView()
  : myXmin (0),
    myYmin (0),
    myXmax (0),
    myYmax (0),
    myCurZoom (0.0),
    myRect (new AIS_RubberBand (Quantity_Color(Quantity_NOC_WHITE), Aspect_TOL_SOLID, 1.0) )
{
  myRect->SetTransformPersistence (new Graphic3d_TransformPers (Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER));
  if (myRect->ZLayer() != Graphic3d_ZLayerId_TopOSD)
  {
    myRect->SetZLayer (Graphic3d_ZLayerId_TopOSD);
  }
  
}

//=======================================================================
//function : Destructor
//purpose  :
//=======================================================================
OCC_BaseView::~OCC_BaseView()
{

}

//=======================================================================
//function : GetDocument
//purpose  :
//=======================================================================
OCC_BaseDoc* OCC_BaseView::GetDocument() // non-debug version is inline
{
	return (OCC_BaseDoc*)m_pDocument;
}

//=======================================================================
//function : drawRectangle
//purpose  :
//=======================================================================
void OCC_BaseView::drawRectangle (const Standard_Integer theMinX,
                                  const Standard_Integer theMinY,
                                  const Standard_Integer theMaxX,
                                  const Standard_Integer theMaxY,
                                  const Handle(AIS_InteractiveContext)& theContext,
                                  const Standard_Boolean toDraw)
{
  if (toDraw)
  {
    CRect aRect;
    GetWindowRect(aRect);
    myRect->SetRectangle (theMinX, aRect.Height() - theMinY, theMaxX, aRect.Height() - theMaxY);

    if (!theContext->IsDisplayed (myRect))
    {
      theContext->Display (myRect, Standard_False);
    }
    else
    {
      theContext->Redisplay (myRect, Standard_False);
    }
  }
  else
  {
    theContext->Remove (myRect, Standard_False);
  }

  theContext->CurrentViewer()->RedrawImmediate();
}

