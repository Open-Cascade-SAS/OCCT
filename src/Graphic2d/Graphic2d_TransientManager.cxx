// K4		GG_161297
//		les methodes qui "tirent" GEOMLIGHT disparaissent
// PRO13311	GG_220498
//		Reinitialiser les parametres de vue dans tous les cas
// PRO14041	//GG_150698
//		Reinitialiser la taille de la zone visualisable dans tous
//		les cas.

#define G002	//GG 28/04/00 Add protection on MinMax() methods
//			      Add perf improvment when the used transient
//		     05/05/00 inherits from the current drawer. 
//			      Add DrawElement() DrawVertex() methods.

#define OCC402  // SAV 14/11/01 Added DrawPickedElements() - highlights picked elements.

#include <Graphic2d_TransientManager.ixx>
#include <Graphic2d_Drawer.hxx>
#include <Graphic2d_DisplayList.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <Aspect_Window.hxx>
#include <Graphic2d_Primitive.pxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <Graphic2d_View.hxx>

#define MAXPOINTS 1024
#define MAXSEGMENTS 1024

Graphic2d_TransientManager::Graphic2d_TransientManager (
	const Handle(Graphic2d_View)& aView) : 
		myPView(aView.operator->()),
		myTrsf(),myCompositeTrsf(),
		myTypeOfComposition(Graphic2d_TOC_REPLACE),
		myDrawingIsStarted(Standard_False),
		myTrsfIsDefined(Standard_False),
		myMappingIsEnable(Standard_True),
		myTypeOfPrimitive(Aspect_TOP_UNKNOWN)
{
}

Graphic2d_TransientManager::Graphic2d_TransientManager (
	const Graphic2d_ViewPtr& aView) : 
		myPView(aView),
		myTrsf(),myCompositeTrsf(),
		myTypeOfComposition(Graphic2d_TOC_REPLACE),
		myDrawingIsStarted(Standard_False),
		myTrsfIsDefined(Standard_False),
		myMappingIsEnable(Standard_True),
		myTypeOfPrimitive(Aspect_TOP_UNKNOWN)
{ 
}

//=======================================================================
Standard_Boolean Graphic2d_TransientManager::BeginDraw(
	const Handle(Aspect_WindowDriver)& aDriver,
        const Standard_Boolean ClearBefore) {
Standard_Boolean status = Standard_True;
Standard_Boolean reset_minmax = Standard_True;

	if( myDrawingIsStarted ) return Standard_False;

	if( myDriver == aDriver ) {
	  if( ClearBefore ) {
	    status = RestoreArea(aDriver);
	  } else reset_minmax = Standard_False;
	}
	
#ifdef G002
    Handle(Graphic2d_Drawer) theDrawer = myPView->Drawer();
    Handle(Graphic2d_Drawer) theDrawer1 = this;
	if ( theDrawer1 != theDrawer )
#endif
	{
          Standard_ShortReal XF, YF, SF, XT, YT, ST, ZF;
	  theDrawer->Values(XF, YF, SF, XT, YT, ST, ZF);
	  Graphic2d_Drawer::SetValues(XF, YF, SF, XT, YT, ST, ZF);

	  Standard_ShortReal DP, DC;
	  Aspect_TypeOfDeflection TD;

	  theDrawer->DrawPrecision(DP,DC,TD);
	  Graphic2d_Drawer::SetDrawPrecision(DP,DC,TD);

	  Standard_ShortReal TP = theDrawer->TextPrecision();
	  Graphic2d_Drawer::SetTextPrecision(TP);
	}

        Graphic2d_Drawer::SetDriver(aDriver);

					// Begin new graphics 
        aDriver->BeginDraw(Standard_False,0);

	this->EnableMinMax(Standard_True,reset_minmax);

	myDrawingIsStarted = Standard_True; 
	myTrsfIsDefined = Standard_False;
	myTypeOfPrimitive = Aspect_TOP_UNKNOWN;

	return status;
}

//=======================================================================
Standard_Boolean Graphic2d_TransientManager::BeginDraw(
	const Handle(Aspect_WindowDriver)& aDriver,
        const Handle(Graphic2d_ViewMapping)& aViewMapping,
        const Standard_Real aXPosition,
        const Standard_Real aYPosition,
        const Standard_Real aScale,
        const Standard_Boolean ClearBefore) {
Standard_Boolean status = Standard_True;
Standard_Boolean reset_minmax = Standard_True;

	if( myDrawingIsStarted ) return Standard_False;

	if( myDriver == aDriver ) {
	  if( ClearBefore ) {
	    status = RestoreArea(aDriver);
	  } else reset_minmax = Standard_False;
	} else {
          Graphic2d_Drawer::SetDriver(aDriver);
	}
					// Set new drawer attributes 
#ifdef G002
    Handle(Graphic2d_Drawer) theDrawer = myPView->Drawer();
    Handle(Graphic2d_Drawer) theDrawer1 = this;
    if( theDrawer1 != theDrawer )
#endif
	{
          Standard_Real XCenter, YCenter, Size;
 
          aViewMapping->ViewMapping(XCenter, YCenter, Size);
          Graphic2d_Drawer::SetValues (XCenter, YCenter, Size,
                        aXPosition, aYPosition, aScale, aViewMapping->Zoom());
	  Standard_ShortReal DP, DC;
	  Aspect_TypeOfDeflection TD;

	  theDrawer->DrawPrecision(DP,DC,TD);
	  Graphic2d_Drawer::SetDrawPrecision(DP,DC,TD);

	  Standard_ShortReal TP = theDrawer->TextPrecision();
	  Graphic2d_Drawer::SetTextPrecision(TP);
	}
					// Begin new graphics 
        aDriver->BeginDraw(Standard_False,0);

	this->EnableMinMax(Standard_True,reset_minmax);

	myDrawingIsStarted = Standard_True; 
	myTrsfIsDefined = Standard_False;
	myTypeOfPrimitive = Aspect_TOP_UNKNOWN;

	return status;
}

