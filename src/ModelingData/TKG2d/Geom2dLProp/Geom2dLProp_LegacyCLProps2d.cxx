#include <Geom2dLProp_LegacyCLProps2d.hxx>

#include <Geom2d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <LProp_NotDefined.hxx>
#include <Standard_OutOfRange.hxx>

#include <algorithm>
#include <cmath>

namespace
{
constexpr double THE_MIN_STEP = 1.0e-7;
static int continuity(const occ::handle<Geom2d_Curve>& theCurve)
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

Geom2dLProp_LegacyCLProps2d::Geom2dLProp_LegacyCLProps2d(const occ::handle<Geom2d_Curve>& theCurve,
                                                         double                           theParam,
                                                         int                              theDerOrder,
                                                         double                           theResolution)
    : myCurve(theCurve),
      myU(theParam),
      myDerOrder(theDerOrder),
      myCN(continuity(theCurve)),
      myLinTol(theResolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(theDerOrder < 0 || theDerOrder > 3, "Geom2dLProp_LegacyCLProps2d()");
  switch (myDerOrder)
  {
    case 0: myPnt = myCurve->Value(myU); break;
    case 1: myCurve->D1(myU, myPnt, myDerivArr[0]); break;
    case 2: myCurve->D2(myU, myPnt, myDerivArr[0], myDerivArr[1]); break;
    case 3: myCurve->D3(myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]); break;
  }
}

const gp_Pnt2d& Geom2dLProp_LegacyCLProps2d::Value() const { return myPnt; }
const gp_Vec2d& Geom2dLProp_LegacyCLProps2d::D1() { if (myDerOrder < 1) { myDerOrder = 1; myCurve->D1(myU, myPnt, myDerivArr[0]); } return myDerivArr[0]; }
const gp_Vec2d& Geom2dLProp_LegacyCLProps2d::D2() { if (myDerOrder < 2) { myDerOrder = 2; myCurve->D2(myU, myPnt, myDerivArr[0], myDerivArr[1]); } return myDerivArr[1]; }
const gp_Vec2d& Geom2dLProp_LegacyCLProps2d::D3() { if (myDerOrder < 3) { myDerOrder = 3; myCurve->D3(myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]); } return myDerivArr[2]; }
bool Geom2dLProp_LegacyCLProps2d::IsTangentDefined()
{
  if (myTangentStatus == LProp_Undefined) return false;
  if (myTangentStatus >= LProp_Defined) return true;
  const double aTol = myLinTol * myLinTol;
  for (int anOrder = 1; anOrder < 4; ++anOrder)
  {
    if (myCN < anOrder) { myTangentStatus = LProp_Undefined; return false; }
    const gp_Vec2d& aVec = anOrder == 1 ? D1() : (anOrder == 2 ? D2() : D3());
    if (aVec.SquareMagnitude() > aTol)
    {
      mySignificantFirstDerivativeOrder = anOrder;
      myTangentStatus = LProp_Defined;
      return true;
    }
  }
  return false;
}
void Geom2dLProp_LegacyCLProps2d::Tangent(gp_Dir2d& theDir)
{
  if (!IsTangentDefined()) throw LProp_NotDefined();
  if (mySignificantFirstDerivativeOrder == 1) { theDir = gp_Dir2d(myDerivArr[0]); return; }
  const double aFirst = myCurve->FirstParameter(), aLast = myCurve->LastParameter();
  const double aRange = (aLast >= RealLast() || aFirst <= RealFirst()) ? 0.0 : aLast - aFirst;
  const double aDelta = std::max(aRange * 1.0e-3, THE_MIN_STEP);
  const double anOther = myU - aFirst < aDelta ? myU + aDelta : myU - aDelta;
  gp_Vec2d aVec = myDerivArr[mySignificantFirstDerivativeOrder - 1];
  if (aVec.Dot(gp_Vec2d(myCurve->Value(std::min(myU, anOther)), myCurve->Value(std::max(myU, anOther)))) < 0.0) aVec = -aVec;
  theDir = gp_Dir2d(aVec);
}
double Geom2dLProp_LegacyCLProps2d::Curvature()
{
  const bool isDefined = IsTangentDefined(); (void)isDefined;
  LProp_NotDefined_Raise_if(!isDefined, "Geom2dLProp_LegacyCLProps2d::Curvature()");
  if (mySignificantFirstDerivativeOrder > 1) return RealLast();
  const double aTol = myLinTol * myLinTol;
  const double aDD1 = myDerivArr[0].SquareMagnitude();
  const double aDD2 = myDerivArr[1].SquareMagnitude();
  if (aDD2 <= aTol) myCurvature = 0.0;
  else
  {
    const double aCross = myDerivArr[0].Crossed(myDerivArr[1]);
    const double aTest = aCross * aCross / aDD1 / aDD2;
    myCurvature = aTest <= aTol ? 0.0 : aCross / aDD1 / std::sqrt(aDD1);
  }
  return myCurvature;
}
void Geom2dLProp_LegacyCLProps2d::Normal(gp_Dir2d& theDir)
{
  const double aCurv = Curvature();
  if (aCurv == RealLast() || std::abs(aCurv) <= myLinTol) throw LProp_NotDefined();
  gp_Vec2d aNorm(-myDerivArr[0].Y(), myDerivArr[0].X());
  if (aNorm.Dot(myDerivArr[1]) < 0.0) aNorm = -aNorm;
  theDir = gp_Dir2d(aNorm);
}
void Geom2dLProp_LegacyCLProps2d::CentreOfCurvature(gp_Pnt2d& thePoint)
{
  if (std::abs(Curvature()) <= myLinTol) throw LProp_NotDefined();
  gp_Dir2d aNorm;
  Normal(aNorm);
  gp_Vec2d aShift(aNorm);
  aShift.Divide(myCurvature);
  thePoint = myPnt.Translated(aShift);
}
