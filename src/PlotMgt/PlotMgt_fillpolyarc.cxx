// Created on: 1997-05-21
// Created by: Prihodyko Michael
// Copyright (c) 1997-1999 Matra Datavision
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

// Modified by mpo, Tue Jul  8 15:31:23 1997
// Modified     23/02/98 : FMN ; Remplacement PI par Standard_PI


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// EXAMPLE OF USE //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/*
  mpo_one_line aLine;
  // Drawing filled arc
  Standard_Integer n = __InitFillArc (X, Y, anXradius, anYradius, aStartAngle, anOpenAngle,
                                      0.0, 0.0, 0.2*(myHeight/myWidth));
  for (Standard_Integer i = 1; i <= n; i++) {
    __GetLineOfFilledArc (i, aLine);
    DrawSegment (aLine.X1, aLine.Y1, aLine.X2, aLine.Y2);
    if (aLine.type == 2)
      DrawSegment (aLine.X3, aLine.Y3, aLine.X4, aLine.Y4);
  }
  __DoneFillArc ();
*/

//--------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#ifndef __MATH_WNT_H
# include <Standard_math.hxx>
#endif  /* __MATH_WNT_H */
#include <Standard_Stream.hxx>
#include <PlotMgt_fill.hxx>
#define maxDouble ((double)1.E+30)
//--------------------------------------------------------

/*
// Description:
//   fill sectorof arc with rotated lines and
//   arc can be rotated too...

// Some about algorithm:
//   rotate everything so, that lines become horizontal and then
//   seek points of intersection between each line and sector and
//   then rotate it in other way

// Some about using it:
//   Call __InitFillArc() to initialize computations
//   Using __GetLineOfFilledArc() you can get each line of filling
//   Call of __DoneFillArc() will free all allocated memory and will end your computations

// Initialize sector filling
// X, Y -- coordinates of ellipse
// a, b -- radiuses of ellipse
// alpha -- start angle of fill, beta -- add angle of filling
// gamma -- rotation angle of filling (non-dependent of delta)
// delta -- rotation angle of ellipse
// step -- filling step
int __InitFillArc(double X,  double Y, double a, double b, double alpha, 
                  double beta, double gamma, double delta, double step);

// Get another line of fill
// n -- number of line
// data -- structure for retrieving coordinates of points
// data->n -- number of lines = {0, 1, 2}
//   if data->n = 1 line is (data->X1, data->Y1, data->X2, data->Y2)
//   if data->n = 2 line is (data->X1, data->Y1, data->X2, data->Y2) and 
//                          (data->X3, data->Y3, data->X4, data->Y4)
int __GetLineOfFilledArc(int n, mpo_one_line &data);

// Finish all job
void __DoneFillArc(void);
*/

//----------------------------- Implementation -----------------------

int mpo_count;
int mpo_start;
mpo_one_line *mpo_lines;

//Truncate double number
double ftrunc(double a)
{
  a = floor(a);
  if (a<0) a++;
  return a;
}

//trigon functs
double fsin(double a)
{
  return sin(a);
}
double fcos(double a)
{
  return cos(a);
}
double ftan(double a)
{
  return tan(a);
}
double fatan2(double a, double b)
{
  return atan2(a, b);
}
double fsqrt(double a)
{
  return sqrt(a);
}

// Determine is angle a is between b and b+c (c>0)
int mpo_inside(double a, double b, double c)
{
  while (b<0) {b += 2*M_PI;}; b -= 2*M_PI*ftrunc(b/(2*M_PI));
  c -= 2*M_PI*ftrunc(c/(2*M_PI));
  while (a<0) {a+=2*M_PI;}; a -= 2*M_PI*ftrunc(a/(2*M_PI));
//  while(c<0) {c+=2*M_PI;}; c-=2*M_PI*ftrunc(c/(2*M_PI));
  if ((a > b) && (a < b+c)) return 1;
  else if ((2*M_PI+a > b) && (2*M_PI+a < b+c)) return 1;
  return 0;
}