//=======================================================================
void Graphic2d_TransientManager::EndDraw(const Standard_Boolean Synchronize) {

	if( myDrawingIsStarted ) {
          myDriver->EndDraw(Synchronize) ;
	  myDrawingIsStarted = Standard_False;
	  this->EnableMinMax(Standard_False,Standard_False);
	}
}

//=======================================================================
Standard_Boolean Graphic2d_TransientManager::Restore(
	const Handle(Aspect_WindowDriver)& aDriver) {
Standard_Boolean status = Standard_True;

	if(( status = aDriver->Window()->BackingStore() )) {	
				// Restore the front view from the back
	    aDriver->Window()->Restore();
	  } else { 		// Redraw the View
	    Redraw(aDriver);
	}

	return status;
}

//=======================================================================
Standard_Boolean Graphic2d_TransientManager::RestoreArea(
	const Handle(Aspect_WindowDriver)& aDriver) {
Standard_Integer pxmin,pymin,pxmax,pymax;
Standard_Boolean status = Standard_True;

  	if( this->MinMax(pxmin,pxmax,pymin,pymax) ) {
	  if(( status = aDriver->Window()->BackingStore() )) {	
				// Restore the front view area from the back
	    aDriver->Window()->RestoreArea((pxmin+pxmax)/2,(pymin+pymax)/2,
						pxmax-pxmin+10,pymax-pymin+10);
	  } else { 		// Redraw the View
	    Redraw(aDriver);
	  } 
#ifdef G002
	  this->EnableMinMax(Standard_True,Standard_True);
#endif
	}

	return status;
}

//=======================================================================
void Graphic2d_TransientManager::Redraw(
	const Handle(Aspect_WindowDriver)& aDriver) {

Handle(Graphic2d_DisplayList) theDisplayList = myPView->DisplayList();
Handle(Graphic2d_GraphicObject) theGraphicObject;
Handle(Graphic2d_Drawer) theDrawer = myPView->Drawer();
Standard_Integer i;

        aDriver->BeginDraw(Standard_True);
        aDriver->Window()->Clear();
        for (i=1; i<=theDisplayList->Length(); i++) {
	  theGraphicObject = theDisplayList->Value(i);
          theGraphicObject->Redraw (theDrawer);
	}
        aDriver->EndDraw();
}

//=======================================================================
void Graphic2d_TransientManager::Destroy () {
}

//=======================================================================
void Graphic2d_TransientManager::Draw (
                	const Handle(Graphic2d_Primitive) &aPrimitive) {

        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"Bad primitive type");

	if( myTrsfIsDefined ) {
	  Handle(Graphic2d_GraphicObject) theGO = 
					aPrimitive->PGraphicObject();
	  gp_GTrsf2d theOldTrsf = theGO->Transform(); 
	  if( theGO->IsTransformed() && // Sets the new transformation
		(myTypeOfComposition != Graphic2d_TOC_REPLACE) ) {
	    gp_GTrsf2d theNewTrsf = myCompositeTrsf.Multiplied(theOldTrsf);
	    theGO->SetTransform(theNewTrsf);
	  } else {
	    theGO->SetTransform(myTrsf);
	  }
	  aPrimitive->Draw(this);
	  theGO->SetTransform(theOldTrsf);	// Restore the old
	} else {
	  aPrimitive->Draw(this);
	}
} 

//=======================================================================
void Graphic2d_TransientManager::Draw (
                	const Handle(Graphic2d_GraphicObject) &aGraphicObject) {

        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"Bad primitive type");

	if( myTrsfIsDefined ) {
	  gp_GTrsf2d theOldTrsf = aGraphicObject->Transform(); 
	  if( aGraphicObject->IsTransformed() && // Sets the new transformation
		(myTypeOfComposition != Graphic2d_TOC_REPLACE) ) {
	    gp_GTrsf2d theNewTrsf = myCompositeTrsf.Multiplied(theOldTrsf);
	    aGraphicObject->SetTransform(theNewTrsf);
	  } else {
	    aGraphicObject->SetTransform(myTrsf);
	  }
	  aGraphicObject->Redraw(this);
	  aGraphicObject->SetTransform(theOldTrsf);	// Restore the old
	} else {
	  aGraphicObject->Redraw(this);
	}
}

