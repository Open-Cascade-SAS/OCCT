// Created on: 1996-03-18
// Created by: Stagiaire Frederic CALOONE
// Copyright (c) 1996-1999 Matra Datavision
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

// Modified:	Wed Mar  5 09:45:42 1997
//    by:	Joelle CHAUVET
//              ajout de la methode DefPlan et des options POption et NOption
//              modif des methodes Create et BasePlan
// Modified:	Thu Mar 20 09:15:36 1997
//    by:	Joelle CHAUVET
//              correction sur le tri des valeurs propres quand valeurs egales

#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <GeomLib.hxx>
#include <GeomPlate_Aij.hxx>
#include <GeomPlate_BuildAveragePlane.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Jacobi.hxx>
#include <math_Matrix.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
//function : GeomPlate_BuildAveragePlane
//purpose  : 
//=======================================================================
GeomPlate_BuildAveragePlane::
GeomPlate_BuildAveragePlane(const Handle(TColgp_HArray1OfPnt)& Pts,
			    const Standard_Integer NbBoundPoints,
                            const Standard_Real Tol,
                            const Standard_Integer POption,
                            const Standard_Integer NOption) :
myPts(Pts),
myTol(Tol),
myNbBoundPoints(NbBoundPoints)
{
  gp_Vec OZ = DefPlan(NOption);

  if (OZ.SquareMagnitude()>0) {

    if (POption==1) {
      myPlane = new Geom_Plane(myG,OZ);
      myOX = myPlane->Pln().XAxis().Direction();
      myOY = myPlane->Pln().YAxis().Direction();
    }
    else {
      BasePlan(OZ);
      gp_Dir NDir(myOX^myOY);
      gp_Dir UDir(myOX);
      gp_Ax3 triedre(myG,NDir,UDir);
      myPlane = new Geom_Plane(triedre);
    }
    Standard_Integer i,nb=myPts->Length();
    gp_Pln P=myPlane->Pln();
    ElSLib::Parameters(P,myG,myUmax,myVmax);
    myUmin=myUmax;
    myVmin=myVmax;
    Standard_Real U=0,V=0;
    for (i=1; i<=nb; i++) {
      ElSLib::Parameters(P,myPts->Value(i),U,V);
      if ( myUmax < U ) myUmax=U;
      if ( myUmin > U ) myUmin=U;
      if ( myVmax < V ) myVmax=V;
      if ( myVmin > V ) myVmin=V;
    }
  }

  if (IsLine()) {
    myLine = new Geom_Line(myG,myOX);
  }
}

GeomPlate_BuildAveragePlane::GeomPlate_BuildAveragePlane( const TColgp_SequenceOfVec& Normals,
							  const Handle( TColgp_HArray1OfPnt )& Pts ) :
