// Created on: 1994-08-18
// Created by: Laurent PAINNOT
// Copyright (c) 1994-1999 Matra Datavision
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

// 08-Aug-95 : xab : interpolation uses BSplCLib::Interpolate

#include <BSplCLib.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <gp_Vec.hxx>
#include <PLib.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

static bool CheckPoints(const NCollection_Array1<gp_Pnt>& PointArray,
                                    const double       Tolerance)
{
  int ii;
  double    tolerance_squared = Tolerance * Tolerance, distance_squared;
  bool result            = true;
  for (ii = PointArray.Lower(); result && ii < PointArray.Upper(); ii++)
  {
    distance_squared = PointArray.Value(ii).SquareDistance(PointArray.Value(ii + 1));
    result           = (distance_squared >= tolerance_squared);
  }
  return result;
}

//=================================================================================================

static bool CheckTangents(const NCollection_Array1<gp_Vec>&      Tangents,
                                      const NCollection_Array1<bool>& TangentFlags,
                                      const double            Tolerance)
{
  int ii, index;
  double    tolerance_squared = Tolerance * Tolerance, distance_squared;
  bool result            = true;
  index                              = TangentFlags.Lower();
  for (ii = Tangents.Lower(); result && ii <= Tangents.Upper(); ii++)
  {
    if (TangentFlags.Value(index))
    {
      distance_squared = Tangents.Value(ii).SquareMagnitude();
      result           = (distance_squared >= tolerance_squared);
    }
    index += 1;
  }
  return result;
}

//=================================================================================================

static bool CheckParameters(const NCollection_Array1<double>& Parameters)
{
  int ii;
  double    distance;
  bool result = true;
  for (ii = Parameters.Lower(); result && ii < Parameters.Upper(); ii++)
  {
    distance = Parameters.Value(ii + 1) - Parameters.Value(ii);
    result   = (distance >= RealSmall());
  }
  return result;
}

//=================================================================================================

static void BuildParameters(const bool         PeriodicFlag,
                            const NCollection_Array1<gp_Pnt>&      PointsArray,
                            occ::handle<NCollection_HArray1<double>>& ParametersPtr)
{
  int ii, index;
  double    distance;
  int num_parameters = PointsArray.Length();
  if (PeriodicFlag)
  {
    num_parameters += 1;
  }
  ParametersPtr = new NCollection_HArray1<double>(1, num_parameters);
  ParametersPtr->SetValue(1, 0.0e0);
  index = 2;
  for (ii = PointsArray.Lower(); ii < PointsArray.Upper(); ii++)
  {
    distance = PointsArray.Value(ii).Distance(PointsArray.Value(ii + 1));
    ParametersPtr->SetValue(index, ParametersPtr->Value(ii) + distance);
    index += 1;
  }
  if (PeriodicFlag)
  {
    distance =
      PointsArray.Value(PointsArray.Upper()).Distance(PointsArray.Value(PointsArray.Lower()));
    ParametersPtr->SetValue(index, ParametersPtr->Value(ii) + distance);
  }
}

//=================================================================================================

static void BuildPeriodicTangent(const NCollection_Array1<gp_Pnt>&   PointsArray,
                                 NCollection_Array1<gp_Vec>&         TangentsArray,
                                 NCollection_Array1<bool>&    TangentFlags,
                                 const NCollection_Array1<double>& ParametersArray)
{
  int ii, degree;
  double *  point_array, *parameter_array, eval_result[2][3];

  gp_Vec a_vector;

  if (PointsArray.Length() < 3)
  {
    throw Standard_ConstructionError();
  }

  if (!TangentFlags.Value(1))
  {
    degree = 3;
    if (PointsArray.Length() == 3)
    {
      degree = 2;
    }
    point_array     = (double*)&PointsArray.Value(PointsArray.Lower());
    parameter_array = (double*)&ParametersArray.Value(1);
    TangentFlags.SetValue(1, true);
    PLib::EvalLagrange(ParametersArray.Value(1),
                       1,
                       degree,
                       3,
                       point_array[0],
                       parameter_array[0],
                       eval_result[0][0]);
    for (ii = 1; ii <= 3; ii++)
    {
      a_vector.SetCoord(ii, eval_result[1][ii - 1]);
    }
    TangentsArray.SetValue(1, a_vector);
  }
}

//=================================================================================================

