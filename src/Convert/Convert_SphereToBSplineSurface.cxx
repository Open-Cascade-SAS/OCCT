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

//JCV 16/10/91

#include <Convert_SphereToBSplineSurface.hxx>
#include <gp.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>
#include <Standard_DomainError.hxx>

static const Standard_Integer TheUDegree  = 2;
static const Standard_Integer TheVDegree  = 2;
static const Standard_Integer MaxNbUKnots = 4;
static const Standard_Integer MaxNbVKnots = 3;
static const Standard_Integer MaxNbUPoles = 7;
static const Standard_Integer MaxNbVPoles = 5;


static void ComputePoles ( const Standard_Real R,
			   const Standard_Real U1, 
			   const Standard_Real U2,
			   const Standard_Real V1,
			   const Standard_Real V2,
			         TColgp_Array2OfPnt& Poles)
{
  Standard_Real deltaU = U2 - U1;
  Standard_Real deltaV = V2 - V1;

  Standard_Integer i, j;

  // Number of spans : maximum opening = 150 degrees ( = PI / 1.2 rds)
  Standard_Integer 
    nbUSpans = (Standard_Integer)IntegerPart( 1.2 * deltaU / M_PI) + 1;
  Standard_Integer  
    nbVSpans = (Standard_Integer)IntegerPart( 1.2 * deltaV / M_PI) + 1;
  Standard_Real AlfaU = deltaU / ( nbUSpans * 2);
  Standard_Real AlfaV = deltaV / ( nbVSpans * 2);

  Standard_Integer nbVP = 2 * nbVSpans + 1;

  Standard_Real x[MaxNbVPoles];
  Standard_Real z[MaxNbVPoles];

  x[0] = R * Cos( V1);
  z[0] = R * Sin( V1);

  Standard_Real VStart = V1;
  for ( i = 1; i <= nbVSpans; i++) {
    x[2*i-1] = R * Cos( VStart + AlfaV) / Cos( AlfaV);
    z[2*i-1] = R * Sin( VStart + AlfaV) / Cos( AlfaV);
    x[2*i]   = R * Cos( VStart + 2 * AlfaV);
    z[2*i]   = R * Sin( VStart + 2 * AlfaV);
    VStart += 2*AlfaV;
  }

  Standard_Real UStart = U1;
  for ( j = 0; j <= nbVP-1; j++) {
    Poles( 1, j+1) = gp_Pnt(x[j]*Cos(UStart),x[j]*Sin(UStart),z[j]);
  }

  for ( i = 1; i <= nbUSpans; i++) {
    for ( j = 0; j<= nbVP-1; j++) {
      Poles( 2*i, j+1) = gp_Pnt( x[j] * Cos(UStart+AlfaU) / Cos(AlfaU),
				 x[j] * Sin(UStart+AlfaU) / Cos(AlfaU),
				 z[j] );
      Poles(2*i+1,j+1) = gp_Pnt( x[j] * Cos(UStart+2*AlfaU),
			         x[j] * Sin(UStart+2*AlfaU),
			         z[j] );
    }
    UStart += 2*AlfaU;
  }
}

//=======================================================================
//function : Convert_SphereToBSplineSurface
//purpose  : 
//=======================================================================

Convert_SphereToBSplineSurface::Convert_SphereToBSplineSurface 
  (const gp_Sphere&    Sph,
   const Standard_Real U1 , 
   const Standard_Real U2 , 
   const Standard_Real V1 , 
   const Standard_Real V2  ) 
