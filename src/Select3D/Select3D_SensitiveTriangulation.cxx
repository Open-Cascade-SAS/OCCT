// Created on: 1997-05-15
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
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

//Modified      Thur Apr 09 98 by rob : No more computation of free edges.
//                                      fix bug on Compute Depth (don't forget
//                                      Location...)

#define BUC60858	//GG 27/03/01 Avoid to crash when selecting
//			a triangle containing confused or aligned points.

#include <Select3D_SensitiveTriangulation.ixx>
#include <gp_Pnt2d.hxx>
#include <Poly.hxx>
#include <Poly_Connect.hxx>
#include <CSLib_Class2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Precision.hxx>
#include <ElCLib.hxx>
#include <CSLib_Class2d.hxx>


static Standard_Integer S3D_NumberOfFreeEdges(const Handle(Poly_Triangulation)& Trg)
{
  Standard_Integer nFree = 0;
  Poly_Connect pc(Trg);
  Standard_Integer t[3];
  Standard_Integer i,j;
  for (i = 1; i <= Trg->NbTriangles(); i++) {
    pc.Triangles(i,t[0],t[1],t[2]);
    for (j = 0; j < 3; j++)
      if (t[j] == 0) nFree++;
  }
  return nFree;
}
static Standard_Boolean S3D_STriangul_NearSegment (const gp_XY& p0, const gp_XY& p1, const gp_XY& TheP,
                                                   const Standard_Real aTol, Standard_Real& aDMin)
{
  Bnd_Box2d B;
  B.SetVoid();
  B.Set(p0);
  B.Update(p1.X(),p1.Y());
  B.Enlarge(aTol*3);
  if(B.IsOut(TheP)) return Standard_False;

  gp_XY V01(p1);V01-=p0;
  gp_XY Vec(TheP);Vec -= p0;

  Standard_Real u = Vec*V01.Normalized();
  if(u<-aTol) return Standard_False;
  Standard_Real u1 = u-aTol;
  Standard_Real modmod = V01.SquareModulus();
  if(u1*u1> modmod) return Standard_False;

  gp_XY N01 (-V01.Y(),V01.X());
  N01.Normalize();
  aDMin = Abs (Vec * N01);
  return aDMin <= aTol;
}

// static Standard_Real S3D_SquareDistanceFromEdge(gp_Pnt2d PCur,
// 						gp_Pnt2d PEdg1,
// 						gp_Pnt2d PEdg2,
// 						const Standard_Real TolTol)
// {
//   gp_XY VEdg (PEdg1.XY());
//   gp_XY VCur (PEdg1.XY());
//   VEdg-= PEdg2.XY();
//   VCur-=PCur.XY();
//   Standard_Real long1 = VEdg.SquareModulus();

//   if(long1<=TolTol)
//     return VCur.SquareModulus();
//   Standard_Real Val = VEdg^VCur;
//   return Val*Val/long1;

// }

