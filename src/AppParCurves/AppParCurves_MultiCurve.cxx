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


#include <AppParCurves_MultiCurve.hxx>
#include <AppParCurves_MultiPoint.hxx>
#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <PLib.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

AppParCurves_MultiCurve::AppParCurves_MultiCurve() {}


AppParCurves_MultiCurve::AppParCurves_MultiCurve (const Standard_Integer NbPol) 
{
  tabPoint = new AppParCurves_HArray1OfMultiPoint(1, NbPol);
}



AppParCurves_MultiCurve::AppParCurves_MultiCurve (const AppParCurves_Array1OfMultiPoint& tabMU)
{
  tabPoint = new AppParCurves_HArray1OfMultiPoint(1, tabMU.Length());
  Standard_Integer i, Lower = tabMU.Lower();
  for (i = 1; i <= tabMU.Length(); i++) {
    tabPoint->SetValue(i, tabMU.Value(Lower+i-1));
  }

}

AppParCurves_MultiCurve::~AppParCurves_MultiCurve()
{
}

Standard_Integer AppParCurves_MultiCurve::Dimension (const Standard_Integer Index) const {
  Standard_Integer Lo = tabPoint->Lower();
  Standard_Integer nb = tabPoint->Value(Lo).NbPoints() + tabPoint->Value(Lo).NbPoints2d();
  if ((Index <= 0) || (Index > nb)) {
    throw Standard_OutOfRange();
  }
  return tabPoint->Value(Lo).Dimension(Index);
}


Standard_Integer AppParCurves_MultiCurve::NbCurves () const {
  if (tabPoint.IsNull())
    return 0;
  AppParCurves_MultiPoint MP = tabPoint->Value(1);
  return MP.NbPoints() + MP.NbPoints2d();
}


Standard_Integer AppParCurves_MultiCurve::NbPoles() const {
  if (tabPoint.IsNull())
    return 0;
  return tabPoint->Length();
}


Standard_Integer AppParCurves_MultiCurve::Degree() const {
  return tabPoint->Length()-1;
}


void AppParCurves_MultiCurve::SetNbPoles(const Standard_Integer nbPoles) 
{
  tabPoint = new AppParCurves_HArray1OfMultiPoint(1, nbPoles);
}


void AppParCurves_MultiCurve::SetValue (const Standard_Integer Index, 
			          const AppParCurves_MultiPoint& MPoint) {

  if ((Index <= 0) || (Index > tabPoint->Length())) {
    throw Standard_OutOfRange();
  }
  tabPoint->SetValue(Index, MPoint);
}


void AppParCurves_MultiCurve::Curve (const Standard_Integer CuIndex,
			       TColgp_Array1OfPnt& TabPnt) const {
  if ((CuIndex <= 0)) {
    throw Standard_OutOfRange();
  }
  for ( Standard_Integer i = 1; i <= tabPoint->Length(); i++) {
    TabPnt(i) = tabPoint->Value(i).Point(CuIndex);
  }
}


void AppParCurves_MultiCurve::Curve (const Standard_Integer CuIndex,
			       TColgp_Array1OfPnt2d& TabPnt2d) const {
  if ((CuIndex <= 0)) {
    throw Standard_OutOfRange();
  }
  for ( Standard_Integer i = 1; i <= tabPoint->Length(); i++) {
    TabPnt2d(i) = tabPoint->Value(i).Point2d(CuIndex);
  }
}



const gp_Pnt& AppParCurves_MultiCurve::Pole(const Standard_Integer CuIndex,
					    const Standard_Integer Nieme) const
{
  if ((CuIndex <= 0) && Nieme <= 0) {
    throw Standard_OutOfRange();
  }
  return tabPoint->Value(Nieme).Point(CuIndex);
}

const gp_Pnt2d& AppParCurves_MultiCurve::Pole2d(const Standard_Integer CuIndex,
					    const Standard_Integer Nieme)const
{
  if ((CuIndex <= 0) && Nieme <= 0) {
    throw Standard_OutOfRange();
  }
  return tabPoint->Value(Nieme).Point2d(CuIndex);
}



const AppParCurves_MultiPoint& AppParCurves_MultiCurve::Value (const Standard_Integer Index) const {
  if ((Index <= 0) || (Index > tabPoint->Length())) {
    throw Standard_OutOfRange();
  }
  return tabPoint->Value(Index);
}

void AppParCurves_MultiCurve::Transform(const Standard_Integer CuIndex,
					const Standard_Real    x,
					const Standard_Real    dx,
					const Standard_Real    y,
					const Standard_Real    dy,
					const Standard_Real    z,
					const Standard_Real    dz) 
{
  if (Dimension(CuIndex) != 3) throw Standard_OutOfRange();

  for (Standard_Integer i = 1 ; i <= tabPoint->Length(); i++) {
    (tabPoint->ChangeValue(i)).Transform(CuIndex, x, dx, y, dy, z, dz);
  } 
}

