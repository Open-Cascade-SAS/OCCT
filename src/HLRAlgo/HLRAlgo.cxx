// File:	HLRAlgo.cxx
// Created:	Tue Sep 12 17:05:53 1995
// Author:	Christophe MARION
//		<cma@ecolox>

#include <HLRAlgo.ixx>

static Standard_Real cosu0 = cos(0*PI/14);
static Standard_Real sinu0 = sin(0*PI/14);
static Standard_Real cosu1 = cos(1*PI/14);
static Standard_Real sinu1 = sin(1*PI/14);
static Standard_Real cosu2 = cos(2*PI/14);
static Standard_Real sinu2 = sin(2*PI/14);
static Standard_Real cosu3 = cos(3*PI/14);
static Standard_Real sinu3 = sin(3*PI/14);
static Standard_Real cosu4 = cos(4*PI/14);
static Standard_Real sinu4 = sin(4*PI/14);
static Standard_Real cosu5 = cos(5*PI/14);
static Standard_Real sinu5 = sin(5*PI/14);
static Standard_Real cosu6 = cos(6*PI/14);
static Standard_Real sinu6 = sin(6*PI/14);

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
  Standard_Real d00,d01,d02,d03,d04,d05,d06,d07;
  Standard_Real d08,d09,d10,d11,d12,d13,d14,d15;
  d00 = cosu0 * x + sinu0 * y;
  d01 = sinu0 * x - cosu0 * y;
  d02 = cosu1 * x + sinu1 * y;
  d03 = sinu1 * x - cosu1 * y;
  d04 = cosu2 * x + sinu2 * y;
  d05 = sinu2 * x - cosu2 * y;
  d06 = cosu3 * x + sinu3 * y;
  d07 = sinu3 * x - cosu3 * y;
  d08 = cosu4 * x + sinu4 * y;
  d09 = sinu4 * x - cosu4 * y;
  d10 = cosu5 * x + sinu5 * y;
  d11 = sinu5 * x - cosu5 * y;
  d12 = cosu6 * x + sinu6 * y;
  d13 = sinu6 * x - cosu6 * y;
  d14 = z;
  d15 = z;
  
  if (((Standard_Real*)Min)[ 0] > d00) ((Standard_Real*)Min) [ 0] = d00;
  if (((Standard_Real*)Max)[ 0] < d00) ((Standard_Real*)Max) [ 0] = d00;
  if (((Standard_Real*)Min)[ 1] > d01) ((Standard_Real*)Min) [ 1] = d01;
  if (((Standard_Real*)Max)[ 1] < d01) ((Standard_Real*)Max) [ 1] = d01;
  if (((Standard_Real*)Min)[ 2] > d02) ((Standard_Real*)Min) [ 2] = d02;
  if (((Standard_Real*)Max)[ 2] < d02) ((Standard_Real*)Max) [ 2] = d02;
  if (((Standard_Real*)Min)[ 3] > d03) ((Standard_Real*)Min) [ 3] = d03;
  if (((Standard_Real*)Max)[ 3] < d03) ((Standard_Real*)Max) [ 3] = d03;
  if (((Standard_Real*)Min)[ 4] > d04) ((Standard_Real*)Min) [ 4] = d04;
  if (((Standard_Real*)Max)[ 4] < d04) ((Standard_Real*)Max) [ 4] = d04;
  if (((Standard_Real*)Min)[ 5] > d05) ((Standard_Real*)Min) [ 5] = d05;
  if (((Standard_Real*)Max)[ 5] < d05) ((Standard_Real*)Max) [ 5] = d05;
  if (((Standard_Real*)Min)[ 6] > d06) ((Standard_Real*)Min) [ 6] = d06;
  if (((Standard_Real*)Max)[ 6] < d06) ((Standard_Real*)Max) [ 6] = d06;
  if (((Standard_Real*)Min)[ 7] > d07) ((Standard_Real*)Min) [ 7] = d07;
  if (((Standard_Real*)Max)[ 7] < d07) ((Standard_Real*)Max) [ 7] = d07;
  if (((Standard_Real*)Min)[ 8] > d08) ((Standard_Real*)Min) [ 8] = d08;
  if (((Standard_Real*)Max)[ 8] < d08) ((Standard_Real*)Max) [ 8] = d08;
  if (((Standard_Real*)Min)[ 9] > d09) ((Standard_Real*)Min) [ 9] = d09;
  if (((Standard_Real*)Max)[ 9] < d09) ((Standard_Real*)Max) [ 9] = d09;
  if (((Standard_Real*)Min)[10] > d10) ((Standard_Real*)Min) [10] = d10;
  if (((Standard_Real*)Max)[10] < d10) ((Standard_Real*)Max) [10] = d10;
  if (((Standard_Real*)Min)[11] > d11) ((Standard_Real*)Min) [11] = d11;
  if (((Standard_Real*)Max)[11] < d11) ((Standard_Real*)Max) [11] = d11;
  if (((Standard_Real*)Min)[12] > d12) ((Standard_Real*)Min) [12] = d12;
  if (((Standard_Real*)Max)[12] < d12) ((Standard_Real*)Max) [12] = d12;
  if (((Standard_Real*)Min)[13] > d13) ((Standard_Real*)Min) [13] = d13;
  if (((Standard_Real*)Max)[13] < d13) ((Standard_Real*)Max) [13] = d13;
  if (((Standard_Real*)Min)[14] > d14) ((Standard_Real*)Min) [14] = d14;
  if (((Standard_Real*)Max)[14] < d14) ((Standard_Real*)Max) [14] = d14;
  if (((Standard_Real*)Min)[15] > d15) ((Standard_Real*)Min) [15] = d15;
  if (((Standard_Real*)Max)[15] < d15) ((Standard_Real*)Max) [15] = d15;
}

