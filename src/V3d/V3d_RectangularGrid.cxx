/***********************************************************************
 
     FONCTION :
     ----------
        Classe V3d_CircularGrid :
 
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      16-08-98 : CAL ; S3892. Ajout grilles 3d.
      13-10-98 : CAL ; S3892. Ajout de la gestion de la taille des grilles 3d.
      23-10-98 : CAL ; PRO 15885. Patch K4403 et K4404
      03-11-98 : CAL ; PRO 16161. Patch K4418 et K4419
      IMP230300: GG The color fields and methods have moved in Aspect_Grid


************************************************************************/

#define IMP200100	//GG 
//			-> Compute the case Aspect_GDM_None
//			-> Initialize the grid size according to the
//			  viewer default size.

#define IMP200300	//GG
//			-> Recompute the grid when any graphic parameter is
//			  modified.

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <V3d_RectangularGrid.ixx>

#include <TColStd_Array2OfReal.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Visual3d_ViewManager.hxx>
#include <V3d_Viewer.hxx>

/*----------------------------------------------------------------------*/
/*
 * Constant
 */

#define NO_TRACE_MINMAX
#define NO_TRACE_MAKE
#define NO_TRACE_VALUES
#define NO_TRACE_PLANE 

#define MYMINMAX 25.
#define MYFACTOR 50.

/*----------------------------------------------------------------------*/

V3d_RectangularGrid::V3d_RectangularGrid (const V3d_ViewerPointer& aViewer, const Quantity_Color& aColor, const Quantity_Color& aTenthColor):
	Aspect_RectangularGrid (1.,1.),
	myStructure (new Graphic3d_Structure (aViewer->Viewer ())),
	myGroup1 (new Graphic3d_Group (myStructure)),
	myGroup2 (new Graphic3d_Group (myStructure)),
	myViewer (aViewer),
	myCurAreDefined (Standard_False)
#ifdef TRACE_PLANE
	,myStructureGridPlane (new Graphic3d_Structure (aViewer->Viewer ())),
	myStructureViewPlane (new Graphic3d_Structure (aViewer->Viewer ())),
	myGroupGridPlane (new Graphic3d_Group (myStructureGridPlane)),
	myGroupViewPlane (new Graphic3d_Group (myStructureViewPlane)),
	myGridPlane (gp_Ax3 (gp_Pnt (0.,0.,0), gp_Dir (0.,0.,1.), gp_Dir (1.,0.,0.)))
#endif
{
	myColor = aColor;
	myTenthColor = aTenthColor;

	myStructure->SetInfiniteState (Standard_True);
#ifdef TRACE_PLANE
	myStructureGridPlane->SetInfiniteState (Standard_True);
	myStructureViewPlane->SetInfiniteState (Standard_True);
#endif

#ifdef IMP200100
	Standard_Real step = 10.;
	Standard_Real gstep = step/MYFACTOR;
	Standard_Real size = myViewer->DefaultViewSize()/2.;
#else
	Standard_Real step = 10.;
	Standard_Real gstep = step/MYFACTOR;
	Standard_Real size = MYMINMAX;
#endif
	SetGraphicValues (size, size, gstep);
	SetXStep (step);
	SetYStep (step);
}

void V3d_RectangularGrid::SetColors (const Quantity_Color& aColor, const Quantity_Color& aTenthColor) {
	if( myColor != aColor || myTenthColor != aTenthColor ) {
	  myColor = aColor;
	  myTenthColor = aTenthColor;
	  myCurAreDefined = Standard_False;
  	  UpdateDisplay();
	}
}

void V3d_RectangularGrid::Display () {
	myStructure->Display (1);
#ifdef TRACE_PLANE
	myStructureGridPlane->Display (1);
	myStructureViewPlane->Display (1);
#endif
}

void V3d_RectangularGrid::Erase () const {
	myStructure->Erase ();
#ifdef TRACE_PLANE
	myStructureGridPlane->Erase ();
	myStructureViewPlane->Erase ();
#endif
}

Standard_Boolean V3d_RectangularGrid::IsDisplayed () const {
	return myStructure->IsDisplayed ();
}

