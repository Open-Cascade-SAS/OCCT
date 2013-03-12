// File:	GeoAlgo_Sol.cxx
// Created:	Mon Dec 15 16:32:27 1997
// Author:	Cascade_Manager
//		<cascade@savv04>

#include "stdafx.h"

#include "GeoAlgo_Sol.hxx"

#include <Geom_BSplineSurface.hxx>
#include <Geom_Plane.hxx>

#include <GeomPlate_Surface.hxx>
#include <GeomPlate_MakeApprox.hxx>

#include <Plate_Plate.hxx>
#include <Plate_PinpointConstraint.hxx>

#include <TColgp_SequenceOfXYZ.hxx>
#include <TColgp_Array1OfXYZ.hxx>
#include <TColgp_Array1OfXY.hxx>
#include <TColgp_Array2OfPnt.hxx>

#include <TCollection_AsciiString.hxx>

#include <gp_Vec.hxx>

#include <Standard_Stream.hxx>



//=============================================================================
// Empty constructor
//=============================================================================
GeoAlgo_Sol::GeoAlgo_Sol():myIsDone(Standard_False)
{

}



//=============================================================================
// Constructor with a file name 
//=============================================================================
GeoAlgo_Sol::GeoAlgo_Sol(const Standard_CString aGroundName)
{
  myGround = Read(aGroundName);
  // if an error occurs in the construction the method IsDone 
  // returns False.
}



//=============================================================================
// Build(File)
// Build method from an empty object
//=============================================================================
void GeoAlgo_Sol::Build(const Standard_CString aGroundName)
{
  myGround = Read(aGroundName);
  // if an error occurs in the construction the method IsDone 
  // returns False.
}

//=============================================================================
// Build(Sequence of Points)
// Build method from an empty object
// Called also from the Builde method from a file
//=============================================================================
void GeoAlgo_Sol::Build(const TColgp_SequenceOfXYZ& seqOfXYZ)
{
  // Build the surface:
  // points are projected on plane z = 0
  // the projection vector for each point is computed 
  // These data give the input constraints loaded into plate algorithm

  myIsDone = Standard_True;
  Standard_Integer nbPnt = seqOfXYZ.Length();
  Standard_Integer i;

  //Filling plate
  Plate_Plate myPlate;
  cout<<"  * Nunber of points  = "<< nbPnt << endl;
  for (i=1; i<= nbPnt; i++) {
    gp_Pnt ptProj(seqOfXYZ.Value(i).X(), seqOfXYZ.Value(i).Y(), 0. );
    gp_Vec aVec( ptProj, seqOfXYZ.Value(i));
    gp_XY  pntXY(seqOfXYZ.Value(i).X(),seqOfXYZ.Value(i).Y());
    Plate_PinpointConstraint PCst( pntXY,aVec.XYZ()  );
    myPlate.Load(PCst);// Load plate
  }
  myPlate.SolveTI(2, 1.);// resolution 
  if (!myPlate.IsDone()) {
    cout<<" plate computation has failed"<< endl;
    myIsDone=Standard_False;
  }

// Computation of plate surface
  gp_Pnt Or(0,0,0.);
  gp_Dir Norm(0., 0., 1.);
  Handle(Geom_Plane) myPlane = 
    new Geom_Plane(Or, Norm);// Plane of normal Oz
  Handle(GeomPlate_Surface) myPlateSurf = 
    new GeomPlate_Surface( myPlane, myPlate);//plate surface

  GeomPlate_MakeApprox aMKS(myPlateSurf, Precision::Approximation(), 4, 7, 0.001, 0);//bspline surface
  myGround = aMKS.Surface();
  // if an error occurs in the construction the method IsDone 
  // returns False.
}


//=============================================================================
// Surface() 
// Returns the resulting surface as a bspline surface
//=============================================================================
Handle(Geom_BSplineSurface) GeoAlgo_Sol::Surface() const
{
  return myGround;
}



//============================================================================
// IsDone()
// Checks the  construction of the surface
//============================================================================ 
Standard_Boolean GeoAlgo_Sol::IsDone() const
{
// Returns True if the construction successes, False otherwise
  return myIsDone;
}



//=============================================================================
// Read(File)
// Private method called from constructor
//=============================================================================
Handle(Geom_BSplineSurface) GeoAlgo_Sol::Read(const Standard_CString aGroundName)
{
  // This methods read a file of points ans build a surface using plate algorithm

  myIsDone = Standard_True;
  Standard_Integer nbPnt=0;

  // Read points from the file
  filebuf fic;
  istream in(&fic);

  if (!fic.open(aGroundName,ios::in)){
    cout << " impossible to open a file : "<<aGroundName<<endl;
    myIsDone = Standard_False;
    return 0;
  }
  // Store the points into a sequence
  TColgp_SequenceOfXYZ seqOfXYZ;
  gp_XYZ pntXYZ;
  Standard_Real x,y,z;
  while (!in.fail()|| !in.eof()){
    if (in >> x && in >> y && in >> z){
      pntXYZ.SetX(x);
      pntXYZ.SetY(y);
      pntXYZ.SetZ(z);
      nbPnt++;
      seqOfXYZ.Append(pntXYZ);
    }
  }
  fic.close();
  Build(seqOfXYZ);
  return myGround;
}
