// Created on: 1995-11-16
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

// Programme cree

#include <BSplCLib.hxx>
#include <Law_BSpline.hxx>
#include <Law_Interpolate.hxx>
#include <PLib.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

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
                            const NCollection_Array1<double>&    PointsArray,
                            occ::handle<NCollection_HArray1<double>>& ParametersPtr)
{
  int ii, index = 2;
  double    distance;
  int num_parameters = PointsArray.Length();
  if (PeriodicFlag)
  {
    num_parameters += 1;
  }
  ParametersPtr = new NCollection_HArray1<double>(1, num_parameters);
  ParametersPtr->SetValue(1, 0.);
  for (ii = PointsArray.Lower(); ii < PointsArray.Upper(); ii++)
  {
    distance = std::abs(PointsArray.Value(ii) - PointsArray.Value(ii + 1));
    ParametersPtr->SetValue(index, ParametersPtr->Value(ii) + distance);
    index += 1;
  }
  if (PeriodicFlag)
  {
    distance =
      std::abs(PointsArray.Value(PointsArray.Upper()) - PointsArray.Value(PointsArray.Lower()));
    ParametersPtr->SetValue(index, ParametersPtr->Value(ii) + distance);
  }
}

//=================================================================================================

static void BuildPeriodicTangent(const NCollection_Array1<double>& PointsArray,
                                 NCollection_Array1<double>&       TangentsArray,
                                 NCollection_Array1<bool>&    TangentFlags,
                                 const NCollection_Array1<double>& ParametersArray)
{
  double point_array[3], parameter_array[3], eval_result[2];

  if (PointsArray.Length() < 2)
  {
    TangentFlags.SetValue(1, true);
    TangentsArray.SetValue(1, 0.);
  }
  else if (!TangentFlags.Value(1))
  {
    // Pour les periodiques on evalue la tangente du point de fermeture
    // par une interpolation de degre 2 entre le dernier point, le point
    // de fermeture et le deuxieme point.
    int degree = 2;
    double    period = (ParametersArray.Value(ParametersArray.Upper())
                            - ParametersArray.Value(ParametersArray.Lower()));
    point_array[0]          = PointsArray.Value(PointsArray.Upper());
    point_array[1]          = PointsArray.Value(PointsArray.Lower());
    point_array[2]          = PointsArray.Value(PointsArray.Lower() + 1);
    parameter_array[0]      = ParametersArray.Value(ParametersArray.Upper() - 1) - period;
    parameter_array[1]      = ParametersArray.Value(ParametersArray.Lower());
    parameter_array[2]      = ParametersArray.Value(ParametersArray.Lower() + 1);
    TangentFlags.SetValue(1, true);
    PLib::EvalLagrange(parameter_array[1],
                       1,
                       degree,
                       1,
                       point_array[0],
                       parameter_array[0],
                       eval_result[0]);
    TangentsArray.SetValue(1, eval_result[1]);
  }
}

//=================================================================================================

static void BuildTangents(const NCollection_Array1<double>& PointsArray,
                          NCollection_Array1<double>&       TangentsArray,
                          NCollection_Array1<bool>&    TangentFlags,
                          const NCollection_Array1<double>& ParametersArray)
{
  int degree = 3; //,ii;
  double *  point_array, *parameter_array, eval_result[2];

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
                       1,
                       point_array[0],
                       parameter_array[0],
                       eval_result[0]);
    TangentsArray.SetValue(1, eval_result[1]);
  }
  if (!TangentFlags.Value(TangentFlags.Upper()))
  {
    point_array = (double*)&PointsArray.Value(PointsArray.Upper() - degree);
    TangentFlags.SetValue(TangentFlags.Upper(), true);
    int iup = ParametersArray.Upper() - degree;
    parameter_array      = (double*)&ParametersArray.Value(iup);
    PLib::EvalLagrange(ParametersArray.Value(ParametersArray.Upper()),
                       1,
                       degree,
                       1,
                       point_array[0],
                       parameter_array[0],
                       eval_result[0]);
    TangentsArray.SetValue(TangentsArray.Upper(), eval_result[1]);
  }
}

