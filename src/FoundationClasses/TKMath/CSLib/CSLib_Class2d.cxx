// Created on: 1995-03-08
// Created by: Laurent BUCHARD
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

// #define No_Standard_OutOfRange

#include <CSLib_Class2d.hxx>
#include <gp_Pnt2d.hxx>

static inline Standard_Real Transform2d(const Standard_Real u,
                                        const Standard_Real umin,
                                        const Standard_Real umaxmumin);

//=================================================================================================

template <class TCol_Containers2d>
void CSLib_Class2d::Init(const TCol_Containers2d& TP2d,
                         const Standard_Real      aTolu,
                         const Standard_Real      aTolv,
                         const Standard_Real      umin,
                         const Standard_Real      vmin,
                         const Standard_Real      umax,
                         const Standard_Real      vmax)
{
  Umin = umin;
  Vmin = vmin;
  Umax = umax;
  Vmax = vmax;
  //
  if ((umax <= umin) || (vmax <= vmin) || (TP2d.Length() < 3))
  {
    MyPnts2dX.Nullify();
    MyPnts2dY.Nullify();
    N = 0;
  }
  //
  else
  {
    Standard_Integer i, iLower;
    Standard_Real    du, dv, aPrc;
    //
    aPrc      = 1.e-10;
    N         = TP2d.Length();
    Tolu      = aTolu;
    Tolv      = aTolv;
    MyPnts2dX = new TColStd_Array1OfReal(0, N);
    MyPnts2dY = new TColStd_Array1OfReal(0, N);
    du        = umax - umin;
    dv        = vmax - vmin;
    //
    iLower = TP2d.Lower();
    for (i = 0; i < N; ++i)
    {
      const gp_Pnt2d& aP2D      = TP2d(i + iLower);
      MyPnts2dX->ChangeValue(i) = Transform2d(aP2D.X(), umin, du);
      MyPnts2dY->ChangeValue(i) = Transform2d(aP2D.Y(), vmin, dv);
    }
    MyPnts2dX->ChangeLast() = MyPnts2dX->First();
    MyPnts2dY->ChangeLast() = MyPnts2dY->First();
    //
    if (du > aPrc)
    {
      Tolu /= du;
    }
    if (dv > aPrc)
    {
      Tolv /= dv;
    }
  }
}

//=================================================================================================

CSLib_Class2d::CSLib_Class2d(const TColgp_Array1OfPnt2d& thePnts2d,
                             const Standard_Real         theTolU,
                             const Standard_Real         theTolV,
                             const Standard_Real         theUMin,
                             const Standard_Real         theVMin,
                             const Standard_Real         theUMax,
                             const Standard_Real         theVMax)
{
  Init(thePnts2d, theTolU, theTolV, theUMin, theVMin, theUMax, theVMax);
}

//=================================================================================================

CSLib_Class2d::CSLib_Class2d(const TColgp_SequenceOfPnt2d& thePnts2d,
                             const Standard_Real           theTolU,
                             const Standard_Real           theTolV,
                             const Standard_Real           theUMin,
                             const Standard_Real           theVMin,
                             const Standard_Real           theUMax,
                             const Standard_Real           theVMax)
{
  Init(thePnts2d, theTolU, theTolV, theUMin, theVMin, theUMax, theVMax);
}

//=================================================================================================

Standard_Integer CSLib_Class2d::SiDans(const gp_Pnt2d& P) const
{
  if (!N)
  {
    return 0;
  }
  //
  Standard_Real x, y, aTolu, aTolv;
  //
  x     = P.X();
  y     = P.Y();
  aTolu = Tolu * (Umax - Umin);
  aTolv = Tolv * (Vmax - Vmin);
  //
  if (Umin < Umax && Vmin < Vmax)
  {
    if ((x < (Umin - aTolu)) || (x > (Umax + aTolu)) || (y < (Vmin - aTolv))
        || (y > (Vmax + aTolv)))
    {
      return -1;
    }
    x = Transform2d(x, Umin, Umax - Umin);
    y = Transform2d(y, Vmin, Vmax - Vmin);
  }

  Standard_Integer res = InternalSiDansOuOn(x, y);
  if (res == -1)
  {
    return 0;
  }
  if (Tolu || Tolv)
  {
    if (res != InternalSiDans(x - Tolu, y - Tolv))
      return 0;
    if (res != InternalSiDans(x + Tolu, y - Tolv))
      return 0;
    if (res != InternalSiDans(x - Tolu, y + Tolv))
      return 0;
    if (res != InternalSiDans(x + Tolu, y + Tolv))
      return 0;
  }
  //
  return ((res) ? 1 : -1);
}

//=================================================================================================

