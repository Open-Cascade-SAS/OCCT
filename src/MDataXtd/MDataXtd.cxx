// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//      	------------
// modified     Sergey Zaritchny
// Version:	0.0
//Version	Date		Purpose
//		0.0	Aug  4 1997	Creation



#include <MDataXtd.ixx>
#include <MDF_ASDriverTable.hxx>
#include <MDF_ARDriverTable.hxx>
#include <CDM_MessageDriver.hxx>
// Storage
#include <MDataXtd_ShapeStorageDriver.hxx>
#include <MDataXtd_PointStorageDriver.hxx>
#include <MDataXtd_AxisStorageDriver.hxx>
#include <MDataXtd_PlaneStorageDriver.hxx>
#include <MDataXtd_GeometryStorageDriver.hxx>
#include <MDataXtd_ConstraintStorageDriver.hxx>
#include <MDataXtd_PlacementStorageDriver.hxx>
#include <MDataXtd_PatternStdStorageDriver.hxx>

// Retrieval
#include <MDataXtd_ShapeRetrievalDriver.hxx>
#include <MDataXtd_PointRetrievalDriver.hxx>
#include <MDataXtd_AxisRetrievalDriver.hxx>
#include <MDataXtd_PlaneRetrievalDriver.hxx>
#include <MDataXtd_GeometryRetrievalDriver.hxx>
#include <MDataXtd_ConstraintRetrievalDriver.hxx>
#include <MDataXtd_PlacementRetrievalDriver.hxx>
#include <MDataXtd_PatternStdRetrievalDriver.hxx>

// enums
#include <TDataStd_RealEnum.hxx>
#include <TDataXtd_ConstraintEnum.hxx>
#include <TDataXtd_GeometryEnum.hxx>
#include <Standard_DomainError.hxx>
#include <MgtGeom.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Point.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Direction.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <PGeom_Curve.hxx>
#include <PGeom_Surface.hxx>
#include <PGeom_Point.hxx>
#include <PGeom_Axis1Placement.hxx>
#include <PGeom_Axis2Placement.hxx>
#include <PGeom_Direction.hxx>
#include <PGeom_VectorWithMagnitude.hxx>
#include <Standard_NullObject.hxx>

//=======================================================================
//function : AddStorageDriver
//purpose  : 
//=======================================================================

void MDataXtd::AddStorageDrivers
(const Handle(MDF_ASDriverHSequence)& aDriverSeq, const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MDataXtd_ShapeStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataXtd_PointStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataXtd_AxisStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataXtd_PlaneStorageDriver(theMsgDriver));   
  aDriverSeq->Append(new MDataXtd_GeometryStorageDriver(theMsgDriver));  
  aDriverSeq->Append(new MDataXtd_ConstraintStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataXtd_PlacementStorageDriver(theMsgDriver));
  aDriverSeq->Append(new MDataXtd_PatternStdStorageDriver(theMsgDriver));
}


//=======================================================================
//function : AddRetrievalDriver
//purpose  : 
//=======================================================================

void MDataXtd::AddRetrievalDrivers
(const Handle(MDF_ARDriverHSequence)& aDriverSeq,const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MDataXtd_ShapeRetrievalDriver(theMsgDriver)); 
  aDriverSeq->Append(new MDataXtd_PointRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataXtd_AxisRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataXtd_PlaneRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataXtd_GeometryRetrievalDriver(theMsgDriver)); 
  aDriverSeq->Append(new MDataXtd_ConstraintRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataXtd_PlacementRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MDataXtd_PatternStdRetrievalDriver(theMsgDriver));
}

//=======================================================================
//function : ConstraintTypeToInteger
//purpose  : 
//=======================================================================

