//syl le 28/9/98: reactivation de 3 polices buggees + reagencement des polices
#include <V2d_DefaultMap.ixx>
#include <TColStd_Array1OfInteger.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Aspect_TypeMapEntry.hxx>
#include <Aspect_LineStyle.hxx>
#include <Aspect_WidthMapEntry.hxx>
#include <Aspect_FontMapEntry.hxx>
#include <Aspect_FontStyle.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_MarkMapEntry.hxx>
#include <Aspect_MarkerStyle.hxx>
#include <Aspect_Units.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>

static Handle(Aspect_GenericColorMap) V2dDMGCM;
static Handle(Aspect_TypeMap) V2dDMTM;
static Handle(Aspect_WidthMap) V2dDMWM;
static Handle(Aspect_FontMap) V2dDMFM;
static Handle(Aspect_MarkMap) V2dDMMM;

Handle(Aspect_GenericColorMap) V2d_DefaultMap::ColorMap () {
  if (V2dDMGCM.IsNull()) {
    Standard_Integer nbcol=12;
    TColStd_Array1OfInteger a(1,nbcol);
    a( 1) = (Standard_Integer) Quantity_NOC_WHITE;
    a( 2) = (Standard_Integer) Quantity_NOC_BLACK;
    a( 3) = (Standard_Integer) Quantity_NOC_RED;
    a( 4) = (Standard_Integer) Quantity_NOC_GREEN;
    a( 5) = (Standard_Integer) Quantity_NOC_BLUE1;
    a( 6) = (Standard_Integer) Quantity_NOC_YELLOW;
    a( 7) = (Standard_Integer) Quantity_NOC_SIENNA;
    a( 8) = (Standard_Integer) Quantity_NOC_ORANGE2;
    a( 9) = (Standard_Integer) Quantity_NOC_LIGHTGRAY;
    a(10) = (Standard_Integer) Quantity_NOC_LIGHTSEAGREEN;
    a(11) = (Standard_Integer) Quantity_NOC_GRAY50;
    a(12) = (Standard_Integer) Quantity_NOC_GRAY70;

    V2dDMGCM = new Aspect_GenericColorMap();
    Aspect_ColorMapEntry e;
    for (Standard_Integer i = 1; i <= nbcol; i++) {
//JR/Hp
      Standard_Integer iq = a(i) ;
      e.SetValue(i,Quantity_Color( (Quantity_NameOfColor) iq ));
//      e.SetValue(i,Quantity_Color((Quantity_NameOfColor) a(i)));
      V2dDMGCM->AddEntry(e);
    }
  }

  return V2dDMGCM;

}


Handle(Aspect_TypeMap) V2d_DefaultMap::TypeMap() {
  if(V2dDMTM.IsNull()) {
    V2dDMTM = new Aspect_TypeMap();
    V2dDMTM->AddEntry(Aspect_TypeMapEntry(1, Aspect_LineStyle(Aspect_TOL_SOLID)));
    V2dDMTM->AddEntry(Aspect_TypeMapEntry(2, Aspect_LineStyle(Aspect_TOL_DASH)));
    V2dDMTM->AddEntry(Aspect_TypeMapEntry(3, Aspect_LineStyle(Aspect_TOL_DOT)));
    V2dDMTM->AddEntry(Aspect_TypeMapEntry(4, Aspect_LineStyle(Aspect_TOL_DOTDASH)));
  }
  return V2dDMTM;
}

Handle(Aspect_WidthMap) V2d_DefaultMap::WidthMap() {
  if(V2dDMWM.IsNull()) {
    V2dDMWM = new Aspect_WidthMap();
    V2dDMWM->AddEntry(Aspect_WidthMapEntry(1,0.00013 METER));
    V2dDMWM->AddEntry(Aspect_WidthMapEntry(2,0.00018 METER));
    V2dDMWM->AddEntry(Aspect_WidthMapEntry(3,0.00025 METER));
    V2dDMWM->AddEntry(Aspect_WidthMapEntry(4,0.00035 METER));
    V2dDMWM->AddEntry(Aspect_WidthMapEntry(5,0.00050 METER));
    V2dDMWM->AddEntry(Aspect_WidthMapEntry(6,0.00070 METER));
    V2dDMWM->AddEntry(Aspect_WidthMapEntry(7,0.00100 METER));
    V2dDMWM->AddEntry(Aspect_WidthMapEntry(8,0.00140 METER));
  }
  return V2dDMWM;
}

