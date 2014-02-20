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

// Modified	
//    22-12-98  : FMN ; Rename CSF_WALKTHROW en CSF_WALKTHROUGH
//    23-07-07  : NKV ; Define custom PROJECTION matrix for OpenGl context


//-Version	

//-Design	Declaration of variables specific to mapping of views

//-Warning	Mapping of a view is defined by :
//		- reference point of projection
//		- type of projection
//		- distance for the Back Plane
//		- distance for the Front Plane
//		- distance for the Projection Plane

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Visual3d_ViewMapping.ixx>
#include <Precision.hxx>

// Perspective
#include <OSD_Environment.hxx>
static OSD_Environment env_walkthrow; 

static Standard_Boolean Visual3dWalkthrow()
{
  static Standard_Integer isWalkthrow( -1 );
  if ( isWalkthrow < 0 ) {
    isWalkthrow = 1;
    OSD_Environment WalkThrow("CSF_WALKTHROUGH");
    if ( WalkThrow.Value().IsEmpty() )
      isWalkthrow = 0;
  }                       
  return ( isWalkthrow != 0 );
}


//-Aliases

//-Global data definitions

//	-- le point reference de projection
//	MyReferencePoint		:	Vertex;

//	-- le type de projection
//	MyProjectionType		:	TypeOfProjection;

//	-- la distance pour le Back Plane
//	MyBackPlaneDistance		:	Standard_Real;

//	-- la distance pour le Front Plane
//	MyFrontPlaneDistance		:	Standard_Real;

//	-- la distance pour le Projection Plane
//	MyViewPlaneDistance		:	Standard_Real;

//	-- les limites de la partie visible du plan
//	-- MyWindowLimits[0] = u du coin inferieur gauche.
//	-- MyWindowLimits[1] = v du coin inferieur gauche.
//	-- MyWindowLimits[2] = u du coin superieur droit.
//	-- MyWindowLimits[3] = v du coin superieur droit.
//	MyWindowLimits			:	Standard_Real[4];

//-Constructors

//-Destructors

//-Methods, in order

Visual3d_ViewMapping::Visual3d_ViewMapping ():
MyReferencePoint (0.5, 0.5, 2.0),
MyProjectionType (Visual3d_TOP_PARALLEL) {
     	if ( Visual3dWalkthrow() )
	{
	    MyBackPlaneDistance  = -1.0;
	    MyFrontPlaneDistance = 1.0;
	    MyViewPlaneDistance  = 0.0;
	}
	else
	{
	    MyBackPlaneDistance  = 0.0;
	    MyFrontPlaneDistance = 1.0;
	    MyViewPlaneDistance  = 1.0;
	}

	MyWindowLimits[0]	= 0.0;
	MyWindowLimits[1]	= 0.0;
	MyWindowLimits[2]	= 1.0;
	MyWindowLimits[3]	= 1.0;

}

Visual3d_ViewMapping::Visual3d_ViewMapping (const Visual3d_TypeOfProjection AType, const Graphic3d_Vertex& PRP, const Standard_Real BPD, const Standard_Real FPD, const Standard_Real VPD, const Standard_Real WUmin, const Standard_Real WVmin, const Standard_Real WUmax, const Standard_Real WVmax):
MyReferencePoint (PRP),
MyProjectionType (AType),
MyBackPlaneDistance (BPD),
MyFrontPlaneDistance (FPD),
MyViewPlaneDistance (VPD) {

	if ( (WUmin >= WUmax) || (WVmin >= WVmax) )
		Visual3d_ViewMappingDefinitionError::Raise
			("Invalid window; WUmin > WUmax or WVmin > WVmax");

	if (BPD > FPD)
		Visual3d_ViewMappingDefinitionError::Raise
			("The back plane is in front of the front plane");

/*
	A TESTER AVEC LE VRP ?
		Visual3d_ViewMappingDefinitionError::Raise
	("The projection reference point is between the front and back planes");

	if (PRP.Z () == VPD)
		Visual3d_ViewMappingDefinitionError::Raise
("The projection reference point cannot be positioned on the view plane");
*/

	MyWindowLimits[0]	= WUmin;
	MyWindowLimits[1]	= WVmin;
	MyWindowLimits[2]	= WUmax;
	MyWindowLimits[3]	= WVmax;

}

void Visual3d_ViewMapping::SetProjection (const Visual3d_TypeOfProjection AType) {

	MyProjectionType	= AType;

}

Visual3d_TypeOfProjection Visual3d_ViewMapping::Projection () const {

	return (MyProjectionType);

}

void Visual3d_ViewMapping::SetProjectionReferencePoint (const Graphic3d_Vertex& PRP) {

/*
Standard_Real VPD, BPD, FPD;

	VPD	= MyViewPlaneDistance;
	BPD	= MyBackPlaneDistance;
	FPD	= MyFrontPlaneDistance;

	A TESTER AVEC LE VRP ?
		Visual3d_ViewMappingDefinitionError::Raise
	("The projection reference point is between the front and back planes");

	if (PRP.Z () == VPD)
		Visual3d_ViewMappingDefinitionError::Raise
("The projection reference point cannot be positioned on the view plane");
*/

	MyReferencePoint	= PRP;

}

Graphic3d_Vertex Visual3d_ViewMapping::ProjectionReferencePoint () const {

	return (MyReferencePoint);

}

