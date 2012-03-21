// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

/***********************************************************************
 
     FONCTION :
     ----------
        Classe V3d_CircularGrid :
 
     VERSION HISTORY  :
     --------------------------------
      IMP230300: GG The color fields and methods have moved in Aspect_Grid


************************************************************************/

#define IMP200100       //GG
//			-> Compute the case Aspect_GDM_None
//                      -> Initialize the grid size according to the
//                        viewer default size.

#define IMP200300       //GG
//                      -> Recompute the grid when any graphic parameter is
//                        modified.

#define IMP300300	//GG
//			-> Draw circles with 2 colors (tenth and normal colors)

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <V3d_CircularGrid.ixx>

#include <TColStd_Array2OfReal.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
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

#define DIVISION 8
#define MYMINMAX 25.
#define MYFACTOR 50.

/*----------------------------------------------------------------------*/

V3d_CircularGrid::V3d_CircularGrid (const V3d_ViewerPointer& aViewer, const Quantity_Color& aColor, const Quantity_Color& aTenthColor):
	Aspect_CircularGrid (1.,8),
	myStructure (new Graphic3d_Structure (aViewer->Viewer ())),
	myGroup1 (new Graphic3d_Group (myStructure)),
	myGroup2 (new Graphic3d_Group (myStructure)),
//	myColor (aColor),
//	myTenthColor (aTenthColor),
	myViewer (aViewer),
	myCurAreDefined (Standard_False)
{
	myColor = aColor;
	myTenthColor = aTenthColor;

	myStructure->SetInfiniteState (Standard_True);

Standard_Real step = 10.;
#ifdef IMP200100
Standard_Real size = myViewer->DefaultViewSize()/2.;
	SetGraphicValues (size, step/MYFACTOR);
#else
	SetGraphicValues (25., step/MYFACTOR);
#endif
	SetRadiusStep (step);
}

void V3d_CircularGrid::SetColors (const Quantity_Color& aColor, const Quantity_Color& aTenthColor) {
        if( myColor != aColor || myTenthColor != aTenthColor ) {
          myColor = aColor;
          myTenthColor = aTenthColor;
          myCurAreDefined = Standard_False;
          UpdateDisplay();
        }
}

void V3d_CircularGrid::Display () {
	myStructure->Display (1);
}

void V3d_CircularGrid::Erase () const {
	myStructure->Erase ();
}

Standard_Boolean V3d_CircularGrid::IsDisplayed () const {
	return myStructure->IsDisplayed ();
}

void V3d_CircularGrid::UpdateDisplay () {
#ifdef TRACE_MINMAX
	cout << "V3d_CircularGrid::UpdateDisplay ()\n";
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
		// Transformation  change of marker
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
			if (! myCurAreDefined || myCurAreDefined &&
				myCurDrawMode == Aspect_GDM_Lines)
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

#ifdef TRACE_VALUES
Standard_Real a11, a12, a13, a21, a22, a23, a31, a32, a33;
	ThePlane.XDirection ().Coord (a11, a12, a13);
	ThePlane.YDirection ().Coord (a21, a22, a23);
	ThePlane.Direction ().Coord (a31, a32, a33);
cout << "Test Plane XDirection " << a11 << ", " << a12 << ", " << a13 << endl;
cout << "Test Plane YDirection " << a21 << ", " << a22 << ", " << a23 << endl;
cout << "Test Plane Direction " << a31 << ", " << a32 << ", " << a33 << endl;
#endif
}

