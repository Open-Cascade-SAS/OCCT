// Created on: 1995-03-13
// Created by: Robert COUBLANC
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

#include <Select3D_SensitiveCurve.ixx>
#include <SelectBasics_BasicTool.hxx>
#include <gp_Lin2d.hxx>
#include <Precision.hxx>
#include <ElCLib.hxx>
#include <CSLib_Class2d.hxx>
#include <Extrema_ExtElC.hxx>

//==================================================
// Function: Creation
// Purpose :
//==================================================

Select3D_SensitiveCurve
::Select3D_SensitiveCurve(const Handle(SelectBasics_EntityOwner)& OwnerId,
                          const Handle(Geom_Curve)& C,
                          const Standard_Integer NbPoints):
Select3D_SensitivePoly(OwnerId, NbPoints),
mylastseg(0),
myCurve(C)
{
  LoadPoints(C,NbPoints);
}

//==================================================
// Function: Creation
// Purpose :
//==================================================

Select3D_SensitiveCurve
::Select3D_SensitiveCurve(const Handle(SelectBasics_EntityOwner)& OwnerId,
                          const Handle(TColgp_HArray1OfPnt)& ThePoints):
Select3D_SensitivePoly(OwnerId, ThePoints),
mylastseg(0)
{
}

//==================================================
// Function: Creation
// Purpose :
//==================================================

Select3D_SensitiveCurve
::Select3D_SensitiveCurve(const Handle(SelectBasics_EntityOwner)& OwnerId,
                          const TColgp_Array1OfPnt& ThePoints):
Select3D_SensitivePoly(OwnerId, ThePoints),
mylastseg(0)
{
}

//==================================================
// Function: Matches
// Purpose :
//==================================================

Standard_Boolean Select3D_SensitiveCurve::Matches (const SelectBasics_PickArgs& thePickArgs,
                                                   Standard_Real& theMatchDMin,
                                                   Standard_Real& theMatchDepth)
{
  Standard_Integer Rank;
  TColgp_Array1OfPnt2d aArrayOf2dPnt(1, mypolyg.Size());
  Points2D(aArrayOf2dPnt);
  if (SelectBasics_BasicTool::MatchPolyg2d (aArrayOf2dPnt,
                                            thePickArgs.X(), thePickArgs.Y(),
                                            thePickArgs.Tolerance(),
                                            theMatchDMin,
                                            Rank))
  {
    // remember detected segment (for GetLastDetected)
    mylastseg = Rank;

    theMatchDepth = ComputeDepth (thePickArgs.PickLine(), Rank);

    return !thePickArgs.IsClipped (theMatchDepth);
  }

  return Standard_False;
}

//==================================================
// Function: Matches
// Purpose : know if a box touches the projected polygon
//           of the Curve.
//==================================================

Standard_Boolean Select3D_SensitiveCurve::
Matches (const Standard_Real XMin,
         const Standard_Real YMin,
         const Standard_Real XMax,
         const Standard_Real YMax,
         const Standard_Real aTol)
{
  Bnd_Box2d BoundBox;
  BoundBox.Update(XMin-aTol,YMin-aTol,XMax+aTol,YMax+aTol);

  for(Standard_Integer anIndex=0; anIndex<mypolyg.Size(); ++anIndex)
    {
      if(BoundBox.IsOut(mypolyg.Pnt2d(anIndex)))
        return Standard_False;
    }
  return Standard_True;
}

//=======================================================================
//function : Matches
//purpose  :
//=======================================================================

Standard_Boolean Select3D_SensitiveCurve::
Matches (const TColgp_Array1OfPnt2d& aPoly,
         const Bnd_Box2d& aBox,
         const Standard_Real aTol)
{
  Standard_Real Umin,Vmin,Umax,Vmax;
  aBox.Get(Umin,Vmin,Umax,Vmax);
  CSLib_Class2d aClassifier2d(aPoly,aTol,aTol,Umin,Vmin,Umax,Vmax);

  for(Standard_Integer anIndex=0;anIndex<mypolyg.Size();++anIndex)
  {
    Standard_Integer RES = aClassifier2d.SiDans(mypolyg.Pnt2d(anIndex));
    if(RES!=1)
      return Standard_False;
  }
  return Standard_True;
}

//==================================================
// Function: LoadPoints
// Purpose :
//==================================================

