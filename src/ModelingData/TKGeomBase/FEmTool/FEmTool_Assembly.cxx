// Created on: 1998-11-17
// Created by: Igor FEOKTISTOV
// Copyright (c) 1998-1999 Matra Datavision
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

#include <FEmTool_Assembly.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_List.hxx>
#include <FEmTool_ProfileMatrix.hxx>
#include <math_Matrix.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

static int MinIndex(const occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>>& Table)
{
  int dim, el, nvar, Imin;
  int diml = Table->LowerRow(), dimu = Table->UpperRow(), ell = Table->LowerCol(),
                   elu = Table->UpperCol(), nvarl, nvaru;

  occ::handle<NCollection_HArray1<int>> T = Table->Value(diml, ell);

  nvarl = T->Lower();

  Imin = T->Value(nvarl);

  for (dim = diml; dim <= dimu; dim++)
    for (el = ell; el <= elu; el++)
    {
      T     = Table->Value(dim, el);
      nvarl = T->Lower();
      nvaru = T->Upper();
      for (nvar = nvarl; nvar <= nvaru; nvar++)
      {
        Imin = std::min(Imin, T->Value(nvar));
      }
    }
  return Imin;
}

//=================================================================================================

static int MaxIndex(const occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>>& Table)
{
  int dim, el, nvar, Imax;
  int diml = Table->LowerRow(), dimu = Table->UpperRow(), ell = Table->LowerCol(),
                   elu = Table->UpperCol(), nvarl, nvaru;

  occ::handle<NCollection_HArray1<int>> T = Table->Value(diml, ell);

  nvarl = T->Lower();

  Imax = T->Value(nvarl);

  for (dim = diml; dim <= dimu; dim++)
    for (el = ell; el <= elu; el++)
    {
      T     = Table->Value(dim, el);
      nvarl = T->Lower();
      nvaru = T->Upper();
      for (nvar = nvarl; nvar <= nvaru; nvar++)
      {
        Imax = std::max(Imax, T->Value(nvar));
      }
    }
  return Imax;
}

//=================================================================================================

FEmTool_Assembly::FEmTool_Assembly(const NCollection_Array2<int>&        Dependence,
                                   const occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>>& Table)
    : myDepTable(1, Dependence.ColLength(), 1, Dependence.RowLength()),
      B(MinIndex(Table), MaxIndex(Table))

{
  IsSolved   = false;
  myDepTable = Dependence;
  myRefTable = Table;

  NCollection_Array1<int> FirstIndexes(1, B.Length());
  FirstIndexes.Init(B.Length());

  int dim, el, nvar, Imin, I0 = 1 - B.Lower(), i;

  int diml = Table->LowerRow(), dimu = Table->UpperRow(), ell = Table->LowerCol(),
                   elu = Table->UpperCol(), nvarl, nvaru;

  occ::handle<NCollection_HArray1<int>> T;
  for (dim = diml; dim <= dimu; dim++)
    for (el = ell; el <= elu; el++)
    {

      T     = Table->Value(dim, el);
      nvarl = T->Lower();
      nvaru = T->Upper();
      Imin  = T->Value(nvarl) + I0;

      for (nvar = nvarl; nvar <= nvaru; nvar++)
        Imin = std::min(Imin, T->Value(nvar) + I0);

      for (nvar = nvarl; nvar <= nvaru; nvar++)
      {
        i               = T->Value(nvar) + I0;
        FirstIndexes(i) = std::min(FirstIndexes(i), Imin);
      }
    }

  H = new FEmTool_ProfileMatrix(FirstIndexes);

  NullifyMatrix();
  NullifyVector();
}

void FEmTool_Assembly::NullifyMatrix()
{
  H->Init(0.);
  IsSolved = false;
}

//=================================================================================================