void V3d_RectangularGrid::UpdateDisplay () {
#ifdef TRACE_MINMAX
	cout << "V3d_RectangularGrid::UpdateDisplay ()" << endl;
#endif

gp_Ax3 ThePlane;
	ThePlane = myViewer->PrivilegedPlane ();

Standard_Boolean MakeTransform = Standard_False;
Standard_Real xl, yl, zl;
Standard_Real xdx, xdy, xdz;
Standard_Real ydx, ydy, ydz;
Standard_Real dx, dy, dz;
	ThePlane.Location ().Coord (xl, yl, zl);
	ThePlane.XDirection ().Coord (xdx, xdy, xdz);
	ThePlane.YDirection ().Coord (ydx, ydy, ydz);
	ThePlane.Direction ().Coord (dx, dy, dz);
	if (! myCurAreDefined)
	    MakeTransform = Standard_True;
	else {
	    if (RotationAngle () != myCurAngle
	    ||  XOrigin () != myCurXo ||  YOrigin () != myCurYo)
	    	MakeTransform = Standard_True;
	    if (! MakeTransform) {
Standard_Real curxl, curyl, curzl;
Standard_Real curxdx, curxdy, curxdz;
Standard_Real curydx, curydy, curydz;
Standard_Real curdx, curdy, curdz;
		myCurViewPlane.Location ().Coord (curxl, curyl, curzl);
		myCurViewPlane.XDirection ().Coord (curxdx, curxdy, curxdz);
		myCurViewPlane.YDirection ().Coord (curydx, curydy, curydz);
		myCurViewPlane.Direction ().Coord (curdx, curdy, curdz);
		if (xl != curxl || yl != curyl || zl != curzl
		||  xdx != curxdx || xdy != curxdy || xdz != curxdz
		||  ydx != curydx || ydy != curydy || ydz != curydz
		||  dx != curdx || dy != curdy || dz != curdz)
			MakeTransform = Standard_True;
	    }
	}

	if (MakeTransform) {
#ifdef TRACE_MAKE
		cout << "MakeTransform" << endl;
#endif
Standard_Real CosAlpha = Cos (RotationAngle ());
Standard_Real SinAlpha = Sin (RotationAngle ());
TColStd_Array2OfReal Trsf (1, 4, 1, 4);
		Trsf (4, 4) = 1.0;
		Trsf (4, 1) = Trsf (4, 2) = Trsf (4, 3) = 0.0;
		// Translation
		Trsf (1, 4) = xl,
		Trsf (2, 4) = yl,
		Trsf (3, 4) = zl;
		// Transformation of change of marker
		Trsf (1, 1) = xdx,
		Trsf (2, 1) = xdy,
		Trsf (3, 1) = xdz,
		Trsf (1, 2) = ydx,
		Trsf (2, 2) = ydy,
		Trsf (3, 2) = ydz,
		Trsf (1, 3) = dx,
		Trsf (2, 3) = dy,
		Trsf (3, 3) = dz;
		myStructure->SetTransform (Trsf, Graphic3d_TOC_REPLACE);

		// Translation of the origin
		Trsf (1, 4) = -XOrigin (),
		Trsf (2, 4) = -YOrigin (),
		Trsf (3, 4) = 0.0;
		// Rotation Alpha around axis -Z
		Trsf (1, 1) = CosAlpha,
		Trsf (2, 1) = -SinAlpha,
		Trsf (3, 1) = 0.0,
		Trsf (1, 2) = SinAlpha,
		Trsf (2, 2) = CosAlpha,
		Trsf (3, 2) = 0.0,
		Trsf (1, 3) = 0.0,
		Trsf (2, 3) = 0.0,
		Trsf (3, 3) = 1.0;
		myStructure->SetTransform (Trsf,Graphic3d_TOC_POSTCONCATENATE);


#ifdef TRACE_VALUES
Standard_Integer i, j;
TColStd_Array2OfReal TheMatrix (1, 4, 1, 4);
		myStructure->Transform (TheMatrix);

		for (i=1; i<=4; i++) {
			for (j=1; j<=4; j++)
				cout << TheMatrix (i, j) << "\t";
			cout << endl;
		}
#endif

		myCurAngle = RotationAngle ();
		myCurXo = XOrigin (), myCurYo = YOrigin ();
		myCurViewPlane = ThePlane;
	}

	switch (DrawMode ()) {
		default:
		case Aspect_GDM_Points:
#ifdef IMP210100
			if (! myCurAreDefined || 
				(myCurDrawMode == Aspect_GDM_Lines) )
#else
			if (! myCurAreDefined || myCurAreDefined &&
				myCurDrawMode == Aspect_GDM_Lines)
#endif
					myGroup2->Clear ();
			DefinePoints ();
			myCurDrawMode = Aspect_GDM_Points;
		break;
		case Aspect_GDM_Lines:
			DefineLines ();
			myCurDrawMode = Aspect_GDM_Lines;
		break;
#ifdef IMP210100
		case Aspect_GDM_None:
			myCurDrawMode = Aspect_GDM_None;
		break;
#endif
	}
	myCurAreDefined = Standard_True;

#ifdef TRACE_PLANE
Handle(Graphic3d_AspectLine3d) LineAttrib = new Graphic3d_AspectLine3d ();
Handle(Graphic3d_AspectText3d) TextAttrib = new Graphic3d_AspectText3d ();
Graphic3d_Array1OfVertex Points (0, 1);
Standard_Real ay, by, cy;

	myGroupViewPlane->Clear ();

	LineAttrib->SetColor (Quantity_Color (Quantity_NOC_GREEN));
	myGroupViewPlane->SetGroupPrimitivesAspect (LineAttrib);

	TextAttrib->SetColor (Quantity_Color(Quantity_NOC_GREEN));
	myGroupViewPlane->SetGroupPrimitivesAspect (TextAttrib);

	ThePlane.Location ().Coord (xl, yl, zl);
	Points (0).SetCoord (xl, yl, zl);

	myGroupViewPlane->BeginPrimitives ();
		myGroupViewPlane->Text ("VIEWER PLANE", Points (0), 1./81.);

		myViewer->PrivilegedPlane ().XDirection ().Coord (ay, by, cy);
		Points (1).SetCoord
		(xl + XStep ()*ay, yl + XStep ()*by, zl + XStep ()*cy);
		myGroupViewPlane->Polyline (Points);
		myGroupViewPlane->Text ("X", Points (1), 1./81.);

		myViewer->PrivilegedPlane ().YDirection ().Coord (ay, by, cy);
		Points (1).SetCoord
		(xl + XStep ()*ay, yl + XStep ()*by, zl + XStep ()*cy);
		myGroupViewPlane->Polyline (Points);
		myGroupViewPlane->Text ("Y", Points (1), 1./81.);

		myViewer->PrivilegedPlane ().Direction ().Coord (ay, by, cy);
		Points (1).SetCoord
		(xl + XStep ()*ay, yl + XStep ()*by, zl + XStep ()*cy);
		myGroupViewPlane->Polyline (Points);
		myGroupViewPlane->Text ("Z", Points (1), 1./81.);
	myGroupViewPlane->EndPrimitives ();

#ifdef TRACE_VALUES
Standard_Real xangle, yangle, angle;
Standard_Real a11, a12, a13, a21, a22, a23, a31, a32, a33;
	xangle = ThePlane.XDirection ().Angle (myGridPlane.XDirection ());
	yangle = ThePlane.YDirection ().Angle (myGridPlane.YDirection ());
	angle = ThePlane.Direction ().Angle (myGridPlane.Direction ());
cout << "Angle entre Directions " << angle << endl;
cout << "Angle entre XDirections " << xangle << endl;
cout << "Angle entre YDirections " << yangle << endl;
	ThePlane.XDirection ().Coord (a11, a12, a13);
	ThePlane.YDirection ().Coord (a21, a22, a23);
	ThePlane.Direction ().Coord (a31, a32, a33);
cout << "Test Plane XDirection " << a11 << ", " << a12 << ", " << a13 << endl;
cout << "Test Plane YDirection " << a21 << ", " << a22 << ", " << a23 << endl;
cout << "Test Plane Direction " << a31 << ", " << a32 << ", " << a33 << endl;
#endif

	myGroupGridPlane->Clear ();

	LineAttrib->SetColor (Quantity_Color (Quantity_NOC_RED));
	myGroupGridPlane->SetGroupPrimitivesAspect (LineAttrib);

	TextAttrib->SetColor (Quantity_Color(Quantity_NOC_RED));
	myGroupGridPlane->SetGroupPrimitivesAspect (TextAttrib);

	myGridPlane.Location ().Coord (xl, yl, zl);
	Points (0).SetCoord (xl, yl, zl);

	myGroupGridPlane->BeginPrimitives ();
		myGroupGridPlane->Text ("GRID PLANE", Points (0), 1./81.);

		myGridPlane.XDirection ().Coord (ay, by, cy);
		Points (1).SetCoord
		(xl + XStep ()*ay, yl + XStep ()*by, zl + XStep ()*cy);
		myGroupGridPlane->Polyline (Points);
		myGroupGridPlane->Text ("X", Points (1), 1./81.);

		myGridPlane.YDirection ().Coord (ay, by, cy);
		Points (1).SetCoord
		(xl + XStep ()*ay, yl + XStep ()*by, zl + XStep ()*cy);
		myGroupGridPlane->Polyline (Points);
		myGroupGridPlane->Text ("Y", Points (1), 1./81.);

		myGridPlane.Direction ().Coord (ay, by, cy);
		Points (1).SetCoord
		(xl + XStep ()*ay, yl + XStep ()*by, zl + XStep ()*cy);
		myGroupGridPlane->Polyline (Points);
		myGroupGridPlane->Text ("Z", Points (1), 1./81.);
	myGroupGridPlane->EndPrimitives ();
#endif
}

