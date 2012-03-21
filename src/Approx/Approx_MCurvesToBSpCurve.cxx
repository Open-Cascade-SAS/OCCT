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

#include <Approx_MCurvesToBSpCurve.ixx>
#include <Convert_CompBezierCurvesToBSplineCurve.hxx>
#include <Convert_CompBezierCurves2dToBSplineCurve2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <AppParCurves_MultiPoint.hxx>
#include <AppParCurves_Array1OfMultiPoint.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <BSplCLib.hxx>
#include <PLib.hxx>

#ifdef DEB
static void DEBUG(const AppParCurves_MultiCurve& MC) {
  Standard_Integer i, j;
  Standard_Integer nbcu = MC.NbCurves();
  Standard_Integer nbpoles = MC.NbPoles();
  TColgp_Array1OfPnt Poles(1, nbpoles);
  TColgp_Array1OfPnt2d Poles2d(1, nbpoles);

  for (i = 1; i <= nbcu; i++) {
    cout << " Curve No. " << i << endl;
    if (MC.Dimension(i) == 3) {
      MC.Curve(i, Poles);
      for (j = 1; j <= nbpoles; j++) {
	cout<< " Pole = " << Poles(j).X() <<" "<<Poles(j).Y()<<" "<<Poles(j).Z()<< endl;
      }
    }
    else {
      MC.Curve(i, Poles2d);
      for (j = 1; j <= nbpoles; j++) {
	cout<< " Pole = " << Poles2d(j).X() <<" "<<Poles2d(j).Y()<< endl;
      }
    }
  }

}
#endif



Approx_MCurvesToBSpCurve::Approx_MCurvesToBSpCurve()
{
  myDone = Standard_False;
}

void Approx_MCurvesToBSpCurve::Reset()
{
  myDone = Standard_False;
  myCurves.Clear();
}

void Approx_MCurvesToBSpCurve::Append(const AppParCurves_MultiCurve& MC)
{
  myCurves.Append(MC);
}


void Approx_MCurvesToBSpCurve::Perform()
{
  Perform(myCurves);
}