#ifdef G002
//=======================================================================
void Graphic2d_TransientManager::DrawElement (
                	const Handle(Graphic2d_Primitive) &aPrimitive,
			        const Standard_Integer anIndex) {

        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"Bad primitive type");

	if( myTrsfIsDefined ) {
	  Handle(Graphic2d_GraphicObject) theGO = 
					aPrimitive->PGraphicObject();
	  gp_GTrsf2d theOldTrsf = theGO->Transform(); 
	  if( theGO->IsTransformed() && // Sets the new transformation
		(myTypeOfComposition != Graphic2d_TOC_REPLACE) ) {
	    gp_GTrsf2d theNewTrsf = myCompositeTrsf.Multiplied(theOldTrsf);
	    theGO->SetTransform(theNewTrsf);
	  } else {
	    theGO->SetTransform(myTrsf);
	  }
	  aPrimitive->DrawElement(this,anIndex);
	  theGO->SetTransform(theOldTrsf);	// Restore the old
	} else {
	  aPrimitive->DrawElement(this,anIndex);
	}
} 


// SAV
//=======================================================================
void Graphic2d_TransientManager
::DrawPickedElements( const Handle(Graphic2d_Primitive) &aPrimitive )
{
#ifdef OCC402
  Graphic2d_TransientDefinitionError_Raise_if(
     !myDrawingIsStarted,"Drawing must be started before" );

  Graphic2d_TransientDefinitionError_Raise_if(
     (myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"Bad primitive type");

  if( myTrsfIsDefined ) {
    Handle(Graphic2d_GraphicObject) theGO = 
      aPrimitive->PGraphicObject();
    gp_GTrsf2d theOldTrsf = theGO->Transform(); 
    if( theGO->IsTransformed() && // Sets the new transformation
       (myTypeOfComposition != Graphic2d_TOC_REPLACE) ) {
      gp_GTrsf2d theNewTrsf = myCompositeTrsf.Multiplied(theOldTrsf);
      theGO->SetTransform(theNewTrsf);
    } else {
      theGO->SetTransform(myTrsf);
    }
    aPrimitive->DrawPickedElements( this );
    theGO->SetTransform(theOldTrsf);	// Restore the old
  } else {
    aPrimitive->DrawPickedElements( this );
  }
#endif
}

//=======================================================================
void Graphic2d_TransientManager::DrawVertex (
                	const Handle(Graphic2d_Primitive) &aPrimitive,
			const Standard_Integer anIndex) {

        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"Bad primitive type");

	if( myTrsfIsDefined ) {
	  Handle(Graphic2d_GraphicObject) theGO = 
					aPrimitive->PGraphicObject();
	  gp_GTrsf2d theOldTrsf = theGO->Transform(); 
	  if( theGO->IsTransformed() && // Sets the new transformation
		(myTypeOfComposition != Graphic2d_TOC_REPLACE) ) {
	    gp_GTrsf2d theNewTrsf = myCompositeTrsf.Multiplied(theOldTrsf);
	    theGO->SetTransform(theNewTrsf);
	  } else {
	    theGO->SetTransform(myTrsf);
	  }
	  aPrimitive->DrawVertex(this,anIndex);
	  theGO->SetTransform(theOldTrsf);	// Restore the old
	} else {
	  aPrimitive->DrawVertex(this,anIndex);
	}
} 
#endif

//=======================================================================
void Graphic2d_TransientManager::BeginPrimitive (
                        const Aspect_TypeOfPrimitive aType,
                        const Standard_Integer aSize) {

        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"A primitive is already open");
        myTypeOfPrimitive = aType;

        switch (myTypeOfPrimitive) {
          case Aspect_TOP_UNKNOWN:
            break;
          case Aspect_TOP_POLYLINE:
            myDriver->BeginPolyline((aSize > 0) ? aSize : MAXPOINTS);
            break;
          case Aspect_TOP_POLYGON:
            myDriver->BeginPolygon((aSize > 0) ? aSize : MAXPOINTS);
            break;
          case Aspect_TOP_SEGMENTS:
            myDriver->BeginSegments();
            break;
          case Aspect_TOP_ARCS:
            myDriver->BeginArcs();
            break;
          case Aspect_TOP_POLYARCS:
            myDriver->BeginPolyArcs();
            break;
          case Aspect_TOP_POINTS:
            myDriver->BeginPoints();
            break;
          case Aspect_TOP_MARKERS:
            myDriver->BeginMarkers();
            break;
      }
}

//=======================================================================
void Graphic2d_TransientManager::ClosePrimitive () {

        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive == Aspect_TOP_UNKNOWN),"A primitive is not open");
        myDriver->ClosePrimitive();

        myTypeOfPrimitive = Aspect_TOP_UNKNOWN;
}

//=======================================================================
void Graphic2d_TransientManager::DrawSegment (
                               const Standard_Real X1,
                               const Standard_Real Y1,
                               const Standard_Real X2,
                               const Standard_Real Y2) {
 
        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN) && 
		(myTypeOfPrimitive != Aspect_TOP_SEGMENTS),"Bad primitive type");

	Standard_ShortReal x1 = Standard_ShortReal(X1), 
                       y1 = Standard_ShortReal(Y1), 
                       x2 = Standard_ShortReal(X2), 
                       y2 = Standard_ShortReal(Y2);
	if( myTrsfIsDefined ) {
	  Standard_Real U = x1, V = y1;
	  myTrsf.Transforms(U,V);
	  x1 = Standard_ShortReal(U); y1 = Standard_ShortReal(V);
	  U = x2,V = y2;
	  myTrsf.Transforms(U,V);
	  x2 = Standard_ShortReal(U); y2 = Standard_ShortReal(V);
	}
	if( myMappingIsEnable )
	  	Graphic2d_Drawer::MapSegmentFromTo(x1,y1,x2,y2);
	else
	  	Graphic2d_Drawer::DrawSegment(x1,y1,x2,y2);
	
}

