/***********************************************************************

     FONCTION :
     ----------
        Classe Visual3d_TransientManager :


        Declaration des variables specifiques au mode transient.

        Une vue est definie par :
                - un ViewManager
                - un ContextView
                - un ViewMapping
                - une ViewOrientation

     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      October 1995 : GG : Creation.
      20-11-97  : CAL ; Disparition de la dependance avec math
      01-05-97 : CAL ; Ajout du Clear sur les TOS_COMPUTED.
      18-12-97 : FMN ; Ajout mode AJOUT.
      27-12-98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)
      10-06-98 : CAL ; Modification de la signature de DrawStructure.
      25-09-98 : CAL ; Mise a jour de theCView = *(CALL_DEF_VIEW *)AView->CView ();
      10-11-98 : CAL ; PRO16361. theCView dans ClearImmediatMode.
      30-11-98 : FMN ; S4069 : Textes toujours visibles.
      01-12-98 : CAL ; S4062. Ajout des layers.

************************************************************************/

#define IMP190100	//GG 
//			-> Enable to have overlapped BeginDraw() EndDraw().
//			-> Don't redraw the scene at ClearDraw()
//			   but erase only the immediat draw stuff.

// for the class
#include <Visual3d_TransientManager.ixx>
#include <Visual3d_ViewPtr.hxx>
#include <Visual3d_View.pxx>

#include <Aspect_CLayer2d.hxx>
#include <Graphic3d_CView.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TypeOfPrimitive.hxx>
#include <Visual3d_Layer.hxx>

//-Aliases
#define Graphic3d_TOP_BEZIER Graphic3d_TOP_UNDEFINED

#define DEBUG_PRO4022
#define DEBUG_TEMPO_FOR_ROB

enum TypeOfImmediat { 
 Immediat_None,
 Immediat_Transient,
 Immediat_Ajout
};

//-Global data definitions
#ifdef IMP190100
static Standard_Integer theDrawingState = 0;
#else
static Standard_Boolean theDrawingState = Standard_False;
#endif
static Standard_Real theMinX,theMinY,theMinZ,theMaxX,theMaxY,theMaxZ;
static TypeOfImmediat theImmediatState = Immediat_None;
static Graphic3d_TypeOfPrimitive theTypeOfPrimitive = Graphic3d_TOP_UNDEFINED;

static Handle(Graphic3d_GraphicDriver)& _theGraphicDriver() {
    static Handle(Graphic3d_GraphicDriver) theGraphicDriver;
return theGraphicDriver;
}
#define theGraphicDriver _theGraphicDriver()

static Graphic3d_CView& _theCView() {
    static Graphic3d_CView theCView; 
return theCView;
}
#define theCView _theCView()

//
//-Constructors
//

Visual3d_TransientManager::Visual3d_TransientManager () {
}

//
//-Destructors
//

void Visual3d_TransientManager::Destroy () {
}

//-Methods, in order