Standard_Integer CSLib_Class2d::SiDans_OnMode(const gp_Pnt2d& P, const Standard_Real Tol) const
{
  if (!N)
  {
    return 0;
  }
  //
  Standard_Real x, y, aTolu, aTolv;
  //
  x     = P.X();
  y     = P.Y();
  aTolu = Tol;
  aTolv = Tol;

  //-- ****** TO DO LATER, ESTIMATE AT EACH POINT Tol2d depending on Tol3d *****
  if (Umin < Umax && Vmin < Vmax)
  {
    if (x < (Umin - aTolu) || (x > Umax + aTolu) || (y < Vmin - aTolv) || (y > Vmax + aTolv))
    {
      return -1;
    }
    x = Transform2d(x, Umin, Umax - Umin);
    y = Transform2d(y, Vmin, Vmax - Vmin);
  }
  //
  Standard_Integer res = InternalSiDansOuOn(x, y);
  if (aTolu || aTolv)
  {
    if (res != InternalSiDans(x - aTolu, y - aTolv))
      return 0;
    if (res != InternalSiDans(x + aTolu, y - aTolv))
      return 0;
    if (res != InternalSiDans(x - aTolu, y + aTolv))
      return 0;
    if (res != InternalSiDans(x + aTolu, y + aTolv))
      return 0;
  }
  return ((res) ? 1 : -1);
}

//=================================================================================================

Standard_Integer CSLib_Class2d::InternalSiDans(const Standard_Real Px, const Standard_Real Py) const
{
  Standard_Integer nbc, i, ip1, SH, NH;
  Standard_Real    x, y, nx, ny;
  //
  nbc = 0;
  i   = 0;
  ip1 = 1;
  x   = (MyPnts2dX->Value(i) - Px);
  y   = (MyPnts2dY->Value(i) - Py);
  SH  = (y < 0.) ? -1 : 1;
  //
  for (i = 0; i < N; i++, ip1++)
  {
    nx = MyPnts2dX->Value(ip1) - Px;
    ny = MyPnts2dY->Value(ip1) - Py;

    NH = (ny < 0.) ? -1 : 1;
    if (NH != SH)
    {
      if (x > 0. && nx > 0.)
      {
        nbc++;
      }
      else
      {
        if (x > 0.0 || nx > 0.)
        {
          if ((x - y * (nx - x) / (ny - y)) > 0.)
          {
            nbc++;
          }
        }
      }
      SH = NH;
    }
    x = nx;
    y = ny;
  }
  return (nbc & 1);
}

// modified by NIZNHY-PKV Fri Jan 15 09:03:48 2010f
//=======================================================================
// function : InternalSiDansOuOn
// purpose  : same code as above + test on ON (return(-1) in this case
//=======================================================================
Standard_Integer CSLib_Class2d::InternalSiDansOuOn(const Standard_Real Px,
                                                   const Standard_Real Py) const
{
  Standard_Integer nbc, i, ip1, SH, NH, iRet;
  Standard_Real    x, y, nx, ny, aX;
  Standard_Real    aYmin;
  //
  nbc   = 0;
  i     = 0;
  ip1   = 1;
  x     = (MyPnts2dX->Value(i) - Px);
  y     = (MyPnts2dY->Value(i) - Py);
  aYmin = y;
  SH    = (y < 0.) ? -1 : 1;
  for (i = 0; i < N; i++, ip1++)
  {

    nx = MyPnts2dX->Value(ip1) - Px;
    ny = MyPnts2dY->Value(ip1) - Py;
    //-- le 14 oct 97
    if (nx < Tolu && nx > -Tolu && ny < Tolv && ny > -Tolv)
    {
      iRet = -1;
      return iRet;
    }
    // find Y coordinate of polyline for current X gka
    // in order to detect possible status ON
    Standard_Real aDx = (MyPnts2dX->Value(ip1) - MyPnts2dX->Value(ip1 - 1));
    if ((MyPnts2dX->Value(ip1 - 1) - Px) * nx < 0.)
    {

      Standard_Real aCurPY =
        MyPnts2dY->Value(ip1) - (MyPnts2dY->Value(ip1) - MyPnts2dY->Value(ip1 - 1)) / aDx * nx;
      Standard_Real aDeltaY = aCurPY - Py;
      if (aDeltaY >= -Tolv && aDeltaY <= Tolv)
      {
        iRet = -1;
        return iRet;
      }
    }
    //

    NH = (ny < 0.) ? -1 : 1;
    if (NH != SH)
    {
      if (x > 0. && nx > 0.)
      {
        nbc++;
      }
      else
      {
        if (x > 0. || nx > 0.)
        {
          aX = x - y * (nx - x) / (ny - y);
          if (aX > 0.)
          {
            nbc++;
          }
        }
      }
      SH = NH;
    }
    else
    { // y has same sign as ny
      if (ny < aYmin)
      {
        aYmin = ny;
      }
    }
    x = nx;
    y = ny;
  } // for(i=0; i<N ; i++,ip1++) {

  iRet = nbc & 1;
  return iRet;
}

// modified by NIZNHY-PKV Fri Jan 15 09:03:55 2010t
//=================================================================================================

Standard_Real Transform2d(const Standard_Real u,
                          const Standard_Real umin,
                          const Standard_Real umaxmumin)
{
  if (umaxmumin > 1e-10)
  {
    Standard_Real U = (u - umin) / umaxmumin;
    return U;
  }
  else
  {
    return u;
  }
}