//=================================================================================================

Law_Interpolate::Law_Interpolate(const occ::handle<NCollection_HArray1<double>>& PointsPtr,
                                 const bool               PeriodicFlag,
                                 const double                  Tolerance)
    : myTolerance(Tolerance),
      myPoints(PointsPtr),
      myIsDone(false),
      myPeriodic(PeriodicFlag),
      myTangentRequest(false)

{
  // int ii;
  myTangents     = new NCollection_HArray1<double>(myPoints->Lower(), myPoints->Upper());
  myTangentFlags = new NCollection_HArray1<bool>(myPoints->Lower(), myPoints->Upper());

  BuildParameters(PeriodicFlag, PointsPtr->Array1(), myParameters);
  myTangentFlags->Init(false);
}

//=================================================================================================

Law_Interpolate::Law_Interpolate(const occ::handle<NCollection_HArray1<double>>& PointsPtr,
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
  // int ii;
  if (PeriodicFlag)
  {
    if ((PointsPtr->Length()) + 1 != ParametersPtr->Length())
    {
      throw Standard_ConstructionError();
    }
  }
  myTangents              = new NCollection_HArray1<double>(myPoints->Lower(), myPoints->Upper());
  myTangentFlags          = new NCollection_HArray1<bool>(myPoints->Lower(), myPoints->Upper());
  bool result = CheckParameters(ParametersPtr->Array1());
  if (!result)
  {
    throw Standard_ConstructionError();
  }
  myTangentFlags->Init(false);
}

//=================================================================================================

void Law_Interpolate::Load(const NCollection_Array1<double>&             Tangents,
                           const occ::handle<NCollection_HArray1<bool>>& TangentFlagsPtr)
{
  // bool result;
  int ii;
  myTangentRequest = true;
  myTangentFlags   = TangentFlagsPtr;
  if (Tangents.Length() != myPoints->Length() || TangentFlagsPtr->Length() != myPoints->Length())
  {
    throw Standard_ConstructionError();
  }
  myTangents = new NCollection_HArray1<double>(Tangents.Lower(), Tangents.Upper());
  for (ii = Tangents.Lower(); ii <= Tangents.Upper(); ii++)
  {
    myTangents->SetValue(ii, Tangents.Value(ii));
  }
}

//=================================================================================================

void Law_Interpolate::Load(const double InitialTangent, const double FinalTangent)
{
  // bool result;
  myTangentRequest = true;
  myTangentFlags->SetValue(1, true);
  myTangents->SetValue(1, InitialTangent);
  myTangentFlags->SetValue(myPoints->Length(), true);
  myTangents->SetValue(myPoints->Length(), FinalTangent);
}

//=================================================================================================

void Law_Interpolate::Perform()
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

