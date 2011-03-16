/***********************************************************************
 
     FONCTION :
     ----------
        Classe V3d_View :
 
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      16-08-98 : CAL ; S3892. Ajout grilles 3d.
      16-09-98 : BGN ; Points d'entree du Triedre (S3819, Phase 1)
      21-09-98 : CAL ; Activation de l'echo que si CSF_GraphicEcho est definie.
      22-09-98 : BGN ; S3989 (anciennement S3819)
                       TypeOfTriedron* from Aspect (pas V3d)
      02.15.100 : JR : Clutter
      28/05/02 :  VSV: ZBUFFER mode of Trihedron

************************************************************************/

#define IMP240100       //GG
//			-> In Compute() method call MyViewer->ShowGridEcho()


/*----------------------------------------------------------------------*/
/*
 * Includes
 */
#include <V3d_View.jxx>

#include <TColStd_Array2OfReal.hxx>

#include <gp_Dir2d.hxx>

#include <Aspect.hxx>

#include <V3d_RectangularGrid.hxx>
#include <V3d_CircularGrid.hxx>

// Pour l'echo du point choisi
#include <Visual3d_TransientManager.hxx>

/*----------------------------------------------------------------------*/
/*
 * Constant
 */

// en cas NO_TRACE_ECHO et de NO_TRACE_POINTS, dans V3d_View_4.cxx ET dans
// V3d_View.cxx, virer MyGridEchoStructure et MyGridEchoGroup dans le cdl
#define TRACE_ECHO
#define NO_TRACE_VALUES
#define NO_TRACE_POINTS

#define MYEPSILON1 0.0001		// Comparaison avec 0.0
#define MYEPSILON2 Standard_PI / 180.	// Delta entre 2 angles

#ifdef TRACE_POINTS
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#endif

#ifdef TRACE_VALUES
#include <Visual3d_ViewOrientation.hxx>
#endif