void Select3D_SensitiveCurve
::LoadPoints (const Handle(Geom_Curve)& aCurve,const Standard_Integer NbP)
{
  /*this method is private and it used only inside of constructor.
    That's why check !NbP==mypolyg3d->Length() was removed*/

  Standard_Real Step = (aCurve->LastParameter()- aCurve->FirstParameter())/(NbP-1);
  Standard_Real Curparam = aCurve->FirstParameter();
  for(Standard_Integer anIndex=0;anIndex<mypolyg.Size();++anIndex)
  {
    mypolyg.SetPnt(anIndex, aCurve->Value(Curparam));
    Curparam+=Step;
  }
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================

void Select3D_SensitiveCurve::Dump(Standard_OStream& S,const Standard_Boolean FullDump) const
{
  S<<"\tSensitiveCurve 3D :"<<endl;
  if (HasLocation())
    S<<"\t\tExisting Location"<<endl;

  S<<"\t\tNumber Of Points :"<<mypolyg.Size()<<endl;

  if(FullDump)
  {
    Select3D_SensitiveEntity::DumpBox(S,mybox2d);
  }
}

//=======================================================================
//function : ComputeDepth
//purpose  :
//=======================================================================

Standard_Real Select3D_SensitiveCurve::ComputeDepth (const gp_Lin& thePickLine,
                                                     const Standard_Integer theSegment) const
{
  Standard_Real aDepth = Precision::Infinite();
  if (theSegment == 0)
  {
    return aDepth;
  }

  // In case if theSegment and theSegment + 1 are not valid
  // the depth will be infinite
  if (theSegment >= mypolyg.Size())
  {
    return aDepth;
  }

  gp_XYZ aCDG = mypolyg.Pnt (theSegment);

  // Check depth of a line forward within the curve.
  if (theSegment + 1 < mypolyg.Size())
  {
    gp_XYZ aCDG1 = mypolyg.Pnt (theSegment + 1);
    if (ComputeDepth(thePickLine, aCDG, aCDG1, aDepth))
    {
      return aDepth;
    }
  }

  // Check depth of a line backward within the curve.
  if (theSegment - 1 >= 0)
  {
    gp_XYZ aCDG1 = mypolyg.Pnt (theSegment - 1);
    if (ComputeDepth(thePickLine, aCDG, aCDG1, aDepth))
    {
      return aDepth;
    }
  }

  // Calculate the depth in the middle point of
  // a next (forward) segment of the curve.
  if (theSegment + 1 < mypolyg.Size())
  {
    aCDG += mypolyg.Pnt(theSegment + 1);
    aCDG /= 2.;
  }

  return ElCLib::Parameter (thePickLine, gp_Pnt (aCDG));
}

//=======================================================================
//function : GetConnected
//purpose  :
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveCurve::GetConnected(const TopLoc_Location &theLocation)
{
  // Create a copy of this
  Handle(Select3D_SensitiveEntity) aNewEntity;
  // this was constructed using Handle(Geom_Curve)
  if (!myCurve.IsNull())
  {
    aNewEntity = new Select3D_SensitiveCurve(myOwnerId, myCurve);
  }
  // this was constructed using TColgp_HArray1OfPnt
  else
  {
    Standard_Integer aSize = mypolyg.Size();
    Handle(TColgp_HArray1OfPnt) aPoints = new TColgp_HArray1OfPnt(1, aSize);
    // Fill the array with points from mypolyg3d
    for (Standard_Integer anIndex = 1; anIndex <= aSize; ++anIndex)
    {
      aPoints->SetValue(anIndex, mypolyg.Pnt(anIndex-1));
    }
     aNewEntity = new Select3D_SensitiveCurve(myOwnerId, aPoints);
  }

  if (HasLocation())
    aNewEntity->SetLocation(Location());

  aNewEntity->UpdateLocation(theLocation);

  return aNewEntity;
}

//=======================================================================
//function : ComputeDepth()
//purpose  : Computes the depth by means of intersection of
//           a segment of the curve defined by <theP1, theP2> and
//           the eye-line <thePickLine>.
//=======================================================================

Standard_Boolean Select3D_SensitiveCurve::ComputeDepth(const gp_Lin& thePickLine,
                                                       const gp_XYZ& theP1,
                                                       const gp_XYZ& theP2,
                                                       Standard_Real& theDepth) const
{
  // The segment may have null length.
  gp_XYZ aVec = theP2 - theP1;
  Standard_Real aLength = aVec.Modulus();
  if (aLength <= gp::Resolution())
  {
    theDepth = ElCLib::Parameter(thePickLine, theP1);
    return Standard_True;
  }

  // Compute an intersection point of the segment-line and the eye-line.
  gp_Lin aLine (theP1, aVec);
  Extrema_ExtElC anExtrema(aLine, thePickLine, Precision::Angular());
  if (anExtrema.IsDone() && !anExtrema.IsParallel() )
  {
    // Iterator on solutions (intersection points).
    for (Standard_Integer i = 1; i <= anExtrema.NbExt(); i++)
    {
      // Get the intersection point.
      Extrema_POnCurv aPointOnLine1, aPointOnLine2;
      anExtrema.Points(i, aPointOnLine1, aPointOnLine2);

      // Check bounds: the point of intersection should lie within the segment.
      if (aPointOnLine1.Parameter() > 0.0 && aPointOnLine1.Parameter() < aLength)
      {
        theDepth = ElCLib::Parameter(thePickLine, aPointOnLine1.Value());
        return Standard_True;
      }
    }
  }

  return Standard_False;
}
