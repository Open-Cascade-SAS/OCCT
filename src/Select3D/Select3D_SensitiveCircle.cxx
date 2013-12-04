// Created on: 1996-02-06
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Modified    Tue Apr 14 1998 by rob : fix Bug : Case of Null Radius Circle...

#include <Select3D_SensitiveCircle.ixx>
#include <Precision.hxx>
#include <gp_Lin2d.hxx>

#include <CSLib_Class2d.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <ElCLib.hxx>
#include <Select3D_Pnt.hxx>
#include <Select3D_Pnt2d.hxx>
#include <Select3D_Projector.hxx>


static Standard_Integer S3D_GetCircleNBPoints(const Handle(Geom_Circle)& C,
                                              const Standard_Integer anInputNumber)
{
  // Check if number of points is invalid.
  // In this case mypolyg raises Standard_ConstructionError
  // exception (look constructor bellow).
  if (anInputNumber <= 0)
    return 0;

  if (C->Radius()>Precision::Confusion())
    return 2*anInputNumber+1;
  // The radius is too small and circle degenerates into point
  return 1;
}

static Standard_Integer S3D_GetArcNBPoints(const Handle(Geom_Circle)& C,
                    const Standard_Integer anInputNumber)
{
  // There is no need to check number of points here.
  // In case of invalid number of points this method returns
  // -1 or smaller value.
  if (C->Radius()>Precision::Confusion())
    return 2*anInputNumber-1;

  // The radius is too small and circle degenerates into point
  return 1;
}

//=======================================================================
//function : Select3D_SensitiveCircle (constructor)
//purpose  : Definition of a sensitive circle
//=======================================================================

Select3D_SensitiveCircle::
Select3D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& OwnerId,
                         const Handle(Geom_Circle)& TheCircle,
                         const Standard_Boolean FilledCircle,
                         const Standard_Integer NbPoints):
Select3D_SensitivePoly(OwnerId, S3D_GetCircleNBPoints(TheCircle,NbPoints)),
myFillStatus(FilledCircle),
myCircle(TheCircle),
mystart(0),
myend(0)
{
  if (mypolyg.Size() != 1)
  {
    gp_Pnt p1,p2;
    gp_Vec v1;
    Standard_Real ustart = TheCircle->FirstParameter(),uend = TheCircle->LastParameter();
    Standard_Real du = (uend-ustart)/NbPoints;
    Standard_Real R = TheCircle->Radius();
    Standard_Integer rank = 1;
    Standard_Real curu =ustart;
    for(Standard_Integer anIndex=1;anIndex<=NbPoints;anIndex++)
    {
      TheCircle->D1(curu,p1,v1);

      v1.Normalize();
      mypolyg.SetPnt(rank-1, p1);
      rank++;
      p2 = gp_Pnt(p1.X()+v1.X()*tan(du/2.)*R,
            p1.Y()+v1.Y()*tan(du/2.)*R,
            p1.Z()+v1.Z()*tan(du/2.)*R);
      mypolyg.SetPnt(rank-1, p2);
      rank++;
      curu+=du;
    }

    // Copy the first point to the last point of mypolyg
    mypolyg.SetPnt(NbPoints*2, mypolyg.Pnt(0));
    // Get myCenter3D
    myCenter3D = TheCircle->Location();
  }
  // Radius = 0.0
  else
  {
    mypolyg.SetPnt(0, TheCircle->Location());
    // Get myCenter3D
    myCenter3D = mypolyg.Pnt(0);
  }
}

//=======================================================================
//function : Select3D_SensitiveCircle (constructor)
//purpose  : Definition of a sensitive arc
//=======================================================================

Select3D_SensitiveCircle::
Select3D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& OwnerId,
                         const Handle(Geom_Circle)& TheCircle,
                         const Standard_Real u1,
                         const Standard_Real u2,
                         const Standard_Boolean FilledCircle,
                         const Standard_Integer NbPoints):