Standard_Integer MDataXtd::ConstraintTypeToInteger(const TDataXtd_ConstraintEnum e) 
{
  switch (e) {
    // planar constraints
  case TDataXtd_RADIUS         : return  0;
  case TDataXtd_DIAMETER       : return  1;
  case TDataXtd_MINOR_RADIUS   : return  2;
  case TDataXtd_MAJOR_RADIUS   : return  3;
  case TDataXtd_TANGENT        : return  4;
  case TDataXtd_PARALLEL       : return  5;
  case TDataXtd_PERPENDICULAR  : return  6;
  case TDataXtd_CONCENTRIC     : return  7;
  case TDataXtd_COINCIDENT     : return  8;
  case TDataXtd_DISTANCE       : return  9;
  case TDataXtd_ANGLE          : return  10;
  case TDataXtd_EQUAL_RADIUS   : return  11;
  case TDataXtd_SYMMETRY       : return  12;
  case TDataXtd_MIDPOINT       : return  13;   
  case TDataXtd_EQUAL_DISTANCE : return  14;
  case TDataXtd_FIX            : return  15;
  case TDataXtd_RIGID          : return  16;
    // placement constraints
  case TDataXtd_FROM           : return  17; 
  case TDataXtd_AXIS           : return  18; 
  case TDataXtd_MATE           : return  19;
  case TDataXtd_ALIGN_FACES    : return  20;
  case TDataXtd_ALIGN_AXES     : return  21;
  case TDataXtd_AXES_ANGLE     : return  22;
  case TDataXtd_FACES_ANGLE    : return  23;
  case TDataXtd_ROUND          : return  24;
  case TDataXtd_OFFSET         : return  25;
  
  default:
    Standard_DomainError::Raise("TDataXtd_ConstraintEnum; enum term unknown");
  }
  return 0;
}

//=======================================================================
//function : IntegerToConstraintType
//purpose  : 
//=======================================================================

TDataXtd_ConstraintEnum MDataXtd::IntegerToConstraintType(const Standard_Integer i) 
{
  switch(i)
    {
      // planar constraints
    case  0 : return TDataXtd_RADIUS;
    case  1 : return TDataXtd_DIAMETER;
    case  2 : return TDataXtd_MINOR_RADIUS; 
    case  3 : return TDataXtd_MAJOR_RADIUS;
    case  4 : return TDataXtd_TANGENT;
    case  5 : return TDataXtd_PARALLEL;
    case  6 : return TDataXtd_PERPENDICULAR;
    case  7 : return TDataXtd_CONCENTRIC;
    case  8 : return TDataXtd_COINCIDENT;
    case  9 : return TDataXtd_DISTANCE;
    case 10 : return TDataXtd_ANGLE; 
    case 11 : return TDataXtd_EQUAL_RADIUS;
    case 12 : return TDataXtd_SYMMETRY;
    case 13 : return TDataXtd_MIDPOINT;       
    case 14 : return TDataXtd_EQUAL_DISTANCE;
    case 15 : return TDataXtd_FIX;   
    case 16 : return TDataXtd_RIGID; 
      // placement constraints
    case 17 : return TDataXtd_FROM; 
    case 18 : return TDataXtd_AXIS; 
    case 19 : return TDataXtd_MATE;   
    case 20 : return TDataXtd_ALIGN_FACES;
    case 21 : return TDataXtd_ALIGN_AXES;
    case 22 : return TDataXtd_AXES_ANGLE;
    case 23 : return TDataXtd_FACES_ANGLE;
    case 24 : return TDataXtd_ROUND;  
    case 25 : return TDataXtd_OFFSET; 
      default :
	Standard_DomainError::Raise("TDataXtd_ConstraintEnum; enum term unknown ");
    }
  return TDataXtd_TANGENT;
}

//=======================================================================
//function : GeometryTypeToInteger
//purpose  : 
//=======================================================================

Standard_Integer MDataXtd::GeometryTypeToInteger(const TDataXtd_GeometryEnum e) 
{
  switch (e) {
  case TDataXtd_ANY_GEOM : return  0;
  case TDataXtd_POINT    : return  1;
  case TDataXtd_LINE     : return  2;
  case TDataXtd_CIRCLE   : return  3;
  case TDataXtd_ELLIPSE  : return  4;
    
  default:
    Standard_DomainError::Raise("TDataXtd_GeometryEnum; enum term unknown");
  }
  return 0;
}

//=======================================================================
//function : IntegerToGeometryType
//purpose  : 
//=======================================================================

TDataXtd_GeometryEnum MDataXtd::IntegerToGeometryType(const Standard_Integer i) 
{
  switch(i)
    {
    case  0 : return TDataXtd_ANY_GEOM;
    case  1 : return TDataXtd_POINT;
    case  2 : return TDataXtd_LINE;
    case  3 : return TDataXtd_CIRCLE;
    case  4 : return TDataXtd_ELLIPSE;

      default :
	Standard_DomainError::Raise("TDataXtd_GeometryEnum; enum term unknown ");
    }
  return TDataXtd_ANY_GEOM;
}

