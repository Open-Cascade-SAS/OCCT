// Created on: 1995-03-27
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

//Modif on jun-24-97 : introduction de CSLib_Class2d de LBR
//                     pour teste si on est dedans ou dehors...
//Modif on jul-21-97 : changement en harray1 pour eventuelles connexions ...

#include <Select3D_SensitiveFace.ixx>
#include <Select3D_Projector.hxx>
#include <SelectBasics_BasicTool.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <ElCLib.hxx>

#include <CSLib_Class2d.hxx>

//==================================================
// Function: Hide this constructor to the next version...
// Purpose : simply avoid interfering with the version update
//==================================================

Select3D_SensitiveFace::
Select3D_SensitiveFace(const Handle(SelectBasics_EntityOwner)& OwnerId,
                       const TColgp_Array1OfPnt& ThePoints,
                       const Select3D_TypeOfSensitivity aType):
Select3D_SensitivePoly(OwnerId, ThePoints),
mytype (aType)
{
}

//==================================================
// Function: Creation
// Purpose :
//==================================================

Select3D_SensitiveFace::
Select3D_SensitiveFace(const Handle(SelectBasics_EntityOwner)& OwnerId,
                       const Handle(TColgp_HArray1OfPnt)& ThePoints,
                       const Select3D_TypeOfSensitivity aType):
Select3D_SensitivePoly(OwnerId, ThePoints),
mytype (aType)
{
}

//==================================================
// Function: Matches
// Purpose :
//==================================================

Standard_Boolean Select3D_SensitiveFace::Matches (const SelectBasics_PickArgs& thePickArgs,
                                                  Standard_Real& theMatchDMin,
                                                  Standard_Real& theMatchDepth)
{
  Standard_Real DMin2 = 0.;
  Standard_Real Xmin = 0.,Ymin = 0.,Xmax = 0.,Ymax = 0.;
  if(!Bnd_Box2d(mybox2d).IsVoid())
  {
    Bnd_Box2d(mybox2d).Get(Xmin,Ymin,Xmax,Ymax);
    DMin2 = gp_XY(Xmax-Xmin,Ymax-Ymin).SquareModulus();
  }
  // calculation of a criterion of minimum distance...
  // from start Dmin = size of the bounding box 2D,
  // then min. distance of the polyhedron or cdg...

  Standard_Real aTol2 = thePickArgs.Tolerance() * thePickArgs.Tolerance();
  Standard_Integer aSize = mypolyg.Size(), anIndex;
  gp_XY CDG;
  for(anIndex=0;anIndex<aSize;++anIndex)
  {
    CDG+=mypolyg.Pnt2d(anIndex);
  }

  if(aSize>1)
  {
    CDG/=(aSize-1);
  }
  DMin2 = Min (DMin2, gp_XY (CDG.X() - thePickArgs.X(), CDG.Y() - thePickArgs.Y()).SquareModulus());
  theMatchDMin = Sqrt(DMin2);

  Standard_Boolean isplane2d(Standard_True);

  for(anIndex=1;anIndex<aSize;++anIndex)
  {
    gp_XY V1(mypolyg.Pnt2d(anIndex)),V(thePickArgs.X(), thePickArgs.Y());
    V1-=mypolyg.Pnt2d(anIndex-1);
    V-=mypolyg.Pnt2d(anIndex-1);
    Standard_Real Vector = V1^V;
    Standard_Real V1V1 = V1.SquareModulus();
    DMin2 =
      (V1V1 <=aTol2) ?
    Min(DMin2,V.SquareModulus()): // if the segment is too small...
      Min(DMin2,Vector*Vector/V1V1);
    //cdg ...
    gp_XY PlaneTest(CDG);
    PlaneTest-=mypolyg.Pnt2d(anIndex-1);
    Standard_Real valtst = PlaneTest^V1;
    if(isplane2d && Abs(valtst) > thePickArgs.Tolerance()) isplane2d=Standard_False;
  }
  if (isplane2d)
  {
    theMatchDepth = ComputeDepth (thePickArgs.PickLine(),
                                  thePickArgs.DepthMin(),
                                  thePickArgs.DepthMax());

    return !thePickArgs.IsClipped (theMatchDepth);
  }

  //otherwise it is checked if the point is in the face...
  TColgp_Array1OfPnt2d aArrayOf2dPnt(1, aSize);
  Points2D(aArrayOf2dPnt);
  CSLib_Class2d TheInOutTool (aArrayOf2dPnt,
                              thePickArgs.Tolerance(),
                              thePickArgs.Tolerance(),
                              Xmin, Ymin, Xmax, Ymax);
  Standard_Integer TheStat = TheInOutTool.SiDans (gp_Pnt2d (thePickArgs.X(), thePickArgs.Y()));

  Standard_Boolean res(Standard_False);
  switch(TheStat)
  {
  case 0:
    res = Standard_True;
  case 1:
    {
      if(mytype!=Select3D_TOS_BOUNDARY)
        res = Standard_True;
    }
  }
  if (res)
  {
    theMatchDepth = ComputeDepth (thePickArgs.PickLine(),
                                  thePickArgs.DepthMin(),
                                  thePickArgs.DepthMax());

    return !thePickArgs.IsClipped (theMatchDepth);
  }
  return Standard_False;
}