void FEmTool_Assembly::AddMatrix(const int Element,
                                 const int Dimension1,
                                 const int Dimension2,
                                 const math_Matrix&     Mat)
{

  if (myDepTable(Dimension1, Dimension2) == 0)
    throw Standard_DomainError("FEmTool_Assembly::AddMatrix");

  const NCollection_Array1<int>& T1 = myRefTable->Value(Dimension1, Element)->Array1();
  const NCollection_Array1<int>& T2 = myRefTable->Value(Dimension2, Element)->Array1();

  int nvarl = T1.Lower(), nvaru = std::min(T1.Upper(), nvarl + Mat.RowNumber() - 1);

  int I, J, I0 = 1 - B.Lower(), i, ii, j,

                         i0 = Mat.LowerRow() - nvarl, j0 = Mat.LowerCol() - nvarl;

  for (i = nvarl; i <= nvaru; i++)
  {
    I  = T1(i) + I0;
    ii = i0 + i;
    for (j = nvarl; j <= i; j++)
    {
      J = T2(j) + I0;
      H->ChangeValue(I, J) += Mat(ii, j0 + j);
    }
  }

  IsSolved = false;
}

//=================================================================================================

void FEmTool_Assembly::NullifyVector()
{
  B.Init(0.);
}

//=================================================================================================

void FEmTool_Assembly::AddVector(const int Element,
                                 const int Dimension,
                                 const math_Vector&     Vec)
{
  const NCollection_Array1<int>& T = myRefTable->Value(Dimension, Element)->Array1();
  int nvarl = T.Lower(), nvaru = std::min(T.Upper(), nvarl + Vec.Length() - 1),
                   i0 = Vec.Lower() - nvarl;

  //  int I, i;
  int i;

  for (i = nvarl; i <= nvaru; i++)
    B(T(i)) += Vec(i0 + i);
}

//=================================================================================================

