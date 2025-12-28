// Created on: 1996-03-29
// Created by: Laurent BOURESCHE
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

// pmn -> 17/01/1996 added : Continuity, (Nb)Intervals, D2, Trim

#include <ElCLib.hxx>
#include <Law_Composite.hxx>
#include <Law_Function.hxx>
#include <Law_Function.hxx>
#include <NCollection_List.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Law_Composite, Law_Function)

//=================================================================================================

Law_Composite::Law_Composite()
    : first(-1.e100),
      last(1.e100),
      periodic(false),
      TFirst(-1.e100),
      TLast(1.e100),
      PTol(0.)

{
}

//=================================================================================================

Law_Composite::Law_Composite(const double First,
                             const double Last,
                             const double Tol)
    : first(-1.e100),
      last(1.e100),
      periodic(false),
      TFirst(First),
      TLast(Last),
      PTol(Tol)

{
}

//=================================================================================================

GeomAbs_Shape Law_Composite::Continuity() const
{
  throw Standard_NotImplemented("Law_Composite::Continuity()");
}

//=======================================================================
// function : NbIntervals
// purpose  : On ne se casse pas la tete, on decoupe pour chaque composant
//=======================================================================
int Law_Composite::NbIntervals(const GeomAbs_Shape S) const
{
  NCollection_List<occ::handle<Law_Function>>::Iterator It(funclist);
  occ::handle<Law_Function>   func;
  int       nbr_interval = 0;

  for (; It.More(); It.Next())
  {
    func = It.Value();
    nbr_interval += func->NbIntervals(S);
  }
  return nbr_interval;
}

//=======================================================================
// function : Intervals
// purpose  : Meme simplifications....
//=======================================================================
void Law_Composite::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  NCollection_List<occ::handle<Law_Function>>::Iterator        It(funclist);
  occ::handle<Law_Function>          func;
  occ::handle<NCollection_HArray1<double>> LocT;
  int              nb_index, Iloc, IGlob = 2;

  func = funclist.First();
  func->Bounds(T(1), T(2));

  for (; It.More(); It.Next())
  {
    func     = It.Value();
    nb_index = func->NbIntervals(S) + 1;
    LocT     = new (NCollection_HArray1<double>)(1, nb_index);
    func->Intervals(LocT->ChangeArray1(), S);
    for (Iloc = 2; Iloc <= nb_index; Iloc++, IGlob++)
    {
      T(IGlob) = LocT->Value(Iloc);
    }
  }
}

//=================================================================================================

double Law_Composite::Value(const double X)
{
  double W = X;
  Prepare(W);
  return curfunc->Value(W);
}

//=================================================================================================

void Law_Composite::D1(const double X, double& F, double& D)
{
  double W = X;
  Prepare(W);
  curfunc->D1(W, F, D);
}

//=================================================================================================

void Law_Composite::D2(const double X, double& F, double& D, double& D2)
{
  double W = X;
  Prepare(W);
  curfunc->D2(W, F, D, D2);
}

//=======================================================================
// function : Trim
// purpose  : ne garde que la partie utile dans le champs.
//=======================================================================

occ::handle<Law_Function> Law_Composite::Trim(const double PFirst,
                                         const double PLast,
                                         const double Tol) const
{
  occ::handle<Law_Composite> l = new (Law_Composite)(PFirst, PLast, Tol);
  l->ChangeLaws()         = funclist;
  return l;
}

//=================================================================================================

void Law_Composite::Bounds(double& PFirst, double& PLast)
{
  PFirst = first;
  PLast  = last;
}

//=======================================================================
// function : Prepare
// purpose  :
// Lorsque le parametre est pres d'un "noeud" on determine la loi en
// fonction du signe de tol:
//   - negatif -> Loi precedente au noeud.
//   - positif -> Loi consecutive au noeud.
//=======================================================================

void Law_Composite::Prepare(double& W)
{
  double f, l, Wtest, Eps;
  if (W - TFirst < TLast - W)
  {
    Eps = PTol;
  }
  else
  {
    Eps = -PTol;
  }

  if (curfunc.IsNull())
  {
    curfunc = funclist.Last();
    curfunc->Bounds(f, last);
    curfunc = funclist.First();
    curfunc->Bounds(first, l);
  }

  Wtest = W + Eps; // Decalage pour discriminer les noeuds
  if (periodic)
  {
    Wtest = ElCLib::InPeriod(Wtest, first, last);
    W     = Wtest - Eps;
  }

  curfunc->Bounds(f, l);
  if (f <= Wtest && Wtest <= l)
    return;
  if (W <= first)
  {
    curfunc = funclist.First();
  }
  else if (W >= last)
  {
    curfunc = funclist.Last();
  }
  else
  {
    NCollection_List<occ::handle<Law_Function>>::Iterator It(funclist);
    for (; It.More(); It.Next())
    {
      curfunc = It.Value();
      curfunc->Bounds(f, l);
      if (f <= Wtest && Wtest <= l)
        return;
    }
  }
}

//=================================================================================================

occ::handle<Law_Function>& Law_Composite::ChangeElementaryLaw(const double W)
{
  double WW = W;
  Prepare(WW);
  return curfunc;
}

//=================================================================================================

NCollection_List<occ::handle<Law_Function>>& Law_Composite::ChangeLaws()
{
  return funclist;
}

//=================================================================================================

bool Law_Composite::IsPeriodic() const
{
  return periodic;
}

//=================================================================================================

void Law_Composite::SetPeriodic()
{
  periodic = true;
}
