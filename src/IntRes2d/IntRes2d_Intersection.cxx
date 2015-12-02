// Created on: 1992-04-28
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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


#include <IntRes2d_Intersection.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <IntRes2d_Position.hxx>
#include <IntRes2d_SequenceOfIntersectionPoint.hxx>
#include <IntRes2d_SequenceOfIntersectionSegment.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

#define PARAMEQUAL(a,b) (Abs((a)-(b))< (1e-8))


static void InternalVerifyPosition(IntRes2d_Transition& T1,
				   IntRes2d_Transition& T2,
				   const Standard_Real PParamOnFirst,
				   const Standard_Real PParamOnSecond,
				   const Standard_Real FirstParam1,
				   const Standard_Real LastParam1,
				   const Standard_Real FirstParam2,
				   const Standard_Real LastParam2);



//----------------------------------------------------------------------
static Standard_Boolean TransitionEqual( const IntRes2d_Transition& T1
					,const IntRes2d_Transition& T2); 


Standard_Boolean TransitionEqual( const IntRes2d_Transition& T1
				 ,const IntRes2d_Transition& T2) {
  
  if(T1.PositionOnCurve() == T2.PositionOnCurve())  {
    if(T1.TransitionType() == T2.TransitionType()) {
      if(T1.TransitionType() == IntRes2d_Touch) {
	if(T1.IsTangent()==T2.IsTangent()) {
	  if(T1.Situation() == T2.Situation()) {
	    if(T1.IsOpposite() == T2.IsOpposite()) {
	      return(Standard_True);
	    }
	  }
	}
      }
      else {
	return(Standard_True);
      }
    }
  }
  return(Standard_False);
}



void IntRes2d_Intersection::Insert(const IntRes2d_IntersectionPoint& Pnt) {
  Standard_Integer n = lpnt.Length();
  if(n==0) {
    lpnt.Append(Pnt);
  }
  else {
    Standard_Real u = Pnt.ParamOnFirst();
    Standard_Integer i = 1;
    Standard_Integer b = n+1;                    
    while(i<=n) {
      const IntRes2d_IntersectionPoint& Pnti=lpnt(i);
      Standard_Real ui = Pnti.ParamOnFirst();
      if(ui >= u) { b=i; i=n; }
      if(PARAMEQUAL(ui,u)) {
	if(PARAMEQUAL((Pnt.ParamOnSecond()),(Pnti.ParamOnSecond()))) {
	  if(   (TransitionEqual(Pnt.TransitionOfFirst(),Pnti.TransitionOfFirst()))
	     && (TransitionEqual(Pnt.TransitionOfSecond(),Pnti.TransitionOfSecond()))) {
	    b=0;
	    i=n;
	  }
	}
      }
      //----------------------------------------------------------------

      i++;
    }
    if(b>n)          { lpnt.Append(Pnt); } 
    else if(b>0)     { lpnt.InsertBefore(b,Pnt); } 
  }
}

void IntRes2d_Intersection::SetValues(const IntRes2d_Intersection& Other) {
  Standard_Integer i ;
  if(Other.done) {
    lseg.Clear();
    lpnt.Clear();
    Standard_Integer N = Other.lpnt.Length();
    for( i=1; i<= N ; i++) { 
      lpnt.Append(Other.lpnt(i));
    }
    N = Other.lseg.Length();
    for(i=1; i<= N ; i++) { 
      lseg.Append(Other.lseg(i));
    } 
    //-----------------------
    //lpnt=Other.lpnt;  Pose des problemes 
    //lseg=Other.lseg;  pour des objets composites
    //-----------------------
    done=Standard_True;
  }
  else {
    done=Standard_False;
  }
}

