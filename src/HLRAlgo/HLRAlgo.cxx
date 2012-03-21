// Created on: 1995-09-12
// Created by: Christophe MARION
// Copyright (c) 1995-1999 Matra Datavision
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


#include <HLRAlgo.ixx>

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
                            const Standard_Address Min,
                            const Standard_Address Max)
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

  Standard_Integer i = 0;
  while (i < 16)
  {
    if (((Standard_Real*)Min)[i] > d[i]) ((Standard_Real*)Min)[i] = d[i];
    if (((Standard_Real*)Max)[i] < d[i]) ((Standard_Real*)Max)[i] = d[i];
    i++;
  }
}

//=======================================================================
//function : EnlargeMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::EnlargeMinMax (const Standard_Real tol,
                             const Standard_Address Min,
                             const Standard_Address Max)
{
  Standard_Integer i = 0;
  while (i < 16)
  {
    ((Standard_Real*)Min)[i] -= tol;
    ((Standard_Real*)Max)[i] += tol;
    i++;
  }
}

//=======================================================================
//function :InitMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::InitMinMax (const Standard_Real Big,
                          const Standard_Address Min,
                          const Standard_Address Max)
{
  Standard_Integer i = 0;
  while (i < 16)
  {
    ((Standard_Real*)Min)[i] =  Big;
    ((Standard_Real*)Max)[i] = -Big;
    i++;
  }
}