void Law_Interpolate::PerformPeriodic()
{
  int degree, ii,
    // jj,
    index, index1,
    // index2,
    mult_index, half_order, inversion_problem, num_points, num_distinct_knots, num_poles;

  double period;

  // gp_Pnt a_point;

  num_points = myPoints->Length();
  period = myParameters->Value(myParameters->Upper()) - myParameters->Value(myParameters->Lower());
  num_poles          = num_points + 1;
  num_distinct_knots = num_points + 1;
  half_order         = 2;
  degree             = 3;
  num_poles += 2;
  if (myTangentRequest)
  {
    for (ii = myTangentFlags->Lower() + 1; ii <= myTangentFlags->Upper(); ii++)
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
  NCollection_Array1<int> contact_order_array(1, num_poles);
  NCollection_Array1<double>    poles(1, num_poles);

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

  BuildPeriodicTangent(myPoints->Array1(),
                       myTangents->ChangeArray1(),
                       myTangentFlags->ChangeArray1(),
                       myParameters->Array1());

  contact_order_array.SetValue(2, 1);
  parameters.SetValue(1, myParameters->Value(1));
  parameters.SetValue(2, myParameters->Value(1));
  poles.SetValue(1, myPoints->Value(1));
  poles.SetValue(2, myTangents->Value(1));
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
        poles.SetValue(index, myTangents->Value(ii));
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
  poles.SetValue(num_poles - 1, myTangents->Value(1));
  parameters.SetValue(num_poles, myParameters->Value(myParameters->Upper()));
  poles.SetValue(num_poles, myPoints->Value(1));

  BSplCLib::Interpolate(degree,
                        flatknots,
                        parameters,
                        contact_order_array,
                        1,
                        poles(1),
                        inversion_problem);
  if (!inversion_problem)
  {
    NCollection_Array1<double> newpoles(poles.Value(1), 1, num_poles - 2);
    myCurve  = new Law_BSpline(newpoles, myParameters->Array1(), mults, degree, myPeriodic);
    myIsDone = true;
  }
}

//=================================================================================================

void Law_Interpolate::PerformNonPeriodic()
{
  int degree, ii,
    // jj,
    index, index1, index2, index3, mult_index, inversion_problem, num_points, num_distinct_knots,
    num_poles;

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
    {
      for (ii = myTangentFlags->Lower() + 1; ii < myTangentFlags->Upper(); ii++)
      {
        if (myTangentFlags->Value(ii))
        {
          num_poles += 1;
        }
      }
    }
  }
  NCollection_Array1<double>    parameters(1, num_poles);
  NCollection_Array1<double>    flatknots(1, num_poles + degree + 1);
  NCollection_Array1<int> mults(1, num_distinct_knots);
  NCollection_Array1<double>    knots(1, num_distinct_knots);
  NCollection_Array1<int> contact_order_array(1, num_poles);
  NCollection_Array1<double>    poles(1, num_poles);

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
      myCurve  = new Law_BSpline(poles, myParameters->Array1(), mults, degree);
      myIsDone = true;
      break;
    case 2:
      knots.SetValue(1, myParameters->Value(1));
      knots.SetValue(2, myParameters->Value(num_poles));
      for (ii = 1; ii <= num_poles; ii++)
      {
        poles.SetValue(ii, myPoints->Value(ii));
      }
      BSplCLib::Interpolate(degree,
                            flatknots,
                            myParameters->Array1(),
                            contact_order_array,
                            1,
                            poles(1),
                            inversion_problem);
      if (!inversion_problem)
      {
        myCurve  = new Law_BSpline(poles, knots, mults, degree);
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
      poles.SetValue(2, myTangents->Value(1));
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
            poles.SetValue(index, myTangents->Value(ii));
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
      poles.SetValue(num_poles - 1, myTangents->Value(num_points));

      contact_order_array.SetValue(num_poles - 1, 1);
      parameters.SetValue(num_poles, myParameters->Value(myParameters->Upper()));
      parameters.SetValue(num_poles - 1, myParameters->Value(myParameters->Upper()));

      poles.SetValue(num_poles, myPoints->Value(num_points));

      BSplCLib::Interpolate(degree,
                            flatknots,
                            parameters,
                            contact_order_array,
                            1,
                            poles(1),
                            inversion_problem);
      if (!inversion_problem)
      {
        myCurve  = new Law_BSpline(poles, myParameters->Array1(), mults, degree);
        myIsDone = true;
      }
      break;
  }
}

//=======================================================================
// function : occ::handle<Geom_BSplineCurve>&
// purpose  :
//=======================================================================

const occ::handle<Law_BSpline>& Law_Interpolate::Curve() const
{
  if (!myIsDone)
    throw StdFail_NotDone(" ");
  return myCurve;
}

//=================================================================================================

bool Law_Interpolate::IsDone() const
{
  return myIsDone;
}