: Convert_ElementarySurfaceToBSplineSurface (MaxNbUPoles, MaxNbVPoles, 
					     MaxNbUKnots, MaxNbVKnots,
					     TheUDegree , TheVDegree)
{
  Standard_Real deltaU = U2 - U1;
  Standard_Real deltaV = V2 - V1;
  Standard_DomainError_Raise_if( (deltaU>2*M_PI) || (deltaU<0.) ||
				 (V1 < -M_PI/2.0) || (V2 > M_PI/2),
				"Convert_SphereToBSplineSurface");

  isuperiodic = Standard_False;
  isvperiodic = Standard_False;

  Standard_Integer i,j;
  // construction of the sphere in the reference mark xOy.

  // Number of spans : maximum opening = 150 degrees ( = PI / 1.2 rds)
  Standard_Integer 
    nbUSpans = (Standard_Integer)IntegerPart( 1.2 * deltaU / M_PI) + 1;
  Standard_Integer  
    nbVSpans = (Standard_Integer)IntegerPart( 1.2 * deltaV / M_PI) + 1;
  Standard_Real AlfaU = deltaU / ( nbUSpans * 2);
  Standard_Real AlfaV = deltaV / ( nbVSpans * 2);

  nbUPoles = 2 * nbUSpans + 1;
  nbVPoles = 2 * nbVSpans + 1;
  nbUKnots = nbUSpans + 1;
  nbVKnots = nbVSpans + 1;

  Standard_Real R = Sph.Radius();

  ComputePoles( R, U1, U2, V1, V2, poles); 
  
  for ( i = 1; i<= nbUKnots; i++) {
    uknots(i) = U1 + (i-1) * 2 * AlfaU;
    umults(i) = 2;
  }
  umults(1)++; umults(nbUKnots)++;
  for ( i = 1; i<= nbVKnots; i++) {
    vknots(i) = V1 + (i-1) * 2 * AlfaV;
    vmults(i) = 2;
  }
  vmults(1)++; vmults(nbVKnots)++;


  // Replace the bspline in the reference of the sphere.
  // and calculate the weight of the bspline.
  Standard_Real W1, W2;
  gp_Trsf Trsf;
  Trsf.SetTransformation( Sph.Position(), gp::XOY());

  for ( i = 1; i <= nbUPoles; i++) {
    if ( i % 2 == 0)  W1 = Cos(AlfaU);
    else              W1 = 1.;

    for ( j = 1; j <= nbVPoles; j++) {
      if ( j % 2 == 0)  W2 = Cos(AlfaV);
      else              W2 = 1.;

      weights( i, j) = W1 * W2;
      poles( i, j).Transform( Trsf);
    }
  }
}


//=======================================================================
//function : Convert_SphereToBSplineSurface
//purpose  : 
//=======================================================================

Convert_SphereToBSplineSurface::Convert_SphereToBSplineSurface 
  (const gp_Sphere&       Sph   , 
   const Standard_Real    Param1, 
   const Standard_Real    Param2,
   const Standard_Boolean UTrim  )