bool FEmTool_Assembly::Solve()
{
  IsSolved = H->Decompose();

#ifdef OCCT_DEBUG
  if (!IsSolved)
  {
    std::cout << "Solve Echec  H = " << std::endl;
    H->OutM();
  }
#endif

  /* ????
    while(!IsSolved && count < 5) {
      int i;
      for(i = 1; i <= H->RowNumber(); i++) {
        H->ChangeValue(i,i) *= 2.;
      }
      IsSolved = H->Decompose();
      count++;
    }
  */

  // int count = 0;
  if (!G.IsEmpty() && IsSolved)
  {
    // calculating H-1 Gt
    math_Vector gi(B.Lower(), B.Upper()), qi(B.Lower(), B.Upper());

    if (GHGt.IsNull() || GHGt->RowNumber() != G.Length())
    {
      NCollection_Array1<int> FirstIndexes(1, G.Length());
      //-----------------------------------------------------------------
      NCollection_Array2<int> H1(1, NbGlobVar(), 1, NbGlobVar());
      H1.Init(1);
      int i, j, k, l, BlockBeg = 1, BlockEnd;
      bool Block, Zero;
      for (i = 2; i <= NbGlobVar(); i++)
      {
        BlockEnd = i - 1;
        if (!H->IsInProfile(i, BlockEnd))
        {
          // Maybe, begin of block
          Block = true;
          for (j = i + 1; j <= NbGlobVar(); j++)
          {
            if (H->IsInProfile(j, BlockEnd))
            {
              Block = false;
              break;
            }
          }
          if (Block)
          {
            for (j = i; j <= NbGlobVar(); j++)
            {
              for (k = BlockBeg; k <= BlockEnd; k++)
              {
                H1(j, k) = 0;
                H1(k, j) = 0;
              }
            }
            BlockBeg = BlockEnd + 1;
          }
          else
            i = j;
        }
      }

      NCollection_List<occ::handle<NCollection_HArray1<double>>>::Iterator Iter1;
      NCollection_List<occ::handle<NCollection_HArray1<double>>>::Iterator Iter2;
      for (i = 1; i <= G.Length(); i++)
      {
        const NCollection_List<occ::handle<NCollection_HArray1<double>>>& Gi = G.Value(i);
        for (j = 1; j <= i; j++)
        {
          const NCollection_List<occ::handle<NCollection_HArray1<double>>>& Gj = G.Value(j);
          Zero                            = true;
          for (Iter1.Initialize(Gi); Iter1.More(); Iter1.Next())
          {
            const occ::handle<NCollection_HArray1<double>>& a = Iter1.Value();
            for (k = a->Lower(); k <= a->Upper(); k++)
            {
              for (Iter2.Initialize(Gj); Iter2.More(); Iter2.Next())
              {
                const occ::handle<NCollection_HArray1<double>>& b = Iter2.Value();
                for (l = b->Lower(); l <= b->Upper(); l++)
                {
                  if (H1(k, l) != 0)
                  {
                    Zero = false;
                    break;
                  }
                }
                if (!Zero)
                  break;
              }
              if (!Zero)
                break;
            }
            if (!Zero)
              break;
          }
          if (!Zero)
          {
            FirstIndexes(i) = j;
            break;
          }
        }
      }
      //-----------------------------------------------------------------------
      //      for(i = FirstIndexes.Lower(); i <= FirstIndexes.Upper(); i++)
      //	std::cout << "FirstIndexes(" << i << ") = " << FirstIndexes(i) << std::endl;
      //      FirstIndexes.Init(1); // temporary GHGt is full matrix
      GHGt = new FEmTool_ProfileMatrix(FirstIndexes);
    }

    GHGt->Init(0.);
    int                    i, j, k;
    NCollection_List<occ::handle<NCollection_HArray1<double>>>::Iterator Iter;

    for (i = 1; i <= G.Length(); i++)
    {

      const NCollection_List<occ::handle<NCollection_HArray1<double>>>& L = G.Value(i);
      gi.Init(0.);
      // preparing i-th line of G (or column of Gt)
      for (Iter.Initialize(L); Iter.More(); Iter.Next())
      {

        const occ::handle<NCollection_HArray1<double>>& a = Iter.Value();

        for (j = a->Lower(); j <= a->Upper(); j++)
          gi(j) = a->Value(j); // gi - full line of G
      }
      //                                     -1 t
      H->Solve(gi, qi); // solving H*qi = gi, qi is column of H  G

      //                               -1 t
      // Calculation of product M = G H G
      // for each i all elements of i-th column of M are calculated for k >= i
      for (k = i; k <= G.Length(); k++)
      {

        if (GHGt->IsInProfile(k, i))
        {
          double m = 0.; // m = M(k,i)

          const NCollection_List<occ::handle<NCollection_HArray1<double>>>& aL = G.Value(k);

          for (Iter.Initialize(aL); Iter.More(); Iter.Next())
          {

            const occ::handle<NCollection_HArray1<double>>& a = Iter.Value();
            for (j = a->Lower(); j <= a->Upper(); j++)
              m += qi(j) * a->Value(j); // scalar product of
                                        // k-th line of G and i-th column of H-1 Gt
          }

          GHGt->ChangeValue(k, i) = m;
        }
      }
    }

    IsSolved = GHGt->Decompose();
    /*    count = 0;
        while(!IsSolved && count < 5) {
          for(i = 1; i <= GHGt->RowNumber(); i++) {
        GHGt->ChangeValue(i,i) *= 2.;
          }
          IsSolved = GHGt->Decompose();
          count++;
        }*/
  }

  return IsSolved;
}

//=================================================================================================

