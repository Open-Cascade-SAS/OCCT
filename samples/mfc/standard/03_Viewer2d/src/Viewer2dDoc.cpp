// Viewer2dDoc.cpp : implementation of the CViewer2dDoc class
//

#include "stdafx.h"

#include "Viewer2dDoc.h"

#include "OCC_BaseApp.h"

#include <Aspect_ColorMap.hxx>
#include <Aspect_MarkMapEntry.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_GenericColorMap.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>

#include <BRep_Builder.hxx>

#include <BRepTools.hxx>

#include "Primitive\Sample2D_Markers.h"
#include "Primitive\Sample2D_Line.h"
#include "Primitive\Sample2D_Text.h"
#include "Primitive\Sample2D_Face.h"
#include "Primitive\Sample2D_Curve.h"
#include "Primitive\Sample2D_Rectangle.h"
#include "Primitive\Sample2D_Image.h"

#include "PropertiesSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CViewer2dDoc

IMPLEMENT_DYNCREATE(CViewer2dDoc, CDocument)

BEGIN_MESSAGE_MAP(CViewer2dDoc, CDocument)
	//{{AFX_MSG_MAP(CViewer2dDoc)
	ON_COMMAND(ID_MENU_CASCADE_PROPERTIES, OnBUTTONTest2DProperties)
	ON_COMMAND(ID_BUTTON_Test_Text, OnBUTTONTestText)
 	ON_COMMAND(ID_BUTTON_Test_Markers, OnBUTTONTestMarkers)
	ON_COMMAND(ID_BUTTON_Test_Line, OnBUTTONTestLine)
	ON_COMMAND(ID_BUTTON_Erase, OnBUTTONErase)
	ON_COMMAND(ID_BUTTON_Test_Face, OnBUTTONTestFase)
	ON_COMMAND(ID_BUTTON_Test_Rect, OnBUTTONTestRect)
	ON_COMMAND(ID_BUTTON_Test_Curve, OnBUTTONTestCurve)
	ON_COMMAND(ID_BUTTON32793, OnBUTTONTestImage) // test image
	ON_COMMAND(ID_BUTTON_Test_MultiImages, OnBUTTONMultipleImage)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewer2dDoc construction/destruction

CViewer2dDoc::CViewer2dDoc()
{
}

CViewer2dDoc::~CViewer2dDoc()
{
}

#ifdef _DEBUG
void CViewer2dDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CViewer2dDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewer2dDoc commands

void CViewer2dDoc::OnBUTTONTest2DProperties() 
{
// TODO: Add your command handler code here
  CPropertiesSheet aDial;
  aDial.SetViewer(my2DViewer);
  aDial.DoModal();
}

//=================================================================
//=================================================================
//=================================================================

void CViewer2dDoc::OnBUTTONErase() 
{
   myAISInteractiveContext2D->EraseAll();
  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON_Erase;
  TCollection_AsciiString Message("\
call :    \n\
   myAISInteractiveContext2D->EraseAll();");

  // Update The Result Dialog
  UpdateResultDialog(anID,Message);
}



void CViewer2dDoc::OnBUTTONTestText() 
{
 Standard_Integer color=1;
 Standard_Real j;
 for (j=15;j<=20;j++)
  {
    TCollection_AsciiString Text("font 0 scale ");Text+=j/20;
    Handle (Sample2D_Text) aText  = 
        new Sample2D_Text(Text,
                       0,15*(j-15),      //  anX  , anY 
                       Aspect_TOT_SOLID,//  aType  
                       20.0*PI180 ,            //  anAngle  
                       0.0 ,            //  aSlant  
                       color++,               //  aColorIndex  
                       0,               //  aFontIndex  
                       j/20,                //  aScale 
                       Standard_False,  // aIsUnderline
                       Standard_False,   // aIsZoomable
                       Graphic2d_TOA_LEFT); // aTypeOfAlignment
    myAISInteractiveContext2D->Display(aText,            // object
                                       Standard_False);  // Redraw
    if (color == my2DViewer->ColorMap()->Size()) color =1;
  }

  for (int j=5;j<=10;j+=1)
  {
    TCollection_AsciiString Text("font 1 scale ");Text+=j/10;
    Handle (Sample2D_Text) aText  = 
        new Sample2D_Text(Text,
                       80,15*(j-5),  //  anX  , anY 
                       Aspect_TOT_SOLID,//  aType  
                       0.0 ,            //  anAngle  
                       0.0 ,            //  aSlant  
                       color++,         //  aColorIndex  
                       1,               //  aFontIndex  
                       j/10,                //  aScale 
                       Standard_True,  // aIsUnderline
                       Standard_True,   // aIsZoomable
                       Graphic2d_TOA_CENTER); // aTypeOfAlignment

    myAISInteractiveContext2D->Display(aText,            // object
                                       Standard_False);  // Redraw
    if (color == my2DViewer->ColorMap()->Size()) color =1;
  }
    
  for (j=5;j<=10;j+=1)
  {
    TCollection_AsciiString Text("font 2 scale ");Text+=j/10;
    Handle (Sample2D_Text) aText  = 
        new Sample2D_Text(Text,
                       140,15*(j-5),  //  anX  , anY 
                       Aspect_TOT_SOLID,//  aType  
                       0.0 ,            //  anAngle  
                       0.0 ,            //  aSlant  
                       color++,         //  aColorIndex  
                       2,               //  aFontIndex  
                       j/10,               //  aScale 
                       Standard_False,  // aIsUnderline
                       Standard_True,   // aIsZoomable
                       Graphic2d_TOA_TOPLEFT); // aTypeOfAlignment
    myAISInteractiveContext2D->Display(aText,            // object
                                       Standard_False);  // Redraw
    if (color == my2DViewer->ColorMap()->Size()) color =1;
  }
    
  for (j=10;j<=15;j+=1)
  {
    TCollection_AsciiString Text("font 3 scale ");Text+=j/10;
    Handle (Sample2D_Text) aText  = 
        new Sample2D_Text(Text,
                       260,15*(j-10),      //  anX  , anY 
                       Aspect_TOT_SOLID,//  aType  
                       0 ,            //  anAngle  
                       0.0 ,            //  aSlant  
                       color++,               //  aColorIndex  
                       3,               //  aFontIndex  
                       j/10,                //  aScale 
                       Standard_False,  // aIsUnderline
                       Standard_False,   // aIsZoomable
                       Graphic2d_TOA_LEFT); // aTypeOfAlignment
    myAISInteractiveContext2D->Display(aText,            // object
                                       Standard_False);  // Redraw
    if (color == my2DViewer->ColorMap()->Size()) color =1;
  }

  FitAll2DViews(Standard_True); // Update Viewer
  // Update Get information to update Result dialog
  UINT anID=ID_BUTTON_Test_Text;
  TCollection_AsciiString Message("  \
create a set of Sample2D_Text,display it and declare selectable :    \n\
------------------------------------------------------------------------------ \n\
    Handle (Sample2D_Text) aText;                          \n\
    // aText = new ...                                            \n\
   myAISInteractiveContext2D->Display(aText,            // object     \n\
                                      Standard_False);  // Redraw       \n\
------------------------------------------------------------------------------ \n\
   after that the system call the SetContext Method for the display,\n\
------------------------------------------------------------------------------ \n\
                                                                  \n\
void Sample2D_Text::SetContext(const Handle(AIS2D_InteractiveContext)& theContext) \n\
{ \n\
  AIS2D_InteractiveObject::SetContext(theContext); \n\
\n\
  Handle(Graphic2d_Text) text; \n\
  text = new Graphic2d_Text(this, myText, myX, myY, myAngle,myTypeOfText,myScale); \n\
  text->SetFontIndex  (myFontIndex); \n\
  text->SetColorIndex (myColorIndex); \n\
  text->SetSlant      (mySlant); \n\
  text->SetUnderline  (myIsUnderline); \n\
  text->SetZoomable   (myIsZoomable); \n\
  text->SetAlignment  (myTypeOfAlignment); \n\
}");

  // Update The Result Dialog
  UpdateResultDialog(anID,Message);
}