: Convert_ElementarySurfaceToBSplineSurface (MaxNbUPoles, MaxNbVPoles,
					     MaxNbUKnots, MaxNbVKnots,
					     TheUDegree , TheVDegree)
{
#ifndef No_Exception
  Standard_Real delta = Param2 - Param1;
#endif
  Standard_DomainError_Raise_if( (delta>2*M_PI) || (delta<0.),
				"Convert_SphereToBSplineSurface");

  Standard_Integer i, j;
  Standard_Real deltaU, deltaV;

  isuperiodic = !UTrim;
  isvperiodic = Standard_False;

  Standard_Real R = Sph.Radius();
  
  Standard_Real W1, W2, CosU, CosV;
  
  if ( isuperiodic) {
    ComputePoles(R, 0., 2.*M_PI, Param1, Param2, poles);
    
    nbUPoles = 6;
    nbUKnots = 4;
    
    deltaV = Param2 - Param1;
    Standard_Integer  
      nbVSpans = (Standard_Integer)IntegerPart( 1.2 * deltaV / M_PI) + 1;
    Standard_Real AlfaV = deltaV / ( nbVSpans * 2);
    nbVPoles = 2 * nbVSpans + 1;
    nbVKnots = nbVSpans + 1;
    
    for ( i = 1; i <= nbUKnots; i++) {
      uknots(i) = ( i-1) * 2. * M_PI /3.;
      umults(i) = 2;
    }
    for ( i = 1; i <= nbVKnots; i++) {
      vknots(i) = Param1 + (i-1) * 2 * AlfaV;
      vmults(i) = 2;
    }
    vmults(1)++; vmults(nbVKnots)++;
    
    CosU = 0.5;       // = Cos(pi /3)
    CosV = Cos(AlfaV);
  }
  else {
    ComputePoles(R, Param1, Param2, -M_PI/2., M_PI/2., poles);
    
    nbVPoles = 5;
    nbVKnots = 3;
    
    deltaU = Param2 - Param1;
    Standard_Integer  
      nbUSpans = (Standard_Integer)IntegerPart( 1.2 * deltaU / M_PI) + 1;
    Standard_Real AlfaU = deltaU / ( nbUSpans * 2);
    nbUPoles = 2 * nbUSpans + 1;
    nbUKnots = nbUSpans + 1;
    
    vknots(1) = -M_PI/2.;  vmults(1) = 3;
    vknots(2) =     0.;  vmults(2) = 2;
    vknots(3) =  M_PI/2.;  vmults(3) = 3;
    for ( i = 1; i <= nbUKnots; i++) {
      uknots(i) = Param1 + (i-1) * 2 * AlfaU;
      umults(i) = 2;
    }
    umults(1)++; umults(nbUKnots)++;

    CosV = 0.5;       // = Cos(pi /3)
    CosU = Cos(AlfaU);
  }

  // Replace the bspline in the mark of the sphere.
  // and calculate the weight of bspline.
  gp_Trsf Trsf;
  Trsf.SetTransformation( Sph.Position(), gp::XOY());

  for ( i = 1; i <= nbUPoles; i++) {
    if ( i % 2 == 0)  W1 = CosU;
    else              W1 = 1.;

    for ( j = 1; j <= nbVPoles; j++) {
      if ( j % 2 == 0)  W2 = CosV;
      else              W2 = 1.;

      weights( i, j) = W1 * W2;
      poles( i, j).Transform( Trsf);
    }
  }
}


//=======================================================================
//function : Convert_SphereToBSplineSurface
//purpose  : 
//=======================================================================

Convert_SphereToBSplineSurface::Convert_SphereToBSplineSurface 
  (const gp_Sphere& Sph)
: Convert_ElementarySurfaceToBSplineSurface (MaxNbUPoles, MaxNbVPoles,
					     MaxNbUKnots, MaxNbVKnots,
					     TheUDegree , TheVDegree)
{
  isuperiodic = Standard_True;
  isvperiodic = Standard_False;

  Standard_Real W1, W2;
  Standard_Integer i, j;

  nbUPoles = 6;
  nbVPoles = 5;
  nbUKnots = 4;
  nbVKnots = 3;

  // Construction of the sphere in the reference mark xOy.
  
  Standard_Real R = Sph.Radius();

  ComputePoles( R, 0., 2.*M_PI, -M_PI/2., M_PI/2., poles);

  uknots( 1) = 0.;
  uknots( 2) = 2. * M_PI / 3.;
  uknots( 3) = 4. * M_PI / 3.;
  uknots( 4) = 2. * M_PI;
  vknots( 1) = -M_PI/2.;
  vknots( 2) =     0.;
  vknots( 3) =  M_PI/2.;
  for ( i = 1; i <= 4; i++) {
    umults( i) = 2;
  }
  vmults(1) = vmults(3) = 3;
  vmults(2) = 2;

  // Replace the bspline in the mark of the sphere.
  // and calculate the weight of the bspline.
  gp_Trsf Trsf;
  Trsf.SetTransformation( Sph.Position(), gp::XOY());

  for ( i = 1; i <= nbUPoles; i++) {
    if ( i % 2 == 0)  W1 = 0.5;
    else              W1 = 1.;

    for ( j = 1; j <= nbVPoles; j++) {
      if ( j % 2 == 0)  W2 = Sqrt(2.) /2.;
      else              W2 = 1.;

      weights( i, j) = W1 * W2;
      poles( i, j).Transform( Trsf);
    }
  }
}

