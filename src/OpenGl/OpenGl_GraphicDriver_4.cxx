

// File   OpenGl_GraphicDriver_4.cxx
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

void OpenGl_GraphicDriver::ClearStructure (const Graphic3d_CStructure& ACStructure) {

  Graphic3d_CStructure MyCStructure = ACStructure;


  if (MyTraceLevel) {
    PrintFunction ("call_togl_clearstructure");
    PrintCStructure (MyCStructure, 1);
  }
  call_togl_clearstructure (&MyCStructure);

}

void OpenGl_GraphicDriver::Connect (const Graphic3d_CStructure& AFather, const Graphic3d_CStructure& ASon) {

  Graphic3d_CStructure MyFather = AFather;
  Graphic3d_CStructure MySon = ASon;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_connect");
    PrintString ("AFather", "");
    PrintCStructure (MyFather, 1);
    PrintString ("ASon", "");
    PrintCStructure (MySon, 1);
  }
  call_togl_connect (&MyFather, &MySon);

}

void OpenGl_GraphicDriver::ContextStructure (const Graphic3d_CStructure& ACStructure) {

  Graphic3d_CStructure MyCStructure = ACStructure;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_contextstructure");
    PrintCStructure (MyCStructure, 1);
    PrintCStructure (MyCStructure, 2);
  }
  call_togl_contextstructure (&MyCStructure);

}

void OpenGl_GraphicDriver::Disconnect (const Graphic3d_CStructure& AFather, const Graphic3d_CStructure& ASon) {

  Graphic3d_CStructure MyFather = AFather;
  Graphic3d_CStructure MySon = ASon;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_disconnect");
    PrintString ("AFather", "");
    PrintCStructure (AFather, 1);
    PrintString ("ASon", "");
    PrintCStructure (MySon, 1);
  }
  call_togl_disconnect (&MyFather, &MySon);

}

void OpenGl_GraphicDriver::DisplayStructure (const Graphic3d_CView& ACView, const Graphic3d_CStructure& ACStructure, const Standard_Integer APriority) {

  Graphic3d_CView MyCView = ACView;
  Graphic3d_CStructure MyCStructure = ACStructure;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_displaystructure");
    PrintCView (MyCView, 1);
    PrintCStructure (MyCStructure, 1);
    PrintInteger ("Priority", APriority);
  }
  call_togl_displaystructure (&MyCView, MyCStructure.Id, int (APriority));

}

void OpenGl_GraphicDriver::EraseStructure (const Graphic3d_CView& ACView, const Graphic3d_CStructure& ACStructure) {

  Graphic3d_CView MyCView = ACView;
  Graphic3d_CStructure MyCStructure = ACStructure;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_erasestructure");
    PrintCView (MyCView, 1);
    PrintCStructure (MyCStructure, 1);
  }
  call_togl_erasestructure (MyCView.ViewId, MyCStructure.Id);

}

void OpenGl_GraphicDriver::RemoveStructure (const Graphic3d_CStructure& ACStructure) {

  Graphic3d_CStructure MyCStructure = ACStructure;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_removestructure");
    PrintCStructure (MyCStructure, 1);
  }
  call_togl_removestructure (&MyCStructure);

}

void OpenGl_GraphicDriver::Structure (Graphic3d_CStructure& ACStructure) {

  Graphic3d_CStructure MyCStructure = ACStructure;
  if (MyTraceLevel) {
    PrintFunction ("call_togl_structure");
    PrintCStructure (MyCStructure, 1);
  }
  call_togl_structure (&MyCStructure);

}
