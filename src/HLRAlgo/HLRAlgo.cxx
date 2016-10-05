// Created on: 1995-09-12
// Created by: Christophe MARION
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


#include <HLRAlgo.hxx>

static const Standard_Real cosu0 = cos(0.*M_PI/14.);
static const Standard_Real sinu0 = sin(0.*M_PI/14.);
static const Standard_Real cosu1 = cos(1.*M_PI/14.);
static const Standard_Real sinu1 = sin(1.*M_PI/14.);
static const Standard_Real cosu2 = cos(2.*M_PI/14.);
static const Standard_Real sinu2 = sin(2.*M_PI/14.);
static const Standard_Real cosu3 = cos(3.*M_PI/14.);
static const Standard_Real sinu3 = sin(3.*M_PI/14.);
static const Standard_Real cosu4 = cos(4.*M_PI/14.);
static const Standard_Real sinu4 = sin(4.*M_PI/14.);
static const Standard_Real cosu5 = cos(5.*M_PI/14.);
static const Standard_Real sinu5 = sin(5.*M_PI/14.);
static const Standard_Real cosu6 = cos(6.*M_PI/14.);
static const Standard_Real sinu6 = sin(6.*M_PI/14.);

//=======================================================================
//function : UpdateMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::UpdateMinMax (const Standard_Real x,
                            const Standard_Real y,
                            const Standard_Real z,
                            Standard_Real Min[16],
                            Standard_Real Max[16])
{
  Standard_Real d[16];
  d[ 0] = cosu0 * x + sinu0 * y;
  d[ 1] = sinu0 * x - cosu0 * y;
  d[ 2] = cosu1 * x + sinu1 * y;
  d[ 3] = sinu1 * x - cosu1 * y;
  d[ 4] = cosu2 * x + sinu2 * y;
  d[ 5] = sinu2 * x - cosu2 * y;
  d[ 6] = cosu3 * x + sinu3 * y;
  d[ 7] = sinu3 * x - cosu3 * y;
  d[ 8] = cosu4 * x + sinu4 * y;
  d[ 9] = sinu4 * x - cosu4 * y;
  d[10] = cosu5 * x + sinu5 * y;
  d[11] = sinu5 * x - cosu5 * y;
  d[12] = cosu6 * x + sinu6 * y;
  d[13] = sinu6 * x - cosu6 * y;
  d[14] = z;
  d[15] = z;

  for (Standard_Integer i = 0; i < 16; ++i)
  {
    if (Min[i] > d[i])
    {
      Min[i] = d[i];
    }
    if (Max[i] < d[i])
    {
      Max[i] = d[i];
    }
  }
}

//=======================================================================
//function : EnlargeMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::EnlargeMinMax (const Standard_Real tol,
                             Standard_Real Min[16],
                             Standard_Real Max[16])
{
  Standard_Integer i = 0;
  while (i < 16)
  {
    Min[i] -= tol;
    Max[i] += tol;
    i++;
  }
}

//=======================================================================
//function :InitMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::InitMinMax (const Standard_Real Big,
                          Standard_Real Min[16],
                          Standard_Real Max[16])
{
  Standard_Integer i = 0;
  while (i < 16)
  {
    Min[i] =  Big;
    Max[i] = -Big;
    i++;
  }
}

//=======================================================================
//function : EncodeMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::EncodeMinMax (HLRAlgo_EdgesBlock::MinMaxIndices& Min,
                            HLRAlgo_EdgesBlock::MinMaxIndices& Max,
                            HLRAlgo_EdgesBlock::MinMaxIndices& MM)
{
  MM.Min[0] =  Min.Min[1] & 0x00007fff;
  MM.Max[0] =  Max.Min[1] & 0x00007fff;
  MM.Min[0] += (Min.Min[0] & 0x00007fff) << 16;
  MM.Max[0] += (Max.Min[0] & 0x00007fff) <<16;
  MM.Min[1] =  Min.Min[3] & 0x00007fff;
  MM.Max[1] =  Max.Min[3] & 0x00007fff;
  MM.Min[1] += (Min.Min[2] & 0x00007fff) << 16;
  MM.Max[1] += (Max.Min[2] & 0x00007fff) << 16;
  MM.Min[2] =  Min.Min[5] & 0x00007fff;
  MM.Max[2] =  Max.Min[5] & 0x00007fff;
  MM.Min[2] += (Min.Min[4] & 0x00007fff) << 16;
  MM.Max[2] += (Max.Min[4] & 0x00007fff) << 16;
  MM.Min[3] =  Min.Min[7] & 0x00007fff;
  MM.Max[3] =  Max.Min[7] & 0x00007fff;
  MM.Min[3] += (Min.Min[6] & 0x00007fff) << 16;
  MM.Max[3] += (Max.Min[6] & 0x00007fff) << 16;
  MM.Min[4] =  Min.Max[1] & 0x00007fff;
  MM.Max[4] =  Max.Max[1] & 0x00007fff;
  MM.Min[4] += (Min.Max[0] & 0x00007fff) << 16;
  MM.Max[4] += (Max.Max[0] & 0x00007fff) << 16;
  MM.Min[5] =  Min.Max[3] & 0x00007fff;
  MM.Max[5] =  Max.Max[3] & 0x00007fff;
  MM.Min[5] += (Min.Max[2] & 0x00007fff) << 16;
  MM.Max[5] += (Max.Max[2] & 0x00007fff) << 16;
  MM.Min[6] =  Min.Max[5] & 0x00007fff;
  MM.Max[6] =  Max.Max[5] & 0x00007fff;
  MM.Min[6] += (Min.Max[4] & 0x00007fff) << 16;
  MM.Max[6] += (Max.Max[4] & 0x00007fff) << 16;
  MM.Min[7] =  Min.Max[7] & 0x00007fff;
  MM.Max[7] =  Max.Max[7] & 0x00007fff;
  MM.Min[7] += (Min.Max[6] & 0x00007fff) << 16;
  MM.Max[7] += (Max.Max[6] & 0x00007fff) << 16;
}