Select3D_SensitivePoly(OwnerId, S3D_GetArcNBPoints(TheCircle,NbPoints)),
myFillStatus(FilledCircle),
myCircle(TheCircle),
mystart(u1),
myend(u2)
{
  if (mypolyg.Size() != 1)
  {
    gp_Pnt p1,p2;
    gp_Vec v1;

    if (u1 > u2)
    {
      mystart = u2;
      myend = u1;
    }

    Standard_Real du = (myend-mystart)/(NbPoints-1);
    Standard_Real R = TheCircle->Radius();
    Standard_Integer rank = 1;
    Standard_Real curu = mystart;

    for(Standard_Integer anIndex=1;anIndex<=NbPoints-1;anIndex++)
    {
      TheCircle->D1(curu,p1,v1);
      v1.Normalize();
      mypolyg.SetPnt(rank-1, p1);
      rank++;
      p2 = gp_Pnt(p1.X()+v1.X()*tan(du/2.)*R,
            p1.Y()+v1.Y()*tan(du/2.)*R,
            p1.Z()+v1.Z()*tan(du/2.)*R);
      mypolyg.SetPnt(rank-1, p2);
      rank++;
      curu+=du;
    }
    TheCircle->D0(myend,p1);
    mypolyg.SetPnt(NbPoints*2-2, p1);
    // Get myCenter3D
    myCenter3D = TheCircle->Location();
  }
  else
  {
    mypolyg.SetPnt(0, TheCircle->Location());
    // Get myCenter3D
    myCenter3D = mypolyg.Pnt(0);
  }
}

//=======================================================================
//function : Select3D_SensitiveCircle
//purpose  :
//=======================================================================

Select3D_SensitiveCircle::Select3D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& OwnerId,
                                                   const Handle(TColgp_HArray1OfPnt)& Thepolyg3d,
                                                   const Standard_Boolean FilledCircle):
Select3D_SensitivePoly(OwnerId, Thepolyg3d),
myFillStatus(FilledCircle),
mystart(0),
myend(0)
{
  if (mypolyg.Size() != 1)
    ComputeCenter3D();
  else
    myCenter3D = mypolyg.Pnt(0);
}

//=======================================================================
//function : Select3D_SensitiveCircle
//purpose  :
//=======================================================================

Select3D_SensitiveCircle::Select3D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& OwnerId,
                                                   const TColgp_Array1OfPnt& Thepolyg3d,
                                                   const Standard_Boolean FilledCircle):
Select3D_SensitivePoly(OwnerId, Thepolyg3d),
myFillStatus(FilledCircle),
mystart(0),
myend(0)
{
  if (mypolyg.Size() != 1)
    ComputeCenter3D();
  else
    myCenter3D = mypolyg.Pnt(0);
}

//=======================================================================
//function : Matches
//purpose  :
//=======================================================================