//=======================================================================
void Graphic2d_TransientManager::DrawInfiniteLine (
                               const Standard_Real X,
                               const Standard_Real Y,
                               const Standard_Real Dx,
                               const Standard_Real Dy) {
 
        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"Bad primitive type");

	Standard_ShortReal x  = Standard_ShortReal(X), 
                       y  = Standard_ShortReal(Y), 
                       dx = Standard_ShortReal(Dx), 
                       dy = Standard_ShortReal(Dy);
	if( myTrsfIsDefined ) {
	  Standard_Real U = x,V = y;
	  myTrsf.Transforms(U,V);
	  x = Standard_ShortReal(U); y = Standard_ShortReal(V);
	  U = dx, V = dy;
	  myTrsf.Transforms(U,V);
	  dx = Standard_ShortReal(U) - x; dy = Standard_ShortReal(V) - y;
	}
	if( myMappingIsEnable )
	  	Graphic2d_Drawer::MapInfiniteLineFromTo(x,y,dx,dy);
	else
	  	Graphic2d_Drawer::DrawInfiniteLine(x,y,dx,dy);
}

//=======================================================================
void Graphic2d_TransientManager::DrawArc(
                        const Standard_Real Xc,
                        const Standard_Real Yc,
                        const Standard_Real aRadius,
                        const Standard_Real Angle1,
                        const Standard_Real Angle2) {
 
        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN) &&
		(myTypeOfPrimitive != Aspect_TOP_ARCS),"Bad primitive type");

	Standard_ShortReal xc = Standard_ShortReal(Xc), 
                       yc = Standard_ShortReal(Yc), 
                       r  = Standard_ShortReal(aRadius), 
                       a1 = Standard_ShortReal(Angle1), 
                       a2 = Standard_ShortReal(Angle2);
	if( myTrsfIsDefined ) {
	  Standard_Real U = xc,V = yc;
	  myTrsf.Transforms(U,V);
	  xc = Standard_ShortReal(U); yc = Standard_ShortReal(V);
	  U = r * Cos(a1); V = r * Sin(a1);
	  myTrsf.Transforms(U,V); U -= Standard_Real(xc); V -= Standard_Real(yc);
	  r = Standard_ShortReal(Sqrt( U*U + V*V )) ;
	  Standard_ShortReal a = Standard_ShortReal(atan2(U,V));
	  a2 += a - a1; a1 = a;
	}
	if( myMappingIsEnable )
		Graphic2d_Drawer::MapArcFromTo(xc,yc,r,a1,a2);
	else
		Graphic2d_Drawer::DrawArc(xc,yc,r,a1,a2);
}

//=======================================================================
void Graphic2d_TransientManager::DrawPolyArc(
                        const Standard_Real Xc,
                        const Standard_Real Yc,
                        const Standard_Real aRadius,
                        const Standard_Real Angle1,
                        const Standard_Real Angle2) {
 
        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN) &&
		(myTypeOfPrimitive != Aspect_TOP_POLYARCS),"Bad primitive type");

	Standard_ShortReal xc = Standard_ShortReal(Xc), 
                       yc = Standard_ShortReal(Yc), 
                       r  = Standard_ShortReal(aRadius), 
                       a1 = Standard_ShortReal(Angle1), 
                       a2 = Standard_ShortReal(Angle2);
	if( myTrsfIsDefined ) {
	  Standard_Real U = xc,V = yc;
	  myTrsf.Transforms(U,V);
	  xc = Standard_ShortReal(U); yc = Standard_ShortReal(V);
	  U = r * Cos(a1); V = r * Sin(a1);
	  myTrsf.Transforms(U,V); U -= Standard_Real(xc); V -= Standard_Real(yc);
	  r = Standard_ShortReal(Sqrt( U*U + V*V )) ;
	  Standard_ShortReal a = Standard_ShortReal(atan2(U,V));
	  a2 += a - a1; a1 = a;
	}
	if( myMappingIsEnable )
		Graphic2d_Drawer::MapPolyArcFromTo(xc,yc,r,a1,a2);
	else
		Graphic2d_Drawer::DrawPolyArc(xc,yc,r,a1,a2);
}

//=======================================================================
void Graphic2d_TransientManager::DrawPoint (
                const Standard_Real X, const Standard_Real Y) {
 
        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN) &&
		(myTypeOfPrimitive != Aspect_TOP_POLYLINE) &&
		(myTypeOfPrimitive != Aspect_TOP_POLYGON) &&
		(myTypeOfPrimitive != Aspect_TOP_MARKERS),"Bad primitive type");

	Standard_ShortReal x = Standard_ShortReal(X), y = Standard_ShortReal(Y);
	if( myTrsfIsDefined ) {
	  Standard_Real U = x,V = y;
	  myTrsf.Transforms(U,V);
	  x = Standard_ShortReal(U); y = Standard_ShortReal(V);
	}
	if( myMappingIsEnable )
		Graphic2d_Drawer::MapMarkerFromTo(0,x,y,0.,0.,0.);
	else
		Graphic2d_Drawer::DrawMarker(0,x,y,0.,0.,0.);
}

