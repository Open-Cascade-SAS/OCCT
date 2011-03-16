// Modified     10/04/98 : FMN ; Ajout include stdio.h pour WNT
#define PRO12324  // 03/07/98 : SYL ; bascule echo/pas echo pour test affichage et plotter

#define TEST_IMAGE
#define TEST_PARAGRAPH

#include <V2d.hxx>
#include <Graphic2d_GraphicObject.hxx>
#include <Graphic2d_Circle.hxx>
#include <Graphic2d_InfiniteLine.hxx>
#include <V2d_Viewer.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Graphic2d_Text.hxx>
#include <Graphic2d_View.hxx>
#include <Graphic2d_FramedText.hxx>
#include <Graphic2d_HidingText.hxx>
#include <V2d_View.hxx>
#include <Graphic2d_InfiniteLine.hxx>
#include <Graphic2d_Segment.hxx>
#include <Graphic2d_CircleMarker.hxx>
#include <Graphic2d_Marker.hxx>
#include <Graphic2d_ViewMapping.hxx>
#include <Aspect_Units.hxx>
#include <Aspect_FontMap.hxx>
#include <Aspect_FontMapEntry.hxx>
#include <Aspect_FontStyle.hxx>
#include <stdio.h>

#ifdef TEST_IMAGE
# include <OSD_Path.hxx>
# include <OSD_File.hxx>
# include <OSD_Protection.hxx>
# include <OSD_Environment.hxx>
# include <Graphic2d_ImageFile.hxx>
#endif // TEST_IMAGE

#ifdef TEST_PARAGRAPH
# include <Graphic2d_Paragraph.hxx>
# include <OSD_Environment.hxx>
#endif // TEST_PARAGRAPH

#define TRACE

static char alphabet[]="abcdefghijklmnopqrstuvwzyz {}[]|`~!@#$%^&*()-_=+;:',<.>/?ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789";
static Standard_Boolean jeveuxecho = Standard_False;
static Standard_Boolean TestImage  = Standard_False;
static Standard_Boolean TestPara   = Standard_False;