//=======================================================================
//function : EnlargeMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::EnlargeMinMax (const Standard_Real tol,
			     const Standard_Address Min,
			     const Standard_Address Max)
{
  ((Standard_Real*)Min)[ 0] -= tol;
  ((Standard_Real*)Max)[ 0] += tol;
  ((Standard_Real*)Min)[ 1] -= tol;
  ((Standard_Real*)Max)[ 1] += tol;
  ((Standard_Real*)Min)[ 2] -= tol;
  ((Standard_Real*)Max)[ 2] += tol;
  ((Standard_Real*)Min)[ 3] -= tol;
  ((Standard_Real*)Max)[ 3] += tol;
  ((Standard_Real*)Min)[ 4] -= tol;
  ((Standard_Real*)Max)[ 4] += tol;
  ((Standard_Real*)Min)[ 5] -= tol;
  ((Standard_Real*)Max)[ 5] += tol;
  ((Standard_Real*)Min)[ 6] -= tol;
  ((Standard_Real*)Max)[ 6] += tol;
  ((Standard_Real*)Min)[ 7] -= tol;
  ((Standard_Real*)Max)[ 7] += tol;
  ((Standard_Real*)Min)[ 8] -= tol;
  ((Standard_Real*)Max)[ 8] += tol;
  ((Standard_Real*)Min)[ 9] -= tol;
  ((Standard_Real*)Max)[ 9] += tol;
  ((Standard_Real*)Min)[10] -= tol;
  ((Standard_Real*)Max)[10] += tol;
  ((Standard_Real*)Min)[11] -= tol;
  ((Standard_Real*)Max)[11] += tol;
  ((Standard_Real*)Min)[12] -= tol;
  ((Standard_Real*)Max)[12] += tol;
  ((Standard_Real*)Min)[13] -= tol;
  ((Standard_Real*)Max)[13] += tol;
  ((Standard_Real*)Min)[14] -= tol;
  ((Standard_Real*)Max)[14] += tol;
  ((Standard_Real*)Min)[15] -= tol;
  ((Standard_Real*)Max)[15] += tol;
}

//=======================================================================
//function :InitMinMax
//purpose  : 
//=======================================================================

