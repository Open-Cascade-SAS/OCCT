

// File   OpenGl_GraphicDriver_1.cxx
// Created  Mardi 28 janvier 1997
// Author CAL
// Modified GG 27/12/02 IMP120302 Add new method Begin(Aspect_Display)

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

//=======================================================================
//function : Begin
//purpose  : 
//=======================================================================

Standard_Boolean OpenGl_GraphicDriver::Begin (const Standard_CString ADisplay)
{


  Standard_Boolean Result;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_begin");
    PrintString ("Display", ADisplay);
  }
  Result = call_togl_begin ((Standard_PCharacter)ADisplay);
  if (MyTraceLevel) {
    PrintIResult ("call_togl_begin", Result);
  }

  return Result;

}

//RIC120302
//=======================================================================
//function : Begin
//purpose  : 
//=======================================================================

Standard_Boolean OpenGl_GraphicDriver::Begin (const Aspect_Display ADisplay)
{

  Standard_Boolean Result;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_begin_display");
  }
  Result = call_togl_begin_display (ADisplay);
  if (MyTraceLevel) {
    PrintIResult ("call_togl_begin_display", Result);
  }

  return Result;

}
//RIC120302

//=======================================================================
//function : End
//purpose  : 
//=======================================================================

void OpenGl_GraphicDriver::End () {

  if (MyTraceLevel) {
    PrintFunction ("call_togl_end");
  }
  call_togl_end ();

}

//=======================================================================
//function : BeginAnimation
//purpose  : 
//=======================================================================

void OpenGl_GraphicDriver::BeginAnimation (const Graphic3d_CView& ACView) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_begin_animation");
    PrintCView (MyCView, 1);
  }
  call_togl_begin_animation (&MyCView);

}

//=======================================================================
//function : EndAnimation
//purpose  : 
//=======================================================================

void OpenGl_GraphicDriver::EndAnimation (const Graphic3d_CView& ACView) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_end_animation");
    PrintCView (MyCView, 1);
  }
  call_togl_end_animation (&MyCView);

}
