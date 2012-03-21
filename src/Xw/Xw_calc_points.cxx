// Created on: 2002-02-20
// Created by: Sergey ALTUKHOV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

