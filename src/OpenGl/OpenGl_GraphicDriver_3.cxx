

// File   OpenGl_GraphicDriver_3.cxx
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

int GenerateMarkerBitmap( int theId, unsigned int theWidth, unsigned int theHeight, unsigned char* theArray);

//-Aliases

//-Global data definitions

//-Methods, in order

void OpenGl_GraphicDriver::ClearGroup (const Graphic3d_CGroup& ACGroup) {

  Graphic3d_CGroup MyCGroup = ACGroup;

  if (MyTraceLevel) 
  {
    PrintFunction ("call_togl_cleargroup");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_cleargroup (&MyCGroup);

}

void OpenGl_GraphicDriver::CloseGroup (const Graphic3d_CGroup& ACGroup) {

  Graphic3d_CGroup MyCGroup = ACGroup;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_closegroup");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_closegroup (&MyCGroup);

}

void OpenGl_GraphicDriver::FaceContextGroup (const Graphic3d_CGroup& ACGroup, const Standard_Integer NoInsert) {

  Graphic3d_CGroup MyCGroup = ACGroup;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_facecontextgroup");
    PrintCGroup (MyCGroup, 1);
    PrintInteger ("NoInsert", NoInsert);
  }
  call_togl_facecontextgroup (&MyCGroup, int (NoInsert));

}

void OpenGl_GraphicDriver::Group (Graphic3d_CGroup& ACGroup) {

  Graphic3d_CGroup MyCGroup = ACGroup;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_group");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_group (&MyCGroup);

}

void OpenGl_GraphicDriver::LineContextGroup (const Graphic3d_CGroup& ACGroup, const Standard_Integer NoInsert) {

  Graphic3d_CGroup MyCGroup = ACGroup;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_linecontextgroup");
    PrintCGroup (MyCGroup, 1);
    PrintInteger ("NoInsert", NoInsert);
  }
  call_togl_linecontextgroup (&MyCGroup, int (NoInsert));

}

void OpenGl_GraphicDriver::MarkerContextGroup (const Graphic3d_CGroup& ACGroup, const Standard_Integer NoInsert) {

  Graphic3d_CGroup MyCGroup = ACGroup;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_markercontextgroup");
    PrintCGroup (MyCGroup, 1);
    PrintInteger ("NoInsert", NoInsert);
  }
  call_togl_markercontextgroup (&MyCGroup, int (NoInsert));

}

void OpenGl_GraphicDriver::MarkerContextGroup (const Graphic3d_CGroup& ACGroup, 
                                               const Standard_Integer NoInsert,
                                               const Standard_Integer AMarkWidth,
                                               const Standard_Integer AMarkHeight,
                                               const Handle(TColStd_HArray1OfByte)& ATexture ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  int aByteWidth = AMarkWidth / 8;

  unsigned char *anArray = (unsigned char*) malloc(ATexture->Length());
  for( int anIndex = ATexture->Upper() - ATexture->Lower() - aByteWidth + 1; anIndex >= 0; anIndex -= aByteWidth )
    for( int i = 0; i < aByteWidth; i++ )
      anArray[ATexture->Upper() - ATexture->Lower() - aByteWidth + 1 - anIndex + i ] = ATexture->Value( anIndex + i + 1 );

  GenerateMarkerBitmap( (int)ACGroup.ContextMarker.Scale, AMarkWidth, AMarkHeight, anArray );
  if (MyTraceLevel) {
    PrintFunction ("call_togl_markercontextgroup");
    PrintCGroup (MyCGroup, 1);
    PrintInteger ("NoInsert", NoInsert);
  }
  call_togl_markercontextgroup (&MyCGroup, int (NoInsert));

  free(anArray);
}


void OpenGl_GraphicDriver::OpenGroup (const Graphic3d_CGroup& ACGroup) {

  Graphic3d_CGroup MyCGroup = ACGroup;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_opengroup");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_opengroup (&MyCGroup);

}

void OpenGl_GraphicDriver::RemoveGroup (const Graphic3d_CGroup& ACGroup) {

  Graphic3d_CGroup MyCGroup = ACGroup;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_removegroup");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_removegroup (&MyCGroup);

}

void OpenGl_GraphicDriver::TextContextGroup (const Graphic3d_CGroup& ACGroup, const Standard_Integer NoInsert) {

  Graphic3d_CGroup MyCGroup = ACGroup;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_textcontextgroup");
    PrintCGroup (MyCGroup, 1);
    PrintInteger ("NoInsert", NoInsert);
  }
  call_togl_textcontextgroup (&MyCGroup, int (NoInsert));

}
