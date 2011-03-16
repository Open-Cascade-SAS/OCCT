
// File   OpenGl_GraphicDriver_703.cxx
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
#include <Aspect_TypeOfMarker.hxx>

#include <Aspect_DriverDefinitionError.hxx>

#include <OpenGl_tgl_funcs.hxx>

int GenerateMarkerBitmap( unsigned int theWidth, unsigned int theHeight, unsigned char* theArray);

//-Aliases

//-Global data definitions

//-Methods, in order

void OpenGl_GraphicDriver::Marker (const Graphic3d_CGroup& ACGroup, const Graphic3d_Vertex& APoint, const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  CALL_DEF_MARKER amarker;

  amarker.x = float (APoint.X ());
  amarker.y = float (APoint.Y ());
  amarker.z = float (APoint.Z ());

  Standard_Real AMR, AMG, AMB, R, G, B;
  Aspect_TypeOfMarker AMType;
  Standard_Real AMScale, AScale;

  // Sauvegarde des valeurs initiales
  AMR = R = Standard_Real (MyCGroup.ContextMarker.Color.r);
  AMG = G = Standard_Real (MyCGroup.ContextMarker.Color.g);
  AMB = B = Standard_Real (MyCGroup.ContextMarker.Color.b);
  AMType  = Aspect_TypeOfMarker (MyCGroup.ContextMarker.MarkerType);
  AMScale = AScale = Standard_Real (MyCGroup.ContextMarker.Scale);

  int noinsert  = 0;
  Standard_Real H, L, S;
  Standard_Real LastS;
  Standard_Real Limit=0, Delta;

  switch (AMType) 
  {

  case Aspect_TOM_POINT :
  case Aspect_TOM_PLUS :
  case Aspect_TOM_STAR :
  case Aspect_TOM_O :
  case Aspect_TOM_X :
  case Aspect_TOM_USERDEFINED:
    if (MyTraceLevel) 
    {
      PrintFunction ("call_togl_marker");
      PrintCGroup (MyCGroup, 1);
    }
    call_togl_marker (&MyCGroup, &amarker);
    break;

  case Aspect_TOM_O_POINT :
    MyCGroup.ContextMarker.IsDef  = 1;

    // we draw the circle
    MyCGroup.ContextMarker.MarkerType =
      int (Aspect_TOM_O);
    call_togl_markercontextgroup (&MyCGroup, noinsert);

    MyCGroup.ContextMarker.IsSet  = 1;

    if (MyTraceLevel) {
      PrintFunction ("call_togl_marker");
      PrintCGroup (MyCGroup, 1);
    }
    call_togl_marker (&MyCGroup, &amarker);

    // we draw the point
    MyCGroup.ContextMarker.MarkerType =
      int (Aspect_TOM_POINT);
    call_togl_markercontextgroup (&MyCGroup, noinsert);
    if (MyTraceLevel) {
      PrintFunction ("call_togl_marker");
      PrintCGroup (MyCGroup, 1);
    }
    call_togl_marker (&MyCGroup, &amarker);
    break;

  case Aspect_TOM_O_PLUS :
    MyCGroup.ContextMarker.IsDef  = 1;

    // we draw the circle
    MyCGroup.ContextMarker.MarkerType =
      int (Aspect_TOM_O);
    call_togl_markercontextgroup (&MyCGroup, noinsert);

    MyCGroup.ContextMarker.IsSet  = 1;

    if (MyTraceLevel) {
      PrintFunction ("call_togl_marker");
      PrintCGroup (MyCGroup, 1);
    }
    call_togl_marker (&MyCGroup, &amarker);

    // we draw the plus
    MyCGroup.ContextMarker.MarkerType =
      int (Aspect_TOM_PLUS);
    call_togl_markercontextgroup (&MyCGroup, noinsert);
    if (MyTraceLevel) {
      PrintFunction ("call_togl_marker");
      PrintCGroup (MyCGroup, 1);
    }
    call_togl_marker (&MyCGroup, &amarker);
    break;

  case Aspect_TOM_O_STAR :
    MyCGroup.ContextMarker.IsDef  = 1;

    // we draw the circle
    MyCGroup.ContextMarker.MarkerType =
      int (Aspect_TOM_O);
    call_togl_markercontextgroup (&MyCGroup, noinsert);

    MyCGroup.ContextMarker.IsSet  = 1;

    if (MyTraceLevel) {
      PrintFunction ("call_togl_marker");
      PrintCGroup (MyCGroup, 1);
    }
    call_togl_marker (&MyCGroup, &amarker);

    // we draw the star
    MyCGroup.ContextMarker.MarkerType =
      int (Aspect_TOM_STAR);
    call_togl_markercontextgroup (&MyCGroup, noinsert);
    if (MyTraceLevel) {
      PrintFunction ("call_togl_marker");
      PrintCGroup (MyCGroup, 1);
    }
    call_togl_marker (&MyCGroup, &amarker);
    break;

  case Aspect_TOM_O_X :
    MyCGroup.ContextMarker.IsDef  = 1;

    // we draw the circle
    MyCGroup.ContextMarker.MarkerType =
      int (Aspect_TOM_O);
    call_togl_markercontextgroup (&MyCGroup, noinsert);

    MyCGroup.ContextMarker.IsSet  = 1;

    if (MyTraceLevel) {
      PrintFunction ("call_togl_marker");
      PrintCGroup (MyCGroup, 1);
    }
    call_togl_marker (&MyCGroup, &amarker);

    // we draw the cross
    MyCGroup.ContextMarker.MarkerType =
      int (Aspect_TOM_X);
    call_togl_markercontextgroup (&MyCGroup, noinsert);
    if (MyTraceLevel) {
      PrintFunction ("call_togl_marker");
      PrintCGroup (MyCGroup, 1);
    }
    call_togl_marker (&MyCGroup, &amarker);
    break;

  case Aspect_TOM_BALL :
    MyCGroup.ContextMarker.IsDef  = 1;

    // we draw a set of circles
    MyCGroup.ContextMarker.MarkerType =
      int (Aspect_TOM_O);
    Delta = 0.1;
    while (AScale >= 1.0) {
      Quantity_Color::RgbHls (R, G, B, H, L, S);
      // Modification de la saturation de 5 %
      LastS = S;
      S = S - S * 0.05;
      if (S < 0.0) S = LastS;
      Quantity_Color::HlsRgb (H, L, S, R, G, B);
      MyCGroup.ContextMarker.Color.r  = float (R);
      MyCGroup.ContextMarker.Color.g  = float (G);
      MyCGroup.ContextMarker.Color.b  = float (B);
      MyCGroup.ContextMarker.Scale  = float (AScale);
      call_togl_markercontextgroup (&MyCGroup, noinsert);

      MyCGroup.ContextMarker.IsSet  = 1;

      if (MyTraceLevel) {
        PrintFunction ("call_togl_marker");
        PrintCGroup (MyCGroup, 1);
      }
      call_togl_marker (&MyCGroup, &amarker);
      AScale  -= Delta;
    }
    break;

  case Aspect_TOM_RING1 :
  case Aspect_TOM_RING2 :
  case Aspect_TOM_RING3 :
    MyCGroup.ContextMarker.IsDef  = 1;

    // we draw a set of circles
    MyCGroup.ContextMarker.MarkerType =
      int (Aspect_TOM_O);
    if (AMType == Aspect_TOM_RING1)
      Limit = AScale - AScale * 0.8;
    if (AMType == Aspect_TOM_RING2)
      Limit = AScale - AScale * 0.5;
    if (AMType == Aspect_TOM_RING3)
      Limit = AScale - AScale * 0.2;
    Delta = 0.1;
    while (AScale > Limit && AScale >= 1.0) {
      MyCGroup.ContextMarker.Scale  = float (AScale);
      call_togl_markercontextgroup (&MyCGroup, noinsert);

      MyCGroup.ContextMarker.IsSet  = 1;

      if (MyTraceLevel) {
        PrintFunction ("call_togl_marker");
        PrintCGroup (MyCGroup, 1);
      }
      call_togl_marker (&MyCGroup, &amarker);
      AScale  -= Delta;
    }
    break ;
  default:
    break ;
  }

  // Restauration des valeurs initiales
  MyCGroup.ContextMarker.Color.r  = float (AMR);
  MyCGroup.ContextMarker.Color.g  = float (AMR);
  MyCGroup.ContextMarker.Color.b  = float (AMR);
  MyCGroup.ContextMarker.MarkerType = int (AMType);
  MyCGroup.ContextMarker.Scale    = float (AMScale);

}