Standard_Boolean Select3D_SensitiveCircle::Matches (const SelectBasics_PickArgs& thePickArgs,
                                                    Standard_Real& theMatchDMin,
                                                    Standard_Real& theMatchDepth)
{
  Standard_Integer aSize = mypolyg.Size();
  Standard_Integer aDetectedIndex = -1;
  gp_XY aPickXY (thePickArgs.X(), thePickArgs.Y());
  if (aSize != 1)
  {
    Standard_Boolean Found = Standard_False;
    Standard_Integer anIndex = 0;

    if(!myFillStatus)
    {
      while(anIndex < aSize-2 && !Found)
      {
        Standard_Integer TheStat =
          Select3D_SensitiveTriangle::Status(mypolyg.Pnt2d(anIndex),
                                             mypolyg.Pnt2d(anIndex+1),
                                             mypolyg.Pnt2d(anIndex+2),
                                             aPickXY, thePickArgs.Tolerance(),
                                             theMatchDMin);
        Found = (TheStat != 2);
        if (Found)
        {
          aDetectedIndex = anIndex;
        }

        anIndex += 2;
      }
    }
    else
    {
      Standard_Real Xmin,Ymin,Xmax,Ymax;

      // Get coordinates of the bounding box
      Bnd_Box2d(mybox2d).Get(Xmin,Ymin,Xmax,Ymax);
      TColgp_Array1OfPnt2d anArrayOf2dPnt(1, aSize);

      // Fill anArrayOf2dPnt with points from mypolig2d
      Points2D(anArrayOf2dPnt);

      CSLib_Class2d anInOutTool (anArrayOf2dPnt,
                                 thePickArgs.Tolerance(),
                                 thePickArgs.Tolerance(),
                                 Xmin, Ymin, Xmax, Ymax);

      // Method SiDans returns the status :
      //  1 - the point is inside the circle
      //  0 - the point is on the circle
      // -1 - the point is outside the circle
      Standard_Integer aStat = anInOutTool.SiDans (gp_Pnt2d (aPickXY));
      if(aStat != -1)
      {
        // Compute DMin (a distance between the center and the point)
        theMatchDMin = gp_XY (myCenter2D.x - aPickXY.X(), myCenter2D.y - aPickXY.Y()).Modulus();

        theMatchDepth = ComputeDepth (thePickArgs.PickLine(), aDetectedIndex);

        return !thePickArgs.IsClipped (theMatchDepth);
      }

      return Standard_False;
    }

    if (Found)
    {
      theMatchDepth = ComputeDepth (thePickArgs.PickLine(), aDetectedIndex);

      return !thePickArgs.IsClipped (theMatchDepth);
    }

    return Standard_False;
  }

  // Circle degenerates into point
  theMatchDMin = gp_Pnt2d(aPickXY).Distance(mypolyg.Pnt2d(0));
  if (theMatchDMin <= thePickArgs.Tolerance() * SensitivityFactor())
  {
    theMatchDepth = ComputeDepth (thePickArgs.PickLine(), aDetectedIndex);

    return !thePickArgs.IsClipped (theMatchDepth);
  }

  return Standard_False;
}

//=======================================================================
//function : Matches
//purpose  :
//=======================================================================

