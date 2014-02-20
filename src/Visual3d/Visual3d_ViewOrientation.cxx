// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//-Version	

//-Design	Declaration of variables specific to the orientation of views

//-Warning	The view orientation is defined by :
//		- the point of origin of the reference mark 
//		- the normal vector to the visualisation plane
//		- the vertical vector of the viewer

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Visual3d_ViewOrientation.ixx>

//-Aliases

//-Global data definitions

//	-- le point origine du repere
//	MyViewReferencePoint	:	Vertex;

//	-- le vecteur normal au plan de visualisation
//	MyViewPlaneNormal	:	Vector;

//	-- le vecteur vertical de l'observateur
//	MyViewUpVector		:	Vector;

//-Constructors

//-Destructors

//-Methods, in order

Visual3d_ViewOrientation::Visual3d_ViewOrientation ():
MyViewReferencePoint (0.0, 0.0, 0.0),
MyViewPlaneNormal (0.0, 0.0, 1.0),
MyViewUpVector (0.0, 1.0, 0.0),
MyScaleX(1.0),
MyScaleY(1.0),
MyScaleZ(1.0) {
}

Visual3d_ViewOrientation::Visual3d_ViewOrientation (const Graphic3d_Vertex& VRP, const Graphic3d_Vector& VPN, const Graphic3d_Vector& VUP):
MyViewReferencePoint (VRP),
MyViewPlaneNormal (VPN),
MyViewUpVector (VUP),
MyScaleX(1.0),
MyScaleY(1.0),
MyScaleZ(1.0) {

	if (Graphic3d_Vector::NormeOf (VPN) == 0.0)
		Visual3d_ViewOrientationDefinitionError::Raise
			("Bad value for ViewPlaneNormal");

	if (Graphic3d_Vector::NormeOf (VUP) == 0.0)
		Visual3d_ViewOrientationDefinitionError::Raise
			("Bad value for ViewUpVector");

	if (Graphic3d_Vector::IsParallel (VPN, VUP))
		Visual3d_ViewOrientationDefinitionError::Raise
			("ViewPlaneNormal and ViewUpVector are parallel");

}

Visual3d_ViewOrientation::Visual3d_ViewOrientation (const Graphic3d_Vertex& VRP, const Graphic3d_Vector& VPN, const Standard_Real /*Twist*/) {

	if (Graphic3d_Vector::NormeOf (VPN) == 0.0)
		Visual3d_ViewOrientationDefinitionError::Raise
			("Bad value for ViewPlaneNormal");

	cout << "\nVisual3d_ViewOrientation : Not Yet Implemented\n\n" << flush;

	MyViewReferencePoint	= VRP;
	MyViewPlaneNormal	= VPN;

}

Visual3d_ViewOrientation::Visual3d_ViewOrientation (const Graphic3d_Vertex& VRP, const Standard_Real /*Azim*/, const Standard_Real /*Inc*/, const Standard_Real /*Twist*/) {

	cout << "\nVisual3d_ViewOrientation : Not Yet Implemented\n\n" << flush;

	MyViewReferencePoint	= VRP;

}

void Visual3d_ViewOrientation::SetViewReferencePoint (const Graphic3d_Vertex& VRP) {

	MyViewReferencePoint	= VRP;

}

Graphic3d_Vertex Visual3d_ViewOrientation::ViewReferencePoint () const {

	return (MyViewReferencePoint);

}

void Visual3d_ViewOrientation::SetViewReferencePlane (const Graphic3d_Vector& VPN) {

	if (Graphic3d_Vector::NormeOf (VPN) == 0.0)
		Visual3d_ViewOrientationDefinitionError::Raise
			("Bad value for ViewPlaneNormal");

	MyViewPlaneNormal	= VPN;

}

Graphic3d_Vector Visual3d_ViewOrientation::ViewReferencePlane () const {

	return (MyViewPlaneNormal);

}

void Visual3d_ViewOrientation::SetViewReferenceUp (const Graphic3d_Vector& VUP) {

	if (Graphic3d_Vector::NormeOf (VUP) == 0.0)
		Visual3d_ViewOrientationDefinitionError::Raise
			("Bad value for ViewUpVector");

	MyViewUpVector		= VUP;

}

void Visual3d_ViewOrientation::SetAxialScale (const Standard_Real Sx, const Standard_Real Sy, const Standard_Real Sz) {
  if ( Sx <= 0. || Sy <= 0. || Sz <= 0. )
    Visual3d_ViewOrientationDefinitionError::Raise
      ("Bad value for ViewUpVector");
	MyScaleX = Sx;
	MyScaleY = Sy;
	MyScaleZ = Sz;
}