void Visual3d_ViewMapping::SetViewPlaneDistance (const Standard_Real VPD) {

/*
Standard_Real PRPZ, BPD, FPD;

	PRPZ	= MyReferencePoint.Z ();
	BPD	= MyBackPlaneDistance;
	FPD	= MyFrontPlaneDistance;

	A TESTER AVEC LE VRP ?
		Visual3d_ViewMappingDefinitionError::Raise
	("The projection reference point is between the front and back planes");

	if (PRPZ == VPD)
		Visual3d_ViewMappingDefinitionError::Raise
("The projection reference point cannot be positioned on the view plane");
*/

	MyViewPlaneDistance	= VPD;

}

Standard_Real Visual3d_ViewMapping::ViewPlaneDistance () const {

	return (MyViewPlaneDistance);

}

void Visual3d_ViewMapping::SetBackPlaneDistance (const Standard_Real BPD) {

/*
Standard_Real VPD, PRPZ, FPD;

	PRPZ	= MyReferencePoint.Z ();
	VPD	= MyViewPlaneDistance;
	FPD	= MyFrontPlaneDistance;

	if (BPD > FPD)
		Visual3d_ViewMappingDefinitionError::Raise
			("The back plane is in front of the front plane");

	A TESTER AVEC LE VRP ?
		Visual3d_ViewMappingDefinitionError::Raise
	("The projection reference point is between the front and back planes");
*/

	MyBackPlaneDistance	= BPD;
}

Standard_Real Visual3d_ViewMapping::BackPlaneDistance () const {

	return (MyBackPlaneDistance);
}

void Visual3d_ViewMapping::SetFrontPlaneDistance (const Standard_Real FPD) {

/*
Standard_Real VPD, BPD, PRPZ;

	PRPZ	= MyReferencePoint.Z ();
	VPD	= MyViewPlaneDistance;
	BPD	= MyBackPlaneDistance;

	if (BPD > FPD)
		Visual3d_ViewMappingDefinitionError::Raise
			("The back plane is in front of the front plane");

	A TESTER AVEC LE VRP ?
		Visual3d_ViewMappingDefinitionError::Raise
	("The projection reference point is between the front and back planes");
*/

	MyFrontPlaneDistance	= FPD;

}

Standard_Real Visual3d_ViewMapping::FrontPlaneDistance () const {

	return (MyFrontPlaneDistance);

}

void Visual3d_ViewMapping::SetWindowLimit (const Standard_Real Umin, const Standard_Real Vmin, const Standard_Real Umax, const Standard_Real Vmax) {

	if ( (Umin >= Umax) || (Vmin >= Vmax) )
		Visual3d_ViewMappingDefinitionError::Raise
			("Invalid window; WUmin > WUmax or WVmin > WVmax");

	if( (Umax - Umin) < Precision::Confusion() || (Vmax - Vmin) < Precision::Confusion())
		Visual3d_ViewMappingDefinitionError::Raise
			("Window is too small");

	MyWindowLimits[0]	= Umin;
	MyWindowLimits[1]	= Vmin;
	MyWindowLimits[2]	= Umax;
	MyWindowLimits[3]	= Vmax;

}

void Visual3d_ViewMapping::WindowLimit (Standard_Real& Umin, Standard_Real& Vmin, Standard_Real& Umax, Standard_Real& Vmax) const {

	Umin	= MyWindowLimits[0];
	Vmin	= MyWindowLimits[1];
	Umax	= MyWindowLimits[2];
	Vmax	= MyWindowLimits[3];

}

void Visual3d_ViewMapping::SetCustomProjectionMatrix(const Handle(TColStd_HArray2OfReal)& Mat)
{
  MyProjectionMatrix = Mat;
}

Standard_Boolean Visual3d_ViewMapping::IsCustomMatrix() const
{
  return !MyProjectionMatrix.IsNull() 
      && MyProjectionMatrix->LowerRow() == 0
      && MyProjectionMatrix->LowerCol() == 0
      && MyProjectionMatrix->UpperRow() == 3
      && MyProjectionMatrix->UpperCol() == 3;
}


void Visual3d_ViewMapping::Assign (const Visual3d_ViewMapping& Other) {

Standard_Real X, Y, Z;

	(Other.ProjectionReferencePoint ()).Coord (X, Y, Z);
	MyReferencePoint.SetCoord (X, Y, Z);

	MyProjectionType	= Other.Projection ();

	MyBackPlaneDistance	= Other.BackPlaneDistance ();

	MyFrontPlaneDistance	= Other.FrontPlaneDistance ();

	MyViewPlaneDistance	= Other.ViewPlaneDistance ();

	Other.WindowLimit (MyWindowLimits[0], MyWindowLimits[1],
			    MyWindowLimits[2], MyWindowLimits[3]);

        if ( Other.IsCustomMatrix() ) {
          MyProjectionMatrix = new TColStd_HArray2OfReal( 0, 3, 0, 3 );
	  for (Standard_Integer i = 0; i < 4; i++)
	    for (Standard_Integer j = 0; j < 4; j++)
	      MyProjectionMatrix->SetValue( i, j, Other.MyProjectionMatrix->Value(i, j) );
	}
        else
	  MyProjectionMatrix.Nullify();
}