void V3d_RectangularGrid::DefineLines () {

Handle(Graphic3d_AspectLine3d) LineAttrib = new Graphic3d_AspectLine3d ();
	LineAttrib->SetColor (myColor);
	LineAttrib->SetType (Aspect_TOL_SOLID);
	LineAttrib->SetWidth (1.0);

Standard_Real aXStep = XStep ();
Standard_Real aYStep = YStep ();
Standard_Integer nblines, p;
Standard_Real xl, yl, zl = myOffSet;

Graphic3d_Vertex P1, P2;

Standard_Boolean MakeVerticales = Standard_False;
	if (! myCurAreDefined || myCurDrawMode == Aspect_GDM_Points)
		MakeVerticales = Standard_True;
	else {
		if (aXStep != myCurXStep)
			MakeVerticales = Standard_True;
	}

	if (MakeVerticales) {
#ifdef TRACE_MAKE
		cout << "MakeVerticales" << endl;
#endif
		// verticals
		myGroup1->Clear ();
		LineAttrib->SetColor (myTenthColor);
		myGroup1->SetGroupPrimitivesAspect (LineAttrib);
		myGroup1->BeginPrimitives ();
		P1.SetCoord (0.0, -myYSize, -zl);
		P2.SetCoord (0.0, myYSize, -zl);
		myGroup1->Polyline (P1, P2, Standard_False);
		nblines = 1; // start with a dark grey line 
		LineAttrib->SetColor (myColor);
		myGroup1->SetPrimitivesAspect (LineAttrib);
		for (xl = aXStep; xl < myXSize; xl += aXStep) {
			P1.SetCoord (xl, -myYSize, -zl);
			P2.SetCoord (xl, myYSize, -zl);
			p = Modulus (nblines, 10);
			if (p != 0) {
				myGroup1->Polyline (P1, P2, Standard_False);
				P1.SetCoord (-xl, -myYSize, -zl);
				P2.SetCoord (-xl, myYSize, -zl);
				myGroup1->Polyline (P1, P2, Standard_False);
			}
			else {
				LineAttrib->SetColor (myTenthColor);
				myGroup1->SetPrimitivesAspect (LineAttrib);
				myGroup1->Polyline (P1, P2, Standard_False);
				P1.SetCoord (-xl, -myYSize, -zl);
				P2.SetCoord (-xl, myYSize, -zl);
				myGroup1->Polyline (P1, P2, Standard_False);
				LineAttrib->SetColor (myColor);
				myGroup1->SetPrimitivesAspect (LineAttrib);
			}
			nblines ++;
	    	}
		myGroup1->EndPrimitives ();
		myGroup1->SetMinMaxValues
			(-myXSize, -myYSize, 0.0, myXSize, myYSize, 0.0);
	}

Standard_Boolean MakeHorizontales = Standard_False;
	if (! myCurAreDefined || myCurDrawMode == Aspect_GDM_Points)
		MakeHorizontales = Standard_True;
	else {
		if (aYStep != myCurYStep)
			MakeHorizontales = Standard_True;
	}

	if (MakeHorizontales) {
#ifdef TRACE_MAKE
		cout << "MakeHorizontales" << endl;
#endif
		// horizontals
		myGroup2->Clear ();
		LineAttrib->SetColor (myTenthColor);
		myGroup2->SetGroupPrimitivesAspect (LineAttrib);
		myGroup2->BeginPrimitives ();
		P1.SetCoord (-myXSize, 0., -zl);
		P2.SetCoord (myXSize, 0., -zl);
		myGroup2->Polyline (P1, P2, Standard_False);
		nblines = 1; // start with a dark grey line
		LineAttrib->SetColor (myColor);
		myGroup2->SetPrimitivesAspect (LineAttrib);
		for (yl = aYStep; yl < myYSize; yl += aYStep) {
			P1.SetCoord (-myXSize, yl, -zl);
			P2.SetCoord (myXSize, yl, -zl);
			p = Modulus (nblines, 10);
			if (p != 0) {
				myGroup2->Polyline (P1, P2, Standard_False);
				P1.SetCoord (-myXSize, -yl, -zl);
				P2.SetCoord (myXSize, -yl, -zl);
				myGroup2->Polyline (P1, P2, Standard_False);
			}
			else {
				LineAttrib->SetColor (myTenthColor);
				myGroup2->SetPrimitivesAspect (LineAttrib);
				myGroup2->Polyline (P1, P2, Standard_False);
				P1.SetCoord (-myXSize, -yl, -zl);
				P2.SetCoord (myXSize, -yl, -zl);
				myGroup2->Polyline (P1, P2, Standard_False);
				LineAttrib->SetColor (myColor);
				myGroup2->SetPrimitivesAspect (LineAttrib);
			}
			nblines ++;
		}
		myGroup2->EndPrimitives ();
		myGroup2->SetMinMaxValues
			(-myXSize, -myYSize, 0.0, myXSize, myYSize, 0.0);
	}
	myCurXStep = aXStep, myCurYStep = aYStep;
}