void CViewer2dDoc::OnBUTTONTestMarkers() 
{
    {    // generic Markers
      Standard_Integer MaxIndex = my2DViewer->MarkMap()->Size();
      
	  for (int i=1;i<=MaxIndex;i++)
      {
        Standard_Integer EntryNumber =  (my2DViewer->MarkMap()->Entry(i)).Index () ;

        Handle (Sample2D_Markers) Marker  = 
          new Sample2D_Markers(EntryNumber, // generic marker index
                               10*i,0,// display point
                               5,5, // size
                               0); // angle in radian
        myAISInteractiveContext2D->Display(Marker,            // object
                                           Standard_False);  // Redraw

      }
      for (int i=1;i<=MaxIndex;i++)
      {
        Standard_Integer EntryNumber =  (my2DViewer->MarkMap()->Entry(i)).Index () ;
        Handle (Sample2D_Markers) Marker  = 
          new Sample2D_Markers(EntryNumber, // generic marker index
                               10*i,10,// display point
                               5,5, // size
                               20*PI180); // angle in radian
        myAISInteractiveContext2D->Display(Marker,            // object
                                           Standard_False);  // Redraw
      }
    }

    
    {  // a Circle Marker 
      Handle (Sample2D_Markers) Marker  = 
        new Sample2D_Markers(10,20,// reference point
                             0,0, // display point
                             2); // radius
      myAISInteractiveContext2D->Display(Marker,            // object
                                         Standard_False);  // Redraw

    }
    
    {  // an ellips Marker 
      Handle (Sample2D_Markers) Marker  = 
        new Sample2D_Markers(20,20, // reference point
                             0,0,  // display point
                             3,2,    // major, minor radius
                             20*PI180); // angle
      myAISInteractiveContext2D->Display(Marker,            // object
                                         Standard_False);  // Redraw
    }

    {  // a polyline  Marker 

      Graphic2d_Array1OfVertex myListVertex(1,6);
      myListVertex(1)=Graphic2d_Vertex(0.,0.);
      myListVertex(2)=Graphic2d_Vertex(0.,5.);
      myListVertex(3)=Graphic2d_Vertex(10.,0.);
      myListVertex(4)=Graphic2d_Vertex(10.,5.);
      myListVertex(5)=Graphic2d_Vertex(20.,0.);
      myListVertex(6)=Graphic2d_Vertex(20.,5.);

      Handle (Sample2D_Markers) Marker  = 
        new Sample2D_Markers(30,20, // reference point
                             myListVertex); 
      myAISInteractiveContext2D->Display(Marker,            // object
                                         Standard_True);  // Redraw

    }

  FitAll2DViews(Standard_False); // Update Viewer
  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON_Test_Markers;
  TCollection_AsciiString Message(" \
create a set of Sample2D_Markers and display it :\n\
---------------------------------------------------\n\
    {\n\
      Standard_Integer MaxIndex = my2DViewer->MarkMap()->Size();\n\
      for (int i=1;i<=MaxIndex;i++) {\n\
        Standard_Integer EntryNumber =  (my2DViewer->MarkMap()->Entry(10)).Index () ;\n\
        Handle (Sample2D_Markers) Marker  = \n\
          new Sample2D_Markers(EntryNumber, // generic marker index\n\
                               10*i,0,// display point\n\
                               5,5, // size\n\
                               0); // angle in radian\n\
        myAISInteractiveContext2D->Display(Marker,            // object\n\
                                           Standard_False);  // Redraw\n\
\n\
    }\n\
  //...\n\
\n\
   after that the system call the SetContext Methods :                    \n\
\n\
void Sample2D_Markers::SetContext(const Handle(AIS2D_InteractiveContext)& theContext)  \n\
{ \n\
 AIS2D_InteractiveObject::SetContext(theContext); \n\
 if (myCurrentTypeOfMarker == 2D_CTOM_Generic) \n\
   if ( myIndex == 0 ) {\n\
     Handle(Graphic2d_Marker) aMarker=  \n\
        new Graphic2d_Marker(this, \n\
        myXPosition ,myYPosition); \n\
   }\n\
   else {\n\
     Handle(Graphic2d_Marker) aMarker=  \n\
        new Graphic2d_Marker(this,myIndex, \n\
        myXPosition ,myYPosition, \n\
        myWidth,myHeight,myAngle); \n\
   }\n\
 \n\
 if (myCurrentTypeOfMarker == 2D_CTOM_Polyline) \n\
   Handle(Graphic2d_PolylineMarker) aMarker=  \n\
      new Graphic2d_PolylineMarker(this, \n\
      myXPosition,myYPosition, \n\
      myListVertex); \n\
 \n\
 if (myCurrentTypeOfMarker == 2D_CTOM_Circle) \n\
   Handle(Graphic2d_CircleMarker) aMarker=  \n\
      new Graphic2d_CircleMarker(this, \n\
      myXPosition,myYPosition, \n\
      myX ,myY,myRadius  ); \n\
 \n\
 if (myCurrentTypeOfMarker == 2D_CTOM_Ellips) \n\
   Handle(Graphic2d_EllipsMarker) aMarker=  \n\
      new Graphic2d_EllipsMarker(this, \n\
      myXPosition,myYPosition, \n\
      myX,myY, \n\
      myMajorRadius,myMinorRadius, \n\
      myAngle ); \n\
 \n\
} \n\n");
  // Update The Result Dialog
  UpdateResultDialog(anID,Message);
}


