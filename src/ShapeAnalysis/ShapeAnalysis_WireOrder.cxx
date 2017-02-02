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

//:o4 abv 17.02.99: r0301_db.stp #53082: treatment of open wires implemented
// pdn 11.03.99 S4135 changing reordering algorithm in order to make it independent on tolerance
//szv#4 S4163
//    pdn 09.05.99: S4174: preserve order of edges for complete torus

#include <gp_Pnt.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TColgp_Array1OfXYZ.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_SequenceOfTransient.hxx>

//=======================================================================
//function : ShapeAnalysis_WireOrder
//purpose  : 
//=======================================================================
ShapeAnalysis_WireOrder::ShapeAnalysis_WireOrder()
  : myKeepLoops(Standard_False) , myGap (0.) , myStat (0)  , myMode (Standard_True) 
{
  myTol = Precision::Confusion();
  Clear();
}

//=======================================================================
//function : ShapeAnalysis_WireOrder
//purpose  : 
//=======================================================================

ShapeAnalysis_WireOrder::ShapeAnalysis_WireOrder(const Standard_Boolean mode3d,
                                                 const Standard_Real tol)
  : myKeepLoops(Standard_False), myTol (tol), myGap (0.), myStat (0), myMode (mode3d)
{
  Clear();
}

//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================