Handle(Geom_Geometry) MDataXtd::Translate (const Handle(PGeom_Geometry)& Geometry) {
  Handle(Standard_Type) GeometryType = Geometry->DynamicType ();

  if (GeometryType == STANDARD_TYPE(PGeom_Surface)) {
    Handle(PGeom_Surface)& Surface = (Handle(PGeom_Surface)&) Geometry;
    return MgtGeom::Translate (Surface);
  }
  else if (GeometryType == STANDARD_TYPE(PGeom_Curve)) {
    Handle(PGeom_Curve)& Curve = (Handle(PGeom_Curve)&) Geometry;
    return MgtGeom::Translate (Curve);
  }
  else if (GeometryType == STANDARD_TYPE(PGeom_Point)) {
    Handle(PGeom_Point)& Point = (Handle(PGeom_Point)&) Geometry;
    return MgtGeom::Translate (Point);
  }
  else if (GeometryType == STANDARD_TYPE(PGeom_Axis1Placement)) {
    Handle(PGeom_Axis1Placement)& Axis1 = (Handle(PGeom_Axis1Placement)&) Geometry;
    return MgtGeom::Translate (Axis1);
  }
  else if (GeometryType == STANDARD_TYPE(Geom_Axis2Placement)) {
    Handle(PGeom_Axis2Placement)& Axis2 = (Handle(PGeom_Axis2Placement)&) Geometry;
    return MgtGeom::Translate (Axis2);
  }
  else if (GeometryType == STANDARD_TYPE(PGeom_Direction)) {
    Handle(PGeom_Direction)& Dir = (Handle(PGeom_Direction)&) Geometry;
    return MgtGeom::Translate (Dir);
  }
  else if (GeometryType == STANDARD_TYPE(PGeom_VectorWithMagnitude)) {
    Handle(PGeom_VectorWithMagnitude)& Vec = (Handle(PGeom_VectorWithMagnitude)&) Geometry;
    return MgtGeom::Translate (Vec);
  }
  else {
    Standard_NullObject::Raise("No mapping for the current Persistent Geometry");
  }

  Handle(Geom_Surface) dummy;
  return dummy;
}


Handle(PGeom_Geometry) MDataXtd::Translate (const Handle(Geom_Geometry)& Geometry) {

  Handle(Standard_Type) GeometryType = Geometry->DynamicType ();

  if (GeometryType == STANDARD_TYPE(Geom_Surface)) {
    Handle(Geom_Surface)& Surface = (Handle(Geom_Surface)&) Geometry;
    return MgtGeom::Translate (Surface);
  }
  else if (GeometryType == STANDARD_TYPE(Geom_Curve)) {
    Handle(Geom_Curve)& Curve = (Handle(Geom_Curve)&) Geometry;
    return MgtGeom::Translate (Curve);
  }
  else if (GeometryType == STANDARD_TYPE(Geom_Point)) {
    Handle(Geom_Point)& Point = (Handle(Geom_Point)&) Geometry;
    return MgtGeom::Translate (Point);
  }
  else if (GeometryType == STANDARD_TYPE(Geom_Axis1Placement)) {
    Handle(Geom_Axis1Placement)& Axis1 = (Handle(Geom_Axis1Placement)&) Geometry;
    return MgtGeom::Translate (Axis1);
  }
  else if (GeometryType == STANDARD_TYPE(Geom_Axis2Placement)) {
    Handle(Geom_Axis2Placement)& Axis2 = (Handle(Geom_Axis2Placement)&) Geometry;
    return MgtGeom::Translate (Axis2);
  }
  else if (GeometryType == STANDARD_TYPE(Geom_Direction)) {
    Handle(Geom_Direction)& Dir = (Handle(Geom_Direction)&) Geometry;
    return MgtGeom::Translate (Dir);
  }
  else if (GeometryType == STANDARD_TYPE(Geom_VectorWithMagnitude)) {
    Handle(Geom_VectorWithMagnitude)& Vec = (Handle(Geom_VectorWithMagnitude)&) Geometry;
    return MgtGeom::Translate (Vec);
  }
  else {
    Standard_NullObject::Raise("No mapping for the current Transient Geometry");
  }

  Handle(PGeom_Surface) dummy;
  return dummy;
}