void CViewer2dDoc::OnBUTTONTestLine() 
{
  //Handle(ISessionSample2D_Line) aLine = new ISessionSample2D_Line();
  Handle(Sample2D_Line) aLine = new Sample2D_Line();

//  myInteractiveContext2D->Display(aLine,           // object
//                                Standard_True);  // Redraw

  myAISInteractiveContext2D->Display(aLine,           // object
                                     Standard_True);  // Redraw

  FitAll2DViews(Standard_False); // Update Viewer
  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON_Test_Line;
  TCollection_AsciiString Message(" \
create a Sample2D_Line and display it :    \n\
------------------------------------------------------------------------------ \n\
  Handle(Sample2D_Line) aLine = new Sample2D_Line();\n\
  myAISInteractiveContext2D->Display(aLine,           // object\n\
                                     Standard_True);  // Redraw\n\
  FitAll2DViews(Standard_False); // Update Viewer\n\
------------------------------------------------------------------------------ \n\
   after that the system call the SetContext Methods :                    \n\
------------------------------------------------------------------------------ \n\
void Sample2D_Line::SetContext(const Handle(AIS2D_InteractiveContext)& theContext) \n\
{\n\
  AIS2D_InteractiveObject::SetContext(theContext); \n\
\n\
  Handle(Prs2d_AspectLine) aLineAspect = new Prs2d_AspectLine;  \n\
  aLineAspect->SetTypeOfFill(Graphic2d_TOPF_FILLED); \n\
  aLineAspect->SetWidthIndex(0); \n\
\n\
 for (int i=0;i<=13;i++)\n\
  for (int j=0;j<=5;j++)\n\
  {\n\
   {\n\
    Graphic2d_Array1OfVertex anArrayVertex(1,5);\n\
    anArrayVertex(1)=Graphic2d_Vertex(20.*i,10.*j+0);\n\
    anArrayVertex(2)=Graphic2d_Vertex(20.*i+10,10.*j+0);\n\
    anArrayVertex(3)=Graphic2d_Vertex(20.*i+10,10.*j+5);\n\
    anArrayVertex(4)=Graphic2d_Vertex(20.*i,10.*j+5);\n\
    anArrayVertex(5)=Graphic2d_Vertex(20.*i,10.*j+0);\n\
    Handle(Graphic2d_Polyline) aPolyline = \n\
        new Graphic2d_Polyline(this,anArrayVertex);\n\
\n\
    SetAspect(aLineAspect, aPolyline); \n\
\n\
    aPolyline->SetInteriorColorIndex(i); // to use with Graphic2d_TOPF_FILLED \n\
    aPolyline->SetTypeIndex(j);          // type of the polyline \n\
   }\n\
 }\n}");

  // Update The Result Dialog
  UpdateResultDialog(anID,Message);
}