myPts(Pts)
{
  Standard_Integer i, j, k, n, m;

  gp_Vec BestVec;
  Standard_Integer NN = Normals.Length();

  if (NN == 1)
    BestVec = Normals(1);
  else if (NN == 2)
  {
    BestVec = Normals(1) + Normals(2);
    const Standard_Real aSqMagn = BestVec.SquareMagnitude();
    if(aSqMagn < Precision::SquareConfusion())
    {
      const Standard_Real aSq1 = Normals(1).SquareMagnitude(),
        aSq2 = Normals(2).SquareMagnitude();

      if(aSq1 > aSq2)
        BestVec = Normals(1).Normalized();
      else
        BestVec = Normals(2).Normalized();
    }
    else
    {
      BestVec.Divide(sqrt(aSqMagn));
    }      
  }
  else //the common case
    {
      Standard_Real MaxAngle = 0.;
      for (i = 1; i <= NN-1; i++)
	for (j = i+1; j <= NN; j++)
	  {
	    Standard_Real Angle = Normals(i).Angle( Normals(j) );
	    if (Angle > MaxAngle)
	      MaxAngle = Angle;
	  }
      MaxAngle *= 1.2;
      MaxAngle /= 2.;
      Standard_Integer Nint = 50;
      
      TColgp_Array1OfVec OptVec( 1, NN*(NN-1)/2 );
      TColStd_Array1OfReal OptScal( 1, NN*(NN-1)/2 );
      gp_Vec Vec, Vec1;
      gp_Dir Cross1, Cross2;
      
      k = 1;
      for (i = 1; i <= NN-1; i++)
        for (j = i+1; j <= NN; j++, k++)
        {
          OptScal(k) = RealFirst();  

          Standard_Real Step = MaxAngle/Nint;
          Vec = Normals(i) + Normals(j);

          const Standard_Real aSqMagn = Vec.SquareMagnitude();

          if(aSqMagn < Precision::SquareConfusion())
          {
            continue;
          }

          Vec.Divide(sqrt(aSqMagn));

          Cross1 = Normals(i) ^ Normals(j);
          Cross2 = Vec ^ Cross1;
          gp_Ax1 Axe( gp_Pnt(0,0,0), Cross2 );

          Vec1 = Vec.Rotated( Axe, -MaxAngle );
          //Vec2 = Vec.Rotated( Axe, MaxAngle );

          for (n = 0; n <= 2*Nint; n++)
          {
            Vec1.Rotate( Axe, Step );
            Standard_Real minScal = RealLast();
            for (m = 1; m <= NN; m++)
            {
              Standard_Real Scal = Vec1 * Normals(m);
              if (Scal < minScal)
                minScal = Scal;
            }
            if (minScal > OptScal(k))
            {
              OptScal(k) = minScal;
              OptVec(k) = Vec1;
            }
          }
        } // for i, for j
      //Find maximum among all maximums

      Standard_Real BestScal = RealFirst();
      Standard_Integer Index=0;
      for (k = 1; k <= OptScal.Length(); k++)
	if (OptScal(k) > BestScal)
	  {
	    BestScal = OptScal(k);
	    Index = k;
	  }
      BestVec = OptVec(Index);
    }

  //Making the plane myPlane
  gp_Ax2 Axe;
  Standard_Boolean IsSingular;
  TColgp_Array1OfPnt PtsArray( 1, myPts->Length() );
  for (i = 1; i <= myPts->Length(); i++)
    PtsArray(i) = myPts->Value(i);
  GeomLib::AxeOfInertia( PtsArray, Axe, IsSingular );
  gp_Dir BestDir( BestVec );
  gp_Dir XDir = BestDir ^ Axe.XDirection();
  XDir ^= BestDir;

  gp_Ax3 Axe3( Axe.Location(), BestDir, XDir );
  myPlane = new Geom_Plane( Axe3 );

  //Initializing myUmin, myVmin, myUmax, myVmax
  gp_Pln Pln = myPlane->Pln();
  ElSLib::Parameters( Pln, Axe.Location(), myUmax, myVmax );
  myUmin = myUmax;
  myVmin = myVmax;
  Standard_Real U,V;
  for (i = 1; i <= myPts->Length(); i++)
    {
      gp_Vec aVec( Pln.Location(), myPts->Value(i) );
      gp_Vec NormVec = Pln.Axis().Direction();
      NormVec = (aVec * NormVec) * NormVec;

      ElSLib::Parameters( Pln, myPts->Value(i).Translated( -NormVec ), U, V ); //????? Real projecting?
      if (U > myUmax)
	myUmax = U;
      if (U < myUmin)
	myUmin = U;
      if (V > myVmax)
	myVmax = V;
      if (V < myVmin)
	myVmin = V;
    }
  //Initializing myOX, myOY
  myOX = myPlane->Pln().XAxis().Direction();
  myOY = myPlane->Pln().YAxis().Direction();
}


//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

Handle(Geom_Plane) GeomPlate_BuildAveragePlane::Plane() const 
{
  Standard_NoSuchObject_Raise_if ( IsLine() , "Cannot use the function 'GeomPlate_BuildAveragePlane::Plane()', the Object is a 'Geom_Line'"); 
  return myPlane;
}


