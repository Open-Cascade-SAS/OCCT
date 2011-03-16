

// File   OpenGl_GraphicDriver_2.cxx
// Created  Mardi 28 janvier 1997
// Author CAL

//-Copyright  MatraDatavision 1997

//-Version  

//-Design Declaration des variables specifiques aux Drivers

//-Warning  Un driver encapsule les Pex et OpenGl drivers

//-References 

//-Language C++ 2.0

//-Declarations

// for the class
#include <OpenGl_GraphicDriver.jxx>

#include <Aspect_DriverDefinitionError.hxx>

#include <OpenGl_tgl_funcs.hxx>

//-Aliases

//-Global data definitions

//-Methods, in order

Standard_Integer OpenGl_GraphicDriver::InquireLightLimit () {

  if (MyTraceLevel) {
    PrintFunction ("call_togl_inquirelight");
  }
  Standard_Integer Result = call_togl_inquirelight ();
  if (MyTraceLevel) {
    PrintIResult ("call_togl_inquirelight", Result);
  }
  return Result;

}

void OpenGl_GraphicDriver::InquireMat (const Graphic3d_CView& ACView, TColStd_Array2OfReal& AMatO, TColStd_Array2OfReal& AMatM) {

  Graphic3d_CView MyCView = ACView;

  Standard_Integer i, j;

  CALL_DEF_MATRIX4X4 ori_matrix;
  CALL_DEF_MATRIX4X4 map_matrix;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_inquiremat");
    PrintCView (MyCView, 1);
  }
  if (call_togl_inquiremat (&MyCView, ori_matrix, map_matrix) == 0)
    for (i=0; i<=3; i++)
      for (j=0; j<=3; j++) {
        AMatO (i, j)  = Standard_Real (ori_matrix[i][j]);
        AMatM (i, j)  = Standard_Real (map_matrix[i][j]);
      }
  else
    for (i=0; i<=3; i++)
      for (j=0; j<=3; j++) {
        if (i == j) {
          AMatM (i, j)  = 1.0;
          AMatO (i, j)  = 1.0;
        }
        else {
          AMatM (i, j)  = 0.0;
          AMatO (i, j)  = 0.0;
        }
      }

      if (MyTraceLevel) {
        PrintMatrix ("Orientation", AMatO);
        PrintMatrix ("Mapping", AMatM);
      }

}

Standard_Integer OpenGl_GraphicDriver::InquirePlaneLimit () {

  if (MyTraceLevel) {
    PrintFunction ("call_togl_inquireplane");
  }
  Standard_Integer Result = call_togl_inquireplane ();
  if (MyTraceLevel) {
    PrintIResult ("call_togl_inquireplane", Result);
  }
  return Result;

}

Standard_Integer OpenGl_GraphicDriver::InquireViewLimit () {

  if (MyTraceLevel) {
    PrintFunction ("call_togl_inquireview");
  }
  Standard_Integer Result = call_togl_inquireview ();
  if (MyTraceLevel) {
    PrintIResult ("call_togl_inquireview", Result);
  }
  return Result;

}



Standard_Boolean OpenGl_GraphicDriver::InquireTextureAvailable () {

  if (MyTraceLevel) {
    PrintFunction ("call_togl_inquiretexture");
  }
  Standard_Integer Result = call_togl_inquiretexture ();
  if (MyTraceLevel) {
    PrintIResult ("call_togl_inquiretexture", Result);
  }
  return Result;

}