//=======================================================================
//function : EncodeMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::EncodeMinMax (const Standard_Address Min,
                            const Standard_Address Max,
                            const Standard_Address MM)
{
  ((Standard_Integer*)MM)[ 0] = ((Standard_Integer*)Min)[ 1]&0x00007fff;
  ((Standard_Integer*)MM)[ 8] = ((Standard_Integer*)Max)[ 1]&0x00007fff;
  ((Standard_Integer*)MM)[ 0]+=(((Standard_Integer*)Min)[ 0]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[ 8]+=(((Standard_Integer*)Max)[ 0]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[ 1] = ((Standard_Integer*)Min)[ 3]&0x00007fff;
  ((Standard_Integer*)MM)[ 9] = ((Standard_Integer*)Max)[ 3]&0x00007fff;
  ((Standard_Integer*)MM)[ 1]+=(((Standard_Integer*)Min)[ 2]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[ 9]+=(((Standard_Integer*)Max)[ 2]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[ 2] = ((Standard_Integer*)Min)[ 5]&0x00007fff;
  ((Standard_Integer*)MM)[10] = ((Standard_Integer*)Max)[ 5]&0x00007fff;
  ((Standard_Integer*)MM)[ 2]+=(((Standard_Integer*)Min)[ 4]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[10]+=(((Standard_Integer*)Max)[ 4]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[ 3] = ((Standard_Integer*)Min)[ 7]&0x00007fff;
  ((Standard_Integer*)MM)[11] = ((Standard_Integer*)Max)[ 7]&0x00007fff;
  ((Standard_Integer*)MM)[ 3]+=(((Standard_Integer*)Min)[ 6]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[11]+=(((Standard_Integer*)Max)[ 6]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[ 4] = ((Standard_Integer*)Min)[ 9]&0x00007fff;
  ((Standard_Integer*)MM)[12] = ((Standard_Integer*)Max)[ 9]&0x00007fff;
  ((Standard_Integer*)MM)[ 4]+=(((Standard_Integer*)Min)[ 8]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[12]+=(((Standard_Integer*)Max)[ 8]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[ 5] = ((Standard_Integer*)Min)[11]&0x00007fff;
  ((Standard_Integer*)MM)[13] = ((Standard_Integer*)Max)[11]&0x00007fff;
  ((Standard_Integer*)MM)[ 5]+=(((Standard_Integer*)Min)[10]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[13]+=(((Standard_Integer*)Max)[10]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[ 6] = ((Standard_Integer*)Min)[13]&0x00007fff;
  ((Standard_Integer*)MM)[14] = ((Standard_Integer*)Max)[13]&0x00007fff;
  ((Standard_Integer*)MM)[ 6]+=(((Standard_Integer*)Min)[12]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[14]+=(((Standard_Integer*)Max)[12]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[ 7] = ((Standard_Integer*)Min)[15]&0x00007fff;
  ((Standard_Integer*)MM)[15] = ((Standard_Integer*)Max)[15]&0x00007fff;
  ((Standard_Integer*)MM)[ 7]+=(((Standard_Integer*)Min)[14]&0x00007fff)<<16;
  ((Standard_Integer*)MM)[15]+=(((Standard_Integer*)Max)[14]&0x00007fff)<<16;
}

//=======================================================================
//function : SizeBox
//purpose  : 
//=======================================================================

Standard_Real HLRAlgo::SizeBox(const Standard_Address Min,
                               const Standard_Address Max)
{
  Standard_Real s = ((Standard_Integer *)Max)[0] - ((Standard_Integer *)Min)[0];
  Standard_Integer i = 1;
  while (i < 14)
  {
    s *= ((Standard_Integer *)Max)[i] - ((Standard_Integer *)Min)[i];
    i++;
  }
  return s;
}

//=======================================================================
//function : DecodeMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::DecodeMinMax (const Standard_Address MM,
                            const Standard_Address Min,
                            const Standard_Address Max)
{
  ((Standard_Integer*)Min)[ 0]=(((Standard_Integer*)MM)[ 0]&0x7fff0000)>>16;
  ((Standard_Integer*)Max)[ 0]=(((Standard_Integer*)MM)[ 8]&0x7fff0000)>>16;
  ((Standard_Integer*)Min)[ 1]= ((Standard_Integer*)MM)[ 0]&0x00007fff;
  ((Standard_Integer*)Max)[ 1]= ((Standard_Integer*)MM)[ 8]&0x00007fff;
  ((Standard_Integer*)Min)[ 2]=(((Standard_Integer*)MM)[ 1]&0x7fff0000)>>16;
  ((Standard_Integer*)Max)[ 2]=(((Standard_Integer*)MM)[ 9]&0x7fff0000)>>16;
  ((Standard_Integer*)Min)[ 3]= ((Standard_Integer*)MM)[ 1]&0x00007fff;
  ((Standard_Integer*)Max)[ 3]= ((Standard_Integer*)MM)[ 9]&0x00007fff;
  ((Standard_Integer*)Min)[ 4]=(((Standard_Integer*)MM)[ 2]&0x7fff0000)>>16;
  ((Standard_Integer*)Max)[ 4]=(((Standard_Integer*)MM)[10]&0x7fff0000)>>16;
  ((Standard_Integer*)Min)[ 5]= ((Standard_Integer*)MM)[ 2]&0x00007fff;
  ((Standard_Integer*)Max)[ 5]= ((Standard_Integer*)MM)[10]&0x00007fff;
  ((Standard_Integer*)Min)[ 6]=(((Standard_Integer*)MM)[ 3]&0x7fff0000)>>16;
  ((Standard_Integer*)Max)[ 6]=(((Standard_Integer*)MM)[11]&0x7fff0000)>>16;
  ((Standard_Integer*)Min)[ 7]= ((Standard_Integer*)MM)[ 3]&0x00007fff;
  ((Standard_Integer*)Max)[ 7]= ((Standard_Integer*)MM)[11]&0x00007fff;
  ((Standard_Integer*)Min)[ 8]=(((Standard_Integer*)MM)[ 4]&0x7fff0000)>>16;
  ((Standard_Integer*)Max)[ 8]=(((Standard_Integer*)MM)[12]&0x7fff0000)>>16;
  ((Standard_Integer*)Min)[ 9]= ((Standard_Integer*)MM)[ 4]&0x00007fff;
  ((Standard_Integer*)Max)[ 9]= ((Standard_Integer*)MM)[12]&0x00007fff;
  ((Standard_Integer*)Min)[10]=(((Standard_Integer*)MM)[ 5]&0x7fff0000)>>16;
  ((Standard_Integer*)Max)[10]=(((Standard_Integer*)MM)[13]&0x7fff0000)>>16;
  ((Standard_Integer*)Min)[11]= ((Standard_Integer*)MM)[ 5]&0x00007fff;
  ((Standard_Integer*)Max)[11]= ((Standard_Integer*)MM)[13]&0x00007fff;
  ((Standard_Integer*)Min)[12]=(((Standard_Integer*)MM)[ 6]&0x7fff0000)>>16;
  ((Standard_Integer*)Max)[12]=(((Standard_Integer*)MM)[14]&0x7fff0000)>>16;
  ((Standard_Integer*)Min)[13]= ((Standard_Integer*)MM)[ 6]&0x00007fff;
  ((Standard_Integer*)Max)[13]= ((Standard_Integer*)MM)[14]&0x00007fff;
  ((Standard_Integer*)Min)[14]=(((Standard_Integer*)MM)[ 7]&0x7fff0000)>>16;
  ((Standard_Integer*)Max)[14]=(((Standard_Integer*)MM)[15]&0x7fff0000)>>16;
  ((Standard_Integer*)Min)[15]= ((Standard_Integer*)MM)[ 7]&0x00007fff;
  ((Standard_Integer*)Max)[15]= ((Standard_Integer*)MM)[15]&0x00007fff;
}

//=======================================================================
//function :CopyMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::CopyMinMax (const Standard_Address IMin,
                          const Standard_Address IMax,
                          const Standard_Address OMin,
                          const Standard_Address OMax)
{
  Standard_Integer i = 0;
  while (i < 16)
  {
    ((Standard_Integer*)OMin)[i]=((Standard_Integer*)IMin)[i];
    ((Standard_Integer*)OMax)[i]=((Standard_Integer*)IMax)[i];
    i++;
  }
}

//=======================================================================
//function :AddMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::AddMinMax (const Standard_Address IMin,
                         const Standard_Address IMax,
                         const Standard_Address OMin,
                         const Standard_Address OMax)
{
  Standard_Integer i = 0;
  while (i < 16)
  {
    if (((Standard_Integer*)OMin)[i] > ((Standard_Integer*)IMin)[i])
      ((Standard_Integer*)OMin)[i]=((Standard_Integer*)IMin)[i];
    if (((Standard_Integer*)OMax)[i] < ((Standard_Integer*)IMax)[i])
      ((Standard_Integer*)OMax)[i]=((Standard_Integer*)IMax)[i];
    i++;
  }
}