//=======================================================================
void Graphic2d_TransientManager::DrawMarker(
                        const Standard_Integer anIndex,
                        const Standard_Real X,
                        const Standard_Real Y,
                        const Standard_Real aWidth,
                        const Standard_Real anHeight,
                        const Standard_Real anAngle) {

        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN) &&
		(myTypeOfPrimitive != Aspect_TOP_MARKERS),"Bad primitive type");

	Standard_ShortReal x = Standard_ShortReal(X), 
                       y = Standard_ShortReal(Y), 
                       w = Standard_ShortReal(aWidth), 
                       h = Standard_ShortReal(anHeight), 
                       a = Standard_ShortReal(anAngle);
	if( myTrsfIsDefined ) {
	  Standard_Real U = x,V = y;
	  myTrsf.Transforms(U,V);
	  x = Standard_ShortReal(U); y = Standard_ShortReal(V);
	}
	if( myMappingIsEnable )
		Graphic2d_Drawer::MapMarkerFromTo(anIndex,x,y,w,h,a);
	else
		Graphic2d_Drawer::DrawMarker(anIndex,x,y,w,h,a);
}

//=======================================================================
void Graphic2d_TransientManager::DrawText(
                                     const TCollection_ExtendedString& aText,
                                     const Standard_Real X,
                                     const Standard_Real Y,
                                     const Standard_Real anAngle,
                                     const Standard_Real aDeltax,
                                     const Standard_Real aDeltay,
                                     const Aspect_TypeOfText aType,
				     const Graphic2d_TypeOfAlignment anAlignment) {

        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"Bad primitive type");

	Standard_ShortReal x = Standard_ShortReal(X), 
                       y = Standard_ShortReal(Y), 
                       a = Standard_ShortReal(anAngle);
	Standard_ShortReal dx = Standard_ShortReal(0.), 
                       dy = Standard_ShortReal(0.), 
                       ox = Standard_ShortReal(aDeltax), 
                       oy = Standard_ShortReal(aDeltay);

	if( anAlignment != Graphic2d_TOA_LEFT ) {
	  Standard_ShortReal tw,th,txo,tyo;
	  if( Graphic2d_Drawer::GetTextSize(aText,tw,th,txo,tyo) ) {
	    switch( anAlignment ) {
              case Graphic2d_TOA_LEFT:
              dx = Standard_ShortReal(0.); dy = Standard_ShortReal(0.);
              break;
              case Graphic2d_TOA_RIGHT:
              dx = -tw; dy = 0.;
              break;
              case Graphic2d_TOA_CENTER:
              dx = Standard_ShortReal(-tw/2.); dy = 0.;
              break;
              case Graphic2d_TOA_TOPLEFT:
              dx = 0.; dy = tyo-th;
              break;
              case Graphic2d_TOA_TOPRIGHT:
              dx = -tw; dy = tyo-th;
              break;
              case Graphic2d_TOA_TOPCENTER:
              dx = Standard_ShortReal(-tw/2.); dy = Standard_ShortReal(tyo-th);
              break;
              case Graphic2d_TOA_MEDIUMLEFT:
              dx = 0.; dy = Standard_ShortReal((tyo-th)/2.);
              break;
              case Graphic2d_TOA_MEDIUMRIGHT:
              dx = Standard_ShortReal(-tw); dy = Standard_ShortReal((tyo-th)/2.);
              break;
              case Graphic2d_TOA_MEDIUMCENTER:
              dx = Standard_ShortReal(-tw/2.); dy = Standard_ShortReal((tyo-th)/2.);
              break;
              case Graphic2d_TOA_BOTTOMLEFT:
              dx = 0.; dy = tyo;
              break;
              case Graphic2d_TOA_BOTTOMRIGHT:
              dx = -tw; dy = tyo;
              break;
              case Graphic2d_TOA_BOTTOMCENTER:
              dx = Standard_ShortReal(-tw/2.); dy = tyo;
              break;
	    }
	  }
	}

	if( myTrsfIsDefined ) {
	  Standard_Real U = x,V = y;
	  myTrsf.Transforms(U,V);
	  x = Standard_ShortReal(U); y = Standard_ShortReal(V);
	  U = Cos(a); V = Sin(a);
	  myTrsf.Transforms(U,V); U -= Standard_Real(x); V -= Standard_Real(y);
	  a = Standard_ShortReal(atan2(U,V));
	}
	if( a != 0. ) {
	  Standard_ShortReal ax = dx, ay = dy;
          Standard_Real cosa = Cos (a);
          Standard_Real sina = Sin (a);
	      dx = Standard_ShortReal(XROTATE(ax,ay));
          dy = Standard_ShortReal(YROTATE(ax,ay));
	}
	if( myMappingIsEnable )
		Graphic2d_Drawer::MapTextFromTo(aText,x+dx,y+dy,a,ox,oy,aType);
	else
		Graphic2d_Drawer::DrawText(aText,x+ox+dx,y+oy+dy,a,aType);
}