Handle(Aspect_FontMap) V2d_DefaultMap::FontMap() {
  if(V2dDMFM.IsNull()) {
    V2dDMFM = new Aspect_FontMap();    

    V2dDMFM->AddEntry(Aspect_FontMapEntry(1,Aspect_FontStyle("TABTXT01",0.01 METER,0.0,Standard_True)));


    V2dDMFM->AddEntry(Aspect_FontMapEntry(2,Aspect_FontStyle("TABTXT02",0.01 METER,0.0,Standard_True)));

    V2dDMFM->AddEntry(Aspect_FontMapEntry(3,Aspect_FontStyle("TABTXT03",0.01 METER,0.0,Standard_True)));

    V2dDMFM->AddEntry(Aspect_FontMapEntry(4,Aspect_FontStyle("TABTXT04",0.01 METER,0.0,Standard_True)));    

    V2dDMFM->AddEntry(Aspect_FontMapEntry(5,Aspect_FontStyle("TABTXT05",0.01 METER,0.0,Standard_True)));

    V2dDMFM->AddEntry(Aspect_FontMapEntry(6,Aspect_FontStyle("TABTXT06",0.01 METER,0.0,Standard_True)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry(7,Aspect_FontStyle("TABTXT07",0.01 METER,0.0,Standard_True)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry(8,Aspect_FontStyle("TABTXT08",0.01 METER,0.0,Standard_True)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry(9,Aspect_FontStyle("TABTXT15",0.01 METER,0.0,Standard_True)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (10,Aspect_FontStyle("Defaultfont",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (11,Aspect_FontStyle("Symbol",0.01 METER,0.0,Standard_False)));
    
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (12,Aspect_FontStyle("Courier",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (13,Aspect_FontStyle("Courier-Bold",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (14,Aspect_FontStyle("Courier-Italic",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (15,Aspect_FontStyle("Courier-BoldItalic",0.01 METER,0.0,Standard_False)));    
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (16,Aspect_FontStyle("Courier-Oblique",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (17,Aspect_FontStyle("Helvetica",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (18,Aspect_FontStyle("Helvetica-Bold",0.01 METER,0.0,Standard_False)));
    
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (19,Aspect_FontStyle("Helvetica-Oblique",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (20,Aspect_FontStyle("Helvetica-BoldOblique",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (21,Aspect_FontStyle("Helvetica-Medium",0.01 METER,0.0,Standard_False)));   
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (22,Aspect_FontStyle("Times",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (23,Aspect_FontStyle("Times-Bold",0.01 METER,0.0,Standard_False)));   
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (24,Aspect_FontStyle("Times-Italic",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (25,Aspect_FontStyle("Times-BoldItalic",0.01 METER,0.0,Standard_False)));  
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (26,Aspect_FontStyle("Times-Roman",0.01 METER,0.0,Standard_False)));

    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (27,Aspect_FontStyle("Utopia-Bold",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (28,Aspect_FontStyle("Utopia-Italic",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (29,Aspect_FontStyle("Utopia-BoldItalic",0.01 METER,0.0,Standard_False)));
    V2dDMFM->AddEntry(Aspect_FontMapEntry
                      (30,Aspect_FontStyle("Utopia-Regular",0.01 METER,0.0,Standard_False)));   
    //V2dDMFM->AddEntry(Aspect_FontMapEntry(10,Aspect_FontStyle("-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1" ,0.003 METER,0.0,Standard_False)));

  }
  return V2dDMFM;
}

Handle(Aspect_MarkMap) V2d_DefaultMap::MarkMap() {
  if(V2dDMMM.IsNull()) {
    V2dDMMM = new Aspect_MarkMap();
    V2dDMMM->AddEntry(Aspect_MarkMapEntry( 1, Aspect_MarkerStyle(Aspect_TOM_POINT)));
    V2dDMMM->AddEntry(Aspect_MarkMapEntry( 2, Aspect_MarkerStyle(Aspect_TOM_PLUS)));
    V2dDMMM->AddEntry(Aspect_MarkMapEntry( 3, Aspect_MarkerStyle(Aspect_TOM_STAR)));
    V2dDMMM->AddEntry(Aspect_MarkMapEntry( 4, Aspect_MarkerStyle(Aspect_TOM_O)));
    V2dDMMM->AddEntry(Aspect_MarkMapEntry( 5, Aspect_MarkerStyle(Aspect_TOM_X)));
    V2dDMMM->AddEntry(Aspect_MarkMapEntry( 6, Aspect_MarkerStyle(Aspect_TOM_O_POINT)));
    V2dDMMM->AddEntry(Aspect_MarkMapEntry( 7, Aspect_MarkerStyle(Aspect_TOM_O_PLUS)));
    V2dDMMM->AddEntry(Aspect_MarkMapEntry( 8, Aspect_MarkerStyle(Aspect_TOM_O_STAR)));
    V2dDMMM->AddEntry(Aspect_MarkMapEntry( 9, Aspect_MarkerStyle(Aspect_TOM_O_X)));
    V2dDMMM->AddEntry(Aspect_MarkMapEntry(10, Aspect_MarkerStyle(Aspect_TOM_BALL)));
    V2dDMMM->AddEntry(Aspect_MarkMapEntry(11, Aspect_MarkerStyle(Aspect_TOM_RING1)));
    V2dDMMM->AddEntry(Aspect_MarkMapEntry(12, Aspect_MarkerStyle(Aspect_TOM_RING2)));
    V2dDMMM->AddEntry(Aspect_MarkMapEntry(13, Aspect_MarkerStyle(Aspect_TOM_RING3)));
    TColStd_Array1OfReal X(1,5);
    TColStd_Array1OfReal Y(1,5);
    TColStd_Array1OfBoolean B(1,5);
    X(1) = 0.;   Y(1) = 0.  ; B(1) = Standard_False;
    X(2) = 1.;   Y(2) = 0.  ; B(2) = Standard_True;
    X(3) = 0.7;  Y(3) = 0.2 ; B(3) = Standard_True;
    X(4) = 1.;   Y(4) = 0.  ; B(4) = Standard_False;
    X(5) = 0.7;  Y(5) = -0.2; B(5) = Standard_True;
    V2dDMMM->AddEntry(Aspect_MarkMapEntry(14,Aspect_MarkerStyle(X,Y,B)));

  }
  return V2dDMMM;
}