Graphic3d_Vector Visual3d_ViewOrientation::ViewReferenceUp () const {

	return (MyViewUpVector);

}

void Visual3d_ViewOrientation::Assign (const Visual3d_ViewOrientation& Other) {

Standard_Real X, Y, Z;

	(Other.ViewReferencePoint ()).Coord (X, Y, Z);
	MyViewReferencePoint.SetCoord (X, Y, Z);

	(Other.ViewReferencePlane ()).Coord (X, Y, Z);
	MyViewPlaneNormal.SetCoord (X, Y, Z);

	(Other.ViewReferenceUp ()).Coord (X, Y, Z);
	MyViewUpVector.SetCoord (X, Y, Z);

        if ( Other.IsCustomMatrix() ) {
          MyModelViewMatrix = new TColStd_HArray2OfReal( 0, 3, 0, 3);
	  for (Standard_Integer i = 0; i < 4; i++)
	    for (Standard_Integer j = 0; j < 4; j++)
	      MyModelViewMatrix->SetValue( i, j, Other.MyModelViewMatrix->Value(i, j) );
        }
        else
           MyModelViewMatrix.Nullify();
}

Standard_Real Visual3d_ViewOrientation::Twist () const {

cout << "\nVisual3d_ViewOrientation::Twist : Not Yet Implemented\n\n" << flush;

#ifdef OK
Standard_Real Xrp, Yrp, Zrp;
Standard_Real Xpn, Ypn, Zpn;
Standard_Real Xup, Yup, Zup;
Standard_Real a1, b1, c1, a2, b2, c2;
Standard_Real pvx, pvy, pvz;
Standard_Real an1, an2;
Standard_Real pvn, sca, angle;

	MyViewReferencePoint.Coord (Xrp, Yrp, Zrp) ;	
	MyViewPlaneNormal.Coord (Xpn, Ypn, Zpn) ;	
	MyViewUpVector.Coord (Xup, Yup, Zup) ;	

	Xrp -= Xpn ; Yrp -= Ypn ; Zrp -= Zpn ;
	Xup -= Xpn ; Yup -= Ypn ; Zup -= Zpn ;
				/* Compute Plane Normal EYE, AT, UP */
	a1 = Yrp*Zup - Yup*Zrp ;
	b1 = Zrp*Xup - Zup*Xrp ;
	c1 = Xrp*Yup - Xup*Yrp ;
				/* Compute Plane Normal EYE, AT, YAXIS */
	a2 = -Zrp ;
	b2 = 0. ;
	c2 = Xrp ;
				/* Compute Cross Vector from 2 last Normals */
	pvx = b1*c2 - c1*b2 ;
	pvy = c1*a2 - a1*c2 ;
	pvz = a1*b2 - b1*a2 ;
				/* Normalize vectors */
	an1 = a1*a1 + b1*b1 + c1*c1 ;
	an2 = a2*a2 + b2*b2 + c2*c2 ;
	pvn = pvx*pvx + pvy*pvy + pvz*pvz ;
				/* Compute Angle */
	if (angle > 1.) angle = 1. ;
	else if ( angle < -1. ) angle = -1. ;
	angle = asin (angle) / (M_PI / 180.0);
	sca = a1*a2 + b1*b2 + c1*c2 ;
	if (sca < 0.) angle = 180. - angle ;
	if ( (angle > 0.) && (angle < 180.) ) {
        sca = - (pvx*Xrp + pvy*Yrp + pvz*Zrp) ;
        if (sca > 0.) angle = 360. - angle ;
	}

	return (angle * M_PI / 180.0);
#else
	return (M_PI / 180.0);
#endif

}

void Visual3d_ViewOrientation::AxialScale(Standard_Real& Sx, Standard_Real& Sy, Standard_Real& Sz)const  {
  Sx = MyScaleX;
  Sy = MyScaleY;
  Sz = MyScaleZ;
}


void Visual3d_ViewOrientation::SetCustomModelViewMatrix(const Handle(TColStd_HArray2OfReal)& Mat)
{
  MyModelViewMatrix = Mat;
}

Standard_Boolean Visual3d_ViewOrientation::IsCustomMatrix() const
{
  return !MyModelViewMatrix.IsNull() 
      && MyModelViewMatrix->LowerRow() == 0
      && MyModelViewMatrix->LowerCol() == 0
      && MyModelViewMatrix->UpperRow() == 3
      && MyModelViewMatrix->UpperCol() == 3;
}
