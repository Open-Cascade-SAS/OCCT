// File   OpenGl_GraphicDriver_6.cxx
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

void OpenGl_GraphicDriver::InitPick () {


  if (MyTraceLevel) {
    PrintFunction ("call_togl_init_pick");
  }
  call_togl_init_pick ();

}

void OpenGl_GraphicDriver::Pick (Graphic3d_CPick& ACPick) {

  Graphic3d_CPick MyCPick = ACPick;


  if (MyTraceLevel) {
    PrintFunction ("call_togl_pick");
    PrintCPick (MyCPick, 1);
  }
  call_togl_pick (&ACPick);

}

void OpenGl_GraphicDriver::PickId (const Graphic3d_CGroup& ACGroup) {

  Graphic3d_CGroup MyCGroup = ACGroup;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_pickid");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_pickid (&MyCGroup);

}