//=======================================================================
//function : MinMaxBox
//purpose  : 
//=======================================================================

void GeomPlate_BuildAveragePlane::MinMaxBox(Standard_Real& Umin, Standard_Real& Umax, Standard_Real& Vmin, Standard_Real& Vmax) const 
{
  Umax=myUmax;
  Umin=myUmin;
  Vmax=myVmax;
  Vmin=myVmin;
}





//=======================================================================
//function : DefPlan
//purpose  : 
//=======================================================================

gp_Vec GeomPlate_BuildAveragePlane::DefPlan(const Standard_Integer NOption)  
{
 
  gp_Pnt GB;
  gp_Vec A,B,C,D;
  gp_Vec OZ;
  Standard_Integer i,nb=myPts->Length();
  GB.SetCoord(0.,0.,0.);
  for (i=1; i<=nb; i++) {
      GB.SetCoord(1,(GB.Coord(1)+myPts->Value(i).Coord(1)));
      GB.SetCoord(2,(GB.Coord(2)+myPts->Value(i).Coord(2)));
      GB.SetCoord(3,(GB.Coord(3)+myPts->Value(i).Coord(3)));
  }
  myG.SetCoord(1,(GB.Coord(1)/nb));
  myG.SetCoord(2,(GB.Coord(2)/nb));
  myG.SetCoord(3,(GB.Coord(3)/nb));

  if (NOption==1) {
    gp_Ax2 Axe;
    Standard_Boolean IsSingular;
    GeomLib::AxeOfInertia( myPts->Array1(), Axe, IsSingular, myTol );

    myOX = Axe.XDirection();
    myOY = Axe.YDirection();

    OZ = Axe.Direction();

    if (myNbBoundPoints != 0 && myPts->Length() != myNbBoundPoints)
      {
	A.SetCoord(0.,0.,0.);
	for (i = 3; i <= myNbBoundPoints; i++)
	  {
	    B.SetCoord(myPts->Value(i-1).Coord(1)-myPts->Value(1).Coord(1),
		       myPts->Value(i-1).Coord(2)-myPts->Value(1).Coord(2),
		       myPts->Value(i-1).Coord(3)-myPts->Value(1).Coord(3));
	    C.SetCoord(myPts->Value(i).Coord(1)-myPts->Value(1).Coord(1),
		       myPts->Value(i).Coord(2)-myPts->Value(1).Coord(2),
		       myPts->Value(i).Coord(3)-myPts->Value(1).Coord(3));
	    D=B^C;
	    A.SetCoord(1,A.Coord(1)+D.Coord(1));
	    A.SetCoord(2,A.Coord(2)+D.Coord(2));
	    A.SetCoord(3,A.Coord(3)+D.Coord(3));
	  }
	gp_Vec OZ1 = A;
	Standard_Real theAngle = OZ.Angle( OZ1 );
	if (theAngle > M_PI/2)
	  theAngle = M_PI - theAngle;
	if (theAngle > M_PI/3)
	  OZ = OZ1;
      }
  }

  else if (NOption==2) {
    A.SetCoord(0.,0.,0.);
    for (i = 3; i <= myNbBoundPoints; i++) {
      B.SetCoord(myPts->Value(i-1).Coord(1)-myPts->Value(1).Coord(1),
	         myPts->Value(i-1).Coord(2)-myPts->Value(1).Coord(2),
	         myPts->Value(i-1).Coord(3)-myPts->Value(1).Coord(3));
      C.SetCoord(myPts->Value(i).Coord(1)-myPts->Value(1).Coord(1),
	         myPts->Value(i).Coord(2)-myPts->Value(1).Coord(2),
	         myPts->Value(i).Coord(3)-myPts->Value(1).Coord(3));
      D=B^C;
      A.SetCoord(1,A.Coord(1)+D.Coord(1));
      A.SetCoord(2,A.Coord(2)+D.Coord(2));
      A.SetCoord(3,A.Coord(3)+D.Coord(3));
    }
    OZ = A;
  }
  return OZ;
}