static void BuildTangents(const NCollection_Array1<gp_Pnt>&   PointsArray,
                          NCollection_Array1<gp_Vec>&         TangentsArray,
                          NCollection_Array1<bool>&    TangentFlags,
                          const NCollection_Array1<double>& ParametersArray)
{
  int ii, degree;
  double *  point_array, *parameter_array,

    eval_result[2][3];
  gp_Vec a_vector;

  degree = 3;

  if (PointsArray.Length() < 3)
  {
    throw Standard_ConstructionError();
  }
  if (PointsArray.Length() == 3)
  {
    degree = 2;
  }
  if (!TangentFlags.Value(1))
  {
    point_array     = (double*)&PointsArray.Value(PointsArray.Lower());
    parameter_array = (double*)&ParametersArray.Value(1);
    TangentFlags.SetValue(1, true);
    PLib::EvalLagrange(ParametersArray.Value(1),
                       1,
                       degree,
                       3,
                       point_array[0],
                       parameter_array[0],
                       eval_result[0][0]);
    for (ii = 1; ii <= 3; ii++)
    {
      a_vector.SetCoord(ii, eval_result[1][ii - 1]);
    }
    TangentsArray.SetValue(1, a_vector);
  }
  if (!TangentFlags.Value(TangentFlags.Upper()))
  {
    point_array = (double*)&PointsArray.Value(PointsArray.Upper() - degree);
    TangentFlags.SetValue(TangentFlags.Upper(), true);
    parameter_array = (double*)&ParametersArray.Value(ParametersArray.Upper() - degree);
    PLib::EvalLagrange(ParametersArray.Value(ParametersArray.Upper()),
                       1,
                       degree,
                       3,
                       point_array[0],
                       parameter_array[0],
                       eval_result[0][0]);
    for (ii = 1; ii <= 3; ii++)
    {
      a_vector.SetCoord(ii, eval_result[1][ii - 1]);
    }
    TangentsArray.SetValue(TangentsArray.Upper(), a_vector);
  }
}

//=======================================================================
// function : BuildTangents
// purpose  : scale the given tangent so that they have the length of
// the size of the derivative of the lagrange interpolation
//
//=======================================================================
static void ScaleTangents(const NCollection_Array1<gp_Pnt>&      PointsArray,
                          NCollection_Array1<gp_Vec>&            TangentsArray,
                          const NCollection_Array1<bool>& TangentFlags,
                          const NCollection_Array1<double>&    ParametersArray)
{
  int ii, jj, degree = 0, index, num_points;

  double *point_array, *parameter_array, value[2], ratio, eval_result[2][3];

  gp_Vec a_vector;

  num_points = PointsArray.Length();
  if (num_points == 2)
  {
    degree = 1;
  }
  else if (num_points >= 3)
  {
    degree = 2;
  }

  index = PointsArray.Lower();
  for (ii = TangentFlags.Lower(); ii <= TangentFlags.Upper(); ii++)
  {
    if (TangentFlags.Value(ii))
    {
      point_array     = (double*)&PointsArray.Value(index);
      parameter_array = (double*)&ParametersArray.Value(index);
      PLib::EvalLagrange(ParametersArray.Value(ii),
                         1,
                         degree,
                         3,
                         point_array[0],
                         parameter_array[0],
                         eval_result[0][0]);
      value[0] = value[1] = 0.0e0;
      for (jj = 1; jj <= 3; jj++)
      {
        value[0] += std::abs(TangentsArray.Value(ii).Coord(jj));
        value[1] += std::abs(eval_result[1][jj - 1]);
      }
      ratio = value[1] / value[0];
      for (jj = 1; jj <= 3; jj++)
      {
        a_vector.SetCoord(jj, ratio * TangentsArray.Value(ii).Coord(jj));
      }
      TangentsArray.SetValue(ii, a_vector);
      if (ii != TangentFlags.Lower())
      {
        index += 1;
      }
      if (index > PointsArray.Upper() - degree)
      {
        index = PointsArray.Upper() - degree;
      }
    }
  }
}

//=================================================================================================

