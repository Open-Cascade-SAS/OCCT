#include <GeomLProp_LegacyCLProps.hxx>

#include <GeomAbs_Shape.hxx>
#include <Geom_Curve.hxx>
#include <LProp_NotDefined.hxx>
#include <Standard_OutOfRange.hxx>

#include <algorithm>
#include <cmath>

namespace
{
constexpr double THE_MIN_STEP = 1.0e-7;

static int continuity(const occ::handle<Geom_Curve>& theCurve)
{
  switch (theCurve->Continuity())
  {
    case GeomAbs_C0: return 0;
    case GeomAbs_C1: return 1;
    case GeomAbs_C2: return 2;
    case GeomAbs_C3: return 3;
    case GeomAbs_CN: return 3;
    default: return 0;
  }
}
}

GeomLProp_LegacyCLProps::GeomLProp_LegacyCLProps(const occ::handle<Geom_Curve>& theCurve,
                                                 double                         theParam,
                                                 int                            theDerOrder,
                                                 double                         theResolution)
    : myCurve(theCurve),
      myDerOrder(theDerOrder),
      myCN(continuity(theCurve)),
      myLinTol(theResolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(theDerOrder < 0 || theDerOrder > 3, "GeomLProp_LegacyCLProps()");
  SetParameter(theParam);
}

GeomLProp_LegacyCLProps::GeomLProp_LegacyCLProps(const occ::handle<Geom_Curve>& theCurve,
                                                 int                            theDerOrder,
                                                 double                         theResolution)
    : myCurve(theCurve),
      myU(RealLast()),
      myDerOrder(theDerOrder),
      myCN(continuity(theCurve)),
      myLinTol(theResolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(theDerOrder < 0 || theDerOrder > 3, "GeomLProp_LegacyCLProps()");
}

GeomLProp_LegacyCLProps::GeomLProp_LegacyCLProps(int theDerOrder, double theResolution)
    : myU(RealLast()),
      myDerOrder(theDerOrder),
      myCN(0),
      myLinTol(theResolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(theDerOrder < 0 || theDerOrder > 3, "GeomLProp_LegacyCLProps()");
}

void GeomLProp_LegacyCLProps::SetParameter(const double theParam)
{
  myU = theParam;
  switch (myDerOrder)
  {
    case 0: myCurve->D0(myU, myPnt); break;
    case 1: myCurve->D1(myU, myPnt, myDerivArr[0]); break;
    case 2: myCurve->D2(myU, myPnt, myDerivArr[0], myDerivArr[1]); break;
    case 3: myCurve->D3(myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]); break;
  }
  myTangentStatus = LProp_Undecided;
}

void GeomLProp_LegacyCLProps::SetCurve(const occ::handle<Geom_Curve>& theCurve)
{
  myCurve = theCurve;
  myCN    = continuity(theCurve);
}

const gp_Pnt& GeomLProp_LegacyCLProps::Value() const { return myPnt; }

const gp_Vec& GeomLProp_LegacyCLProps::D1()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    myCurve->D1(myU, myPnt, myDerivArr[0]);
  }
  return myDerivArr[0];
}

const gp_Vec& GeomLProp_LegacyCLProps::D2()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    myCurve->D2(myU, myPnt, myDerivArr[0], myDerivArr[1]);
  }
  return myDerivArr[1];
}

const gp_Vec& GeomLProp_LegacyCLProps::D3()
{
  if (myDerOrder < 3)
  {
    myDerOrder = 3;
    myCurve->D3(myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
  }
  return myDerivArr[2];
}

bool GeomLProp_LegacyCLProps::IsTangentDefined()
{
  if (myTangentStatus == LProp_Undefined)
    return false;
  if (myTangentStatus >= LProp_Defined)
    return true;

  const double aTol = myLinTol * myLinTol;
  gp_Vec       aVec;
  int          anOrder = 0;
  while (anOrder++ < 4)
  {
    if (myCN >= anOrder)
    {
      switch (anOrder)
      {
        case 1: aVec = D1(); break;
        case 2: aVec = D2(); break;
        case 3: aVec = D3(); break;
      }
      if (aVec.SquareMagnitude() > aTol)
      {
        mySignificantFirstDerivativeOrder = anOrder;
        myTangentStatus                   = LProp_Defined;
        return true;
      }
    }
    else
    {
      myTangentStatus = LProp_Undefined;
      return false;
    }
  }
  return false;
}

void GeomLProp_LegacyCLProps::Tangent(gp_Dir& theDir)
{
  if (!IsTangentDefined())
    throw LProp_NotDefined();

  if (mySignificantFirstDerivativeOrder == 1)
  {
    theDir = gp_Dir(myDerivArr[0]);
    return;
  }

  const double aFirst = myCurve->FirstParameter();
  const double aLast  = myCurve->LastParameter();
  const double aRange = (aLast >= RealLast() || aFirst <= RealFirst()) ? 0.0 : aLast - aFirst;
  const double aDelta = std::max(aRange * 1.0e-3, THE_MIN_STEP);
  double       anOther = myU - aFirst < aDelta ? myU + aDelta : myU - aDelta;
  gp_Pnt       aP1;
  gp_Pnt       aP2;
  myCurve->D0(std::min(myU, anOther), aP1);
  myCurve->D0(std::max(myU, anOther), aP2);
  gp_Vec aVec = myDerivArr[mySignificantFirstDerivativeOrder - 1];
  if (aVec.Dot(gp_Vec(aP1, aP2)) < 0.0)
    aVec = -aVec;
  theDir = gp_Dir(aVec);
}

double GeomLProp_LegacyCLProps::Curvature()
{
  const bool isDefined = IsTangentDefined();
  (void)isDefined;
  LProp_NotDefined_Raise_if(!isDefined, "GeomLProp_LegacyCLProps::Curvature()");
  if (mySignificantFirstDerivativeOrder > 1)
    return RealLast();

  const double aTol = myLinTol * myLinTol;
  const double aDD1 = myDerivArr[0].SquareMagnitude();
  const double aDD2 = myDerivArr[1].SquareMagnitude();
  if (aDD2 <= aTol)
  {
    myCurvature = 0.0;
  }
  else
  {
    const double aCross = myDerivArr[0].CrossSquareMagnitude(myDerivArr[1]);
    if (aCross / aDD1 / aDD2 <= aTol)
    {
      myCurvature = 0.0;
    }
    else
    {
      myCurvature = std::sqrt(aCross) / aDD1 / std::sqrt(aDD1);
    }
  }
  return myCurvature;
}

void GeomLProp_LegacyCLProps::Normal(gp_Dir& theDir)
{
  const double aCurv = Curvature();
  if (aCurv == RealLast() || std::abs(aCurv) <= myLinTol)
    throw LProp_NotDefined("GeomLProp_LegacyCLProps::Normal()");
  gp_Vec aNorm = myDerivArr[1] * (myDerivArr[0] * myDerivArr[0])
                 - myDerivArr[0] * (myDerivArr[0] * myDerivArr[1]);
  theDir = gp_Dir(aNorm);
}

void GeomLProp_LegacyCLProps::CentreOfCurvature(gp_Pnt& thePoint)
{
  if (std::abs(Curvature()) <= myLinTol)
    throw LProp_NotDefined();
  gp_Vec aNorm = myDerivArr[1] * (myDerivArr[0] * myDerivArr[0])
                 - myDerivArr[0] * (myDerivArr[0] * myDerivArr[1]);
  aNorm.Normalize();
  aNorm.Divide(myCurvature);
  thePoint = myPnt.Translated(aNorm);
}