//=======================================================================
void Graphic2d_TransientManager::DrawPolyText(
                                     const TCollection_ExtendedString& aText,
                                     const Standard_Real X,
                                     const Standard_Real Y,
                                     const Standard_Real anAngle,
				                     const Standard_Real aMargin,
                                     const Standard_Real aDeltax,
                                     const Standard_Real aDeltay,
                                     const Aspect_TypeOfText aType,
				                     const Graphic2d_TypeOfAlignment anAlignment) {

        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"Bad primitive type");

	Standard_ShortReal x = Standard_ShortReal(X), y = Standard_ShortReal(Y), 
                       a = Standard_ShortReal(anAngle), m = Standard_ShortReal(aMargin);
	Standard_ShortReal dx = 0., dy = 0., ox = Standard_ShortReal(aDeltax), oy = Standard_ShortReal(aDeltay);

	if( anAlignment != Graphic2d_TOA_LEFT ) {
	  Standard_ShortReal tw,th,txo,tyo;
	  if( Graphic2d_Drawer::GetTextSize(aText,tw,th,txo,tyo) ) {
	    switch( anAlignment ) {
              case Graphic2d_TOA_LEFT:
              dx = 0.; dy = 0.;
              break;
              case Graphic2d_TOA_RIGHT:
              dx = -tw; dy = 0.;
              break;
              case Graphic2d_TOA_CENTER:
              dx = Standard_ShortReal(-tw/2.); dy = 0.;
              break;
              case Graphic2d_TOA_TOPLEFT:
              dx = 0.; dy = tyo-th;
              break;
              case Graphic2d_TOA_TOPRIGHT:
              dx = -tw; dy = tyo-th;
              break;
              case Graphic2d_TOA_TOPCENTER:
              dx = Standard_ShortReal(-tw/2.); dy = tyo-th;
              break;
              case Graphic2d_TOA_MEDIUMLEFT:
              dx = 0.; dy = Standard_ShortReal((tyo-th)/2.);
              break;
              case Graphic2d_TOA_MEDIUMRIGHT:
              dx = -tw; dy = Standard_ShortReal((tyo-th)/2.);
              break;
              case Graphic2d_TOA_MEDIUMCENTER:
              dx = Standard_ShortReal(-tw/2.); dy = Standard_ShortReal((tyo-th)/2.);
              break;
              case Graphic2d_TOA_BOTTOMLEFT:
              dx = 0.; dy = tyo;
              break;
              case Graphic2d_TOA_BOTTOMRIGHT:
              dx = -tw; dy = tyo;
              break;
              case Graphic2d_TOA_BOTTOMCENTER:
              dx = Standard_ShortReal(-tw/2.); dy = tyo;
              break;
	    }
	  }
	}

	if( myTrsfIsDefined ) {
	  Standard_Real U = x,V = y;
	  myTrsf.Transforms(U,V);
	  x = Standard_ShortReal(U); y = Standard_ShortReal(V);
	  U = Cos(a); V = Sin(a);
	  myTrsf.Transforms(U,V); U -= Standard_Real(x); V -= Standard_Real(y);
	  a = Standard_ShortReal(atan2(U,V));
	}
	if( a != 0. ) {
	  Standard_ShortReal ax = dx, ay = dy;
      Standard_Real cosa = Cos (a);
      Standard_Real sina = Sin (a);
	  dx = Standard_ShortReal(XROTATE(ax,ay));
      dy = Standard_ShortReal(YROTATE(ax,ay));
	}
	if( myMappingIsEnable )
		Graphic2d_Drawer::MapPolyTextFromTo(aText,x+dx,y+dy,a,m,ox,oy,aType);
	else
		Graphic2d_Drawer::DrawPolyText(aText,x+ox+dx,y+oy+dy,a,m,aType);
}

