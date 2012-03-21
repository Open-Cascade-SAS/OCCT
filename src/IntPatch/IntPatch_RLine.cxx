// Created on: 1992-04-06
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <IntPatch_RLine.ixx>

#include <IntSurf_PntOn2S.hxx>
#include <Precision.hxx>

IntPatch_RLine::IntPatch_RLine (const Standard_Boolean Tang,
				const IntSurf_TypeTrans Trans1,
				const IntSurf_TypeTrans Trans2) :
  IntPatch_Line(Tang,Trans1,Trans2),
  fipt(Standard_False),lapt(Standard_False)

{
  typ = IntPatch_Restriction;
  onS2=Standard_False;
  onS1=Standard_False;
}


IntPatch_RLine::IntPatch_RLine (const Standard_Boolean Tang,
				const IntSurf_Situation Situ1,
				const IntSurf_Situation Situ2) :
  IntPatch_Line(Tang,Situ1,Situ2),
  fipt(Standard_False),lapt(Standard_False)
{
  typ = IntPatch_Restriction;
  onS2=Standard_False;
  onS1=Standard_False;
}


IntPatch_RLine::IntPatch_RLine (const Standard_Boolean Tang) :
  IntPatch_Line(Tang),
  fipt(Standard_False),lapt(Standard_False)

{
  typ = IntPatch_Restriction;
  onS2=Standard_False;
  onS1=Standard_False;
}

void IntPatch_RLine::ParamOnS1(Standard_Real& a,Standard_Real& b) const { 
  if(onS1) { 
    a=RealLast(); b=-a;
    for(Standard_Integer i=svtx.Length();i>=1;i--) { 
      Standard_Real p=svtx(i).ParameterOnLine();
      if(p<a) a=p;
      if(p>b) b=p;
    }
  }
  else { 
    a=b=0.0;
  }
}

void IntPatch_RLine::ParamOnS2(Standard_Real& a,Standard_Real& b) const { 
  if(onS2) { 
    a=RealLast(); b=-a;
    for(Standard_Integer i=svtx.Length();i>=1;i--) { 
      Standard_Real p=svtx(i).ParameterOnLine();
      if(p<a) a=p;
      if(p>b) b=p;
    }
  }
  else { 
    a=b=0.0;
  }
}


void IntPatch_RLine::SetArcOnS1(const Handle(Adaptor2d_HCurve2d)& A) { 
  theArcOnS1 = A;
  onS1=Standard_True;
}

void IntPatch_RLine::SetArcOnS2(const Handle(Adaptor2d_HCurve2d)& A) { 
  theArcOnS2 = A;
  onS2=Standard_True;
}


void IntPatch_RLine::SetPoint(const Standard_Integer Index,
			      const IntPatch_Point& thepoint) { 
  curv->Value(Index,thepoint.PntOn2S());
}

//void IntPatch_RLine::ComputeVertexParameters(const Standard_Real Tol)
void IntPatch_RLine::ComputeVertexParameters(const Standard_Real )
{
  Standard_Integer i,j,nbvtx;//k;
  
  Standard_Real ParamMinOnLine,ParamMaxOnLine;
  if(fipt) { ParamMinOnLine = Vertex(indf).ParameterOnLine(); } else { ParamMinOnLine = -100000.0; } 
  if(lapt) { ParamMaxOnLine = Vertex(indl).ParameterOnLine(); } else { ParamMaxOnLine =  100000.0; } 
  Standard_Boolean APointDeleted = Standard_False;
  //----------------------------------------------------------
  //--     F i l t r e   s u r   r e s t r i c t i o n s   --
  //----------------------------------------------------------
  //-- deux vertex sur la meme restriction et seulement 
  //-- sur celle ci ne doivent pas avoir le meme parametre
  //--
  nbvtx = NbVertex();
  do { 
    APointDeleted = Standard_False;
    for(i=1; (i<=nbvtx) && (APointDeleted==Standard_False) ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXi.IsOnDomS1()==Standard_True) && (VTXi.IsOnDomS2()==Standard_False)) { 
	for(j=1; (j<=nbvtx) && (APointDeleted==Standard_False) ;j++) {
	  if(i!=j) { 
	    const IntPatch_Point& VTXj   = svtx.Value(j);
	    if((VTXj.IsOnDomS1()==Standard_True) && (VTXj.IsOnDomS2()==Standard_False)) {
	      if(VTXi.ParameterOnLine() == VTXj.ParameterOnLine()) { 
		if(VTXi.ArcOnS1() == VTXj.ArcOnS1()) { 
		  if(VTXi.ParameterOnArc1() == VTXj.ParameterOnArc1()) { 
		    svtx.Remove(j);
		    nbvtx--;
		    if(lapt) { if(indl>=j) indl--; } 
		    if(fipt) { if(indf>=j) indf--; } 
		    APointDeleted = Standard_True;
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  while(APointDeleted == Standard_True);

  do { 
    APointDeleted = Standard_False;
    for(i=1; (i<=nbvtx) && (APointDeleted==Standard_False) ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXi.IsOnDomS2()==Standard_True) && (VTXi.IsOnDomS1()==Standard_False)) { 
	for(j=1; (j<=nbvtx) && (APointDeleted==Standard_False) ;j++) {
	  if(i!=j) { 
	    const IntPatch_Point& VTXj   = svtx.Value(j);
	    if((VTXj.IsOnDomS2()==Standard_True) && (VTXj.IsOnDomS1()==Standard_False)) {
	      if(VTXi.ParameterOnLine() == VTXj.ParameterOnLine()) { 
		if(VTXi.ArcOnS2() == VTXj.ArcOnS2()) { 
		  if(VTXi.ParameterOnArc2() == VTXj.ParameterOnArc2()) { 
		    svtx.Remove(j);
		    nbvtx--;
		    if(lapt) { if(indl>=j) indl--; } 
		    if(fipt) { if(indf>=j) indf--; } 
		    APointDeleted = Standard_True;
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  while(APointDeleted == Standard_True);
	
  nbvtx     = NbVertex();

  //----------------------------------------------------
  //-- On trie les Vertex 
  Standard_Boolean SortIsOK;
  do { 
    SortIsOK = Standard_True;
    for(i=2; i<=nbvtx; i++) { 
      if(svtx.Value(i-1).ParameterOnLine()  > svtx.Value(i).ParameterOnLine()) { 
	SortIsOK = Standard_False;
	svtx.Exchange(i-1,i);
      }
    }
  }
  while(!SortIsOK);
  
  do { 	
    APointDeleted = Standard_False;
    Standard_Boolean restrdiff;
    for(i=1; i<=nbvtx && (APointDeleted == Standard_False); i++) { 
      const IntPatch_Point& VTX   = svtx.Value(i);      
      for(j=1; j<=nbvtx && (APointDeleted == Standard_False) ; j++) { 
	if(i!=j) { 
	  const IntPatch_Point& VTXM1 = svtx.Value(j);
	  
	  Standard_Boolean kill   = Standard_False;
	  Standard_Boolean killm1 = Standard_False;

	  if(VTXM1.ParameterOnLine() == VTX.ParameterOnLine()) { 
	    restrdiff=Standard_False;
	    if(VTXM1.IsOnDomS1() && VTX.IsOnDomS1()) {  //-- OnS1    OnS1
	      if(VTXM1.ArcOnS1() == VTX.ArcOnS1()) {    //-- OnS1 == OnS1
		if(VTX.ParameterOnArc1() == VTXM1.ParameterOnArc1()) { 
		  if(VTXM1.IsOnDomS2()) {                 //-- OnS1 == OnS1  OnS2  
		    if(VTX.IsOnDomS2()==Standard_False) {   //-- OnS1 == OnS1  OnS2 PasOnS2
		      kill=Standard_True;   
		    }
		    else {
		      if(VTXM1.ArcOnS2() == VTX.ArcOnS2()) { //-- OnS1 == OnS1  OnS2 == OnS2
			if(VTX.ParameterOnArc2() == VTXM1.ParameterOnArc2()) { 
			  kill=Standard_True;
			}
		      }
		    }
		  }
		  else {                                  //-- OnS1 == OnS1  PasOnS2  
		    if(VTX.IsOnDomS2()) {                 //-- OnS1 == OnS1  PasOnS2  OnS2
		      killm1=Standard_True;
		    }
		  }
		}
	      }
	      else { 
		restrdiff=Standard_True;
	      }
	    }
	    
	    if((restrdiff==Standard_False) && (!(kill || killm1))) {
	      if(VTXM1.IsOnDomS2() && VTX.IsOnDomS2()) {  //-- OnS2    OnS2
		if(VTXM1.ArcOnS2() == VTX.ArcOnS2()) {    //-- OnS2 == OnS2
		  if(VTX.ParameterOnArc2() == VTXM1.ParameterOnArc2()) { 
		    if(VTXM1.IsOnDomS1()) {                 //-- OnS2 == OnS2  OnS1  
		      if(VTX.IsOnDomS1()==Standard_False) {   //-- OnS2 == OnS2  OnS1 PasOnS1
			kill=Standard_True;   
		      }
		      else {
			if(VTXM1.ArcOnS1() == VTX.ArcOnS1()) { //-- OnS2 == OnS2  OnS1 == OnS1
			  if(VTX.ParameterOnArc1() == VTXM1.ParameterOnArc1()) { 
			    kill=Standard_True;
			  }
			}
		      }
		    }
		    else {                                  //-- OnS2 == OnS2  PasOnS1  
		      if(VTX.IsOnDomS1()) {                 //-- OnS2 == OnS2  PasOnS1  OnS1
			killm1=Standard_True;
		      }
		    }
		  }
		}
		else { 
		  restrdiff=Standard_True;
		}
	      }
	    }
	    if(restrdiff==Standard_False) { 
	      if(kill) { 
		APointDeleted = Standard_True;
		svtx.Remove(i);
		nbvtx--;
	      }
	      else if(killm1) { 
		APointDeleted = Standard_True;
		svtx.Remove(j);
		nbvtx--; 
	      }
	    }
	  }
	}
      }
    }
  }
  while(APointDeleted == Standard_True);
  
  do { 	
    SortIsOK = Standard_True;
    for(i=2; i<=nbvtx && SortIsOK; i++) {
      const IntPatch_Point& Pim1=svtx.Value(i-1);
      const IntPatch_Point& Pii  =svtx.Value(i);
      if(Pim1.ParameterOnLine()==Pii.ParameterOnLine()) { 
	if(   (Pii.IsOnDomS1() == Standard_False)
	   && (Pii.IsOnDomS2() == Standard_False)) { 
	  SortIsOK = Standard_False;
	  svtx.Remove(i);
	  nbvtx--;
	}
	else {
	  if(   (Pim1.IsOnDomS1() == Standard_False)
	     && (Pim1.IsOnDomS2() == Standard_False)) { 
	    SortIsOK = Standard_False;
	    svtx.Remove(i-1);
	    nbvtx--;
	  }	  
	}
      }
    }
  }
  while(!SortIsOK);
  
  //----------------------------------------------------
  //-- On trie les Vertex ( Cas Bizarre )
  nbvtx = NbVertex();
  do { 
    SortIsOK = Standard_True;
    for(i=2; i<=nbvtx; i++) { 
      if(svtx.Value(i-1).ParameterOnLine()  > svtx.Value(i).ParameterOnLine()) { 
	SortIsOK = Standard_False;
	svtx.Exchange(i-1,i);
      }
    }
  }
  while(!SortIsOK);


  SetFirstPoint(1);
  SetLastPoint(nbvtx);


#if 0 
  Standard_Boolean SortIsOK;
  Standard_Integer nbvtx = NbVertex();
  do { 
    SortIsOK = Standard_True;
    for(Standard_Integer i=2; i<=nbvtx; i++) { 
      if(svtx.Value(i-1).ParameterOnLine()  > svtx.Value(i).ParameterOnLine()) { 
	SortIsOK = Standard_False;
	svtx.Exchange(i,i-1);
	if(fipt) {
	  if(indf == i)           indf = i-1;
	  else if(indf == (i-1))  indf = i;
	}
	if(lapt) {
	  if(indl == i)           indl = i-1;
	  else if(indl == (i-1))  indl = i;
	}
      }
    }
  }
  while(!SortIsOK);
#endif
}




