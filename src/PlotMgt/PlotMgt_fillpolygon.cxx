// File:        polyfill.cxx
// Created:     Thu Jun 12 16:46:18 1997
// Author:      Prihodyko Michael
//              <mpo@maniax>
// Modified by mpo, Tue Jul  8 15:45:06 1997

/*
// Description:
//  Fill closed polygon with rotated lines

// Some about algorithm:
//   rotate everything so that lines become horizontal,
//   seek intersections of each line with each side of polygon
//   then sort this array and each two points describe one part of line
//   when doing it some special cases need to be resolved
//   And then rotate it all in other direction

// Some about using it:
//   With InitFillPolygon() intitialize computations
//   Using GetLineOfFilledPolygon() you can get each line of filling
//   Call of DoneFillPolygon() will free all allocated memory and will end your computations

// double *X, *Y - coordinates of a points of a polygon
// n = count of points
// Point(X[0],Y[0]) = Point(X[n-1],Y[n-1]) (if no return 0)
// step - step of filling
// gamma -- angle of rotating filling
// Returns 0 if failts
// else return number of lines
int __InitFillPolygon(double* X, double* Y, int n, double step = 1, double gamma = 0);

// Input: n = n-th point 0 <= n < number_of_lines 
// Output: coordinates of a lines
// N - number of lines
// (X[2*i],Y[2*i+1]) - the i-th line , 0 <= i < N
// Return 0 if error
int __GetLineOfFilledPolygon (int n, int &N, double* &X, double* &Y);

// Done job and free allocated memory
void __DoneFillPolygon(void);
*/

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// EXAMPLE OF USE //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/*
  Standard_Integer count;
  static double Xx[1024], Yy[1024];
  // Fill Xx and Yy by the points
  ...
  // Filling of polygon by lines
  count = __InitFillPolygon(Xx, Yy, n, 0.25);
  for (Standard_Integer j = 0; j < count; j++) {
    Standard_Integer N;
    double *Xxx, *Yyy;
    if (__GetLineOfFilledPolygon(j, N, Xxx, Yyy))
      for (int k = 0; k < N; k += 2)
        DrawSegment(Xxx[k], Yyy[k], Xxx[k+1], Yyy[k+1]);
  }
  if (count) __DoneFillPolygon();
*/

//------------------  Implementation  -----------------------
#include <stdlib.h>
#include <math.h>
#include <Standard_Stream.hxx>
#include <PlotMgt_fill.hxx>

double mpo_Xmin, mpo_Xmax, mpo_Ymin, mpo_Ymax;   // BBox for polygon
double mpo_step; // step of filling
int mpo_size;  //  number of lines of filling (levels)
double **mpo_Xs, **mpo_Ys; // pointer to array of arrays
int *mpo_sizes; // mpo_sizes[i] number of points on level i
double *Xpts, *Ypts; //internal service

//
int mpo_sign(double a)
{
  if (a>0) return 1;
  else if (a == 0) return 0;
  else return -1;
}

//
int mpo_next_sign(int j, int n)
{
  j++;
  while (mpo_sign(Ypts[j+1] - Ypts[j]) == 0)
    if (j == n - 1) j = 0;
    else j++;
//  cout << mpo_sign(Ypts[j+1] - Ypts[j]) << "\t" << j << endl << flush;
  return mpo_sign(Ypts[j+1] - Ypts[j]);
}

//
double mpo_min(double a, double b)
{
  if (a > b) return b;
  else return a;
}

//
double mpo_max(double a, double b)
{
  if (a > b) return a;
  else return b;
}

