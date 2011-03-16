
// File		Visual3d_ClipPlane.cxx
// Created	Mars 1992
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques aux plans de
//		clipping modele

//-Warning	Un plan de clipping est defini par son equation ou
//		par un point et sa normale
//		Equation : A*X + B*Y + C*Z + D = 0

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Visual3d_ClipPlane.ixx>

#include <Graphic3d_GraphicDriver.hxx>

//-Aliases

//-Global data definitions

//-Constructors

//-Destructors

//-Methods, in order

Visual3d_ClipPlane::Visual3d_ClipPlane (const Standard_Real ACoefA, const Standard_Real ACoefB, const Standard_Real ACoefC, const Standard_Real ACoefD) {

	MyCPlane.WsId	= -1;
	MyCPlane.ViewId	= 0; /* not used */

	MyCPlane.CoefA	= float (ACoefA);
	MyCPlane.CoefB	= float (ACoefB);
	MyCPlane.CoefC	= float (ACoefC);
	MyCPlane.CoefD	= float (ACoefD);

        MyCPlane.PlaneId	=
		Graphic3d_GraphicDriver::Plane (MyCPlane, Standard_False);

}

void Visual3d_ClipPlane::SetPlane (const Standard_Real ACoefA, const Standard_Real ACoefB, const Standard_Real ACoefC, const Standard_Real ACoefD) {

	MyCPlane.CoefA	= float (ACoefA);
	MyCPlane.CoefB	= float (ACoefB);
	MyCPlane.CoefC	= float (ACoefC);
	MyCPlane.CoefD	= float (ACoefD);

        MyCPlane.PlaneId	=
		Graphic3d_GraphicDriver::Plane (MyCPlane, Standard_True);

}

void Visual3d_ClipPlane::Plane (Standard_Real& ACoefA, Standard_Real& ACoefB, Standard_Real& ACoefC, Standard_Real& ACoefD) const {

	ACoefA	= Standard_Real (MyCPlane.CoefA);
	ACoefB	= Standard_Real (MyCPlane.CoefB);
	ACoefC	= Standard_Real (MyCPlane.CoefC);
	ACoefD	= Standard_Real (MyCPlane.CoefD);

}

Standard_Integer Visual3d_ClipPlane::Limit () {

#ifdef GER61454
        return 4;       // Obsolete method, calls instead myView->PlaneLimit() 
#else
        // Old method, replaced by GraphicDriver::InquirePlaneLimit ()
        return 0;
#endif
}

Standard_Integer Visual3d_ClipPlane::Identification () const {

	return (Standard_Integer (MyCPlane.PlaneId));

}