GeomAPI_Interpolate::GeomAPI_Interpolate(const occ::handle<NCollection_HArray1<gp_Pnt>>& PointsPtr,
                                         const bool             PeriodicFlag,
                                         const double                Tolerance)
    : myTolerance(Tolerance),
      myPoints(PointsPtr),
      myIsDone(false),
      myPeriodic(PeriodicFlag),
      myTangentRequest(false)
{
  int ii;
  bool result = CheckPoints(PointsPtr->Array1(), Tolerance);
  myTangents              = new NCollection_HArray1<gp_Vec>(myPoints->Lower(), myPoints->Upper());
  myTangentFlags          = new NCollection_HArray1<bool>(myPoints->Lower(), myPoints->Upper());

  if (!result)
  {
    throw Standard_ConstructionError();
  }
  BuildParameters(PeriodicFlag, PointsPtr->Array1(), myParameters);

  for (ii = myPoints->Lower(); ii <= myPoints->Upper(); ii++)
  {
    myTangentFlags->SetValue(ii, false);
  }
}

//=================================================================================================

GeomAPI_Interpolate::GeomAPI_Interpolate(const occ::handle<NCollection_HArray1<gp_Pnt>>&   PointsPtr,
                                         const occ::handle<NCollection_HArray1<double>>& ParametersPtr,
                                         const bool               PeriodicFlag,
                                         const double                  Tolerance)
    : myTolerance(Tolerance),
      myPoints(PointsPtr),
      myIsDone(false),
      myParameters(ParametersPtr),
      myPeriodic(PeriodicFlag),
      myTangentRequest(false)
{
  int ii;

  bool result = CheckPoints(PointsPtr->Array1(), Tolerance);

  if (PeriodicFlag)
  {
    if ((PointsPtr->Length()) + 1 != ParametersPtr->Length())
    {
      throw Standard_ConstructionError();
    }
  }
  myTangents     = new NCollection_HArray1<gp_Vec>(myPoints->Lower(), myPoints->Upper());
  myTangentFlags = new NCollection_HArray1<bool>(myPoints->Lower(), myPoints->Upper());

  if (!result)
  {
    throw Standard_ConstructionError();
  }

  result = CheckParameters(ParametersPtr->Array1());
  if (!result)
  {
    throw Standard_ConstructionError();
  }

  for (ii = myPoints->Lower(); ii <= myPoints->Upper(); ii++)
  {
    myTangentFlags->SetValue(ii, false);
  }
}

//=================================================================================================

void GeomAPI_Interpolate::Load(const NCollection_Array1<gp_Vec>&               Tangents,
                               const occ::handle<NCollection_HArray1<bool>>& TangentFlagsPtr,
                               const bool                  Scale)

{
  bool result;
  int ii;
  myTangentRequest = true;
  myTangentFlags   = TangentFlagsPtr;
  if (Tangents.Length() != myPoints->Length() || TangentFlagsPtr->Length() != myPoints->Length())
  {
    throw Standard_ConstructionError();
  }
  result = CheckTangents(Tangents, TangentFlagsPtr->Array1(), myTolerance);
  if (result)
  {
    myTangents = new NCollection_HArray1<gp_Vec>(Tangents.Lower(), Tangents.Upper());
    for (ii = Tangents.Lower(); ii <= Tangents.Upper(); ii++)
    {
      myTangents->SetValue(ii, Tangents.Value(ii));
    }

    if (Scale)
    {
      ScaleTangents(myPoints->Array1(),
                    myTangents->ChangeArray1(),
                    TangentFlagsPtr->Array1(),
                    myParameters->Array1());
    }
  }
  else
  {
    throw Standard_ConstructionError();
  }
}

//=================================================================================================

void GeomAPI_Interpolate::Load(const gp_Vec&          InitialTangent,
                               const gp_Vec&          FinalTangent,
                               const bool Scale)
{
  bool result;
  myTangentRequest = true;
  myTangentFlags->SetValue(1, true);
  myTangentFlags->SetValue(myPoints->Length(), true);
  myTangents->SetValue(1, InitialTangent);
  myTangents->SetValue(myPoints->Length(), FinalTangent);
  result = CheckTangents(myTangents->Array1(), myTangentFlags->Array1(), myTolerance);
  if (!result)
  {
    throw Standard_ConstructionError();
  }

  if (Scale)
  {
    ScaleTangents(myPoints->Array1(),
                  myTangents->ChangeArray1(),
                  myTangentFlags->Array1(),
                  myParameters->Array1());
  }
}

//=================================================================================================

