// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <AppParCurves_MultiBSpCurve.ixx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <AppParCurves_HArray1OfMultiPoint.hxx>
#include <BSplCLib.hxx>

//=======================================================================
//function : ComputeDegree
//purpose  : 
//=======================================================================

static Standard_Integer ComputeDegree(const TColStd_Array1OfInteger& mults,
				      const Standard_Integer nbPoles)
{
  Standard_Integer i, sum = 0;
  for (i = mults.Lower(); i <= mults.Upper(); i++) {
    sum += mults(i);
  }
  return sum - nbPoles -1;
}

//=======================================================================
//function : AppParCurves_MultiBSpCurve
//purpose  : 
//=======================================================================

AppParCurves_MultiBSpCurve::AppParCurves_MultiBSpCurve() {}


//=======================================================================
//function : AppParCurves_MultiBSpCurve
//purpose  : 
//=======================================================================

AppParCurves_MultiBSpCurve::AppParCurves_MultiBSpCurve
  (const Standard_Integer NbPol): 
  AppParCurves_MultiCurve(NbPol)
{
}



//=======================================================================
//function : AppParCurves_MultiBSpCurve
//purpose  : 
//=======================================================================

AppParCurves_MultiBSpCurve::AppParCurves_MultiBSpCurve
  (const AppParCurves_Array1OfMultiPoint& tabMU,
   const TColStd_Array1OfReal& Knots,
   const TColStd_Array1OfInteger& Mults):
  AppParCurves_MultiCurve(tabMU)
{
  myknots = new TColStd_HArray1OfReal(Knots.Lower(), Knots.Upper());
  myknots->ChangeArray1() = Knots;
  mymults = new TColStd_HArray1OfInteger(Mults.Lower(), Mults.Upper());
  mymults->ChangeArray1() = Mults;
  myDegree = ComputeDegree(Mults,NbPoles());
}


//=======================================================================
//function : AppParCurves_MultiBSpCurve
//purpose  : 
//=======================================================================

AppParCurves_MultiBSpCurve::AppParCurves_MultiBSpCurve
  (const AppParCurves_MultiCurve& SC,
   const TColStd_Array1OfReal& Knots,
   const TColStd_Array1OfInteger& Mults):
  AppParCurves_MultiCurve(SC)
{
  myknots = new TColStd_HArray1OfReal(Knots.Lower(), Knots.Upper());
  myknots->ChangeArray1() = Knots;
  mymults = new TColStd_HArray1OfInteger(Mults.Lower(), Mults.Upper());
  mymults->ChangeArray1() = Mults;
  myDegree = ComputeDegree(Mults,NbPoles());
}


//=======================================================================
//function : SetKnots
//purpose  : 
//=======================================================================

void AppParCurves_MultiBSpCurve::SetKnots(const TColStd_Array1OfReal& theKnots)
{
  myknots = new TColStd_HArray1OfReal(theKnots.Lower(), theKnots.Upper());
  myknots->ChangeArray1() = theKnots;
}

//=======================================================================
//function : SetMultiplicities
//purpose  : 
//=======================================================================

void AppParCurves_MultiBSpCurve::SetMultiplicities(const TColStd_Array1OfInteger& theMults)
{
  mymults = new TColStd_HArray1OfInteger(theMults.Lower(), theMults.Upper());
  mymults->ChangeArray1() = theMults;
  myDegree = ComputeDegree(theMults,NbPoles());
}


//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

const TColStd_Array1OfReal& AppParCurves_MultiBSpCurve::Knots() const
{
  return myknots->Array1();
}

//=======================================================================
//function : Multiplicities
//purpose  : 
//=======================================================================

const TColStd_Array1OfInteger& AppParCurves_MultiBSpCurve::Multiplicities() const
{
  return mymults->Array1();
}