//=======================================================================
void Graphic2d_TransientManager::DrawFramedText(
                                     const TCollection_ExtendedString& aText,
                                     const Standard_Real X,
                                     const Standard_Real Y,
                                     const Standard_Real anAngle,
				                     const Standard_Real aMargin,
                                     const Standard_Real aDeltax,
                                     const Standard_Real aDeltay,
                                     const Aspect_TypeOfText aType,
				                     const Graphic2d_TypeOfAlignment anAlignment) {

        Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

        Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"Bad primitive type");

	Standard_ShortReal x = Standard_ShortReal(X), 
                       y = Standard_ShortReal(Y), 
                       a = Standard_ShortReal(anAngle), 
                       m = Standard_ShortReal(aMargin);
	Standard_ShortReal dx = 0., dy = 0., 
        ox = Standard_ShortReal(aDeltax), oy = Standard_ShortReal(aDeltay);

	if( anAlignment != Graphic2d_TOA_LEFT ) {
	  Standard_ShortReal tw,th,txo,tyo;
	  if( Graphic2d_Drawer::GetTextSize(aText,tw,th,txo,tyo) ) {
	    switch( anAlignment ) {
              case Graphic2d_TOA_LEFT:
              dx = 0.; dy = 0.;
              break;
              case Graphic2d_TOA_RIGHT:
              dx = -tw; dy = 0.;
              break;
              case Graphic2d_TOA_CENTER:
              dx = Standard_ShortReal(-tw/2.); dy = 0.;
              break;
              case Graphic2d_TOA_TOPLEFT:
              dx = 0.; dy = tyo-th;
              break;
              case Graphic2d_TOA_TOPRIGHT:
              dx = -tw; dy = tyo-th;
              break;
              case Graphic2d_TOA_TOPCENTER:
              dx = Standard_ShortReal(-tw/2.); dy = tyo-th;
              break;
              case Graphic2d_TOA_MEDIUMLEFT:
              dx = 0.; dy = Standard_ShortReal((tyo-th)/2.);
              break;
              case Graphic2d_TOA_MEDIUMRIGHT:
              dx = -tw; dy = Standard_ShortReal((tyo-th)/2.);
              break;
              case Graphic2d_TOA_MEDIUMCENTER:
              dx = Standard_ShortReal(-tw/2.); dy = Standard_ShortReal((tyo-th)/2.);
              break;
              case Graphic2d_TOA_BOTTOMLEFT:
              dx = 0.; dy = tyo;
              break;
              case Graphic2d_TOA_BOTTOMRIGHT:
              dx = -tw; dy = tyo;
              break;
              case Graphic2d_TOA_BOTTOMCENTER:
              dx = Standard_ShortReal(-tw/2.); dy = tyo;
              break;
	    }
	  }
	}

	if( myTrsfIsDefined ) {
	  Standard_Real U = x,V = y;
	  myTrsf.Transforms(U,V);
	  x = Standard_ShortReal(U); y = Standard_ShortReal(V);
	  U = Cos(a); V = Sin(a);
	  myTrsf.Transforms(U,V); U -= Standard_Real(x); V -= Standard_Real(y);
	  a = Standard_ShortReal(atan2(U,V));
	}
	if( a != 0. ) {
	  Standard_ShortReal ax = dx, ay = dy;
          Standard_Real cosa = Cos (a);
          Standard_Real sina = Sin (a);
	      dx = Standard_ShortReal(XROTATE(ax,ay));
          dy = Standard_ShortReal(YROTATE(ax,ay));
	}
	if( myMappingIsEnable )
		Graphic2d_Drawer::MapFramedTextFromTo(aText,x+dx,y+dy,a,m,ox,oy,aType);
	else
		Graphic2d_Drawer::DrawFramedText(aText,x+ox+dx,y+oy+dy,a,m,aType);
}

//=======================================================================
void Graphic2d_TransientManager::SetTextAttrib(
				     const Standard_Integer ColorIndex,
                     const Standard_Integer FontIndex,
                     const Standard_Real aSlant,
                     const Standard_Real aHScale,
                     const Standard_Real aWScale,
                     const Standard_Boolean isUnderlined,
                     const Standard_Boolean isZoomable) {
                     
    Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

    Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"Bad primitive type");

    Standard_ShortReal sl = Standard_ShortReal(aSlant), 
                       hsc = (aHScale > 0.) ? Standard_ShortReal(aHScale) : Standard_ShortReal(1.0);
    Standard_ShortReal wsc = (aWScale > 0.) ? Standard_ShortReal(aWScale) : hsc;
    
    if( myTrsfIsDefined ) {
      hsc *= Standard_ShortReal((myTrsf(1,1) + myTrsf(2,2))/2.);
      wsc *= Standard_ShortReal((myTrsf(1,1) + myTrsf(2,2))/2.);
    }
    if( isZoomable ) {
      hsc *= Graphic2d_Drawer::Scale(); wsc *= Graphic2d_Drawer::Scale();
    }
    Graphic2d_Drawer::SetTextAttrib(ColorIndex,FontIndex,sl,hsc,wsc,isUnderlined);

}

//=======================================================================
void Graphic2d_TransientManager::SetHidingTextAttrib(
				     const Standard_Integer ColorIndex,
                                     const Standard_Integer HidingColorIndex,
                                     const Standard_Integer FrameColorIndex,
                                     const Standard_Integer FrameWidthIndex,
                                     const Standard_Integer FontIndex,
                                     const Standard_Real aSlant,
                                     const Standard_Real aHScale,
                                     const Standard_Real aWScale,
                                     const Standard_Boolean isUnderlined,
                                     const Standard_Boolean isZoomable) {

    Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

    Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"Bad primitive type");

    Standard_ShortReal sl = Standard_ShortReal(aSlant), 
        hsc = (aHScale > 0.) ? Standard_ShortReal(aHScale) : Standard_ShortReal(1.0);
    Standard_ShortReal wsc = (aWScale > 0.) ? Standard_ShortReal(aWScale) : hsc;
    if( myTrsfIsDefined ) {
      hsc *= Standard_ShortReal((myTrsf(1,1) + myTrsf(2,2))/2.);
      wsc *= Standard_ShortReal((myTrsf(1,1) + myTrsf(2,2))/2.);
    }
    if( isZoomable ) {
      hsc *= Graphic2d_Drawer::Scale(); wsc *= Graphic2d_Drawer::Scale();
    }
    Graphic2d_Drawer::SetHidingTextAttrib(ColorIndex,HidingColorIndex,
	FrameColorIndex,FrameWidthIndex,FontIndex,sl,hsc,wsc,isUnderlined);
}

