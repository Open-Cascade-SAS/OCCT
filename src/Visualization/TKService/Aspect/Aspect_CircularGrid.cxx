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

#include <Aspect_CircularGrid.hxx>

#include <Standard_NegativeValue.hxx>
#include <Standard_NullValue.hxx>
#include <Standard_NumericError.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Aspect_CircularGrid, Aspect_Grid)

Aspect_CircularGrid::Aspect_CircularGrid(const double    aRadiusStep,
                                         const int aDivisionNumber,
                                         const double    anXOrigin,
                                         const double    anYOrigin,
                                         const double    aRotationAngle)
    : Aspect_Grid(anXOrigin, anYOrigin, aRotationAngle),
      myRadiusStep(aRadiusStep),
      myDivisionNumber(aDivisionNumber)
{
}

void Aspect_CircularGrid::SetRadiusStep(const double aRadiusStep)
{
  Standard_NegativeValue_Raise_if(aRadiusStep < 0., "invalid radius step");
  Standard_NullValue_Raise_if(aRadiusStep == 0., "invalid radius step");
  myRadiusStep = aRadiusStep;
  Init();
  UpdateDisplay();
}

void Aspect_CircularGrid::SetDivisionNumber(const int aNumber)
{
  Standard_NegativeValue_Raise_if(aNumber < 0., "invalid division number");
  Standard_NullValue_Raise_if(aNumber == 0., "invalid division number");
  myDivisionNumber = aNumber;
  Init();
  UpdateDisplay();
}

void Aspect_CircularGrid::SetGridValues(const double    theXOrigin,
                                        const double    theYOrigin,
                                        const double    theRadiusStep,
                                        const int theDivisionNumber,
                                        const double    theRotationAngle)
{
  myXOrigin = theXOrigin;
  myYOrigin = theYOrigin;
  Standard_NegativeValue_Raise_if(theRadiusStep < 0., "invalid radius step");
  Standard_NullValue_Raise_if(theRadiusStep == 0., "invalid radius step");
  myRadiusStep = theRadiusStep;
  Standard_NegativeValue_Raise_if(theDivisionNumber < 0., "invalid division number");
  Standard_NullValue_Raise_if(theDivisionNumber == 0., "invalid division number");
  myDivisionNumber = theDivisionNumber;
  myRotationAngle  = theRotationAngle;
  Init();
  UpdateDisplay();
}

void Aspect_CircularGrid::Compute(const double X,
                                  const double Y,
                                  double&      gridX,
                                  double&      gridY) const
{

  double    xo      = XOrigin();
  double    yo      = YOrigin();
  double    d       = std::sqrt((xo - X) * (xo - X) + (yo - Y) * (yo - Y));
  int n       = (int)(d / myRadiusStep + 0.5);
  double    radius  = double(n) * myRadiusStep;
  double    cosinus = (X - xo) / d;
  double    a       = std::acos(cosinus);
  double    ra      = RotationAngle();
  if (Y < yo)
    a = 2 * M_PI - a;
  n = (int)((a - ra) / myAlpha + std::copysign(0.5, a - ra));

  double    cs = 0, sn = 0;
  bool done = false;
  int nmax = 2 * myDivisionNumber;
  int nquad, qmax;

  if (ra == 0.)
  {
    nquad = 4;
    qmax  = nmax / nquad;
    if ((n == 0) || (!(nmax % nquad) && !(n % qmax)))
    {
      int q = n / qmax;
      switch (q)
      {
        default:
        case 0:
          cs = 1.;
          sn = 0.;
          break;
        case 1:
          cs = 0.;
          sn = 1.;
          break;
        case 2:
          cs = -1.;
          sn = 0.;
          break;
        case 3:
          cs = 0.;
          sn = -1.;
          break;
      }
      done = true;
    }
    else
    {
      nquad = 2;
      qmax  = nmax / nquad;
      if (!(nmax % nquad) && !(n % qmax))
      {
        int q = n / qmax;
        switch (q)
        {
          default:
          case 0:
            cs = 1.;
            sn = 0.;
            break;
          case 1:
            cs = -1.;
            sn = 0.;
            break;
        }
        done = true;
      }
    }
  }

  if (!done)
  {
    double ang = ra + double(n) * myAlpha;
    cs                = std::cos(ang);
    sn                = std::sin(ang);
  }
  gridX = xo + cs * radius;
  gridY = yo + sn * radius;
}

double Aspect_CircularGrid::RadiusStep() const
{
  return myRadiusStep;
}

int Aspect_CircularGrid::DivisionNumber() const
{
  return myDivisionNumber;
}

void Aspect_CircularGrid::Init()
{
  myAlpha = M_PI / double(myDivisionNumber);
  myA1    = std::cos(myAlpha);
  myB1    = std::sin(myAlpha);
}

//=================================================================================================

void Aspect_CircularGrid::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Aspect_Grid)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRadiusStep)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDivisionNumber)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlpha)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myA1)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myB1)
}