//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer AppParCurves_MultiBSpCurve::Degree() const 
{
  return myDegree;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void AppParCurves_MultiBSpCurve::Value (const Standard_Integer CuIndex, 
			      const Standard_Real U, gp_Pnt& Pt) const {

  if (Dimension(CuIndex) != 3) {
    Standard_OutOfRange::Raise();
  }

  TColgp_Array1OfPnt TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D0(U,0,myDegree,Standard_False,TabPoles,BSplCLib::NoWeights(),
	       myknots->Array1(),mymults->Array1(),Pt);
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void AppParCurves_MultiBSpCurve::Value (const Standard_Integer CuIndex, 
			      const Standard_Real U, gp_Pnt2d& Pt) const {

  if (Dimension(CuIndex) != 2) {
    Standard_OutOfRange::Raise();
  }

  TColgp_Array1OfPnt2d TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);
  
  BSplCLib::D0(U,0,myDegree,Standard_False,TabPoles,BSplCLib::NoWeights(),
	       myknots->Array1(),mymults->Array1(),Pt);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void AppParCurves_MultiBSpCurve::D1 (const Standard_Integer CuIndex, 
			   const Standard_Real U, gp_Pnt& Pt, gp_Vec& V1) const {
  if (Dimension(CuIndex) != 3) {
    Standard_OutOfRange::Raise();
  }

  TColgp_Array1OfPnt TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D1(U,0,myDegree,Standard_False,TabPoles,BSplCLib::NoWeights(),
	       myknots->Array1(),mymults->Array1(),Pt,V1);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void AppParCurves_MultiBSpCurve::D2 (const Standard_Integer CuIndex, 
			    const Standard_Real U, 
			    gp_Pnt& Pt,
			    gp_Vec& V1,
			    gp_Vec& V2) const {
  if (Dimension(CuIndex) != 3) {
    Standard_OutOfRange::Raise();
  }

  TColgp_Array1OfPnt TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D2(U,0,myDegree,Standard_False,TabPoles,BSplCLib::NoWeights(),
	       myknots->Array1(),mymults->Array1(),Pt,V1,V2);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void AppParCurves_MultiBSpCurve::D1 (const Standard_Integer CuIndex,
		       const Standard_Real U, gp_Pnt2d& Pt, gp_Vec2d& V1) const {
  if (Dimension(CuIndex) != 2) {
    Standard_OutOfRange::Raise();
  }

  TColgp_Array1OfPnt2d TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D1(U,0,myDegree,Standard_False,TabPoles,BSplCLib::NoWeights(),
	       myknots->Array1(),mymults->Array1(),Pt,V1);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void AppParCurves_MultiBSpCurve::D2 (const Standard_Integer CuIndex,
			    const Standard_Real U, 
			    gp_Pnt2d& Pt, 
			    gp_Vec2d& V1,
			    gp_Vec2d& V2) const {
  if (Dimension(CuIndex) != 2) {
    Standard_OutOfRange::Raise();
  }

  TColgp_Array1OfPnt2d TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D2(U,0,myDegree,Standard_False,TabPoles,BSplCLib::NoWeights(),
	       myknots->Array1(),mymults->Array1(),Pt,V1,V2);
}



//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void AppParCurves_MultiBSpCurve::Dump(Standard_OStream& o) const
{
  o << "AppParCurves_MultiBSpCurve dump:" << endl;
  o << " It contains " << NbCurves() << " BSpline curves "<< endl;
  o << " The poles are: " << endl;
/*  for (Standard_Integer i = 1; i <= NbCurves(); i++) {
    o << " Curve No. " << i << endl;
    if (Dimension(i) == 3) {
      for (Standard_Integer j = 1; j <= tabPoint->Length(); j++) {
	o << " Pole No. " << j << ": " << endl;
	o << " Pole x = " << (tabPoint->Value(j)->Point(i)).X() << endl;
	o << " Pole y = " << (tabPoint->Value(j)->Point(i)).Y() << endl;
	o << " Pole z = " << (tabPoint->Value(j)->Point(i)).Z() << endl;
      }
    }
    else {
      for (Standard_Integer j = 1; j <= tabPoint->Length(); j++) {
	o << " Pole No. " << j << ": " << endl;
	o << " Pole x = " << (tabPoint->Value(j)->Point2d(i)).X() << endl;
	o << " Pole y = " << (tabPoint->Value(j)->Point2d(i)).Y() << endl;
      }
    }
  }
*/
}