void ShapeAnalysis_WireOrder::SetMode(const Standard_Boolean mode3d,const Standard_Real tol) 
{
  if (mode3d != myMode) Clear();
  myOrd.Nullify();  myStat = 0; myGap = 0.;
  myMode = mode3d;
  myTol = (tol > 0.)? tol : 1.e-08; //szv#4:S4163:12Mar99 optimized
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real ShapeAnalysis_WireOrder::Tolerance() const
{
  return myTol;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void ShapeAnalysis_WireOrder::Clear() 
{
  myXYZ = new TColgp_HSequenceOfXYZ();
  myStat = 0;
  myGap = 0.;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void ShapeAnalysis_WireOrder::Add(const gp_XYZ& start3d,const gp_XYZ& end3d) 
{
  //szv#4:S4163:12Mar99 waste raise
  //if (!myMode)
    //throw Standard_TypeMismatch("ShapeAnalysis_WireOrder : AddXYZ");
  if (myMode) {
    myXYZ->Append (start3d);  myXYZ->Append (end3d);
  }
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void ShapeAnalysis_WireOrder::Add(const gp_XY& start2d,const gp_XY& end2d) 
{
  //szv#4:S4163:12Mar99 waste raise
  //if ( myMode)
    //throw Standard_TypeMismatch("ShapeAnalysis_WireOrder : AddXY");
  if (!myMode) {
    gp_XYZ val;
    val.SetCoord (start2d.X(),start2d.Y(),0.);
    myXYZ->Append (val);
    val.SetCoord (end2d.X(),end2d.Y(),0.);
    myXYZ->Append (val);
  }
}

//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

Standard_Integer ShapeAnalysis_WireOrder::NbEdges() const
{
  return myXYZ->Length() / 2;
}


static Standard_Real DISTABS (const gp_XYZ& v1, const gp_XYZ& v2)
{
  return Abs (v1.X() - v2.X()) + Abs (v1.Y() - v2.Y()) + Abs (v1.Z() - v2.Z());
}

//  La routine qui suit gere les boucles internes a un wire. Questce a dire ?
//  Un wire normalement chaine (meme pas dans l ordre et avec des inverses)
//   balaie toutes ses edges au moins une fois dans une seule liste
//  En 3D il peut y avoir des COUTURES ... une, mais evt plusieurs ...
//  En ce cas le critere fin-debut peut definir des sous-parties fermees du
//  wire, ce sont les boucles en question
//  Exemple (cylindre gentil) : la couture (balayee deux fois) : 1 boucle
//   chaque limite (haute et basse) definit aussi une boucle (1 edge ou +)

//  En cours de chainage, il faut donc :
//  1/ sauter la boucle, pour ne pas la rebalayer 36 fois : NextFree y pourvoit
//    en notant les tetes de boucles, on n a pas le droit de les revoir
//   NB: ca marche car en cours de constitution de liste, on s interdit de
//   repasser plus d une fois sur chaque edge (test fol-pre non nul)
//  2/ reprendre les boucles pour les fusionner : pas encore fait
//   (pour l instant, on imprime un petit message, c est tout)

//=======================================================================
//function : KeepLoopsMode
//purpose  : 
//=======================================================================

Standard_Boolean& ShapeAnalysis_WireOrder::KeepLoopsMode()
{
  return myKeepLoops;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

static Standard_Boolean IsBetter(const Standard_Integer first,
				 const Standard_Integer second)
{
  //rln 23.03.99 bm4_al_eye.stp, entity 5281
  //Order in the file is better even if another order has the same distance
  //Lexicograhical order of preference: 0 > 2 > 1 > 3
  if (first == 0 &&  second  > 0                ) return Standard_True;
  if (first == 2 && (second == 1 || second == 3)) return Standard_True;
  if (first == 1 &&  second == 3                ) return Standard_True;
  return Standard_False;
}

void ShapeAnalysis_WireOrder::Perform(const Standard_Boolean /*closed*/) 
{
  myStat = 0;
  Standard_Integer i, nb = NbEdges();
  if(nb == 0)
    return; // no edges loaded, nothing to do -- return with status OK
  myOrd = new TColStd_HArray1OfInteger(1,nb);
  myOrd->Init(0);

  Handle(TColStd_HSequenceOfInteger) seq = new TColStd_HSequenceOfInteger;
  TColStd_SequenceOfTransient loops;
  
  TColgp_Array1OfXYZ debs(0,nb);
  TColgp_Array1OfXYZ fins(0,nb);
  
  TColStd_Array1OfBoolean idone (1, nb);
  idone.Init (Standard_False);
  
//   Calcul des precedents-suivants
  for (i = 1; i <= nb; i ++) {
    debs(i) = myXYZ->Value(2*i-1);
    fins(i) = myXYZ->Value(2*i);
  }

  Standard_Real tol2 = Precision::SquareConfusion();
  idone(1) = Standard_True;
  gp_Pnt wireFirst = debs(1);
  gp_Pnt wireLast  = fins(1);
  seq->Append(1);
  Standard_Boolean done = Standard_False;
  
  //pdn 11.03.99 S4135 constructing closed loops of edges
  while(!done) {
    Standard_Integer resultType = 3;
    Standard_Real distmin = RealLast();
    Standard_Integer ledge = 0;
    Standard_Boolean found = Standard_False;
    Standard_Real closeDist = wireFirst.SquareDistance(wireLast);
    
    for(Standard_Integer iedge = 1; (iedge <= nb) && (distmin||resultType||(resultType!=2)); iedge++)
      if(!idone(iedge)) {
	Standard_Real tailhead = wireLast.SquareDistance(debs(iedge));
	Standard_Real tailtail = wireLast.SquareDistance(fins(iedge));
	Standard_Real headtail = wireFirst.SquareDistance(fins(iedge));
	Standard_Real headhead = wireFirst.SquareDistance(debs(iedge));
	Standard_Real dm1 = tailhead, dm2 = headtail;
	Standard_Integer res1 = 0, res2 = 2;

	if (tailhead > tailtail) {res1 = 1; dm1 = tailtail;}
	if (headtail > headhead) {res2 = 3; dm2 = headhead;}
	Standard_Integer result =0;
	Standard_Real myMin3d = Min (dm1, dm2);
	if(fabs(dm1 - dm2) < tol2 ) {
          Standard_Boolean isB = IsBetter(res1,res2);
	  result = (isB ? res1 : res2);
        }
	else 
	  result = ((dm1 > dm2) ? res2 : res1); // 0 > 2 > 1 > 3
	
	if (distmin > tol2 || IsBetter(result,resultType))
	  if (myMin3d < distmin || ((myMin3d == distmin || myMin3d < tol2) && IsBetter(result,resultType))) {
	    found = Standard_True;
	    distmin = myMin3d;
	    ledge = iedge;
	    resultType = result;
	  }
      }
    if(found) {
      if (distmin == 0 || distmin < closeDist) {
	switch(resultType){
	case 0: seq->Append(ledge); wireLast = fins(ledge); break;
	case 1: seq->Append(-ledge); wireLast = debs(ledge); break;
	case 2: seq->Prepend(ledge); wireFirst = debs(ledge); break;
	case 3: seq->Prepend(-ledge); wireFirst = fins(ledge); break;
	}
      } else {
	//pdn 11.03.99 S4135 closing loop and creating new one
	loops.Append(seq);
	seq = new TColStd_HSequenceOfInteger;
	wireFirst = debs(ledge);
	wireLast  = fins(ledge);
	seq->Append(ledge);
      }
      idone(ledge) = Standard_True;
    } else {
      ledge = -1;
      for (i = 1 ; i <= nb && ledge == -1; i++)
	ledge = idone(i) ? ledge : i;
      if (ledge == -1) 
	done = 1;
      else {
	wireFirst = debs(ledge);
	wireLast  = fins(ledge);
	seq->Append(ledge);
	idone(ledge) = Standard_True;
      }
    }
  }
  loops.Append(seq);

  Handle(TColStd_HSequenceOfInteger) mainSeq;
  if (myKeepLoops) {
    
    //pdn Keeping the loops, adding one after another.
    mainSeq = new TColStd_HSequenceOfInteger;
    for (Standard_Integer ii = 1; ii <= loops.Length(); ii++) {
      Handle(TColStd_HSequenceOfInteger) subLoop = 
        Handle(TColStd_HSequenceOfInteger)::DownCast(loops(ii));
      for (Standard_Integer j = 1; j<= subLoop->Length(); j++)
        mainSeq->Append(subLoop->Value(j));
    }
  }
  else {
    //pdn 11.03.99 S4135 connecting loops.
    mainSeq = Handle(TColStd_HSequenceOfInteger)::DownCast(loops.First());
    loops.Remove(1);
    while(loops.Length()) {
      Standard_Real minLoopDist = RealLast();
      Standard_Integer loopNum=0;
      Standard_Integer loopShift=0;
      Standard_Boolean loopDirect=0;
      Standard_Integer numInLoop=0;
      for(i = 1; i <= loops.Length(); i++) {
        Handle(TColStd_HSequenceOfInteger) loop = Handle(TColStd_HSequenceOfInteger)::DownCast(loops.Value(i));
        Standard_Integer num = loop->Length();
        Standard_Integer LocShift=0;
        Standard_Integer LocNumInLoop=0;
        Standard_Boolean LocDirect = Standard_False;
        Standard_Real minLocDist = RealLast();
        for(Standard_Integer ibegin = 1; ibegin <= loop->Length(); ibegin++) {
          Standard_Integer iend = (ibegin==1 ? num : ibegin -1);
          gp_Pnt loopFirst = (loop->Value(ibegin) > 0 ? debs(loop->Value(ibegin)) : fins(-loop->Value(ibegin)));
          gp_Pnt loopLast = (loop->Value(iend) > 0 ? fins(loop->Value(iend)) : debs(-loop->Value(iend)));
          Standard_Real distmin = RealLast();
          Standard_Integer lloop=0;
          Standard_Boolean direct = Standard_False;
          for(Standard_Integer j = 1; (j <= mainSeq->Length())&& distmin; j++) {
            Standard_Integer k = (j == mainSeq->Length()? 1 : j+1);
            gp_Pnt first = (mainSeq->Value(j) > 0 ? fins(mainSeq->Value(j)) : debs(-mainSeq->Value(j)));
            gp_Pnt last  = (mainSeq->Value(k) > 0 ? debs(mainSeq->Value(k)) : fins(-mainSeq->Value(k)));
            Standard_Real dirDist = loopFirst.SquareDistance(first)+loopLast.SquareDistance(last);
            Standard_Real revDist = loopFirst.SquareDistance(last)+loopLast.SquareDistance(first);
            Standard_Real minDist;
            if((dirDist<tol2)||(dirDist < 2.*revDist)) {
              minDist = dirDist;
              revDist = dirDist;
            }
            else 
              minDist = revDist;
            if(minDist < distmin && Abs(distmin - minDist) > tol2) {
              distmin = minDist;
              direct = (dirDist <= revDist);
              lloop = j;
            }
          }
          if(distmin < minLocDist && Abs(minLocDist - distmin) > tol2) {
            minLocDist = distmin;
            LocDirect = direct;
            LocNumInLoop = lloop;
            LocShift = ibegin;
          }
	  
        }
        if(minLocDist < minLoopDist && Abs(minLoopDist - minLocDist) > tol2) {
          minLoopDist = minLocDist;
          loopNum = i;
          loopDirect = LocDirect;
          numInLoop = LocNumInLoop;
          loopShift = LocShift;
        }
      }
      
      Handle(TColStd_HSequenceOfInteger) loop = Handle(TColStd_HSequenceOfInteger)::DownCast(loops.Value(loopNum));
      Standard_Integer factor = (loopDirect ? 1: -1);
      // skl : in the next block for{} I change "i" to "ii"
      for(Standard_Integer ii = 1; ii <= loop->Length(); ii++) {
        Standard_Integer num = (ii+loopShift-1>loop->Length() ? ii+loopShift-1-loop->Length() : ii+loopShift-1);
        mainSeq->InsertAfter(numInLoop+ii-1,loop->Value(num)*factor);
      }
      loops.Remove(loopNum);
    }
  }
  
  Standard_Integer stTmp=0;
  for(i = 1; i <= mainSeq->Length(); i++) {
    if(i!=mainSeq->Value(i))
      if(stTmp>=0) stTmp = (mainSeq->Value(i) > 0 ? 1 : -1);
    myOrd->SetValue(i,mainSeq->Value(i));
  }
  if (stTmp == 0) {
    myStat = stTmp;
    return;
  }
  else {//check if edges were only shifted in reverse or forward, not reordered
    Standard_Boolean isShiftReverse = Standard_True, isShiftForward = Standard_True;
    Standard_Integer tmpFirst = 0, tmpSecond = 0, length = mainSeq->Length();
    for(i = 1; i <= length - 1; i++) {
      tmpFirst = mainSeq->Value(i);
      tmpSecond = mainSeq->Value(i+1);
      if (!(tmpSecond - tmpFirst == 1 || (tmpFirst == length && tmpSecond == 1)))
        isShiftForward = Standard_False;
      if (!(tmpFirst - tmpSecond == 1 || (tmpSecond == length && tmpFirst == 1)))
        isShiftReverse = Standard_False;
    }
    tmpFirst = mainSeq->Value(length);
    tmpSecond = mainSeq->Value(1);
    if (!(tmpSecond - tmpFirst == 1 || (tmpFirst == length && tmpSecond == 1)))
      isShiftForward = Standard_False;
    if (!(tmpFirst - tmpSecond == 1 || (tmpSecond == length && tmpFirst == 1)))
      isShiftReverse = Standard_False;
    if (isShiftForward || isShiftReverse)
      stTmp = 3;
    myStat = stTmp;
    return;
  }
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeAnalysis_WireOrder::IsDone() const
{
  return  !myOrd.IsNull();
}

//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

 Standard_Integer ShapeAnalysis_WireOrder::Status() const
{
  return  myStat;
}

//=======================================================================
//function : Ordered
//purpose  : 
//=======================================================================

 Standard_Integer ShapeAnalysis_WireOrder::Ordered(const Standard_Integer n) const
{
  if (myOrd.IsNull() || myOrd->Upper() < n) return n;
  Standard_Integer ord = myOrd->Value(n);
  return (ord == 0 ? n : ord);
}

//=======================================================================
//function : XYZ
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireOrder::XYZ(const Standard_Integer num,gp_XYZ& start3d,gp_XYZ& end3d) const
{
  if (num > 0) {
    start3d = myXYZ->Value (2*num-1);
    end3d   = myXYZ->Value (2*num);
  } else {
    start3d = myXYZ->Value (-2*num);
    end3d   = myXYZ->Value (-2*num-1);
  }
}

//=======================================================================
//function : XY
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireOrder::XY(const Standard_Integer num,gp_XY& start2d,gp_XY& end2d) const
{
  const gp_XYZ& st2d = myXYZ->Value ( (num > 0 ? 2*num-1 : -2*num) );
  start2d.SetCoord (st2d.X(),st2d.Y());
  const gp_XYZ& en2d = myXYZ->Value ( (num > 0 ? 2*num   : -2*num -1) );
  end2d.SetCoord   (en2d.X(),en2d.Y());
}

//=======================================================================
//function : Gap
//purpose  : 
//=======================================================================

 Standard_Real ShapeAnalysis_WireOrder::Gap(const Standard_Integer num) const
{
  if (num == 0) return myGap;
  Standard_Integer n1 = Ordered (num);
  Standard_Integer n0 = Ordered (num == 1 ? NbEdges() : num-1);
//  Distance entre fin (n0) et debut (n1)
  return DISTABS (myXYZ->Value( (n0 > 0 ? 2*n0   : -2*n0 -1) ) ,
		  myXYZ->Value( (n1 > 0 ? 2*n1-1 : -2*n1   ) ) );
////  return (myXYZ->Value(2*n0)).Distance (myXYZ->Value(2*n1-1));
}

//=======================================================================
//function : SetChains
//purpose  : 
//=======================================================================

void ShapeAnalysis_WireOrder::SetChains(const Standard_Real gap) 
{
  Standard_Integer n0 = 0, n1, n2, nb = NbEdges(); //szv#4:S4163:12Mar99 o0,o1,o2 not needed
  if (nb == 0) return;
  TColStd_SequenceOfInteger chain;
  n0 = 0;
  chain.Append (1);    // On demarre la partie
  gp_XYZ f3d, l3d, f13d, l13d; //szv#4:S4163:12Mar99 f03d,l03d unused
  for (n1 = 1; n1 <= nb; n1 ++) {
    if (n0 == 0) {    // nouvelle boucle
      n0 = n1;
      //szv#4:S4163:12Mar99 optimized
      XYZ ( Ordered(n0), f13d, l13d );
    }
    //szv#4:S4163:12Mar99 optimized
    n2 = (n1 == nb)? n0 : (n1 + 1);
    XYZ ( Ordered(n2), f3d, l3d );
    if (!f3d.IsEqual (l13d,gap)) { chain.Append (n2);  n0 = 0; }
    f13d = f3d;  l13d = l3d;
  }
  nb = chain.Length();
  if (nb == 0) return;
  myChains = new TColStd_HArray1OfInteger (1,nb);
  for (n1 = 1; n1 <= nb; n1 ++)  myChains->SetValue (n1,chain.Value(n1));
}

//=======================================================================
//function : NbChains
//purpose  : 
//=======================================================================

 Standard_Integer ShapeAnalysis_WireOrder::NbChains() const
{
  return (myChains.IsNull() ? 0 : myChains->Length());
}

//=======================================================================
//function : Chain
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireOrder::Chain(const Standard_Integer num,Standard_Integer& n1,Standard_Integer& n2) const
{
  n1 = n2 = 0;
  if (myChains.IsNull()) return;
  Standard_Integer nb = myChains->Upper();
  if (num == 0 || num > nb) return;
  n1 = myChains->Value (num);
  if (num == nb) n2 = NbEdges();
  else n2 = myChains->Value (num+1) - 1;
}

//=======================================================================
//function : SetCouples
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireOrder::SetCouples(const Standard_Real /*gap*/) 
{
#ifdef OCCT_DEBUG
  cout<<"ShapeAnalysis_WireOrder:SetCouple not yet implemented"<<endl;
#endif
}

//=======================================================================
//function : NbCouples
//purpose  : 
//=======================================================================

 Standard_Integer ShapeAnalysis_WireOrder::NbCouples() const
{
  return (myCouples.IsNull() ? 0 : myCouples->Length());
}

//=======================================================================
//function : Couple
//purpose  : 
//=======================================================================

 void ShapeAnalysis_WireOrder::Couple(const Standard_Integer num,Standard_Integer& n1,Standard_Integer& n2) const
{
  n1 = n2 = 0;
  if (myCouples.IsNull()) return;
  Standard_Integer nb = myCouples->Upper();
  if (num == 0 || num*2 > nb) return;
  n1 = myCouples->Value (2*num-1);
  n2 = myCouples->Value (2*num);
}