void V2d::Draw(const Handle(V2d_Viewer)& aViewer)
{
  Standard_Character txt[255];
  Handle_Graphic2d_ViewMapping aViewMapping;
  Handle(Graphic2d_GraphicObject) go = new Graphic2d_GraphicObject(aViewer->View());

#ifdef TEST_IMAGE
  OSD_Environment envImg ("CSF_V2D_TESTIMAGE");
  OSD_File File;
  if (!envImg.Value().IsEmpty ()) {
    OSD_Protection Protection (OSD_R, OSD_R, OSD_R, OSD_R);
    OSD_Path Path = OSD_Path (TCollection_AsciiString (envImg.Value ()), OSD_Default);
    File.SetPath (Path);
    File.Open (OSD_ReadOnly, Protection);
    if (File.IsOpen ()) {
      File.Close ();
      TestImage = Standard_True;
    }
  }

  if (TestImage) {
    Handle(Graphic2d_ImageFile) myImageFile;
    // Add non-zoomable image to GO
    myImageFile = new Graphic2d_ImageFile (go, File, -0.1 METER, -0.1 METER);
    myImageFile -> SetZoomable (Standard_False);
    myImageFile -> SetColorIndex (3);

    // Add zoomable image to GO
    myImageFile = new Graphic2d_ImageFile (go, File, 0.1 METER, 0.1 METER);
    myImageFile -> SetZoomable (Standard_True);

    // Add some circles
    Standard_Integer col = 1;
//    Standard_Integer i,j;
    Standard_Integer i;
    Standard_Real ray = .10 METER;
    for (i = 1; i<=20; i++) {
      Handle(Graphic2d_Circle) c = new Graphic2d_Circle(go,0. METER,0. METER,ray);
      c->SetColorIndex(col);
      if (i>5)
        c->SetTypeOfPolygonFilling(Graphic2d_TOPF_EMPTY);
      else if (i>10)
        c->SetTypeOfPolygonFilling(Graphic2d_TOPF_FILLED);
      else if (i>15)
        c->SetTypeOfPolygonFilling(Graphic2d_TOPF_PATTERNED);
      ray -= 0.005 METER  ;
      col = col < 10 ? col + 1 : 1;
    }

  }
#endif // TEST_IMAGE

#ifdef TEST_PARAGRAPH
  OSD_Environment envPara ("CSF_V2D_PZOOM");
  TestPara = (!envPara.Value ().IsEmpty());
  if (!TestImage && TestPara) {
    Handle(Graphic2d_Paragraph)  myParagraph;
    Handle(Graphic2d_HidingText) myHText;
    // Texts to add
    TCollection_ExtendedString zText   ("Zoomable Hiding Text");
    TCollection_ExtendedString nzText  ("Non-Zoomable Hiding Text");
    TCollection_ExtendedString pstr1z  ("Zoom Caracteres");
    TCollection_ExtendedString pstr1nz ("Non-Zoom Caracteres");
    TCollection_ExtendedString pstr21  ("Si vous ne goutez point ces");
    TCollection_ExtendedString pstr22  ("je m'en etonne");
    TCollection_ExtendedString pstr3   ("Mais si vous les goutez,je m'en etonne de meme");
    TCollection_ExtendedString pstr4   ("La BRUYERE");

    // Add non-zoomable paragraph in GO
    myParagraph =
      new Graphic2d_Paragraph (go, 0. METER, 0. METER, 0.0, Aspect_CP_NorthEast, 0.7);
    myParagraph -> SetFrameColorIndex   (3);
    myParagraph -> SetFrameWidthIndex   (8);
    myParagraph -> SetCurrentColorIndex (5);
    myParagraph -> SetHidingColorIndex  (9);
    myParagraph -> SetZoomable (Standard_False);

    myParagraph -> SetCurrentFontIndex (13);
    myParagraph -> SetCurrentAlignment (Graphic2d_TOA_CENTER);
    myParagraph -> SetCurrentUnderline (Standard_True);
    myParagraph -> AddText(pstr1nz,  1);

    myParagraph -> SetCurrentFontIndex (18);
    myParagraph -> SetCurrentAlignment (Graphic2d_TOA_LEFT);
    myParagraph -> SetCurrentUnderline (Standard_False);
    myParagraph -> AddText(pstr21, 2);

    myParagraph -> SetCurrentFontIndex (23);
    myParagraph -> SetCurrentAlignment (Graphic2d_TOA_RIGHT);
    myParagraph -> SetCurrentUnderline (Standard_True);
    myParagraph -> AddText(pstr22, 3);

    myParagraph -> SetCurrentFontIndex (27);
    myParagraph -> SetCurrentAlignment (Graphic2d_TOA_LEFT);
    myParagraph -> SetCurrentUnderline (Standard_False);
    myParagraph -> AddText(pstr3,  4);

    myParagraph -> SetCurrentFontIndex (2);
    myParagraph -> SetCurrentAlignment (Graphic2d_TOA_RIGHT);
    myParagraph -> SetCurrentUnderline (Standard_True);
    myParagraph -> AddText(pstr4,  5);

    // Add zoomable paragraph in GO
    myParagraph =
      new Graphic2d_Paragraph (go, 0. METER, 0. METER, 0.0, Aspect_CP_SouthWest, 3.5);
    myParagraph -> SetFrameColorIndex   (3);
    myParagraph -> SetFrameWidthIndex   (8);
    myParagraph -> SetCurrentColorIndex (5);
    myParagraph -> SetZoomable (Standard_True);

    myParagraph -> SetCurrentFontIndex (13);
    myParagraph -> SetCurrentAlignment (Graphic2d_TOA_CENTER);
    myParagraph -> SetCurrentUnderline (Standard_True);
    myParagraph -> AddText(pstr1z,  1);

    myParagraph -> SetCurrentFontIndex (18);
    myParagraph -> SetCurrentAlignment (Graphic2d_TOA_LEFT);
    myParagraph -> SetCurrentUnderline (Standard_False);
    myParagraph -> AddText(pstr21, 2);

    myParagraph -> SetCurrentFontIndex (23);
    myParagraph -> SetCurrentAlignment (Graphic2d_TOA_RIGHT);
    myParagraph -> SetCurrentUnderline (Standard_True);
    myParagraph -> AddText(pstr22, 3);

    myParagraph -> SetCurrentFontIndex (27);
    myParagraph -> SetCurrentAlignment (Graphic2d_TOA_LEFT);
    myParagraph -> SetCurrentUnderline (Standard_False);
    myParagraph -> AddText(pstr3,  4);

    myParagraph -> SetCurrentFontIndex (2);
    myParagraph -> SetCurrentAlignment (Graphic2d_TOA_RIGHT);
    myParagraph -> SetCurrentUnderline (Standard_True);
    myParagraph -> AddText(pstr4,  5);

    // Add zoomable hiding text to GO
    myHText = new Graphic2d_HidingText (go, zText,
                  -0.25 METER, 0.25 METER, 0.0, 0.5, Aspect_TOT_SOLID, 10.);
    myHText -> SetFrameWidthIndex  (8);
    myHText -> SetFrameColorIndex  (1);
    myHText -> SetHidingColorIndex (8);
    myHText -> SetColorIndex       (5);
    myHText -> SetZoomable (Standard_True);

    // Add non-zoomable hiding text to GO
    myHText = new Graphic2d_HidingText (go, nzText,
                  0.25 METER, -0.25 METER, 0.0, 0.5, Aspect_TOT_SOLID, 2.);
    myHText -> SetFrameWidthIndex  (8);
    myHText -> SetFrameColorIndex  (1);
    myHText -> SetHidingColorIndex (10);
    myHText -> SetColorIndex       (5);
    myHText -> SetZoomable (Standard_False);

    // Add some circles
    Standard_Integer col = 1;
//    Standard_Integer i,j;
    Standard_Integer i;
    Standard_Real ray = .10 METER;
    for (i = 1; i<=20; i++) {
      Handle(Graphic2d_Circle) c = new Graphic2d_Circle (go, 0. METER, 0. METER, ray);
      c -> SetColorIndex(col);
      ray -= 0.005 METER;
      col = col < 12 ? col + 1 : 1;
    }

    // Add some markers
    Handle(Graphic2d_Marker) M;
    Standard_ShortReal pos = -0.15 METER;
    for (i = 1; i <= 12; i++) {
      M = new Graphic2d_Marker(go, i, pos, pos, 0.005 METER,0.005 METER);
      M -> SetColorIndex (i);
      pos -= 0.02 METER;
    }
  }
#endif // TEST_PARAGRAPH

  if (!TestImage && !TestPara) {
#ifdef PRO12324 
    // couleur de l' echo                              
    Handle(Graphic2d_View) myGraphicView = aViewer->View();
    myGraphicView->SetDefaultOverrideColor((Standard_Integer) 1); 
#endif
    Handle(Aspect_FontMap) theFontMap = aViewer->FontMap();
    Standard_Real ray;
    Standard_Real post;
    Standard_Real slant = 0.5;
    Standard_Integer col = 1;
    Standard_Integer typ ;
    Standard_Integer wid ;
    Standard_Integer i,j;
    Handle(Graphic2d_Text) T;
    Handle(Graphic2d_FramedText) TF;
    Handle(Graphic2d_HidingText) TH;
    Handle(Graphic2d_Segment) s; 
     // Circles 
#ifdef TRACE
    cout << "Circle " << endl;
#endif
    ray = .10 METER;
    for ( i = 1; i<=20; i++) {
      // cout << "ray=" << ray << " col=" << col << endl;
      Handle(Graphic2d_Circle) c = new Graphic2d_Circle(go,0. METER,0. METER,ray);
      c->SetColorIndex(col);
      if(i>5) 
        c->SetTypeOfPolygonFilling(Graphic2d_TOPF_EMPTY);
      else if (i>10)
        c->SetTypeOfPolygonFilling(Graphic2d_TOPF_FILLED);
      else if (i>15)
        c->SetTypeOfPolygonFilling(Graphic2d_TOPF_PATTERNED);
      ray -= 0.005 METER  ;
      col = col < 10 ? col + 1 : 1;
    }

    // Lines
    Standard_Real pos;
    col = 1;
    typ = 1;
    wid = 1;
    pos = 0.1 METER ;
#ifdef TRACE
    cout << "Lines " << endl;
#endif
    for ( i = 1; i<=8; i++) {
    //   Handle(Graphic2d_InfiniteLine) l = new Graphic2d_InfiniteLine(go, 0.0 METER, pos , pos , pos);
    //   l->SetColorIndex(col);   

     s = new Graphic2d_Segment(go, -0.6 METER, pos , -0.25 METER , pos);
     s->SetColorIndex(col);   
     s->SetTypeIndex(typ);
     s->SetWidthIndex(wid);
     post = pos;
     for ( j = 0; j<10; j++) {
       s = new Graphic2d_Segment(go, -0.25 METER, post , -0.20 METER , post);
       s->SetColorIndex(col);  
       s->SetWidthIndex(1);
       post -= 0.002 METER;
     }
     pos -=  0.02 METER ;
     col = col < 8 ? col + 1 : 1;
     typ  = typ < 4 ? typ + 1 : 1;
     wid = wid < 8 ? wid + 1 : 1;
    }  

    // Markers
#ifdef TRACE
    cout << "Markers " << endl;
#endif
    Handle(Graphic2d_Marker) M ; 
    pos = 0.2 METER;
    for ( i = 1; i<=13; i++) {  
     M = new Graphic2d_Marker(go,i,
                              -.25 METER ,pos,
                              0.005 METER,0.005 METER);
     M->SetColorIndex(2); 
     pos -= 0.02 METER;
    }

    //Handle(TCollection_ExtendedString) txt = new TCollection_ExtendedString(alphabet);

    // Texts
#ifdef TRACE
    cout << "Texts " << endl;
#endif
    Standard_Integer nb = 30;
    col = 1;
    pos = 0.60 METER;
    post = (0.05 + nb * 0.02) METER;
    // traits verticaux tous les 10 mm
    Standard_Real posh;
    for ( i = 1; i<=40; i++) {
     posh = (-0.15 + (i-1) * 0.01) METER;
     s = new Graphic2d_Segment(go, posh , pos , posh , pos-post);
     s->SetColorIndex(2);   
     s->SetTypeIndex(1);
     s->SetWidthIndex(1); 
    }

    Aspect_FontMapEntry entry;
    TCollection_AsciiString aname;
    Aspect_FontStyle style;
    Standard_Integer iindex;
    nb = theFontMap->Size();
    for ( i = 1; i<=nb; i++) {  
     // FontName
     entry = theFontMap->Entry(i) ;   
     iindex = entry.Index() ;
     style = entry.Type();
     aname = style.AliasName();   
     sprintf(txt,"Text Pol %d:%s:%s",iindex,aname.ToCString(),alphabet);
     T = new Graphic2d_Text(go,
                            TCollection_ExtendedString(txt),
                            -0.15 METER, pos, 0.);
     T->SetFontIndex(iindex);
     T->SetColorIndex(col);
     // Trait horizontal 
     s = new Graphic2d_Segment(go, -0.2 METER, pos , 0.5 METER , pos);
     s->SetColorIndex(2);   
     s->SetTypeIndex(1);
     s->SetWidthIndex(1);

     sprintf(txt,"Hiding Text Pol %d:%s",iindex,alphabet);
     TH = new Graphic2d_HidingText(go,
                                   TCollection_ExtendedString(txt),
                                   -0.15 METER, pos - post, 
                                   0. , 0.1 ,Aspect_TOT_SOLID);
     TH->SetFontIndex(iindex);
     TH->SetColorIndex(col);     
     if( i>8 && i<15 ){
       TH->SetFrameColorIndex(col);
     }
     // Frame + Underline + Slant 
     sprintf(txt,"FramedText Pol %d",iindex);
     TF = new Graphic2d_FramedText(go,
                                   TCollection_ExtendedString(txt),
                                   -0.6 METER, pos , 
                                   slant , 0.1 ,Aspect_TOT_SOLID);
     TF->SetFontIndex(iindex);
     TF->SetUnderline(Standard_True);
     TF->SetColorIndex(col);    

     M = new Graphic2d_Marker(go,2,
                              -0.6 METER ,pos,
                              0.005 METER,0.005 METER);
     M->SetColorIndex(2);

     pos -= 0.02 METER;
     col = col < 8 ? col + 1 : 1;
    }
    i=1;
    sprintf(txt,"Texte non zoom Pol %d",i);
    T = new Graphic2d_Text(go,
                          TCollection_ExtendedString(txt),
                          -0.3 METER, -0.22 METER , 
                          0. METER);
    T->SetColorIndex(2);
    T->SetFontIndex(i);
    T->SetZoomable(Standard_False);
  }

#ifdef PRO12324 
 // bascule echo/pas echo
 if(jeveuxecho){
   cout << "---- V2d ------ Highlight " << endl ;
   go->Highlight( (Standard_Integer) 1);
   jeveuxecho = Standard_False;  
 }else{
   cout << "---- V2d ------ Display" << endl ;
   go->Display();  
   jeveuxecho = Standard_True;  
 }
#else
 go->Display();  
#endif

 aViewer->Update();
}