void CViewer2dDoc::OnBUTTONTestFase()
{
  myAISInteractiveContext2D->EraseAll();
  CFileDialog dlg(TRUE,
                  NULL,
                  NULL,
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  "BRep Files (*.brep)|*.brep; ||", 
                  NULL );

  CString initdir(((OCC_BaseApp*) AfxGetApp())->GetInitDataDir());
  initdir += "\\Data";

  dlg.m_ofn.lpstrInitialDir = initdir;

  if (dlg.DoModal() == IDOK) 
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    CString filename = dlg.GetPathName();
    Standard_CString aFileName = (Standard_CString)(LPCTSTR)filename;

    TopoDS_Shape aFace;
	BRep_Builder aBuilder;
	Standard_Boolean result = BRepTools::Read(aFace,aFileName,aBuilder);

	if(aFace.IsNull() || aFace.ShapeType() != TopAbs_FACE) {
		AfxMessageBox("The shape must be not a null Face");
		return;
	}

//	Handle(ISessionSample2D_Face) aDisplayableFace = new ISessionSample2D_Face( TopoDS::Face(aFace)  );
	Handle(Sample2D_Face) aDisplayableFace = new Sample2D_Face( TopoDS::Face(aFace)  );

    myAISInteractiveContext2D->Display(aDisplayableFace,  // object
                                       Standard_True);     // Redraw

    Handle(Aspect_ColorMap) aColorMap = my2DViewer->ColorMap();
    Aspect_ColorMapEntry aFORWARDColorMapEntry  = aColorMap->FindEntry(aDisplayableFace->FORWARDColorIndex());
    Aspect_ColorMapEntry aREVERSEDColorMapEntry = aColorMap->FindEntry(aDisplayableFace->REVERSEDColorIndex());
    Aspect_ColorMapEntry aINTERNALColorMapEntry = aColorMap->FindEntry(aDisplayableFace->INTERNALColorIndex());
    Aspect_ColorMapEntry aEXTERNALColorMapEntry = aColorMap->FindEntry(aDisplayableFace->EXTERNALColorIndex());

    Quantity_Color TheFORWARDColor  =  aFORWARDColorMapEntry.Color();
    Quantity_Color TheREVERSEDColor =  aREVERSEDColorMapEntry.Color();
    Quantity_Color TheINTERNALColor =  aINTERNALColorMapEntry.Color();
    Quantity_Color TheEXTERNALColor =  aEXTERNALColorMapEntry.Color();

    Standard_CString FORWARDStringName (Quantity_Color::StringName(TheFORWARDColor.Name()));
    Standard_CString REVERSEDStringName(Quantity_Color::StringName(TheREVERSEDColor.Name()));
    Standard_CString INTERNALStringName(Quantity_Color::StringName(TheINTERNALColor.Name()));
    Standard_CString EXTERNALStringName(Quantity_Color::StringName(TheEXTERNALColor.Name()));
    FitAll2DViews(Standard_False); // Update Viewer
  // Update Get information to update Result dialog

    UINT anID=ID_BUTTON_Test_Face;
    TCollection_AsciiString Message("Forward is \t");
    Message +=  FORWARDStringName;
    Message += "\nReversed is \t";
    Message +=  REVERSEDStringName;
    Message += "\nInternal is \t";
    Message +=  INTERNALStringName;
    Message += "\nExternal is \t";
    Message +=  EXTERNALStringName;
    Message += "\n";
    Message += "\
------------------------------------------------------------------------------ \n\
create an Sample2D_Face and display it     \n\
------------------------------------------------------------------------------ \n\
//...\n\
	TopoDS_Face aFace;\n\
	BRep_Builder aBuilder;\n\
	Standard_Boolean result = BRepTools::Read(aFace,aFileName,aBuilder);\n\
\n\
	Handle(ISessionSample2D_Face) aDisplayableFace = new ISessionSample2D_Face( aFace  );\n\
	myInteractiveContext2D->Display(aDisplayableFace,  // object\n\
									Standard_True);     // Redraw\n\
//...\n\
------------------------------------------------------------------------------ \n\
void _Face::SetContext() \n\
{\n\
    Handle(Prs2d_AspectLine) aLineAspect_FORWARD = new Prs2d_AspectLine;  \n\
    aLineAspect_FORWARD->SetTypeOfFill(Graphic2d_TOPF_EMPTY);\n\
	aLineAspect_FORWARD->SetTypeIndex(myTypeIndex); \n\
    aLineAspect_FORWARD->SetWidthIndex(myWidthIndex);\n\
    aLineAspect_FORWARD->SetColorIndex(myFORWARDColorIndex);\n\
\n\
    Handle(Prs2d_AspectLine) aLineAspect_REVERSED = new Prs2d_AspectLine;  \n\
    aLineAspect_REVERSED->SetTypeOfFill(Graphic2d_TOPF_EMPTY);\n\
    aLineAspect_REVERSED->SetTypeIndex(myTypeIndex);\n\
    aLineAspect_REVERSED->SetWidthIndex(myWidthIndex);\n\
    aLineAspect_REVERSED->SetColorIndex(myREVERSEDColorIndex);\n\
\n\
    Handle(Prs2d_AspectLine) aLineAspect_INTERNAL = new Prs2d_AspectLine;  \n\
    aLineAspect_INTERNAL->SetTypeOfFill(Graphic2d_TOPF_EMPTY);\n\
    aLineAspect_INTERNAL->SetTypeIndex(myTypeIndex);\n\
    aLineAspect_INTERNAL->SetWidthIndex(myWidthIndex);\n\
    aLineAspect_INTERNAL->SetColorIndex(myINTERNALColorIndex);\n\
\n\
    Handle(Prs2d_AspectLine) aLineAspect_EXTERNAL = new Prs2d_AspectLine;  \n\
    aLineAspect_EXTERNAL->SetTypeOfFill(Graphic2d_TOPF_EMPTY);\n\
    aLineAspect_EXTERNAL->SetTypeIndex(myTypeIndex);\n\
    aLineAspect_EXTERNAL->SetWidthIndex(myWidthIndex);\n\
    aLineAspect_EXTERNAL->SetColorIndex(myEXTERNALColorIndex);";

Message +="\
\n\
//pcurves of the face \n\
\n\
    Handle(Graphic2d_SetOfCurves) segment_FORWARD;\n\
    segment_FORWARD = new Graphic2d_SetOfCurves(this); \n\
\n\
    Handle(Graphic2d_SetOfCurves) segment_REVERSED;\n\
    segment_REVERSED = new Graphic2d_SetOfCurves(this); \n\
\n\
    Handle(Graphic2d_SetOfCurves) segment_INTERNAL;\n\
    segment_INTERNAL = new Graphic2d_SetOfCurves(this); \n\
\n\
    Handle(Graphic2d_SetOfCurves) segment_EXTERNAL;\n\
    segment_EXTERNAL = new Graphic2d_SetOfCurves(this); \n\
\n\
    Standard_Real f,l;\n\
    Standard_Integer i = 1;\n\
    myFace.Orientation(TopAbs_FORWARD);\n\
    TopExp_Explorer ex(myFace,TopAbs_EDGE);\n\
    while (ex.More()) {\n\
      const Handle(Geom2d_Curve) Curve = BRep_Tool::CurveOnSurface\n\
	         (TopoDS::Edge(ex.Current()),TopoDS::Face(myFace),f,l);\n\
\n\
      Handle(Geom2d_TrimmedCurve) c = new Geom2d_TrimmedCurve(Curve,f,l);\n\
";
Message += "\
      if (c.IsNull())\n\
      {\n\
        // null curve --> EDGE have a null lenght\n\
        // --> get first and last Vertices\n\
        TopoDS_Edge CurrentEdge= TopoDS::Edge(ex.Current());\n\
\n\
        TopoDS_Vertex FirstVertex = TopExp::FirstVertex(CurrentEdge);\n\
        TopoDS_Vertex LastVertex  = TopExp::LastVertex(CurrentEdge);\n\
\n\
        if (FirstVertex != LastVertex)\n\
          Standard_Failure::Raise(\"Null Edge pcurve But different vertices\");\n\
\n\
         gp_Pnt aPnt  = BRep_Tool::Pnt(FirstVertex);\n\
\n\
        Handle(Graphic2d_Marker) aMarker= \n\
           new Graphic2d_Marker(this,myMarkerIndex,\n\
                                aPnt.X() ,aPnt.Y(),\n\
                                myMarkerWidth,myMarkerHeight,0);\n\
\n\
";
Message += "\
        switch (ex.Current().Orientation())\n\
        {\n\
          case TopAbs_FORWARD:  aMarker->SetColorIndex (myFORWARDColorIndex);   break;\n\
          case TopAbs_REVERSED: aMarker->SetColorIndex (myREVERSEDColorIndex);  break;\n\
          case TopAbs_INTERNAL: aMarker->SetColorIndex (myINTERNALColorIndex);  break;\n\
          case TopAbs_EXTERNAL: aMarker->SetColorIndex (myEXTERNALColorIndex);  break;\n\
          default : break;\n\
        }\n\
        ex.Next();\n\
      }\n\
      else\n\
      {\n\
        // Display the Curve\n\
        // display the orientation\n\
\n\
        gp_Pnt2d p1,p2;\n\
        gp_Vec2d v;\n\
        c->D1(l,p1,v);\n\
        Graphic2d_Array1OfVertex aListVertex(1,3);\n\
        if (v.Magnitude() > gp::Resolution()) {\n\
            Standard_Real L = 2;\n\
            Standard_Real H = 1;\n\
            gp_Dir2d d(v);\n\
            p2.SetCoord(- L*d.X() - H*d.Y(),- L*d.Y() + H*d.X());\n\
            aListVertex(1)=Graphic2d_Vertex(p2.X(),p2.Y());\n\
            \n\
            p2.SetCoord(- L*d.X() + H*d.Y(),- L*d.Y() - H*d.X());\n\
            aListVertex(2)=Graphic2d_Vertex(0.,0.);\n\
            aListVertex(3)=Graphic2d_Vertex(p2.X(),p2.Y()); \n\
        }\n\
";
Message += "\
\n\
        Handle(Graphic2d_PolylineMarker) aMarker= \n\
          new Graphic2d_PolylineMarker(this,p1.X(),p1.Y(),aListVertex);\n\
\n\
  		switch (ex.Current().Orientation()) \n\
        {\n\
        case TopAbs_FORWARD: {\n\
             segment_FORWARD->Add(c); \n\
             SetAspect(aLineAspect_FORWARD, aMarker);  \n\
             break;\n\
        }\n\
        case TopAbs_REVERSED: {\n\
             segment_REVERSED->Add(c); \n\
             SetAspect(aLineAspect_REVERSED, aMarker); \n\
             break; \n\
        } \n\
        case TopAbs_INTERNAL: { \n\
             segment_INTERNAL->Add(c); \n\
             SetAspect(aLineAspect_INTERNAL, aMarker); \n\
             break; \n\
        } \n\
        case TopAbs_EXTERNAL: { \n\
             segment_EXTERNAL->Add(c); \n\
             SetAspect(aLineAspect_EXTERNAL, aMarker); \n\
             break; \n\
        } \n\
        default : break; \n\
        } \n\
      }\n\
      ex.Next();\n\
    }\n\
\n\
	SetAspect(aLineAspect_FORWARD, segment_FORWARD);\n\
	SetAspect(aLineAspect_REVERSED, segment_REVERSED);\n\
	SetAspect(aLineAspect_INTERNAL, segment_INTERNAL);\n\
	SetAspect(aLineAspect_EXTERNAL, segment_EXTERNAL);\n\
\n\
}\n\
";

  // Update The Result Dialog
  UpdateResultDialog(anID,Message);
  }

}