void GeomAPI_Interpolate::Perform()
{
  if (myPeriodic)
  {
    PerformPeriodic();
  }
  else
  {
    PerformNonPeriodic();
  }
}

//=================================================================================================

void GeomAPI_Interpolate::PerformPeriodic()
{
  int degree, ii, jj, index, index1,
    //  index2,
    mult_index, half_order, inversion_problem, num_points, num_distinct_knots, num_poles;

  double period;

  gp_Pnt a_point;

  num_points = myPoints->Length();
  period = myParameters->Value(myParameters->Upper()) - myParameters->Value(myParameters->Lower());
  num_poles = num_points + 1;
  if (num_points == 2 && !myTangentRequest)
  {
    //
    // build a periodic curve of degree 1
    //

    degree = 1;
    NCollection_Array1<int> deg1_mults(1, num_poles);
    for (ii = 1; ii <= num_poles; ii++)
    {
      deg1_mults.SetValue(ii, 1);
    }

    myCurve  = new Geom_BSplineCurve(myPoints->Array1(),
                                    myParameters->Array1(),
                                    deg1_mults,
                                    degree,
                                    myPeriodic);
    myIsDone = true;
  }
  else
  {
    num_distinct_knots = num_points + 1;
    half_order         = 2;
    degree             = 3;
    num_poles += 2;
    if (myTangentRequest)
      for (ii = myTangentFlags->Lower() + 1; ii <= myTangentFlags->Upper(); ii++)
      {
        if (myTangentFlags->Value(ii))
        {
          num_poles += 1;
        }
      }

    NCollection_Array1<double>    parameters(1, num_poles);
    NCollection_Array1<double>    flatknots(1, num_poles + degree + 1);
    NCollection_Array1<int> mults(1, num_distinct_knots);
    NCollection_Array1<int> contact_order_array(1, num_poles);
    NCollection_Array1<gp_Pnt>      poles(1, num_poles);

    for (ii = 1; ii <= half_order; ii++)
    {
      flatknots.SetValue(ii, myParameters->Value(myParameters->Upper() - 1) - period);
      flatknots.SetValue(ii + half_order, myParameters->Value(myParameters->Lower()));
      flatknots.SetValue(num_poles + ii, myParameters->Value(myParameters->Upper()));
      flatknots.SetValue(num_poles + half_order + ii, myParameters->Value(half_order) + period);
    }
    for (ii = 1; ii <= num_poles; ii++)
    {
      contact_order_array.SetValue(ii, 0);
    }
    for (ii = 2; ii < num_distinct_knots; ii++)
    {
      mults.SetValue(ii, 1);
    }
    mults.SetValue(1, half_order);
    mults.SetValue(num_distinct_knots, half_order);
    if (num_points >= 3)
    {

      //
      //   only enter here if there are more than 3 points otherwise
      //   it means we have already the tangent
      //
      BuildPeriodicTangent(myPoints->Array1(),
                           myTangents->ChangeArray1(),
                           myTangentFlags->ChangeArray1(),
                           myParameters->Array1());
    }
    contact_order_array.SetValue(2, 1);
    parameters.SetValue(1, myParameters->Value(1));
    parameters.SetValue(2, myParameters->Value(1));
    poles.SetValue(1, myPoints->Value(1));
    for (jj = 1; jj <= 3; jj++)
    {
      a_point.SetCoord(jj, myTangents->Value(1).Coord(jj));
    }
    poles.SetValue(2, a_point);

    mult_index = 2;
    index      = 3;
    index1     = degree + 2;
    if (myTangentRequest)
    {
      for (ii = myTangentFlags->Lower() + 1; ii <= myTangentFlags->Upper(); ii++)
      {
        parameters.SetValue(index, myParameters->Value(ii));
        flatknots.SetValue(index1, myParameters->Value(ii));
        poles.SetValue(index, myPoints->Value(ii));
        index += 1;
        index1 += 1;
        if (myTangentFlags->Value(ii))
        {
          mults.SetValue(mult_index, mults.Value(mult_index) + 1);
          contact_order_array(index) = 1;

          parameters.SetValue(index, myParameters->Value(ii));
          flatknots.SetValue(index1, myParameters->Value(ii));
          for (jj = 1; jj <= 3; jj++)
          {
            a_point.SetCoord(jj, myTangents->Value(ii).Coord(jj));
          }
          poles.SetValue(index, a_point);
          index += 1;
          index1 += 1;
        }
        mult_index += 1;
      }
    }
    else
    {
      index  = degree + 1;
      index1 = 2;
      for (ii = myParameters->Lower(); ii <= myParameters->Upper(); ii++)
      {
        parameters.SetValue(index1, myParameters->Value(ii));
        flatknots.SetValue(index, myParameters->Value(ii));
        index += 1;
        index1 += 1;
      }
      index = 3;
      for (ii = myPoints->Lower() + 1; ii <= myPoints->Upper(); ii++)
      {
        //
        // copy all the given points since the last one will be initialized
        // below by the first point in the array myPoints
        //
        poles.SetValue(index, myPoints->Value(ii));
        index += 1;
      }
    }
    contact_order_array.SetValue(num_poles - 1, 1);
    parameters.SetValue(num_poles - 1, myParameters->Value(myParameters->Upper()));
    //
    // for the periodic curve ONLY  the tangent of the first point
    // will be used since the curve should close itself at the first
    // point See BuildPeriodicTangent
    //
    for (jj = 1; jj <= 3; jj++)
    {
      a_point.SetCoord(jj, myTangents->Value(1).Coord(jj));
    }
    poles.SetValue(num_poles - 1, a_point);

    parameters.SetValue(num_poles, myParameters->Value(myParameters->Upper()));

    poles.SetValue(num_poles, myPoints->Value(1));

    BSplCLib::Interpolate(degree,
                          flatknots,
                          parameters,
                          contact_order_array,
                          poles,
                          inversion_problem);
    if (!inversion_problem)
    {
      NCollection_Array1<gp_Pnt> newpoles(poles.Value(1), 1, num_poles - 2);
      myCurve  = new Geom_BSplineCurve(newpoles, myParameters->Array1(), mults, degree, myPeriodic);
      myIsDone = true;
    }
  }
}