int __InitFillArc(double X,  double Y, double a, double b, double alpha, 
                  double beta, double gamma, double delta, double step)
{
  int i ;
  alpha -= 2*M_PI*ftrunc(alpha/(2*M_PI)); beta -= 2*M_PI*ftrunc(beta/(2*M_PI));
  if (beta < 0.0) 
    {
      alpha += beta; beta = -beta;
    }
  delta -= gamma; alpha -= gamma;

  double C1 = b*b*fcos(delta)*fcos(delta) + a*a*fsin(delta)*fsin(delta);
  double C3 = b*b*fsin(delta)*fsin(delta) + a*a*fcos(delta)*fcos(delta);
  double C2 = fsin(delta)*fcos(delta)*(b*b - a*a);
//  cout << "C1 = " << C1 << ";    C2 = " << C2 << ";   C3 = " << C3 << endl << flush;
  double Ys = a*b/fsqrt(C3 - C2*C2/C1)-0.5*step;
  int size = int( ftrunc(2*Ys/step) + 1 );
//  cout << "Ysize = " << Ys << endl << flush;
//  cout << "Size  = " << size << endl << flush;
//  cout << "Alpha = " << alpha*180/M_PI << endl << flush;
//  cout << "Beta  = " << beta*180/M_PI << endl << flush;
  mpo_lines = (mpo_one_line*)malloc(sizeof(mpo_one_line)*size);
  for ( i = 0; i < size; i++)
    {
      double Yt = Ys - i*step;
      (mpo_lines+i)->X1 = (-C2*Yt - fsqrt(C2*C2*Yt*Yt - C1*(C3*Yt*Yt - a*a*b*b)))/C1;
      (mpo_lines+i)->X2 = (-C2*Yt + fsqrt(C2*C2*Yt*Yt - C1*(C3*Yt*Yt - a*a*b*b)))/C1;
//    cout << "alpha = " << alpha*180/M_PI << "    alpha+beta = " << (alpha+beta)*180/M_PI << endl << flush;
//    cout << "" << fatan2(Yt, (mpo_lines+i)->X2)*180/M_PI << "     type " << (mpo_lines+i)->type << endl << flush;;
//    cout << "Xleft = " << (mpo_lines+i)->X1 << "     Xright = " << (mpo_lines+i)->X2 << endl << flush;
//    cout << "C2*C2 - C1*(C3*Yt*Yt - a*a*b*b) = " << C2*C2 - C1*(C3*Yt*Yt - a*a*b*b) << endl << flush;
//    cout << "C1*(C3*Yt*Yt - a*a*b*b) = " << C1*(C3*Yt*Yt - a*a*b*b) << endl << flush;
//    cout << "C2*C2 = " << C2*C2 << endl << flush;
      if (Yt > 0.0)
        {
          if (fsin(alpha) <= 0.0) (mpo_lines+i)->X3 = maxDouble;
          else (mpo_lines+i)->X3 = Yt/ftan(alpha);
          if (fsin(alpha+beta) <= 0.0) (mpo_lines+i)->X4 = maxDouble;
          else (mpo_lines+i)->X4 = Yt/ftan(alpha+beta);
          if (((mpo_lines+i)->X3 > (mpo_lines+i)->X2) || ((mpo_lines+i)->X3 < (mpo_lines+i)->X1))
            (mpo_lines+i)->X3 = maxDouble; 
          if (((mpo_lines+i)->X4 > (mpo_lines+i)->X2) || ((mpo_lines+i)->X4 < (mpo_lines+i)->X1))
            (mpo_lines+i)->X4 = maxDouble; 
          if (((mpo_lines+i)->X3 != maxDouble)&&((mpo_lines+i)->X4 != maxDouble))
            if ((mpo_lines+i)->X3 <= (mpo_lines+i)->X4) (mpo_lines+i)->type = 0;
            else 
              {
                (mpo_lines+i)->type = 1;
                double tmp = (mpo_lines+i)->X3; (mpo_lines+i)->X3 = (mpo_lines+i)->X4; (mpo_lines+i)->X4 = tmp; 
              }
          else if ((mpo_lines+i)->X3 != maxDouble) 
            {
              (mpo_lines+i)->type = 2; (mpo_lines+i)->X4 = (mpo_lines+i)->X3;
            }
          else if ((mpo_lines+i)->X4 != maxDouble) 
            {
              (mpo_lines+i)->type = 3; (mpo_lines+i)->X3 = (mpo_lines+i)->X4;
            }
          else if (mpo_inside(fatan2((double)Yt,(double)((mpo_lines+i)->X2)), alpha, beta))
            (mpo_lines+i)->type = 5;
          else (mpo_lines+i)->type = 4;
        }
      else if (Yt <= 0.0)   //!!!!!
        {
          if (fsin(alpha) >= 0.0) (mpo_lines+i)->X3 = maxDouble;
          else (mpo_lines+i)->X3 = Yt/ftan(alpha);
          if (fsin(alpha+beta) >= 0.0) (mpo_lines+i)->X4 = maxDouble;
          else (mpo_lines+i)->X4 = Yt/ftan(alpha+beta);
          if (((mpo_lines+i)->X3 > (mpo_lines+i)->X2) || ((mpo_lines+i)->X3 < (mpo_lines+i)->X1))
            (mpo_lines+i)->X3 = maxDouble; 
          if (((mpo_lines+i)->X4 > (mpo_lines+i)->X2) || ((mpo_lines+i)->X4 < (mpo_lines+i)->X1))
            (mpo_lines+i)->X4 = maxDouble; 
          if (((mpo_lines+i)->X3 != maxDouble)&&((mpo_lines+i)->X4 != maxDouble))
            if ((mpo_lines+i)->X3 >= (mpo_lines+i)->X4) 
              {
                (mpo_lines+i)->type = 0; 
                double tmp = (mpo_lines+i)->X3; (mpo_lines+i)->X3 = (mpo_lines+i)->X4; (mpo_lines+i)->X4 = tmp; 
              }
            else (mpo_lines+i)->type = 1;
          else if ((mpo_lines+i)->X3 != maxDouble) (mpo_lines+i)->type = 3;
          else if ((mpo_lines+i)->X4 != maxDouble) (mpo_lines+i)->type = 2;
          else if (mpo_inside(fatan2((double)Yt, (double)((mpo_lines+i)->X2)), alpha, beta))
            (mpo_lines+i)->type = 5;
          else (mpo_lines+i)->type = 4;
        }
      else;
//      cout << "Xlm  = " << (mpo_lines+i)->X3 << "      Xrm  = " << (mpo_lines+i)->X4 << endl << flush;
//      cout << "------------ i = " << i << endl << flush;
    }
  mpo_start = 0; mpo_count = 0;
  while (((mpo_lines+mpo_start)->type == 4)&&(mpo_start < size)) mpo_start++;
  while (((mpo_lines+mpo_start+mpo_count)->type != 4)&&((mpo_start+mpo_count < size))) mpo_count++;  
//  cout << "start = " << mpo_start << "   count = " << mpo_count << endl << flush;
  for (i = mpo_start; i < mpo_count+mpo_start; i++) 
    {
      double Yt = Ys - i*step;
      (mpo_lines+i)->Y1 =  (mpo_lines+i)->X1*fsin(gamma) + Yt*fcos(gamma) + Y;
      (mpo_lines+i)->X1 =  (mpo_lines+i)->X1*fcos(gamma) - Yt*fsin(gamma) + X;
      (mpo_lines+i)->Y2 =  (mpo_lines+i)->X2*fsin(gamma) + Yt*fcos(gamma) + Y;
      (mpo_lines+i)->X2 =  (mpo_lines+i)->X2*fcos(gamma) - Yt*fsin(gamma) + X;
      (mpo_lines+i)->Y3 =  (mpo_lines+i)->X3*fsin(gamma) + Yt*fcos(gamma) + Y;
      (mpo_lines+i)->X3 =  (mpo_lines+i)->X3*fcos(gamma) - Yt*fsin(gamma) + X;
      (mpo_lines+i)->Y4 =  (mpo_lines+i)->X4*fsin(gamma) + Yt*fcos(gamma) + Y;
      (mpo_lines+i)->X4 =  (mpo_lines+i)->X4*fcos(gamma) - Yt*fsin(gamma) + X;
    }
  return mpo_count;
}