void CViewer2dDoc::OnBUTTONTestRect()
{
   Handle (Sample2D_Rectangle) aRectangle = new Sample2D_Rectangle(
	                                         0,0,       // display point
                                             50,80,     // size
                                             20*PI180); // angle in radian

   aRectangle->SetHighlightMode(AIS2D_TOD_OBJECT);
   myAISInteractiveContext2D->Display(aRectangle, Standard_False);  // Display rectangle
  

 
   Handle (Sample2D_Rectangle) aRectangle2 = new Sample2D_Rectangle(
	                                       80,0,       // display point
                                           50,90,     // size
                                           20*PI180); // angle in radian

   aRectangle2->SetHighlightMode(AIS2D_TOD_ELEMENT);
   myAISInteractiveContext2D->Display(aRectangle2);  // Display rectangle
  

   FitAll2DViews(Standard_False); // Update Viewer

  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON_Test_Rect;
  TCollection_AsciiString Message(" \
create a two of Sample2D_Rectangle and display it :    \n\
------------------------------------------------------------------------------ \n\
  {                                                                    \n\
   Handle (Sample2D_Rectangle) aRectangle  =                   \n\
        new Sample2D_Rectangle(0,0,       // display point \n\
                                             50,80,     // size              \n\
                                             20*PI180); // angle in radian  \n\
\n\
   aRectangle->SetHighlightMode(AIS2D_TOD_OBJECT); \n\
   myASIInteractiveContext2D->Display(aRectangle, Standard_False);  // Display rectangle  \n\
  }                                                              \n\
  {                                                                 \n\
  Handle (Sample2D_Rectangle) aRectangle  =  \n\
        new Sample2D_Rectangle(80,0,       // display point \n\
                                             50,90,     // size  \n\
                                             20*PI180); // angle in radian \n\
\n\
  aRectangle2->SetHighlightMode(AIS2D_TOD_ELEMENT);  //Set selection sub elements    \n\
  myAISInteractiveContext2D->Display(aRectangle);  // Dispaly rectangle    \n\
  }\n\
  FitAll2DViews(Standard_False); // Update Viewer\n\
\n\
---------------------------------------------------------------- \n\
   after that the system call the SetContext Methods :                    \n\
---------------------------------------------------------------- \n\
void Sample2D_Rectangle::SetContext(const Handle(AIS2D_InteractiveContext)& theContext) \n\
{\n\
\n\
  AIS2D_InteractiveObject::SetContext(theContext); \n\
\n\
  // add the polyline in the representation, for all modes\n\
  Handle(Graphic2d_Polyline) ThePolyline;\n\
  ThePolyline = new Graphic2d_Polyline(this,myArray1OfVertex );\n\
  ThePolyline->SetColorIndex ( 1 );\n\
  ThePolyline->SetWidthIndex ( 1 );\n\
  ThePolyline->SetTypeIndex  ( 1 );\n\
\n");
Message+= "\
------------------------------------------------------------------------------ \n\
   after on WM_MOUSEMOVE event call a method MoveTo of AIS2D_InteractiveContext  \n\
   and on WM_LBUTTONUP enent call a method Select of AIS2D_InteractiveContext\n\
------------------------------------------------------------------------------";

  // Update The Result Dialog
  UpdateResultDialog(anID,Message);
}

