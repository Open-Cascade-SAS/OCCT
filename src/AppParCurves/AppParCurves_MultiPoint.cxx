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


#include <AppParCurves_MultiPoint.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Transient.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>

#define tabPoint   Handle(TColgp_HArray1OfPnt)::DownCast (ttabPoint)
#define tabPoint2d Handle(TColgp_HArray1OfPnt2d)::DownCast (ttabPoint2d)

AppParCurves_MultiPoint::AppParCurves_MultiPoint() {}


AppParCurves_MultiPoint::AppParCurves_MultiPoint (const Standard_Integer NbPoles, 
						  const Standard_Integer NbPoles2d)
{
  nbP = NbPoles;
  nbP2d = NbPoles2d;
  if (nbP != 0)  {
    Handle(TColgp_HArray1OfPnt) tab3d = 
      new TColgp_HArray1OfPnt(1, NbPoles);
    ttabPoint = tab3d;
  }
  if (nbP2d != 0) {
    Handle(TColgp_HArray1OfPnt2d) tab2d = 
      new TColgp_HArray1OfPnt2d(1, NbPoles2d);
    ttabPoint2d = tab2d;
  }
}



AppParCurves_MultiPoint::AppParCurves_MultiPoint(const TColgp_Array1OfPnt& tabP)
{
  nbP2d = 0;
  nbP = tabP.Length();
  Handle(TColgp_HArray1OfPnt) tab3d = 
    new TColgp_HArray1OfPnt(1, nbP);
  ttabPoint = tab3d;
  Standard_Integer Lower = tabP.Lower();
  TColgp_Array1OfPnt& P3d = tabPoint->ChangeArray1();
  for (Standard_Integer i = 1; i <= tabP.Length(); i++) {
    P3d.SetValue(i, tabP.Value(Lower+i-1));
  }
}



AppParCurves_MultiPoint::AppParCurves_MultiPoint(const TColgp_Array1OfPnt2d& tabP2d)
{
  nbP = 0;
  nbP2d = tabP2d.Length();
  Handle(TColgp_HArray1OfPnt2d) tab2d = 
    new TColgp_HArray1OfPnt2d(1, nbP2d);
  ttabPoint2d = tab2d;
  Standard_Integer Lower = tabP2d.Lower();
  TColgp_Array1OfPnt2d& P2d = tabPoint2d->ChangeArray1();
  for (Standard_Integer i = 1; i <= nbP2d; i++) {
    P2d.SetValue(i, tabP2d.Value(Lower+i-1));
  }
}


AppParCurves_MultiPoint::AppParCurves_MultiPoint(const TColgp_Array1OfPnt&   tabP,
						 const TColgp_Array1OfPnt2d& tabP2d)
{
  nbP = tabP.Length();
  nbP2d = tabP2d.Length();
  Handle(TColgp_HArray1OfPnt) t3d = 
    new TColgp_HArray1OfPnt(1, nbP);
  ttabPoint = t3d;

  Handle(TColgp_HArray1OfPnt2d) t2d = 
    new TColgp_HArray1OfPnt2d(1, nbP2d);
  ttabPoint2d = t2d;

  TColgp_Array1OfPnt& P3d = tabPoint->ChangeArray1();
  Standard_Integer i, Lower = tabP.Lower();
  for (i = 1; i <= nbP; i++) {
    P3d.SetValue(i, tabP.Value(Lower+i-1));
  }
  Lower = tabP2d.Lower();
  TColgp_Array1OfPnt2d& P2d = tabPoint2d->ChangeArray1();
  for (i = 1; i <= nbP2d; i++) {
    P2d.SetValue(i, tabP2d.Value(Lower+i-1));
  }
}

AppParCurves_MultiPoint::~AppParCurves_MultiPoint()
{
}

void AppParCurves_MultiPoint::Transform(const Standard_Integer CuIndex,
					const Standard_Real    x,
					const Standard_Real    dx,
					const Standard_Real    y,
					const Standard_Real    dy,
					const Standard_Real    z,
					const Standard_Real    dz) 
{
  if (Dimension(CuIndex) != 3) throw Standard_OutOfRange();

  gp_Pnt P, newP;
  P = Point(CuIndex);
  newP.SetCoord(x + P.X()*dx, y + P.Y()*dy, z + P.Z()*dz);
  tabPoint->SetValue(CuIndex, newP);
}


void AppParCurves_MultiPoint::Transform2d(const Standard_Integer CuIndex,
					  const Standard_Real    x,
					  const Standard_Real    dx,
					  const Standard_Real    y,
					  const Standard_Real    dy) 
{
  if (Dimension(CuIndex) != 2) throw Standard_OutOfRange();

  gp_Pnt2d P, newP;
  P = Point2d(CuIndex);
  newP.SetCoord(x + P.X()*dx, y + P.Y()*dy);
  SetPoint2d(CuIndex, newP);
}

void AppParCurves_MultiPoint::SetPoint (const Standard_Integer Index, 
					const gp_Pnt&          Point) {
  Standard_OutOfRange_Raise_if ((Index <= 0) || (Index > nbP),
                                "AppParCurves_MultiPoint::SetPoint() - wrong index");
  tabPoint->SetValue(Index, Point);
}

const gp_Pnt& AppParCurves_MultiPoint::Point (const Standard_Integer Index) const 
{
  Standard_OutOfRange_Raise_if ((Index <= 0) || (Index > nbP),
                                "AppParCurves_MultiPoint::Point() - wrong index");
  return tabPoint->Value(Index);
}

void AppParCurves_MultiPoint::SetPoint2d (const Standard_Integer Index, 
					  const gp_Pnt2d& Point)
 {
  Standard_OutOfRange_Raise_if ((Index <= nbP) || (Index > nbP+nbP2d),
                                "AppParCurves_MultiPoint::SetPoint2d() - wrong index");
  tabPoint2d->SetValue(Index-nbP, Point);
}

const gp_Pnt2d& AppParCurves_MultiPoint::Point2d (const Standard_Integer Index) const 
{
  Standard_OutOfRange_Raise_if ((Index <= nbP) || (Index > nbP+nbP2d),
                                "AppParCurves_MultiPoint::Point2d() - wrong index");
  return tabPoint2d->Value(Index-nbP);
}

void AppParCurves_MultiPoint::Dump(Standard_OStream& o) const
{
  o << "AppParCurves_MultiPoint dump:" << endl;
  const Standard_Integer  aNbPnts3D = NbPoints(),
                          aNbPnts2D = NbPoints2d();
  o << "It contains " << aNbPnts3D << " 3d points and " << aNbPnts2D <<" 2d points." << endl;
  
  if(aNbPnts3D > 0)
  {
    for(Standard_Integer i = tabPoint->Lower(); i <= tabPoint->Upper(); i++)
    {
      o << "3D-Point #" << i << endl;

      o << " Pole x = " << (tabPoint->Value(i)/*->Point(j)*/).X() << endl;
      o << " Pole y = " << (tabPoint->Value(i)/*->Point(j)*/).Y() << endl;
      o << " Pole z = " << (tabPoint->Value(i)/*->Point(j)*/).Z() << endl;
    }
  }
  
  if(aNbPnts2D > 0)
  {
    for(Standard_Integer i = tabPoint2d->Lower(); i <= tabPoint2d->Upper(); i++)
    {
      o << "2D-Point #" << i << endl;

      o << " Pole x = " << (tabPoint2d->Value(i)/*->Point2d(j)*/).X() << endl;
      o << " Pole y = " << (tabPoint2d->Value(i)/*->Point2d(j)*/).Y() << endl;
    }
  }
}