static Standard_Boolean S3D_IsEdgeIn(const Standard_Integer e1,
                                     const Standard_Integer e2,
                                     const Standard_Integer N1,
                                     const Standard_Integer N2,
                                     const Standard_Integer N3)
{
  Standard_Integer bid1  = (e1 == N1) ? N1 : ((e1 == N2) ? N2 : ( e1==N3 ? N3 : 0));
  if(bid1==0) return Standard_False;
  Standard_Integer bid2  = (e2 == N1) ? N1 : ((e2 == N2) ? N2 : ( e2==N3 ? N3 : 0));

  if(bid2==0 || bid2 ==bid1) return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : Select3D_SensitiveTriangulation
//purpose  :
//=======================================================================

Select3D_SensitiveTriangulation::
Select3D_SensitiveTriangulation(const Handle(SelectBasics_EntityOwner)& OwnerId,
                                const Handle(Poly_Triangulation)& Trg,
                                const TopLoc_Location& Loc,
                                const Standard_Boolean InteriorFlag):
Select3D_SensitiveEntity(OwnerId),
myTriangul(Trg),
myiniloc(Loc),
myIntFlag(InteriorFlag),
myNodes2d(1,Trg->NbNodes()),
myDetectedTr(-1)
{
  // calculate free edges and cdg 3d of the triangulation:
  // This code should have been integrated in poly_triangulation...

  Standard_Integer fr = 1;
  const Poly_Array1OfTriangle& triangles = myTriangul->Triangles();
  const TColgp_Array1OfPnt& Nodes = myTriangul->Nodes();
  Standard_Integer nbTriangles (myTriangul->NbTriangles());
  gp_XYZ cdg(0,0,0);
  Standard_Integer n[3];

  // to find connections in case when the border is not concerned...
  if(!myIntFlag)
  {
    myFreeEdges = new TColStd_HArray1OfInteger(1,2*S3D_NumberOfFreeEdges(Trg));
    TColStd_Array1OfInteger& FreeE = myFreeEdges->ChangeArray1();
    Poly_Connect pc(myTriangul);
    Standard_Integer t[3];
    Standard_Integer i,j;
    for ( i = 1; i <= nbTriangles; i++)
    {
      pc.Triangles(i,t[0],t[1],t[2]);
      triangles(i).Get(n[0],n[1],n[2]);
      cdg += (Nodes(n[0]).XYZ() + Nodes(n[1]).XYZ()+ Nodes(n[2]).XYZ())/3.;
      for (j = 0; j < 3; j++)
      {
        Standard_Integer k = (j+1) % 3;
        if (t[j] == 0)
        {
          FreeE(fr)  = n[j];
          FreeE(fr+1)= n[k];
          fr += 2;
        }
      }
    }
  }
  else{
    for (Standard_Integer i = 1; i <= nbTriangles; i++)
    {
      triangles(i).Get(n[0],n[1],n[2]);
      cdg += (Nodes(n[0]).XYZ() + Nodes(n[1]).XYZ()+ Nodes(n[2]).XYZ())/3.;
    }
  }

  if(nbTriangles!=0) cdg /= nbTriangles;
  myCDG3D = gp_Pnt(cdg);

  ComputeTotalTrsf();

  if(myTrsf.Form()!=gp_Identity)
    myCDG3D.Transform(myTrsf);
}


//=======================================================================
//function : Select3D_SensitiveTriangulation
//purpose  :
//=======================================================================

Select3D_SensitiveTriangulation::
Select3D_SensitiveTriangulation(const Handle(SelectBasics_EntityOwner)& OwnerId,
                                const Handle(Poly_Triangulation)& Trg,
                                const TopLoc_Location& Loc,
                                const Handle(TColStd_HArray1OfInteger)& FreeEdges,
                                const gp_Pnt& TheCDG,
                                const Standard_Boolean InteriorFlag):
Select3D_SensitiveEntity(OwnerId),
myTriangul(Trg),
myiniloc(Loc),
myCDG3D(TheCDG),
myFreeEdges(FreeEdges),
myIntFlag(InteriorFlag),
myNodes2d(1,Trg->NbNodes()),
myDetectedTr(-1)
{
}

//=======================================================================
//function : Project
//purpose  :
//=======================================================================

void Select3D_SensitiveTriangulation::Project(const Handle(Select3D_Projector)& aPrj)
{
  mybox2d.SetVoid();
  const TColgp_Array1OfPnt& Nodes = myTriangul->Nodes();

  gp_Pnt2d ProjPT;

  for(Standard_Integer I=1;I<=myTriangul->NbNodes();I++){
    if(myTrsf.Form()!=gp_Identity)
      aPrj->Project(Nodes(I).Transformed(myTrsf),ProjPT);
    else
      aPrj->Project(Nodes(I),ProjPT);

    myNodes2d.SetValue(I,ProjPT);
    mybox2d.Add(ProjPT);
  }

  aPrj->Project(myCDG3D,myCDG2D);
}

//=======================================================================
//function : Areas
//purpose  :
//=======================================================================

void Select3D_SensitiveTriangulation::Areas(SelectBasics_ListOfBox2d& boxes)
{
  boxes.Append(mybox2d);
}

//=======================================================================
//function : Matches
//purpose  :
//=======================================================================

Standard_Boolean Select3D_SensitiveTriangulation::Matches (const SelectBasics_PickArgs& thePickArgs,
                                                           Standard_Real& theMatchDMin,
                                                           Standard_Real& theMatchDepth)
{
  theMatchDMin = Precision::Infinite();
  gp_XY BidPoint (thePickArgs.X(), thePickArgs.Y());
  myDetectedTr = -1;
  const Poly_Array1OfTriangle& triangles = myTriangul->Triangles();

  // it is checked if we are inside the triangle 2d.
  if(myIntFlag)
  {
    gp_Lin aPickingLine = thePickArgs.PickLine();

    if (myTrsf.Form() != gp_Identity)
    {
      aPickingLine.Transform (myTrsf.Inverted());
    }

    Standard_Real aMinDepth = Precision::Infinite();
    const TColgp_Array1OfPnt& Nodes = myTriangul->Nodes();
    for (Standard_Integer itr=1; itr<=myTriangul->NbTriangles(); itr++)
    {
      Standard_Integer n1,n2,n3;
      triangles(itr).Get(n1,n2,n3);
      const gp_XY& aPnt2d1 = myNodes2d(n1).XY();
      const gp_XY& aPnt2d2 = myNodes2d(n2).XY();
      const gp_XY& aPnt2d3 = myNodes2d(n3).XY();
      gp_XY aUV;
      Standard_Real aDistSquare = Poly::PointOnTriangle (aPnt2d1, aPnt2d2, aPnt2d3, BidPoint, aUV);
      if (aDistSquare > thePickArgs.Tolerance() * thePickArgs.Tolerance())
        continue;

      // get interpolated depth of the triangle nodes
      Standard_Real aDepth1 = ElCLib::Parameter (aPickingLine, Nodes(n1));
      Standard_Real aDepth2 = ElCLib::Parameter (aPickingLine, Nodes(n2));
      Standard_Real aDepth3 = ElCLib::Parameter (aPickingLine, Nodes(n3));
      Standard_Real aDepth = aDepth1 + aUV.X() * (aDepth2 - aDepth1) +
                                       aUV.Y() * (aDepth3 - aDepth1);

      // accept triangle with lowest depth and within defined depth interval
      if (aDepth < aMinDepth && !thePickArgs.IsClipped(aDepth))
      {
        aMinDepth = aDepth;
        myDetectedTr = itr;
        theMatchDMin = Sqrt (aDistSquare);
      }
    }
  }

  //    Case only Test on Border of the triangulation...
  //
  else
  {
    //Standard_Integer ifirst;
    TColStd_Array1OfInteger& FreeE = myFreeEdges->ChangeArray1();
    Standard_Integer nn = FreeE.Length(), Node1,Node2;
    //Standard_Real LEdg;
    //Standard_Real DMinDMin,TolTol = aTol*aTol;

    for (Standard_Integer ifri =1; ifri <= nn && myDetectedTr < 0; ifri+=2)
    {
      Node1 = FreeE(ifri);
      Node2 = FreeE(ifri+1);
      if (S3D_STriangul_NearSegment (myNodes2d(Node1).XY(),
                                     myNodes2d(Node2).XY(),
                                     BidPoint, thePickArgs.Tolerance(), theMatchDMin))
      {
        for(Standard_Integer itr=1; itr <= myTriangul->NbTriangles(); itr++)
        {
          Standard_Integer n1,n2,n3;
          triangles(itr).Get(n1,n2,n3);
          if(S3D_IsEdgeIn(Node1,Node2,n1,n2,n3))
          {
            myDetectedTr = itr;
            break; // return first found; selection of closest is not implemented yet
          }
        }
      }
    }
  }
  if ( myDetectedTr <= 0 )
    return Standard_False;

  // get precise depth for the detected triangle
  theMatchDepth = ComputeDepth (thePickArgs.PickLine(), myDetectedTr);

  // this test should not fail if the topmost triangle is taken from the
  // first if-case block (for other cases this test make sense?)
  return !thePickArgs.IsClipped (theMatchDepth);
}

//=======================================================================
//function : Matches
//purpose  :
//=======================================================================

Standard_Boolean Select3D_SensitiveTriangulation::
Matches(const Standard_Real XMin,
        const Standard_Real YMin,
        const Standard_Real XMax,
        const Standard_Real YMax,
        const Standard_Real aTol)
{
  Bnd_Box2d B;
  B.Update(Min(XMin,XMax)-aTol,
           Min(YMin,YMax)-aTol,
           Max(XMin,XMax)+aTol,
           Max(YMin,YMax)+aTol);

  for(Standard_Integer i=myNodes2d.Lower();i<=myNodes2d.Upper();i++)
  {
    if(B.IsOut(myNodes2d(i)))
      return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : Matches
//purpose  :
//=======================================================================

Standard_Boolean Select3D_SensitiveTriangulation::
Matches (const TColgp_Array1OfPnt2d& aPoly,
         const Bnd_Box2d& aBox,
         const Standard_Real aTol)
{
  Standard_Real Umin,Vmin,Umax,Vmax;
  aBox.Get(Umin,Vmin,Umax,Vmax);
  CSLib_Class2d aClassifier2d(aPoly,aTol,aTol,Umin,Vmin,Umax,Vmax);

  for(Standard_Integer j=1;j<=myNodes2d.Length();j++)
  {
    Standard_Integer RES = aClassifier2d.SiDans(myNodes2d(j));
    if(RES!=1) return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : Status
//purpose  :
//=======================================================================

Standard_Integer Select3D_SensitiveTriangulation::
Status (const gp_XY& TheP,
        const gp_XY& Proj0,
        const gp_XY& Proj1,
        const gp_XY& Proj2,
        const Standard_Real aTol,
        Standard_Real& DD) const
{
  return Select3D_SensitiveTriangle::Status(Proj0,Proj1,Proj2,TheP,aTol,DD);
}

//=======================================================================
//function : IsFree
//purpose  :
//=======================================================================

Standard_Boolean Select3D_SensitiveTriangulation::IsFree(const Standard_Integer IndexOfTriangle,
                                                         Standard_Integer& FoundIndex) const
{
  FoundIndex=-1;
  Standard_Integer n[3];
  const Poly_Array1OfTriangle& triangles = myTriangul->Triangles();
  triangles(IndexOfTriangle).Get(n[0],n[1],n[2]);
  TColStd_Array1OfInteger& FreeE = myFreeEdges->ChangeArray1();

  for(Standard_Integer I=1;I<=FreeE.Length() && FoundIndex==-1;I+=2)
  {
    if(FreeE(I) == n[0])
    {
      if(FreeE(I+1)== n[1] || FreeE(I+1)== n[2])
        FoundIndex=I;
    }
    else if(FreeE(I) == n[1])
    {
      if(FreeE(I+1)== n[0] || FreeE(I+1)== n[2])
        FoundIndex=I;
    }
    else if(FreeE(I) == n[2])
    {
      if(FreeE(I+1)== n[0] || FreeE(I+1)== n[1])
        FoundIndex=I;
    }
  }

  return FoundIndex!=-1;
}


//=======================================================================
//function : GetConnected
//purpose  :
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveTriangulation::
GetConnected(const TopLoc_Location& aLoc)
{
  Handle(Select3D_SensitiveTriangulation) NiouEnt =
    new Select3D_SensitiveTriangulation(myOwnerId,myTriangul,myiniloc,myFreeEdges,myCDG3D,myIntFlag);

  if(HasLocation())
    NiouEnt->SetLocation(Location());
//  TopLoc_Location TheLocToApply = HasLocation() ?  Location()*aLoc : aLoc;
//  if(!TheLocToApply.IsIdentity())
  NiouEnt->UpdateLocation(aLoc);

  return NiouEnt;
}

//=======================================================================
//function : ResetLocation
//purpose  :
//=======================================================================

void Select3D_SensitiveTriangulation::ResetLocation()
{
  Select3D_SensitiveEntity::ResetLocation();
  ComputeTotalTrsf();
}

//=======================================================================
//function : SetLocation
//purpose  :
//=======================================================================

void Select3D_SensitiveTriangulation::SetLocation(const TopLoc_Location& aLoc)
{
  Select3D_SensitiveEntity::SetLocation(aLoc);
  ComputeTotalTrsf();
}


//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
void Select3D_SensitiveTriangulation::Dump(Standard_OStream& S,const Standard_Boolean FullDump) const
{
  S<<"\tSensitiveTriangulation 3D :"<<endl;
  if(myiniloc.IsIdentity())
    S<<"\t\tNo Initial Location"<<endl;
  else
    S<<"\t\tExisting Initial Location"<<endl;
  if(HasLocation())
    S<<"\t\tExisting Location"<<endl;

  S<<"\t\tNb Triangles : "<<myTriangul->NbTriangles()<<endl;
  S<<"\t\tNb Nodes     : "<<myTriangul->NbNodes()<<endl;
  S<<"\t\tNb Free Edges: "<<myFreeEdges->Length()/2<<endl;

  if(FullDump)
  {
//    S<<"\t\t\tOwner:"<<myOwnerId<<endl;
    Select3D_SensitiveEntity::DumpBox(S,mybox2d);
  }
}

//=======================================================================
//function : ComputeDepth
//purpose  :
//=======================================================================

Standard_Real Select3D_SensitiveTriangulation::ComputeDepth(const gp_Lin& thePickLine,
                                                            const Standard_Integer theTriangle) const
{
  if (theTriangle == -1)
  {
    return Precision::Infinite(); // currently not implemented...
  }

  const Poly_Array1OfTriangle& triangles = myTriangul->Triangles();
  const TColgp_Array1OfPnt& Nodes = myTriangul->Nodes();

  Standard_Integer n1,n2,n3;
  triangles (theTriangle).Get (n1,n2,n3);
  gp_Pnt P[3]={Nodes(n1),Nodes(n2),Nodes(n3)};

  if (myTrsf.Form() != gp_Identity)
  {
    for (Standard_Integer i =0; i<=2; i++)
    {
      P[i].Transform (myTrsf);
    }
  }

  // formula calculate the parameter of the point on the intersection
  // t = (P1P2 ^P1P3)* OP1  / ((P1P2^P1P3)*Dir)
  Standard_Real prof (Precision::Infinite());
  gp_Pnt Oye  = thePickLine.Location(); // origin of the target line eye/point...
  gp_Dir Dir  = thePickLine.Direction();

  gp_Vec Vtr[3];
  for (Standard_Integer i=0; i<=2; i++)
  {
    Vtr[i] = gp_Vec (P[i%3], P[(i+1)%3]);
  }

  Vtr[2] = -Vtr[2];

  // remove singular cases immediately...
  Standard_Integer SingularCase (-1);
  if (Vtr[0].SquareMagnitude() <= Precision::Confusion())
  {
    SingularCase = 0;
  }

  if (Vtr[1].SquareMagnitude() <= Precision::Confusion())
  {
    SingularCase = (SingularCase == -1) ? 1 : 2;
  }

#ifdef BUC60858
  if (Vtr[2].SquareMagnitude() <= Precision::Confusion())
  {
    if( SingularCase < 0 ) SingularCase = 1;
  }
#endif

  // 3 pts mixed...
  if (SingularCase ==2)
  {
    prof = ElCLib::Parameter (thePickLine, P[0]);
    return prof;
  }

  if (SingularCase!=0)
  {
    Vtr[0].Normalize();
  }

  if (SingularCase!=1 && SingularCase!=2)
  {
    Vtr[2].Normalize();
  }

  gp_Vec OPo (Oye, P[0]);

  // 2 points mixed... the intersection between the segment and the target line eye/point.
  if (SingularCase!=-1)
  {
    gp_Vec V = SingularCase==0 ? Vtr[2] : Vtr[0];
    gp_Vec Det = Dir^V;
    gp_Vec VSM = OPo^V;

    if (Det.X() > Precision::Confusion())
    {
      prof = VSM.X() / Det.X();
    }
    else if (Det.Y() > Precision::Confusion())
    {
      prof = VSM.Y() / Det.Y();
    }
    else if (Det.Z() > Precision::Confusion())
    {
      prof = VSM.Z() / Det.Z();
    }
  }
  else
  {
    Standard_Real val1 = OPo.DotCross (Vtr[0], Vtr[2]);
    Standard_Real val2 = Dir.DotCross (Vtr[0], Vtr[2]);

    if (Abs (val2) > Precision::Confusion())
    {
      prof = val1 / val2;
    }
  }

  if (prof == Precision::Infinite())
  {
    prof= ElCLib::Parameter (thePickLine, P[0]);
    prof = Min (prof, ElCLib::Parameter (thePickLine, P[1]));
    prof = Min (prof, ElCLib::Parameter (thePickLine, P[2]));
  }

  return prof;
}

//=======================================================================
//function : DetectedTriangle
//purpose  :
//=======================================================================

Standard_Boolean Select3D_SensitiveTriangulation::
DetectedTriangle(gp_Pnt& P1,
                 gp_Pnt& P2,
                 gp_Pnt& P3) const
{
  if(myDetectedTr==-1) return Standard_False; // currently not implemented...
  const Poly_Array1OfTriangle& triangles = myTriangul->Triangles();
  const TColgp_Array1OfPnt& Nodes = myTriangul->Nodes();
  Standard_Integer n1,n2,n3;
  triangles(myDetectedTr).Get(n1,n2,n3);

  P1 = Nodes(n1);
  P2 = Nodes(n2);
  P3 = Nodes(n3);
  if(myTrsf.Form()!=gp_Identity)
  {
    P1.Transform(myTrsf);
    P2.Transform(myTrsf);
    P3.Transform(myTrsf);
  }

  return Standard_True;
}

//=============================================================================
// Function : DetectedTriangle2d
// Purpose  :
//=============================================================================

Standard_Boolean Select3D_SensitiveTriangulation::
DetectedTriangle2d(gp_Pnt2d& P1,
                   gp_Pnt2d& P2,
                   gp_Pnt2d& P3) const
{
  if(myDetectedTr==-1)
    return Standard_False; //  currently not implemented...
  const Poly_Array1OfTriangle& triangles = myTriangul->Triangles();
  Standard_Integer n1,n2,n3;
  triangles( myDetectedTr ).Get(n1,n2,n3);

  int aLower = myNodes2d.Lower();
  int anUpper = myNodes2d.Upper();
  if ( n1 >= aLower && n1 <= anUpper &&
       n2 >= aLower && n2 <= anUpper &&
       n3 >= aLower && n3 <= anUpper )
  {
    P1 = myNodes2d.Value( n1 );
    P2 = myNodes2d.Value( n2 );
    P3 = myNodes2d.Value( n3 );
    return Standard_True;
  }
  else
    return Standard_False;
}

//=============================================================================
// Function : ComputeTotalTrsf
// Purpose  :
//=============================================================================

void Select3D_SensitiveTriangulation::ComputeTotalTrsf()
{
  Standard_Boolean hasloc = (HasLocation() || !myiniloc.IsIdentity());

  if(hasloc)
  {
    if(myiniloc.IsIdentity())
      myTrsf = Location().Transformation();
    else if(HasLocation())
    {
      myTrsf = (Location()*myiniloc).Transformation();
    }
    else
      myTrsf = myiniloc.Transformation();
  }
  else
  {
    gp_Trsf TheId;
    myTrsf = TheId;
  }
}