int __InitFillPolygon(double* X, double* Y, int n, double step, double gamma)
{
  int i ;
  int j ;
  // checking
  if ((n < 2) || (X[0] != X[n-1]) || (Y[0] != Y[n-1])) return 0;
  mpo_step = step;
  
  // create local variables
  Xpts = (double*)malloc(sizeof(double)*(n+1));
  Ypts = (double*)malloc(sizeof(double)*(n+1));
  for ( i = 0; i < n; i++) Xpts[i] = X[i], Ypts[i] = Y[i];
  Xpts[n] = X[1]; Ypts[n] = Y[1];
    
  // rotate on gamma...
  for (i = 0; i < n; i++) 
    {
      double Yt = Ypts[i], Xt = Xpts[i]; 
      Xpts[i] =  Xt*cos(gamma) + Yt*sin(gamma);
      Ypts[i] = -Xt*sin(gamma) + Yt*cos(gamma);
    }
  
  // allocating variables
  mpo_Xmin = mpo_Xmax = Xpts[0];
  mpo_Ymin = mpo_Ymax = Ypts[0];
  for (i = 1; i < n; i++)
    {
      if (mpo_Xmin > Xpts[i]) mpo_Xmin = Xpts[i];
      if (mpo_Xmax < Xpts[i]) mpo_Xmax = Xpts[i];
      if (mpo_Ymin > Ypts[i]) mpo_Ymin = Ypts[i];
      if (mpo_Ymax < Ypts[i]) mpo_Ymax = Ypts[i];
    }
  mpo_size = int( floor((mpo_Ymax - mpo_Ymin)/step) );
  mpo_Xs = (double **)malloc(sizeof(double)*mpo_size);
  mpo_Ys = (double **)malloc(sizeof(double)*mpo_size);
  mpo_sizes = (int *)malloc(sizeof(int)*mpo_size);
  for (i = 0; i < mpo_size; i++)
    {
      int k = 0;
      double level = mpo_Ymax - step*i;
      for ( j = 0; j < n-1; j++)
        if (Ypts[j] - Ypts[j+1] != 0)
          {
            if (((level > mpo_min(Ypts[j], Ypts[j+1])) && (level < mpo_max(Ypts[j], Ypts[j+1]))) 
                || (level == Ypts[j])) k++;
            if (level == Ypts[j+1])
              if (mpo_sign(Ypts[j] - Ypts[j+1]) == mpo_next_sign(j, n)) k++;
              else;
            else;
          }
      mpo_Xs[i] = (double *)malloc(sizeof(double)*k);
      mpo_Ys[i] = (double *)malloc(sizeof(double)*k);
      for (j = 0; j < k; j++) mpo_Ys[i][j] = level;
      mpo_sizes[i] = k;
    }
  
  // evaluating variables
  for (i = 0; i < mpo_size; i++)
    {
      int k = 0;
      double level = mpo_Ymax - step*i;
      for ( j = 0; j < n-1; j++)
        if (Ypts[j] - Ypts[j+1] != 0)
          {
            if (((level > mpo_min(Ypts[j], Ypts[j+1])) && (level < mpo_max(Ypts[j], Ypts[j+1]))) 
                || (level == Ypts[j])) 
              {
                if (Ypts[j+1] - Ypts[j] == 0) mpo_Xs[i][k] = Xpts[j];
                else mpo_Xs[i][k] = Xpts[j] + (Xpts[j+1] - Xpts[j])*(level - Ypts[j])/(Ypts[j+1] - Ypts[j]); 
                k++;
              }
            if (level == Ypts[j+1])
              if (mpo_sign(Ypts[j] - Ypts[j+1]) == mpo_next_sign(j, n)) 
                {
                  mpo_Xs[i][k] = Xpts[j+1]; k++;
                }
              else;
            else;
          }

      if (k != mpo_sizes[i]) 
        {
          cout << "Number mismatch: k = " << k << "\tsizes[i] = " << mpo_sizes[i] << endl << flush;
          free(Xpts); free(Ypts);
          return 0;
        }
    }
  
  // sorting
  for (int m = 0; m < mpo_size; m++)
    for (i = 0; i < mpo_sizes[m]-1; i++)
      for ( j = 0; j < mpo_sizes[m]-1; j++)
        if (mpo_Xs[m][j] > mpo_Xs[m][j+1])
          {
            double tmp;
            tmp = mpo_Xs[m][j+1]; mpo_Xs[m][j+1] = mpo_Xs[m][j]; mpo_Xs[m][j] = tmp;
            tmp = mpo_Ys[m][j+1]; mpo_Ys[m][j+1] = mpo_Ys[m][j]; mpo_Ys[m][j] = tmp;
          }

  // rotate on -gamma
  for (i = 0; i < mpo_size; i++) 
    for ( j = 0; j < mpo_sizes[i]; j++)
      {
        double Yt = mpo_Ys[i][j], Xt = mpo_Xs[i][j]; 
        mpo_Xs[i][j] =  Xt*cos(gamma) - Yt*sin(gamma);
        mpo_Ys[i][j] =  Xt*sin(gamma) + Yt*cos(gamma);
      }
  
  // end
  free(Xpts); free(Ypts);
  return mpo_size;
}

int __GetLineOfFilledPolygon (int n, int &N, double* &X, double* &Y)
{
//  cout << "GetLineOfFilledPolygon -- n = " << n << endl << flush;
  if ((n < 0) || (n >= mpo_size)) return 0;
  N = mpo_sizes[n];
  X = mpo_Xs[n]; Y = mpo_Ys[n];
  return 1;
}

void __DoneFillPolygon(void)
{
  for (int i = 0; i < mpo_size; i++) 
    {
      free(mpo_Xs[i]); free(mpo_Ys[i]);
    }
  free(mpo_sizes);
}