//=======================================================================
//function : BasePlan
//purpose  : 
//=======================================================================

void GeomPlate_BuildAveragePlane::BasePlan(const gp_Vec& OZ)  
{
    math_Matrix M (1, 3, 1, 3);
    M.Init(0.);
    gp_Vec Proj;
    Standard_Integer i,nb=myPts->Length();
    Standard_Real scal;

    for (i=1; i<=nb; i++) {
      Proj.SetCoord(1,myPts->Value(i).Coord(1) - myG.Coord(1));
      Proj.SetCoord(2,myPts->Value(i).Coord(2) - myG.Coord(2));
      Proj.SetCoord(3,myPts->Value(i).Coord(3) - myG.Coord(3));
      scal = Proj.Coord(1)*OZ.Coord(1)
	+ Proj.Coord(2)*OZ.Coord(2)
	+ Proj.Coord(3)*OZ.Coord(3);
      scal /= OZ.Coord(1)*OZ.Coord(1)
	+ OZ.Coord(2)*OZ.Coord(2)
	+ OZ.Coord(3)*OZ.Coord(3);
      Proj.SetCoord(1,Proj.Coord(1) - scal*OZ.Coord(1));
      Proj.SetCoord(2,Proj.Coord(2) - scal*OZ.Coord(2));
      Proj.SetCoord(3,Proj.Coord(3) - scal*OZ.Coord(3));
      M(1,1) += Proj.Coord(1)*Proj.Coord(1);
      M(2,2) += Proj.Coord(2)*Proj.Coord(2);
      M(3,3) += Proj.Coord(3)*Proj.Coord(3);
      M(1,2) += Proj.Coord(1)*Proj.Coord(2);
      M(1,3) += Proj.Coord(1)*Proj.Coord(3);
      M(2,3) += Proj.Coord(2)*Proj.Coord(3);
    }
    M(2,1) = M(1,2) ;
    M(3,1) = M(1,3) ;
    M(3,2) = M(2,3) ;
    math_Jacobi J(M);
    Standard_Real n1,n2,n3;
    math_Vector V1(1,3),V2(1,3),V3(1,3);
    n1=J.Value(1);
    n2=J.Value(2);
    n3=J.Value(3);

    Standard_Real r1 = Min(Min(n1,n2),n3), r2;
    Standard_Integer m1, m2, m3;
    if (r1==n1) {
      m1 = 1;
      r2 = Min(n2,n3);
      if (r2==n2) {
        m2 = 2;
        m3 = 3;
      }
      else {
        m2 = 3;
        m3 = 2;
      }
    }
    else {
      if (r1==n2) {
        m1 = 2 ;
        r2 = Min(n1,n3);
        if (r2==n1) {
          m2 = 1;
          m3 = 3;
        }
        else {
          m2 = 3;
          m3 = 1;
        }
      }
      else {
        m1 = 3 ;
        r2 = Min(n1,n2);
        if (r2==n1) {
          m2 = 1;
          m3 = 2;
        }
        else {
          m2 = 2;
          m3 = 1;
        }
      }
    }
    J.Vector(m1,V1);
    J.Vector(m2,V2);
    J.Vector(m3,V3);

    if (((Abs(n1)<=myTol)&&(Abs(n2)<=myTol))
          || ((Abs(n2)<=myTol)&&(Abs(n3)<=myTol))
          || ((Abs(n1)<=myTol)&&(Abs(n3)<=myTol))) {
      myOX.SetCoord(V3(1),V3(2),V3(3));
      myOY.SetCoord(0,0,0);
    }
    else {
      myOX.SetCoord(V3(1),V3(2),V3(3));
      myOY.SetCoord(V2(1),V2(2),V2(3));
    }
}

  

//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