void FEmTool_Assembly::Solution(math_Vector& Solution) const
{
  if (!IsSolved)
    throw StdFail_NotDone("FEmTool_Assembly::Solution");

  if (G.IsEmpty())
    H->Solve(B, Solution);
  else
  {

    math_Vector v1(B.Lower(), B.Upper());
    H->Solve(B, v1);

    math_Vector                         l(1, G.Length()), v2(1, G.Length());
    int                    i, j;
    NCollection_List<occ::handle<NCollection_HArray1<double>>>::Iterator Iter;

    for (i = 1; i <= G.Length(); i++)
    {

      const NCollection_List<occ::handle<NCollection_HArray1<double>>>& L = G.Value(i);
      double                m = 0.;

      for (Iter.Initialize(L); Iter.More(); Iter.Next())
      {

        const occ::handle<NCollection_HArray1<double>>& a = Iter.Value();
        for (j = a->Lower(); j <= a->Upper(); j++)
          m += v1(j) * a->Value(j); // scalar product
                                    // G v1
      }

      v2(i) = m - C.Value(i);
    }

    GHGt->Solve(v2, l); // Solving M*l = v2

    v1 = B;
    // Calculation v1 = B-Gt*l
    // v1(j) = B(j) - Gt(j,i)*l(i) = B(j) - G(i,j)*l(i)
    //
    for (i = 1; i <= G.Length(); i++)
    {

      const NCollection_List<occ::handle<NCollection_HArray1<double>>>& L = G.Value(i);

      for (Iter.Initialize(L); Iter.More(); Iter.Next())
      {

        const occ::handle<NCollection_HArray1<double>>& a = Iter.Value();
        for (j = a->Lower(); j <= a->Upper(); j++)
          v1(j) -= l(i) * a->Value(j);
      }
    }

    H->Solve(v1, Solution);
  }
}

int FEmTool_Assembly::NbGlobVar() const
{

  return B.Length();
}

void FEmTool_Assembly::GetAssemblyTable(occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>>& AssTable) const
{
  AssTable = myRefTable;
}

void FEmTool_Assembly::ResetConstraint()
{
  G.Clear();
  C.Clear();
}

void FEmTool_Assembly::NullifyConstraint()
{
  NCollection_List<occ::handle<NCollection_HArray1<double>>>::Iterator Iter;
  int                    i;

  for (i = 1; i <= G.Length(); i++)
  {

    C.SetValue(i, 0.);

    for (Iter.Initialize(G.Value(i)); Iter.More(); Iter.Next())
      Iter.Value()->Init(0.);
  }
}

//=================================================================================================