//=======================================================================
//function : Matches
//purpose  :
//=======================================================================

Standard_Boolean Select3D_SensitiveFace::
Matches (const Standard_Real XMin,
         const Standard_Real YMin,
         const Standard_Real XMax,
         const Standard_Real YMax,
         const Standard_Real aTol)
{
  Bnd_Box2d BoundBox;
  BoundBox.Update(XMin-aTol,YMin-aTol,XMax+aTol,YMax+aTol);

  for(Standard_Integer anIndex=0;anIndex<mypolyg.Size();++anIndex)
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

Standard_Boolean Select3D_SensitiveFace::
Matches (const TColgp_Array1OfPnt2d& aPoly,
         const Bnd_Box2d& aBox,
         const Standard_Real aTol)
{
  Standard_Real Umin,Vmin,Umax,Vmax;
  aBox.Get(Umin,Vmin,Umax,Vmax);
  CSLib_Class2d aClassifier2d(aPoly,aTol,aTol,Umin,Vmin,Umax,Vmax);

  gp_Pnt2d aPnt2d;
  for(Standard_Integer anIndex=0;anIndex<mypolyg.Size();++anIndex)
  {
    Standard_Integer RES = aClassifier2d.SiDans(mypolyg.Pnt2d(anIndex));
    if(RES!=1)
      return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================

void Select3D_SensitiveFace::Dump(Standard_OStream& S,const Standard_Boolean FullDump) const
{
  S<<"\tSensitiveFace 3D :"<<endl;;
  if(HasLocation())
    S<<"\t\tExisting Location"<<endl;

  if(mytype==Select3D_TOS_BOUNDARY)
    S<<"\t\tSelection Of Bounding Polyline Only"<<endl;

  if(FullDump)
  {
    S<<"\t\tNumber Of Points :"<<mypolyg.Size()<<endl;
    Select3D_SensitiveEntity::DumpBox(S,mybox2d);
  }
}

//=======================================================================
//function : ComputeDepth
//purpose  :
//=======================================================================

Standard_Real Select3D_SensitiveFace::ComputeDepth (const gp_Lin& thePickLine,
                                                    const Standard_Real theDepthMin,
                                                    const Standard_Real theDepthMax) const
{
  Standard_Real aDepth = Precision::Infinite();
  Standard_Real aPointDepth;

  for (Standard_Integer anIndex = 0; anIndex < mypolyg.Size()-1; ++anIndex)
  {
    aPointDepth = ElCLib::Parameter (thePickLine, mypolyg.Pnt(anIndex));
    if (aPointDepth < aDepth && (aPointDepth > theDepthMin) && (aPointDepth < theDepthMax))
    {
      aDepth = aPointDepth;
    }
  }
  return aDepth;
}

//=======================================================================
//function : ComputeDepth
//purpose  :
//=======================================================================

void Select3D_SensitiveFace::ComputeDepth(const gp_Lin& /*theEyeLine*/) const
{
  // this method is obsolete.
}

//=======================================================================
//function : GetConnected
//purpose  :
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveFace::GetConnected(const TopLoc_Location &theLocation)
{
  // Create a copy of this
  Standard_Integer aSize = mypolyg.Size();
  TColgp_Array1OfPnt aPoints(1, aSize);
  for (Standard_Integer anIndex = 1; anIndex <= aSize; ++anIndex)
  {
    aPoints.SetValue(anIndex, mypolyg.Pnt(anIndex-1));
  }

  Handle(Select3D_SensitiveEntity) aNewEntity =
    new Select3D_SensitiveFace(myOwnerId, aPoints, mytype);

  if (HasLocation())
    aNewEntity->SetLocation(Location());

  aNewEntity->UpdateLocation(theLocation);

  return aNewEntity;
}