Handle(Geom_Line) GeomPlate_BuildAveragePlane::Line() const
{
  Standard_NoSuchObject_Raise_if ( IsPlane() , "Cannot use the function 'GeomPlate_BuildAveragePlane::Line()', the Object is a 'Geom_Plane'");
  return myLine;
}

//=======================================================================
//function : IsPlane
//purpose  : 
//=======================================================================

Standard_Boolean GeomPlate_BuildAveragePlane::IsPlane() const
{
  gp_Vec OZ=myOX^myOY;
  if (OZ.SquareMagnitude()==0)
    return Standard_False;
  else
    return Standard_True;
      
}  

//=======================================================================
//function : IsLine
//purpose  : 
//=======================================================================

Standard_Boolean GeomPlate_BuildAveragePlane::IsLine() const
{
  gp_Vec OZ=myOX^myOY;
  if (OZ.SquareMagnitude()==0)
    return Standard_True;
  else
    return Standard_False;
}  


Standard_Boolean GeomPlate_BuildAveragePlane::HalfSpace( const TColgp_SequenceOfVec& NewNormals,
							 TColgp_SequenceOfVec& Normals,
							 GeomPlate_SequenceOfAij& Bset,
							 const Standard_Real LinTol,
							 const Standard_Real AngTol )
{
  Standard_Real SquareTol = LinTol * LinTol;

  TColgp_SequenceOfVec SaveNormals;
  GeomPlate_SequenceOfAij SaveBset;
  // 1
  SaveNormals = Normals;
  SaveBset = Bset;

  gp_Vec Cross, NullVec( 0, 0, 0 );
  GeomPlate_SequenceOfAij B1set, B2set;
  Standard_Integer i, j, k;
  
  i = 1;
  if (Normals.IsEmpty())
    {
      if (NewNormals.Length() == 1)
	{
	  Normals.Append( NewNormals.Last() );
	  return Standard_True;
	}
      // 2
      Cross = NewNormals(1) ^ NewNormals(2);
      if (Cross.SquareMagnitude() <= SquareTol)
	return Standard_False;

      Cross.Normalize();
      Bset.Append( GeomPlate_Aij( 1, 2, Cross ) );
      Bset.Append( GeomPlate_Aij( 2, 1, -Cross) );
      Normals.Append( NewNormals(1) );
      Normals.Append( NewNormals(2) );
      i = 3;
    }

  for (; i <= NewNormals.Length(); i++)
    {
      // 3
      Standard_Real Scal;
      for (j = 1; j <= Bset.Length(); j++)
	if ((Scal = Bset(j).Vec * NewNormals(i)) >= -LinTol)
	  B2set.Append( Bset(j) );

      Standard_Integer ii = Normals.Length()+1;
      for (j = 1; j <= ii-1; j++)
	{
	  if (Normals(j).SquareMagnitude() == 0.)
	    continue;
	  // 4
	  Cross = NewNormals(i) ^ Normals(j);
	  if (Cross.SquareMagnitude() <= SquareTol)
	    {
	      Normals = SaveNormals;
	      Bset = SaveBset;
	      return Standard_False;
	    }
	  Cross.Normalize();
	  Standard_Boolean isNew = Standard_True;
	  for (k = 1; k <= B2set.Length(); k++)
	    if (B2set(k).Vec.IsOpposite( -Cross, AngTol )) //if (B2set(k).Vec.IsEqual( Cross, LinTol, AngTol ))
	      {
		gp_Vec Cross1, Cross2;
		Standard_Integer ind1 = B2set(k).Ind1, ind2 = B2set(k).Ind2;
		if (ind1 == ii || ind2 == ii)
		  {
		    isNew = Standard_False;
		    break;
		  }
		Cross1 = Normals( ind1 ) ^ NewNormals(i);
		Cross2 = Normals( ind2 ) ^ NewNormals(i);
		if (Cross1.SquareMagnitude() <= SquareTol || Cross2.SquareMagnitude() <= SquareTol)
		  {
		    Normals = SaveNormals;
		    Bset = SaveBset;
		    return Standard_False;
		  }
		if (Cross1.IsOpposite( Cross2, AngTol ))
		  {
		    Cross2 = Normals( ind1 ) ^ Normals( ind2 );
		    if (Cross1.IsOpposite( Cross2, AngTol ))
		      {
			Normals = SaveNormals;
			Bset = SaveBset;
			return Standard_False;
		      }
		  }
		else
		  {
		    if (NewNormals(i).Angle( Normals( ind1 ) ) > NewNormals(i).Angle( Normals( ind2 ) ))
		      {
			B2set(k).Ind2 = ind1;
			B2set(k).Ind1 = ii;
		      }
		    else
		      B2set(k).Ind1 = ii;
		  }
		isNew = Standard_False;
		break;
	      }
	  if (isNew)
	    B1set.Append( GeomPlate_Aij( ii, j, Cross ) );

	  Cross.Reverse();
	  isNew = Standard_True;
	  for (k = 1; k <= B2set.Length(); k++)
	    if (B2set(k).Vec.IsOpposite( -Cross, AngTol )) //if (B2set(k).Vec.IsEqual( Cross, LinTol, AngTol ))
	      {
		gp_Vec Cross1, Cross2;
		Standard_Integer ind1 = B2set(k).Ind1, ind2 = B2set(k).Ind2;
		if (ind1 == ii || ind2 == ii)
		  {
		    isNew = Standard_False;
		    break;
		  }
		Cross1 = Normals( ind1 ) ^ NewNormals(i);
		Cross2 = Normals( ind2 ) ^ NewNormals(i);
		if (Cross1.SquareMagnitude() <= SquareTol || Cross2.SquareMagnitude() <= SquareTol)
		  {
		    Normals = SaveNormals;
		    Bset = SaveBset;
		    return Standard_False;
		  }
		if (Cross1.IsOpposite( Cross2, AngTol ))
		  {
		    Cross2 = Normals( ind1 ) ^ Normals( ind2 );
		    if (Cross1.IsOpposite( Cross2, AngTol ))
		      {
			Normals = SaveNormals;
			Bset = SaveBset;
			return Standard_False;
		      }
		  }
		else
		  {
		    if (NewNormals(i).Angle( Normals( ind1 ) ) > NewNormals(i).Angle( Normals( ind2 ) ))
		      {
			B2set(k).Ind2 = ind1;
			B2set(k).Ind1 = ii;
		      }
		    else
		      B2set(k).Ind1 = ii;
		  }
		isNew = Standard_False;
		break;
	      }
	  if (isNew)
	    B1set.Append( GeomPlate_Aij( ii, j, Cross ) );
	}
      
      // 5
      for (j = 1; j <= B1set.Length(); j++)
	{
	  Standard_Boolean isGEnull = Standard_True;
	  for (k = 1; k <= Normals.Length(); k++)
	    {
	      if (Normals(k).SquareMagnitude() == 0.)
		continue;
	      if (B1set(j).Vec * Normals(k) < -LinTol)
		{
		  isGEnull = Standard_False;
		  break;
		}
	    }
	  if (isGEnull)
	    B2set.Append( B1set(j) );
	}

      // 6
      if (B2set.IsEmpty())
	{
	  Normals = SaveNormals;
	  Bset = SaveBset;
	  return Standard_False;
	}

      // 7
      Bset = B2set;
      B2set.Clear();
      B1set.Clear();
      Normals.Append( NewNormals(i) );

      // 8
      for (j = 1; j <= Normals.Length(); j++)
	{
	  if (Normals(j).SquareMagnitude() == 0.)
	    continue;
	  Standard_Boolean isFound = Standard_False;
	  for (k = 1; k <= Bset.Length(); k++)
	    if (j == Bset(k).Ind1 || j == Bset(k).Ind2)
	      {
		isFound = Standard_True;
		break;
	      }
	  if (! isFound)
	    Normals(j) = NullVec;
	}
    }

  return Standard_True;
}