void FEmTool_Assembly::AddConstraint(const int IndexofConstraint,
                                     const int Element,
                                     const int Dimension,
                                     const math_Vector&     LinearForm,
                                     const double    Value)
{
  while (G.Length() < IndexofConstraint)
  {
    // Add new lines in G
    NCollection_List<occ::handle<NCollection_HArray1<double>>> L;
    G.Append(L);
    C.Append(0.);
  }

  NCollection_List<occ::handle<NCollection_HArray1<double>>>& L = G.ChangeValue(IndexofConstraint);

  occ::handle<NCollection_HArray1<int>> Indexes = myRefTable->Value(Dimension, Element);
  int                 i, Imax = 0, Imin = NbGlobVar();

  for (i = Indexes->Lower(); i <= Indexes->Upper(); i++)
  {
    Imin = std::min(Imin, Indexes->Value(i));
    Imax = std::max(Imax, Indexes->Value(i));
  }

  occ::handle<NCollection_HArray1<double>> Coeff;

  if (L.IsEmpty())
  {
    Coeff = new NCollection_HArray1<double>(Imin, Imax);
    Coeff->Init(0.);
    L.Append(Coeff);
  }
  else
  {
    NCollection_List<occ::handle<NCollection_HArray1<double>>>::Iterator Iter(L);
    double                       s1 = 0, s2 = 0;
    occ::handle<NCollection_HArray1<double>>       Aux1, Aux2;
    for (i = 1; Iter.More(); Iter.Next(), i++)
    {
      if (Imin >= Iter.Value()->Lower())
      {
        s1   = i;
        Aux1 = Iter.Value();
        if (Imax <= Iter.Value()->Upper())
        {
          s2    = s1;
          Coeff = Iter.Value();
          break;
        }
      }

      if (Imax <= Iter.Value()->Upper())
      {
        s2   = i;
        Aux2 = Iter.Value();
      }
    }

    if (s1 != s2)
    {
      if (s1 == 0)
      {
        if (Imax < Aux2->Lower())
        {
          // inserting before first segment
          Coeff = new NCollection_HArray1<double>(Imin, Imax);
          Coeff->Init(0.);
          L.Prepend(Coeff);
        }
        else
        {
          // merge new and first segment
          Coeff = new NCollection_HArray1<double>(Imin, Aux2->Upper());
          for (i = Imin; i <= Aux2->Lower() - 1; i++)
            Coeff->SetValue(i, 0.);
          for (i = Aux2->Lower(); i <= Aux2->Upper(); i++)
            Coeff->SetValue(i, Aux2->Value(i));
          L.First() = Coeff;
        }
      }
      else if (s2 == 0)
      {
        if (Imin > Aux1->Upper())
        {
          // append new
          Coeff = new NCollection_HArray1<double>(Imin, Imax);
          Coeff->Init(0.);
          L.Append(Coeff);
        }
        else
        {
          // merge new and last segment
          Coeff = new NCollection_HArray1<double>(Aux1->Lower(), Imax);
          for (i = Aux1->Lower(); i <= Aux1->Upper(); i++)
            Coeff->SetValue(i, Aux1->Value(i));
          for (i = Aux1->Upper() + 1; i <= Imax; i++)
            Coeff->SetValue(i, 0.);
          L.Last() = Coeff;
        }
      }
      else if (Imin <= Aux1->Upper() && Imax < Aux2->Lower())
      {
        // merge s1 and new
        Coeff = new NCollection_HArray1<double>(Aux1->Lower(), Imax);
        for (i = Aux1->Lower(); i <= Aux1->Upper(); i++)
          Coeff->SetValue(i, Aux1->Value(i));
        for (i = Aux1->Upper() + 1; i <= Imax; i++)
          Coeff->SetValue(i, 0.);
        Iter.Initialize(L);
        for (i = 1; i < s1; Iter.Next(), i++)
          ;
        Iter.ChangeValue() = Coeff;
      }
      else if (Imin > Aux1->Upper() && Imax >= Aux2->Lower())
      {
        // merge new and first segment
        Coeff = new NCollection_HArray1<double>(Imin, Aux2->Upper());
        for (i = Imin; i <= Aux2->Lower() - 1; i++)
          Coeff->SetValue(i, 0.);
        for (i = Aux2->Lower(); i <= Aux2->Upper(); i++)
          Coeff->SetValue(i, Aux2->Value(i));
        Iter.Initialize(L);
        for (i = 1; i < s2; Iter.Next(), i++)
          ;
        Iter.ChangeValue() = Coeff;
      }
      else if (Imin > Aux1->Upper() && Imax < Aux2->Lower())
      {
        // inserting new between s1 and s2
        Coeff = new NCollection_HArray1<double>(Imin, Imax);
        Coeff->Init(0.);
        Iter.Initialize(L);
        for (i = 1; i < s1; Iter.Next(), i++)
          ;
        L.InsertAfter(Coeff, Iter);
      }
      else
      {
        // merge s1, new, s2 and remove s2
        Coeff = new NCollection_HArray1<double>(Aux1->Lower(), Aux2->Upper());
        for (i = Aux1->Lower(); i <= Aux1->Upper(); i++)
          Coeff->SetValue(i, Aux1->Value(i));
        for (i = Aux1->Upper() + 1; i <= Aux2->Lower() - 1; i++)
          Coeff->SetValue(i, 0.);
        for (i = Aux2->Lower(); i <= Aux2->Upper(); i++)
          Coeff->SetValue(i, Aux2->Value(i));
        Iter.Initialize(L);
        for (i = 1; i < s1; Iter.Next(), i++)
          ;
        Iter.ChangeValue() = Coeff;
        Iter.Next();
        L.Remove(Iter);
      }
    }
  }

  // adding
  int j = LinearForm.Lower();
  for (i = Indexes->Lower(); i <= Indexes->Upper(); i++, j++)
  {
    Coeff->ChangeValue(Indexes->Value(i)) += LinearForm(j);
  }

  C.ChangeValue(IndexofConstraint) += Value;
}