void V3d_CircularGrid::DefineLines () {

Handle(Graphic3d_AspectLine3d) LineAttrib = new Graphic3d_AspectLine3d ();
	LineAttrib->SetColor (myColor);
	LineAttrib->SetType (Aspect_TOL_SOLID);
	LineAttrib->SetWidth (1.0);

Standard_Real r;
Standard_Real aStep = RadiusStep ();
Standard_Real aDivision = DivisionNumber ();
Standard_Real alpha = M_PI / aDivision;

Standard_Integer Division = (Standard_Integer )( (aDivision >= DIVISION ? aDivision : DIVISION));
Graphic3d_Array1OfVertex Cercle (0, 2*Division);
Standard_Real xl, yl, zl = myOffSet;

Standard_Integer i;

Graphic3d_Vertex P1, P2;

Standard_Boolean MakeDiametres = Standard_False;
	if (! myCurAreDefined || myCurDrawMode == Aspect_GDM_Points)
		MakeDiametres = Standard_True;
	else {
		if (aDivision != myCurDivi)
			MakeDiametres = Standard_True;
	}

	if (MakeDiametres) {
#ifdef TRACE_MAKE
		cout << "MakeDiametres" << endl;
#endif
		// diametres
		myGroup1->Clear ();
#ifdef IMP300300
		LineAttrib->SetColor (myTenthColor);
#endif
		myGroup1->SetGroupPrimitivesAspect (LineAttrib);
		myGroup1->BeginPrimitives ();
			P1.SetCoord (0.0, 0.0, -zl);
			for (i=1; i<=2*aDivision; i++) {
				xl = Cos (alpha*i)*myRadius;
				yl = Sin (alpha*i)*myRadius;
				P2.SetCoord (xl, yl, -zl);
				myGroup1->Polyline (P1, P2, Standard_False);
			}
		myGroup1->EndPrimitives ();
		myGroup1->SetMinMaxValues
			(-myRadius, -myRadius, 0.0, myRadius, myRadius, 0.0);
	}

Standard_Boolean MakeCercles = Standard_False;
	if (! myCurAreDefined || myCurDrawMode == Aspect_GDM_Points)
		MakeCercles = Standard_True;
	else {
		if (aStep != myCurStep || aDivision != myCurDivi)
			MakeCercles = Standard_True;
	}

	if (MakeCercles) {
#ifdef TRACE_MAKE
		cout << "MakeCercles" << endl;
#endif
		// circles
#ifdef IMP300300
		Standard_Integer nblines = 0;
#endif
		myGroup2->Clear ();
#ifndef IMP300300
		myGroup2->SetGroupPrimitivesAspect (LineAttrib);
#endif
		myGroup2->BeginPrimitives ();
			alpha = M_PI / Division;
			for (r=aStep; r<=myRadius; r+=aStep) {
				for (i=0; i<=2*Division; i++) {
					xl = Cos (alpha*i)*r;
					yl = Sin (alpha*i)*r;
					Cercle (i).SetCoord (xl, yl, -zl);
				}
#ifdef IMP300300
                                if (Modulus (nblines, 10) != 0) {
				  myGroup2->Polyline (Cercle, Standard_False);
				} else {
				  LineAttrib->SetColor (myTenthColor);
				  myGroup2->SetPrimitivesAspect (LineAttrib);
				  myGroup2->Polyline (Cercle, Standard_False);
				  LineAttrib->SetColor (myColor);
				  myGroup2->SetPrimitivesAspect (LineAttrib);
				}
				nblines++;
#else
				myGroup2->Polyline (Cercle, Standard_False);
#endif
			}
		myGroup2->EndPrimitives ();
		myGroup2->SetMinMaxValues
			(-myRadius, -myRadius, 0.0, myRadius, myRadius, 0.0);
	}
	myCurStep = aStep, myCurDivi = (Standard_Integer ) aDivision;
}

void V3d_CircularGrid::DefinePoints () {

Handle(Graphic3d_AspectMarker3d) MarkerAttrib = new Graphic3d_AspectMarker3d ();
	MarkerAttrib->SetColor (myColor);
	MarkerAttrib->SetType (Aspect_TOM_POINT);
	MarkerAttrib->SetScale (3.);

Standard_Real r;
Standard_Real aStep = RadiusStep ();
Standard_Real aDivision = DivisionNumber ();
Standard_Real alpha = M_PI / aDivision;

Graphic3d_Array1OfVertex Cercle (0,(Standard_Integer )(2*aDivision));
Standard_Real xl, yl, zl = myOffSet;

Standard_Integer i;

Graphic3d_Vertex Point (0.0, 0.0, -zl);

Standard_Boolean MakeDiametres = Standard_False;
	if (! myCurAreDefined || myCurDrawMode == Aspect_GDM_Lines)
		MakeDiametres = Standard_True;
	else {
		if (aDivision != myCurDivi || aStep != myCurStep)
			MakeDiametres = Standard_True;
	}

	if (MakeDiametres) {
#ifdef TRACE_MAKE
		cout << "MakeDiametres" << endl;
#endif
		// diameters
		myGroup1->Clear ();
		myGroup1->SetGroupPrimitivesAspect (MarkerAttrib);
		myGroup1->BeginPrimitives ();
			myGroup1->Marker (Point, Standard_False);
			for (r=aStep; r<=myRadius; r+=aStep) {
				for (i=0; i<=2*aDivision; i++) {
					xl = Cos (alpha*i)*r;
					yl = Sin (alpha*i)*r;
					Cercle (i).SetCoord (xl, yl, -zl);
				}
				myGroup1->MarkerSet (Cercle, Standard_False);
			}
		myGroup1->EndPrimitives ();
		myGroup1->SetMinMaxValues
			(-myRadius, -myRadius, 0.0, myRadius, myRadius, 0.0);
	}
	myCurStep = aStep, myCurDivi = (Standard_Integer ) aDivision;
}

void V3d_CircularGrid::GraphicValues (Standard_Real& theRadius, Standard_Real& theOffSet) const {

	theRadius = myRadius;
	theOffSet = myOffSet;
}

void V3d_CircularGrid::SetGraphicValues (const Standard_Real theRadius, const Standard_Real theOffSet) {

#ifdef TRACE_MINMAX
	cout << "theRadius " << theRadius << " theOffSet " << theOffSet << endl;
#endif
	if (! myCurAreDefined) {
		myRadius = theRadius;
		myOffSet = theOffSet;
	}
	if (myRadius != theRadius) {
		myRadius = theRadius;
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