// Get another line of fill
// n -- number of line
// data -- structure for retrieving coordinates of points
// data->n -- number of lines = {0, 1, 2}
//   data->n = 1 line is (data->X1, data->Y1, data->X2, data->Y2)
//   data->n = 2 line is (data->X1, data->Y1, data->X2, data->Y2) and (data->X3, data->Y3, data->X4, data->Y4)
int __GetLineOfFilledArc(int n, mpo_one_line &data)
{
  if (n > mpo_count) return 0; n--; n += mpo_start;
  switch((mpo_lines+n)->type) {
    case 0:
      data.type = 2;
      data.X1 = (mpo_lines+n)->X1;
      data.Y1 = (mpo_lines+n)->Y1;
      data.X2 = (mpo_lines+n)->X3;
      data.Y2 = (mpo_lines+n)->Y3;
      data.X3 = (mpo_lines+n)->X4;
      data.Y3 = (mpo_lines+n)->Y4;
      data.X4 = (mpo_lines+n)->X2;
      data.Y4 = (mpo_lines+n)->Y2;
      break;
    case 1:
      data.type = 1;
      data.X1 = (mpo_lines+n)->X3;
      data.Y1 = (mpo_lines+n)->Y3;
      data.X2 = (mpo_lines+n)->X4;
      data.Y2 = (mpo_lines+n)->Y4;
      break;
    case 2:
      data.type = 1;
      data.X1 = (mpo_lines+n)->X1;
      data.Y1 = (mpo_lines+n)->Y1;
      data.X2 = (mpo_lines+n)->X4;
      data.Y2 = (mpo_lines+n)->Y4;
      break;
    case 3:
      data.type = 1;
      data.X1 = (mpo_lines+n)->X3;
      data.Y1 = (mpo_lines+n)->Y3;
      data.X2 = (mpo_lines+n)->X2;
      data.Y2 = (mpo_lines+n)->Y2;
      break;
    case 4:
      cout << "??????????????????" << endl << flush;  // doesn't exist
      break;
    case 5:
      data.type = 1;
      data.X1 = (mpo_lines+n)->X1;
      data.Y1 = (mpo_lines+n)->Y1;
      data.X2 = (mpo_lines+n)->X2;
      data.Y2 = (mpo_lines+n)->Y2;
      break;
  }
  return 1;
}

//Ending job with filler
void __DoneFillArc(void)
{
  free(mpo_lines);
}