void AppParCurves_MultiCurve::Transform2d(const Standard_Integer CuIndex,
					  const Standard_Real    x,
					  const Standard_Real    dx,
					  const Standard_Real    y,
					  const Standard_Real    dy) 
{
  if (Dimension(CuIndex) != 2) throw Standard_OutOfRange();

  for (Standard_Integer i = 1 ; i <= tabPoint->Length(); i++) {
    (tabPoint->ChangeValue(i)).Transform2d(CuIndex, x, dx, y, dy);
  } 
}


void AppParCurves_MultiCurve::Value (const Standard_Integer CuIndex, 
			      const Standard_Real U, gp_Pnt& Pt) const {
  
  if (Dimension(CuIndex) != 3)throw Standard_OutOfRange();

  TColgp_Array1OfPnt TabPoles(1, tabPoint->Length());
  
  for (Standard_Integer i =1 ; i <= tabPoint->Length(); i++) {
    TabPoles(i) = tabPoint->Value(i).Point(CuIndex);
  } 
  
  BSplCLib::D0 (U, TabPoles,BSplCLib::NoWeights(), Pt);
}


void AppParCurves_MultiCurve::Value (const Standard_Integer CuIndex, 
			      const Standard_Real U, gp_Pnt2d& Pt) const {
  if (Dimension(CuIndex) != 2) {
    throw Standard_OutOfRange();
  }

  TColgp_Array1OfPnt2d TabPole(1, tabPoint->Length());

  for (Standard_Integer i =1 ; i <= tabPoint->Length(); i++) {
    TabPole(i) = tabPoint->Value(i).Point2d(CuIndex);
  }
  
  BSplCLib::D0 (U, TabPole, BSplCLib::NoWeights(), Pt);
}


void AppParCurves_MultiCurve::D1 (const Standard_Integer CuIndex, 
				  const Standard_Real U, 
				  gp_Pnt& Pt, 
				  gp_Vec& V1) const {

  if (Dimension(CuIndex) != 3) {
    throw Standard_OutOfRange();
  }

  TColgp_Array1OfPnt TabPole(1, tabPoint->Length());

  for (Standard_Integer i =1 ; i <= tabPoint->Length(); i++) {
    TabPole(i) = tabPoint->Value(i).Point(CuIndex);
  }

  BSplCLib::D1 (U, TabPole, BSplCLib::NoWeights(), Pt, V1);
}


void AppParCurves_MultiCurve::D2 (const Standard_Integer CuIndex, 
			    const Standard_Real U, 
			    gp_Pnt& Pt,
			    gp_Vec& V1,
			    gp_Vec& V2) const {

  if (Dimension(CuIndex) != 3) {
    throw Standard_OutOfRange();
  }

  TColgp_Array1OfPnt TabPole(1, tabPoint->Length());

  for (Standard_Integer i =1 ; i <= tabPoint->Length(); i++) {
    TabPole(i) = tabPoint->Value(i).Point(CuIndex);
  }

  BSplCLib::D2 (U, TabPole, BSplCLib::NoWeights(), Pt, V1, V2);
}


void AppParCurves_MultiCurve::D1 (const Standard_Integer CuIndex,
		       const Standard_Real U, gp_Pnt2d& Pt, gp_Vec2d& V1) const {

  if (Dimension(CuIndex) != 2) {
    throw Standard_OutOfRange();
  }

  TColgp_Array1OfPnt2d TabPole(1, tabPoint->Length());

  for (Standard_Integer i =1 ; i <= tabPoint->Length(); i++) {
    TabPole(i) = tabPoint->Value(i).Point2d(CuIndex);
  }

  BSplCLib::D1 (U, TabPole, BSplCLib::NoWeights(), Pt, V1);
}


void AppParCurves_MultiCurve::D2 (const Standard_Integer CuIndex,
			    const Standard_Real U, 
			    gp_Pnt2d& Pt, 
			    gp_Vec2d& V1,
			    gp_Vec2d& V2) const {

  if (Dimension(CuIndex) != 2) {
    throw Standard_OutOfRange();
  }

  TColgp_Array1OfPnt2d TabPole(1, tabPoint->Length());

  for (Standard_Integer i =1 ; i <= tabPoint->Length(); i++) {
    TabPole(i) = tabPoint->Value(i).Point2d(CuIndex);
  }

  BSplCLib::D2(U, TabPole, BSplCLib::NoWeights(), Pt, V1, V2);
}



void AppParCurves_MultiCurve::Dump(Standard_OStream& o) const
{
  o << "AppParCurves_MultiCurve dump:" << endl;
  o << " It contains " << NbCurves() << " Bezier curves of degree " << tabPoint->Length()-1 << endl;
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