//----------------------------------------------------------------------
//-- Function used to Merge the results of two Intersections .
//-- for Composite Curves only. FirstParam and LastParam are the
//--  parameter of the bounds of the composite Curve
//-- Merge of two Intersection Segments S1 and S2 when :
//--  
//--     S1 : U1First,PosU1Fisrt   -->  U1Last,PosU1Last
//--          V1First,PosV1Fisrt   -->  V1Last,PosV1Last
//--     S2 : U2First,PosU2Fisrt   -->  U2Last,PosU2Last
//--          V2First,PosV2Fisrt   -->  V2Last,PosV2Last
//--
//--   1       U :      X------1-------E H-----2-------X   U -->
//--           V :      X------1-------X X-----2-------X  <- V -> ?
//--
//--      PosU1Last == End    &&  PosU2First == Head
//--      && U1Last == U2First
//--      && V1Last == V2First
//--
//--   OR  
//--
//--   2       U :      X------1-------H E-----2-------X   U <--
//--           V :      X------1-------X X-----2-------X  <- V -> ?
//--
//--      PosU1Last == Head   &&  PosU2First == End
//--      && U1Last == U2First
//--      && V1Last == V2First
//--
//--  merge the two Segment in :
//--
//--           U :      X------1----------2-------X     U 
//--           V :      X------1----------2-------X  <- V -> ?
//--
//--        
//--  
//--  The Position of Intersection Point is set to Middle 
//--  when the Parameters U et V are between FirstParam1, EndParam1
//--  and FirstParam2, EndParam2
//--  
void IntRes2d_Intersection::Append( const IntRes2d_Intersection& Other
				   ,const Standard_Real FirstParam1
				   ,const Standard_Real LastParam1
				   ,const Standard_Real FirstParam2
				   ,const Standard_Real LastParam2) {
  
  
  if(Other.done) {
    //-- Verification of the Position of the IntersectionPoints
    Standard_Integer n=Other.lpnt.Length();
    Standard_Integer i ;
    for( i=1; i<=n ; i++) {
      
      const IntRes2d_IntersectionPoint& P=Other.lpnt(i);
      Standard_Real PParamOnFirst=P.ParamOnFirst();
      Standard_Real PParamOnSecond=P.ParamOnSecond();
      IntRes2d_Transition T1=P.TransitionOfFirst();
      IntRes2d_Transition T2=P.TransitionOfSecond();
      gp_Pnt2d Pt=P.Value();
      
      
      InternalVerifyPosition(T1,T2
			     ,PParamOnFirst,PParamOnSecond
			     ,FirstParam1,LastParam1
			     ,FirstParam2,LastParam2);
	  
      this->Insert(IntRes2d_IntersectionPoint(Pt,
					      PParamOnFirst,
					      PParamOnSecond,
					      T1,T2,
					      Standard_False));
    }
    
    //--------------------------------------------------
    //-- IntersectionSegment 
    //-- (we assume that a composite curve is always bounded)
    //-- (a segment has always a FirstPoint and a LastPoint)
    //--------------------------------------------------
    n=Other.lseg.Length();
    Standard_Real SegModif_P1First=0,SegModif_P1Second=0;
    Standard_Real SegModif_P2First=0,SegModif_P2Second=0;

    for(i=1; i<=n ; i++) {
      
      const IntRes2d_IntersectionPoint& P1=Other.lseg(i).FirstPoint();
      
      Standard_Real P1PParamOnFirst=P1.ParamOnFirst();
      Standard_Real P1PParamOnSecond=P1.ParamOnSecond();
      IntRes2d_Transition P1T1=P1.TransitionOfFirst();
      IntRes2d_Transition P1T2=P1.TransitionOfSecond();
      const gp_Pnt2d& P1Pt=P1.Value();
      
      InternalVerifyPosition(P1T1,P1T2
			     ,P1PParamOnFirst,P1PParamOnSecond
			     ,FirstParam1,LastParam1
			     ,FirstParam2,LastParam2);

      const IntRes2d_IntersectionPoint& P2=Other.lseg(i).LastPoint();

      Standard_Real P2PParamOnFirst=P2.ParamOnFirst();
      Standard_Real P2PParamOnSecond=P2.ParamOnSecond();
      IntRes2d_Transition P2T1=P2.TransitionOfFirst();
      IntRes2d_Transition P2T2=P2.TransitionOfSecond();
      const gp_Pnt2d& P2Pt=P2.Value();
      
      Standard_Boolean Opposite=Other.lseg(i).IsOpposite();
      
      InternalVerifyPosition(P2T1,P2T2
			     ,P2PParamOnFirst,P2PParamOnSecond
			     ,FirstParam1,LastParam1
			     ,FirstParam2,LastParam2);



      //-- Loop on the previous segments 
      //--
      Standard_Integer an=lseg.Length();
      Standard_Boolean NotYetModified=Standard_True;

      for(Standard_Integer j=1;(j<=an)&&(NotYetModified);j++) {

	const IntRes2d_IntersectionPoint& AnP1=lseg(j).FirstPoint();
	Standard_Real AnP1PParamOnFirst=AnP1.ParamOnFirst();
	Standard_Real AnP1PParamOnSecond=AnP1.ParamOnSecond();
	
	const IntRes2d_IntersectionPoint& AnP2=lseg(j).LastPoint();
	Standard_Real AnP2PParamOnFirst=AnP2.ParamOnFirst();
	Standard_Real AnP2PParamOnSecond=AnP2.ParamOnSecond();

	if(Opposite == lseg(j).IsOpposite()) {
	  //---------------------------------------------------------------
	  //--    AnP1---------AnP2
	  //--                  P1-------------P2
	  //--
	  if(  PARAMEQUAL(P1PParamOnFirst,AnP2PParamOnFirst)
	     &&PARAMEQUAL(P1PParamOnSecond,AnP2PParamOnSecond)) {
	    NotYetModified=Standard_False;
	    lseg(j)=IntRes2d_IntersectionSegment(AnP1,P2,
						 Opposite,Standard_False);
	    SegModif_P1First   = AnP1PParamOnFirst;
	    SegModif_P1Second  = AnP1PParamOnSecond;
	    SegModif_P2First   = P2PParamOnFirst;
	    SegModif_P2Second  = P2PParamOnSecond;
	  }
	  //---------------------------------------------------------------
	  //--                                AnP1---------AnP2
	  //--                  P1-------------P2
	  //--
	  else if(  PARAMEQUAL(P2PParamOnFirst,AnP1PParamOnFirst)
		  &&PARAMEQUAL(P2PParamOnSecond,AnP1PParamOnSecond)) {
	    NotYetModified=Standard_False;
	    lseg(j)=IntRes2d_IntersectionSegment(P1,AnP2,
						 Opposite,Standard_False);
	    SegModif_P1First   = P1PParamOnFirst;
	    SegModif_P1Second  = P1PParamOnSecond;
	    SegModif_P2First   = AnP2PParamOnFirst;
	    SegModif_P2Second  = AnP2PParamOnSecond;
	  }
	  //---------------------------------------------------------------
	  //--    AnP2---------AnP1
	  //--                  P1-------------P2
	  //--
	  if(  PARAMEQUAL(P1PParamOnFirst,AnP1PParamOnFirst)
	     &&PARAMEQUAL(P1PParamOnSecond,AnP1PParamOnSecond)) {
	    NotYetModified=Standard_False;
	    lseg(j)=IntRes2d_IntersectionSegment(AnP2,P2,
						 Opposite,Standard_False);
	    SegModif_P1First   = P2PParamOnFirst;
	    SegModif_P1Second  = P2PParamOnSecond;
	    SegModif_P2First   = AnP2PParamOnFirst;
	    SegModif_P2Second  = AnP2PParamOnSecond;
	  }
	  //---------------------------------------------------------------
	  //--                                AnP2---------AnP1
	  //--                  P1-------------P2
	  //--
	  else if(  PARAMEQUAL(P2PParamOnFirst,AnP2PParamOnFirst)
		  &&PARAMEQUAL(P2PParamOnSecond,AnP2PParamOnSecond)) {
	    NotYetModified=Standard_False;
	    lseg(j)=IntRes2d_IntersectionSegment(P1,AnP1,
						 Opposite,Standard_False);
	    SegModif_P1First   = P1PParamOnFirst;
	    SegModif_P1Second  = P1PParamOnSecond;
	    SegModif_P2First   = AnP1PParamOnFirst;
	    SegModif_P2Second  = AnP1PParamOnSecond;
	  }
	}
      }
      if(NotYetModified) {
	this->Append(IntRes2d_IntersectionSegment(
			    IntRes2d_IntersectionPoint(P1Pt,
						       P1PParamOnFirst,
						       P1PParamOnSecond,
						       P1T1,P1T2,
						       Standard_False),
			    IntRes2d_IntersectionPoint(P2Pt,
						       P2PParamOnFirst,
						       P2PParamOnSecond,
						       P2T1,P2T2,
						       Standard_False),
						  Opposite,Standard_False));
	
      }    //-- if(NotYetModified) 
      else {
	//--------------------------------------------------------------
	//-- Are some Existing Points in this segment ? 
	//--------------------------------------------------------------
	Standard_Integer rnbpts=lpnt.Length();
	for(Standard_Integer rp=1; (rp<=rnbpts)&&(rp>=1); rp++) {
	  Standard_Real PonFirst=lpnt(rp).ParamOnFirst();
	  Standard_Real PonSecond=lpnt(rp).ParamOnSecond();
	  
	  if(  ((PonFirst  >=   SegModif_P1First    &&    PonFirst  <=  SegModif_P2First)
	      ||(PonFirst  <=   SegModif_P1First    &&    PonFirst  >=  SegModif_P2First))
	    && ((PonSecond >=   SegModif_P1Second   &&    PonSecond <=  SegModif_P2Second)
	      ||(PonSecond<=    SegModif_P1Second   &&    PonSecond >=  SegModif_P2Second))) {
	    lpnt.Remove(rp);
	    rp--;
	    rnbpts--;
	  }
	}  //-- for(Standard_Integer rp=1; (rp<=rnbpts)&&(rp>=1); rp++) 
      }
    }
    //--------------------------------------------------
    //-- Remove some Points ? 
    //-- Example : Points which lie in a segment.
    //--------------------------------------------------
    
    done=Standard_True;
  }
  else {
    done=Standard_False;
  }
}


