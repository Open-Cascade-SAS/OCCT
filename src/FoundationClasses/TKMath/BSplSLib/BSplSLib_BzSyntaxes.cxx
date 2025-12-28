// Created on: 1995-10-27
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

//  pmn  16-10-96 : Correction de PolesCoefficient (PRO5782)
//                  ColLength et RowLength avaient encore frappes !!

#define No_Standard_RangeError
#define No_Standard_OutOfRange

#include <BSplSLib.hxx>
#include <BSplCLib.hxx>
#include <NCollection_Array1.hxx>

//=================================================================================================

void BSplSLib::PolesCoefficients(const NCollection_Array2<gp_Pnt>& Poles,
                                 const NCollection_Array2<double>* Weights,
                                 NCollection_Array2<gp_Pnt>&       CachePoles,
                                 NCollection_Array2<double>*       CacheWeights)
{
  int                        i;
  int                        uclas = Poles.ColLength();
  int                        vclas = Poles.RowLength();
  NCollection_Array1<double> biduflatknots(1, uclas << 1);
  NCollection_Array1<double> bidvflatknots(1, vclas << 1);

  for (i = 1; i <= uclas; i++)
  {
    biduflatknots(i)         = 0.;
    biduflatknots(i + uclas) = 1.;
  }

  for (i = 1; i <= vclas; i++)
  {
    bidvflatknots(i)         = 0.;
    bidvflatknots(i + vclas) = 1.;
  }
  if (uclas > vclas)
  {
    BSplSLib::BuildCache(0.,
                         0.,
                         1.,
                         1.,
                         false,
                         false,
                         uclas - 1,
                         vclas - 1,
                         0,
                         0,
                         biduflatknots,
                         bidvflatknots,
                         Poles,
                         Weights,
                         CachePoles,
                         CacheWeights);
  }
  else
  {
    // BuilCache exige que les resultats soient formates en [MaxCoeff,MinCoeff]
    NCollection_Array2<gp_Pnt> CPoles(1, vclas, 1, uclas);
    NCollection_Array2<double> CWeights(1, vclas, 1, uclas);
    int                        ii, jj;
    BSplSLib::BuildCache(0.,
                         0.,
                         1.,
                         1.,
                         false,
                         false,
                         uclas - 1,
                         vclas - 1,
                         0,
                         0,
                         biduflatknots,
                         bidvflatknots,
                         Poles,
                         Weights,
                         CPoles,
                         &CWeights);
    if (Weights == nullptr)
    {

      for (ii = 1; ii <= uclas; ii++)
      {

        for (jj = 1; jj <= vclas; jj++)
        {
          CachePoles(ii, jj) = CPoles(jj, ii);
        }
      }
    }
    else
    {

      for (ii = 1; ii <= uclas; ii++)
      {

        for (jj = 1; jj <= vclas; jj++)
        {
          CachePoles(ii, jj)      = CPoles(jj, ii);
          (*CacheWeights)(ii, jj) = CWeights(jj, ii);
        }
      }
    }
  }
}