void CViewer2dDoc::OnBUTTONTestCurve()
{
  Standard_Integer color = 1;
  Standard_Integer type = 1;
  Standard_Integer width = 1;

  for (Standard_Integer i=0;i<=5;i++) 
   for (Standard_Integer j=0;j<=5;j++)
   {
     Handle_Geom2d_Curve aNewCurve  = GCE2d_MakeSegment ( gp_Pnt2d(10*i,10*j) , 
                                                          gp_Pnt2d(10*i+5,10*j+10 )).Value() ; 
     Handle(Sample2D_Curve) aCurve   = new Sample2D_Curve(aNewCurve);

	 aCurve->SetTypeOfLineIndex(type++);
	 aCurve->SetWidthOfLineIndex(width++);
     aCurve->SetColorIndex(color++);
     
	 aCurve->SetHighlightMode(AIS2D_TOD_OBJECT);

	 myAISInteractiveContext2D->Display(aCurve,            // object
                                        Standard_False);  // Redraw

     if (type == 5) type = 1; 
     if (width == 9) width = 1;       
     if (color == 2) color = 3;  // 2 = noir !!
     if (color == 13) color = 1; // 13 = nbcolor by default
   }

  FitAll2DViews(Standard_True); // Update Viewer
  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON_Test_Curve;
 
  TCollection_AsciiString Message(" \
create a set of Sample2D_Curve and display it:    \n\
------------------------------------------------------------------------------ \n\
  Standard_Integer color = 1;\n\
  Standard_Integer type = 1; \n\
  Standard_Integer width = 1; \n\
\n\
  for (Standard_Integer i=0;i<=5;i++) \n\
   for (Standard_Integer j=0;j<=5;j++)\n\
   {\n\
     Handle_Geom2d_Curve aNewCurve  = GCE2d_MakeSegment ( gp_Pnt2d(10*i,10*j) , \n\
                                                          gp_Pnt2d(10*i+5,10*j+10 )).Value() ; \n\
     Handle(Sample2D_Curve) aCurve   = new Sample2D_Curve(aNewCurve);\n\
     aCurve->SetColorIndex(color++);\n\
     aCurve->SetTypeOfLineIndex(type++);\n\
     aCurve->SetWidthOfLineIndex(width++);\n\
     aCurve->SetHighlightMode(AIS2D_TOD_OBJECT); \n\
 \n\
     myAISInteractiveContext2D->Display(aCurve,            // object\n\
                                        Standard_False);   // Redraw\n\
     if (type == 5) type = 1; //Max number of types + 1\n\
     if (width == 9) width = 1;  \n\
     if (color == 2) color =3;  // 2 = noir !!\n\
     if (color == 13) color =1; // 13 = nbcolor by default\n\
   }\n\
------------------------------------------------------------------------------ \n\
   after that the system call the SetContext Methods :                    \n\
------------------------------------------------------------------------------ \n");
Message += "\
void Sample2D_Curve::Compute(const Handle(AIS2D_InteractiveContext)& theContext) \n\
{\n\
  AIS2D_InteractiveObject::SetContext(theContext); \n\
\n\
  Handle(Prs2d_AspectLine) aLineAspect = new Prs2d_AspectLine;  \n\
  aLineAspect->SetTypeOfFill(Graphic2d_TOPF_FILLED); \n\
\n\
  \n\
  Handle(Graphic2d_SetOfCurves) segment;\n\
  segment = new Graphic2d_SetOfCurves(this);\n\
\n\
  segment->Add(myGeom2dCurve);\n\
  \n\
\n\
  segment->SetColorIndex (myColorIndex);\n\
  segment->SetWidthIndex (myWidthOfLineIndex );\n\
  segment->SetTypeIndex  (myTypeOfLineIndex  );\n\
  \n\
  SetAspect(aLineAspect); \n\
}\n\
\n";

  // Update The Result Dialog
  UpdateResultDialog(anID,Message);
}