Standard_Boolean Select3D_SensitiveCircle::
Matches(const Standard_Real XMin,
        const Standard_Real YMin,
        const Standard_Real XMax,
        const Standard_Real YMax,
        const Standard_Real aTol)
{
  Bnd_Box2d abox;
  abox.Update(Min(XMin,XMax),Min(YMin,YMax),Max(XMin,XMax),Max(YMin,YMax));
  abox.Enlarge(aTol);

  for(Standard_Integer anIndex=0;anIndex<mypolyg.Size();anIndex++)
  {
    if(abox.IsOut(mypolyg.Pnt2d(anIndex)))
      return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : Matches
//purpose  :
//=======================================================================

Standard_Boolean Select3D_SensitiveCircle::
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

//=======================================================================
//function : ArrayBounds
//purpose  :
//=======================================================================

void Select3D_SensitiveCircle::
ArrayBounds(Standard_Integer & Low,
            Standard_Integer & Up) const
{
    Low = 0;
    Up = mypolyg.Size()-1;
}

//=======================================================================
//function : GetPoint3d
//purpose  :
//=======================================================================

gp_Pnt Select3D_SensitiveCircle::
GetPoint3d(const Standard_Integer Rank) const
{
  if(Rank>=0 && Rank<mypolyg.Size())
    return mypolyg.Pnt(Rank);
  return gp_Pnt();
}


//=======================================================================
//function : Dump
//purpose  :
//=======================================================================

void Select3D_SensitiveCircle::Dump(Standard_OStream& S,const Standard_Boolean FullDump) const
{
  Standard_Integer aSize = mypolyg.Size();

  S<<"\tSensitiveCircle 3D :";

  Standard_Boolean isclosed = 1== aSize;
  if(isclosed)
    S<<"(Closed Circle)"<<endl;
  else
    S<<"(Arc Of Circle)"<<endl;

  if(HasLocation())
    S<<"\t\tExisting Location"<<endl;


  if(FullDump)
  {
    gp_XYZ aCenter = myCenter3D;
    Standard_Real R = (aCenter-mypolyg.Pnt(0)).Modulus();

    S<<"\t\t Center : ("<<aCenter.X()<<" , "<<aCenter.Y()<<" , "<<aCenter.Z()<<" )"<<endl;
    S<<"\t\t Radius :"<<R<<endl;
  }
}

//=======================================================================
//function : ComputeDepth
//purpose  :
//=======================================================================

Standard_Real Select3D_SensitiveCircle::ComputeDepth (const gp_Lin& thePickLine,
                                                      const Standard_Integer theDetectedIndex) const
{
  gp_XYZ aCDG;
  if (theDetectedIndex == -1)
  {
    aCDG = myCenter3D;
  }
  else
  {
    aCDG += mypolyg.Pnt (theDetectedIndex);
    aCDG += mypolyg.Pnt (theDetectedIndex + 1);
    aCDG += mypolyg.Pnt (theDetectedIndex + 2);
    aCDG /= 3.;
  }

  return ElCLib::Parameter (thePickLine, gp_Pnt (aCDG));
}

//=======================================================================
//function : GetConnected
//purpose  :
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveCircle::GetConnected(const TopLoc_Location& theLocation)
{
  // Create a copy of this
  Handle(Select3D_SensitiveEntity) aNewEntity;
  // this was constructed using Handle(Geom_Circle)
  if(!myCircle.IsNull())
  {
    if((myend-mystart) > Precision::Confusion())
    {
      // Arc
      aNewEntity = new Select3D_SensitiveCircle(myOwnerId, myCircle, mystart, myend, myFillStatus);
    }
    else
    {
      // Circle
      aNewEntity = new Select3D_SensitiveCircle(myOwnerId, myCircle, myFillStatus);
    }
  }
  // this was constructed using TColgp_Array1OfPnt
  else
  {
    Standard_Integer aSize = mypolyg.Size();
    TColgp_Array1OfPnt aPolyg(1, aSize);
    for(Standard_Integer anIndex = 1; anIndex <= aSize; ++anIndex)
    {
      aPolyg.SetValue(anIndex, mypolyg.Pnt(anIndex-1));
    }
    aNewEntity = new Select3D_SensitiveCircle(myOwnerId, aPolyg, myFillStatus);
  }

  if(HasLocation())
    aNewEntity->SetLocation(Location());

  aNewEntity->UpdateLocation(theLocation);

  return aNewEntity;
}

//=======================================================================
//function : Project
//purpose  :
//=======================================================================

void Select3D_SensitiveCircle::Project(const Handle_Select3D_Projector &aProjector)
{
  Select3D_SensitivePoly::Project(aProjector);
  gp_Pnt2d aCenter;
  aProjector->Project(myCenter3D, aCenter);
  myCenter2D = aCenter;
}

//=======================================================================
//function : ComputeCenter3D
//purpose  :
//=======================================================================

void Select3D_SensitiveCircle::ComputeCenter3D()
{
  gp_XYZ aCenter;
  Standard_Integer nbpoints = mypolyg.Size();
  if (nbpoints != 1)
  {
    // The mass of points system
    Standard_Integer aMass = nbpoints - 1;
    // Find the circle barycenter
    for (Standard_Integer anIndex = 0; anIndex < nbpoints-1; ++anIndex)
    {
      aCenter += mypolyg.Pnt(anIndex);
    }
    myCenter3D = aCenter / aMass;
  }
  else
  {
    myCenter3D = mypolyg.Pnt(0);
  }
}