//=======================================================================
//function : SizeBox
//purpose  : 
//=======================================================================

Standard_Real HLRAlgo::SizeBox(HLRAlgo_EdgesBlock::MinMaxIndices& Min,
                               HLRAlgo_EdgesBlock::MinMaxIndices& Max)
{
  Standard_Real s = Max.Min[0] - Min.Min[0];
  for (Standard_Integer aI = 1; aI < 8; ++aI)
  {
    s *= Max.Min[aI] - Min.Min[aI];
  }
  for (Standard_Integer aI = 0; aI < 6; ++aI)
  {
    s *= Max.Max[aI] - Min.Max[aI];
  }
  return s;
}

//=======================================================================
//function : DecodeMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::DecodeMinMax (const HLRAlgo_EdgesBlock::MinMaxIndices& MM,
                            HLRAlgo_EdgesBlock::MinMaxIndices& Min,
                            HLRAlgo_EdgesBlock::MinMaxIndices& Max)
{
  Min.Min[0] =(MM.Min[0] & 0x7fff0000)>>16;
  Max.Min[0] =(MM.Max[0] & 0x7fff0000)>>16;
  Min.Min[1] = MM.Min[0] & 0x00007fff;
  Max.Min[1] = MM.Max[0] & 0x00007fff;
  Min.Min[2] =(MM.Min[1] & 0x7fff0000)>>16;
  Max.Min[2] =(MM.Max[1] & 0x7fff0000)>>16;
  Min.Min[3] = MM.Min[1] & 0x00007fff;
  Max.Min[3] = MM.Max[1] & 0x00007fff;
  Min.Min[4] =(MM.Min[2] & 0x7fff0000)>>16;
  Max.Min[4] =(MM.Max[2] & 0x7fff0000)>>16;
  Min.Min[5] = MM.Min[2] & 0x00007fff;
  Max.Min[5] = MM.Max[2] & 0x00007fff;
  Min.Min[6] =(MM.Min[3] & 0x7fff0000)>>16;
  Max.Min[6] =(MM.Max[3] & 0x7fff0000)>>16;
  Min.Min[7] = MM.Min[3] & 0x00007fff;
  Max.Min[7] = MM.Max[3] & 0x00007fff;
  Min.Max[0] =(MM.Min[4] & 0x7fff0000)>>16;
  Max.Max[0] =(MM.Max[4] & 0x7fff0000)>>16;
  Min.Max[1] = MM.Min[4] & 0x00007fff;
  Max.Max[1] = MM.Max[4] & 0x00007fff;
  Min.Max[2] =(MM.Min[5] & 0x7fff0000)>>16;
  Max.Max[2] =(MM.Max[5] & 0x7fff0000)>>16;
  Min.Max[3] = MM.Min[5] & 0x00007fff;
  Max.Max[3] = MM.Max[5] & 0x00007fff;
  Min.Max[4] =(MM.Min[6] & 0x7fff0000)>>16;
  Max.Max[4] =(MM.Max[6] & 0x7fff0000)>>16;
  Min.Max[5] = MM.Min[6] & 0x00007fff;
  Max.Max[5] = MM.Max[6] & 0x00007fff;
  Min.Max[6] =(MM.Min[7] & 0x7fff0000)>>16;
  Max.Max[6] =(MM.Max[7] & 0x7fff0000)>>16;
  Min.Max[7] = MM.Min[7] & 0x00007fff;
  Max.Max[7] = MM.Max[7] & 0x00007fff;
}

//=======================================================================
//function :AddMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::AddMinMax (HLRAlgo_EdgesBlock::MinMaxIndices& IMin,
                         HLRAlgo_EdgesBlock::MinMaxIndices& IMax,
                         HLRAlgo_EdgesBlock::MinMaxIndices& OMin,
                         HLRAlgo_EdgesBlock::MinMaxIndices& OMax)
{
  OMin.Minimize(IMin);
  OMax.Maximize(IMax);
}
