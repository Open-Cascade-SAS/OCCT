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
#include <Standard_OutOfRange.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#define tabPoint occ::down_cast<NCollection_HArray1<gp_Pnt>>(ttabPoint)
#define tabPoint2d occ::down_cast<NCollection_HArray1<gp_Pnt2d>>(ttabPoint2d)

AppParCurves_MultiPoint::AppParCurves_MultiPoint()
    : nbP(0),
      nbP2d(0)
{
}

AppParCurves_MultiPoint::AppParCurves_MultiPoint(const int NbPoles, const int NbPoles2d)
{
  nbP   = NbPoles;
  nbP2d = NbPoles2d;
  if (nbP != 0)
  {
    occ::handle<NCollection_HArray1<gp_Pnt>> tab3d = new NCollection_HArray1<gp_Pnt>(1, NbPoles);
    ttabPoint                                      = tab3d;
  }
  if (nbP2d != 0)
  {
    occ::handle<NCollection_HArray1<gp_Pnt2d>> tab2d =
      new NCollection_HArray1<gp_Pnt2d>(1, NbPoles2d);
    ttabPoint2d = tab2d;
  }
}

AppParCurves_MultiPoint::AppParCurves_MultiPoint(const NCollection_Array1<gp_Pnt>& tabP)
{
  nbP2d                                          = 0;
  nbP                                            = tabP.Length();
  occ::handle<NCollection_HArray1<gp_Pnt>> tab3d = new NCollection_HArray1<gp_Pnt>(1, nbP);
  ttabPoint                                      = tab3d;
  int                         Lower              = tabP.Lower();
  NCollection_Array1<gp_Pnt>& P3d                = tabPoint->ChangeArray1();
  for (int i = 1; i <= tabP.Length(); i++)
  {
    P3d.SetValue(i, tabP.Value(Lower + i - 1));
  }
}

AppParCurves_MultiPoint::AppParCurves_MultiPoint(const NCollection_Array1<gp_Pnt2d>& tabP2d)
{
  nbP                                              = 0;
  nbP2d                                            = tabP2d.Length();
  occ::handle<NCollection_HArray1<gp_Pnt2d>> tab2d = new NCollection_HArray1<gp_Pnt2d>(1, nbP2d);
  ttabPoint2d                                      = tab2d;
  int                           Lower              = tabP2d.Lower();
  NCollection_Array1<gp_Pnt2d>& P2d                = tabPoint2d->ChangeArray1();
  for (int i = 1; i <= nbP2d; i++)
  {
    P2d.SetValue(i, tabP2d.Value(Lower + i - 1));
  }
}

AppParCurves_MultiPoint::AppParCurves_MultiPoint(const NCollection_Array1<gp_Pnt>&   tabP,
                                                 const NCollection_Array1<gp_Pnt2d>& tabP2d)
{
  nbP                                          = tabP.Length();
  nbP2d                                        = tabP2d.Length();
  occ::handle<NCollection_HArray1<gp_Pnt>> t3d = new NCollection_HArray1<gp_Pnt>(1, nbP);
  ttabPoint                                    = t3d;

  occ::handle<NCollection_HArray1<gp_Pnt2d>> t2d = new NCollection_HArray1<gp_Pnt2d>(1, nbP2d);
  ttabPoint2d                                    = t2d;

  NCollection_Array1<gp_Pnt>& P3d = tabPoint->ChangeArray1();
  int                         i, Lower = tabP.Lower();
  for (i = 1; i <= nbP; i++)
  {
    P3d.SetValue(i, tabP.Value(Lower + i - 1));
  }
  Lower                             = tabP2d.Lower();
  NCollection_Array1<gp_Pnt2d>& P2d = tabPoint2d->ChangeArray1();
  for (i = 1; i <= nbP2d; i++)
  {
    P2d.SetValue(i, tabP2d.Value(Lower + i - 1));
  }
}

AppParCurves_MultiPoint::~AppParCurves_MultiPoint() {}

void AppParCurves_MultiPoint::Transform(const int    CuIndex,
                                        const double x,
                                        const double dx,
                                        const double y,
                                        const double dy,
                                        const double z,
                                        const double dz)
{
  if (Dimension(CuIndex) != 3)
    throw Standard_OutOfRange();

  gp_Pnt P, newP;
  P = Point(CuIndex);
  newP.SetCoord(x + P.X() * dx, y + P.Y() * dy, z + P.Z() * dz);
  tabPoint->SetValue(CuIndex, newP);
}

void AppParCurves_MultiPoint::Transform2d(const int    CuIndex,
                                          const double x,
                                          const double dx,
                                          const double y,
                                          const double dy)
{
  if (Dimension(CuIndex) != 2)
    throw Standard_OutOfRange();

  gp_Pnt2d P, newP;
  P = Point2d(CuIndex);
  newP.SetCoord(x + P.X() * dx, y + P.Y() * dy);
  SetPoint2d(CuIndex, newP);
}

void AppParCurves_MultiPoint::SetPoint(const int Index, const gp_Pnt& Point)
{
  Standard_OutOfRange_Raise_if((Index <= 0) || (Index > nbP),
                               "AppParCurves_MultiPoint::SetPoint() - wrong index");
  tabPoint->SetValue(Index, Point);
}

const gp_Pnt& AppParCurves_MultiPoint::Point(const int Index) const
{
  Standard_OutOfRange_Raise_if((Index <= 0) || (Index > nbP),
                               "AppParCurves_MultiPoint::Point() - wrong index");
  return tabPoint->Value(Index);
}

void AppParCurves_MultiPoint::SetPoint2d(const int Index, const gp_Pnt2d& Point)
{
  Standard_OutOfRange_Raise_if((Index <= nbP) || (Index > nbP + nbP2d),
                               "AppParCurves_MultiPoint::SetPoint2d() - wrong index");
  tabPoint2d->SetValue(Index - nbP, Point);
}

const gp_Pnt2d& AppParCurves_MultiPoint::Point2d(const int Index) const
{
  Standard_OutOfRange_Raise_if((Index <= nbP) || (Index > nbP + nbP2d),
                               "AppParCurves_MultiPoint::Point2d() - wrong index");
  return tabPoint2d->Value(Index - nbP);
}

void AppParCurves_MultiPoint::Dump(Standard_OStream& o) const
{
  o << "AppParCurves_MultiPoint dump:" << std::endl;
  const int aNbPnts3D = NbPoints(), aNbPnts2D = NbPoints2d();
  o << "It contains " << aNbPnts3D << " 3d points and " << aNbPnts2D << " 2d points." << std::endl;

  if (aNbPnts3D > 0)
  {
    for (int i = tabPoint->Lower(); i <= tabPoint->Upper(); i++)
    {
      o << "3D-Point #" << i << std::endl;

      o << " Pole x = " << (tabPoint->Value(i) /*->Point(j)*/).X() << std::endl;
      o << " Pole y = " << (tabPoint->Value(i) /*->Point(j)*/).Y() << std::endl;
      o << " Pole z = " << (tabPoint->Value(i) /*->Point(j)*/).Z() << std::endl;
    }
  }

  if (aNbPnts2D > 0)
  {
    for (int i = tabPoint2d->Lower(); i <= tabPoint2d->Upper(); i++)
    {
      o << "2D-Point #" << i << std::endl;

      o << " Pole x = " << (tabPoint2d->Value(i) /*->Point2d(j)*/).X() << std::endl;
      o << " Pole y = " << (tabPoint2d->Value(i) /*->Point2d(j)*/).Y() << std::endl;
    }
  }
}
