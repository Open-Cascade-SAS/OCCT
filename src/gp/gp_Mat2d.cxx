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

#ifndef OCCT_DEBUG
#define No_Standard_OutOfRange
#define No_Standard_ConstructionError
#endif

#include <gp_Mat2d.hxx>

#include <gp_GTrsf2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_XY.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>

// =======================================================================
// function : gp_Mat2d
// purpose  :
// =======================================================================
gp_Mat2d::gp_Mat2d (const gp_XY& theCol1, const gp_XY& theCol2)
{
  myMat[0][0] = theCol1.X(); myMat[1][0] = theCol1.Y();
  myMat[0][1] = theCol2.X(); myMat[1][1] = theCol2.Y();
}

// =======================================================================
// function : SetCol
// purpose  :
// =======================================================================
void gp_Mat2d::SetCol (const Standard_Integer theCol,
                       const gp_XY& theValue)
{
  Standard_OutOfRange_Raise_if (theCol < 1 || theCol > 2, "gp_Mat2d::SetCol() - invalid index");
  if  (theCol == 1)
  {
    myMat[0][0] = theValue.X();
    myMat[1][0] = theValue.Y();
  }
  else
  {
    myMat[0][1] = theValue.X();
    myMat[1][1] = theValue.Y();
  }
}

// =======================================================================
// function : SetCols
// purpose  :
// =======================================================================
void gp_Mat2d::SetCols (const gp_XY& theCol1,
                        const gp_XY& theCol2)
{
  myMat[0][0] = theCol1.X(); myMat[1][0] = theCol1.Y();
  myMat[0][1] = theCol2.X(); myMat[1][1] = theCol2.Y();
}

// =======================================================================
// function : SetRow
// purpose  :
// =======================================================================
void gp_Mat2d::SetRow (const Standard_Integer theRow, const gp_XY& theValue)
{
  Standard_OutOfRange_Raise_if (theRow < 1 || theRow > 2, "gp_Mat2d::SetRow() - invalid index");
  if (theRow == 1)
  {
    myMat[0][0] = theValue.X();
    myMat[0][1] = theValue.Y();
  }
  else
  {
    myMat[1][0] = theValue.X();
    myMat[1][1] = theValue.Y();
  }
}

// =======================================================================
// function : SetRows
// purpose  :
// =======================================================================
void gp_Mat2d::SetRows (const gp_XY& theRow1, const gp_XY& theRow2)
{
  myMat[0][0] = theRow1.X(); myMat[0][1] = theRow1.Y();
  myMat[1][0] = theRow2.X(); myMat[1][1] = theRow2.Y();
}

// =======================================================================
// function : Column
// purpose  :
// =======================================================================
gp_XY gp_Mat2d::Column (const Standard_Integer theCol) const
{
  Standard_OutOfRange_Raise_if (theCol < 1 || theCol > 2, "gp_Mat2d::Column() - invalid index");
  if (theCol == 1)
  {
    return gp_XY (myMat[0][0], myMat[1][0]);
  }
  return gp_XY (myMat[0][1], myMat[1][1]);
}

// =======================================================================
// function : Diagonal
// purpose  :
// =======================================================================
gp_XY gp_Mat2d::Diagonal() const
{ 
  return gp_XY (myMat[0][0], myMat[1][1]);
}

// =======================================================================
// function : Row
// purpose  :
// =======================================================================
gp_XY gp_Mat2d::Row (const Standard_Integer theRow) const
{
  Standard_OutOfRange_Raise_if (theRow < 1 || theRow > 2, "gp_Mat2d::Row() - invalid index");
  if (theRow == 1)
  {
    return gp_XY (myMat[0][0], myMat[0][1]);
  }
  return gp_XY (myMat[1][0], myMat[1][1]);
}

// =======================================================================
// function : Invert
// purpose  :
// =======================================================================
void gp_Mat2d::Invert()
{
  Standard_Real aNewMat[2][2];
  aNewMat[0][0] =  myMat[1][1];
  aNewMat[0][1] = -myMat[0][1];
  aNewMat[1][0] = -myMat[1][0];
  aNewMat[1][1] =  myMat[0][0];
  Standard_Real aDet = aNewMat[0][0] * aNewMat[1][1] - aNewMat[0][1] * aNewMat[1][0];
  Standard_Real val = aDet;
  if (val < 0) val = - val;
  Standard_ConstructionError_Raise_if (val <= gp::Resolution(), "gp_Mat2d::Invert() - matrix has zero determinant");
  aDet = 1.0 / aDet;
  myMat[0][0] = aNewMat[0][0] * aDet;
  myMat[1][0] = aNewMat[1][0] * aDet;
  myMat[0][1] = aNewMat[0][1] * aDet;
  myMat[1][1] = aNewMat[1][1] * aDet;
}

// =======================================================================
// function : Power
// purpose  :
// =======================================================================
void gp_Mat2d::Power (const Standard_Integer theN)
{
  if      (theN ==  1) { }
  else if (theN ==  0) { SetIdentity(); }
  else if (theN == -1) { Invert(); }
  else {
    if (theN < 0) Invert();
    Standard_Integer Npower = theN;
    if (Npower < 0) Npower = - Npower;
    Npower--;
    gp_Mat2d aTemp = *this;
    for(;;) {
      if (IsOdd(Npower)) Multiply (aTemp);
      if (Npower == 1)   break;
      aTemp.Multiply (aTemp);
      Npower = Npower/2;
    }
  }
}