void Approx_MCurvesToBSpCurve::Perform
  (const AppParCurves_SequenceOfMultiCurve& TheSeq)
{

  Standard_Integer i, j, deg=0;
  Standard_Integer nbcu = TheSeq.Length();
  AppParCurves_MultiCurve CU;
  Standard_Integer nbpolesspl=0, nbknots=0;
#ifdef DEB
  Standard_Boolean debug = Standard_False;
#endif  

  if (nbcu == 1) {
    CU = TheSeq.Value(1);
    deg = CU.Degree();
    TColStd_Array1OfReal Knots(1, 2);
    TColStd_Array1OfInteger Mults(1, 2);
    Knots(1) = 0.0;
    Knots(2) = 1.0;
    Mults(1) = Mults(2) = deg+1;
    mySpline = AppParCurves_MultiBSpCurve (CU, Knots, Mults);
  }
  else {

    AppParCurves_MultiPoint P = TheSeq.Value(nbcu).Value(1);
    Standard_Integer nb3d = P.NbPoints();
    Standard_Integer nb2d = P.NbPoints2d();

    Convert_CompBezierCurvesToBSplineCurve conv;
    Convert_CompBezierCurves2dToBSplineCurve2d conv2d;

    if (nb3d != 0) {
      for (i = 1; i <= nbcu; i++) {
	CU = TheSeq.Value(i);
	TColgp_Array1OfPnt ThePoles3d(1, CU.NbPoles());
	CU.Curve(1, ThePoles3d);
	conv.AddCurve(ThePoles3d);
      }
      conv.Perform();
    }

    
    else if (nb2d != 0) {
      for (i = 1; i <= nbcu; i++) {
	CU = TheSeq.Value(i);
	TColgp_Array1OfPnt2d ThePoles2d(1, CU.NbPoles());
	CU.Curve(1+nb3d, ThePoles2d);
	conv2d.AddCurve(ThePoles2d);
      }
      conv2d.Perform();
    }
    
    
    // Recuperation:
    if (nb3d != 0) {
      nbpolesspl = conv.NbPoles();
      nbknots = conv.NbKnots();
    }
    else if (nb2d != 0) {
      nbpolesspl = conv2d.NbPoles();
      nbknots = conv2d.NbKnots();
    }    
    
    AppParCurves_Array1OfMultiPoint tabMU(1, nbpolesspl);
    TColgp_Array1OfPnt PolesSpl(1, nbpolesspl);
    TColgp_Array1OfPnt2d PolesSpl2d(1, nbpolesspl);
    TColStd_Array1OfInteger TheMults(1, nbknots);
    TColStd_Array1OfReal TheKnots(1, nbknots);
    
    if (nb3d != 0) {
      conv.KnotsAndMults(TheKnots, TheMults);
      conv.Poles(PolesSpl);
      deg = conv.Degree();
    }
    else if (nb2d != 0) {
      conv2d.KnotsAndMults(TheKnots, TheMults);
      conv2d.Poles(PolesSpl2d);
      deg = conv2d.Degree();
    }


    for (j = 1; j <= nbpolesspl; j++) {
      AppParCurves_MultiPoint MP(nb3d, nb2d);
      if (nb3d!=0) {
	MP.SetPoint(1, PolesSpl(j));
      }
      else if (nb2d!=0) {
	MP.SetPoint2d(1+nb3d, PolesSpl2d(j));
      }
      tabMU.SetValue(j, MP);
    }

    Standard_Integer kpol = 1, kpoles3d=1, kpoles2d=1;
    Standard_Integer mydegre, k;
    Standard_Integer first, last, Inc, thefirst;
    if (nb3d != 0) thefirst = 1;
    else thefirst = 2;

    for (i = 1; i <= nbcu; i++) {
      CU = TheSeq.Value(i);
      mydegre = CU.Degree();
      if (TheMults(i+1) == deg) last = deg+1;      // Continuite C0
      else last = deg;                             // Continuite C1
      if (i==nbcu) {last = deg+1;}
      first = 1;
      if (i==1) first = 1;
      else if ((TheMults(i)== deg-1) || (TheMults(i)==deg)) first = 2;

      for (j = 2; j <= nb3d; j++) {
	kpol = kpoles3d;
	TColgp_Array1OfPnt ThePoles(1, CU.NbPoles());
	CU.Curve(j, ThePoles);

	Inc = deg-mydegre;
	TColgp_Array1OfPnt Points(1, deg+1);
	if (Inc > 0) {
	  BSplCLib::IncreaseDegree(deg, ThePoles, PLib::NoWeights(),
				   Points, PLib::NoWeights());
	}
	else {
	  Points = ThePoles;
	}

	for (k = first; k <= last; k++) {
	  tabMU.ChangeValue(kpol++).SetPoint(j, Points(k));
	}
      }
      kpoles3d = kpol;

      for (j = thefirst; j <= nb2d; j++) {
	kpol = kpoles2d;
	TColgp_Array1OfPnt2d ThePoles2d(1, CU.NbPoles());
	CU.Curve(j+nb3d, ThePoles2d);
	
	Inc = deg-mydegre;
	TColgp_Array1OfPnt2d Points2d(1, deg+1);
	if (Inc > 0) {
	  BSplCLib::IncreaseDegree(deg, ThePoles2d, PLib::NoWeights(),
				   Points2d, PLib::NoWeights());
	}
	else {
	  Points2d = ThePoles2d;
	}
	for (k = first; k <= last; k++) {
	  tabMU.ChangeValue(kpol++).SetPoint2d(j+nb3d, Points2d(k));
	}
      }
      kpoles2d = kpol;
    }
    
    mySpline = AppParCurves_MultiBSpCurve(tabMU, TheKnots, TheMults);
  }
#ifdef DEB
if(debug) DEBUG(mySpline);
#endif

  myDone = Standard_True;
}


const AppParCurves_MultiBSpCurve& Approx_MCurvesToBSpCurve::Value() const
{
  return mySpline;
}


const AppParCurves_MultiBSpCurve& Approx_MCurvesToBSpCurve::ChangeValue()
{
  return mySpline;
}