Standard_Boolean Visual3d_TransientManager::BeginDraw (const Handle(Visual3d_View)& AView, const Standard_Boolean DoubleBuffer, const Standard_Boolean RetainMode) {

#ifdef IMP190100
	if (theDrawingState > 0) {
	  CALL_DEF_VIEW* pview = (CALL_DEF_VIEW*) AView->CView();
	  if( theImmediatState == Immediat_Transient &&
	      pview->ViewId == theCView.ViewId ) {
	    theDrawingState++;
	    return theDrawingState;
	  } else
#else
	if (theDrawingState) {
#endif
		Visual3d_TransientDefinitionError::Raise
			("Drawing in progress !");
	}

Handle(Visual3d_Layer) OverLayer = AView->OverLayer ();
Handle(Visual3d_Layer) UnderLayer = AView->UnderLayer ();
Aspect_CLayer2d OverCLayer;
Aspect_CLayer2d UnderCLayer;
	OverCLayer.ptrLayer = UnderCLayer.ptrLayer = NULL;
	theCView	= *(CALL_DEF_VIEW *)AView->CView ();

	if (! UnderLayer.IsNull ()){
		UnderCLayer = UnderLayer->CLayer ();
		theCView.ptrUnderLayer = (CALL_DEF_LAYER *) &(UnderLayer->CLayer ());
	}
	if (! OverLayer.IsNull ()){
		OverCLayer = OverLayer->CLayer ();
		theCView.ptrOverLayer = (CALL_DEF_LAYER *) &(OverLayer->CLayer ());
	}

	// Begin rendering
	Handle(Aspect_GraphicDriver) agd = AView->GraphicDriver ();

	theGraphicDriver = *(Handle(Graphic3d_GraphicDriver) *) &agd;

	if (theGraphicDriver->BeginImmediatMode
		(theCView, UnderCLayer, OverCLayer, DoubleBuffer, RetainMode)) {
#ifdef IMP190100
		theDrawingState++;
#else
		theDrawingState = Standard_True;
#endif
		theTypeOfPrimitive = Graphic3d_TOP_UNDEFINED;
		theImmediatState = Immediat_Transient;
		// Reset MinMax
		theMinX = RealLast ();
		theMinY = RealLast ();
		theMinZ = RealLast ();
		theMaxX = RealFirst ();
		theMaxY = RealFirst ();
		theMaxZ = RealFirst ();
	}

	return theDrawingState;
}

void Visual3d_TransientManager::EndDraw (const Standard_Boolean Synchronize) {

#ifdef IMP190100
	if( theDrawingState <= 0 )
#else
	if( !theDrawingState )
#endif
	  Visual3d_TransientDefinitionError::Raise ("Drawing not started !");

#ifdef IMP190100
	theDrawingState--;
	if( theDrawingState > 0 ) return;
#else
	theDrawingState = Standard_False;
#endif
	theImmediatState = Immediat_None;

			// Flush all graphics
	theGraphicDriver->EndImmediatMode(Synchronize);
}

void Visual3d_TransientManager::ClearDraw (const Handle(Visual3d_View)& AView,
                                           const Standard_Boolean aFlush)
{

#ifdef IMP190100
	if (theDrawingState > 0)
#else
	if (theDrawingState)
#endif
		Visual3d_TransientDefinitionError::Raise
			("Drawing in progress !");

	// Begin rendering
	theCView	= *(CALL_DEF_VIEW *)AView->CView ();
	if (!AView->UnderLayer().IsNull()) 
		theCView.ptrUnderLayer = (CALL_DEF_LAYER *)&(AView->UnderLayer()->CLayer());
	if (!AView->OverLayer().IsNull()) 
		theCView.ptrOverLayer = (CALL_DEF_LAYER *)&(AView->OverLayer()->CLayer());

	Handle(Aspect_GraphicDriver) agd = AView->GraphicDriver ();

	theGraphicDriver = *(Handle(Graphic3d_GraphicDriver) *) &agd;

	theGraphicDriver->ClearImmediatMode (theCView, aFlush);

#ifndef IMP190100	
	// Reaffichage
	AView->Redraw ();
#endif
}

//
//-Mode Ajout
//

Standard_Boolean Visual3d_TransientManager::BeginAddDraw (const Handle(Visual3d_View)& AView) {

#ifdef IMP190100
	if (theDrawingState > 0) {
	  CALL_DEF_VIEW* pview = (CALL_DEF_VIEW*) AView->CView();
	  if( theImmediatState == Immediat_Ajout &&
	      pview->ViewId == theCView.ViewId ) {
	    theDrawingState++;
	    return theDrawingState;
	  } else
#else
	if (theDrawingState) {
#endif
		Visual3d_TransientDefinitionError::Raise
			("Drawing in progress !");
	}

	// Begin rendering
	theCView	= *(CALL_DEF_VIEW *)AView->CView ();
	if (!AView->UnderLayer().IsNull()) 
		theCView.ptrUnderLayer = (CALL_DEF_LAYER *) &(AView->UnderLayer()->CLayer ());
	if (!AView->OverLayer().IsNull()) 
		theCView.ptrOverLayer = (CALL_DEF_LAYER *) &(AView->OverLayer()->CLayer ());

	Handle(Aspect_GraphicDriver) agd = AView->GraphicDriver ();

	theGraphicDriver = *(Handle(Graphic3d_GraphicDriver) *) &agd;

	if (theGraphicDriver->BeginAddMode (theCView)) {
#ifdef IMP190100
		theDrawingState++;
#else
		theDrawingState = Standard_True;
#endif
		theTypeOfPrimitive = Graphic3d_TOP_UNDEFINED;
		theImmediatState = Immediat_Ajout;
		// Reset MinMax
		theMinX = RealLast ();
		theMinY = RealLast ();
		theMinZ = RealLast ();
		theMaxX = RealFirst ();
		theMaxY = RealFirst ();
		theMaxZ = RealFirst ();
	}

	return theDrawingState;
}

void Visual3d_TransientManager::EndAddDraw () {

#ifdef IMP190100
	if( theDrawingState <= 0 )
#else
	if( !theDrawingState )
#endif
	  Visual3d_TransientDefinitionError::Raise ("Drawing not started !");

#ifdef IMP190100
	theDrawingState--;
	if( theDrawingState > 0 ) return;
#else
	theDrawingState = Standard_False;
#endif
	theImmediatState = Immediat_None;
			// Flush all graphics
	theGraphicDriver->EndAddMode();
}


//
//-Graphic definition methods
//

void Visual3d_TransientManager::BeginPolyline () {

	if( !theDrawingState )
		Visual3d_TransientDefinitionError::Raise
			("Drawing is not open !");
	if( theTypeOfPrimitive != Graphic3d_TOP_UNDEFINED )
		Visual3d_TransientDefinitionError::Raise
			("One primitive is already opened !");

	theTypeOfPrimitive = Graphic3d_TOP_POLYLINE;

	theGraphicDriver->BeginPolyline();

}

void Visual3d_TransientManager::BeginPolygon () {

	if( !theDrawingState )
		Visual3d_TransientDefinitionError::Raise
			("Drawing is not open !");
	if( theTypeOfPrimitive != Graphic3d_TOP_UNDEFINED )
		Visual3d_TransientDefinitionError::Raise
			("One primitive is already opened !");

	theTypeOfPrimitive = Graphic3d_TOP_POLYGON;

}

void Visual3d_TransientManager::BeginTriangleMesh () {

	if( !theDrawingState )
		Visual3d_TransientDefinitionError::Raise
			("Drawing is not open !");
	if( theTypeOfPrimitive != Graphic3d_TOP_UNDEFINED )
		Visual3d_TransientDefinitionError::Raise
			("One primitive is already opened !");

	theTypeOfPrimitive = Graphic3d_TOP_TRIANGLEMESH;

}

void Visual3d_TransientManager::BeginMarker () {

	if( !theDrawingState )
		Visual3d_TransientDefinitionError::Raise
			("Drawing is not open !");
	if( theTypeOfPrimitive != Graphic3d_TOP_UNDEFINED )
		Visual3d_TransientDefinitionError::Raise
			("One primitive is already opened !");

	theTypeOfPrimitive = Graphic3d_TOP_MARKER;

}

void Visual3d_TransientManager::BeginBezier () {

	if( !theDrawingState )
		Visual3d_TransientDefinitionError::Raise
			("Drawing is not open !");
	if( theTypeOfPrimitive != Graphic3d_TOP_UNDEFINED )
		Visual3d_TransientDefinitionError::Raise
			("One primitive is already opened !");

	theTypeOfPrimitive = Graphic3d_TOP_BEZIER;

}

void Visual3d_TransientManager::AddVertex (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Boolean AFlag) {
Standard_ShortReal x = Standard_ShortReal(X);
Standard_ShortReal y = Standard_ShortReal(Y);
Standard_ShortReal z = Standard_ShortReal(Z);

	switch (theTypeOfPrimitive) {
	  case Graphic3d_TOP_POLYLINE :
	    if( AFlag ) theGraphicDriver->Draw (x, y, z);
	    else        theGraphicDriver->Move (x, y, z);
	    break;
	  case Graphic3d_TOP_POLYGON :
	    break;
	  case Graphic3d_TOP_TRIANGLEMESH :
	    break;
	  case Graphic3d_TOP_QUADRANGLEMESH :
	    break;
	  case Graphic3d_TOP_TEXT :
	    break;
	  case Graphic3d_TOP_MARKER :
	    break;
	  case Graphic3d_TOP_BEZIER :
	    break;
	  default:
	    Visual3d_TransientDefinitionError::Raise
			("Bad Primitive type!");
	}
	theMinX  = Min(theMinX,X);
	theMinY  = Min(theMinY,Y);
	theMinZ  = Min(theMinZ,Z);
	theMaxX  = Max(theMaxX,X);
	theMaxY  = Max(theMaxY,Y);
	theMaxZ  = Max(theMaxZ,Z);
}

void Visual3d_TransientManager::AddVertex (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real W, const Standard_Boolean AFlag) {

	switch (theTypeOfPrimitive) {
	  case Graphic3d_TOP_POLYLINE :
	    break;
	  case Graphic3d_TOP_POLYGON :
	    break;
	  case Graphic3d_TOP_TRIANGLEMESH :
	    break;
	  case Graphic3d_TOP_QUADRANGLEMESH :
	    break;
	  case Graphic3d_TOP_TEXT :
	    break;
	  case Graphic3d_TOP_MARKER :
	    break;
	  case Graphic3d_TOP_BEZIER :
	    break;
	  default:
	    Visual3d_TransientDefinitionError::Raise
			("Bad Primitive type!");
	}
	theMinX  = Min(theMinX,X);
	theMinY  = Min(theMinY,Y);
	theMinZ  = Min(theMinZ,Z);
	theMaxX  = Max(theMaxX,X);
	theMaxY  = Max(theMaxY,Y);
	theMaxZ  = Max(theMaxZ,Z);
}

void Visual3d_TransientManager::AddVertex (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real NX, const Standard_Real NY, const Standard_Real NZ, const Standard_Boolean AFlag) {

	switch (theTypeOfPrimitive) {
	  case Graphic3d_TOP_POLYLINE :
	    break;
	  case Graphic3d_TOP_POLYGON :
	    break;
	  case Graphic3d_TOP_TRIANGLEMESH :
	    break;
	  case Graphic3d_TOP_QUADRANGLEMESH :
	    break;
	  case Graphic3d_TOP_TEXT :
	    break;
	  case Graphic3d_TOP_MARKER :
	    break;
	  case Graphic3d_TOP_BEZIER :
	    break;
	  default:
	    Visual3d_TransientDefinitionError::Raise
			("Bad Primitive type!");
	}
	theMinX  = Min(theMinX,X);
	theMinY  = Min(theMinY,Y);
	theMinZ  = Min(theMinZ,Z);
	theMaxX  = Max(theMaxX,X);
	theMaxY  = Max(theMaxY,Y);
	theMaxZ  = Max(theMaxZ,Z);
}

void Visual3d_TransientManager::ClosePrimitive () {

	switch (theTypeOfPrimitive) {
	  case Graphic3d_TOP_POLYLINE :
	    theGraphicDriver->EndPolyline();
	    break;
	  case Graphic3d_TOP_POLYGON :
	    break;
	  case Graphic3d_TOP_TRIANGLEMESH :
	    break;
	  case Graphic3d_TOP_QUADRANGLEMESH :
	    break;
	  case Graphic3d_TOP_TEXT :
	    break;
	  case Graphic3d_TOP_MARKER :
	    break;
	  case Graphic3d_TOP_BEZIER :
	    break;
	  default:
	    Visual3d_TransientDefinitionError::Raise
			("Bad Primitive type!");
	}

	theTypeOfPrimitive = Graphic3d_TOP_UNDEFINED;
}

#ifdef WNT
#undef DrawText
#endif
void Visual3d_TransientManager::DrawText (const TCollection_ExtendedString &AText, const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real AHeight, const Quantity_PlaneAngle AAngle, const Graphic3d_TextPath ATp, const Graphic3d_HorizontalTextAlignment AHta, const Graphic3d_VerticalTextAlignment AVta) {

	if( !theDrawingState )
		Visual3d_TransientDefinitionError::Raise
			("Drawing is not open !");
	if( theTypeOfPrimitive != Graphic3d_TOP_UNDEFINED )
		Visual3d_TransientDefinitionError::Raise
			("One primitive is already opened !");
}

void Visual3d_TransientManager::DrawStructure (const Handle(Graphic3d_Structure) &AStructure) {

	if( !theDrawingState )
		Visual3d_TransientDefinitionError::Raise
			("Drawing is not open !");
	if( theTypeOfPrimitive != Graphic3d_TOP_UNDEFINED )
		Visual3d_TransientDefinitionError::Raise
			("One primitive is already opened !");

#ifdef DEBUG_PRO4022
	if (! AStructure->IsEmpty ()) {
Standard_Real RL	= RealLast ();
Standard_Real RF	= RealFirst ();
Standard_Real XMin, YMin, ZMin, XMax, YMax, ZMax;
Standard_ShortReal x1, y1, z1, x2, y2, z2;
		AStructure->MinMaxValues (XMin, YMin, ZMin, XMax, YMax, ZMax);
		if ((XMin == RF) && (YMin == RF) &&
		    (ZMin == RF) && (XMax == RL) &&
		    (YMax == RL) && (ZMax == RL)) {
Standard_ShortReal SRL	= ShortRealLast ();
Standard_ShortReal SRF	= ShortRealFirst ();
			x1 = y1 = z1 = SRF;
			x2 = y2 = z2 = SRL;
		}
		else {
			x1 = Standard_ShortReal (XMin);
			y1 = Standard_ShortReal (YMin);
			z1 = Standard_ShortReal (ZMin);
			x2 = Standard_ShortReal (XMax);
			y2 = Standard_ShortReal (YMax);
			z2 = Standard_ShortReal (ZMax);
		}
		theGraphicDriver->SetMinMax (x1, y1, z1, x2, y2, z2);
#endif /* DEBUG_PRO4022 */

		theGraphicDriver->DrawStructure
			(*(CALL_DEF_STRUCTURE *)AStructure->CStructure ());
	}

}

void Visual3d_TransientManager::SetPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& CTX) {

	if( !theDrawingState )
		Visual3d_TransientDefinitionError::Raise
			("Drawing is not open !");

Standard_Real AWidth;
Quantity_Color AColor;
Aspect_TypeOfLine AType;

	CTX->Values (AColor, AType, AWidth);

	Standard_ShortReal r = Standard_ShortReal(AColor.Red());
	Standard_ShortReal g = Standard_ShortReal(AColor.Green());
	Standard_ShortReal b = Standard_ShortReal(AColor.Blue());
	theGraphicDriver->SetLineColor(r, g, b);

	Standard_Integer t = AType;
	theGraphicDriver->SetLineType(t);

	Standard_ShortReal w = Standard_ShortReal (AWidth);
	theGraphicDriver->SetLineWidth(w);
}

void Visual3d_TransientManager::SetPrimitivesAspect (const Handle(Graphic3d_AspectFillArea3d)& CTX) {

	if( !theDrawingState )
		Visual3d_TransientDefinitionError::Raise
			("Drawing is not open !");

Standard_Real AWidth;
Quantity_Color AIntColor,AEdgeColor;
Aspect_TypeOfLine AType;
Aspect_InteriorStyle AStyle;

	CTX->Values (AStyle,AIntColor,AEdgeColor,AType,AWidth);

}

void Visual3d_TransientManager::SetPrimitivesAspect (const Handle(Graphic3d_AspectText3d)& CTX) {

	if( !theDrawingState )
		Visual3d_TransientDefinitionError::Raise
			("Drawing is not open !");

}

void Visual3d_TransientManager::SetPrimitivesAspect (const Handle(Graphic3d_AspectMarker3d)& CTX) {

	if( !theDrawingState )
		Visual3d_TransientDefinitionError::Raise
			("Drawing is not open !");

Standard_Real AScale;
Quantity_Color AColor;
Aspect_TypeOfMarker AType;

	CTX->Values (AColor,AType,AScale);
 
}

void Visual3d_TransientManager::SetTransform (const TColStd_Array2OfReal& AMatrix, const Graphic3d_TypeOfComposition AType) {

	if( !theDrawingState )
		Visual3d_TransientDefinitionError::Raise
	("Visual3d_TransientManager::SetTransform, Drawing is not open !");

Standard_Integer lr, ur, lc, uc;

	lr = AMatrix.LowerRow ();
	ur = AMatrix.UpperRow ();
	lc = AMatrix.LowerCol ();
	uc = AMatrix.UpperCol ();

	if ( (ur - lr + 1 != 4) || (uc - lc + 1 != 4) )
		Visual3d_TransientDefinitionError::Raise
	("Visual3d_TransientManager::SetTransform, Bad Transformation matrix !");

	theGraphicDriver->Transform (AMatrix, AType);
 
}

void Visual3d_TransientManager::MinMaxValues (Standard_Real& XMin, Standard_Real& YMin, Standard_Real& ZMin, Standard_Real& XMax, Standard_Real& YMax, Standard_Real& ZMax) {

	XMin	= theMinX;
	YMin	= theMinY;
	ZMin	= theMinZ;

	XMax	= theMaxX;
	YMax	= theMaxY;
	ZMax	= theMaxZ;

}

void Visual3d_TransientManager::MinMaxValues (Standard_Real& UMin, Standard_Real& VMin, Standard_Real& UMax, Standard_Real& VMax) {

	UMin	= theMinX;
	VMin	= theMinY;

	UMax	= theMaxX;
	VMax	= theMaxY;

}