//=======================================================================
void Graphic2d_TransientManager::SetFramedTextAttrib(
				     const Standard_Integer ColorIndex,
                                     const Standard_Integer FrameColorIndex,
                                     const Standard_Integer FrameWidthIndex,
                                     const Standard_Integer FontIndex,
                                     const Standard_Real aSlant,
                                     const Standard_Real aHScale,
                                     const Standard_Real aWScale,
                                     const Standard_Boolean isUnderlined,
                                     const Standard_Boolean isZoomable) {

    Graphic2d_TransientDefinitionError_Raise_if (
		!myDrawingIsStarted,"Drawing must be started before");

    Graphic2d_TransientDefinitionError_Raise_if (
		(myTypeOfPrimitive != Aspect_TOP_UNKNOWN),"Bad primitive type");

    Standard_ShortReal sl = Standard_ShortReal(aSlant), hsc = (aHScale > 0.) ? Standard_ShortReal(aHScale) : Standard_ShortReal(1.0);
    Standard_ShortReal wsc = (aWScale > 0.) ? Standard_ShortReal(aWScale) : hsc;
    if( myTrsfIsDefined ) {
      hsc *= Standard_ShortReal((myTrsf(1,1) + myTrsf(2,2))/2.);
      wsc *= Standard_ShortReal((myTrsf(1,1) + myTrsf(2,2))/2.);
    }
    if( isZoomable ) {
      hsc *= Graphic2d_Drawer::Scale(); wsc *= Graphic2d_Drawer::Scale();
    }
    Graphic2d_Drawer::SetFramedTextAttrib(ColorIndex,FrameColorIndex,
			FrameWidthIndex,FontIndex,sl,hsc,wsc,isUnderlined);
} 

//=======================================================================
void Graphic2d_TransientManager::SetTransform (
				const gp_GTrsf2d& aTrsf,
				const Graphic2d_TypeOfComposition aType) {
 
        if( myDrawingIsStarted ) {
	  Standard_Real re = RealEpsilon ();
	  if( !myTrsfIsDefined || (aType == Graphic2d_TOC_REPLACE) ) {
            myTrsf  = aTrsf;
	  } else {
	    myTrsf = aTrsf.Multiplied(myTrsf);
	  }
	  myTypeOfComposition = aType;
	  myCompositeTrsf = aTrsf;
	  myTrsfIsDefined = 
                (Abs (myCompositeTrsf.Value (1, 1) - 1.0) > re) ||
                (Abs (myCompositeTrsf.Value (1, 2) - 0.0) > re) ||
                (Abs (myCompositeTrsf.Value (1, 3) - 0.0) > re) ||
                (Abs (myCompositeTrsf.Value (2, 1) - 0.0) > re) ||
                (Abs (myCompositeTrsf.Value (2, 2) - 1.0) > re) ||
                (Abs (myCompositeTrsf.Value (2, 3) - 0.0) > re);
	} else {
          Graphic2d_TransientDefinitionError::Raise ("Drawing must be started");
	}
}

//=======================================================================
void Graphic2d_TransientManager::SetMapping(const Standard_Boolean aStatus) {

        myMappingIsEnable = aStatus;
}

//=======================================================================
void Graphic2d_TransientManager::EnableMinMax(const Standard_Boolean aStatus,
                                       const Standard_Boolean Reset) {
        myMinMaxIsActivated = aStatus;
        if( Reset ) {
          myMinX = myMinY = ShortRealLast();
          myMaxX = myMaxY = ShortRealFirst();
        }
}

//=======================================================================
Standard_Boolean Graphic2d_TransientManager::MinMax (
                Standard_Real &XMin, Standard_Real &YMin,
                Standard_Real &XMax, Standard_Real &YMax) const {

#ifdef G002
	if( myMaxX < myMinX || myMaxY < myMinY ) return Standard_False;
#endif
	Standard_ShortReal xmin = Max(myMinX,Standard_ShortReal(0.));
	Standard_ShortReal ymin = Max(myMinY,Standard_ShortReal(0.));
	Standard_ShortReal xmax = Min(myMaxX,mySpaceWidth); 
	Standard_ShortReal ymax = Min(myMaxY,mySpaceHeight);

        if( xmax > xmin && ymax > ymin ) {
	  Standard_ShortReal uxmin,uymin,uxmax,uymax;
	  Graphic2d_Drawer::UnMapFromTo(xmin,ymin,uxmin,uymin);
	  Graphic2d_Drawer::UnMapFromTo(xmax,ymax,uxmax,uymax);
	  XMin = uxmin; YMin = uymin; XMax = uxmax; YMax = uymax;
	  return Standard_True;
	} else {
	  XMin = YMin = RealLast();
	  XMax = YMax = RealFirst();
	  return Standard_False;
	}
} 

//=======================================================================
Standard_Boolean Graphic2d_TransientManager::MinMax(
                                        Standard_Integer &aMinX,
                                        Standard_Integer &aMaxX,
                                        Standard_Integer &aMinY,
                                        Standard_Integer &aMaxY)const
{
        if (! myDriverIsDefined)
                Graphic2d_TransientDefinitionError::Raise ("No defined driver");
#ifdef G002
	if( myMaxX < myMinX || myMaxY < myMinY ) return Standard_False;
#endif

	Standard_ShortReal xmin = Max(myMinX,Standard_ShortReal(0.));
	Standard_ShortReal ymin = Max(myMinY,Standard_ShortReal(0.));
	Standard_ShortReal xmax = Min(myMaxX,mySpaceWidth); 
	Standard_ShortReal ymax = Min(myMaxY,mySpaceHeight);

        myDriver->Convert(xmin,ymin,aMinX,aMaxY);
        myDriver->Convert(xmax,ymax,aMaxX,aMinY);
        if( aMaxX > aMinX && aMaxY > aMinY ) return Standard_True;
        else return Standard_False;
}
