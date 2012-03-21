// Created on: 1994-03-09
// Created by: Isabelle GRIGNON
// Copyright (c) 1994-1999 Matra Datavision
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



#include <ChFiDS_Stripe.ixx>

ChFiDS_Stripe::ChFiDS_Stripe ():
       begfilled(/*Standard_False*/0), // eap, Apr 29 2002, occ293
       endfilled(/*Standard_False*/0),
       orcurv1(TopAbs_FORWARD),
       orcurv2(TopAbs_FORWARD)
{}

void ChFiDS_Stripe::Reset()
{
  myHdata.Nullify();
  orcurv1 = orcurv2 = TopAbs_FORWARD;
  pcrv1.Nullify();
  pcrv1.Nullify();
  mySpine->Reset();
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

void ChFiDS_Stripe::Parameters(const Standard_Boolean First, 
				  Standard_Real& Pdeb, 
				  Standard_Real& Pfin) const 
{
  if(First) {Pdeb = pardeb1; Pfin = parfin1;}
  else {Pdeb = pardeb2; Pfin = parfin2;}
}


//=======================================================================
//function : SetParameters
//purpose  : 
//=======================================================================

void ChFiDS_Stripe::SetParameters(const Standard_Boolean First, 
				     const Standard_Real Pdeb, 
				     const Standard_Real Pfin)
{
  if(First) {pardeb1 = Pdeb; parfin1 = Pfin;}
  else {pardeb2 = Pdeb; parfin2 = Pfin;}
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_Stripe::Curve(const Standard_Boolean First) const 
{
  if(First) return indexOfcurve1;
  else return indexOfcurve2;
}


//=======================================================================
//function : SetCurve
//purpose  : 
//=======================================================================

void ChFiDS_Stripe::SetCurve(const Standard_Integer Index, 
				const Standard_Boolean First)
{
  if(First) indexOfcurve1 = Index;
  else indexOfcurve2 = Index;
}


//=======================================================================
//function : Handle_Geom2d_Curve&
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)& ChFiDS_Stripe::PCurve
(const Standard_Boolean First) const 
{
  if(First) return pcrv1;
  else return pcrv2;
}


//=======================================================================
//function : ChangePCurve
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve)& ChFiDS_Stripe::ChangePCurve
(const Standard_Boolean First)
{
  if(First) return pcrv1;
  else return pcrv2;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation ChFiDS_Stripe::Orientation
(const Standard_Integer OnS) const 
{
  if(OnS == 1) return myOr1;
  else return myOr2;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

void ChFiDS_Stripe::SetOrientation(const TopAbs_Orientation Or, 
				      const Standard_Integer OnS) 
{
  if(OnS == 1) myOr1 = Or;
  else myOr2 = Or;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation ChFiDS_Stripe::Orientation
(const Standard_Boolean First) const 
{
  if(First) return orcurv1;
  else return orcurv2;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

void ChFiDS_Stripe::SetOrientation(const TopAbs_Orientation Or, 
				      const Standard_Boolean First) 
{
  if(First) orcurv1 = Or;
  else orcurv2 = Or;
}


//=======================================================================
//function : IndexPoint
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_Stripe::IndexPoint
(const Standard_Boolean First, const Standard_Integer OnS) const 
{
  if(First){
    if (OnS == 1) return indexfirstPOnS1;
    else return indexfirstPOnS2;
  }
  else{
    if (OnS == 1) return indexlastPOnS1;
    else return indexlastPOnS2;
  }
}


//=======================================================================
//function : SetIndexPoint
//purpose  : 
//=======================================================================

void ChFiDS_Stripe::SetIndexPoint(const Standard_Integer Index, 
				     const Standard_Boolean First, 
				     const Standard_Integer OnS)
{
  if(First){
    if (OnS == 1) indexfirstPOnS1 = Index;
    else indexfirstPOnS2 = Index;
  }
  else{
    if (OnS == 1) indexlastPOnS1 = Index;
    else indexlastPOnS2 = Index;
  }
}

Standard_Integer ChFiDS_Stripe::SolidIndex()const
{
  return indexOfSolid;
}

void ChFiDS_Stripe::SetSolidIndex(const Standard_Integer Index)
{
  indexOfSolid = Index;
}


//=======================================================================
//function : InDS
//purpose  : 
//=======================================================================

void ChFiDS_Stripe::InDS(const Standard_Boolean First,
			 const Standard_Integer Nb)  // eap, Apr 29 2002, occ293
{
  if(First){
    begfilled = /*Standard_True*/ Nb;
  }
  else{
    endfilled = /*Standard_True*/ Nb;
  }
}


//=======================================================================
//function : IsInDS
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_Stripe::IsInDS(const Standard_Boolean First)const
{
  if(First) return begfilled;
  else return endfilled;
}
