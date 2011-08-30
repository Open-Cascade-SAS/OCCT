
#define NOVIRTUAL

// File   OpenGl_GraphicDriver_710.cxx
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
#include <TCollection_AsciiString.hxx>

#include <Aspect_DriverDefinitionError.hxx>

#include <OpenGl_tgl_funcs.hxx>

//-Aliases

//-Global data definitions

//-Methods, in order

void OpenGl_GraphicDriver::Text 
(
 const Graphic3d_CGroup& ACGroup,
 const Standard_CString AText,
 const Graphic3d_Vertex& APoint,
 const Standard_Real AHeight,
 const Quantity_PlaneAngle AAngle,
 const Graphic3d_TextPath ATp,
 const Graphic3d_HorizontalTextAlignment AHta,
 const Graphic3d_VerticalTextAlignment AVta,
 const Standard_Boolean EvalMinMax
 ) 
{
  TCollection_ExtendedString TheText(AText);
  OpenGl_GraphicDriver::Text(ACGroup,TheText,APoint,AHeight,AAngle,ATp,AHta,AVta,EvalMinMax);
}

void OpenGl_GraphicDriver::Text 
(
 const Graphic3d_CGroup& ACGroup,
 const Standard_CString AText, 
 const Graphic3d_Vertex& APoint, 
 const Standard_Real AHeight, 
 const Standard_Boolean EvalMinMax
 ) 
{
  TCollection_ExtendedString TheText(AText);
  OpenGl_GraphicDriver::Text(ACGroup,TheText,APoint,AHeight,EvalMinMax);
}

void OpenGl_GraphicDriver::Text 
(
 const Graphic3d_CGroup& ACGroup,
 const TCollection_ExtendedString& AText, 
 const Graphic3d_Vertex& APoint, 
 const Standard_Real AHeight, 
 const Quantity_PlaneAngle AAngle, 
 const Graphic3d_TextPath ATp, 
 const Graphic3d_HorizontalTextAlignment AHta, 
 const Graphic3d_VerticalTextAlignment AVta, 
 const Standard_Boolean 
 ) 
{
  Graphic3d_CGroup MyCGroup = ACGroup;

  CALL_DEF_TEXT atext;

  Standard_Real X, Y, Z;
  APoint.Coord (X, Y, Z);
  atext.Position.x  = float (X);
  atext.Position.y  = float (Y);
  atext.Position.z  = float (Z);
  atext.Height    = float (AHeight);
  if (atext.Height < 0)
    atext.Height = DefaultTextHeight();
  atext.Angle   = float (AAngle);
  atext.Path    = int (ATp);
  atext.HAlign    = int (AHta);
  atext.VAlign    = int (AVta);
  atext.string    = (unsigned short *)AText.ToExtString ();

  if (MyTraceLevel) {
    PrintFunction ("call_togl_text");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_text (&MyCGroup, &atext);
}

void OpenGl_GraphicDriver::Text 
(
 const Graphic3d_CGroup& ACGroup,
 const TCollection_ExtendedString& AText, 
 const Graphic3d_Vertex& APoint, 
 const Standard_Real AHeight, 
 const Standard_Boolean 
 )
{
  Graphic3d_CGroup MyCGroup = ACGroup;

  CALL_DEF_TEXT atext;

  Standard_Real X, Y, Z;
  APoint.Coord (X, Y, Z);
  atext.Position.x  = float (X);
  atext.Position.y  = float (Y);
  atext.Position.z  = float (Z);
  atext.Height    = float (AHeight);
  if (atext.Height < 0)
    atext.Height = DefaultTextHeight();
  atext.Angle   = float (Standard_PI / 2.);
  atext.Path    = int (Graphic3d_TP_RIGHT);
  atext.HAlign    = int (Graphic3d_HTA_LEFT);
  atext.VAlign    = int (Graphic3d_VTA_BOTTOM);
  atext.string    = (unsigned short *)AText.ToExtString ();

  if (MyTraceLevel) {
    PrintFunction ("call_togl_text");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_text (&MyCGroup, &atext);
}