void CViewer2dDoc::OnBUTTONTestImage()
{
  CFileDialog dlg(TRUE,
                  NULL,
                  NULL,
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  "image Files (*.bmp , *.gif , *.xwd)|*.bmp; *.gif; *.xwd; | all files (*.*)|*.*;||", 
                  NULL );

  CString initdir(((OCC_BaseApp*) AfxGetApp())->GetInitDataDir());
  initdir += "\\Data";

  dlg.m_ofn.lpstrInitialDir = initdir;
  
  if (dlg.DoModal() == IDOK) 
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    CString filename = dlg.GetPathName();
    Standard_CString aFileName = (Standard_CString)(LPCTSTR)filename;
    Handle(Sample2D_Image) anImage = new Sample2D_Image(aFileName);

    anImage->SetCoord(0,0) ;
    anImage->SetTypeOfPlacement(Aspect_CP_Center) ;
	anImage->SetIsZoomable(Standard_True);

    myAISInteractiveContext2D->Display(anImage,           // object
                                    Standard_True);  // Redraw
  
  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON_Test_Image;
  TCollection_AsciiString Message(" \
create a Sample2D_Image and display it :    \n\
------------------------------------------------------------------------------ \n\
  CFileDialog dlg(TRUE,   \n\
                       NULL,                                         \n\
                        NULL,                         \n\
                        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,\n\
                  \"image Files (*.bmp , *.gif , *.xwd)|*.bmp; *.gif; *.xwd; | all files (*.*)|*.*;||\", \n\
                             NULL );\n\
  if (dlg.DoModal() == IDOK) \n\
  {\n\
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));\n\
    CString filename = dlg.GetPathName();\n\
    Standard_CString aFileName = (Standard_CString)(LPCTSTR)filename;\n\
    Handle(Sample2D_Image) anImage = new Sample2D_Image(aFileName);\n\
\n\
    anImage->SetCoord(0,0) ;\n\
    anImage->SetTypeOfPlacement(Aspect_CP_Center) ;\n\
	anImage->SetIsZoomable(Standard_True);\n\
\n\
    myAISInteractiveContext2D->Display(anImage,           // Display object\n\
                                         Standard_True);  \n\
  }\n\
------------------------------------------------------------------------------ \n\
   after that the system call the SetContext method :                    \n\
------------------------------------------------------------------------------ \n\
void Sample2D_Image::SetContext(const Handle(AIS2D_InteractiveContext) theContext)) \n\
{\n\
  //Set AIS2D_InteractiveContext and default Graphic2d_View \n\
  AIS2D_InteractiveObject::SetContext(theContext); \n\
  Graphic2d_GraphicObject::SetView(theContext->CurrentViewer()->View()); \n\
\n\
  Handle(GraphicSample2D_ImageFile) aGraphic2dImageFile =\n\
    new GraphicSample2D_ImageFile (aGrObj,\n\
			                 myFile,\n\
			                 myX, \n\
			                 myY,\n\
			                 myDx,\n\
			                 myDy,\n\
			                 myTypeOfPlacement,\n\
			                 myScale); \n\
  if (myIsZoomable)\n\
    aGraphic2dImageFile->SetZoomable(Standard_True);\n\
}\n\n");

  // Update The Result Dialog
  UpdateResultDialog(filename,Message);
  }
}