//----------------------------------------------------------------------
//--
#define DEBUGPOSITION 0 

#if DEBUGPOSITION
void AffPosition(IntRes2d_Transition& T,const Standard_Real u,const char *Texte);
void AffPosition(IntRes2d_Transition& T,const Standard_Real u,const char *Texte) { 
  if(T.PositionOnCurve() == IntRes2d_End) { cout <<Texte<<" Param :"<<u<<" End "<<endl; } 
  if(T.PositionOnCurve() == IntRes2d_Middle) { cout <<Texte<<" Param :"<<u<<" Middle "<<endl; } 
  if(T.PositionOnCurve() == IntRes2d_Head) { cout <<Texte<<" Param :"<<u<<" Head "<<endl; }
} 
#endif

void InternalVerifyPosition(IntRes2d_Transition& T1,
			    IntRes2d_Transition& T2,
			    const Standard_Real PParamOnFirst,
			    const Standard_Real PParamOnSecond,
			    const Standard_Real FirstParam1,
			    const Standard_Real LastParam1,
			    const Standard_Real FirstParam2,
			    const Standard_Real LastParam2) {
#if DEBUGPOSITION
AffPosition(T1,PParamOnFirst," Point 1 ");
AffPosition(T2,PParamOnSecond," Point 2 ");
#endif
  if(T1.PositionOnCurve() != IntRes2d_Middle) {
    if(!(   PARAMEQUAL(PParamOnFirst,FirstParam1)  
         || PARAMEQUAL(PParamOnFirst,LastParam1)))  {
      //-- Middle on the Curve 1

      // modified by NIZHNY-MKK  Tue Nov 26 14:23:24 2002.BEGIN
      if((PParamOnFirst > FirstParam1) && (PParamOnFirst < LastParam1))
      // modified by NIZHNY-MKK  Tue Nov 26 14:23:27 2002.END
	T1.SetPosition(IntRes2d_Middle);
    }
  }
  if(T2.PositionOnCurve() != IntRes2d_Middle) {
    if(!(   PARAMEQUAL(PParamOnSecond,FirstParam2)
	 || PARAMEQUAL(PParamOnSecond,LastParam2))) {

      // modified by NIZHNY-MKK  Tue Nov 26 14:24:15 2002.BEGIN
      if((PParamOnSecond > FirstParam2) && (PParamOnSecond < LastParam2))
      // modified by NIZHNY-MKK  Tue Nov 26 14:24:19 2002.END
	T2.SetPosition(IntRes2d_Middle);
    }
  }
}
//----------------------------------------------------------------------
     




     

#if 0 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define Debug(q) cout<<"IntRes2d_Intersectionq ="<<q<<endl;

char *DebugPos(const IntRes2d_Position P);

Debug(FirstParam1);
Debug(LastParam1);
Debug(FirstParam2);
Debug(LastParam2);
Debug(PParamOnFirst);
Debug(PParamOnSecond);
cout<<" ##### T1  <> Middle ###### "<<DebugPos(T1.PositionOnCurve())<<endl;
char *DebugPos(const IntRes2d_Position P) {
  if(P==IntRes2d_Middle) return(" Middle ");
  if(P==IntRes2d_Head) return(" Head ");
  if(P==IntRes2d_End) return(" End ");
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif

