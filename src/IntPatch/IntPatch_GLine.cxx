// Created on: 1992-04-06
// Created by: Jacques GOUSSARD
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


#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_Point.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntPatch_GLine,IntPatch_Line)

#define DEBUG 0

//  modified by Edward AGAPOV (eap) Wed Mar 6 2002 (bug occ212)
//  -- case: points with equal params == PI/2 

//-- Precision::PConfusion()*1000.0  -> 1e-6
//#define PrecisionPConfusion ( Precision::PConfusion()*1000.0 )

#include <gp_Pln.hxx>

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates a Line as intersection line
//           when the transitions are In or Out.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Lin& L,
                                const Standard_Boolean Tang,
                                const IntSurf_TypeTrans Trans1,
								const IntSurf_TypeTrans Trans2) :
  IntPatch_Line(Tang,Trans1,Trans2),
  par1(0.0), par2(0.0),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Lin;
  pos = gp_Pln(L.Location(),L.Direction()).Position().Ax2();
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates a Line as intersection line
//           when the transitions are Touch.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Lin& L,
                                const Standard_Boolean Tang,
                                const IntSurf_Situation Situ1,
                                const IntSurf_Situation Situ2) :
  IntPatch_Line(Tang,Situ1,Situ2),
  par1(0.0), par2(0.0),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Lin;
  pos = gp_Pln(L.Location(),L.Direction()).Position().Ax2();
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates a Line as intersection line
//           when the transitions are Undecided.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Lin& L,
                                const Standard_Boolean Tang) :
  IntPatch_Line(Tang),
  par1(0.0), par2(0.0),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Lin;
  pos = gp_Pln(L.Location(),L.Direction()).Position().Ax2();
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates a circle as intersection line
//           when the transitions are In or Out.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Circ& C,
                                const Standard_Boolean Tang,
                                const IntSurf_TypeTrans Trans1,
                                const IntSurf_TypeTrans Trans2) :
  IntPatch_Line(Tang,Trans1,Trans2),
  pos(C.Position()),
  par1(C.Radius()), par2(0.0),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Circle;
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates a circle as intersection line
//           when the transitions are Touch.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Circ& C,
                                const Standard_Boolean Tang,
                                const IntSurf_Situation Situ1,
                                const IntSurf_Situation Situ2) :
  IntPatch_Line(Tang,Situ1,Situ2),
  pos(C.Position()),
  par1(C.Radius()), par2(0.0),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Circle;
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates a circle as intersection line
//           when the transitions are Undecided.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Circ& C,
                                const Standard_Boolean Tang) :
  IntPatch_Line(Tang),
  pos(C.Position()),
  par1(C.Radius()), par2(0.0),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Circle;
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates an ellipse as intersection line
//           when the transitions are In or Out.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Elips& E,
                                const Standard_Boolean Tang,
                                const IntSurf_TypeTrans Trans1,
                                const IntSurf_TypeTrans Trans2) :
  IntPatch_Line(Tang,Trans1,Trans2),
  pos(E.Position()),
  par1(E.MajorRadius()),
  par2(E.MinorRadius()),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Ellipse;
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates an ellispe as intersection line
//           when the transitions are Touch.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Elips& E,
                                const Standard_Boolean Tang,
                                const IntSurf_Situation Situ1,
                                const IntSurf_Situation Situ2) :
  IntPatch_Line(Tang,Situ1,Situ2),
  pos(E.Position()),
  par1(E.MajorRadius()),
  par2(E.MinorRadius()),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Ellipse;
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates an ellipse as intersection line
//           when the transitions are Undecided.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Elips& E,
                                const Standard_Boolean Tang) :
  IntPatch_Line(Tang),
  pos(E.Position()),
  par1(E.MajorRadius()),
  par2(E.MinorRadius()),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Ellipse;
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates a parabola as intersection line
//           when the transitions are In or Out.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Parab& P,
                                const Standard_Boolean Tang,
                                const IntSurf_TypeTrans Trans1,
                                const IntSurf_TypeTrans Trans2) :
  IntPatch_Line(Tang,Trans1,Trans2),
  pos(P.Position()),
  par1(P.Focal()), par2(0.0),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Parabola;
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates a parabola as intersection line
//           when the transitions are Touch.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Parab& P,
                                const Standard_Boolean Tang,
                                const IntSurf_Situation Situ1,
                                const IntSurf_Situation Situ2) :
  IntPatch_Line(Tang,Situ1,Situ2),
  pos(P.Position()),
  par1(P.Focal()), par2(0.0),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Parabola;
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates a parabola as intersection line 
//           when the transitions are Undecided.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Parab& P,
                                const Standard_Boolean Tang) :
  IntPatch_Line(Tang),
  pos(P.Position()),
  par1(P.Focal()), par2(0.0),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Parabola;
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates an hyperbola as intersection line
//           when the transitions are In or Out.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Hypr& H,
                                const Standard_Boolean Tang,
                                const IntSurf_TypeTrans Trans1,
                                const IntSurf_TypeTrans Trans2) :
  IntPatch_Line(Tang,Trans1,Trans2),
  pos(H.Position()),
  par1(H.MajorRadius()),
  par2(H.MinorRadius()),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Hyperbola;
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates an hyperbola as intersection line
//           when the transitions are Touch.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Hypr& H,
                                const Standard_Boolean Tang,
                                const IntSurf_Situation Situ1,
                                const IntSurf_Situation Situ2) :
  IntPatch_Line(Tang,Situ1,Situ2),
  pos(H.Position()),
  par1(H.MajorRadius()),
  par2(H.MinorRadius()),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Hyperbola;
}

//=======================================================================
//function : IntPatch_GLine
//purpose  : Creates an hyperbola as  intersection line
//           when the transitions are Undecided.
//=======================================================================

IntPatch_GLine::IntPatch_GLine (const gp_Hypr& H,
                                const Standard_Boolean Tang) :
  IntPatch_Line(Tang),
  pos(H.Position()),
  par1(H.MajorRadius()),
  par2(H.MinorRadius()),
  fipt(Standard_False),lapt(Standard_False),
  indf(0), indl(0)
{
  typ = IntPatch_Hyperbola;
}

//=======================================================================
//function : Replace
//purpose  : To replace the element of range Index in the list
//           of points.
//=======================================================================

void IntPatch_GLine::Replace (const Standard_Integer /*Index*/,
                              const IntPatch_Point& Pnt)
{
  svtx.Append(Pnt);
  //--   svtx(Index) = Pnt;
}

//=======================================================================
//function : AddVertex
//purpose  : To add a vertex in the list.
//=======================================================================

void IntPatch_GLine::AddVertex (const IntPatch_Point& Pnt)
{
  //-- On detecte le cas de 2 points 3d identiques 
  //-- pour les ramener au meme parametre sur la 
  //-- GLine 
  if (NbVertex())
  {
    const Standard_Real pf = (fipt? svtx.Value(indf).ParameterOnLine() : 0.0);
    const Standard_Real pl = (lapt? svtx.Value(indl).ParameterOnLine() : 0.0);
    Standard_Real par = Pnt.ParameterOnLine();
    if(ArcType()==IntPatch_Circle || ArcType()==IntPatch_Ellipse)
    {
      if(fipt && lapt) {
        while(par<pf) par+=M_PI+M_PI;
        while(par>pl) par-=M_PI+M_PI;
        if(par<pf) { 
          const Standard_Real PrecisionPConfusion ( Precision::PConfusion()*1000.0 );
          if((pf-par)>PrecisionPConfusion) {
            return;
          }
        }
        IntPatch_Point ParModif = Pnt;
        ParModif.SetParameter(par);
        svtx.Append(ParModif);
        return; 
      }
    }
    else
    {
      if(fipt && lapt) { 
        if(pl<par || par<pf) 
          return;
      }
    }
  }
  svtx.Append(Pnt);
}

//=======================================================================
//function : ComputeVertexParameters
//purpose  : Set the parameters of all the vertex on the line.
//           if a vertex is already in the line, 
//           its parameter is modified
//           else a new point in the line is inserted.
//=======================================================================

void IntPatch_GLine::ComputeVertexParameters(const Standard_Real /*Tol*/)
{ 
  Standard_Boolean   SortIsOK,APointDeleted;
  Standard_Boolean   SortAgain = Standard_True;
  Standard_Integer   i,j;
  const Standard_Real ParamMinOnLine = (fipt? Vertex(indf).ParameterOnLine() : -100000.0);
  const Standard_Real ParamMaxOnLine = (lapt? Vertex(indl).ParameterOnLine() :  100000.0);

  //----------------------------------------------------------
  //--     F i l t r e   s u r   r e s t r i c t i o n s   --
  //----------------------------------------------------------
  //-- deux vertex sur la meme restriction et seulement 
  //-- sur celle ci ne doivent pas avoir le meme parametre
  //==========================================================
  //-- 2 vertices on the same restriction and only
  //-- on that one must not have the same parametres
  
  Standard_Integer nbvtx = NbVertex();

  const Standard_Real PrecisionPConfusion ( Precision::PConfusion()*1000.0 );

  do { 
    APointDeleted = Standard_False;
    for(i=1; (i<=nbvtx) && (!APointDeleted); i++)
    { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if(VTXi.IsOnDomS1() || VTXi.IsOnDomS2())
      {
        for(j=1; (j<=nbvtx) && (!APointDeleted); j++)
        {
          if(i!=j)
          {
            const IntPatch_Point& VTXj   = svtx.Value(j);
            if((!VTXj.IsOnDomS1()) && (!VTXj.IsOnDomS2()))
            {
              if(Abs(VTXi.ParameterOnLine()-VTXj.ParameterOnLine())<=PrecisionPConfusion)
              {
                svtx.Remove(j);
                nbvtx--;
                if(lapt) { if(indl>j) indl--; } 
                if(fipt) { if(indf>j) indf--; } 
                APointDeleted = Standard_True;
              }
            }
          }
        }
      }
    }
  }
  while(APointDeleted && nbvtx > 2);

  do { 
    APointDeleted = Standard_False;
    for(i=1; (i<=nbvtx) && (!APointDeleted); i++)
    {
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if(VTXi.IsOnDomS1() && (!VTXi.IsOnDomS2()))
      {
        for(j=1; (j<=nbvtx) && (!APointDeleted); j++)
        {
          if(i!=j)
          {
            const IntPatch_Point& VTXj   = svtx.Value(j);
            if(VTXj.IsOnDomS1() && (!VTXj.IsOnDomS2()))
            {
              if(Abs(VTXi.ParameterOnArc1()-VTXj.ParameterOnArc1())<=PrecisionPConfusion)
              {
                if(VTXi.ArcOnS1() == VTXj.ArcOnS1())
                {
                  if(VTXi.IsVertexOnS1())
                  {
                    svtx.Remove(j);
                    nbvtx--;
                    if(lapt) { if(indl>j) indl--; } 
                    if(fipt) { if(indf>j) indf--; } 
                  }
                  else
                  {
                    svtx.Remove(i);
                    nbvtx--;
                    if(lapt) { if(indl>i) indl--; } 
                    if(fipt) { if(indf>i) indf--; } 
                  }
                  APointDeleted = Standard_True;
                }
              }
            }
          }
        }
      }
    }
  }
  while(APointDeleted);

  do { 
    APointDeleted = Standard_False;
    for(i=1; (i<=nbvtx) && (!APointDeleted); i++)
    {
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if(VTXi.IsOnDomS2() && (!VTXi.IsOnDomS1()))
      {
        for(j=1; (j<=nbvtx) && (!APointDeleted); j++)
        {
          if(i!=j)
          {
            const IntPatch_Point& VTXj   = svtx.Value(j);
            if(VTXj.IsOnDomS2() && (!VTXj.IsOnDomS1()))
            {
              if(Abs(VTXi.ParameterOnArc2()-VTXj.ParameterOnArc2())<=PrecisionPConfusion)
              {
                if(VTXi.ArcOnS2() == VTXj.ArcOnS2())
                {
                  if(VTXi.IsVertexOnS1())
                  {
                    svtx.Remove(j);
                    nbvtx--;
                    if(lapt) { if(indl>j) indl--; } 
                    if(fipt) { if(indf>j) indf--; } 
                  }
                  else
                  {
                    svtx.Remove(i);
                    nbvtx--;
                    if(lapt) { if(indl>i) indl--; } 
                    if(fipt) { if(indf>i) indf--; } 		    
                  }
                  APointDeleted = Standard_True;		  
                }
              }
            }
          }
        }
      }
    }
  }
  while(APointDeleted);
	

  //----------------------------------------------------------
  //-- Tri des vertex et suppression des Vtx superflus
  //-- 
  //// modified by jgv, 2.11.01 for BUC61033 ////
  Standard_Real u1min = RealLast(), u1max = RealFirst();
  Standard_Real u2min = RealLast(), u2max = RealFirst();
  Standard_Boolean ToBreak = Standard_False;
  ///////////////////////////////////////////////
  do { 
    nbvtx     = NbVertex();
    if(SortAgain)
    { 
      do
      {
        SortIsOK = Standard_True;
        for(i=2; i<=nbvtx; i++)
        {
          if(svtx.Value(i-1).ParameterOnLine() > svtx.Value(i).ParameterOnLine())
          {
            SortIsOK = Standard_False;
            svtx.Exchange(i-1,i);
            if(fipt) { if(indf==i) indf=i-1; else if(indf==i-1) indf=i; }
            if(lapt) { if(indl==i) indl=i-1; else if(indl==i-1) indl=i; }
          }
        }
      }
      while(!SortIsOK);
    }

    //// modified by jgv, 2.11.01 for BUC61033 ////
    if (ToBreak)
      break;
    ///////////////////////////////////////////////

    SortAgain = Standard_False;
    SortIsOK = Standard_True; 
    for(i=2; i<=nbvtx && SortIsOK; i++)
    { 
      IntPatch_Point& VTX   = svtx.ChangeValue(i);      
      for(j=1; j<=nbvtx && SortIsOK; j++)
      {
        if(i!=j)
        {
          IntPatch_Point& VTXM1 = svtx.ChangeValue(j);
          Standard_Boolean kill   = Standard_False;
          Standard_Boolean killm1 = Standard_False;
          if(Abs(VTXM1.ParameterOnLine()-VTX.ParameterOnLine())<PrecisionPConfusion)
          {
            if(VTXM1.IsOnDomS1() && VTX.IsOnDomS1()) //-- OnS1    OnS1
            {
              if(VTXM1.ArcOnS1() == VTX.ArcOnS1())//-- OnS1 == OnS1
              {
                if(VTXM1.IsOnDomS2())             //-- OnS1 == OnS1  OnS2  
                {
                  if(VTX.IsOnDomS2()==Standard_False)//-- OnS1 == OnS1  OnS2 PasOnS2
                  {
                    kill=Standard_True;
                  }
                  else
                  {
                    if(VTXM1.ArcOnS2() == VTX.ArcOnS2()) //-- OnS1 == OnS1  OnS2 == OnS2
                    {
                      if(VTXM1.IsVertexOnS2())
                      {
                        kill=Standard_True;
                      }
                      else
                      {
                        killm1=Standard_True;
                      }
                    }
                  }
                }
                else                    //-- OnS1 == OnS1  PasOnS2  
                {
                  if(VTX.IsOnDomS2())   //-- OnS1 == OnS1  PasOnS2  OnS2
                  {
                    killm1=Standard_True;
                  }
                }
              }
            }
            else                        //-- Pas OnS1  et  OnS1
            {
              if(VTXM1.IsOnDomS2()==Standard_False && VTX.IsOnDomS2()==Standard_False)
              {
                if(VTXM1.IsOnDomS1() && VTX.IsOnDomS1()==Standard_False)
                {
                  kill=Standard_True;
                }
                else if(VTX.IsOnDomS1() && VTXM1.IsOnDomS1()==Standard_False)
                {
                  killm1=Standard_True;
                }
              }
            }

            if(!(kill || killm1))
            {
              if(VTXM1.IsOnDomS2() && VTX.IsOnDomS2())  //-- OnS2    OnS2
              {
                if(VTXM1.ArcOnS2() == VTX.ArcOnS2())    //-- OnS2 == OnS2
                {
                  if(VTXM1.IsOnDomS1())                 //-- OnS2 == OnS2  OnS1 
                  {
                    if(VTX.IsOnDomS1()==Standard_False) //-- OnS2 == OnS2  OnS1 PasOnS1
                    {
                      kill=Standard_True;
                    }
                    else
                    {
                      if(VTXM1.ArcOnS1() == VTX.ArcOnS1()) //-- OnS2 == OnS2  OnS1 == OnS1
                      {
                        if(VTXM1.IsVertexOnS1())
                        {
                          kill=Standard_True;              //-- OnS2 == OnS2  OnS1 == OnS1  Vtx PasVtx
                        }
                        else
                        {
                          killm1=Standard_True;            //-- OnS2 == OnS2  OnS1 == OnS1  PasVtx Vtx
                        }
                      }
                    }
                  }
                  else
                  {                           //-- OnS2 == OnS2  PasOnS1
                    if(VTX.IsOnDomS1())       //-- OnS2 == OnS2  PasOnS1  OnS1
                    {
                      killm1=Standard_True;
                    }
                  }
                }
              }
              else //-- Pas OnS2  et  OnS2
              {
                if(VTXM1.IsOnDomS1()==Standard_False && VTX.IsOnDomS1()==Standard_False)
                {
                  if(VTXM1.IsOnDomS2() && VTX.IsOnDomS2()==Standard_False)
                  {
                    kill=Standard_True;
                  }
                  else if(VTX.IsOnDomS2() && VTXM1.IsOnDomS2()==Standard_False)
                  {
                    killm1=Standard_True;
                  }
                }
              }
            }

            //-- On a j < i
            if(kill)
            {
              SortIsOK = Standard_False;
              if(lapt)
              {
                if(indl>i)
                  indl--;
                else if(indl==i)
                  indl=j;
              }

              if(fipt)
              {
                if(indf>i)
                  indf--;
                else if(indf==i)
                  indf=j;
              }

              svtx.Remove(i);
              nbvtx--;
            }
            else if(killm1)
            {
              SortIsOK = Standard_False;
              if(lapt)
              {
                if(indl>j)
                  indl--;
                else if(indl==j)
                  indl=i-1;
              } 

              if(fipt)
              {
                if(indf>j)
                  indf--;
                else if(indf==j)
                  indf=i-1;
              }

              svtx.Remove(j);
              nbvtx--;
            }//	    else
            else if(ArcType()==IntPatch_Circle || ArcType()==IntPatch_Ellipse) // eap
            {
              //-- deux points de meme parametre qui ne peuvent etre confondus
              //-- On change les parametres d un des points si les points UV sont
              //-- differents. Ceci distingue le cas des aretes de couture.
              // ==========================================================
              //-- 2 points with the same parameters
              //-- Change parametres of one point if points UV are
              //-- different. This is the case of seam edge

              Standard_Real ponline = VTX.ParameterOnLine();
              // eap, =>>
              Standard_Real newParam = ponline;
              const Standard_Real PiPi = M_PI+M_PI;
              Standard_Boolean is2PI = ( Abs(ponline-PiPi) <= PrecisionPConfusion );

              if (nbvtx > 2 && // do this check if seam edge only gives vertices 
                  !is2PI)      // but always change 2PI -> 0
                        continue;

              if (is2PI)
                newParam = 0;
              else if (Abs(ponline) <= PrecisionPConfusion)
                newParam = PiPi;
              else
                newParam -= PiPi;

              // 	      if(  (Abs(ponline)<=PrecisionPConfusion)
              // 		   ||(Abs(ponline-M_PI-M_PI) <=PrecisionPConfusion))
              // eap, <<=

              Standard_Real u1a,v1a,u2a,v2a,u1b,v1b,u2b,v2b;
              VTXM1.Parameters(u1a,v1a,u2a,v2a);
              VTX.Parameters(u1b,v1b,u2b,v2b);
              Standard_Integer flag  = 0;

              if(   (Abs(u1a-u1b)<=PrecisionPConfusion) )
                flag|=1;

              if(   (Abs(v1a-v1b)<=PrecisionPConfusion) )
                flag|=2;
              if(   (Abs(u2a-u2b)<=PrecisionPConfusion) )
                flag|=4;

              if(   (Abs(v2a-v2b)<=PrecisionPConfusion) )
                flag|=8;

              Standard_Boolean TestOn1 = Standard_False;
              Standard_Boolean TestOn2 = Standard_False;

              switch(flag)
              { 
              case 3:   //-- meme point U1 V1  
              case 7:  //-- meme point U1 V1   meme U2
              case 12:  //--                    meme U2 V2
              case 13:  //-- meme point U1      meme U2 V2
              case 10:  //-- meme point    V1   meme    V2   Test si U1a=U1b Mod 2PI et Test si U2a=U2b Mod 2PI
                break;
              case 11:   //-- meme point U1 V1   meme    V2   Test si U2a=U2b Mod 2PI
                {
                  TestOn2 = Standard_True;
                  break;
                }

              case 14:  //-- meme point    V1   meme U2 V2   Test si U1a=U1b Mod 2PI
                {
                  TestOn1 = Standard_True;
                  break;
                }
              default:
                break;
              };

              // eap
              //if(ArcType()==IntPatch_Circle || ArcType()==IntPatch_Ellipse) {}
              if(TestOn1)
              {
                //// modified by jgv, 2.11.01 for BUC61033 ////
                Standard_Real U1A = (u1a < u1b)? u1a : u1b;
                Standard_Real U1B = (u1a < u1b)? u1b : u1a;
                
                if (u1min == RealLast())
                {
                  u1min = U1A;
                  u1max = U1B;
                }
                else
                {
                  if (Abs(U1A-u1min) > PrecisionPConfusion)
                    ToBreak = Standard_True;
                  if (Abs(U1B-u1max) > PrecisionPConfusion)
                    ToBreak = Standard_True;
                }
		    ///////////////////////////////////////////////
		    // eap, =>>
// 		      if (Abs(ponline) <= PrecisionPConfusion) { 
// 		      const Standard_Real PiPi = M_PI+M_PI;
                if(newParam >= ParamMinOnLine && newParam <= ParamMaxOnLine
                  /*PiPi >= ParamMinOnLine && PiPi<=ParamMaxOnLine*/)
                {
                  SortAgain = Standard_True;
                  SortIsOK = Standard_False;
                  if (newParam > ponline)
                  {
                    if(u1a < u1b)
                    {
                      VTX.SetParameter(newParam);
                    } 
                    else
                    {
                      VTXM1.SetParameter(newParam);
                    }
                  }
                  else
                  {
                    if(u1a > u1b)
                    {
                      VTX.SetParameter(newParam);
                    }
                    else
                    {
                      VTXM1.SetParameter(newParam);
                    } 
                  }
                }
// 		    }
// 		    else { 
// 		      if(0.0 >= ParamMinOnLine && 0.0<=ParamMaxOnLine) { 
// 			SortAgain = Standard_True;
// 			SortIsOK = Standard_False;
// 			if(u1a > u1b) { VTX.SetParameter(0.0); } 
// 			else          { VTXM1.SetParameter(0.0); } 
// 		      }
// 		    }
		    // eap, <<=
              }

              if(TestOn2)
              {
                //// modified by jgv, 2.11.01 for BUC61033 ////
                Standard_Real U2A = (u2a < u2b)? u2a : u2b;
                Standard_Real U2B = (u2a < u2b)? u2b : u2a;
                if (u2min == RealLast())
                {
                  u2min = U2A;
                  u2max = U2B;
                }
                else
                {
                  if (Abs(U2A-u2min) > PrecisionPConfusion)
                    ToBreak = Standard_True;
                  
                  if (Abs(U2B-u2max) > PrecisionPConfusion)
                    ToBreak = Standard_True;

                }
		    ///////////////////////////////////////////////
		    // eap, =>>
// 		    if (Abs(ponline) <= PrecisionPConfusion) { 
// 		      const Standard_Real PiPi = M_PI+M_PI;
                if(newParam >= ParamMinOnLine && newParam <= ParamMaxOnLine
                  /*PiPi >= ParamMinOnLine && PiPi<=ParamMaxOnLine*/)
                {
                  SortAgain = Standard_True;
                  SortIsOK = Standard_False;
                  if (newParam > ponline)
                  {
                    if(u2a < u2b)
                    {
                      VTX.SetParameter(newParam);
                    }
                    else
                    {
                      VTXM1.SetParameter(newParam);
                    }
                  }
                  else
                  {
                    if(u2a > u2b)
                    {
                      VTX.SetParameter(newParam);
                    }
                    else
                    {
                      VTXM1.SetParameter(newParam);
                    }
                  }
                }
// 		    }
// 		    else { 
// 		      if(0.0 >= ParamMinOnLine && 0.0<=ParamMaxOnLine) {
// 			SortAgain = Standard_True;
// 			SortIsOK = Standard_False;
// 			if(u2a > u2b) { VTX.SetParameter(0.0); } 
// 			else          { VTXM1.SetParameter(0.0); } 
// 		      }
// 		    }
// 		  }
// 		}
	      // eap, <<=
              }
            }
          }
        }
      } //-- if(i!=j)
    }
  }
  while(!SortIsOK);

  //-- Recalcul de fipt et lapt
  //-- 
  nbvtx=NbVertex();
  if(nbvtx)
  {
    do { 
      SortIsOK = Standard_True;
      for(i=2; i<=nbvtx; i++)
      {
        if(svtx.Value(i-1).ParameterOnLine()  > svtx.Value(i).ParameterOnLine())
        {
          SortIsOK = Standard_False;
          svtx.Exchange(i-1,i);
        }
      }
    }
    while(!SortIsOK);

    indl=nbvtx;
    indf=1;
  }
}