void CViewer2dDoc::OnBUTTONMultipleImage()
{
  CFileDialog dlg(TRUE,
                  NULL,
                  NULL,
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  "image Files (*.bmp , *.gif , *.xwd)|*.bmp; *.gif; *.xwd; | all files (*.*)|*.*;||", 
                  NULL );

  CString initdir(((OCC_BaseApp*) AfxGetApp())->GetInitDataDir());
  initdir += "\\Data";

  dlg.m_ofn.lpstrInitialDir = initdir;
  

  if (dlg.DoModal() == IDOK) 
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    CString filename = dlg.GetPathName();
    Standard_CString aFileName = (Standard_CString)(LPCTSTR)filename;
    {  // 1
    Handle(Sample2D_Image) anImage = new Sample2D_Image(aFileName);

    anImage->SetCoord(40,50) ;
    anImage->SetTypeOfPlacement(Aspect_CP_SouthEast) ;
	anImage->SetIsZoomable(Standard_True);

    myAISInteractiveContext2D->Display(anImage,           // Display object
                                       Standard_False);  
    }
    {  // 2
    Handle(Sample2D_Image) anImage = new Sample2D_Image(aFileName);

    anImage->SetCoord(50,50) ;
    anImage->SetTypeOfPlacement(Aspect_CP_SouthWest) ;
	anImage->SetIsZoomable(Standard_True);

    myAISInteractiveContext2D->Display(anImage,           // Display object
                                       Standard_False);     
    }
    {  // 3
    Handle(Sample2D_Image) anImage = new Sample2D_Image(aFileName);

    anImage->SetCoord(40,40) ;
    anImage->SetTypeOfPlacement(Aspect_CP_NorthEast) ;
	anImage->SetIsZoomable(Standard_True);

    myAISInteractiveContext2D->Display(anImage,           // Display object
                                       Standard_False);  
    }
    {  // 4
    Handle(Sample2D_Image) anImage = new Sample2D_Image(aFileName);

    anImage->SetCoord(50,40) ;
    anImage->SetTypeOfPlacement(Aspect_CP_NorthWest) ;
	anImage->SetIsZoomable(Standard_True);

    myAISInteractiveContext2D->Display(anImage,           // Display object
                                       Standard_False);  
    }
    {  // 5
    Handle(Sample2D_Image) anImage = new Sample2D_Image(aFileName);

    anImage->SetCoord(80,45) ;
    anImage->SetTypeOfPlacement(Aspect_CP_West) ;
	anImage->SetIsZoomable(Standard_True);
	anImage->SetScale(2);
    myAISInteractiveContext2D->Display(anImage,           // Display object
                                       Standard_False);  
    }
    {  // 6 : Non Zoomable 
    Handle(Sample2D_Image) anImage = new Sample2D_Image(aFileName);

    anImage->SetCoord(20,-20) ;
    anImage->SetTypeOfPlacement(Aspect_CP_NorthWest) ;

	anImage->SetIsZoomable(Standard_False);

    myAISInteractiveContext2D->Display(anImage,           // Display object
                                       Standard_False);  
    }
    {  // 7 : Non Zoomable
    Handle(Sample2D_Image) anImage = new Sample2D_Image(aFileName);

    anImage->SetCoord(0,0) ;
    anImage->SetOffset(60,-20) ; // new zoom center
    anImage->SetTypeOfPlacement(Aspect_CP_NorthWest) ;
	anImage->SetScale(0.5);
	anImage->SetIsZoomable(Standard_False);

    myAISInteractiveContext2D->Display(anImage,           // Display object
                                       Standard_False);  
    }
    FitAll2DViews(Standard_True); // Update Viewer
  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON_Test_MultiImages;
  TCollection_AsciiString Message(" \
create a set of Sample2D_Image and display it :    \n\
------------------------------------------------------------------------------ \n\
  CFileDialog dlg(TRUE, \n\
                  NULL, \n\
                  NULL, \n\
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, \n\
                  \"image Files (*.bmp , *.gif , *.xwd)|*.bmp; *.gif; *.xwd; | all files (*.*)|*.*;||\", \n\
                  NULL ); \n\
  if (dlg.DoModal() == IDOK)  \n\
  { \n\
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT)); \n\
    CString filename = dlg.GetPathName(); \n\
    Standard_CString aFileName = (Standard_CString)(LPCTSTR)filename; \n\
    {  // 1 \n\
    Handle(Sample2D_Image) anImage = new Sample2D_Image(aFileName); \n\
 \n\
    anImage->SetCoord(40,50) ; \n\
    anImage->SetTypeOfPlacement(Aspect_CP_SouthEast) ; \n\
	anImage->SetIsZoomable(Standard_True); \n\
 \n\
    myAISInteractiveContext2D->Display(anImage,           // object \n\
                                        Standard_False);  // Redraw \n\
    } \n\
    //... \n\
} \n\
------------------------------------------------------------------------------ \n\
   after that the system call the SetContext Methods :                    \n\
------------------------------------------------------------------------------ \n\
void Sample2D_Image::SetContext(const Handle(AIS2D_InteractiveContext) theContext)) \n\
{\n\
\n\
  if(theContext.IsNull() || theContext->CurrentViewer().IsNull()) return; \n\
\n\
  //Set AIS2D_InteractiveContext and default Graphic2d_View \n\
  AIS2D_InteractiveObject::SetContext(theContext); \n\
  Graphic2d_GraphicObject::SetView(theContext->CurrentViewer()->View()); \n\
\n\
  Handle(GraphicSample2D_ImageFile) aGraphic2dImageFile =\n\
    new GraphicSample2D_ImageFile (aGrObj,\n\
			                 myFile,\n\
			                 myX, \n\
			                 myY,\n\
			                 myDx,\n\
			                 myDy,\n\
			                 myTypeOfPlacement,\n\
			                 myScale); \n\
  if (myIsZoomable)\n\
    aGraphic2dImageFile->SetZoomable(Standard_True);\n\
}\n\n");
  // Update The Result Dialog
  UpdateResultDialog(filename,Message);
  }
}