//=================================================================================================

void GeomAPI_Interpolate::PerformNonPeriodic()
{
  int degree, ii, jj, index, index1, index2, index3, mult_index, inversion_problem,
    num_points, num_distinct_knots, num_poles;

  gp_Pnt a_point;

  num_points = num_distinct_knots = num_poles = myPoints->Length();
  if (num_poles == 2 && !myTangentRequest)
  {
    degree = 1;
  }
  else if (num_poles == 3 && !myTangentRequest)
  {
    degree             = 2;
    num_distinct_knots = 2;
  }
  else
  {
    degree = 3;
    num_poles += 2;
    if (myTangentRequest)
      for (ii = myTangentFlags->Lower() + 1; ii < myTangentFlags->Upper(); ii++)
      {
        if (myTangentFlags->Value(ii))
        {
          num_poles += 1;
        }
      }
  }

  NCollection_Array1<double>    parameters(1, num_poles);
  NCollection_Array1<double>    flatknots(1, num_poles + degree + 1);
  NCollection_Array1<int> mults(1, num_distinct_knots);
  NCollection_Array1<double>    knots(1, num_distinct_knots);
  NCollection_Array1<int> contact_order_array(1, num_poles);
  NCollection_Array1<gp_Pnt>      poles(1, num_poles);

  for (ii = 1; ii <= degree + 1; ii++)
  {
    flatknots.SetValue(ii, myParameters->Value(1));
    flatknots.SetValue(ii + num_poles, myParameters->Value(num_points));
  }
  for (ii = 1; ii <= num_poles; ii++)
  {
    contact_order_array.SetValue(ii, 0);
  }
  for (ii = 2; ii < num_distinct_knots; ii++)
  {
    mults.SetValue(ii, 1);
  }
  mults.SetValue(1, degree + 1);
  mults.SetValue(num_distinct_knots, degree + 1);

  switch (degree)
  {
    case 1:
      for (ii = 1; ii <= num_poles; ii++)
      {
        poles.SetValue(ii, myPoints->Value(ii));
      }
      myCurve  = new Geom_BSplineCurve(poles, myParameters->Array1(), mults, degree);
      myIsDone = true;
      break;
    case 2:
      knots.SetValue(1, myParameters->Value(1));
      knots.SetValue(2, myParameters->Value(3));
      for (ii = 1; ii <= num_poles; ii++)
      {
        poles.SetValue(ii, myPoints->Value(ii));
      }
      BSplCLib::Interpolate(degree,
                            flatknots,
                            myParameters->Array1(),
                            contact_order_array,
                            poles,
                            inversion_problem);
      if (!inversion_problem)
      {
        myCurve  = new Geom_BSplineCurve(poles, knots, mults, degree);
        myIsDone = true;
      }
      break;
    case 3:
      //
      // check if the boundary conditions are set
      //
      if (num_points >= 3)
      {
        //
        // cannot build the tangents with degree 3 with only 2 points
        // if those where not given in advance
        //
        BuildTangents(myPoints->Array1(),
                      myTangents->ChangeArray1(),
                      myTangentFlags->ChangeArray1(),
                      myParameters->Array1());
      }
      contact_order_array.SetValue(2, 1);
      parameters.SetValue(1, myParameters->Value(1));
      parameters.SetValue(2, myParameters->Value(1));
      poles.SetValue(1, myPoints->Value(1));
      for (jj = 1; jj <= 3; jj++)
      {
        a_point.SetCoord(jj, myTangents->Value(1).Coord(jj));
      }
      poles.SetValue(2, a_point);
      mult_index = 2;
      index      = 3;
      index1     = 2;
      index2     = myPoints->Lower() + 1;
      index3     = degree + 2;
      if (myTangentRequest)
      {
        for (ii = myParameters->Lower() + 1; ii < myParameters->Upper(); ii++)
        {
          parameters.SetValue(index, myParameters->Value(ii));
          poles.SetValue(index, myPoints->Value(index2));
          flatknots.SetValue(index3, myParameters->Value(ii));
          index += 1;
          index3 += 1;
          if (myTangentFlags->Value(index1))
          {
            //
            // set the multiplicities, the order of the contact, the
            // the flatknots,
            //
            mults.SetValue(mult_index, mults.Value(mult_index) + 1);
            contact_order_array(index) = 1;
            flatknots.SetValue(index3, myParameters->Value(ii));
            parameters.SetValue(index, myParameters->Value(ii));
            for (jj = 1; jj <= 3; jj++)
            {
              a_point.SetCoord(jj, myTangents->Value(ii).Coord(jj));
            }
            poles.SetValue(index, a_point);
            index += 1;
            index3 += 1;
          }
          mult_index += 1;
          index1 += 1;
          index2 += 1;
        }
      }
      else
      {
        index1 = 2;
        for (ii = myParameters->Lower(); ii <= myParameters->Upper(); ii++)
        {
          parameters.SetValue(index1, myParameters->Value(ii));
          index1 += 1;
        }
        index = 3;
        for (ii = myPoints->Lower() + 1; ii <= myPoints->Upper() - 1; ii++)
        {
          poles.SetValue(index, myPoints->Value(ii));
          index += 1;
        }

        index = degree + 1;
        for (ii = myParameters->Lower(); ii <= myParameters->Upper(); ii++)
        {
          flatknots.SetValue(index, myParameters->Value(ii));
          index += 1;
        }
      }
      for (jj = 1; jj <= 3; jj++)
      {
        a_point.SetCoord(jj, myTangents->Value(num_points).Coord(jj));
      }
      poles.SetValue(num_poles - 1, a_point);

      contact_order_array.SetValue(num_poles - 1, 1);
      parameters.SetValue(num_poles, myParameters->Value(myParameters->Upper()));
      parameters.SetValue(num_poles - 1, myParameters->Value(myParameters->Upper()));

      poles.SetValue(num_poles, myPoints->Value(num_points));

      BSplCLib::Interpolate(degree,
                            flatknots,
                            parameters,
                            contact_order_array,
                            poles,
                            inversion_problem);
      if (!inversion_problem)
      {
        myCurve  = new Geom_BSplineCurve(poles, myParameters->Array1(), mults, degree);
        myIsDone = true;
      }
      break;
  }
}

//=======================================================================
// function : occ::handle<Geom_BSplineCurve>&
// purpose  :
//=======================================================================

const occ::handle<Geom_BSplineCurve>& GeomAPI_Interpolate::Curve() const
{
  if (!myIsDone)
    throw StdFail_NotDone(" ");
  return myCurve;
}

//=================================================================================================

GeomAPI_Interpolate::operator occ::handle<Geom_BSplineCurve>() const
{
  return myCurve;
}

//=================================================================================================

bool GeomAPI_Interpolate::IsDone() const
{
  return myIsDone;
}