void V3d_RectangularGrid::DefinePoints () {

Handle(Graphic3d_AspectMarker3d) MarkerAttrib = new Graphic3d_AspectMarker3d ();
	MarkerAttrib->SetColor (myColor);
	MarkerAttrib->SetType (Aspect_TOM_POINT);
	MarkerAttrib->SetScale (3.);

Standard_Real aXStep = XStep ();
Standard_Real aYStep = YStep ();
Standard_Integer i, nbpoints;
	nbpoints = Standard_Integer (myYSize/aYStep);
	nbpoints = 4 * nbpoints + 2;

Graphic3d_Array1OfVertex Vertical (1, nbpoints);
Standard_Real xl, yl, zl = myOffSet;

Standard_Boolean MakeHorizontales = Standard_False;
	if (! myCurAreDefined || myCurDrawMode == Aspect_GDM_Lines)
		MakeHorizontales = Standard_True;
	else {
		if (aXStep != myCurXStep || aYStep != myCurYStep)
			MakeHorizontales = Standard_True;
	}

	if (MakeHorizontales) {
#ifdef TRACE_MAKE
		cout << "MakeHorizontales" << endl;
#endif
		// horizontals
		myGroup1->Clear ();
		myGroup1->SetGroupPrimitivesAspect (MarkerAttrib);
		myGroup1->BeginPrimitives ();
			for (xl = 0.0; xl <= myXSize; xl += aXStep) {
				i = 1;
				Vertical (i++).SetCoord (xl, 0.0, -zl);
				Vertical (i++).SetCoord (-xl, 0.0, -zl);
				for (yl = aYStep; yl <= myYSize; yl += aYStep) {
	if (i<=nbpoints)		Vertical (i++).SetCoord (xl, yl, -zl);
	if (i<=nbpoints)		Vertical (i++).SetCoord (xl, -yl, -zl);
	if (i<=nbpoints)		Vertical (i++).SetCoord (-xl, yl, -zl);
	if (i<=nbpoints)		Vertical (i++).SetCoord (-xl, -yl, -zl);
				}
			myGroup1->MarkerSet (Vertical, Standard_False);
			}
		myGroup1->EndPrimitives ();
		myGroup1->SetMinMaxValues
			(-myXSize, -myYSize, 0.0, myXSize, myYSize, 0.0);
	}
	myCurXStep = aXStep, myCurYStep = aYStep;
}

void V3d_RectangularGrid::GraphicValues (Standard_Real& theXSize, Standard_Real& theYSize, Standard_Real& theOffSet) const {

	theXSize = myXSize;
	theYSize = myYSize;
	theOffSet = myOffSet;
}

void V3d_RectangularGrid::SetGraphicValues (const Standard_Real theXSize, const Standard_Real theYSize, const Standard_Real theOffSet) {

#ifdef TRACE_MINMAX
	cout << "theXSize " << theXSize << " theYSize " << theYSize
	     << " theOffSet " << theOffSet << endl;
#endif
	if (! myCurAreDefined) {
		myXSize = theXSize;
		myYSize = theYSize;
		myOffSet = theOffSet;
	}
	if (myXSize != theXSize) {
		myXSize = theXSize;
		myCurAreDefined = Standard_False;
	}
	if (myYSize != theYSize) {
		myYSize = theYSize;
		myCurAreDefined = Standard_False;
	}
	if (myOffSet != theOffSet) {
		myOffSet = theOffSet;
		myCurAreDefined = Standard_False;
	}
#ifdef IMP200300
        if( !myCurAreDefined ) UpdateDisplay();
#endif
}