void HLRAlgo::InitMinMax (const Standard_Real Big,
			  const Standard_Address Min,
			  const Standard_Address Max)
{
  ((Standard_Real*)Min)[ 0] =  
  ((Standard_Real*)Min)[ 1] =  
  ((Standard_Real*)Min)[ 2] =  
  ((Standard_Real*)Min)[ 3] =  
  ((Standard_Real*)Min)[ 4] =  
  ((Standard_Real*)Min)[ 5] =  
  ((Standard_Real*)Min)[ 6] =  
  ((Standard_Real*)Min)[ 7] =  
  ((Standard_Real*)Min)[ 8] =  
  ((Standard_Real*)Min)[ 9] =  
  ((Standard_Real*)Min)[10] =  
  ((Standard_Real*)Min)[11] =  
  ((Standard_Real*)Min)[12] =  
  ((Standard_Real*)Min)[13] =  
  ((Standard_Real*)Min)[14] =  
  ((Standard_Real*)Min)[15] =  Big;
  ((Standard_Real*)Max)[ 0] = 
  ((Standard_Real*)Max)[ 1] = 
  ((Standard_Real*)Max)[ 2] = 
  ((Standard_Real*)Max)[ 3] = 
  ((Standard_Real*)Max)[ 4] = 
  ((Standard_Real*)Max)[ 5] = 
  ((Standard_Real*)Max)[ 6] = 
  ((Standard_Real*)Max)[ 7] = 
  ((Standard_Real*)Max)[ 8] = 
  ((Standard_Real*)Max)[ 9] = 
  ((Standard_Real*)Max)[10] = 
  ((Standard_Real*)Max)[11] = 
  ((Standard_Real*)Max)[12] = 
  ((Standard_Real*)Max)[13] = 
  ((Standard_Real*)Max)[14] = 
  ((Standard_Real*)Max)[15] = -Big;
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

Standard_Real  HLRAlgo::SizeBox(const Standard_Address Min,
				const Standard_Address Max) 
{
  Standard_Real s;
  s  = ((Standard_Integer *)Max)[ 0] - ((Standard_Integer *)Min)[ 0];
  s *= ((Standard_Integer *)Max)[ 1] - ((Standard_Integer *)Min)[ 1];
  s *= ((Standard_Integer *)Max)[ 2] - ((Standard_Integer *)Min)[ 2];
  s *= ((Standard_Integer *)Max)[ 3] - ((Standard_Integer *)Min)[ 3];
  s *= ((Standard_Integer *)Max)[ 4] - ((Standard_Integer *)Min)[ 4];
  s *= ((Standard_Integer *)Max)[ 5] - ((Standard_Integer *)Min)[ 5];
  s *= ((Standard_Integer *)Max)[ 6] - ((Standard_Integer *)Min)[ 6];
  s *= ((Standard_Integer *)Max)[ 7] - ((Standard_Integer *)Min)[ 7];
  s *= ((Standard_Integer *)Max)[ 8] - ((Standard_Integer *)Min)[ 8];
  s *= ((Standard_Integer *)Max)[ 9] - ((Standard_Integer *)Min)[ 9];
  s *= ((Standard_Integer *)Max)[10] - ((Standard_Integer *)Min)[10];
  s *= ((Standard_Integer *)Max)[11] - ((Standard_Integer *)Min)[11];
  s *= ((Standard_Integer *)Max)[12] - ((Standard_Integer *)Min)[12];
  s *= ((Standard_Integer *)Max)[13] - ((Standard_Integer *)Min)[13];
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
  ((Standard_Integer*)OMin)[ 0]=((Standard_Integer*)IMin)[ 0];
  ((Standard_Integer*)OMax)[ 0]=((Standard_Integer*)IMax)[ 0];
  ((Standard_Integer*)OMin)[ 1]=((Standard_Integer*)IMin)[ 1];
  ((Standard_Integer*)OMax)[ 1]=((Standard_Integer*)IMax)[ 1];
  ((Standard_Integer*)OMin)[ 2]=((Standard_Integer*)IMin)[ 2];
  ((Standard_Integer*)OMax)[ 2]=((Standard_Integer*)IMax)[ 2];
  ((Standard_Integer*)OMin)[ 3]=((Standard_Integer*)IMin)[ 3];
  ((Standard_Integer*)OMax)[ 3]=((Standard_Integer*)IMax)[ 3];
  ((Standard_Integer*)OMin)[ 4]=((Standard_Integer*)IMin)[ 4];
  ((Standard_Integer*)OMax)[ 4]=((Standard_Integer*)IMax)[ 4];
  ((Standard_Integer*)OMin)[ 5]=((Standard_Integer*)IMin)[ 5];
  ((Standard_Integer*)OMax)[ 5]=((Standard_Integer*)IMax)[ 5];
  ((Standard_Integer*)OMin)[ 6]=((Standard_Integer*)IMin)[ 6];
  ((Standard_Integer*)OMax)[ 6]=((Standard_Integer*)IMax)[ 6];
  ((Standard_Integer*)OMin)[ 7]=((Standard_Integer*)IMin)[ 7];
  ((Standard_Integer*)OMax)[ 7]=((Standard_Integer*)IMax)[ 7];
  ((Standard_Integer*)OMin)[ 8]=((Standard_Integer*)IMin)[ 8];
  ((Standard_Integer*)OMax)[ 8]=((Standard_Integer*)IMax)[ 8];
  ((Standard_Integer*)OMin)[ 9]=((Standard_Integer*)IMin)[ 9];
  ((Standard_Integer*)OMax)[ 9]=((Standard_Integer*)IMax)[ 9];
  ((Standard_Integer*)OMin)[10]=((Standard_Integer*)IMin)[10];
  ((Standard_Integer*)OMax)[10]=((Standard_Integer*)IMax)[10];
  ((Standard_Integer*)OMin)[11]=((Standard_Integer*)IMin)[11];
  ((Standard_Integer*)OMax)[11]=((Standard_Integer*)IMax)[11];
  ((Standard_Integer*)OMin)[12]=((Standard_Integer*)IMin)[12];
  ((Standard_Integer*)OMax)[12]=((Standard_Integer*)IMax)[12];
  ((Standard_Integer*)OMin)[13]=((Standard_Integer*)IMin)[13];
  ((Standard_Integer*)OMax)[13]=((Standard_Integer*)IMax)[13];
  ((Standard_Integer*)OMin)[14]=((Standard_Integer*)IMin)[14];
  ((Standard_Integer*)OMax)[14]=((Standard_Integer*)IMax)[14];
  ((Standard_Integer*)OMin)[15]=((Standard_Integer*)IMin)[15];
  ((Standard_Integer*)OMax)[15]=((Standard_Integer*)IMax)[15];
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
  if (((Standard_Integer*)OMin)[ 0] > ((Standard_Integer*)IMin)[ 0])
    ((Standard_Integer*)OMin)[ 0]=((Standard_Integer*)IMin)[ 0];
  if (((Standard_Integer*)OMax)[ 0] < ((Standard_Integer*)IMax)[ 0])
    ((Standard_Integer*)OMax)[ 0]=((Standard_Integer*)IMax)[ 0];
  if (((Standard_Integer*)OMin)[ 1] > ((Standard_Integer*)IMin)[ 1])
    ((Standard_Integer*)OMin)[ 1]=((Standard_Integer*)IMin)[ 1];
  if (((Standard_Integer*)OMax)[ 1] < ((Standard_Integer*)IMax)[ 1])
    ((Standard_Integer*)OMax)[ 1]=((Standard_Integer*)IMax)[ 1];
  if (((Standard_Integer*)OMin)[ 2] > ((Standard_Integer*)IMin)[ 2])
    ((Standard_Integer*)OMin)[ 2]=((Standard_Integer*)IMin)[ 2];
  if (((Standard_Integer*)OMax)[ 2] < ((Standard_Integer*)IMax)[ 2])
    ((Standard_Integer*)OMax)[ 2]=((Standard_Integer*)IMax)[ 2];
  if (((Standard_Integer*)OMin)[ 3] > ((Standard_Integer*)IMin)[ 3])
    ((Standard_Integer*)OMin)[ 3]=((Standard_Integer*)IMin)[ 3];
  if (((Standard_Integer*)OMax)[ 3] < ((Standard_Integer*)IMax)[ 3])
    ((Standard_Integer*)OMax)[ 3]=((Standard_Integer*)IMax)[ 3];
  if (((Standard_Integer*)OMin)[ 4] > ((Standard_Integer*)IMin)[ 4])
    ((Standard_Integer*)OMin)[ 4]=((Standard_Integer*)IMin)[ 4];
  if (((Standard_Integer*)OMax)[ 4] < ((Standard_Integer*)IMax)[ 4])
    ((Standard_Integer*)OMax)[ 4]=((Standard_Integer*)IMax)[ 4];
  if (((Standard_Integer*)OMin)[ 5] > ((Standard_Integer*)IMin)[ 5])
    ((Standard_Integer*)OMin)[ 5]=((Standard_Integer*)IMin)[ 5];
  if (((Standard_Integer*)OMax)[ 5] < ((Standard_Integer*)IMax)[ 5])
    ((Standard_Integer*)OMax)[ 5]=((Standard_Integer*)IMax)[ 5];
  if (((Standard_Integer*)OMin)[ 6] > ((Standard_Integer*)IMin)[ 6])
    ((Standard_Integer*)OMin)[ 6]=((Standard_Integer*)IMin)[ 6];
  if (((Standard_Integer*)OMax)[ 6] < ((Standard_Integer*)IMax)[ 6])
    ((Standard_Integer*)OMax)[ 6]=((Standard_Integer*)IMax)[ 6];
  if (((Standard_Integer*)OMin)[ 7] > ((Standard_Integer*)IMin)[ 7])
    ((Standard_Integer*)OMin)[ 7]=((Standard_Integer*)IMin)[ 7];
  if (((Standard_Integer*)OMax)[ 7] < ((Standard_Integer*)IMax)[ 7])
    ((Standard_Integer*)OMax)[ 7]=((Standard_Integer*)IMax)[ 7];
  if (((Standard_Integer*)OMin)[ 8] > ((Standard_Integer*)IMin)[ 8])
    ((Standard_Integer*)OMin)[ 8]=((Standard_Integer*)IMin)[ 8];
  if (((Standard_Integer*)OMax)[ 8] < ((Standard_Integer*)IMax)[ 8])
    ((Standard_Integer*)OMax)[ 8]=((Standard_Integer*)IMax)[ 8];
  if (((Standard_Integer*)OMin)[ 9] > ((Standard_Integer*)IMin)[ 9])
    ((Standard_Integer*)OMin)[ 9]=((Standard_Integer*)IMin)[ 9];
  if (((Standard_Integer*)OMax)[ 9] < ((Standard_Integer*)IMax)[ 9])
    ((Standard_Integer*)OMax)[ 9]=((Standard_Integer*)IMax)[ 9];
  if (((Standard_Integer*)OMin)[10] > ((Standard_Integer*)IMin)[10])
    ((Standard_Integer*)OMin)[10]=((Standard_Integer*)IMin)[10];
  if (((Standard_Integer*)OMax)[10] < ((Standard_Integer*)IMax)[10])
    ((Standard_Integer*)OMax)[10]=((Standard_Integer*)IMax)[10];
  if (((Standard_Integer*)OMin)[11] > ((Standard_Integer*)IMin)[11])
    ((Standard_Integer*)OMin)[11]=((Standard_Integer*)IMin)[11];
  if (((Standard_Integer*)OMax)[11] < ((Standard_Integer*)IMax)[11])
    ((Standard_Integer*)OMax)[11]=((Standard_Integer*)IMax)[11];
  if (((Standard_Integer*)OMin)[12] > ((Standard_Integer*)IMin)[12])
    ((Standard_Integer*)OMin)[12]=((Standard_Integer*)IMin)[12];
  if (((Standard_Integer*)OMax)[12] < ((Standard_Integer*)IMax)[12])
    ((Standard_Integer*)OMax)[12]=((Standard_Integer*)IMax)[12];
  if (((Standard_Integer*)OMin)[13] > ((Standard_Integer*)IMin)[13])
    ((Standard_Integer*)OMin)[13]=((Standard_Integer*)IMin)[13];
  if (((Standard_Integer*)OMax)[13] < ((Standard_Integer*)IMax)[13])
    ((Standard_Integer*)OMax)[13]=((Standard_Integer*)IMax)[13];
  if (((Standard_Integer*)OMin)[14] > ((Standard_Integer*)IMin)[14])
    ((Standard_Integer*)OMin)[14]=((Standard_Integer*)IMin)[14];
  if (((Standard_Integer*)OMax)[14] < ((Standard_Integer*)IMax)[14])
    ((Standard_Integer*)OMax)[14]=((Standard_Integer*)IMax)[14];
  if (((Standard_Integer*)OMin)[15] > ((Standard_Integer*)IMin)[15])
    ((Standard_Integer*)OMin)[15]=((Standard_Integer*)IMin)[15];
  if (((Standard_Integer*)OMax)[15] < ((Standard_Integer*)IMax)[15])
    ((Standard_Integer*)OMax)[15]=((Standard_Integer*)IMax)[15];
}

