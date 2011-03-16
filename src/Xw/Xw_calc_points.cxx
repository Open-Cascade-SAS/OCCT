// File:	Xw_points.cxx
// Created:	Wed Feb 20 17:50:06 2002
// Author:	Sergey ALTUKHOV
//		<sav@vovox>

#include <Standard_Integer.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Xw_Extension.h>


#ifdef XW_PROTOTYPE
int PXPOINT( double x, double ratio )
#else
int PXPOINT( x, ratio )
double x, ratio;
#endif /*XW_PROTOTYPE*/
{
  int result = 0;
  try {
    OCC_CATCH_SIGNALS
    result = (int) ( x / ratio );
  }
  catch (Standard_Failure) {
    result = ROUND( x / ratio );
  }
  return result;
}


#ifdef XW_PROTOTYPE
int PYPOINT( double y, double height, double ratio )
#else
int PYPOINT( y, height, ratio )
double y, height, ratio;
#endif /*XW_PROTOTYPE*/
{
  int result = 0;
  try {
    OCC_CATCH_SIGNALS
    result = (int)( height - ( y / ratio ) );
  }
  catch (Standard_Failure) {
    result = ROUND( height - ( y / ratio ) );
  }
  return result;
}


#ifdef XW_PROTOTYPE
int PVALUE( double v, double xratio, double yratio )
#else
int PVALUE( v, xratio, yratio )
double v, xratio, yratio;
#endif /*XW_PROTOTYPE*/
{
  int result = 0;
  try {
    OCC_CATCH_SIGNALS
    result = (int)( v / ( ( xratio + yratio ) / 2. ) );
  }
  catch (Standard_Failure) {
    result = ROUND( v / ( ( xratio + yratio ) / 2. ) );
  }
  return result;
}

