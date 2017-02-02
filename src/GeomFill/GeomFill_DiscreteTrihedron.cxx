// Created on: 2013-02-05
// Created by: Julia GERASIMOVA
// Copyright (c) 2001-2013 OPEN CASCADE SAS
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


#include <Adaptor3d_HCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomFill_DiscreteTrihedron.hxx>
#include <GeomFill_Frenet.hxx>
#include <GeomFill_HSequenceOfAx2.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HSequenceOfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_DiscreteTrihedron,GeomFill_TrihedronLaw)

static const Standard_Real TolConf = Precision::Confusion();

//=======================================================================
//function : GeomFill_DiscreteTrihedron
//purpose  : Constructor
//=======================================================================

GeomFill_DiscreteTrihedron::GeomFill_DiscreteTrihedron() :
    myUseFrenet(Standard_False)
{
  myFrenet = new GeomFill_Frenet();
  myKnots      = new TColStd_HSequenceOfReal();
  myTrihedrons = new GeomFill_HSequenceOfAx2();
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(GeomFill_TrihedronLaw) GeomFill_DiscreteTrihedron::Copy() const
{
  Handle(GeomFill_DiscreteTrihedron) copy = new (GeomFill_DiscreteTrihedron)();
  if (!myCurve.IsNull()) copy->SetCurve(myCurve);
  return copy;
}

//=======================================================================
//function : SetCurve
//purpose  : 
//=======================================================================

void GeomFill_DiscreteTrihedron::SetCurve(const Handle(Adaptor3d_HCurve)& C) 
{
  GeomFill_TrihedronLaw::SetCurve(C);
  if (! C.IsNull()) {
    GeomAbs_CurveType type;
    type = C->GetType();
    switch  (type) {
    case GeomAbs_Circle:
    case GeomAbs_Ellipse:
    case GeomAbs_Hyperbola:
    case GeomAbs_Parabola:
    case GeomAbs_Line:
      {
	// No probleme
	myUseFrenet = Standard_True;
        myFrenet->SetCurve(C);
        break;
      }
    default :
      {
        myUseFrenet = Standard_False;
        // We have to fill <myKnots> and <myTrihedrons>
        Init();
        break;
      }
    }
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomFill_DiscreteTrihedron::Init()
{
  Standard_Integer NbIntervals = myTrimmed->NbIntervals(GeomAbs_CN);
  TColStd_Array1OfReal Knots(1, NbIntervals+1);
  myTrimmed->Intervals(Knots, GeomAbs_CN);

  //Standard_Real Tol = Precision::Confusion();
  Standard_Integer NbSamples = 10;

  Standard_Integer i, j;
  for (i = 1; i <= NbIntervals; i++)
  {
    Standard_Real delta = (Knots(i+1) - Knots(i))/NbSamples;
    for (j = 0; j < NbSamples; j++)
    {
      Standard_Real Param = Knots(i) + j*delta;
      myKnots->Append(Param);
    }
  }
  myKnots->Append(Knots(NbIntervals+1));
  
  
  gp_Pnt Origin(0.,0.,0.), Pnt, SubPnt;
  gp_Vec Tangent;
  gp_Dir TangDir;
  Standard_Real norm;
  for (i = 1; i <= myKnots->Length(); i++)
  {
    Standard_Real Param = myKnots->Value(i);
    myTrimmed->D1(Param, Pnt, Tangent);
    norm = Tangent.Magnitude();
    if (norm < TolConf)
    {
      Standard_Real subdelta = (myKnots->Value(i+1) - myKnots->Value(i))/NbSamples;
      if (subdelta < Precision::PConfusion())
        subdelta = myKnots->Value(i+1) - myKnots->Value(i);
      SubPnt = myTrimmed->Value(Param + subdelta);
      Tangent.SetXYZ(SubPnt.XYZ() - Pnt.XYZ());
    }
    //Tangent.Normalize();
    TangDir = Tangent; //normalize;
    Tangent = TangDir;
    if (i == 1) //first point
    {
      gp_Ax2 FirstAxis(Origin, TangDir);
      myTrihedrons->Append(FirstAxis);
    }
    else
    {
      gp_Ax2 LastAxis = myTrihedrons->Value(myTrihedrons->Length());
      gp_Vec LastTangent = LastAxis.Direction();
      gp_Vec AxisOfRotation = LastTangent ^ Tangent;
      if (AxisOfRotation.Magnitude() <= gp::Resolution()) //tangents are equal or opposite
      {
        Standard_Real ScalarProduct = LastTangent * Tangent;
        if (ScalarProduct > 0.) //tangents are equal
          myTrihedrons->Append(LastAxis);
        else //tangents are opposite
        {
          Standard_Real NewParam = (myKnots->Value(i-1) + myKnots->Value(i))/2.;
          if (NewParam - myKnots->Value(i-1) < gp::Resolution())
            throw Standard_ConstructionError("GeomFill_DiscreteTrihedron : impassable singularities on path curve");
          myKnots->InsertBefore(i, NewParam);
          i--;
        }
      }
      else //good value of angle
      {
        Standard_Real theAngle = LastTangent.AngleWithRef(Tangent, AxisOfRotation);
        gp_Ax1 theAxisOfRotation(Origin, AxisOfRotation);
        gp_Ax2 NewAxis = LastAxis.Rotated(theAxisOfRotation, theAngle);
        NewAxis.SetDirection(TangDir); //to prevent accumulation of floating computations error
        myTrihedrons->Append(NewAxis);
      }
    }
  }
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_DiscreteTrihedron::D0(const Standard_Real Param,
                                                gp_Vec& Tangent,
                                                gp_Vec& Normal,
                                                gp_Vec& BiNormal)
{
  if (myUseFrenet)
  {
    myFrenet->D0(Param, Tangent, Normal, BiNormal);
  }
  else
  {
    //Locate <Param> in the sequence <myKnots>
    Standard_Integer Index = -1;
    Standard_Real TolPar = Precision::PConfusion();
    //Standard_Real TolConf = Precision::Confusion();
    Standard_Integer NbSamples = 10;
    gp_Pnt Origin(0.,0.,0.);
    
    Standard_Integer i;
    //gp_Ax2 PrevAxis;
    //Standard_Real PrevParam;

    Standard_Integer I1, I2;
    I1 = 1;
    I2 = myKnots->Length();
    for (;;)
    {
      i = (I1 + I2)/2;
      if (Param <= myKnots->Value(i))
        I2 = i;
      else
        I1 = i;
      if (I2 - I1 <= 1)
        break;
    }
    Index = I1;
    if (Abs(Param - myKnots->Value(I2)) < TolPar)
      Index = I2;

    Standard_Real PrevParam = myKnots->Value(Index);
    gp_Ax2        PrevAxis  = myTrihedrons->Value(Index);
    gp_Ax2 theAxis;
    if (Abs(Param - PrevParam) < TolPar)
      theAxis = PrevAxis;
    else //<Param> is between knots
    {
      myTrimmed->D1(Param, myPoint, Tangent);
      Standard_Real norm = Tangent.Magnitude();
      if (norm < TolConf)
      {
        Standard_Real subdelta = (myKnots->Value(Index+1) - Param)/NbSamples;
        if (subdelta < Precision::PConfusion())
          subdelta = myKnots->Value(Index+1) - Param;
        gp_Pnt SubPnt = myTrimmed->Value(Param + subdelta);
        Tangent.SetXYZ(SubPnt.XYZ() - myPoint.XYZ());
      }
      //Tangent.Normalize();
      gp_Dir TangDir(Tangent); //normalize;
      Tangent = TangDir;
      gp_Vec PrevTangent = PrevAxis.Direction();
      gp_Vec AxisOfRotation = PrevTangent ^ Tangent;
      if (AxisOfRotation.Magnitude() <= gp::Resolution()) //tangents are equal
      {
        //we assume that tangents can not be opposite
        theAxis = PrevAxis;
      }
      else //good value of angle
      {
        Standard_Real theAngle = PrevTangent.AngleWithRef(Tangent, AxisOfRotation);
        gp_Ax1 theAxisOfRotation(Origin, AxisOfRotation);
        theAxis = PrevAxis.Rotated(theAxisOfRotation, theAngle);
      }
      theAxis.SetDirection(TangDir); //to prevent accumulation of floating computations error
    } //end of else (Param is between knots)

    Tangent  = theAxis.Direction();
    Normal   = theAxis.XDirection();
    BiNormal = theAxis.YDirection();
  }
  return Standard_True;
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_DiscreteTrihedron::D1(const Standard_Real Param,
                                                gp_Vec& Tangent,
                                                gp_Vec& DTangent,
                                                gp_Vec& Normal,
                                                gp_Vec& DNormal,
                                                gp_Vec& BiNormal,
                                                gp_Vec& DBiNormal) 
{
  if (myUseFrenet)
  {
    myFrenet->D1(Param, Tangent, DTangent, Normal, DNormal, BiNormal, DBiNormal);
  }
  else
  {
    D0(Param, Tangent, Normal, BiNormal);

    DTangent.SetCoord(0.,0.,0.);
    DNormal.SetCoord(0.,0.,0.);
    DBiNormal.SetCoord(0.,0.,0.);
  }
  return Standard_True;
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_DiscreteTrihedron::D2(const Standard_Real Param,
                                                gp_Vec& Tangent,
                                                gp_Vec& DTangent,
                                                gp_Vec& D2Tangent,
                                                gp_Vec& Normal,
                                                gp_Vec& DNormal,
                                                gp_Vec& D2Normal,
                                                gp_Vec& BiNormal,
                                                gp_Vec& DBiNormal,
                                                gp_Vec& D2BiNormal) 
{
  if (myUseFrenet)
  {
    myFrenet->D2(Param, Tangent, DTangent, D2Tangent,
                 Normal, DNormal, D2Normal,
                 BiNormal, DBiNormal, D2BiNormal);
  }
  else
  {
    D0(Param, Tangent, Normal, BiNormal);
    
    DTangent.SetCoord(0.,0.,0.);
    DNormal.SetCoord(0.,0.,0.);
    DBiNormal.SetCoord(0.,0.,0.);
    D2Tangent.SetCoord(0.,0.,0.);
    D2Normal.SetCoord(0.,0.,0.);
    D2BiNormal.SetCoord(0.,0.,0.);
  }
  return Standard_True;
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer GeomFill_DiscreteTrihedron::NbIntervals(const GeomAbs_Shape) const
{
  return (myTrimmed->NbIntervals(GeomAbs_CN));
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void GeomFill_DiscreteTrihedron::Intervals(TColStd_Array1OfReal& T,
                                           const GeomAbs_Shape) const
{
  myTrimmed->Intervals(T, GeomAbs_CN);
}

 void GeomFill_DiscreteTrihedron::GetAverageLaw(gp_Vec& ATangent,
                                     gp_Vec& ANormal,
                                     gp_Vec& ABiNormal) 
{
  Standard_Integer Num = 20; //order of digitalization
  gp_Vec T, N, BN;
  ATangent = gp_Vec(0, 0, 0);
  ANormal = gp_Vec(0, 0, 0);
  ABiNormal = gp_Vec(0, 0, 0);
  Standard_Real Step = (myTrimmed->LastParameter() - 
                        myTrimmed->FirstParameter()) / Num;
  Standard_Real Param;
  for (Standard_Integer i = 0; i <= Num; i++) {
    Param = myTrimmed->FirstParameter() + i*Step;
    if (Param > myTrimmed->LastParameter()) Param = myTrimmed->LastParameter();
    D0(Param, T, N, BN);
    ATangent += T;
    ANormal += N;
    ABiNormal += BN;
  }
  ATangent /= Num + 1;
  ANormal /= Num + 1;

  ATangent.Normalize();
  ABiNormal = ATangent.Crossed(ANormal).Normalized();
  ANormal = ABiNormal.Crossed(ATangent);
}

//=======================================================================
//function : IsConstant
//purpose  : 
//=======================================================================

 Standard_Boolean GeomFill_DiscreteTrihedron::IsConstant() const
{
  return (myCurve->GetType() == GeomAbs_Line);
}

//=======================================================================
//function : IsOnlyBy3dCurve
//purpose  : 
//=======================================================================

 Standard_Boolean GeomFill_DiscreteTrihedron::IsOnlyBy3dCurve() const
{
  return Standard_True;
}
