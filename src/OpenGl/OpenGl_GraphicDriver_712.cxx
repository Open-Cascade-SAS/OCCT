
// File         OpenGl_GraphicDriver_712.cxx
// Created      Mardi 28 janvier 1997
// Author       CAL

//-Copyright    MatraDatavision 1997

//-Version

//-Design       Declaration des variables specifiques aux Drivers

//-Warning      Un driver encapsule les Pex et OpenGl drivers

//-References

//-Language     C++ 2.0

//-Declarations

// for the class
#include <OpenGl_GraphicDriver.jxx>

#include <Aspect_DriverDefinitionError.hxx>

#include <OpenGl_tgl_funcs.hxx>

//-Aliases

//-Global data definitions

//-Methods, in order

void OpenGl_GraphicDriver::Bezier (const Graphic3d_CGroup& ACGroup,
                                   const Graphic3d_Array1OfVertex& ListVertex,
                                   const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Integer Lower  = ListVertex.Lower ();

  CALL_DEF_LISTPOINTS alpoints;

  alpoints.NbPoints = int (ListVertex.Length ());
  alpoints.TypePoints = 1;
  alpoints.UPoints.Points = (CALL_DEF_POINT *) &ListVertex (Lower);

  if (MyTraceLevel) {
    PrintFunction ("call_togl_bezier");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_bezier (&MyCGroup, &alpoints);

}

void OpenGl_GraphicDriver::Bezier (const Graphic3d_CGroup& ACGroup,
                                   const Graphic3d_Array1OfVertex& ListVertex,
                                   const TColStd_Array1OfReal& ListWeight,
                                   const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Integer i, j;
  Standard_Integer Lower  = ListVertex.Lower ();
  Standard_Integer Upper  = ListVertex.Upper ();

  CALL_DEF_LISTREALS alweights;
  float *weights;
  CALL_DEF_LISTPOINTS alpoints;

  j = ListWeight.Length ();

  alpoints.NbPoints = int (ListVertex.Length ());
  alpoints.TypePoints = 1;
  alpoints.UPoints.Points = (CALL_DEF_POINT *) &ListVertex (Lower);

  weights = new float [j];
  alweights.NbReals = j;
  alweights.Reals   = weights;

  Lower = ListWeight.Lower ();
  Upper = ListWeight.Upper ();

  // Parcours des poids
  for (j=0, i=Lower; i<=Upper; i++, j++)
    weights[j]  = float (ListWeight (i));

  if (MyTraceLevel) {
    PrintFunction ("call_togl_bezier_weight");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_bezier_weight (&MyCGroup, &alpoints, &alweights);

  // Desallocation dynamique
  delete [] weights;

}