void OpenGl_GraphicDriver::MarkerSet (const Graphic3d_CGroup& ACGroup,
                                      const Graphic3d_Array1OfVertex& ListVertex,
                                      const Standard_Boolean ) 
{
  //                                      const Standard_Boolean EvalMinMax) {

  Graphic3d_CGroup MyCGroup = ACGroup;

  //static void (*fp2) (Graphic3d_CGroup *, CALL_DEF_LISTMARKERS *) = NULL;

  //if (! fp2) {
  //OSD_Function osdfp = NULL;
  //osdfp = MySharedLibrary.DlSymb ("call_togl_marker_set");
  //fp2 = (void (*)(Graphic3d_CGroup *, CALL_DEF_LISTMARKERS *)) osdfp;
  //if (! osdfp)
  //Aspect_DriverDefinitionError::Raise
  //(MySharedLibrary.DlError ());
  //}

  //static void (*fp3) (Graphic3d_CGroup *, int) = NULL;

  //if (! fp3) {
  //OSD_Function osdfp = NULL;
  //osdfp = MySharedLibrary.DlSymb ("call_togl_markercontextgroup");
  //fp3 = (void (*)(Graphic3d_CGroup *, int)) osdfp;
  //if (! osdfp)
  //Aspect_DriverDefinitionError::Raise
  //(MySharedLibrary.DlError ());
  //}

  Standard_Integer Lower  = ListVertex.Lower ();

  CALL_DEF_LISTMARKERS almarkers;

  almarkers.NbMarkers = int (ListVertex.Length ());
  almarkers.Markers = (CALL_DEF_MARKER *) &ListVertex (Lower);

  Standard_Real AMR, AMG, AMB, R, G, B;
  Aspect_TypeOfMarker AMType;
  Standard_Real AMScale, AScale;

  // Sauvegarde des valeurs initiales
  AMR = R = Standard_Real (MyCGroup.ContextMarker.Color.r);
  AMG = G = Standard_Real (MyCGroup.ContextMarker.Color.g);
  AMB = B = Standard_Real (MyCGroup.ContextMarker.Color.b);
  AMType  = Aspect_TypeOfMarker (MyCGroup.ContextMarker.MarkerType);
  AMScale = AScale = Standard_Real (MyCGroup.ContextMarker.Scale);

  int noinsert  = 0;
  Standard_Real H, L, S;
  Standard_Real LastS;
  Standard_Real Limit=0, Delta;

  switch (AMType) {

    case Aspect_TOM_POINT :
    case Aspect_TOM_PLUS :
    case Aspect_TOM_STAR :
    case Aspect_TOM_O :
    case Aspect_TOM_X :
    case Aspect_TOM_USERDEFINED:
      if (MyTraceLevel) {
        PrintFunction ("call_togl_marker_set");
        PrintCGroup (MyCGroup, 1);
      }
      call_togl_marker_set (&MyCGroup, &almarkers);
      break;

    case Aspect_TOM_O_POINT :
      MyCGroup.ContextMarker.IsDef  = 1;

      // we draw the circle
      MyCGroup.ContextMarker.MarkerType =
        int (Aspect_TOM_O);
      call_togl_markercontextgroup (&MyCGroup, noinsert);

      MyCGroup.ContextMarker.IsSet  = 1;

      if (MyTraceLevel) {
        PrintFunction ("call_togl_marker_set");
        PrintCGroup (MyCGroup, 1);
      }
      call_togl_marker_set (&MyCGroup, &almarkers);

      // we draw the point
      MyCGroup.ContextMarker.MarkerType =
        int (Aspect_TOM_POINT);
      call_togl_markercontextgroup (&MyCGroup, noinsert);
      if (MyTraceLevel) {
        PrintFunction ("call_togl_marker_set");
        PrintCGroup (MyCGroup, 1);
      }
      call_togl_marker_set (&MyCGroup, &almarkers);
      break;

    case Aspect_TOM_O_PLUS :
      MyCGroup.ContextMarker.IsDef  = 1;

      // we draw the circle
      MyCGroup.ContextMarker.MarkerType =
        int (Aspect_TOM_O);
      call_togl_markercontextgroup (&MyCGroup, noinsert);

      MyCGroup.ContextMarker.IsSet  = 1;

      if (MyTraceLevel) {
        PrintFunction ("call_togl_marker_set");
        PrintCGroup (MyCGroup, 1);
      }
      call_togl_marker_set (&MyCGroup, &almarkers);

      // we draw the plus
      MyCGroup.ContextMarker.MarkerType =
        int (Aspect_TOM_PLUS);
      call_togl_markercontextgroup (&MyCGroup, noinsert);
      if (MyTraceLevel) {
        PrintFunction ("call_togl_marker_set");
        PrintCGroup (MyCGroup, 1);
      }
      call_togl_marker_set (&MyCGroup, &almarkers);
      break;

    case Aspect_TOM_O_STAR :
      MyCGroup.ContextMarker.IsDef  = 1;

      // we draw the circle
      MyCGroup.ContextMarker.MarkerType =
        int (Aspect_TOM_O);
      call_togl_markercontextgroup (&MyCGroup, noinsert);

      MyCGroup.ContextMarker.IsSet  = 1;

      if (MyTraceLevel) {
        PrintFunction ("call_togl_marker_set");
        PrintCGroup (MyCGroup, 1);
      }
      call_togl_marker_set (&MyCGroup, &almarkers);

      // we draw the star
      MyCGroup.ContextMarker.MarkerType =
        int (Aspect_TOM_STAR);
      call_togl_markercontextgroup (&MyCGroup, noinsert);
      if (MyTraceLevel) {
        PrintFunction ("call_togl_marker_set");
        PrintCGroup (MyCGroup, 1);
      }
      call_togl_marker_set (&MyCGroup, &almarkers);
      break;

    case Aspect_TOM_O_X :
      MyCGroup.ContextMarker.IsDef  = 1;

      // we draw the circle
      MyCGroup.ContextMarker.MarkerType =
        int (Aspect_TOM_O);
      call_togl_markercontextgroup (&MyCGroup, noinsert);

      MyCGroup.ContextMarker.IsSet  = 1;

      if (MyTraceLevel) {
        PrintFunction ("call_togl_marker_set");
        PrintCGroup (MyCGroup, 1);
      }
      call_togl_marker_set (&MyCGroup, &almarkers);

      // we draw the cross
      MyCGroup.ContextMarker.MarkerType =
        int (Aspect_TOM_X);
      call_togl_markercontextgroup (&MyCGroup, noinsert);
      if (MyTraceLevel) {
        PrintFunction ("call_togl_marker_set");
        PrintCGroup (MyCGroup, 1);
      }
      call_togl_marker_set (&MyCGroup, &almarkers);
      break;

    case Aspect_TOM_BALL :
      MyCGroup.ContextMarker.IsDef  = 1;

      // we draw a set of circles
      MyCGroup.ContextMarker.MarkerType =
        int (Aspect_TOM_O);
      Delta = 0.1;
      while (AScale >= 1.0) {
        Quantity_Color::RgbHls (R, G, B, H, L, S);
        // Modification de la saturation de 5 %
        LastS = S;
        S = S - S * 0.05;
        if (S < 0.0) S = LastS;
        Quantity_Color::HlsRgb (H, L, S, R, G, B);
        MyCGroup.ContextMarker.Color.r  = float (R);
        MyCGroup.ContextMarker.Color.g  = float (G);
        MyCGroup.ContextMarker.Color.b  = float (B);
        MyCGroup.ContextMarker.Scale  = float (AScale);
        call_togl_markercontextgroup (&MyCGroup, noinsert);

        MyCGroup.ContextMarker.IsSet  = 1;

        if (MyTraceLevel) {
          PrintFunction ("call_togl_marker_set");
          PrintCGroup (MyCGroup, 1);
        }
        call_togl_marker_set (&MyCGroup, &almarkers);
        AScale  -= Delta;
      }
      break;

    case Aspect_TOM_RING1 :
    case Aspect_TOM_RING2 :
    case Aspect_TOM_RING3 :
      MyCGroup.ContextMarker.IsDef  = 1;

      // we draw a set of circles
      MyCGroup.ContextMarker.MarkerType =
        int (Aspect_TOM_O);
      if (AMType == Aspect_TOM_RING1)
        Limit = AScale - AScale * 0.8;
      if (AMType == Aspect_TOM_RING2)
        Limit = AScale - AScale * 0.5;
      if (AMType == Aspect_TOM_RING3)
        Limit = AScale - AScale * 0.2;
      Delta = 0.1;
      while (AScale > Limit && AScale >= 1.0) {
        MyCGroup.ContextMarker.Scale  = float (AScale);
        call_togl_markercontextgroup (&MyCGroup, noinsert);

        MyCGroup.ContextMarker.IsSet  = 1;

        if (MyTraceLevel) {
          PrintFunction ("call_togl_marker_set");
          PrintCGroup (MyCGroup, 1);
        }
        call_togl_marker_set (&MyCGroup, &almarkers);
        AScale  -= Delta;
      }
      break;
    default:
      break;
  }

  // Restauration des valeurs initiales
  MyCGroup.ContextMarker.Color.r  = float (AMR);
  MyCGroup.ContextMarker.Color.g  = float (AMR);
  MyCGroup.ContextMarker.Color.b  = float (AMR);
  MyCGroup.ContextMarker.MarkerType = int (AMType);
  MyCGroup.ContextMarker.Scale    = float (AMScale);

}
