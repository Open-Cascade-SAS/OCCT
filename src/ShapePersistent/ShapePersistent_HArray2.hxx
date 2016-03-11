// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _ShapePersistent_HArray2_HeaderFile
#define _ShapePersistent_HArray2_HeaderFile

#include <StdLPersistent_HArray2.hxx>
#include <StdObject_gp_Vectors.hxx>
#include <StdObject_gp_Curves.hxx>

#include <TColgp_HArray2OfXYZ.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColgp_HArray2OfDir.hxx>
#include <TColgp_HArray2OfVec.hxx>
#include <TColgp_HArray2OfXY.hxx>
#include <TColgp_HArray2OfPnt2d.hxx>
#include <TColgp_HArray2OfDir2d.hxx>
#include <TColgp_HArray2OfVec2d.hxx>
#include <TColgp_HArray2OfLin2d.hxx>
#include <TColgp_HArray2OfCirc2d.hxx>


class ShapePersistent_HArray2 : private StdLPersistent_HArray2
{
public:
  typedef instance<TColgp_HArray2OfXYZ>    XYZ;
  typedef instance<TColgp_HArray2OfPnt>    Pnt;
  typedef instance<TColgp_HArray2OfDir>    Dir;
  typedef instance<TColgp_HArray2OfVec>    Vec;
  typedef instance<TColgp_HArray2OfXY>     XY;
  typedef instance<TColgp_HArray2OfPnt2d>  Pnt2d;
  typedef instance<TColgp_HArray2OfDir2d>  Dir2d;
  typedef instance<TColgp_HArray2OfVec2d>  Vec2d;
  typedef instance<TColgp_HArray2OfLin2d>  Lin2d;
  typedef instance<TColgp_HArray2OfCirc2d> Circ2d;
};

#endif