#if defined TRACE_VALUES || defined TRACE_POINTS
static char *CAR [26] = {"a", "b", "c", "d", "e", "f", "g", "h",
			"i", "j", "k", "l", "m", "n", "o", "p",
			"q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
#endif

/*----------------------------------------------------------------------*/

void V3d_View::SetGrid (const gp_Ax3& aPlane, const Handle(Aspect_Grid)& aGrid) {

	MyPlane	= aPlane;
	MyGrid	= aGrid;

#ifdef TRACE_VALUES
        if (MyGrid->IsKind (STANDARD_TYPE (Aspect_RectangularGrid))) {
		cout << "Aspect_RectangularGrid" << endl;
Handle(Aspect_RectangularGrid) theGrid = *(Handle(Aspect_RectangularGrid) *) &MyGrid;
		cout << "\tOrigine : " << theGrid->XOrigin () << ", "
		     << theGrid->YOrigin () << endl;
		cout << "\tAngle : " << theGrid->RotationAngle () << endl;
		cout << "\tSteps : " << theGrid->XStep () << ", "
		     << theGrid->YStep () << endl;
	}
	else {
		cout << "Aspect_CircularGrid" << endl;
Handle(Aspect_CircularGrid) theGrid = *(Handle(Aspect_CircularGrid) *) &MyGrid;
		cout << "\tOrigine : " << theGrid->XOrigin () << ", "
		     << theGrid->YOrigin () << endl;
		cout << "\tAngle : " << theGrid->RotationAngle () << endl;
		cout << "\tRadiusStep, Division : " << theGrid->RadiusStep ()
		     << ", " << theGrid->DivisionNumber () << endl;
	}

#endif

Standard_Real xl, yl, zl;
Standard_Real xdx, xdy, xdz;
Standard_Real ydx, ydy, ydz;
Standard_Real dx, dy, dz;
	aPlane.Location ().Coord (xl, yl, zl);
	aPlane.XDirection ().Coord (xdx, xdy, xdz);
	aPlane.YDirection ().Coord (ydx, ydy, ydz);
	aPlane.Direction ().Coord (dx, dy, dz);

#ifdef TRACE_VALUES
cout << "Grid Plane " << xl << ", " << yl << ", " << zl << endl;
cout << "\tLocation " << xl << ", " << yl << ", " << zl << endl;
cout << "\tDirection " << dx << ", " << dy << ", " << dz << endl;
cout << "\tXDirection " << xdx << ", " << xdy << ", " << xdz << endl;
cout << "\tYDirection " << ydx << ", " << ydy << ", " << ydz << endl;
#endif

Standard_Real CosAlpha = Cos (MyGrid->RotationAngle ());
Standard_Real SinAlpha = Sin (MyGrid->RotationAngle ());

TColStd_Array2OfReal Trsf1 (1, 4, 1, 4);
	Trsf1 (4, 4) = 1.0;
	Trsf1 (4, 1) = Trsf1 (4, 2) = Trsf1 (4, 3) = 0.0;
	// Translation
	Trsf1 (1, 4) = xl,
	Trsf1 (2, 4) = yl,
	Trsf1 (3, 4) = zl;
	// Transformation de changement de repere
	Trsf1 (1, 1) = xdx,
	Trsf1 (2, 1) = xdy,
	Trsf1 (3, 1) = xdz,
	Trsf1 (1, 2) = ydx,
	Trsf1 (2, 2) = ydy,
	Trsf1 (3, 2) = ydz,
	Trsf1 (1, 3) = dx,
	Trsf1 (2, 3) = dy,
	Trsf1 (3, 3) = dz;

TColStd_Array2OfReal Trsf2 (1, 4, 1, 4);
	Trsf2 (4, 4) = 1.0;
	Trsf2 (4, 1) = Trsf2 (4, 2) = Trsf2 (4, 3) = 0.0;
	// Translation de l'origine
	Trsf2 (1, 4) = -MyGrid->XOrigin (),
	Trsf2 (2, 4) = -MyGrid->YOrigin (),
	Trsf2 (3, 4) = 0.0;
	// Rotation Alpha autour de l'axe -Z
	Trsf2 (1, 1) = CosAlpha,
	Trsf2 (2, 1) = -SinAlpha,
	Trsf2 (3, 1) = 0.0,
	Trsf2 (1, 2) = SinAlpha,
	Trsf2 (2, 2) = CosAlpha,
	Trsf2 (3, 2) = 0.0,
	Trsf2 (1, 3) = 0.0,
	Trsf2 (2, 3) = 0.0,
	Trsf2 (3, 3) = 1.0;

Standard_Real valuetrsf;
Standard_Real valueoldtrsf;
Standard_Real valuenewtrsf;
Standard_Integer i, j, k;
	// Le calcul de produit de matrices
	for (i=1; i<=4; i++)
	    for (j=1; j<=4; j++) {
		MyTrsf (i, j) = 0.0;
		for (k=1; k<=4; k++) {
		    valueoldtrsf = Trsf1 (i, k);
		    valuetrsf	 = Trsf2 (k, j);
		    valuenewtrsf = MyTrsf (i, j) + valueoldtrsf * valuetrsf;
		    MyTrsf (i, j) = valuenewtrsf;
		}
	   }

}

void V3d_View::SetGridActivity (const Standard_Boolean AFlag) {

	if (AFlag) MyGrid->Activate ();
	else MyGrid->Deactivate ();

}

void V3d_View::SetGridGraphicValues (const Handle(Aspect_Grid)&
#ifdef TRACE_VALUES
                                                                aGrid
#endif
                                    ) {

#ifdef TRACE_VALUES
Standard_Real XSize, YSize, Radius, OffSet;
        if (aGrid->IsKind (STANDARD_TYPE (V3d_RectangularGrid))) {
		cout << "V3d_RectangularGrid" << endl;
Handle(V3d_RectangularGrid) theGrid = *(Handle(V3d_RectangularGrid) *) &aGrid;
		theGrid->GraphicValues (XSize, YSize, OffSet);
		cout << "\tSizes : " << XSize << ", " << YSize << endl;
		cout << "\tOffSet : " << OffSet << endl;
	}
	else {
		cout << "V3d_CircularGrid" << endl;
Handle(V3d_CircularGrid) theGrid = *(Handle(V3d_CircularGrid) *) &aGrid;
		theGrid->GraphicValues (Radius, OffSet);
		cout << "\tRadius : " << Radius << endl;
		cout << "\tOffSet : " << OffSet << endl;
	}
#endif

}

Graphic3d_Vertex V3d_View::Compute (const Graphic3d_Vertex & AVertex) const {

Graphic3d_Vertex CurPoint, NewPoint;
Standard_Real X1, Y1, Z1, X2, Y2, Z2;
Standard_Real XPp, YPp;

	MyView->ViewOrientation ().ViewReferencePlane ().Coord (X1, Y1, Z1);
	MyPlane.Direction ().Coord (X2, Y2, Z2);

gp_Dir VPN (X1, Y1, Z1);
gp_Dir GPN (X2, Y2, Z2);

#ifdef TRACE_VALUES
	cout << "View Plane : " << X1 << ", " << Y1 << ", " << Z1 << endl;
	cout << "Grid Plane " << X2 << ", " << Y2 << ", " << Z2 << endl;
	cout << "Angle VPN GPN : " << VPN.Angle (GPN) << endl;
#endif

	AVertex.Coord (X1, Y1, Z1);
	Project (X1, Y1, Z1, XPp, YPp);

#ifdef TRACE_VALUES
	cout << "AVertex : " << X1 << ", " << Y1 << ", " << Z1 << endl;
#endif

	// Cas ou le plan de la grille et le plan de la vue
	// sont perpendiculaires a MYEPSILON2 radians pres
	if (Abs (VPN.Angle (GPN) - Standard_PI / 2.) < MYEPSILON2) {
		NewPoint.SetCoord (X1, Y1, Z1);
#ifdef TRACE_POINTS
		TextAttrib->SetColor (Quantity_Color (Quantity_NOC_RED));
		MyGridEchoGroup->SetPrimitivesAspect (TextAttrib);
		MyGridEchoGroup->Text ("Q", NewPoint, 1./81.);
		MyGridEchoGroup->Polyline (AVertex, NewPoint);
		Update ();
#endif
#ifdef IMP240100
		MyViewer->ShowGridEcho(this,NewPoint);
#else
#ifdef TRACE_ECHO
char *trace_echo = NULL;
		trace_echo = (char *)(getenv ("CSF_GraphicEcho"));
		if (trace_echo) {
			MyGridEchoGroup->Clear ();
			MyGridEchoGroup->Marker (NewPoint);
			Visual3d_TransientManager::BeginDraw
				(MyView, Standard_False, Standard_False);
			Visual3d_TransientManager::DrawStructure (MyGridEchoStructure);
			Visual3d_TransientManager::EndDraw (Standard_True);
		}
#endif
#endif	//IMP240100
		return NewPoint;
	}

#ifdef TRACE_POINTS
Handle(Graphic3d_AspectLine3d) LineAttrib = new Graphic3d_AspectLine3d ();
Handle(Graphic3d_AspectText3d) TextAttrib = new Graphic3d_AspectText3d ();

	MyGridEchoGroup->Clear ();

	LineAttrib->SetColor (Quantity_Color (Quantity_NOC_RED));
	MyGridEchoGroup->SetGroupPrimitivesAspect (LineAttrib);

	TextAttrib->SetColor (Quantity_Color (Quantity_NOC_RED));
	MyGridEchoGroup->SetGroupPrimitivesAspect (TextAttrib);

	MyGridEchoGroup->Text ("P", AVertex, 1./81.);
	CurPoint.SetCoord (AVertex.X (), AVertex.Y (), AVertex.Z ());

	TextAttrib->SetColor (Quantity_Color (Quantity_NOC_GREEN));
	MyGridEchoGroup->SetPrimitivesAspect (TextAttrib);
#endif

Standard_Boolean IsRectangular = 
		MyGrid->IsKind (STANDARD_TYPE (Aspect_RectangularGrid));

Graphic3d_Vertex P1;

Standard_Real XO = 0.0, YO = 0.0;
Standard_Real XOp, YOp;
Standard_Real XAp, YAp;
Standard_Real XBp, YBp;

	X1 = XO, Y1 = YO, Z1 = 0.0;
	// MyTrsf * Point pour se retrouver sur le plan de la grille 3D
	P1.SetCoord (X1, Y1, Z1);
	CurPoint = V3d_View::TrsPoint (P1, MyTrsf);
	CurPoint.Coord (X2, Y2, Z2);
	Project (X2, Y2, Z2, XOp, YOp);
	XPp = XPp - XOp, YPp = YPp - YOp;

#ifdef TRACE_POINTS
	MyGridEchoGroup->Text ("O", CurPoint, 1./81.);
#endif

#ifdef TRACE_VALUES
	cout << "Projection de P : " << XPp << ", " << YPp << endl;
	cout << "Projection de O : " << XOp << ", " << YOp << endl;
#endif

        if (IsRectangular) {
Standard_Real XS, YS;
Handle(Aspect_RectangularGrid) theGrid =
			*(Handle(Aspect_RectangularGrid) *) &MyGrid;
		XS = theGrid->XStep (), YS = theGrid->YStep ();

		X1 = XO + XS, Y1 = YO, Z1 = 0.0;
		// MyTrsf * Point pour se retrouver sur le plan de la grille 3D
		P1.SetCoord (X1, Y1, Z1);
		CurPoint = V3d_View::TrsPoint (P1, MyTrsf);
		CurPoint.Coord (X2, Y2, Z2);
		Project (X2, Y2, Z2, XAp, YAp);
		XAp = XAp - XOp, YAp = YAp - YOp;

#ifdef TRACE_POINTS
		MyGridEchoGroup->Text ("A", CurPoint, 1./81.);
#endif

		X1 = XO, Y1 = YO + YS, Z1 = 0.0;
		// MyTrsf * Point pour se retrouver sur le plan de la grille 3D
		P1.SetCoord (X1, Y1, Z1);
		CurPoint = V3d_View::TrsPoint (P1, MyTrsf);
		CurPoint.Coord (X2, Y2, Z2);
		Project (X2, Y2, Z2, XBp, YBp);
		XBp = XBp - XOp, YBp = YBp - YOp;

#ifdef TRACE_POINTS
		MyGridEchoGroup->Text ("B", CurPoint, 1./81.);
#endif

#ifdef TRACE_VALUES
		cout << "Projection de A : " << XAp << ", " << YAp << endl;
		cout << "Projection de B : " << XBp << ", " << YBp << endl;
#endif

Standard_Real Determin = XAp*YBp - XBp*YAp;

		Z1 = 0.0;
		if (Abs (Determin) > MYEPSILON1) {
			X1 = (YBp*XPp - XBp*YPp) / Determin;
			Y1 = (XAp*YPp - YAp*XPp) / Determin;
			X1 = (X1 > 0. ?
			Standard_Real (Standard_Integer (Abs (X1)+0.5)) * XS :
			- Standard_Real (Standard_Integer (Abs (X1)+0.5)) * XS);
			Y1 = (Y1 > 0. ?
			Standard_Real (Standard_Integer (Abs (Y1)+0.5)) * YS :
			- Standard_Real (Standard_Integer (Abs (Y1)+0.5)) * YS);
#ifdef TRACE_VALUES
			cout << "Point choisi : " << X1 << ", " << Y1 << endl;
#endif
			// MyTrsf * Point pour se retrouver sur le plan
			// de la grille 3D
			P1.SetCoord (X1, Y1, Z1);
			CurPoint = V3d_View::TrsPoint (P1, MyTrsf);
			CurPoint.Coord (X2, Y2, Z2);
		}
		else {
cout << "*****************" << endl;
cout << "Determinant nul !" << endl;
cout << "*****************" << endl;
#ifdef TRACE_VALUES
			cout << "Determinant nul !" << endl;
#endif
			AVertex.Coord (X2, Y2, Z2);
			CurPoint.SetCoord (X2, Y2, Z2);
		}
	} // IsRectangular
        else {
Standard_Real RS;
Standard_Integer DN;
Standard_Real Alpha;
Handle(Aspect_CircularGrid) theGrid =
			*(Handle(Aspect_CircularGrid) *) &MyGrid;
		RS = theGrid->RadiusStep ();
		DN = theGrid->DivisionNumber ();
		Alpha = Standard_PI / Standard_Real (DN);

Standard_Real DistOP = Sqrt (XPp*XPp + YPp*YPp);

Standard_Integer i, ICur=0;
Standard_Real Angle, AngleCur;
Standard_Real XCurp=0, YCurp=0;
gp_Dir2d OP (XPp, YPp);
		AngleCur = 2 * Standard_PI;
		for (i=1; i<=DN*2; i++) {
			X1 = XO + Cos (Alpha * i) * RS,
			Y1 = YO + Sin (Alpha * i) * RS,
			Z1 = 0.0;
			// MyTrsf * Point pour se retrouver sur le plan
			// de la grille 3D
			P1.SetCoord (X1, Y1, Z1);
			CurPoint = V3d_View::TrsPoint (P1, MyTrsf);
			CurPoint.Coord (X2, Y2, Z2);
			Project (X2, Y2, Z2, XAp, YAp);
			XAp = XAp - XOp, YAp = YAp - YOp;
			gp_Dir2d OA (XAp, YAp);
			Angle = OP.Angle (OA);
			if (Abs (AngleCur) > Abs (Angle)) {
				ICur = i;
				AngleCur = Angle;
				XCurp = XAp, YCurp = YAp;
#ifdef TRACE_VALUES
				cout << "Angle entre OP et O" << CAR [i-1]
				     << " : " << Angle << endl;
#endif
			}

#ifdef TRACE_POINTS
			MyGridEchoGroup->Text (CAR [i-1], CurPoint, 1./81.);
#endif
		} // for (i=1; i<=DN*2; i++)

Standard_Real DistOCur = Sqrt (XCurp*XCurp + YCurp*YCurp);

		// Determination du cercle de la grille le plus proche de P
Standard_Integer N = Standard_Integer (DistOP / DistOCur + 0.5);
Standard_Real Radius = N * RS;
#ifdef TRACE_VALUES
		cout << "Cercle : " << N << " Radius : " << Radius << endl;
#endif

		X1 = Cos (Alpha * ICur) * Radius,
		Y1 = Sin (Alpha * ICur) * Radius,
		Z1 = 0.0;

#ifdef TRACE_VALUES
		cout << "Point choisi : " << X1 << ", " << Y1 << endl;
#endif
		// MyTrsf * Point pour se retrouver sur le plan de la grille 3D
		P1.SetCoord (X1, Y1, Z1);
		CurPoint = V3d_View::TrsPoint (P1, MyTrsf);
		CurPoint.Coord (X2, Y2, Z2);
	} // IsCircular

	NewPoint.SetCoord (CurPoint.X (), CurPoint.Y (), CurPoint.Z ());

#ifdef TRACE_POINTS
	TextAttrib->SetColor (Quantity_Color (Quantity_NOC_RED));
	MyGridEchoGroup->SetPrimitivesAspect (TextAttrib);
	MyGridEchoGroup->Text ("Q", NewPoint, 1./81.);
	MyGridEchoGroup->Polyline (AVertex, NewPoint);
	Update ();
#endif
#ifdef IMP240100
	MyViewer->ShowGridEcho(this,NewPoint);
#else
#ifdef TRACE_ECHO
char *trace_echo = NULL;
	trace_echo = (char *)(getenv ("CSF_GraphicEcho"));
	if (trace_echo) {
		MyGridEchoGroup->Clear ();
		MyGridEchoGroup->Marker (NewPoint);
		Visual3d_TransientManager::BeginDraw
			(MyView, Standard_False, Standard_False);
		Visual3d_TransientManager::DrawStructure (MyGridEchoStructure);
		Visual3d_TransientManager::EndDraw (Standard_True);
	}
#endif
#endif	//IMP240100
	return NewPoint;
}

// Triedron methods : the Triedron is a non-zoomable object.

void V3d_View::ZBufferTriedronSetup(const Quantity_NameOfColor XColor,
				    const Quantity_NameOfColor YColor,
				    const Quantity_NameOfColor ZColor,
				    const Standard_Real        SizeRatio,
				    const Standard_Real        AxisDiametr,
				    const Standard_Integer     NbFacettes)
{
  MyView->ZBufferTriedronSetup(XColor, YColor, ZColor, SizeRatio, AxisDiametr, NbFacettes);
}

void V3d_View::TriedronDisplay (const Aspect_TypeOfTriedronPosition APosition,
 const Quantity_NameOfColor AColor, const Standard_Real AScale, const V3d_TypeOfVisualization AMode ) {

	MyView->TriedronDisplay (APosition, AColor, AScale, (AMode == V3d_WIREFRAME));
}

void V3d_View::TriedronErase ( ) {

	MyView->TriedronErase ( );
}

void V3d_View::TriedronEcho (const Aspect_TypeOfTriedronEcho AType ) {

	MyView->TriedronEcho (AType);
}

void V3d_View::GetGraduatedTrihedron(/* Names of axes */
                                     Standard_CString& xname, 
                                     Standard_CString& yname, 
                                     Standard_CString& zname,
                                     /* Draw names */
                                     Standard_Boolean& xdrawname, 
                                     Standard_Boolean& ydrawname, 
                                     Standard_Boolean& zdrawname,
                                     /* Draw values */
                                     Standard_Boolean& xdrawvalues, 
                                     Standard_Boolean& ydrawvalues, 
                                     Standard_Boolean& zdrawvalues,
                                     /* Draw grid */
                                     Standard_Boolean& drawgrid,
                                     /* Draw axes */
                                     Standard_Boolean& drawaxes,
                                     /* Number of splits along axes */
                                     Standard_Integer& nbx, 
                                     Standard_Integer& nby, 
                                     Standard_Integer& nbz,
                                     /* Offset for drawing values */
                                     Standard_Integer& xoffset, 
                                     Standard_Integer& yoffset, 
                                     Standard_Integer& zoffset,
                                     /* Offset for drawing names of axes */
                                     Standard_Integer& xaxisoffset, 
                                     Standard_Integer& yaxisoffset, 
                                     Standard_Integer& zaxisoffset,
                                     /* Draw tickmarks */
                                     Standard_Boolean& xdrawtickmarks, 
                                     Standard_Boolean& ydrawtickmarks, 
                                     Standard_Boolean& zdrawtickmarks,
                                     /* Length of tickmarks */
                                     Standard_Integer& xtickmarklength, 
                                     Standard_Integer& ytickmarklength, 
                                     Standard_Integer& ztickmarklength,
                                     /* Grid color */
                                     Quantity_Color& gridcolor,
                                     /* X name color */
                                     Quantity_Color& xnamecolor,
                                     /* Y name color */
                                     Quantity_Color& ynamecolor,
                                     /* Z name color */
                                     Quantity_Color& znamecolor,
                                     /* X color of axis and values */
                                     Quantity_Color& xcolor,
                                     /* Y color of axis and values */
                                     Quantity_Color& ycolor,
                                     /* Z color of axis and values */
                                     Quantity_Color& zcolor,
                                     /* Name of font for names of axes */
                                     Standard_CString& fontOfNames,
                                     /* Style of names of axes */
                                     OSD_FontAspect& styleOfNames,
                                     /* Size of names of axes */
                                     Standard_Integer& sizeOfNames,
                                     /* Name of font for values */
                                     Standard_CString& fontOfValues,
                                     /* Style of values */
                                     OSD_FontAspect& styleOfValues,
                                     /* Size of values */
                                     Standard_Integer& sizeOfValues) const
{
    MyView->GetGraduatedTrihedron(/* Names of axes */
                                  xname, 
                                  yname, 
                                  zname,
                                  /* Draw names */
                                  xdrawname, 
                                  ydrawname, 
                                  zdrawname,
                                  /* Draw values */
                                  xdrawvalues, 
                                  ydrawvalues, 
                                  zdrawvalues,
                                  /* Draw grid */
                                  drawgrid,
                                  /* Draw axes */
                                  drawaxes,
                                  /* Number of splits along axes */
                                  nbx, 
                                  nby, 
                                  nbz,
                                  /* Offset for drawing values */
                                  xoffset, 
                                  yoffset, 
                                  zoffset,
                                  /* Offset for drawing names of axes */
                                  xaxisoffset, 
                                  yaxisoffset, 
                                  zaxisoffset,
                                  /* Draw tickmarks */
                                  xdrawtickmarks, 
                                  ydrawtickmarks, 
                                  zdrawtickmarks,
                                  /* Length of tickmarks */
                                  xtickmarklength, 
                                  ytickmarklength, 
                                  ztickmarklength,
                                  /* Grid color */
                                  gridcolor,
                                  /* X name color */
                                  xnamecolor,
                                  /* Y name color */
                                  ynamecolor,
                                  /* Z name color */
                                  znamecolor,
                                  /* X color of axis and values */
                                  xcolor,
                                  /* Y color of axis and values */
                                  ycolor,
                                  /* Z color of axis and values */
                                  zcolor,
                                  /* Name of font for names of axes */
                                  fontOfNames,
                                  /* Style of names of axes */
                                  styleOfNames,
                                  /* Size of names of axes */
                                  sizeOfNames,
                                  /* Name of font for values */
                                  fontOfValues,
                                  /* Style of values */
                                  styleOfValues,
                                  /* Size of values */
                                  sizeOfValues);
}

void V3d_View::GraduatedTrihedronDisplay(/* Names of axes */
                                         const Standard_CString xname, 
                                         const Standard_CString yname, 
                                         const Standard_CString zname,
                                         /* Draw names */
                                         const Standard_Boolean xdrawname, 
                                         const Standard_Boolean ydrawname, 
                                         const Standard_Boolean zdrawname,
                                         /* Draw values */
                                         const Standard_Boolean xdrawvalues, 
                                         const Standard_Boolean ydrawvalues, 
                                         const Standard_Boolean zdrawvalues,
                                         /* Draw grid */
                                         const Standard_Boolean drawgrid,
                                         /* Draw axes */
                                         const Standard_Boolean drawaxes,
                                         /* Number of splits along axes */
                                         const Standard_Integer nbx, 
                                         const Standard_Integer nby, 
                                         const Standard_Integer nbz,
                                         /* Offset for drawing values */
                                         const Standard_Integer xoffset, 
                                         const Standard_Integer yoffset, 
                                         const Standard_Integer zoffset,
                                         /* Offset for drawing names of axes */
                                         const Standard_Integer xaxisoffset, 
                                         const Standard_Integer yaxisoffset, 
                                         const Standard_Integer zaxisoffset,
                                         /* Draw tickmarks */
                                         const Standard_Boolean xdrawtickmarks, 
                                         const Standard_Boolean ydrawtickmarks, 
                                         const Standard_Boolean zdrawtickmarks,
                                         /* Length of tickmarks */
                                         const Standard_Integer xtickmarklength, 
                                         const Standard_Integer ytickmarklength, 
                                         const Standard_Integer ztickmarklength,
                                         /* Grid color */
                                         const Quantity_Color& gridcolor,
                                         /* X name color */
                                         const Quantity_Color& xnamecolor,
                                         /* Y name color */
                                         const Quantity_Color& ynamecolor,
                                         /* Z name color */
                                         const Quantity_Color& znamecolor,
                                         /* X color of axis and values */
                                         const Quantity_Color& xcolor,
                                         /* Y color of axis and values */
                                         const Quantity_Color& ycolor,
                                         /* Z color of axis and values */
                                         const Quantity_Color& zcolor,
                                         /* Name of font for names of axes */
                                         const Standard_CString fontOfNames,
                                         /* Style of names of axes */
                                         const OSD_FontAspect styleOfNames,
                                         /* Size of names of axes */
                                         const Standard_Integer sizeOfNames,
                                         /* Name of font for values */
                                         const Standard_CString fontOfValues,
                                         /* Style of values */
                                         const OSD_FontAspect styleOfValues,
                                         /* Size of values */
                                         const Standard_Integer sizeOfValues)
{
    MyView->GraduatedTrihedronDisplay(/* Names of axes */
                                      xname, 
                                      yname, 
                                      zname,
                                      /* Draw names */
                                      xdrawname, 
                                      ydrawname, 
                                      zdrawname,
                                      /* Draw values */
                                      xdrawvalues, 
                                      ydrawvalues, 
                                      zdrawvalues,
                                      /* Draw grid */
                                      drawgrid,
                                      /* Draw axes */
                                      drawaxes,
                                      /* Number of splits along axes */
                                      nbx, 
                                      nby, 
                                      nbz,
                                      /* Offset for drawing values */
                                      xoffset, 
                                      yoffset, 
                                      zoffset,
                                      /* Offset for drawing names of axes */
                                      xaxisoffset, 
                                      yaxisoffset, 
                                      zaxisoffset,
                                      /* Draw tickmarks */
                                      xdrawtickmarks, 
                                      ydrawtickmarks, 
                                      zdrawtickmarks,
                                      /* Length of tickmarks */
                                      xtickmarklength, 
                                      ytickmarklength, 
                                      ztickmarklength,
                                      /* Grid color */
                                      gridcolor,
                                      /* X name color */
                                      xnamecolor,
                                      /* Y name color */
                                      ynamecolor,
                                      /* Z name color */
                                      znamecolor,
                                      /* X color of axis and values */
                                      xcolor,
                                      /* Y color of axis and values */
                                      ycolor,
                                      /* Z color of axis and values */
                                      zcolor,
                                      /* Name of font for names of axes */
                                      fontOfNames,
                                      /* Style of names of axes */
                                      styleOfNames,
                                      /* Size of names of axes */
                                      sizeOfNames,
                                      /* Name of font for values */
                                      fontOfValues,
                                      /* Style of values */
                                      styleOfValues,
                                      /* Size of values */
                                      sizeOfValues);
}

void V3d_View::GraduatedTrihedronErase()
{
    MyView->GraduatedTrihedronErase();
}

