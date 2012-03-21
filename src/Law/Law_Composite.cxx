// Created on: 1996-03-29
// Created by: Laurent BOURESCHE
// Copyright (c) 1996-1999 Matra Datavision
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


// pmn -> 17/01/1996 added : Continuity, (Nb)Intervals, D2, Trim

#include <Law_Composite.ixx>
#include <ElCLib.hxx>
#include <Law_ListIteratorOfLaws.hxx>
#include <Law_Laws.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <Standard_NotImplemented.hxx>

//=======================================================================
//function : Law_Composite
//purpose  : 
//=======================================================================

Law_Composite::Law_Composite() 
                :  first(-1.e100),last(1.e100), 
		   periodic(Standard_False),
		   TFirst(-1.e100), TLast(1.e100), PTol(0.)
		  
{
}
//=======================================================================
//function : Law_Composite
//purpose  : 
//=======================================================================

Law_Composite::Law_Composite(const Standard_Real First,
			     const Standard_Real Last,
			     const Standard_Real Tol) :
			     first(-1.e100),last(1.e100),
			     periodic(Standard_False),
			     TFirst(First), TLast(Last),PTol(Tol) 
			     
{
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================
GeomAbs_Shape Law_Composite::Continuity() const 
{
  Standard_NotImplemented::Raise("Law_Composite::Continuity()");
  return GeomAbs_C0;
}

//=======================================================================
//function : NbIntervals
//purpose  : On ne se casse pas la tete, on decoupe pour chaque composant
//=======================================================================
Standard_Integer Law_Composite::NbIntervals(const GeomAbs_Shape S) const 
{
 Law_ListIteratorOfLaws It(funclist);
 Handle(Law_Function) func;
 Standard_Integer nbr_interval =0;

 for(; It.More(); It.Next()){
   func = It.Value();
   nbr_interval += func->NbIntervals(S);
 }
 return nbr_interval;
}

//=======================================================================
//function : Intervals
//purpose  : Meme simplifications....
//=======================================================================
void Law_Composite::Intervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const 
{
  Law_ListIteratorOfLaws It(funclist);
  Handle(Law_Function) func;
  Handle(TColStd_HArray1OfReal) LocT;
  Standard_Integer nb_index, Iloc, IGlob=2;

  func = funclist.First();
  func->Bounds(T(1),T(2));

  for(; It.More(); It.Next()){
    func = It.Value();
    nb_index = func->NbIntervals(S)+1;
    LocT = new (TColStd_HArray1OfReal)(1, nb_index);
    func->Intervals(LocT->ChangeArray1(), S);
    for (Iloc=2; Iloc<=nb_index; Iloc++, IGlob++) {
      T(IGlob) = LocT->Value(Iloc);
    }
  }
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Law_Composite::Value(const Standard_Real X)
{
  Standard_Real W = X;
  Prepare(W);
  return curfunc->Value(W);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Law_Composite::D1(const Standard_Real X, Standard_Real& F, Standard_Real& D)
{
  Standard_Real W = X;
  Prepare(W);
  curfunc->D1(W,F,D);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Law_Composite::D2(const Standard_Real X, 
		       Standard_Real& F, 
		       Standard_Real& D, 
		       Standard_Real& D2)
{
  Standard_Real W = X;
  Prepare(W);
  curfunc->D2(W,F,D,D2);
}

//=======================================================================
//function : Trim
//purpose  : ne garde que la partie utile dans le champs.
//=======================================================================

Handle(Law_Function) Law_Composite::Trim(const Standard_Real PFirst, 
				      const Standard_Real PLast, 
				      const Standard_Real Tol) const 
{
  Handle(Law_Composite) l = new (Law_Composite)(PFirst, PLast, Tol );
  l->ChangeLaws() = funclist;
  return l;
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Law_Composite::Bounds(Standard_Real& PFirst, Standard_Real& PLast)
{
  PFirst = first;
  PLast = last;
}

//=======================================================================
//function : Prepare
//purpose  : 
// Lorsque le parametre est pres d'un "noeud" on determine la loi en
// fonction du signe de tol:
//   - negatif -> Loi precedente au noeud.
//   - positif -> Loi consecutive au noeud. 
//=======================================================================

void Law_Composite::Prepare(Standard_Real& W)
{
  Standard_Real f,l, Wtest, Eps;
  if (W-TFirst < TLast-W) { Eps = PTol; }
  else                    { Eps = -PTol;}

  if(curfunc.IsNull()){
    curfunc = funclist.Last();
    curfunc->Bounds(f,last);
    curfunc = funclist.First();
    curfunc->Bounds(first,l);
  }
  
  Wtest = W+Eps; //Decalage pour discriminer les noeuds
  if(periodic){
    Wtest = ElCLib::InPeriod(Wtest,first,last);
    W = Wtest-Eps;
  }

  curfunc->Bounds(f,l); 
  if(f <= Wtest && Wtest <= l) return; 
  if(W <= first) {
    curfunc = funclist.First();
  }
  else if(W >= last) {
    curfunc = funclist.Last();
  }
  else{
    Law_ListIteratorOfLaws It(funclist);
    for(; It.More(); It.Next()){
      curfunc = It.Value();
      curfunc->Bounds(f,l);
      if (f <= Wtest && Wtest <= l) return;
    }
  }
}


//=======================================================================
//function : ChangeElementaryLaw
//purpose  : 
//=======================================================================

Handle(Law_Function)& Law_Composite::ChangeElementaryLaw(const Standard_Real W)
{
  Standard_Real WW = W;
  Prepare(WW);
  return curfunc;
}

//=======================================================================
//function : ChangeLaws
//purpose  : 
//=======================================================================

Law_Laws& Law_Composite::ChangeLaws()
{
  return funclist;
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Law_Composite::IsPeriodic() const 
{
  return periodic;
}

//=======================================================================
//function : SetPeriodic
//purpose  : 
//=======================================================================

void Law_Composite::SetPeriodic()
{
  periodic = Standard_True;
}
