// Copyright: 	Matra-Datavision 1995
// File:	SelectBasics_BasicTool.cxx
// Created:	Thu Jun  8 11:08:55 1995
// Author:	Robert COUBLANC
//		<rob>


#include <SelectBasics_BasicTool.ixx>
#include <Precision.hxx>
#include <gp_Vec2d.hxx>


//==================================================
// Function: 
// Purpose :
//==================================================

Standard_Boolean SelectBasics_BasicTool::
MatchSegments(const gp_Pnt2d & A,
	      const gp_Pnt2d & B,
	      const gp_Pnt2d & C,
	      const gp_Pnt2d & D)
{

  Standard_Real d[6],det,deta,detb;

  if(Max(A.X(),B.X())<Min(C.X(),D.X())) return Standard_False;
  if(Min(A.X(),B.X())>Max(C.X(),D.X())) return Standard_False;
  if(Max(A.Y(),B.Y())<Min(C.Y(),D.Y())) return Standard_False;
  if(Min(A.Y(),B.Y())>Max(C.Y(),D.Y())) return Standard_False;
  
  d[0] = B.X()-A.X();d[1]=C.X()-D.X();d[2]=C.X()-A.X();
  d[3] = B.Y()-A.Y();d[4]=C.Y()-D.Y();d[5]=C.Y()-A.Y();
  
  det  = d[0]*d[4]-d[3]*d[1];
  deta = d[4]*d[2]-d[5]*d[1];
  detb = d[0]*d[5]-d[3]*d[2];

  if(Abs(det)<=Precision::Confusion()) return Standard_False;
  if(deta/det<Precision::Confusion()) return Standard_False;
  if(deta/det>1+Precision::Confusion()) return Standard_False;
  if(detb/det<Precision::Confusion()) return Standard_False;
  if(detb/det>1+Precision::Confusion()) return Standard_False;

  return Standard_True;
}



//==================================================
// Function: MatchSegment
// Purpose : Return True if Segment(pBegin, pEnd) is Selected
//==================================================
Standard_Boolean SelectBasics_BasicTool::MatchSegment(const gp_Pnt2d& pBegin,const gp_Pnt2d& pEnd,
						      const Standard_Real X,
						      const Standard_Real Y,
						      const Standard_Real aTol,
						      Standard_Real& DMin)
{
  Standard_Boolean Found= Standard_False;
  const Standard_Real SqTol = aTol * aTol;	
  gp_Vec2d AB, AC, BC; 
  const gp_Pnt2d apoint(X,Y);
  
  AB.SetCoord(pEnd.X()-pBegin.X(),pEnd.Y()-pBegin.Y());
  AC.SetCoord(X-pBegin.X(),Y-pBegin.Y());
  BC.SetCoord(pEnd.X()-X,pEnd.Y()-Y);
  
  //1. Check the ends, do not estimate distance to the segment itself here
  if((apoint.SquareDistance(pBegin)<SqTol) ||
     (apoint.SquareDistance(pEnd)<SqTol)){
    DMin = 0.; 
    return Standard_True;
  }

  //2. Checking if the mouse point projection onto the segment`s line
  //   falls inside the segment.
  if(AB.Dot(AC)>=0. && AB.Dot(BC)>=0.){
    //3. Estimate distance from the mouse point to the segment 
    //   if length of segment exceeds tolerance
    const Standard_Real aSegLen = AB.Magnitude();
    if (aSegLen>aTol){
      DMin=Abs(AB.Crossed(gp_Vec2d(pBegin,apoint))/aSegLen);
      if (DMin<aTol){
	  return Standard_True;
	}
    }
  }
    
  return Standard_False;
} 



//==================================================
// Function: 
// Purpose :
//==================================================

Standard_Boolean SelectBasics_BasicTool:: 
AutoInter (const TColgp_Array1OfPnt2d& points)
{
  for (Standard_Integer i=3;i<=points.Length()-1;i++)
    {     
      for (Standard_Integer j=1;j<=i-2;j++)
	{
	  if (MatchSegments (points(i),
			     points(i+1),
			     points(j),
			     points(j+1))) return Standard_True;
	}
    }
  return Standard_False;
}


//==================================================
// Function: 
// Purpose :
//==================================================

Standard_Boolean SelectBasics_BasicTool::
MatchPolyg2d (const TColgp_Array1OfPnt2d& tabpoint,
	      const Standard_Real X,
	      const Standard_Real Y,
	      const Standard_Real aTol,
	      Standard_Real& DMin,
	      Standard_Integer& Rank)
{
	Rank =0;
	Standard_Boolean Found= Standard_False;

	//In the cycle towarded enumeration of possibilities segment, which is selected from wire
	for(Standard_Integer i=tabpoint.Lower();i<=tabpoint.Upper()-1&& !Found;i++)
	{	
		if(MatchSegment(tabpoint.Value(i),tabpoint.Value(i+1),X,Y,aTol,DMin))
		{
			Rank=i;
			return Standard_True;
		}	
	}
	return Standard_False;
}
