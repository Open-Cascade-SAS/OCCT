// GeomSources.cpp: implementation of the GeomSources class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeomSources.h"
#include "GeometryApp.h"
#include "MainFrm.h"
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>

GeomSources::GeomSources()
{
}

GeomSources::~GeomSources()
{
}
void GeomSources::PreProcess(CGeometryDoc* aDoc,DisplayType aDisplayType)
{
  if (aDisplayType == No2D3D )
  {   
    aDoc->GetAISContext()->EraseAll (Standard_True);
    aDoc->Put3DOnTop(); 
  }

  if (aDisplayType == a2DNo3D)
  { 
    aDoc->GetISessionContext()->EraseAll (Standard_True);
    aDoc->Put2DOnTop();
  }

  if (aDisplayType != No2D3D && aDisplayType != a2D3D)
  {  
    aDoc->Minimize3D();
  }

  if (aDisplayType != a2DNo3D && aDisplayType != a2D3D)
  {  
    aDoc->Minimize2D();
  }

  if (aDisplayType == a2D3D)
  {
    aDoc->GetAISContext()->EraseAll (Standard_True);
    aDoc->GetISessionContext()->EraseAll (Standard_True);
    aDoc->Put3DOnTop(false); 
    aDoc->Put2DOnTop(false);

    // both on top
    // send the message Title Horizontaly to the child of doc main frame
    CGeometryApp* TheAppli = (CGeometryApp*)AfxGetApp();
    CMainFrame* TheMainFrame= (CMainFrame*)TheAppli->m_pMainWnd;
    ::SendMessage(TheMainFrame->m_hWndMDIClient, WM_MDITILE, 0, 0);
  }
}

void GeomSources::PostProcess (CGeometryDoc* aDoc, UINT anID, DisplayType aDisplayType,
                               const TCollection_AsciiString& theString, Standard_Boolean UpdateViewer, Standard_Real Coef)
{
  Standard_CString aString = theString.ToCString();
  if (UpdateViewer)
  {
    if (aDisplayType == No2D3D || aDisplayType == a2D3D)
    {
      aDoc->Fit3DViews(Coef);
    }

    if (aDisplayType == a2DNo3D || aDisplayType == a2D3D)
    {
      aDoc->Fit2DViews();
    }
  }

  TCollection_AsciiString Message("Results are ");

  switch (aDisplayType)
  {
  case No2DNo3D: Message = "All results are in this box \n";
    break;
  case No2D3D:   Message += "only in 3D \n";
    break;
  case a2DNo3D:   Message += "only in 2d \n";
    break;
  case a2D3D:     Message += "in both 2D and 3D \n";
    break;
  }   
  Message += "====================================\n";
  Message += aString;

  CString text(Message.ToCString());
  aDoc->myCResultDialog.SetText(text);

  CString s;
  if (! s.LoadString( anID ))
    AfxMessageBox (L"Error Loading String: ");

  CString Title = s.Left( s.Find( '\n' ));

  aDoc->myCResultDialog.SetTitle(Title);
  aDoc->SetTitle(Title);
}

void GeomSources::AddSeparator(CGeometryDoc* /*aDoc*/,TCollection_AsciiString& aMessage)
{
  aMessage+= "------------------------------------------------------------------------\n";
}
void GeomSources::DisplayPoint (CGeometryDoc* theDoc,
                                const gp_Pnt2d& thePoint,
                                const char* theText,
                                Standard_Boolean theToUpdateViewer,
                                Standard_Real theXoffset,
                                Standard_Real theYoffset,
                                Standard_Real theTextScale)
{
  Handle(ISession_Point) aGraphicPoint = new ISession_Point (thePoint);
  theDoc->GetISessionContext()->Display (aGraphicPoint, Standard_False);

  Handle(AIS_TextLabel) aLabel = new AIS_TextLabel();
  aLabel->SetText (theText);
  aLabel->SetPosition (gp_Pnt (thePoint.X() + theXoffset, thePoint.Y() + theYoffset, 0.0));
  //aLabel->SetHeight (theTextScale);
  (void )theTextScale;
  theDoc->GetISessionContext()->Display (aLabel, theToUpdateViewer);
}

void GeomSources::DisplayPoint (CGeometryDoc* theDoc,
                                const gp_Pnt& thePoint,
                                const char* theText,
                                Standard_Boolean theToUpdateViewer,
                                Standard_Real theXoffset,
                                Standard_Real theYoffset,
                                Standard_Real theZoffset,
                                Standard_Real theTextScale)
{
  Handle(ISession_Point) aGraphicPoint = new ISession_Point (thePoint);
  theDoc->GetAISContext()->Display (aGraphicPoint, Standard_False);

  Handle(AIS_TextLabel) aLabel = new AIS_TextLabel();
  aLabel->SetText (theText);
  aLabel->SetPosition (gp_Pnt (thePoint.X() + theXoffset, thePoint.Y() + theYoffset, thePoint.Z() + theZoffset));
  //aLabel->SetHeight (theTextScale);
  (void )theTextScale;
  theDoc->GetAISContext()->Display (aLabel, theToUpdateViewer);
}

void GeomSources::DisplayCurve(CGeometryDoc* aDoc,
                          Handle(Geom2d_Curve) aCurve,
                          Standard_Integer aColorIndex,
                          Standard_Boolean UpdateViewer)
{
  Handle(ISession2D_Curve) aGraphicCurve = new ISession2D_Curve(aCurve);
  aGraphicCurve->SetColorIndex(aColorIndex) ;
  aDoc->GetISessionContext()->Display(aGraphicCurve,UpdateViewer);
}

void GeomSources::DisplayCurveAndCurvature(CGeometryDoc* aDoc,
                          Handle(Geom2d_Curve) aCurve,
                          Standard_Integer aColorIndex,
                          Standard_Boolean UpdateViewer)
{
  Handle(ISession2D_Curve) aGraphicCurve = new ISession2D_Curve(aCurve);
  aGraphicCurve->SetDisplayCurbure(Standard_True) ;
  aGraphicCurve->SetDiscretisation(20);
  aGraphicCurve->SetColorIndex(aColorIndex) ;
  aDoc->GetISessionContext()->Display(aGraphicCurve,UpdateViewer);
}

void GeomSources::DisplayCurve (CGeometryDoc* theDoc,
                                Handle(Geom_Curve) theCurve,
                                Quantity_NameOfColor theNameOfColor,
                                Standard_Boolean theToUpdateViewer)
{
  Handle(ISession_Curve) aGraphicCurve = new ISession_Curve (theCurve);
  aGraphicCurve->Attributes()->SetLineAspect (new Prs3d_LineAspect (theNameOfColor, Aspect_TOL_SOLID, 1.0));
  theDoc->GetAISContext()->Display (aGraphicCurve, theToUpdateViewer);
}

void GeomSources::DisplayCurve(CGeometryDoc* aDoc,
                          Handle(Geom_Curve) aCurve,
                          Standard_Boolean UpdateViewer)
{
    Handle(ISession_Curve) aGraphicCurve = new ISession_Curve(aCurve);
    aDoc->GetAISContext()->Display(aGraphicCurve,UpdateViewer);
}

void GeomSources::DisplaySurface (CGeometryDoc* theDoc,
                                  Handle(Geom_Surface) theSurface,
                                  Quantity_NameOfColor theNameOfColor,
                                  Standard_Boolean theToUpdateViewer)
{
  const Handle(AIS_InteractiveContext)& aCtx = theDoc->GetAISContext();
  Handle(Prs3d_ShadingAspect) aShadeAspect = new Prs3d_ShadingAspect();
  Handle(Prs3d_LineAspect)    aLineAspect  = new Prs3d_LineAspect (theNameOfColor, Aspect_TOL_SOLID, 1.0);
  Handle(Prs3d_IsoAspect)     anIsoAspect  = new Prs3d_IsoAspect  (theNameOfColor, Aspect_TOL_SOLID, 1.0,
                                                                   aCtx->DefaultDrawer()->UIsoAspect()->Number());
  aShadeAspect->SetColor (theNameOfColor);

  Handle(ISession_Surface) aGraphicalSurface = new ISession_Surface (theSurface);
  const Handle(Prs3d_Drawer)& aDrawer      = aGraphicalSurface->Attributes();
  aDrawer->SetShadingAspect      (aShadeAspect);
  aDrawer->SetLineAspect         (aLineAspect);
  aDrawer->SetFreeBoundaryAspect (aLineAspect);
  aDrawer->SetUIsoAspect         (anIsoAspect);
  aDrawer->SetVIsoAspect         (anIsoAspect);
  aCtx->Display (aGraphicalSurface, theToUpdateViewer);
}

void GeomSources::DisplaySurface(CGeometryDoc* aDoc,
                          Handle(Geom_Surface) aSurface,
                          Standard_Boolean UpdateViewer)
{
  Handle(ISession_Surface) aGraphicalSurface = new ISession_Surface(aSurface);
  aDoc->GetAISContext()->Display(aGraphicalSurface,UpdateViewer);
}

void GeomSources::ResetView(CGeometryDoc* aDoc)
{
  aDoc->GetAISContext()->CurrentViewer()->InitActiveViews();
  Handle(V3d_View) aView = aDoc->GetAISContext()->CurrentViewer()->ActiveView();
  aView->Reset();
}

// Function name	: GeomSources::gpTest1
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest1(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  gp_XYZ A(1,2,3);                        
  gp_XYZ B(2,2,2);                        
  gp_XYZ C(3,2,3);                        
  Standard_Real result = A.DotCross(B,C); 
                                        
//==============================================================
  TCollection_AsciiString Message ("\
                                        \n\
gp_XYZ A(1,2,3);                        \n\
gp_XYZ B(2,2,2);                        \n\
gp_XYZ C(3,2,3);                        \n\
Standard_Real result = A.DotCross(B,C); \n\
                                        \n");
  AddSeparator(aDoc,Message);
//--------------------------------------------------------------

    DisplayPoint(aDoc,gp_Pnt(A),"A (1,2,3)",false,0.1);
    DisplayPoint(aDoc,gp_Pnt(B),"B (2,2,2)",false,0.1);
    DisplayPoint(aDoc,gp_Pnt(C),"C (3,2,3)",false,0.1);

// to add a numeric value in a TCollectionAsciiString	
  TCollection_AsciiString Message2 (result);
    
  Message+= " result = ";
  Message+= Message2;
  PostProcess(aDoc,ID_BUTTON_Test_1,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest2
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest2(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
//==============================================================
                   
  gp_Pnt P1(1,2,3);  
                   
//==============================================================
  TCollection_AsciiString Message ("\
                  \n\
gp_Pnt P1(1,2,3); \n\
                  \n");
  AddSeparator(aDoc,Message);
//--------------------------------------------------------------
  DisplayPoint(aDoc,P1,"P1 (1,2,3)",false,30);
  PostProcess(aDoc,ID_BUTTON_Test_2,TheDisplayType,Message,Standard_False);
  ResetView(aDoc);
}


// Function name	: GeomSources::gpTest3
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest3(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
//==============================================================
                  
  gp_XYZ A(1,2,3);  
  gp_Pnt P2(A);     
                  
//==============================================================
  TCollection_AsciiString Message ("\
                   \n\
gp_XYZ A(1,2,3);   \n\
gp_Pnt P2(A);      \n\
                   \n");
  AddSeparator(aDoc,Message);
//--------------------------------------------------------------
  DisplayPoint(aDoc,P2,"P2 (1,2,3)",false,30);
  PostProcess(aDoc,ID_BUTTON_Test_3,TheDisplayType,Message,Standard_False);
  ResetView(aDoc);
}


// Function name	: GeomSources::gpTest4
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest4(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
//==============================================================
                                              
  gp_Pnt P3 = gp::Origin();                     
  Standard_Real TheX = P3.X();
  Standard_Real TheY = P3.Y();
  Standard_Real TheZ = P3.Z();
  
                                              
//==============================================================
    TCollection_AsciiString Message ("\
                               \n\
gp_Pnt P3 = gp::Origin();      \n\
Standard_Real TheX = P3.X();   \n\
Standard_Real TheY = P3.Y();   \n\
Standard_Real TheZ = P3.Z();   \n\
                               \n");
  AddSeparator(aDoc,Message);
//--------------------------------------------------------------
  DisplayPoint(aDoc,P3,"P3 = gp::Origin()",false,30);

  TCollection_AsciiString Message2 (TheX);
  TCollection_AsciiString Message3 (TheY);
  TCollection_AsciiString Message4 (TheZ);

  Message += " TheX = ";
  Message += Message2;
  Message += " TheY = "; 
  Message += Message3;
  Message += " TheZ = "; 
  Message4 = TheZ;
  Message += Message4;

  PostProcess(aDoc,ID_BUTTON_Test_4,TheDisplayType,Message,Standard_False);
  ResetView(aDoc);
}


// Function name	: GeomSources::gpTest5
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest5(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
//==============================================================
                               
  gp_Pnt P1(1,2,3);              
  gp_Pnt P2(3,4,5);              
  gp_Pnt PB = P1;                
  Standard_Real alpha = 3;       
  Standard_Real beta = 7;        
  PB.BaryCenter(alpha,P2,beta);  
                               
//==============================================================
  TCollection_AsciiString Message ("\
                                 \n\
gp_Pnt P1(1,2,3);                \n\
gp_Pnt P2(3,4,5);                \n\
gp_Pnt PB = P1;                  \n\
Standard_Real alpha = 3;         \n\
Standard_Real beta = 7;          \n\
PB.BaryCenter(alpha,P2,beta);    \n\
                                 \n");

  AddSeparator(aDoc,Message);
//--------------------------------------------------------------

  DisplayPoint(aDoc,P1,"P1",false,0.2);
  DisplayPoint(aDoc,P2,"P2",false,0.2);
  DisplayPoint(aDoc,PB,"PB = barycenter ( 3 * P1 , 7 * P2) ",false,0.2);

  TCollection_AsciiString Message2 (PB.X());
  TCollection_AsciiString Message3 (PB.Y());
  TCollection_AsciiString Message4 (PB.Z());

  Message += " PB ( ";
  Message += Message2; 
  Message += " , ";
  Message += Message3; 
  Message += " , ";
  Message += Message4; 
  Message += " ); ";
  PostProcess(aDoc,ID_BUTTON_Test_5,TheDisplayType,Message);
}


// Function name	: GeomSources::gpTest6
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest6(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
//==============================================================
                                                                 
//  Compute a 3d point P as BaryCenter of an array of point
  gp_Pnt P1(0,0,5);
  gp_Pnt P2(1,2,3);
  gp_Pnt P3(2,3,-2);
  gp_Pnt P4(4,3,5);
  gp_Pnt P5(5,5,4);                                                
  TColgp_Array1OfPnt array (1,5); // sizing array                  
  array.SetValue(1,P1);
  array.SetValue(2,P2);
  array.SetValue(3,P3);
  array.SetValue(4,P4);
  array.SetValue(5,P5);

  Standard_Real Tolerance = 8; // ajout de la tolerance            
  GProp_PEquation PE (array,Tolerance);

  gp_Pnt P; // P declaration
  Standard_Boolean IsPoint;

  if (PE.IsPoint())
  {
    IsPoint = true;
    P = PE .Point();
  }
  else 
  { 
    IsPoint = false;
  }                     

  if (PE.IsLinear()){ /*... */ }
  if (PE.IsPlanar()){ /*... */ }
  if (PE.IsSpace()) { /*... */ }
                                                                 
//==============================================================
  TCollection_AsciiString Message ("\
                                                                 \n\
                                                                 \n\
//  Compute a 3d point P as BaryCenter of an array of point      \n\
gp_Pnt P1(0,0,5);                                                \n\
gp_Pnt P2(1,2,3);                                                \n\
gp_Pnt P3(2,3,-2);                                               \n\
gp_Pnt P4(4,3,5);                                                \n\
gp_Pnt P5(5,5,4);                                                \n\
TColgp_Array1OfPnt array (1,5); // sizing array                  \n\
array.SetValue(1,P1);                                            \n\
array.SetValue(2,P2);                                            \n\
array.SetValue(3,P3);                                            \n\
array.SetValue(4,P4);                                            \n\
array.SetValue(5,P5);                                            \n\
                                                                 \n\
Standard_Real Tolerance = 8; // ajout de la tolerance            \n\
GProp_PEquation PE (array,Tolerance);                            \n\
                                                                 \n\
gp_Pnt P; // P declaration                                       \n\
Standard_Boolean IsPoint;                                        \n\
if (PE.IsPoint()){IsPoint = true;                                \n\
                  P = PE .Point();}                              \n\
                  else { IsPoint = false;  }                     \n\
if (PE.IsLinear()){ /*... */ }                                   \n\
if (PE.IsPlanar()){ /*... */ }                                   \n\
if (PE.IsSpace()) { /*... */ }                                   \n\
                                                                 \n");
  AddSeparator(aDoc,Message);
    //--------------------------------------------------------------

  TCollection_AsciiString PointName("P");
    

	for(Standard_Integer i= array.Lower();i <= array.Upper(); i++)
  { 
    TCollection_AsciiString TheString (i);
    TheString = PointName+ TheString;
    DisplayPoint(aDoc,array(i),TheString.ToCString(),false,0.5);
  }

  DisplayPoint(aDoc,P,"P",false,0.5);
  TCollection_AsciiString Message2 (P.X());
  TCollection_AsciiString Message3 (P.Y());
  TCollection_AsciiString Message4 (P.Z());

  Message += " IsPoint = ";
  if (IsPoint) 
  {
    Message += "True   -->  ";
    Message += " P ( ";

    Message += Message2; Message += " , ";
    Message += Message3; Message += " , ";
    Message += Message4; Message += " ); \n";
	}  
  else 
    Message += "False\n";

  Message += " IsLinear = ";  
  if (PE.IsLinear()) 
    Message += "True \n"; 
  else
    Message += "False\n";

  Message += " IsPlanar = ";
  if (PE.IsPlanar())
    Message += "True \n";
  else
    Message += "False\n";

  Message += " IsSpace = ";
  if
    (PE.IsSpace())
    Message += "True \n";
  else
    Message += "False\n";

    PostProcess(aDoc,ID_BUTTON_Test_6,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest7
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest7(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);
//==============================================================
                                           
  gp_Pnt2d P1(0,5);                                       
  gp_Pnt2d P2(5.5,1);                                     
  gp_Pnt2d P3(-2,2);                                      

  Handle(Geom2d_Curve) C =                         
    GCE2d_MakeArcOfCircle (P1,P2,P3).Value();           

  Standard_Real FirstParameter = C->FirstParameter();
  Standard_Real LastParameter = C->LastParameter();
  Standard_Real MiddleParameter = (FirstParameter+LastParameter)/2;
  Standard_Real param = MiddleParameter; //in radians    

  gp_Pnt2d P;                                             
  gp_Vec2d V;                                             
  C->D1(param,P,V);                          
// we recover point P and the vector V     
                                           
//==============================================================
  TCollection_AsciiString Message ("\
                                                    \n\
                                                    \n\
gp_Pnt2d P1(0,5);                                   \n\
gp_Pnt2d P2(5.5,1);                                 \n\
gp_Pnt2d P3(-2,2);                                  \n\
                                                    \n\
Handle(Geom2d_TrimmedCurve) C =                     \n\
    GCE2d_MakeArcOfCircle (P1,P2,P3).Value();       \n\
                                                    \n\
Standard_Real FirstParameter = C->FirstParameter(); \n\
Standard_Real LastParameter = C->LastParameter();   \n\
Standard_Real MiddleParameter =                     \n\
           (FirstParameter+LastParameter)/2;        \n\
Standard_Real param = MiddleParameter; //in radians \n\
                                                    \n\
gp_Pnt2d P;                                         \n\
gp_Vec2d V;                                         \n\
C->D1(param,P,V);                                   \n\
// we recover point P and the vector V              \n\
                                                    \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplayCurve(aDoc,C);
  Handle(ISession_Direction) aDirection = new ISession_Direction(P,V);
  aDoc->GetISessionContext()->Display(aDirection, Standard_False);

  DisplayPoint(aDoc,P,"P",false,0.5);

  PostProcess(aDoc,ID_BUTTON_Test_7,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest8
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest8(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);
//==============================================================
                                                   
  Standard_Real radius = 5;                          
  Handle(Geom2d_Circle) C =                          
    new Geom2d_Circle(gp::OX2d(),radius);          
  Standard_Real param = 1.2*M_PI;                      
  Geom2dLProp_CLProps2d CLP                          
    (C,param,2,Precision::PConfusion());           
  gp_Dir2d D;                                    
  CLP.Tangent(D);                                    
// D is the Tangent direction at parameter 1.2*PI  
                                                   
//==============================================================
  TCollection_AsciiString Message (" \
                                                   \n\
Standard_Real radius = 5;                          \n\
Handle(Geom2d_Circle) C =                          \n\
    new Geom2d_Circle(gp::OX2d(),radius);          \n\
Standard_Real param = 1.2*PI;                      \n\
Geom2dLProp_CLProps2d CLP                          \n\
    (C,param,2,Precision::PConfusion());           \n\
    gp_Dir2d D;                                    \n\
CLP.Tangent(D);                                    \n\
// D is the Tangent direction at parameter 1.2*PI  \n\
                                                   \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------
  Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(C);
  aDoc->GetISessionContext()->Display(aCurve,Standard_False);
  Handle(ISession_Direction) aDirection = new ISession_Direction(gp_Pnt2d(0,0),D,2);
  aDoc->GetISessionContext()->Display(aDirection,Standard_False);

  TCollection_AsciiString Message2 (D.X());
  TCollection_AsciiString Message3 (D.Y());

  Message += " D ( ";
  Message += Message2; Message += " , ";
  Message += Message3; Message += " ); \n";

  PostProcess(aDoc,ID_BUTTON_Test_8,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest9
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest9(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================
                                                             
  Standard_Real radius = 5;                                    
  Handle(Geom2d_Circle) C =                                    
    new Geom2d_Circle(gp::OX2d(),radius);                    
  Geom2dAdaptor_Curve GAC (C);                                 
  Standard_Real startparam = 10*M_PI/180;                                
  Standard_Real abscissa = 45*M_PI/180;                                  
  gp_Pnt2d P1;
  C->D0(startparam,P1);                                                
  // abscissa is the distance along the curve from startparam  
  GCPnts_AbscissaPoint AP (GAC, abscissa, startparam);         
  gp_Pnt2d P2;                                                  
  if (AP.IsDone()){C->D0(AP.Parameter(),P2);}                   
  // P is now correctly set                                    

  //==============================================================
  TCollection_AsciiString Message (" \n\
                                                             \n\
                                                             \n\
Standard_Real radius = 5;                                    \n\
Handle(Geom2d_Circle) C =                                    \n\
    new Geom2d_Circle(gp::OX2d(),radius);                    \n\
Geom2dAdaptor_Curve GAC (C);                                 \n\
Standard_Real startparam = 10*PI180;                            \n\
Standard_Real abscissa = 45*PI180;                                 \n\
gp_Pnt2d P1;                                                 \n\
C->D0(startparam,P1);                                                \n\
// abscissa is the distance along the curve from startparam  \n\
GCPnts_AbscissaPoint AP (GAC, abscissa, startparam);         \n\
gp_Pnt2d P2;                                                  \n\
if (AP.IsDone()){C->D0(AP.Parameter(),P2);}                   \n\
// P is now correctly set                                    \n\
                                                             \n\
                                                             \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------
  Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(C);
  aDoc->GetISessionContext()->Display(aCurve,Standard_False);

  DisplayPoint(aDoc,P1,"P1");
  if (AP.IsDone())    DisplayPoint(aDoc,P2,"P2");

  TCollection_AsciiString Message2 (P1.X());
  TCollection_AsciiString Message3 (P1.Y());

  TCollection_AsciiString Message4 (P2.X());
  TCollection_AsciiString Message5 (P2.Y());

  Message += " P1 ( ";
  Message += Message2; Message += " , ";
  Message += Message3; Message += " ); \n";

  Message += " P2 ( ";
  Message += Message4; Message += " , ";
  Message += Message5; Message += " ); \n";
  PostProcess(aDoc,ID_BUTTON_Test_9,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest10
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest10(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================
                                                        
  gp_Pnt2d P;
  Standard_Real radius = 5;
  Handle(Geom2d_Circle) C =
    new Geom2d_Circle(gp::OX2d(),radius);
  Geom2dAdaptor_Curve GAC (C);
  Standard_Real abscissa = 3;
  GCPnts_UniformAbscissa UA (GAC,abscissa);
  TColgp_SequenceOfPnt2d aSequence;
  if (UA.IsDone())
  {                                                     
    Standard_Real N = UA.NbPoints();
    Standard_Integer count = 1;
    for(;count<=N;count++)
    {                                                  
      C->D0(UA.Parameter(count),P);
      //Standard_Real Parameter = UA.Parameter(count);
      // append P in a Sequence
      aSequence.Append(P);
    }                                                   
  }                                                       
  Standard_Real Abscissa  = UA.Abscissa();                
                                                        
  //==============================================================
  TCollection_AsciiString Message (" \
                                                        \n\
gp_Pnt2d P;                                             \n\
Standard_Real radius = 5;                               \n\
Handle(Geom2d_Circle) C =                               \n\
    new Geom2d_Circle(gp::OX2d(),radius);               \n\
Geom2dAdaptor_Curve GAC (C);                            \n\
Standard_Real abscissa = 3;                             \n\
GCPnts_UniformAbscissa UA (GAC,abscissa);               \n\
TColgp_SequenceOfPnt2d aSequence;                       \n\
if (UA.IsDone())                                        \n\
  {                                                     \n\
    Standard_Real N = UA.NbPoints();                    \n\
    Standard_Integer count = 1;                         \n\
    for(;count<=N;count++)                              \n\
     {                                                  \n\
        C->D0(UA.Parameter(count),P);                   \n\
        Standard_Real Parameter = UA.Parameter(count);  \n\
        // append P in a Sequence                       \n\
        aSequence.Append(P);                            \n\
    }                                                   \n\
}                                                       \n\
Standard_Real Abscissa  = UA.Abscissa();                \n\
                                                        \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------
  Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(C);
  aDoc->GetISessionContext()->Display(aCurve,Standard_False);

  TCollection_AsciiString aString;
  for (Standard_Integer i=1;i<= aSequence.Length();i++)
  {

    TCollection_AsciiString Message2 (i);
    TCollection_AsciiString Message3 (UA.Parameter(i));

    aString = "P";
    aString += Message2; 
    aString +=": Parameter : "; 
    aString += Message3;

    //   First and Last texts are displayed with an Y offset, point 4 is upper
    Standard_Real YOffset = -0.3;
    YOffset +=  0.2 * ( i == 1 );
    YOffset +=  0.4 * ( i == 4 );
    YOffset += -0.2 * ( i == aSequence.Length() ); 

    DisplayPoint(aDoc,aSequence(i),aString.ToCString(),false,0.5,YOffset,0.04);
  }

  TCollection_AsciiString Message3 (Abscissa);

  Message += "Abscissa  = ";
  Message += Message3;
  Message += " \n";

  PostProcess(aDoc,ID_BUTTON_Test_10,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest11
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest11(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================
                 
  Standard_Real radius = 5;
  Handle(Geom_SphericalSurface) SP =
    new Geom_SphericalSurface(gp_Ax3(gp::XOY()),radius);
  Standard_Real u = 2;
  Standard_Real v = 3;
  gp_Pnt P = SP->Value(u,v);                
                       
//==============================================================
    TCollection_AsciiString Message (" \
                                                          \n\
Standard_Real radius = 5;                                 \n\
Handle(Geom_SphericalSurface) SP =                        \n\
    new Geom_SphericalSurface(gp_Ax3(gp::XOY()),radius);  \n\
Standard_Real u = 2;                                      \n\
Standard_Real v = 3;                                      \n\
gp_Pnt P = SP->Value(u,v);                                \n\
                                                          \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplaySurface(aDoc,SP);
  DisplayPoint(aDoc,P,"P",false,0.5);
  TCollection_AsciiString Message2 (P.X());
  TCollection_AsciiString Message3 (P.Y());
	
  Message += " P ( ";
  Message += Message2; 
  Message += " , ";
  Message += Message3; 
  Message += " ); \n";

  PostProcess(aDoc,ID_BUTTON_Test_11,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest12
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest12(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  gp_Pnt N,Q,P(1,2,3);
  Standard_Real distance, radius = 5;
  Handle(Geom_Curve) C = new Geom_Circle(gp::XOY(),radius);
  GeomAPI_ProjectPointOnCurve PPC (P,C);
  N = PPC.NearestPoint();
  Standard_Integer NbResults = PPC.NbPoints();

  if(NbResults>0)
  {
    for(Standard_Integer i = 1;i<=NbResults;i++)
    {
      Q = PPC.Point(i);
      distance = PPC.Distance(i);
      // do something with Q or distance here
    }
  }                                                          

  //==============================================================
  TCollection_AsciiString Message (" \
                                                            \n\
gp_Pnt N,Q,P(1,2,3);                                        \n\
Standard_Real distance, radius = 5;                         \n\
Handle(Geom_Circle) C = new Geom_Circle(gp::XOY(),radius);  \n\
GeomAPI_ProjectPointOnCurve PPC (P,C);                      \n\
N = PPC.NearestPoint();                                     \n\
Standard_Integer NbResults = PPC.NbPoints();                \n\
                                                            \n\
if(NbResults>0){                                            \n\
    for(Standard_Integer i = 1;i<=NbResults;i++){           \n\
      Q = PPC.Point(i);                                     \n\
      distance = PPC.Distance(i);                           \n\
        // do something with Q or distance here             \n\
      }                                                     \n\
 }                                                          \n\
                                                            \n");
    AddSeparator(aDoc,Message);
//--------------------------------------------------------------

  TCollection_AsciiString aString;

  DisplayPoint(aDoc,P,"P",false,0.5);

  TCollection_AsciiString Message2 (PPC.LowerDistance());

  aString = "N : at Distance : "; 
  aString += Message2;

  DisplayPoint(aDoc,N,aString.ToCString(),false,0.5,0,-0.5);
  DisplayCurve(aDoc,C,Quantity_NOC_YELLOW,false);

  if(NbResults>0)
  {
    for(Standard_Integer i = 1;i<=NbResults;i++)
    {
      Q = PPC.Point(i);
      distance = PPC.Distance(i);	
      TCollection_AsciiString Message3 (i);
      TCollection_AsciiString Message4 (distance);

      aString = "Q";
      aString += Message3; 
      aString +=": at Distance : "; 
      aString += Message4;
      DisplayPoint(aDoc,Q,aString.ToCString(),false,0.5);
    }
  }

  PostProcess(aDoc,ID_BUTTON_Test_12,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest13
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest13(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================
        
  gp_Pnt N,Q,P(7,8,9);
  Standard_Real distance, radius = 5;
  Handle(Geom_SphericalSurface) SP =
    new Geom_SphericalSurface(gp_Ax3(gp::XOY()),radius);
  GeomAPI_ProjectPointOnSurf PPS(P,SP);
  N = PPS.NearestPoint();
  Standard_Integer NbResults = PPS.NbPoints();
  if(NbResults>0)
  {
    for(Standard_Integer i = 1;i<=NbResults;i++)
    {
      Q = PPS.Point(i);
      distance = PPS.Distance(i);
      // do something with Q or distance here
    }
  }                                                         
                                                          
  //==============================================================
    TCollection_AsciiString Message (" \
                                                          \n\
gp_Pnt N,Q,P(7,8,9);                                      \n\
Standard_Real distance, radius = 5;                       \n\
Handle(Geom_SphericalSurface) SP =                        \n\
    new Geom_SphericalSurface(gp_Ax3(gp::XOY()),radius);  \n\
GeomAPI_ProjectPointOnSurf PPS(P,SP);                     \n\
N = PPS.NearestPoint();                                   \n\
Standard_Integer NbResults = PPS.NbPoints();              \n\
if(NbResults>0){                                          \n\
    for(Standard_Integer i = 1;i<=NbResults;i++){         \n\
    Q = PPS.Point(i);                                     \n\
    distance = PPS.Distance(i);                           \n\
    // do something with Q or distance here               \n\
    }                                                     \n\
}                                                         \n\
                                                          \n");
 AddSeparator(aDoc,Message);
 //--------------------------------------------------------------
 TCollection_AsciiString aString;

 DisplayPoint(aDoc,P,"P",false,0.5);
 TCollection_AsciiString Message2 (PPS.LowerDistance());

 aString = "N  : at Distance : "; aString += Message2;
 DisplayPoint(aDoc,N,aString.ToCString(),false,0.5,0,-0.6);

 Handle(ISession_Surface) aSurface = new ISession_Surface(SP);
 Handle(Prs3d_Drawer) CurDrawer = aSurface->Attributes();
 CurDrawer->UIsoAspect()->SetNumber(10);
 CurDrawer->VIsoAspect()->SetNumber(10);
 aDoc->GetAISContext()->SetLocalAttributes(aSurface, CurDrawer, Standard_False);
 aDoc->GetAISContext()->Display(aSurface, Standard_False);

 if(NbResults>0)
 {
   for(Standard_Integer i = 1;i<=NbResults;i++)
   {

     Q = PPS.Point(i);
     distance = PPS.Distance(i);	
     TCollection_AsciiString Message3 (i);
     TCollection_AsciiString Message4 (distance);

     aString = "Q";
     aString += Message3; 
     aString +=": at Distance : "; 
     aString += Message4;

     DisplayPoint(aDoc,Q,aString.ToCString(),false,0.5);			
   }
 }
 PostProcess(aDoc,ID_BUTTON_Test_13,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest14
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest14(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================
                                                          
  gp_Pnt P; 
  gp_Ax3 theAxe(gp::XOY());
  gp_Pln PL(theAxe);                            
  Standard_Real MinorRadius = 5;                            
  Standard_Real MajorRadius = 8;                            
  gp_Elips EL (gp::YOZ(),MajorRadius,MinorRadius);          
  IntAna_IntConicQuad ICQ                                   
    (EL,PL,Precision::Angular(),Precision::Confusion());  
  if (ICQ.IsDone()){                                        
    Standard_Integer NbResults = ICQ.NbPoints();          
    if (NbResults>0){                                     
      for(Standard_Integer i = 1;i<=NbResults;i++){         
        P = ICQ.Point(i);                                 
        // do something with P here                       
      }                                                 
    }                                                      
  }                                                         
                                                          
  //==============================================================
  TCollection_AsciiString Message (" \
                                                          \n\
gp_Pnt P;                                                 \n\
gp_Pln PL (gp_Ax3(gp::XOY()));                            \n\
Standard_Real MinorRadius = 5;                            \n\
Standard_Real MajorRadius = 8;                            \n\
gp_Elips EL (gp::YOZ(),MajorRadius,MinorRadius);          \n\
IntAna_IntConicQuad ICQ                                   \n\
    (EL,PL,Precision::Angular(),Precision::Confusion());  \n\
if (ICQ.IsDone()){                                        \n\
    Standard_Integer NbResults = ICQ.NbPoints();          \n\
    if (NbResults>0){                                     \n\
    for(Standard_Integer i = 1;i<=NbResults;i++){         \n\
        P = ICQ.Point(i);                                 \n\
        // do something with P here                       \n\
        }                                                 \n\
   }                                                      \n\
}                                                         \n\
                                                          \n");
 AddSeparator(aDoc,Message);
 //--------------------------------------------------------------

 Handle(Geom_Plane) aPlane = GC_MakePlane(PL).Value();
 Handle(Geom_RectangularTrimmedSurface) aSurface= new Geom_RectangularTrimmedSurface(aPlane,-8.,8.,-12.,12.);

 DisplaySurface(aDoc,aSurface);

 Handle(Geom_Curve) anEllips = GC_MakeEllipse(EL).Value();
 DisplayCurve(aDoc,anEllips,Quantity_NOC_YELLOW,false);

 TCollection_AsciiString aString;

 if (ICQ.IsDone())
 {
   Standard_Integer NbResults = ICQ.NbPoints();
   if (NbResults>0)
   {
     for(Standard_Integer i = 1;i<=NbResults;i++)
     {

       TCollection_AsciiString Message2(i);

       P = ICQ.Point(i);
       aString = "P";aString += Message2; 
       DisplayPoint(aDoc,P,aString.ToCString(),false,0.5);
     }
   }
 }

 PostProcess(aDoc,ID_BUTTON_Test_14,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest15
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest15(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================
 
  gp_Pnt P1(1,2,3);    
  gp_Pnt P1Copy = P1;  
  gp_Pnt P2(5,4,6);    
  gp_Trsf TRSF;        
  TRSF.SetMirror(P2);  
  P1Copy.Transform(TRSF);  

  //==============================================================
    TCollection_AsciiString Message (" \
                         \n\
gp_Pnt P1(1,2,3);        \n\
gp_Pnt P1Copy = P1;      \n\
gp_Pnt P2(5,4,6);        \n\
gp_Trsf TRSF;            \n\
TRSF.SetMirror(P2);      \n\
P1Copy.Transform(TRSF);  \n\
                         \n");
 AddSeparator(aDoc,Message);
 //--------------------------------------------------------------

 DisplayPoint(aDoc,P1Copy,"P1Copy",false,0.5);
 DisplayPoint(aDoc,P1,"P1",false,0.5);
 DisplayPoint(aDoc,P2,"P2",false,0.5);

 PostProcess(aDoc,ID_BUTTON_Test_15,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest16
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest16(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
//==============================================================
                                         
  gp_Pnt P1(1,2,3);                        
  gp_Pnt P2(5,4,6);                        
  gp_Vec V1 (P1,P2);                       

  gp_Pnt P3(10,4,7);                       
  gp_Pnt P4(2,0,1);                        
  gp_Vec V2 (P3,P4);                       

  Standard_Boolean result =                
    V1.IsOpposite(V2,Precision::Angular());  
  // result should be true                 
                                         
//==============================================================
  TCollection_AsciiString Message (" \
                                         \n\
gp_Pnt P1(1,2,3);                        \n\
gp_Pnt P2(5,4,6);                        \n\
gp_Vec V1 (P1,P2);                       \n\
                                         \n\
gp_Pnt P3(10,4,7);                       \n\
gp_Pnt P4(2,0,1);                        \n\
gp_Vec V2 (P3,P4);                       \n\
                                         \n\
Standard_Boolean result =                \n\
V1.IsOpposite(V2,Precision::Angular());  \n\
// result should be true                 \n\
                                         \n");
 AddSeparator(aDoc,Message);
 //--------------------------------------------------------------

 DisplayPoint(aDoc,P1,"P1",false,0.5);
 DisplayPoint(aDoc,P2,"P2",false,0.5);
 DisplayPoint(aDoc,P3,"P3",false,0.5);
 DisplayPoint(aDoc,P4,"P4",false,0.5);

 Handle(ISession_Direction) aDirection1 = new ISession_Direction(P1,V1);
 aDoc->GetAISContext()->Display(aDirection1, Standard_False);

 Handle(ISession_Direction) aDirection2 = new ISession_Direction(P3,V2);
 aDoc->GetAISContext()->Display(aDirection2, Standard_False);

 Message += "result = ";
 if (result) Message += "True \n"; else Message += "False \n";

 PostProcess(aDoc,ID_BUTTON_Test_16,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest17
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest17(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  gp_Dir D1(1,2,3);
  gp_Dir D2(3,4,5);
  Standard_Real ang = D1.Angle(D2);
  // the result is in radians in the range [0,PI]  
                                                 
  //==============================================================
  TCollection_AsciiString Message (" \
                                                 \n\
gp_Dir D1(1,2,3);                                \n\
gp_Dir D2(3,4,5);                                \n\
Standard_Real ang = D1.Angle(D2);                \n\
// the result is in radians in the range [0,PI]  \n\
                                                 \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  Handle(ISession_Direction) aDirection1 = new ISession_Direction(gp_Pnt(0,0,0),D1,3);
  aDoc->GetAISContext()->Display(aDirection1, Standard_False);

  Handle(ISession_Direction) aDirection2 = new ISession_Direction(gp_Pnt(0,0,0),D2,3);
  aDoc->GetAISContext()->Display(aDirection2, Standard_False);

  cout<<" D1.Angle(D2) : "<<ang<<endl;

  TCollection_AsciiString Message2 (ang);
  TCollection_AsciiString Message3 (ang/M_PI/180);

  Message += " ang =  "; 
  Message += Message2; 
  Message += "   radian \n";
  Message += " ang/PI180 =  "; 
  Message += Message3; 
  Message += "   degree \n";

  PostProcess(aDoc,ID_BUTTON_Test_17,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest18
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest18(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  gp_Pnt2d P(2,3);
  gp_Dir2d D(4,5);
  gp_Ax22d A(P,D);
  gp_Parab2d Para(A,6);
  // P is the vertex point                  
  // P and D give the axis of symmetry      
  // 6 is the focal length of the parabola

  //==============================================================
  TCollection_AsciiString Message (" \
                                          \n\
gp_Pnt2d P(2,3);                          \n\
gp_Dir2d D(4,5);                          \n\
gp_Ax22d A(P,D);                          \n\
gp_Parab2d Para(A,6);                     \n\
// P is the vertex point                  \n\
// P and D give the axis of symmetry      \n\
// 6 is the focal length of the parabola  \n\
                                          \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplayPoint(aDoc,P,"P",false,0.5,0,3);

  Handle(ISession_Direction) aDirection = new ISession_Direction(P,D,200);
  aDoc->GetISessionContext()->Display(aDirection,Standard_False);
  Handle(Geom2d_Parabola) aParabola = GCE2d_MakeParabola(Para);
  Handle(Geom2d_TrimmedCurve) aTrimmedCurve = new Geom2d_TrimmedCurve(aParabola,-100,100);
  Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(aTrimmedCurve);
  //aCurve->SetColorIndex(3);
  aDoc->GetISessionContext()->Display(aCurve, Standard_False);  

  Message += " The entity A of type gp_Ax22d is not displayable \n ";
  Message += " The entity D of type gp_Dir2d is displayed as a vector \n    ( mean with a length != 1 ) \n ";
  PostProcess(aDoc,ID_BUTTON_Test_18,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest19
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest19(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================
  
  gp_Pnt P1(2,3,4);                           
  gp_Dir D(4,5,6);                            
  gp_Ax3 A(P1,D);                              
  Standard_Boolean IsDirectA = A.Direct();    

  gp_Dir AXDirection = A.XDirection() ;       
  gp_Dir AYDirection = A.YDirection() ;       

  gp_Pnt P2(5,3,4);                           
  gp_Ax3 A2(P2,D);                            
  A2.YReverse();                              
  // axis3 is now left handed                 
  Standard_Boolean IsDirectA2 = A2.Direct();  
              
  gp_Dir A2XDirection = A2.XDirection() ;
  gp_Dir A2YDirection = A2.YDirection() ;

  //==============================================================
  TCollection_AsciiString Message (" \
                                            \n\
gp_Pnt P1(2,3,4);                           \n\
gp_Dir D(4,5,6);                            \n\
gp_Ax3 A(P,D);                              \n\
Standard_Boolean IsDirectA = A.Direct();    \n\
                                            \n\
gp_Dir AXDirection = A.XDirection() ;       \n\
gp_Dir AYDirection = A.YDirection() ;       \n\
                                            \n\
gp_Pnt P2(5,3,4);                           \n\
gp_Ax3 A2(P2,D);                            \n\
A2.YReverse();                              \n\
// axis3 is now left handed                 \n\
Standard_Boolean IsDirectA2 = A2.Direct();  \n\
                                            \n\
gp_Dir A2XDirection = A2.XDirection() ;     \n\
gp_Dir A2YDirection = A2.YDirection() ;     \n\
                                            \n");
 AddSeparator(aDoc,Message);
 //--------------------------------------------------------------

 // Set style for vector lines
 Handle(Prs3d_LineAspect) anAxesAspect = new Prs3d_LineAspect (Quantity_NOC_GREEN, Aspect_TOL_SOLID, 1.0);

 DisplayPoint(aDoc,P1,"P1",false,0.1);
 Handle(ISession_Direction) aDirection = new ISession_Direction(P1,D,2);
 aDoc->GetAISContext()->Display(aDirection, Standard_False);

 Handle(ISession_Direction) aDirection2 = new ISession_Direction(P1,AXDirection,2);
 aDirection2->SetText("A.XDirection");
 aDirection2->SetLineAspect (anAxesAspect);
 aDoc->GetAISContext()->Display(aDirection2, Standard_False);
 Handle(ISession_Direction) aDirection3 = new ISession_Direction(P1,AYDirection,2);
 aDirection3->SetText("A.YDirection");
 aDirection3->SetLineAspect (anAxesAspect);
 aDoc->GetAISContext()->Display(aDirection3, Standard_False);

 DisplayPoint(aDoc,P2,"P2",false,0.1);
 Handle(ISession_Direction) aDirection4 = new ISession_Direction(P2,D,2);
 aDoc->GetAISContext()->Display(aDirection4, Standard_False);

 Handle(ISession_Direction) aDirection5 = new ISession_Direction(P2,A2XDirection,2);
 aDirection5->SetText("A2 XDirection");
 aDirection5->SetLineAspect (anAxesAspect);
 aDoc->GetAISContext()->Display(aDirection5, Standard_False);
 Handle(ISession_Direction) aDirection6 = new ISession_Direction(P2,A2YDirection,2);
 aDirection6->SetText("A2 YDirection");
 aDirection6->SetLineAspect (anAxesAspect);
 aDoc->GetAISContext()->Display(aDirection6, Standard_False);

 Message += "IsDirectA = ";
 if(IsDirectA)
   Message += "True = Right Handed \n";
 else
   Message += "False = Left Handed \n";

 Message += "IsDirectA2 = ";
 if(IsDirectA2)
   Message += "True = Right Handed \n";
 else
   Message += "False = Left Handed  \n";

 PostProcess(aDoc,ID_BUTTON_Test_19,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest20
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest20(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  TColgp_Array1OfPnt2d array (1,5); // sizing array                    
  array.SetValue(1,gp_Pnt2d (0,0));
  array.SetValue(2,gp_Pnt2d (1,2));
  array.SetValue(3,gp_Pnt2d (2,3));
  array.SetValue(4,gp_Pnt2d (4,3));
  array.SetValue(5,gp_Pnt2d (5,5));
  Handle(Geom2d_BSplineCurve) SPL1 =
    Geom2dAPI_PointsToBSpline(array);
                                                                     
  Handle(TColgp_HArray1OfPnt2d) harray =
    new TColgp_HArray1OfPnt2d (1,5); // sizing harray
  harray->SetValue(1,gp_Pnt2d (7+ 0,0));
  harray->SetValue(2,gp_Pnt2d (7+ 1,2));
  harray->SetValue(3,gp_Pnt2d (7+ 2,3));
  harray->SetValue(4,gp_Pnt2d (7+ 4,3));
  harray->SetValue(5,gp_Pnt2d (7+ 5,5));
  Geom2dAPI_Interpolate anInterpolation(harray,Standard_False,0.01);
  anInterpolation.Perform();
  Handle(Geom2d_BSplineCurve) SPL2 = anInterpolation.Curve();

  Handle(TColgp_HArray1OfPnt2d) harray2 =
    new TColgp_HArray1OfPnt2d (1,5); // sizing harray
  harray2->SetValue(1,gp_Pnt2d (11+ 0,0));
  harray2->SetValue(2,gp_Pnt2d (11+ 1,2));
  harray2->SetValue(3,gp_Pnt2d (11+ 2,3));
  harray2->SetValue(4,gp_Pnt2d (11+ 4,3));
  harray2->SetValue(5,gp_Pnt2d (11+ 5,5));
  Geom2dAPI_Interpolate anInterpolation2(harray2,Standard_True,0.01);
  anInterpolation2.Perform();
  Handle(Geom2d_BSplineCurve) SPL3 = anInterpolation2.Curve();
  // redefined C++ operator allows these assignments
 
  //==============================================================
  TCollection_AsciiString Message (" \
                                                                     \n\
TColgp_Array1OfPnt2d array (1,5); // sizing array                    \n\
array.SetValue(1,gp_Pnt2d (0,0));                                    \n\
array.SetValue(2,gp_Pnt2d (1,2));                                    \n\
array.SetValue(3,gp_Pnt2d (2,3));                                    \n\
array.SetValue(4,gp_Pnt2d (4,3));                                    \n\
array.SetValue(5,gp_Pnt2d (5,5));                                    \n\
Handle(Geom2d_BSplineCurve) SPL1 =                                   \n\
    Geom2dAPI_PointsToBSpline(array);                                \n\
                                                                     \n\
Handle(TColgp_HArray1OfPnt2d) harray =                               \n\
    new TColgp_HArray1OfPnt2d (1,5); // sizing harray                \n\
harray->SetValue(1,gp_Pnt2d (7+ 0,0));                               \n\
harray->SetValue(2,gp_Pnt2d (7+ 1,2));                               \n\
harray->SetValue(3,gp_Pnt2d (7+ 2,3));                               \n\
harray->SetValue(4,gp_Pnt2d (7+ 4,3));                               \n\
harray->SetValue(5,gp_Pnt2d (7+ 5,5));                               \n\
Geom2dAPI_Interpolate anInterpolation(harray,Standard_False,0.01);   \n\
anInterpolation.Perform();                                           \n\
Handle(Geom2d_BSplineCurve) SPL2 = anInterpolation.Curve();          \n\
                                                                     \n\
Handle(TColgp_HArray1OfPnt2d) harray2 =                              \n\
    new TColgp_HArray1OfPnt2d (1,5); // sizing harray                \n");
  Message += "\
harray2->SetValue(1,gp_Pnt2d (11+ 0,0));                             \n\
harray2->SetValue(2,gp_Pnt2d (11+ 1,2));                             \n\
harray2->SetValue(3,gp_Pnt2d (11+ 2,3));                             \n\
harray2->SetValue(4,gp_Pnt2d (11+ 4,3));                             \n\
harray2->SetValue(5,gp_Pnt2d (11+ 5,5));                             \n\
Geom2dAPI_Interpolate anInterpolation2(harray2,Standard_True,0.01);  \n\
anInterpolation2.Perform();                                          \n\
Handle(Geom2d_BSplineCurve) SPL3 = anInterpolation2.Curve();         \n\
// redefined C++ operator allows these assignments                   \n\
                                                                     \n";
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------
  TCollection_AsciiString aString;
  for(int i = array.Lower();i<=array.Upper();i++)
  {
    gp_Pnt2d P = array(i);
    TCollection_AsciiString Message2 (i);
    aString = "array ";aString += Message2;
    DisplayPoint(aDoc,P,aString.ToCString(),false,0.5);
  }
  for( int i = harray->Lower();i<=harray->Upper();i++)
  {
    gp_Pnt2d P = harray->Value(i);
    TCollection_AsciiString Message2 (i);
    aString = "harray ";aString += Message2; 
    DisplayPoint(aDoc,P,aString.ToCString(),false,0.5);
  }
  for( int i = harray2->Lower();i<=harray2->Upper();i++)
  {
    gp_Pnt2d P = harray2->Value(i);
    TCollection_AsciiString Message2 (i);
    aString = "harray2 ";aString += Message2; 
    DisplayPoint(aDoc,P,aString.ToCString(),false,0.5);
  }

  if (!SPL1.IsNull())
  { 
    Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(SPL1);
    aCurve->SetColorIndex(3);
    aDoc->GetISessionContext()->Display(aCurve, Standard_False);
  }
  else
    MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"SPL1.IsNull()", L"CasCade Error", MB_ICONERROR);

  if (!SPL2.IsNull())
  {
    Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(SPL2);
    aCurve2->SetColorIndex(5);
    aDoc->GetISessionContext()->Display(aCurve2, Standard_False);  
  }
  else
    MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"SPL2.IsNull()", L"CasCade Error", MB_ICONERROR);

  if (!SPL3.IsNull())
  {
    Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(SPL3);
    aCurve2->SetColorIndex(6);
    aDoc->GetISessionContext()->Display(aCurve2, Standard_False);  
  }
  else
    MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"SPL3.IsNull()", L"CasCade Error", MB_ICONERROR);

  Message += " SPL1  is Red  \n";
  Message += " SPL2  is Blue \n";   
  Message += " SPL3  is Yellow \n";   

  PostProcess(aDoc,ID_BUTTON_Test_20,TheDisplayType,Message);
}

void GeomSources::gpTest21(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================

  gp_Pnt2d P1(-184, 101);
  gp_Pnt2d P2(20 ,84);
  Standard_Real aheight = 1;
  FairCurve_Batten B (P1,P2,aheight);
  B.SetAngle1(22*M_PI/180);
  B.SetAngle2(44*M_PI/180);
  FairCurve_AnalysisCode anAnalysisCode;
  B.Compute(anAnalysisCode);
  Handle(Geom2d_BSplineCurve) C = B.Curve();

  //==============================================================
  TCollection_AsciiString Message (" \
                                            \n\
gp_Pnt2d P1(-184, 101);                     \n\
gp_Pnt2d P2(20 ,84);                        \n\
Standard_Real aheight = 1;                  \n\
FairCurve_Batten B (P1,P2,aheight);         \n\
B.SetAngle1(22*PI180);                      \n\
B.SetAngle2(44*PI180);                      \n\
FairCurve_AnalysisCode anAnalysisCode;      \n\
B.Compute(anAnalysisCode);                  \n\
Handle(Geom2d_BSplineCurve) C = B.Curve();  \n\
                                            \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplayCurveAndCurvature(aDoc,C,6,Standard_False);               

  PostProcess(aDoc,ID_BUTTON_Test_21,TheDisplayType,Message);
}

void GeomSources::gpTest22(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================
                                                
  gp_Pnt2d P1(-184, 41);                          
  gp_Pnt2d P2(20 ,24);                            
  Standard_Real aheight = 1;                      
  FairCurve_MinimalVariation MV (P1,P2,aheight);  
  MV.SetAngle1(22*M_PI/180);                         
  MV.SetAngle2(44*M_PI/180);                         

  FairCurve_AnalysisCode anAnalysisCode;          
  MV.Compute(anAnalysisCode);                     

  Handle(Geom2d_BSplineCurve) C = MV.Curve();     

  //==============================================================
  TCollection_AsciiString Message (" \
                                                \n\
gp_Pnt2d P1(-184, 41);                          \n\
gp_Pnt2d P2(20 ,24);                            \n\
Standard_Real aheight = 1;                      \n\
FairCurve_MinimalVariation MV (P1,P2,aheight);  \n\
MV.SetAngle1(22*PI180);                         \n\
MV.SetAngle2(44*PI180);                         \n\
                                                \n\
FairCurve_AnalysisCode anAnalysisCode;          \n\
MV.Compute(anAnalysisCode);                     \n\
                                                \n\
Handle(Geom2d_BSplineCurve) C = MV.Curve();     \n\
                                                \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplayCurveAndCurvature(aDoc,C,7,Standard_False);                   
  DisplayPoint(aDoc,P1,"P1",false,0.5);
  DisplayPoint(aDoc,P2,"P2",false,0.5);

  PostProcess(aDoc,ID_BUTTON_Test_22,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest23
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest23(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  Standard_Real major = 12;                                               
  Standard_Real minor = 4;                                                
  gp_Ax2d axis = gp::OX2d();                                              
  Handle(Geom2d_Ellipse) E = GCE2d_MakeEllipse (axis,major,minor);
  Handle(Geom2d_TrimmedCurve) TC = new Geom2d_TrimmedCurve(E,-1,2);

  // The segment goes in the direction Vfrom P1  
  // to the point projected on this line by P2   
  // In the example (0,6).
  Handle(Geom2d_BSplineCurve) SPL =
    Geom2dConvert::CurveToBSplineCurve(TC);

  //==============================================================
  TCollection_AsciiString Message (" \
                                                                   \n\
Standard_Real major = 12;                                          \n\
Standard_Real minor = 4;                                           \n\
gp_Ax2d axis = gp::OX2d();                                         \n\
Handle(Geom2d_Ellipse) E = GCE2d_MakeEllipse (axis,major,minor);   \n\
                                                                   \n\
Handle(Geom2d_TrimmedCurve) TC = new Geom2d_TrimmedCurve(E,-1,2);  \n\
                                                                   \n\
// The segment goes in the direction Vfrom P1                      \n\
// to the point projected on this line by P2                       \n\
// In the example (0,6).                                           \n\
Handle(Geom2d_BSplineCurve) SPL =                                  \n\
    Geom2dConvert::CurveToBSplineCurve(TC);                        \n\
                                                                   \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(E);
  aCurve->SetColorIndex(3); // Red
  aCurve->SetTypeOfLine(Aspect_TOL_DOTDASH);
  aDoc->GetISessionContext()->Display(aCurve, Standard_False);

  Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(SPL);
  aDoc->GetISessionContext()->Display(aCurve2, Standard_False);

  PostProcess(aDoc,ID_BUTTON_Test_23,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest24
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest24(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================
  
  Standard_Real radius = 5;
  gp_Ax2d ax2d(gp_Pnt2d(2,3),gp_Dir2d(1,0));

  Handle(Geom2d_Circle) circ2d =
    new Geom2d_Circle(ax2d,radius);

  gp_Ax2d circ2dXAxis = circ2d->XAxis();

  // create a 3D curve in a given plane
  Handle(Geom_Curve) C3D =
    GeomAPI::To3d(circ2d,gp_Pln(gp_Ax3(gp::XOY())));
  Handle(Geom_Circle) C3DCircle =
    Handle(Geom_Circle)::DownCast(C3D);

  gp_Ax1 C3DCircleXAxis = C3DCircle->XAxis();

  // project it to a 2D curve in another plane

  gp_Pln ProjectionPlane(gp_Pnt(1,1,0),gp_Dir( 1,1,1 ));

  Handle(Geom2d_Curve) C2D =
    GeomAPI::To2d(C3D,ProjectionPlane);

  Handle(Geom2d_Circle) C2DCircle =
    Handle(Geom2d_Circle)::DownCast(C2D);
  gp_Ax2d C2DCircleXAxis = C2DCircle->XAxis();

  //==============================================================
  TCollection_AsciiString Message (" \
                                                        \n\
Standard_Real radius = 5;                               \n\
gp_Ax2d ax2d(gp_Pnt2d(2,3),gp_Dir2d(1,0));              \n\
                                                        \n\
Handle(Geom2d_Circle) circ2d =                          \n\
    new Geom2d_Circle(ax2d,radius);                     \n\
                                                        \n\
gp_Ax2d circ2dXAxis = circ2d->XAxis();                  \n\
                                                        \n\
// create a 3D curve in a given plane                   \n\
Handle(Geom_Curve) C3D =                                \n\
    GeomAPI::To3d(circ2d,gp_Pln(gp_Ax3(gp::XOY())));    \n\
Handle(Geom_Circle) C3DCircle =                         \n\
   Handle(Geom_Circle)::DownCast(C3D);                  \n\
                                                        \n\
gp_Ax1 C3DCircleXAxis = C3DCircle->XAxis();             \n\
                                                        \n\
// project it to a 2D curve in another plane            \n\
                                                        \n\
gp_Pln ProjectionPlane(gp_Pnt(1,1,0),gp_Dir( 1,1,1 ));  \n\
                                                        \n\
Handle(Geom2d_Curve) C2D =                              \n\
    GeomAPI::To2d(C3D,ProjectionPlane);                 \n\
                                                        \n\
Handle(Geom2d_Circle) C2DCircle =                       \n\
  Handle(Geom2d_Circle)::DownCast(C2D);                 \n\
gp_Ax2d C2DCircleXAxis = C2DCircle->XAxis();            \n\
                                                        \n");
 AddSeparator(aDoc,Message);
 //--------------------------------------------------------------
 Handle(Geom_Plane) aPlane = GC_MakePlane(gp_Pln(gp_Ax3(gp::XOY()))).Value();
 Handle(Geom_RectangularTrimmedSurface) aSurface= new Geom_RectangularTrimmedSurface(aPlane,-8.,8.,-12.,12.);   
 DisplaySurface(aDoc,aSurface);

 Handle(Geom_Plane) aProjectionPlane = GC_MakePlane(ProjectionPlane).Value();
 Handle(Geom_RectangularTrimmedSurface) aProjectionPlaneSurface=
   new Geom_RectangularTrimmedSurface(aProjectionPlane,-8.,8.,-12.,12.);

 DisplaySurface(aDoc,aProjectionPlaneSurface);

 Standard_CString aC3DEntityTypeName = C3D->DynamicType()->Name();        
 Standard_CString aC2DEntityTypeName = C2D->DynamicType()->Name();        

 Message += " C3D->DynamicType()->Name() = ";
 Message += aC3DEntityTypeName; Message += " \n";
 Message += " C2D->DynamicType()->Name() = ";
 Message += aC2DEntityTypeName; Message += " \n";

 DisplayCurve(aDoc,circ2d,4,false);
 DisplayCurve(aDoc,C3D,Quantity_NOC_YELLOW,false);
 DisplayCurve(aDoc,C2D,5,false);

 Handle(ISession_Direction) aC3DCircleXAxisDirection = new ISession_Direction(C3DCircleXAxis.Location(),C3DCircleXAxis.Direction(),5.2);
 aDoc->GetAISContext()->Display(aC3DCircleXAxisDirection, Standard_False);

 Handle(ISession_Direction) acirc2dXAxisDirection = new ISession_Direction(circ2dXAxis.Location(),circ2dXAxis.Direction(),5.2);
 aDoc->GetISessionContext()->Display(acirc2dXAxisDirection, Standard_False);

 Handle(ISession_Direction) aC2DCircleXAxisDirection = new ISession_Direction(C2DCircleXAxis.Location(),C2DCircleXAxis.Direction(),5.2);
 aDoc->GetISessionContext()->Display(aC2DCircleXAxisDirection, Standard_False);

 PostProcess(aDoc,ID_BUTTON_Test_24,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest25
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest25(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================

  Handle(TColgp_HArray1OfPnt2d) harray =
    new TColgp_HArray1OfPnt2d (1,5); // sizing harray
  harray->SetValue(1,gp_Pnt2d (0,0));
  harray->SetValue(2,gp_Pnt2d (-3,1));
  harray->SetValue(3,gp_Pnt2d (-2,5));
  harray->SetValue(4,gp_Pnt2d (2,9));
  harray->SetValue(5,gp_Pnt2d (-4,14));

Geom2dAPI_Interpolate anInterpolation(harray,Standard_False,0.01);
anInterpolation.Perform();
Handle(Geom2d_BSplineCurve) SPL = anInterpolation.Curve();

gp_Pnt2d P1(-1,-2);
gp_Pnt2d P2(0,15);
gp_Dir2d V1 = gp::DY2d();
Handle(Geom2d_TrimmedCurve) TC1 =
    GCE2d_MakeSegment(P1,V1,P2);

Standard_Real tolerance = Precision::Confusion();
Geom2dAPI_InterCurveCurve ICC (SPL,TC1,tolerance);
Standard_Integer NbPoints =ICC.NbPoints();
gp_Pnt2d PK;

for (Standard_Integer k = 1;k<=NbPoints;k++)
{                                                                 
  PK = ICC.Point(k);                                              
  // do something with each intersection point                    
}                                                                 

  //==============================================================
  TCollection_AsciiString Message (" \
                                                                    \n\
Handle(TColgp_HArray1OfPnt2d) harray =                              \n\
    new TColgp_HArray1OfPnt2d (1,5); // sizing harray               \n\
harray->SetValue(1,gp_Pnt2d (0,0));                                 \n\
harray->SetValue(2,gp_Pnt2d (-3,1));                                \n\
harray->SetValue(3,gp_Pnt2d (-2,5));                                \n\
harray->SetValue(4,gp_Pnt2d (2,9));                                 \n\
harray->SetValue(5,gp_Pnt2d (-4,14));                               \n\
                                                                    \n\
Geom2dAPI_Interpolate anInterpolation(harray,Standard_False,0.01);  \n\
anInterpolation.Perform();                                          \n\
Handle(Geom2d_BSplineCurve) SPL = anInterpolation.Curve();          \n\
                                                                    \n\
gp_Pnt2d P1(-1,-2);                                                 \n\
gp_Pnt2d P2(0,15);                                                  \n\
gp_Dir2d V1 = gp::DY2d();                                           \n\
Handle(Geom2d_TrimmedCurve) TC1=                                    \n\
    GCE2d_MakeSegment(P1,V1,P2);                                    \n\
                                                                    \n\
Standard_Real tolerance = Precision::Confusion();                   \n\
Geom2dAPI_InterCurveCurve ICC (SPL,TC1,tolerance);                  \n\
Standard_Integer NbPoints =ICC.NbPoints();                          \n\
gp_Pnt2d PK;                                                        \n\
for (Standard_Integer k = 1;k<=NbPoints;k++)                        \n\
  {                                                                 \n\
    PK = ICC.Point(k);                                              \n\
    // do something with each intersection point                    \n\
  }                                                                 \n\
                                                                    \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  Handle(ISession2D_Curve) aCurve1 = new ISession2D_Curve(SPL);
  aCurve1->SetDisplayPole(Standard_False);
  aDoc->GetISessionContext()->Display(aCurve1, Standard_False);
  Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(TC1);
  aDoc->GetISessionContext()->Display(aCurve2, Standard_False);

  TCollection_AsciiString aString;
  for (Standard_Integer i = 1;i<=NbPoints;i++)
  {
    PK = ICC.Point(i);
    // do something with each intersection point
    TCollection_AsciiString Message2 (i);
    TCollection_AsciiString Message3 (PK.X());
    TCollection_AsciiString Message4 (PK.Y());
    aString = "PK_";
    aString += Message2; 

    DisplayPoint(aDoc,PK,aString.ToCString(),false,0.5);

    Message += "PK_"; 
    Message += Message2; 
    Message += " ( ";
    Message += Message3; 
    Message += " , "; 
    Message += Message4; 
    Message += " )\n"; 
  }

  PostProcess(aDoc,ID_BUTTON_Test_25,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest26
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest26(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================

  //----------- Build TC1 -----------------------
  gp_Pnt2d P1(0,0);  gp_Pnt2d P2(2,6);
  gp_Dir2d V1 = gp::DY2d();
  Handle(Geom2d_TrimmedCurve) TC1 =  GCE2d_MakeSegment(P1,V1,P2);
  Standard_Real FP1 = TC1->FirstParameter();
  Standard_Real LP1 = TC1->LastParameter();
  //----------- Build TC2 -----------------------
  gp_Pnt2d P3(-9,6.5);       gp_Dir2d V2 = gp::DX2d();
  Handle(Geom2d_TrimmedCurve) TC2 = GCE2d_MakeSegment(P3,V2,P2);
  Standard_Real FP2 = TC1->FirstParameter();
  Standard_Real LP2 = TC1->LastParameter();
  //----------- Extrema TC1 / TC2 ---------------
  Geom2dAPI_ExtremaCurveCurve ECC (TC1,TC2, FP1,LP1, FP2,LP2);
  Standard_Real shortestdistance =-1;
  if (ECC.NbExtrema() != 0)  shortestdistance = ECC.LowerDistance();
  //----------- Build SPL1 ---------------------- 
  TColgp_Array1OfPnt2d array (1,5); // sizing array
  array.SetValue(1,gp_Pnt2d (-4,0)); array.SetValue(2,gp_Pnt2d (-7,2));
  array.SetValue(3,gp_Pnt2d (-6,3)); array.SetValue(4,gp_Pnt2d (-4,3));
  array.SetValue(5,gp_Pnt2d (-3,5));
  Handle(Geom2d_BSplineCurve) SPL1 = Geom2dAPI_PointsToBSpline(array);
  Standard_Real FPSPL1 = SPL1->FirstParameter();
  Standard_Real LPSPL1 = SPL1->LastParameter();
  //----------- Extrema TC1 / SPL1  -------------                       
  Geom2dAPI_ExtremaCurveCurve ECC2 (TC1,SPL1, FP1,LP1, FPSPL1,LPSPL1);
  Standard_Real SPL1shortestdistance =-1;
  if (ECC2.NbExtrema()!=0) SPL1shortestdistance = ECC2.LowerDistance();
  Standard_Integer NbExtrema = ECC2.NbExtrema();
  TColgp_Array2OfPnt2d aSolutionArray(1,NbExtrema,1,2);
  for(int i=1;i <= NbExtrema; i++)
  {                                  
    gp_Pnt2d P1x,P2x;
    ECC2.Points(i,P1x,P2x);
    aSolutionArray(i,1) = P1x;
    aSolutionArray(i,2) = P2x;
  }            

  //==============================================================
  TCollection_AsciiString Message (" \
//----------- Build TC1 -----------------------                       \n\
gp_Pnt2d P1(0,0);  gp_Pnt2d P2(2,6);                                  \n\
gp_Dir2d V1 = gp::DY2d();                                             \n\
Handle(Geom2d_TrimmedCurve) TC1 =  GCE2d_MakeSegment(P1,V1,P2);       \n\
Standard_Real FP1 = TC1->FirstParameter();                            \n\
Standard_Real LP1 = TC1->LastParameter();                             \n\
//----------- Build TC2 -----------------------                       \n\
gp_Pnt2d P3(-9,6.5);       gp_Dir2d V2 = gp::DX2d();                    \n\
Handle(Geom2d_TrimmedCurve) TC2 = GCE2d_MakeSegment(P3,V2,P2);        \n\
Standard_Real FP2 = TC1->FirstParameter();                            \n\
Standard_Real LP2 = TC1->LastParameter();                             \n\
//----------- Extrema TC1 / TC2 ---------------                       \n\
Geom2dAPI_ExtremaCurveCurve ECC (TC1,TC2, FP1,LP1, FP2,LP2);          \n\
Standard_Real shortestdistance =-1;                                   \n\
if (ECC.NbExtrema() != 0)  shortestdistance = ECC.LowerDistance();    \n\
//----------- Build SPL1 ----------------------                       \n\
TColgp_Array1OfPnt2d array (1,5); // sizing array                     \n\
array.SetValue(1,gp_Pnt2d (-4,0)); array.SetValue(2,gp_Pnt2d (-7,2)); \n\
array.SetValue(3,gp_Pnt2d (-6,3)); array.SetValue(4,gp_Pnt2d (-4,3)); \n\
array.SetValue(5,gp_Pnt2d (-3,5));                                    \n\
Handle(Geom2d_BSplineCurve) SPL1 = Geom2dAPI_PointsToBSpline(array);  \n\
Standard_Real FPSPL1 = SPL1->FirstParameter();                        \n");
Message += "\
Standard_Real LPSPL1 = SPL1->LastParameter();                         \n\
//----------- Extrema TC1 / SPL1  -------------                       \n\
Geom2dAPI_ExtremaCurveCurve ECC2 (TC1,SPL1, FP1,LP1, FPSPL1,LPSPL1);  \n\
Standard_Real SPL1shortestdistance =-1;                               \n\
if (ECC2.NbExtrema()!=0) SPL1shortestdistance = ECC2.LowerDistance(); \n\
Standard_Integer NbExtrema = ECC2.NbExtrema();                        \n\
TColgp_Array2OfPnt2d aSolutionArray(1,NbExtrema,1,2);                 \n\
for(int i=1;i <= NbExtrema; i++)   {                                  \n\
    gp_Pnt2d P1,P2;                                                   \n\
    ECC2.Points(i,P1,P2);                                             \n\
    aSolutionArray(i,1) = P1;  aSolutionArray(i,2) = P2; }            \n";
 AddSeparator(aDoc,Message);
 //--------------------------------------------------------------

 TCollection_AsciiString aString;
 for(int i = array.Lower();i<=array.Upper();i++)
 {
   TCollection_AsciiString Message2 (i);
   gp_Pnt2d P = array(i);
   aString = "array ";
   aString += Message2; 
   DisplayPoint(aDoc,P,aString.ToCString(),false,0.5);
 }

 if (!SPL1.IsNull())
 { 
   Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(SPL1);
   aCurve->SetDisplayPole(Standard_False);
   aCurve->SetColorIndex(3);
   aDoc->GetISessionContext()->Display(aCurve, Standard_False);
 }
 else
   MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"SPL1.IsNull()", L"CasCade Error", MB_ICONERROR);

 Handle(ISession2D_Curve) aCurve1 = new ISession2D_Curve(TC1);
 aCurve1->SetColorIndex(6);
 aDoc->GetISessionContext()->Display(aCurve1, Standard_False);
 Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(TC2);
 aCurve2->SetColorIndex(5);
 aDoc->GetISessionContext()->Display(aCurve2, Standard_False);

 for(int i=1;i <= NbExtrema; i++)
 {
   gp_Pnt2d P1x =aSolutionArray(i,1);

   TCollection_AsciiString Message2 (i);
   aString = "P1_";
   aString += Message2; 
   DisplayPoint(aDoc,P1x,aString.ToCString(),false,0.7*i);

   gp_Pnt2d P2x = aSolutionArray(i,2);

   Handle(Geom2d_TrimmedCurve) SolutionCurve =
     GCE2d_MakeSegment(P1x,P2x);
   Handle(ISession2D_Curve) aSolutionCurve = new ISession2D_Curve(SolutionCurve);
   aDoc->GetISessionContext()->Display(aSolutionCurve, Standard_False);
 }

 Message += "TC1 is  Yellow ,TC2 is  Blue ,SPL1 is Red \n";
 Message += "ECC.NbExtrema()  = ";
 Message += ECC.NbExtrema();
 Message += "    shortestdistance = ";
 Message+= shortestdistance;
 Message += "\n";
 Message += "ECC2.NbExtrema() = ";
 Message += NbExtrema;
 Message += "    SPL1shortestdistance = ";
 Message+= SPL1shortestdistance;
 Message += "\n";

 PostProcess(aDoc,ID_BUTTON_Test_26,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest27
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest27(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  TColgp_Array1OfPnt2d array (1,5); // sizing array
  array.SetValue(1,gp_Pnt2d (-4,0)); array.SetValue(2,gp_Pnt2d (-7,2));
  array.SetValue(3,gp_Pnt2d (-6,3)); array.SetValue(4,gp_Pnt2d (-4,3));
  array.SetValue(5,gp_Pnt2d (-3,5));
  Handle(Geom2d_BSplineCurve) SPL1 = Geom2dAPI_PointsToBSpline(array);

  Standard_Real dist = 1;
  Handle(Geom2d_OffsetCurve) OC =
    new Geom2d_OffsetCurve(SPL1,dist);
  Standard_Boolean result = OC->IsCN(2);
  Standard_Real dist2 = 1.5;
  Handle(Geom2d_OffsetCurve) OC2 =
    new Geom2d_OffsetCurve(SPL1,dist2);
  Standard_Boolean result2 = OC2->IsCN(2);

  //==============================================================
  TCollection_AsciiString Message (" \
                                                                       \n\
TColgp_Array1OfPnt2d array (1,5); // sizing array                      \n\
array.SetValue(1,gp_Pnt2d (-4,0)); array.SetValue(2,gp_Pnt2d (-7,2));  \n\
array.SetValue(3,gp_Pnt2d (-6,3)); array.SetValue(4,gp_Pnt2d (-4,3));  \n\
array.SetValue(5,gp_Pnt2d (-3,5));                                     \n\
Handle(Geom2d_BSplineCurve) SPL1 = Geom2dAPI_PointsToBSpline(array);   \n\
                                                                       \n\
Standard_Real dist = 1;                                                \n\
Handle(Geom2d_OffsetCurve) OC =                                        \n\
       new Geom2d_OffsetCurve(SPL1,dist);                              \n\
Standard_Boolean result = OC->IsCN(2);                                 \n\
                                                                       \n\
Standard_Real dist2 = 1.5;                                             \n\
Handle(Geom2d_OffsetCurve) OC2 =                                       \n\
       new Geom2d_OffsetCurve(SPL1,dist2);                             \n\
Standard_Boolean result2 = OC2->IsCN(2);                               \n\
                                                                       \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------
  Handle(ISession2D_Curve) aCurve1 = new ISession2D_Curve(SPL1);
  aCurve1->SetColorIndex(6);
  aDoc->GetISessionContext()->Display(aCurve1, Standard_False);
  Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(OC);
  aCurve2->SetColorIndex(5);
  aDoc->GetISessionContext()->Display(aCurve2, Standard_False);
  Handle(ISession2D_Curve) aCurve3 = new ISession2D_Curve(OC2);
  aCurve3->SetColorIndex(3);
  aDoc->GetISessionContext()->Display(aCurve3, Standard_False);


  Message += "SPL1 is Yellow \n";
  Message += "OC   is Blue \n";
  Message += "OC2  is Red \n\n";
  Message += "  Warning, Continuity is not guaranteed :  \n ";
  if(result)
    Message += " result  = True  \n";
  else
    Message += " result  = False \n";
  if(result2)
    Message += " result2 = True  \n";
  else
    Message += " result2 = False \n";

  PostProcess(aDoc,ID_BUTTON_Test_27,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest28
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest28(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  gp_Pnt2d P1(1,2);
  gp_Pnt2d P2(4,5);
  gp_Lin2d L = gce_MakeLin2d(P1,P2);
  // assignment by overloaded operator  

  //==============================================================
  TCollection_AsciiString Message (" \
                                      \n\
gp_Pnt2d P1(1,2);                     \n\
gp_Pnt2d P2(4,5);                     \n\
gp_Lin2d L = gce_MakeLin2d(P1,P2);    \n\
// assignment by overloaded operator  \n\
                                      \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplayPoint(aDoc,P1,"P1",false,0.5);
  DisplayPoint(aDoc,P2,"P2",false,0.5);

  Handle(Geom2d_TrimmedCurve) aLine = GCE2d_MakeSegment(L,-3,8);
  Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(aLine);
  aDoc->GetISessionContext()->Display(aCurve, Standard_False);

  PostProcess(aDoc,ID_BUTTON_Test_28,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest29
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest29(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================
                             
  gp_Pnt2d P1(1,2);
  gp_Pnt2d P2(4,5);
  gp_Lin2d L;
  GccAna_Pnt2dBisec B(P1,P2);
  if (B.IsDone())
  {
    L = B.ThisSolution();
  }

  //==============================================================
  TCollection_AsciiString Message (" \
                             \n\
gp_Pnt2d P1(1,2);            \n\
gp_Pnt2d P2(4,5);            \n\
gp_Lin2d L;                  \n\
GccAna_Pnt2dBisec B(P1,P2);  \n\
if (B.IsDone())              \n\
    {                        \n\
    L = B.ThisSolution();    \n\
    }                        \n\
                             \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplayPoint(aDoc,P1,"P1",false,0.5);
  DisplayPoint(aDoc,P2,"P2",false,0.5);

  if (B.IsDone())
  {
    Handle(Geom2d_TrimmedCurve) aLine = GCE2d_MakeSegment(L,-8,8);
    Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(aLine);
    aDoc->GetISessionContext()->Display(aCurve, Standard_False);
  }

  if (B.IsDone()) Message += " \n   B Is Done   ";
  else            Message += " \n   B Is not Done    ";
  PostProcess(aDoc,ID_BUTTON_Test_29,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest30
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest30(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType =a2DNo3D ;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================

  gp_Pnt2d P1 (2,3);
  gp_Pnt2d P2 (4,4);
  gp_Pnt2d P3 (6,7);
  gp_Pnt2d P4 (10,10);
  gp_Circ2d C = gce_MakeCirc2d (P1,P2,P3);
  GccEnt_QualifiedCirc QC = GccEnt::Outside(C);
  GccAna_Lin2d2Tan LT (QC,P4,Precision::Confusion());
  if (LT.IsDone())
  {
    Standard_Integer NbSol = LT.NbSolutions();
    for(Standard_Integer k=1; k<=NbSol; k++)
    {
      gp_Lin2d L = LT.ThisSolution(k);
      // do something with L
    }
  }

  //==============================================================
  TCollection_AsciiString Message (" \
                                                     \n\
gp_Pnt2d P1 (2,3);                                   \n\
gp_Pnt2d P2 (4,4);                                   \n\
gp_Pnt2d P3 (6,7);                                   \n\
gp_Pnt2d P4 (10,10);                                 \n\
gp_Circ2d C = gce_MakeCirc2d (P1,P2,P3);             \n\
GccEnt_QualifiedCirc QC = GccEnt::Outside(C);        \n\
GccAna_Lin2d2Tan LT (QC,P4,Precision::Confusion());  \n\
Standard_Integer NbSol;                              \n\
if (LT.IsDone())                                     \n\
  {                                                  \n\
      NbSol = LT.NbSolutions();                      \n\
      for(Standard_Integer k=1; k<=NbSol; k++)       \n\
        {                                            \n\
         gp_Lin2d L = LT.ThisSolution(k);            \n\
          // do something with L                     \n\
        }                                            \n\
  }                                                  \n\
                                                     \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplayPoint(aDoc,P1,"P1",false,0.5,-1,0.1);
  DisplayPoint(aDoc,P2,"P2",false,0.5,-0.7,0.1);
  DisplayPoint(aDoc,P3,"P3",false,0.5,-0.5,0.1);
  DisplayPoint(aDoc,P4,"P4",false,0.5,0,0.1);

  Handle(Geom2d_Circle) aCircle = new Geom2d_Circle(C);
  Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(aCircle);
  aCurve->SetColorIndex(5);
  aDoc->GetISessionContext()->Display(aCurve, Standard_False);

  if (LT.IsDone())
  {
    Standard_Integer NbSol = LT.NbSolutions();
    for(Standard_Integer k=1; k<=NbSol; k++)
    {
      gp_Lin2d L = LT.ThisSolution(k);
      Handle(Geom2d_TrimmedCurve) aLine = GCE2d_MakeSegment(L,-10,20);
      Handle(ISession2D_Curve) aCurveN = new ISession2D_Curve(aLine);
      aDoc->GetISessionContext()->Display(aCurveN, Standard_False);
    }
  }
  Message += " C is Blue \n\n";
  Message += "LT.IsDone() = "; 
  if (LT.IsDone())  Message += "True \n"; else Message += "False \n";
  TCollection_AsciiString Message2 (LT.NbSolutions());
  Message += "NbSol       = "; Message += Message2      ; Message += "\n";

  PostProcess(aDoc,ID_BUTTON_Test_30,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest31
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest31(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================

gp_Pnt2d P1 (9,6);
gp_Pnt2d P2 (10,4);
gp_Pnt2d P3 (6,7);
gp_Circ2d C = gce_MakeCirc2d (P1,P2,P3);
GccEnt_QualifiedCirc QC = GccEnt::Outside(C);
gp_Pnt2d P4 (-2,7);
gp_Pnt2d P5 (12,-3);
gp_Lin2d L = GccAna_Lin2d2Tan(P4,P5,Precision::Confusion()).ThisSolution(1);
GccEnt_QualifiedLin QL = GccEnt::Unqualified(L);
Standard_Real radius = 2;
GccAna_Circ2d2TanRad TR (QC,QL,radius,Precision::Confusion());
Standard_Real parsol,pararg;
gp_Pnt2d tangentpoint1,tangentpoint2;
gp_Circ2d circ;
if (TR.IsDone())
{
  Standard_Integer NbSol = TR.NbSolutions();
  for (Standard_Integer k=1; k<=NbSol; k++)
  {
    circ = TR.ThisSolution(k);
    // find the solution circle
    TR.Tangency1(k,parsol,pararg,tangentpoint1);
    // find the first tangent point
    TR.Tangency2(k,parsol,pararg,tangentpoint2);
    // find the second tangent point
  }
}

  //==============================================================
  TCollection_AsciiString Message;
  Message = "\
                                                                              \n\
gp_Pnt2d P1 (9,6);                                                            \n\
gp_Pnt2d P2 (10,4);                                                           \n\
gp_Pnt2d P3 (6,7);                                                            \n\
gp_Circ2d C = gce_MakeCirc2d (P1,P2,P3);                                      \n\
GccEnt_QualifiedCirc QC = GccEnt::Outside(C);                                 \n\
gp_Pnt2d P4 (-2,7);                                                           \n\
gp_Pnt2d P5 (12,-3);                                                          \n\
gp_Lin2d L = GccAna_Lin2d2Tan(P4,P5,Precision::Confusion()).ThisSolution(1);  \n\
GccEnt_QualifiedLin QL = GccEnt::Unqualified(L);                              \n\
Standard_Real radius = 2;                                                     \n\
GccAna_Circ2d2TanRad TR (QC,QL,radius,Precision::Confusion());                \n\
Standard_Real parsol,pararg;                                                  \n\
gp_Pnt2d tangentpoint1,tangentpoint2;                                         \n\
gp_Circ2d circ;                                                               \n\
if (TR.IsDone())                                                              \n\
    {                                                                         \n\
      Standard_Integer NbSol = TR.NbSolutions();                              \n\
      for (Standard_Integer k=1; k<=NbSol; k++)                               \n\
        {                                                                     \n";
  Message += "\
          circ = TR.ThisSolution(k);                                          \n\
          // find the solution circle                                         \n\
          TR.Tangency1(k,parsol,pararg,tangentpoint1);                        \n\
          // find the first tangent point                                     \n\
          TR.Tangency2(k,parsol,pararg,tangentpoint2);                        \n\
          // find the second tangent point                                    \n\
        }                                                                     \n\
    }                                                                         \n\
                                                                              \n";
  AddSeparator(aDoc,Message);

  //--------------------------------------------------------------
  DisplayPoint(aDoc,P1,"P1",false,0.3);
  DisplayPoint(aDoc,P2,"P2",false,0.3);
  DisplayPoint(aDoc,P3,"P3",false,0.3);
  DisplayPoint(aDoc,P4,"P4",false,0.3);
  DisplayPoint(aDoc,P5,"P5",false,0.3);

  Handle(Geom2d_Circle) aCircle = new Geom2d_Circle(C);
  Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(aCircle);
  aCurve->SetColorIndex(3);

  aDoc->GetISessionContext()->Display(aCurve, Standard_False);
  Handle(Geom2d_TrimmedCurve) aLine = GCE2d_MakeSegment(L,-2,20);
  Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(aLine);
  aCurve2->SetColorIndex(5);
  aDoc->GetISessionContext()->Display(aCurve2, Standard_False);

  if (TR.IsDone())
  {
    Standard_Integer NbSol = TR.NbSolutions();
    for (Standard_Integer k=1; k<=NbSol; k++)
    {
      circ = TR.ThisSolution(k);
      Handle(Geom2d_Circle) aCircleN = new Geom2d_Circle(circ);
      Handle(ISession2D_Curve) aCurveN = new ISession2D_Curve(aCircleN);
      aDoc->GetISessionContext()->Display(aCurveN, Standard_False);

      // find the solution circle
      TR.Tangency1(k,parsol,pararg,tangentpoint1);
      // find the first tangent point			 			 			 
      TR.Tangency2(k,parsol,pararg,tangentpoint2);
      // find the second tangent point
      DisplayPoint(aDoc,tangentpoint1,"tangentpoint1",false,0.3);
      DisplayPoint(aDoc,tangentpoint2,"tangentpoint2",false,0.3);
    }
  }
  Message += "C is Red \n";
  Message += "L is Blue \n";
  PostProcess(aDoc,ID_BUTTON_Test_31,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest32
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest32(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================

  Standard_Real major = 12;
  Standard_Real minor = 4;
  gp_Ax2d axis = gp::OX2d();
  gp_Elips2d EE(axis,major,minor);
  Handle(Geom2d_TrimmedCurve) arc = GCE2d_MakeArcOfEllipse(EE,0.0,M_PI/4);

  //==============================================================
  TCollection_AsciiString Message (" \
                                                                        \n\
Standard_Real major = 12;                                               \n\
Standard_Real minor = 4;                                                \n\
gp_Ax2d axis = gp::OX2d();                                              \n\
gp_Elips2d EE(axis,major,minor);                                        \n\
Handle(Geom2d_TrimmedCurve) arc = GCE2d_MakeArcOfEllipse(EE,0.0,PI/4);  \n\
                                                                        \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------
  Handle(Geom2d_Ellipse) E = GCE2d_MakeEllipse(EE);
  Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(E);
  aCurve->SetColorIndex(3);
  aCurve->SetTypeOfLine(Aspect_TOL_DOTDASH);
  //SetWidthOfLine                 
  aDoc->GetISessionContext()->Display(aCurve, Standard_False);
  Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(arc);
  aDoc->GetISessionContext()->Display(aCurve2, Standard_False);
  TCollection_AsciiString Message2 (M_PI);
  Message += " PI = ";Message+= Message2;

  PostProcess(aDoc,ID_BUTTON_Test_32,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest33
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest33(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================

  gp_Pnt P1(0,0,1);
  gp_Pnt P2(1,2,2);
  gp_Pnt P3(2,3,3);
  gp_Pnt P4(4,3,4);
  gp_Pnt P5(5,5,5);
  TColgp_Array1OfPnt array (1,5); // sizing array
  array.SetValue(1,P1);
  array.SetValue(2,P2);
  array.SetValue(3,P3);
  array.SetValue(4,P4);
  array.SetValue(5,P5);
  Handle(TColgp_HArray1OfPnt) harray =
    new TColgp_HArray1OfPnt (1,5); // sizing harray
  harray->SetValue(1,P1.Translated(gp_Vec(4,0,0)));
  harray->SetValue(2,P2.Translated(gp_Vec(4,0,0)));
  harray->SetValue(3,P3.Translated(gp_Vec(4,0,0)));
  harray->SetValue(4,P4.Translated(gp_Vec(4,0,0)));
  harray->SetValue(5,P5.Translated(gp_Vec(4,0,0)));
  Handle(Geom_BSplineCurve) SPL1 =
    GeomAPI_PointsToBSpline(array).Curve();

  GeomAPI_Interpolate anInterpolation(harray,Standard_False,Precision::Approximation());
  anInterpolation.Perform();

  Handle(Geom_BSplineCurve) SPL2;
  if (anInterpolation.IsDone())
    SPL2 = anInterpolation.Curve();
  else
    MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"The Interpolation is Not done", L"CasCade Warning", MB_ICONWARNING);

  //==============================================================
  TCollection_AsciiString Message (" \
                                                                                        \n\
gp_Pnt P1(0,0,1);                                                                       \n\
gp_Pnt P2(1,2,2);                                                                       \n\
gp_Pnt P3(2,3,3);                                                                       \n\
gp_Pnt P4(4,3,4);                                                                       \n\
gp_Pnt P5(5,5,5);                                                                       \n\
TColgp_Array1OfPnt array (1,5); // sizing array                                         \n\
array.SetValue(1,P1);                                                                   \n\
array.SetValue(2,P2);                                                                   \n\
array.SetValue(3,P3);                                                                   \n\
array.SetValue(4,P4);                                                                   \n\
array.SetValue(5,P5);                                                                   \n\
Handle(TColgp_HArray1OfPnt) harray =                                                    \n\
    new TColgp_HArray1OfPnt (1,5); // sizing harray                                     \n\
harray->SetValue(1,P1.Translated(gp_Vec(4,0,0)));                                       \n\
harray->SetValue(2,P2.Translated(gp_Vec(4,0,0)));                                       \n\
harray->SetValue(3,P3.Translated(gp_Vec(4,0,0)));                                       \n\
harray->SetValue(4,P4.Translated(gp_Vec(4,0,0)));                                       \n\
harray->SetValue(5,P5.Translated(gp_Vec(4,0,0)));                                       \n\
Handle(Geom_BSplineCurve) SPL1 =                                                        \n\
    GeomAPI_PointsToBSpline(array).Curve();                                             \n");
 Message += "\
                                                                                        \n\
GeomAPI_Interpolate anInterpolation(harray,Standard_False,Precision::Approximation());  \n\
anInterpolation.Perform();                                                              \n\
                                                                                        \n\
Handle(Geom_BSplineCurve) SPL2;                                                         \n\
if (anInterpolation.IsDone())                                                           \n\
       SPL2 = anInterpolation.Curve();                                                  \n\
else                                                                                    \n\
   MessageBox(0,\"The Interpolation is Not done\",\"CasCade Warning\",MB_ICONWARNING);  \n\
                                                                                        \n";
  AddSeparator(aDoc,Message);

  //--------------------------------------------------------------

  TCollection_AsciiString aString;
  for(Standard_Integer i = array.Lower();i<=array.Upper();i++)
  {

    TCollection_AsciiString Message2 (i);
    gp_Pnt P = array(i);

    aString = "P";
    aString += Message2; 
    if (i == 1) aString += " (array)  ";
    DisplayPoint(aDoc,P,aString.ToCString(),false,0.5);\

      aString = "P";
    aString += Message2; 
    if (i == 1) aString += " (harray)  ";
    DisplayPoint(aDoc,P.Translated(gp_Vec(4,0,0)),aString.ToCString(),false,0.5);\

  }

  Handle(ISession_Curve) aCurve = new ISession_Curve(SPL1);
  aDoc->GetAISContext()->SetDisplayMode(aCurve,1, Standard_False);
  aDoc->GetAISContext()->Display(aCurve, Standard_False);

  if (anInterpolation.IsDone())
  {
    Handle(ISession_Curve) aCurve2 = new ISession_Curve(SPL2);
    aDoc->GetAISContext()->SetDisplayMode(aCurve2,1, Standard_False);
    aDoc->GetAISContext()->Display(aCurve2, Standard_False);
  }

  PostProcess(aDoc,ID_BUTTON_Test_33,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest34
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest34(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================

  TColgp_Array1OfPnt array (1,5); // sizing array
  array.SetValue(1,gp_Pnt(0,0,1));
  array.SetValue(2,gp_Pnt(1,2,2));
  array.SetValue(3,gp_Pnt(2,3,3));
  array.SetValue(4,gp_Pnt(4,4,4));
  array.SetValue(5,gp_Pnt(5,5,5));

  GProp_PEquation PE (array,1.5 );

  if (PE.IsPoint())
  {/* ... */}
  gp_Lin L;
  if (PE.IsLinear())
  {
    L = PE.Line();
  }
  if (PE.IsPlanar())
  {/* ... */}
  if (PE.IsSpace())
  {/* ... */}
                                                                 
  //==============================================================
  TCollection_AsciiString Message (" \
                                                \n\
TColgp_Array1OfPnt array (1,5); // sizing array \n\
array.SetValue(1,gp_Pnt(0,0,1));                \n\
array.SetValue(2,gp_Pnt(1,2,2));                \n\
array.SetValue(3,gp_Pnt(2,3,3));                \n\
array.SetValue(4,gp_Pnt(4,4,4));                \n\
array.SetValue(5,gp_Pnt(5,5,5));                \n\
                                                \n\
GProp_PEquation PE (array,1.5 );                \n\
                                                \n\
if (PE.IsPoint()){ /* ... */  }                 \n\
gp_Lin L;                                       \n\
if (PE.IsLinear()) {  L = PE.Line();    }       \n\
if (PE.IsPlanar()){ /* ... */  }                \n\
if (PE.IsSpace()) { /* ... */  }                \n\
                                                \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------
  TCollection_AsciiString aString;
  for(Standard_Integer i = array.Lower();i<=array.Upper();i++)
  {
    TCollection_AsciiString Message2 (i);
    gp_Pnt P = array(i);

    aString = "P";
    aString += Message2; 
    DisplayPoint(aDoc,P,aString.ToCString(),false,0.5);
  }

  Message += " PE.IsPoint()  = ";
  if (PE.IsPoint())
    Message += "True \n";
  else
    Message += "False\n";

  if (PE.IsLinear()) { 
    Message += " PE.IsLinear() = True \n";
    L = PE.Line();
    Handle(Geom_Line) aLine = new Geom_Line(L);
    Handle(Geom_TrimmedCurve) aTrimmedCurve = new Geom_TrimmedCurve(aLine,-10,10);
    Handle(ISession_Curve) aCurve = new ISession_Curve(aTrimmedCurve);
    aDoc->GetAISContext()->Display(aCurve, Standard_False);
  }
  else
    Message += "PE.IsLinear() = False \n";

  Message += " PE.IsPlanar() = ";
  if (PE.IsPlanar())
    Message += "True \n";
  else
    Message += "False\n";

  Message += " PE.IsSpace() = ";
  if (PE.IsSpace())
    Message += "True \n";
  else
    Message += "False\n";

  PostProcess(aDoc,ID_BUTTON_Test_34,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest35
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest35(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================

  gp_Pnt P1(-5,-5,0);
  gp_Pnt P2(9,9,9);
  Handle(Geom_Curve) aCurve = GC_MakeSegment(P1,P2).Value();
  gp_Pnt P3(3,0,0);
  gp_Pnt P4(3,0,10);
  Standard_Real radius1 = 3;
  Standard_Real radius2 = 2;
  Handle(Geom_Surface) aSurface =
      GC_MakeConicalSurface(P3,P4,radius1,radius2).Value();
  GeomAPI_IntCS CS (aCurve,aSurface);
  Handle(Geom_Curve) segment;

  Standard_Integer NbSeg = 0;
  Standard_Integer NbPoints = 0;
  if(CS.IsDone())
  {
    NbSeg = CS.NbSegments();
    for (Standard_Integer k=1; k<=NbSeg; k++)
    {
      segment = CS.Segment(k);
      // do something with the segment
    }

    NbPoints = CS.NbPoints();
    for (int k=1; k<=NbPoints; k++)
    {
      gp_Pnt aPoint=CS.Point(k);
      // do something with the point
    }
  }

  //==============================================================
  TCollection_AsciiString Message (" \
                                                            \n\
gp_Pnt P1(-5,-5,0);                                         \n\
gp_Pnt P2(9,9,9);                                           \n\
Handle(Geom_Curve) aCurve = GC_MakeSegment(P1,P2).Value();  \n\
gp_Pnt P3(3,0,0);                                           \n\
gp_Pnt P4(3,0,10);                                          \n\
Standard_Real radius1 = 3;                                  \n\
Standard_Real radius2 = 2;                                  \n\
Handle(Geom_Surface) aSurface =                             \n\
    GC_MakeConicalSurface(P3,P4,radius1,radius2).Value();   \n\
GeomAPI_IntCS CS (aCurve,aSurface);                         \n\
Handle(Geom_Curve) segment;                                 \n\
                                                            \n\
Standard_Integer NbSeg;                                     \n\
Standard_Integer NbPoints;                                  \n\
if(CS.IsDone())                                             \n\
    {                                                       \n\
      NbSeg = CS.NbSegments();                              \n\
      for (Standard_Integer k=1; k<=NbSeg; k++)             \n\
        {                                                   \n\
          segment = CS.Segment(k);                          \n\
         // do something with the segment                   \n\
        }                                                   \n\
                                                            \n\
      NbPoints = CS.NbPoints();                             \n\
      for (k=1; k<=NbPoints; k++)                           \n\
        {                                                   \n\
          gp_Pnt aPoint=CS.Point(k);                        \n\
          // do something with the point                    \n\
        }                                                   \n\
    }                                                       \n\
                                                            \n");
  AddSeparator(aDoc,Message);

  //--------------------------------------------------------------
 
  Handle(ISession_Curve) aCurve2 = new ISession_Curve(aCurve);
  aDoc->GetAISContext()->Display(aCurve2, Standard_False);

  Handle(Geom_RectangularTrimmedSurface) aTrimmedSurface= new Geom_RectangularTrimmedSurface(aSurface,-50.,50.,false);

  DisplaySurface(aDoc,aTrimmedSurface);

  TCollection_AsciiString aString;
  Standard_Integer k;
  if(CS.IsDone())
  {
    NbSeg = CS.NbSegments();
    for (k=1; k<=NbSeg; k++)
    {
      TCollection_AsciiString Message2 (k);
      segment = CS.Segment(k);
      aString = "S_";aString += Message2;
      Handle(ISession_Curve) aCurveN = new ISession_Curve(segment);
      aDoc->GetAISContext()->Display(aCurveN, Standard_False);
    }

    for ( k=1; k<=NbPoints; k++)
    {
      TCollection_AsciiString Message2 (k);
      gp_Pnt aPoint=CS.Point(k);
      aString = "P_";aString += Message2;
      DisplayPoint(aDoc,aPoint,aString.ToCString(),false,0.5);
      // do something with the point
    }
  }
  TCollection_AsciiString Message2 (NbSeg);
  TCollection_AsciiString Message3 (NbPoints);

  Message += "NbSeg       = "; Message += Message2      ; Message += "\n";
  Message += "NbPoints  = "; Message += Message3      ; Message += "\n";

  PostProcess(aDoc,ID_BUTTON_Test_35,TheDisplayType,Message);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Function name	: GeomSources::gpTest36
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest36(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
//==============================================================

  gp_Pnt centre (5,5,0); gp_Pnt axispoint (9,9,0);
  Standard_Real radius = 3;
  Handle(Geom_Circle) circle =
  GC_MakeCircle(centre,axispoint,radius);

  Handle(Geom_Geometry) aRotatedEntity = circle->Rotated(gp::OZ(),M_PI/4);
  Standard_CString aRotatedEntityTypeName = aRotatedEntity->DynamicType()->Name();

  Handle(Geom_Geometry) aMirroredEntity = aRotatedEntity->Mirrored(gp::ZOX());
  Standard_CString aMirroredEntityTypeName = aMirroredEntity->DynamicType()->Name();

  gp_Pnt scalepoint (4,8,0);
  Standard_Real scalefactor = 0.2;
  Handle(Geom_Geometry) aScaledEntity =
  aMirroredEntity->Scaled(scalepoint, scalefactor);
  Standard_CString aScaledEntityTypeName = aScaledEntity->DynamicType()->Name();

  Handle (Geom_Transformation) GT = GC_MakeTranslation (centre, scalepoint);
  gp_Trsf TR = GT->Trsf();

  gp_Vec aTranslationVector(TR.TranslationPart ());
  Handle(Geom_Geometry) aTranslatedEntity =
  aScaledEntity->Translated( aTranslationVector );
  Standard_CString aTranslatedEntityTypeName = aTranslatedEntity->DynamicType()->Name();

  gp_Mat matrix = TR.HVectorialPart();
  Standard_Real value = matrix.Determinant();

//==============================================================
    TCollection_AsciiString Message (" \
                                                                                        \n\
gp_Pnt centre (5,5,0);  gp_Pnt axispoint (9,9,0);                                       \n\
Standard_Real radius = 3;                                                               \n\
Handle(Geom_Circle) circle =                                                            \n\
    GC_MakeCircle(centre,axispoint,radius);                                             \n\
                                                                                        \n\
Handle(Geom_Geometry) aRotatedEntity  = circle->Rotated(gp::OZ(),PI/4);                 \n\
Standard_CString aRotatedEntityTypeName = aRotatedEntity->DynamicType()->Name();        \n\
                                                                                        \n\
Handle(Geom_Geometry) aMirroredEntity = aRotatedEntity->Mirrored(gp::ZOX());            \n\
Standard_CString aMirroredEntityTypeName = aMirroredEntity->DynamicType()->Name();      \n\
                                                                                        \n\
gp_Pnt scalepoint (4,8,0);                                                              \n\
Standard_Real scalefactor = 0.2;                                                        \n\
Handle(Geom_Geometry) aScaledEntity =                                                   \n\
     aMirroredEntity->Scaled(scalepoint, scalefactor);                                  \n\
Standard_CString aScaledEntityTypeName = aScaledEntity->DynamicType()->Name();          \n\
                                                                                        \n\
Handle (Geom_Transformation) GT =   GC_MakeTranslation  (centre, scalepoint);           \n\
gp_Trsf TR = GT->Trsf();                                                                \n\
                                                                                        \n");
   Message +="\
gp_Vec aTranslationVector(TR.TranslationPart ());                                       \n\
Handle(Geom_Geometry) aTranslatedEntity =                                               \n\
       aScaledEntity->Translated(  aTranslationVector  );                               \n\
Standard_CString aTranslatedEntityTypeName = aTranslatedEntity->DynamicType()->Name();  \n\
                                                                                        \n\
gp_Mat matrix = TR.HVectorialPart();                                                    \n\
Standard_Real value = matrix.Determinant();                                             \n\
                                                                                        \n";
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplayPoint(aDoc,centre,"centre",false,0.5);
  DisplayPoint(aDoc,axispoint,"axispoint",false,0.5);
  DisplayPoint(aDoc,scalepoint,"scalepoint",false,0.5);

  DisplayCurve(aDoc,circle, Quantity_NOC_RED,false);
  DisplayCurve(aDoc,Handle(Geom_Curve)::DownCast(aRotatedEntity),Quantity_NOC_PEACHPUFF, false);
  DisplayCurve(aDoc,Handle(Geom_Curve)::DownCast(aMirroredEntity), Quantity_NOC_YELLOWGREEN,false);
  DisplayCurve(aDoc,Handle(Geom_Curve)::DownCast(aScaledEntity), Quantity_NOC_GREEN,false);
  DisplayCurve(aDoc,Handle(Geom_Curve)::DownCast(aTranslatedEntity),Quantity_NOC_WHITE,false);

  TCollection_AsciiString Message0 (M_PI);
  Message += " PI = ";
  Message+= Message0;
  Message += "\n";
  Message += " circle is Red; aRotatedEntity is Peach; aMirroredEntity is Yellow Green\n";
  Message += " aScaleEntity is Green; aTranslatedEntity is White\n\n";

  TCollection_AsciiString Message2 (aTranslationVector.X());
  TCollection_AsciiString Message3 (aTranslationVector.Y());
  TCollection_AsciiString Message4 (aTranslationVector.Z());
  Message += " aTranslationVector ( ";
  Message += Message2; Message += " , ";
  Message += Message3; Message += " , ";
  Message += Message4; Message += " ); \n";

  TCollection_AsciiString Message5 (value);
  Message += " value = ";Message+= Message5; Message += "\n";

  TCollection_AsciiString Message6 (aRotatedEntityTypeName);
  TCollection_AsciiString Message7 (aMirroredEntityTypeName);
  TCollection_AsciiString Message8 (aScaledEntityTypeName);
  TCollection_AsciiString Message9 (aTranslatedEntityTypeName);

  Message += " aRotatedEntityTypeName = ";Message+= Message6; Message += "\n";
  Message += " aMirroredEntityTypeName = ";Message+= Message7; Message += "\n";
  Message += " aScaledEntityTypeName = ";Message+= Message8; Message += "\n";
  Message += " aTranslatedEntityTypeName = ";Message+= Message9; Message += "\n";

  PostProcess(aDoc,ID_BUTTON_Test_36,TheDisplayType,Message);
}

// Function name	: GeomSources::gpTest37
// Description	    : 
// Return type		: void 
// Argument         : CGeometryDoc* aDoc
void GeomSources::gpTest37(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  TColgp_Array1OfPnt anArrayofPnt (1,5); // sizing array
  anArrayofPnt.SetValue(1,gp_Pnt(0,0,1));
  anArrayofPnt.SetValue(2,gp_Pnt(1,2,2));
  anArrayofPnt.SetValue(3,gp_Pnt(2,3,3));
  anArrayofPnt.SetValue(4,gp_Pnt(4,3,4));
  anArrayofPnt.SetValue(5,gp_Pnt(5,5,5));

  Standard_Real Tolerance = 1;

  gp_Pln P;
  GProp_PEquation PE (anArrayofPnt,Tolerance);
  if (PE.IsPlanar()) { P = PE.Plane();}

  if (PE.IsPoint()) { /* ... */ }
  if (PE.IsLinear()) { /* ... */ }
  if (PE.IsPlanar()) { P = PE.Plane();}
  if (PE.IsSpace()) { /* ... */ }

  //==============================================================
    TCollection_AsciiString Message (" \
                                                                 \n\
TColgp_Array1OfPnt anArrayofPnt (1,5); // sizing array           \n\
anArrayofPnt.SetValue(1,gp_Pnt(0,0,1));                          \n\
anArrayofPnt.SetValue(2,gp_Pnt(1,2,2));                          \n\
anArrayofPnt.SetValue(3,gp_Pnt(2,3,3));                          \n\
anArrayofPnt.SetValue(4,gp_Pnt(4,3,4));                          \n\
anArrayofPnt.SetValue(5,gp_Pnt(5,5,5));                          \n\
                                                                 \n\
Standard_Real Tolerance = 1;                                     \n\
                                                                 \n\
gp_Pln P;                                                        \n\
GProp_PEquation PE (anArrayofPnt,Tolerance);                     \n\
if (PE.IsPlanar()) { P = PE.Plane();}                            \n\
                                                                 \n\
if (PE.IsPoint())  { /* ... */  }                                \n\
if (PE.IsLinear()) { /* ... */  }                                \n\
if (PE.IsPlanar()) { P = PE.Plane();}                            \n\
if (PE.IsSpace())  { /* ... */  }                                \n\
                                                                 \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  TCollection_AsciiString aString;
  for(Standard_Integer i = anArrayofPnt.Lower();i<=anArrayofPnt.Upper();i++){
    TCollection_AsciiString Message2(i);
    gp_Pnt aP = anArrayofPnt(i);
    aString = "P";
    aString += Message2; 
    DisplayPoint(aDoc,aP,aString.ToCString(),false,0.5);
  }

  Message += " PE.IsPoint()  = ";  if (PE.IsPoint()) Message += "True \n";  else Message += "False\n";
  Message += " PE.IsLinear() = ";  if (PE.IsLinear()) Message += "True \n";  else Message += "False\n";

  if (PE.IsPlanar()) { 
    Message +=  " PE.IsPlanar() = True \n";
    P = PE.Plane();
    Handle(Geom_Plane) aPlane = new Geom_Plane(P);
    Handle(Geom_RectangularTrimmedSurface) aSurface= new Geom_RectangularTrimmedSurface(aPlane,-4.,4.,-4.,4.);

    DisplaySurface(aDoc,aSurface);

  }
  else
    Message += " PE.IsPlanar() = False \n";

  Message += " PE.IsSpace() = ";   if (PE.IsSpace() ) Message += "True \n";  else Message += "False\n";

  PostProcess(aDoc,ID_BUTTON_Test_37,TheDisplayType,Message);
}

void GeomSources::gpTest38(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  TColgp_Array1OfPnt array1 (1,5); // sizing array
  array1.SetValue(1,gp_Pnt (-4,0,2 )); array1.SetValue(2,gp_Pnt (-7,2,2 ));
  array1.SetValue(3,gp_Pnt (-6,3,1 )); array1.SetValue(4,gp_Pnt (-4,3,-1));
  array1.SetValue(5,gp_Pnt (-3,5,-2));
  Handle(Geom_BSplineCurve) SPL1 = GeomAPI_PointsToBSpline(array1).Curve();

  TColgp_Array1OfPnt array2 (1,5); // sizing array
  array2.SetValue(1,gp_Pnt (-4,0, 2)); array2.SetValue(2,gp_Pnt (-2,2,0 ));
  array2.SetValue(3,gp_Pnt (2 ,3,-1)); array2.SetValue(4,gp_Pnt (3 ,7,-2));
  array2.SetValue(5,gp_Pnt (4 ,9,-1));
  Handle(Geom_BSplineCurve) SPL2 = GeomAPI_PointsToBSpline(array2).Curve();

  GeomFill_FillingStyle Type = GeomFill_StretchStyle;
  GeomFill_BSplineCurves aGeomFill1(SPL1,SPL2,Type);
  Handle(Geom_BSplineSurface) aBSplineSurface1 = aGeomFill1.Surface();

  Type = GeomFill_CoonsStyle;
  GeomFill_BSplineCurves aGeomFill2(
  Handle(Geom_BSplineCurve)::DownCast(SPL1->Translated(gp_Vec(10,0,0))),
  Handle(Geom_BSplineCurve)::DownCast(SPL2->Translated(gp_Vec(10,0,0))),Type);
  Handle(Geom_BSplineSurface) aBSplineSurface2 = aGeomFill2.Surface();
  Type = GeomFill_CurvedStyle;
  GeomFill_BSplineCurves aGeomFill3(
  Handle(Geom_BSplineCurve)::DownCast(SPL1->Translated(gp_Vec(20,0,0))),
  Handle(Geom_BSplineCurve)::DownCast(SPL2->Translated(gp_Vec(20,0,0))),Type);
  Handle(Geom_BSplineSurface) aBSplineSurface3 = aGeomFill3.Surface();

  //==============================================================
  TCollection_AsciiString Message (" \
                                                                                      \n\
TColgp_Array1OfPnt array1 (1,5); // sizing array                                      \n\
array1.SetValue(1,gp_Pnt (-4,0,2 )); array1.SetValue(2,gp_Pnt (-7,2,2 ));             \n\
array1.SetValue(3,gp_Pnt (-6,3,1 )); array1.SetValue(4,gp_Pnt (-4,3,-1));             \n\
array1.SetValue(5,gp_Pnt (-3,5,-2));                                                  \n\
Handle(Geom_BSplineCurve) SPL1 = GeomAPI_PointsToBSpline(array1).Curve();             \n\
                                                                                      \n\
TColgp_Array1OfPnt array2 (1,5); // sizing array                                      \n\
array2.SetValue(1,gp_Pnt (-4,0, 2)); array2.SetValue(2,gp_Pnt (-2,2,0 ));             \n\
array2.SetValue(3,gp_Pnt (2 ,3,-1)); array2.SetValue(4,gp_Pnt (3 ,7,-2));             \n\
array2.SetValue(5,gp_Pnt (4 ,9,-1));                                                  \n\
Handle(Geom_BSplineCurve) SPL2 = GeomAPI_PointsToBSpline(array2).Curve();             \n\
                                                                                      \n\
GeomFill_FillingStyle Type = GeomFill_StretchStyle;                                   \n\
GeomFill_BSplineCurves aGeomFill1(SPL1,SPL2,Type);                                    \n\
Handle(Geom_BSplineSurface)    aBSplineSurface1 = aGeomFill1.Surface();               \n\
                                                                                      \n\
Type = GeomFill_CoonsStyle;                                                           \n\
GeomFill_BSplineCurves aGeomFill2(                                                    \n");
Message += "\
        Handle(Geom_BSplineCurve)::DownCast(SPL1->Translated(gp_Vec(10,0,0))),        \n\
        Handle(Geom_BSplineCurve)::DownCast(SPL2->Translated(gp_Vec(10,0,0))),Type);  \n\
Handle(Geom_BSplineSurface)    aBSplineSurface2 = aGeomFill2.Surface();               \n\
Type = GeomFill_CurvedStyle;                                                          \n\
GeomFill_BSplineCurves aGeomFill3(                                                    \n\
        Handle(Geom_BSplineCurve)::DownCast(SPL1->Translated(gp_Vec(20,0,0))),        \n\
        Handle(Geom_BSplineCurve)::DownCast(SPL2->Translated(gp_Vec(20,0,0))),Type);  \n\
Handle(Geom_BSplineSurface)    aBSplineSurface3 = aGeomFill3.Surface();               \n\
                                                                                      \n";
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  // Trace des frontieres.  -> FreeBoundaryAspect
  // Trace des isoparametriques.  --> UIsoAspect()

  DisplaySurface(aDoc,aBSplineSurface1,Quantity_NOC_YELLOW);
  DisplaySurface(aDoc,aBSplineSurface2,Quantity_NOC_SALMON);
  DisplaySurface(aDoc,aBSplineSurface3,Quantity_NOC_HOTPINK);

  for (int i=0;i<=2;i++)
  {
    DisplayCurve(aDoc,Handle(Geom_BSplineCurve)::DownCast(SPL1->Translated(gp_Vec(i*10,0,0))), Quantity_NOC_RED,false);
    DisplayCurve(aDoc,Handle(Geom_BSplineCurve)::DownCast(SPL2->Translated(gp_Vec(i*10,0,0))), Quantity_NOC_GREEN,false);
  }

  Message += "SPL1                      is Red; \n";
  Message += "SPL2                      is Green; \n";
  Message += "aBSplineSurface1  is Yellow;       ( GeomFill_StretchStyle )\n";   
  Message += "aBSplineSurface2  is Salmon;     ( GeomFill_CoonsStyle ) \n";
  Message += "aBSplineSurface3  is Hot pink;   ( GeomFill_CurvedStyle ) \n";

  PostProcess(aDoc,ID_BUTTON_Test_38,TheDisplayType,Message);
}
void GeomSources::gpTest39(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  TColgp_Array1OfPnt array1 (1,5); // sizing array
  array1.SetValue(1,gp_Pnt (-4,0,2 ));
  array1.SetValue(2,gp_Pnt (-5,1,0 ));
  array1.SetValue(3,gp_Pnt (-6,2,-2 ));
  array1.SetValue(4,gp_Pnt (-5,4,-7));
  array1.SetValue(5,gp_Pnt (-3,5,-12));

  TColgp_Array1OfPnt array2 (1,5); // sizing array
  array2.SetValue(1,gp_Pnt (-4,0, 2));
  array2.SetValue(2,gp_Pnt (-3,2,1 ));
  array2.SetValue(3,gp_Pnt (-1,5,0));
  array2.SetValue(4,gp_Pnt (2 ,7,-1));
  array2.SetValue(5,gp_Pnt (4 ,9,-1));

  TColgp_Array1OfPnt array3 (1,4); // sizing array
  array3.SetValue(1,gp_Pnt (-3,5, -12));
  array3.SetValue(2,gp_Pnt (-2,6,-7 ));
  array3.SetValue(3,gp_Pnt (0 ,8,-3));
  array3.SetValue(4,gp_Pnt (4 ,9,-1));

  Handle(Geom_BSplineCurve) SPL1 = GeomAPI_PointsToBSpline(array1).Curve();
  Handle(Geom_BSplineCurve) SPL2 = GeomAPI_PointsToBSpline(array2).Curve();
  Handle(Geom_BSplineCurve) SPL3 = GeomAPI_PointsToBSpline(array3).Curve();

  Handle(GeomAdaptor_HCurve) SPL1Adaptor = new GeomAdaptor_HCurve(SPL1);
  Handle(GeomFill_SimpleBound) B1 =
  new GeomFill_SimpleBound(SPL1Adaptor,Precision::Approximation(),Precision::Angular());
  Handle(GeomAdaptor_HCurve) SPL2Adaptor = new GeomAdaptor_HCurve(SPL2);
  Handle(GeomFill_SimpleBound) B2 =
  new GeomFill_SimpleBound(SPL2Adaptor,Precision::Approximation(),Precision::Angular());
  Handle(GeomAdaptor_HCurve) SPL3Adaptor = new GeomAdaptor_HCurve(SPL3);
  Handle(GeomFill_SimpleBound) B3 =
  new GeomFill_SimpleBound(SPL3Adaptor,Precision::Approximation(),Precision::Angular());
  Standard_Boolean NoCheck= Standard_False;

  Standard_Integer MaxDeg = 8;
  Standard_Integer MaxSeg = 2;
  GeomFill_ConstrainedFilling aConstrainedFilling(MaxDeg, MaxSeg);

  aConstrainedFilling.Init(B1,B2,B3,NoCheck);

  Handle(Geom_BSplineSurface) aBSplineSurface = aConstrainedFilling.Surface();

//==============================================================
    TCollection_AsciiString Message (" \
                                                                                           \n\
TColgp_Array1OfPnt array1 (1,5); // sizing array                                           \n\
...                                                                                        \n\
Handle(Geom_BSplineCurve) SPL1 = GeomAPI_PointsToBSpline(array1).Curve();                  \n\
Handle(Geom_BSplineCurve) SPL2 = GeomAPI_PointsToBSpline(array2).Curve();                  \n\
Handle(Geom_BSplineCurve) SPL3 = GeomAPI_PointsToBSpline(array3).Curve();                  \n\
                                                                                           \n\
Handle(GeomAdaptor_HCurve) SPL1Adaptor = new GeomAdaptor_HCurve(SPL1);                     \n\
Handle(GeomFill_SimpleBound) B1 =                                                          \n\
   new GeomFill_SimpleBound(SPL1Adaptor,Precision::Approximation(),Precision::Angular());  \n\
Handle(GeomAdaptor_HCurve) SPL2Adaptor = new GeomAdaptor_HCurve(SPL2);                     \n\
Handle(GeomFill_SimpleBound) B2 =                                                          \n\
   new GeomFill_SimpleBound(SPL2Adaptor,Precision::Approximation(),Precision::Angular());  \n\
Handle(GeomAdaptor_HCurve) SPL3Adaptor = new GeomAdaptor_HCurve(SPL3);                     \n\
Handle(GeomFill_SimpleBound) B3 =                                                          \n\
   new GeomFill_SimpleBound(SPL3Adaptor,Precision::Approximation(),Precision::Angular());  \n\
Standard_Boolean NoCheck= Standard_False;                                                  \n\
                                                                                           \n\
Standard_Integer MaxDeg = 8;                                                               \n");
Message += "\
Standard_Integer MaxSeg = 2;                                                               \n\
GeomFill_ConstrainedFilling aConstrainedFilling(MaxDeg, MaxSeg);                           \n\
                                                                                           \n\
aConstrainedFilling.Init(B1,B2,B3,NoCheck);                                                \n\
                                                                                           \n\
Handle(Geom_BSplineSurface) aBSplineSurface = aConstrainedFilling.Surface();               \n\
                                                                                           \n";
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  TCollection_AsciiString aString;

  DisplaySurface(aDoc,aBSplineSurface,Quantity_NOC_YELLOW);
  DisplayCurve(aDoc,SPL1,Quantity_NOC_RED ,false);
  DisplayCurve(aDoc,SPL2,Quantity_NOC_GREEN ,false);
  DisplayCurve(aDoc,SPL3,Quantity_NOC_BLUE1 ,false);

  Message += "SPL1                      is Red; \n";
  Message += "SPL2                      is Green; \n";
  Message += "SPL3                      is Blue; \n";

  Message += "aBSplineSurface  is Yellow; \n";   

  PostProcess(aDoc,ID_BUTTON_Test_39,TheDisplayType,Message);
}

void GeomSources::gpTest40(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  TColgp_Array1OfPnt array1 (1,5); // sizing array
  array1.SetValue(1,gp_Pnt (-4,0,2 ));
  array1.SetValue(2,gp_Pnt (-5,1,0 ));
  array1.SetValue(3,gp_Pnt (-6,2,-2 ));
  array1.SetValue(4,gp_Pnt (-5,4,-7));
  array1.SetValue(5,gp_Pnt (-3,5,-12));

  Handle(Geom_Curve) SPL1 =
  GeomAPI_PointsToBSpline(array1).Curve();

  GeomFill_Pipe aPipe(SPL1,1);
  aPipe.Perform();
  Handle(Geom_Surface) aSurface= aPipe.Surface();
  Standard_CString aSurfaceEntityTypeName="Not Computed";
  if (!aSurface.IsNull())
  aSurfaceEntityTypeName = aSurface->DynamicType()->Name();

  Handle(Geom_Ellipse) E = GC_MakeEllipse( gp::XOY() ,3,1).Value();
  GeomFill_Pipe aPipe2(SPL1,E);
  aPipe2.Perform();
  Handle(Geom_Surface) aSurface2= aPipe2.Surface();
  Standard_CString aSurfaceEntityTypeName2="Not Computed";
  if (!aSurface2.IsNull()) {
  aSurfaceEntityTypeName2 = aSurface2->DynamicType()->Name();
  aSurface2->Translate(gp_Vec(5,0,0)); }

  Handle(Geom_TrimmedCurve) TC1 =
  GC_MakeSegment(gp_Pnt(1,1,1),gp_Pnt(5,5,5));
  Handle(Geom_TrimmedCurve) TC2 =
  GC_MakeSegment(gp_Pnt(1,1,0),gp_Pnt(4,5,6));
  GeomFill_Pipe aPipe3(SPL1,TC1,TC2);
  aPipe3.Perform();
  Handle(Geom_Surface) aSurface3 = aPipe3.Surface();
  Standard_CString aSurfaceEntityTypeName3="Not Computed";
  if (!aSurface3.IsNull())
  {
  aSurfaceEntityTypeName3 = aSurface3->DynamicType()->Name();
  aSurface3->Translate(gp_Vec(10,0,0));
  }

  //==============================================================
    TCollection_AsciiString Message (" \
                                                                   \n\
                                                                   \n\
TColgp_Array1OfPnt array1 (1,5); // sizing array                   \n\
array1.SetValue(1,gp_Pnt (-4,0,2 ));                               \n\
array1.SetValue(2,gp_Pnt (-5,1,0 ));                               \n\
array1.SetValue(3,gp_Pnt (-6,2,-2 ));                              \n\
array1.SetValue(4,gp_Pnt (-5,4,-7));                               \n\
array1.SetValue(5,gp_Pnt (-3,5,-12));                              \n\
                                                                   \n\
Handle(Geom_BSplineCurve) SPL1 =                                   \n\
    GeomAPI_PointsToBSpline(array1).Curve();                       \n\
                                                                   \n\
GeomFill_Pipe aPipe(SPL1,1);                                       \n\
aPipe.Perform();                                                   \n\
Handle(Geom_Surface) aSurface= aPipe.Surface();                    \n\
Standard_CString aSurfaceEntityTypeName=\"Not Computed\";            \n\
if (!aSurface.IsNull())                                            \n\
   aSurfaceEntityTypeName = aSurface->DynamicType()->Name();       \n\
                                                                   \n\
Handle(Geom_Ellipse) E = GC_MakeEllipse( gp::XOY() ,3,1).Value();  \n\
GeomFill_Pipe aPipe2(SPL1,E);                                      \n\
aPipe2.Perform();                                                  \n");
Message += "\
Handle(Geom_Surface) aSurface2= aPipe2.Surface();                  \n\
Standard_CString aSurfaceEntityTypeName2=\"Not Computed\";           \n\
if (!aSurface2.IsNull())  {                                        \n\
    aSurfaceEntityTypeName2 = aSurface2->DynamicType()->Name();    \n\
    aSurface2->Translate(gp_Vec(5,0,0));  }                        \n\
                                                                   \n\
Handle(Geom_TrimmedCurve) TC1 =                                    \n\
    GC_MakeSegment(gp_Pnt(1,1,1),gp_Pnt(5,5,5));                   \n\
Handle(Geom_TrimmedCurve) TC2 =                                    \n\
    GC_MakeSegment(gp_Pnt(1,1,0),gp_Pnt(4,5,6));                   \n\
GeomFill_Pipe aPipe3(SPL1,TC1,TC2);                                \n\
aPipe3.Perform();                                                  \n\
Handle(Geom_Surface) aSurface3 = aPipe3.Surface();                 \n\
Standard_CString aSurfaceEntityTypeName3=\"Not Computed\";           \n\
if (!aSurface3.IsNull())                                           \n\
  {                                                                \n\
    aSurfaceEntityTypeName3 = aSurface3->DynamicType()->Name();    \n\
        aSurface3->Translate(gp_Vec(10,0,0));                      \n\
  }                                                                \n\
                                                                 \n";
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  if (!aSurface.IsNull())
  {
    DisplaySurface(aDoc,aSurface,Quantity_NOC_YELLOW);
  }
  if (!aSurface2.IsNull())
  {
    DisplaySurface(aDoc,aSurface2,Quantity_NOC_YELLOW);
  }
  if (!aSurface3.IsNull())
  {
    DisplaySurface(aDoc,aSurface3,Quantity_NOC_YELLOW);
  }

  DisplayCurve(aDoc,SPL1,Quantity_NOC_RED ,false);

  Message += "SPL1                      is Red; \n";


  TCollection_AsciiString Message2(aSurfaceEntityTypeName);
  TCollection_AsciiString Message3(aSurfaceEntityTypeName2);
  TCollection_AsciiString Message4(aSurfaceEntityTypeName3);

  Message += " aSurfaceEntityTypeName     = ";Message+= Message2; Message += "\n";
  Message += " aSurfaceEntityTypeName2     = ";Message+= Message3; Message += "\n";
  Message += " aSurfaceEntityTypeName3     = ";Message+= Message4; Message += "\n";

  PostProcess(aDoc,ID_BUTTON_Test_40,TheDisplayType,Message);
}

void GeomSources::gpTest41(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  TColgp_Array1OfPnt array1 (1,5);
  array1.SetValue(1,gp_Pnt (-4,0,2 ));
  array1.SetValue(2,gp_Pnt (-5,1,0 ));
  array1.SetValue(3,gp_Pnt (-6,2,-2 ));
  array1.SetValue(4,gp_Pnt (-5,4,-7));
  array1.SetValue(5,gp_Pnt (-3,5,-12));

  Handle(Geom_BSplineCurve) SPL1 =
    GeomAPI_PointsToBSpline(array1).Curve();
  Handle(Geom_Curve) FirstSect =
    GC_MakeSegment(gp_Pnt(-4,0,2),gp_Pnt(-4,0,10)).Value();

  GeomFill_Pipe aPipe(SPL1,FirstSect);
  aPipe.Perform();

  Handle(Geom_BSplineSurface) aPipeSurface =
    Handle(Geom_BSplineSurface)::DownCast(aPipe.Surface());
  Handle(Geom_BSplineSurface) anotherBSplineSurface =
    GeomConvert::SplitBSplineSurface(aPipeSurface,1,2,3,6);

  //==============================================================
  TCollection_AsciiString Message (" \
                                                                   \n\
TColgp_Array1OfPnt array1 (1,5);                                   \n\
array1.SetValue(1,gp_Pnt (-4,0,2 ));                               \n\
array1.SetValue(2,gp_Pnt (-5,1,0 ));                               \n\
array1.SetValue(3,gp_Pnt (-6,2,-2 ));                              \n\
array1.SetValue(4,gp_Pnt (-5,4,-7));                               \n\
array1.SetValue(5,gp_Pnt (-3,5,-12));                              \n\
                                                                   \n\
Handle(Geom_BSplineCurve) SPL1 =                                   \n\
    GeomAPI_PointsToBSpline(array1).Curve();                       \n\
Handle(Geom_Curve) FirstSect =                                     \n\
    GC_MakeSegment(gp_Pnt(-4,0,2),gp_Pnt(-4,0,10)).Value();        \n\
                                                                   \n\
GeomFill_Pipe aPipe(SPL1,FirstSect);                               \n\
aPipe.Perform();                                                   \n\
                                                                   \n\
Handle(Geom_BSplineSurface) aPipeSurface =                         \n\
    Handle(Geom_BSplineSurface)::DownCast(aPipe.Surface());        \n\
Handle(Geom_BSplineSurface) anotherBSplineSurface =                \n\
    GeomConvert::SplitBSplineSurface(aPipeSurface,1,2,3,6);        \n\
                                                                   \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  if(!aPipeSurface.IsNull())
  {
    DisplaySurface(aDoc,aPipeSurface,Quantity_NOC_YELLOW);
  }

  if(!anotherBSplineSurface.IsNull())
  {
    DisplaySurface(aDoc,anotherBSplineSurface,Quantity_NOC_HOTPINK);
  }

  DisplayCurve(aDoc,SPL1,Quantity_NOC_RED ,false);
  DisplayCurve(aDoc,FirstSect,Quantity_NOC_GREEN ,false);

  Message += "SPL1                              is Red; \n";
  Message += "SPL2                              is Green; \n";
  Message += "SPL3                              is Blue; \n";
  Message += "aBSplineSurface            is Yellow; \n";   
  Message += "anotherBSplineSurface  is Hot Pink; \n";   

  PostProcess(aDoc,ID_BUTTON_Test_41,TheDisplayType,Message);
}


void GeomSources::gpTest42(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================

  TColgp_Array2OfPnt array1(1,3,1,3);
  TColgp_Array2OfPnt array2(1,3,1,3);
  TColgp_Array2OfPnt array3(1,3,1,3);
  TColgp_Array2OfPnt array4(1,3,1,3);

  array1.SetValue(1,1,gp_Pnt(1,1,1));
  array1.SetValue(1,2,gp_Pnt(2,1,2));
  array1.SetValue(1,3,gp_Pnt(3,1,1));
  array1.SetValue(2,1,gp_Pnt(1,2,1));
  array1.SetValue(2,2,gp_Pnt(2,2,2));
  array1.SetValue(2,3,gp_Pnt(3,2,0));
  array1.SetValue(3,1,gp_Pnt(1,3,2));
  array1.SetValue(3,2,gp_Pnt(2,3,1));
  array1.SetValue(3,3,gp_Pnt(3,3,0));

  array2.SetValue(1,1,gp_Pnt(3,1,1));
  array2.SetValue(1,2,gp_Pnt(4,1,1));
  array2.SetValue(1,3,gp_Pnt(5,1,2));
  array2.SetValue(2,1,gp_Pnt(3,2,0));
  array2.SetValue(2,2,gp_Pnt(4,2,1));
  array2.SetValue(2,3,gp_Pnt(5,2,2));
  array2.SetValue(3,1,gp_Pnt(3,3,0));
  array2.SetValue(3,2,gp_Pnt(4,3,0));
  array2.SetValue(3,3,gp_Pnt(5,3,1));

  array3.SetValue(1,1,gp_Pnt(1,3,2));
  array3.SetValue(1,2,gp_Pnt(2,3,1));
  array3.SetValue(1,3,gp_Pnt(3,3,0));
  array3.SetValue(2,1,gp_Pnt(1,4,1));
  array3.SetValue(2,2,gp_Pnt(2,4,0));
  array3.SetValue(2,3,gp_Pnt(3,4,1));
  array3.SetValue(3,1,gp_Pnt(1,5,1));
  array3.SetValue(3,2,gp_Pnt(2,5,1));
  array3.SetValue(3,3,gp_Pnt(3,5,2));

  array4.SetValue(1,1,gp_Pnt(3,3,0));
  array4.SetValue(1,2,gp_Pnt(4,3,0));
  array4.SetValue(1,3,gp_Pnt(5,3,1));
  array4.SetValue(2,1,gp_Pnt(3,4,1));
  array4.SetValue(2,2,gp_Pnt(4,4,1));
  array4.SetValue(2,3,gp_Pnt(5,4,1));
  array4.SetValue(3,1,gp_Pnt(3,5,2));
  array4.SetValue(3,2,gp_Pnt(4,5,2));
  array4.SetValue(3,3,gp_Pnt(5,5,1));

  Handle(Geom_BezierSurface) BZ1 =
    new Geom_BezierSurface(array1);
  Handle(Geom_BezierSurface) BZ2 =
    new Geom_BezierSurface(array2);
  Handle(Geom_BezierSurface) BZ3 =
    new Geom_BezierSurface(array3);
  Handle(Geom_BezierSurface) BZ4 =
    new Geom_BezierSurface(array4);

  TColGeom_Array2OfBezierSurface bezierarray(1,2,1,2);
  bezierarray.SetValue(1,1,BZ1);
  bezierarray.SetValue(1,2,BZ2);
  bezierarray.SetValue(2,1,BZ3);
  bezierarray.SetValue(2,2,BZ4);

  GeomConvert_CompBezierSurfacesToBSplineSurface BB (bezierarray);
  Handle(Geom_BSplineSurface) BSPLSURF ;
  if (BB.IsDone())
  {
    BSPLSURF = new Geom_BSplineSurface(
      BB.Poles()->Array2(),
      BB.UKnots()->Array1(),
      BB.VKnots()->Array1(),
      BB.UMultiplicities()->Array1(),
      BB.VMultiplicities()->Array1(),
      BB.UDegree(),
      BB.VDegree() );
    BSPLSURF->Translate(gp_Vec(0,0,2));
  }

  //==============================================================

  TCollection_AsciiString Message (" \
                                                                  \n\
TColgp_Array2OfPnt array1(1,3,1,3);                               \n\
TColgp_Array2OfPnt array2(1,3,1,3);                               \n\
TColgp_Array2OfPnt array3(1,3,1,3);                               \n\
TColgp_Array2OfPnt array4(1,3,1,3);                               \n\
                                                                  \n\
// array1.SetValue(  ...                                          \n\
                                                                  \n\
Handle(Geom_BezierSurface) BZ1 =                                  \n\
    new Geom_BezierSurface(array1);                               \n\
Handle(Geom_BezierSurface) BZ2 =                                  \n\
    new Geom_BezierSurface(array2);                               \n\
Handle(Geom_BezierSurface) BZ3 =                                  \n\
    new Geom_BezierSurface(array3);                               \n\
Handle(Geom_BezierSurface) BZ4 =                                  \n\
    new Geom_BezierSurface(array4);                               \n\
                                                                  \n\
TColGeom_Array2OfBezierSurface bezierarray(1,2,1,2);              \n\
bezierarray.SetValue(1,1,BZ1);                                    \n\
bezierarray.SetValue(1,2,BZ2);                                    \n\
bezierarray.SetValue(2,1,BZ3);                                    \n\
bezierarray.SetValue(2,2,BZ4);                                    \n\
                                                                  \n\
GeomConvert_CompBezierSurfacesToBSplineSurface BB (bezierarray);  \n\
   Handle(Geom_BSplineSurface) BSPLSURF ;                         \n\
if (BB.IsDone()){                                                 \n\
   BSPLSURF = new Geom_BSplineSurface(                            \n\
        BB.Poles()->Array2(),                                     \n\
        BB.UKnots()->Array1(),                                    \n");
        Message += "\
        BB.VKnots()->Array1(),                                    \n\
        BB.UMultiplicities()->Array1(),                           \n\
        BB.VMultiplicities()->Array1(),                           \n\
        BB.UDegree(),                                             \n\
        BB.VDegree() );                                           \n\
    BSPLSURF->Translate(gp_Vec(0,0,2));                           \n\
    }                                                             \n\
                                                                  \n";
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplaySurface(aDoc,BZ1,Quantity_NOC_RED);
  DisplaySurface(aDoc,BZ2,Quantity_NOC_GREEN);
  DisplaySurface(aDoc,BZ3,Quantity_NOC_BLUE1);
  DisplaySurface(aDoc,BZ4,Quantity_NOC_BROWN);

  if (BB.IsDone()){
    DisplaySurface(aDoc,BSPLSURF,Quantity_NOC_HOTPINK);
  }

  Message += "BZ1 is Red; \n";
  Message += "BZ2 is Green; \n";
  Message += "BZ3 is Blue; \n";
  Message += "BZ4 is Brown; \n";
  Message += "BSPLSURF is Hot Pink; \n";

  PostProcess(aDoc,ID_BUTTON_Test_42,TheDisplayType,Message);
}

void GeomSources::gpTest43(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  TColgp_Array1OfPnt array1 (1,5);
  array1.SetValue(1,gp_Pnt (-4,5,5 ));
  array1.SetValue(2,gp_Pnt (-3,6,6 ));
  array1.SetValue(3,gp_Pnt (-1,7,7 ));
  array1.SetValue(4,gp_Pnt (0,8,8));
  array1.SetValue(5,gp_Pnt (2,9,9));
  Handle(Geom_BSplineCurve) SPL1 =
    GeomAPI_PointsToBSpline(array1).Curve();

  TColgp_Array1OfPnt array2 (1,5);
  array2.SetValue(1,gp_Pnt (-4,5,2 ));
  array2.SetValue(2,gp_Pnt (-3,6,3 ));
  array2.SetValue(3,gp_Pnt (-1,7,4 ));
  array2.SetValue(4,gp_Pnt (0,8,5));
  array2.SetValue(5,gp_Pnt (2,9,6));
  Handle(Geom_BSplineCurve) SPL2 =
    GeomAPI_PointsToBSpline(array2).Curve();

  GeomFill_FillingStyle Type = GeomFill_StretchStyle;
  GeomFill_BSplineCurves aGeomFill1(SPL1,SPL2,Type);
  Handle(Geom_BSplineSurface) aGeomSurface = aGeomFill1.Surface();
  Standard_Real offset = 1;
  Handle(Geom_OffsetSurface) GOS = new Geom_OffsetSurface(aGeomSurface, offset);
  offset = 2;
  Handle(Geom_OffsetSurface) GOS1 = new Geom_OffsetSurface(aGeomSurface, offset);
  offset = 3;
  Handle(Geom_OffsetSurface) GOS2 = new Geom_OffsetSurface(aGeomSurface, offset);

  //==============================================================
  TCollection_AsciiString Message (" \
                                                                                 \n\
TColgp_Array1OfPnt array1 (1,5);                                                 \n\
//array1.SetValue( ...                                                           \n\
Handle(Geom_BSplineCurve) SPL1 =                                                 \n\
	GeomAPI_PointsToBSpline(array1).Curve();                                     \n\
                                                                                 \n\
TColgp_Array1OfPnt array2 (1,5);                                                 \n\
// array2.SetValue( ...                                                          \n\
                                                                                 \n\
Handle(Geom_BSplineCurve) SPL2 =                                                 \n\
	GeomAPI_PointsToBSpline(array2).Curve();                                     \n\
                                                                                 \n\
GeomFill_FillingStyle Type = GeomFill_StretchStyle;                              \n\
GeomFill_BSplineCurves aGeomFill1(SPL1,SPL2,Type);                               \n\
Handle(Geom_BSplineSurface) aGeomSurface = aGeomFill1.Surface();                 \n\
Standard_Real offset = 1;                                                        \n\
Handle(Geom_OffsetSurface) GOS = new Geom_OffsetSurface(aGeomSurface, offset);   \n\
 offset = 2;                                                                     \n\
Handle(Geom_OffsetSurface) GOS1 = new Geom_OffsetSurface(aGeomSurface, offset);  \n\
offset = 3;                                                                      \n\
Handle(Geom_OffsetSurface) GOS2 = new Geom_OffsetSurface(aGeomSurface, offset);  \n\
                                                                                 \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplaySurface(aDoc,aGeomSurface,Quantity_NOC_BLUE1);
  DisplaySurface(aDoc,GOS,Quantity_NOC_GREEN);
  DisplaySurface(aDoc,GOS1,Quantity_NOC_GREEN);
  DisplaySurface(aDoc,GOS2,Quantity_NOC_GREEN);

  DisplayCurve(aDoc,SPL1,Quantity_NOC_RED ,false);
  DisplayCurve(aDoc,SPL2,Quantity_NOC_HOTPINK ,false);

  Message += "aGeomSurface  is Blue; \n";
  Message += "GOS              are Green; \n";

  PostProcess(aDoc,ID_BUTTON_Test_43,TheDisplayType,Message);
}

void GeomSources::gpTest44(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  gp_Pnt P1(0,0,1);
  gp_Pnt P2(1,2,2);
  gp_Pnt P3(2,3,3);
  gp_Pnt P4(4,3,4);
  gp_Pnt P5(5,5,5);
  TColgp_Array1OfPnt array (1,5);
  array.SetValue(1,P1);
  array.SetValue(2,P2);
  array.SetValue(3,P3);
  array.SetValue(4,P4);
  array.SetValue(5,P5);
  Handle(Geom_BSplineCurve) aCurve =
    GeomAPI_PointsToBSpline(array).Curve();
  gp_Dir aDir(1,2,3);
  Handle(Geom_SurfaceOfLinearExtrusion) SOLE =
    new Geom_SurfaceOfLinearExtrusion(aCurve,aDir);

  Handle(Geom_RectangularTrimmedSurface) aTrimmedSurface =
    new Geom_RectangularTrimmedSurface(SOLE,-10,10,false);

  Standard_CString SOLEEntityTypeName="Not Computed";
  if (!SOLE.IsNull())
  {
    SOLEEntityTypeName = SOLE->DynamicType()->Name();
  }

  //==============================================================

  TCollection_AsciiString Message (" \
                                                           \n\
gp_Pnt P1(0,0,1);                                          \n\
gp_Pnt P2(1,2,2);                                          \n\
gp_Pnt P3(2,3,3);                                          \n\
gp_Pnt P4(4,3,4);                                          \n\
gp_Pnt P5(5,5,5);                                          \n\
TColgp_Array1OfPnt array (1,5);                            \n\
array.SetValue(1,P1);                                      \n\
array.SetValue(2,P2);                                      \n\
array.SetValue(3,P3);                                      \n\
array.SetValue(4,P4);                                      \n\
array.SetValue(5,P5);                                      \n\
Handle(Geom_BSplineCurve) aCurve =                         \n\
	GeomAPI_PointsToBSpline(array).Curve();                \n\
gp_Dir aDir(1,2,3);                                        \n\
Handle(Geom_SurfaceOfLinearExtrusion) SOLE =               \n\
	new Geom_SurfaceOfLinearExtrusion(aCurve,aDir);        \n\
                                                           \n\
Handle(Geom_RectangularTrimmedSurface) aTrimmedSurface =   \n\
   new Geom_RectangularTrimmedSurface(SOLE,-10,10,false);  \n\
                                                           \n\
Standard_CString SOLEEntityTypeName=\"Not Computed\";        \n\
if (!SOLE.IsNull())                                        \n\
  {                                                        \n\
    SOLEEntityTypeName = SOLE->DynamicType()->Name();      \n\
  }                                                        \n\
                                                           \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplaySurface(aDoc,aTrimmedSurface,Quantity_NOC_GREEN);
  DisplayCurve(aDoc,aCurve,Quantity_NOC_RED ,false);

  Message += "aCurve   is Red; \n";
  Message += "aTrimmedSurface       is Green; \n";

  TCollection_AsciiString Message2 (SOLEEntityTypeName);

  Message += " SOLEEntityTypeName     = ";Message+= Message2; Message += "\n";

  PostProcess(aDoc,ID_BUTTON_Test_44,TheDisplayType,Message);
}

void GeomSources::gpTest45(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================

  TColgp_Array1OfPnt array (1,5);
  array.SetValue(1,gp_Pnt (0,0,1));
  array.SetValue(2,gp_Pnt (1,2,2));
  array.SetValue(3,gp_Pnt (2,3,3));
  array.SetValue(4,gp_Pnt (4,3,4));
  array.SetValue(5,gp_Pnt (5,5,5));
  Handle(Geom_BSplineCurve) aCurve =
    GeomAPI_PointsToBSpline(array).Curve();
  Handle(Geom_SurfaceOfRevolution) SOR =
    new Geom_SurfaceOfRevolution(aCurve,gp::OX());

  Standard_CString SOREntityTypeName="Not Computed";
  if (!SOR.IsNull())
    SOREntityTypeName = SOR->DynamicType()->Name();

  //==============================================================
  TCollection_AsciiString Message (" \
                                                     \n\
TColgp_Array1OfPnt array (1,5);                      \n\
array.SetValue(1,gp_Pnt 0,0,1));                     \n\
array.SetValue(2,gp_Pnt (1,2,2));                    \n\
array.SetValue(3,gp_Pnt (2,3,3));                    \n\
array.SetValue(4,gp_Pnt (4,3,4));                    \n\
array.SetValue(5,gp_Pnt (5,5,5));                    \n\
Handle(Geom_BSplineCurve) aCurve =                   \n\
    GeomAPI_PointsToBSpline(array).Curve();          \n\
Handle(Geom_SurfaceOfRevolution) SOR =               \n\
    new Geom_SurfaceOfRevolution(aCurve,gp::OX());   \n\
                                                     \n\
Standard_CString SOREntityTypeName=\"Not Computed\";   \n\
if (!SOR.IsNull())                                   \n\
    SOREntityTypeName = SOR->DynamicType()->Name();  \n\
                                                     \n");
  AddSeparator(aDoc,Message);

  //--------------------------------------------------------------
  DisplaySurface(aDoc,SOR,Quantity_NOC_GREEN);
  DisplayCurve(aDoc,aCurve,Quantity_NOC_RED ,false);

  Message += "aCurve   is Red; \n";
  Message += "SOR       is Green; \n";
  TCollection_AsciiString Message2 (SOREntityTypeName);
  Message += " SOREntityTypeName     = ";Message+= Message2; Message += "\n";

  PostProcess(aDoc,ID_BUTTON_Test_45,TheDisplayType,Message);
}

void GeomSources::gpTest46(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  TColgp_Array1OfPnt array1 (1,5);
  array1.SetValue(1,gp_Pnt (-4,5,5 ));
  array1.SetValue(2,gp_Pnt (-3,6,6 ));
  array1.SetValue(3,gp_Pnt (-1,6,7 ));
  array1.SetValue(4,gp_Pnt (0,8,8));
  array1.SetValue(5,gp_Pnt (2,9,9));
  Handle(Geom_BSplineCurve) SPL1 =
    GeomAPI_PointsToBSpline(array1).Curve();

  TColgp_Array1OfPnt array2 (1,5);
  array2.SetValue(1,gp_Pnt (-4,5,2 ));
  array2.SetValue(2,gp_Pnt (-3,6,3 ));
  array2.SetValue(3,gp_Pnt (-1,7,4 ));
  array2.SetValue(4,gp_Pnt (0,8,5));
  array2.SetValue(5,gp_Pnt (2,9,6));
  Handle(Geom_BSplineCurve) SPL2 =
    GeomAPI_PointsToBSpline(array2).Curve();

  GeomFill_FillingStyle Type = GeomFill_StretchStyle;
  GeomFill_BSplineCurves aGeomFill1(SPL1,SPL2,Type);
  Handle(Geom_BSplineSurface) aGeomSurface = aGeomFill1.Surface();

  Handle(Geom_BoundedSurface) aTranslatedGeomSurface =
    Handle(Geom_BoundedSurface)::DownCast(aGeomSurface->Copy());

  Standard_Real extension = 3;
  Standard_Integer continuity = 2;
  Standard_Boolean Udirection = Standard_True;
  Standard_Boolean after = Standard_True;
  GeomLib::ExtendSurfByLength (aTranslatedGeomSurface,
    extension,continuity,Udirection,after);

  //==============================================================
    TCollection_AsciiString Message (" \
                                                                  \n\
TColgp_Array1OfPnt array1 (1,5);                                  \n\
// ...                                                            \n\
Handle(Geom_BSplineCurve) SPL1 =                                  \n\
	GeomAPI_PointsToBSpline(array1).Curve();                      \n\
                                                                  \n\
TColgp_Array1OfPnt array2 (1,5);                                  \n\
// ...                                                            \n\
Handle(Geom_BSplineCurve) SPL2 =                                  \n\
	GeomAPI_PointsToBSpline(array2).Curve();                      \n\
                                                                  \n\
GeomFill_FillingStyle Type = GeomFill_StretchStyle;               \n\
GeomFill_BSplineCurves aGeomFill1(SPL1,SPL2,Type);                \n\
Handle(Geom_BSplineSurface) aGeomSurface = aGeomFill1.Surface();  \n\
                                                                  \n\
Handle(Geom_BoundedSurface) aTranslatedGeomSurface =              \n\
   Handle(Geom_BoundedSurface)::DownCast(aGeomSurface->Copy());   \n\
                                                                  \n\
Standard_Real extension = 3;                                      \n\
Standard_Integer continuity = 2;                                  \n\
Standard_Boolean Udirection = Standard_True;                      \n\
Standard_Boolean after = Standard_True;                           \n\
GeomLib::ExtendSurfByLength (aTranslatedGeomSurface,              \n\
	extension,continuity,Udirection,after);                       \n\
                                                                  \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  if (!aGeomSurface.IsNull())
  {
    DisplaySurface(aDoc,aGeomSurface,Quantity_NOC_HOTPINK);
  }

  if (!aTranslatedGeomSurface.IsNull())
  {
    DisplaySurface(aDoc,aTranslatedGeomSurface,Quantity_NOC_BLUE1);
  }

  DisplayCurve(aDoc,SPL1,Quantity_NOC_RED ,false);
  DisplayCurve(aDoc,SPL2,Quantity_NOC_GREEN ,false);

  Message += "aGeomSurface                    is Hot Pink; \n";
  Message += "aTranslatedGeomSurface   is Blue; \n";
  Message += "SPL1                                   is Red; \n";
  Message += "SPL2                                   is Green; \n";

  PostProcess(aDoc,ID_BUTTON_Test_46,TheDisplayType,Message);
}

void GeomSources::gpTest47(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================


  TColgp_Array1OfPnt array1 (1,5);
  array1.SetValue(1,gp_Pnt (-5,1,2));
  array1.SetValue(2,gp_Pnt (-5,2,2));
  array1.SetValue(3,gp_Pnt (-5.3,3,1));
  array1.SetValue(4,gp_Pnt (-5,4,1));
  array1.SetValue(5,gp_Pnt (-5,5,2));
  Handle(Geom_BSplineCurve) SPL1 =
    GeomAPI_PointsToBSpline(array1).Curve();

  TColgp_Array1OfPnt array2 (1,5);
  array2.SetValue(1,gp_Pnt (4,1,2));
  array2.SetValue(2,gp_Pnt (4,2,2));
  array2.SetValue(3,gp_Pnt (3.7,3,1));
  array2.SetValue(4,gp_Pnt (4,4,1));
  array2.SetValue(5,gp_Pnt (4,5,2));
  Handle(Geom_BSplineCurve) SPL2 =
    GeomAPI_PointsToBSpline(array2).Curve();

  GeomFill_FillingStyle Type = GeomFill_StretchStyle;

  GeomFill_BSplineCurves aGeomFill1(SPL1,SPL2,Type);
  Handle(Geom_BSplineSurface) aSurf1 = aGeomFill1.Surface();

  TColgp_Array2OfPnt array3 (1,5,1,5);
  array3.SetValue(1,1,gp_Pnt (-4,-4,5));
  array3.SetValue(1,2,gp_Pnt (-4,-2,5));
  array3.SetValue(1,3,gp_Pnt (-4,0,4));
  array3.SetValue(1,4,gp_Pnt (-4,2,5));
  array3.SetValue(1,5,gp_Pnt (-4,4,5));

  array3.SetValue(2,1,gp_Pnt (-2,-4,4));
  array3.SetValue(2,2,gp_Pnt (-2,-2,4));
  array3.SetValue(2,3,gp_Pnt (-2,0,4));
  array3.SetValue(2,4,gp_Pnt (-2,2,4));
  array3.SetValue(2,5,gp_Pnt (-2,5,4));

  array3.SetValue(3,1,gp_Pnt (0,-4,3.5));
  array3.SetValue(3,2,gp_Pnt (0,-2,3.5));
  array3.SetValue(3,3,gp_Pnt (0,0,3.5));
  array3.SetValue(3,4,gp_Pnt (0,2,3.5));
  array3.SetValue(3,5,gp_Pnt (0,5,3.5));

  array3.SetValue(4,1,gp_Pnt (2,-4,4));
  array3.SetValue(4,2,gp_Pnt (2,-2,4));
  array3.SetValue(4,3,gp_Pnt (2,0,3.5));
  array3.SetValue(4,4,gp_Pnt (2,2,5));
  array3.SetValue(4,5,gp_Pnt (2,5,4));

  array3.SetValue(5,1,gp_Pnt (4,-4,5));
  array3.SetValue(5,2,gp_Pnt (4,-2,5));
  array3.SetValue(5,3,gp_Pnt (4,0,5));
  array3.SetValue(5,4,gp_Pnt (4,2,6));
  array3.SetValue(5,5,gp_Pnt (4,5,5));

  Handle(Geom_BSplineSurface) aSurf2 =
    GeomAPI_PointsToBSplineSurface(array3).Surface();

  GeomAPI_ExtremaSurfaceSurface ESS(aSurf1,aSurf2);
  //Standard_Real dist = ESS.LowerDistance();
  gp_Pnt P1,P2;
  ESS.NearestPoints(P1,P2);

  gp_Pnt P3,P4;
  Handle(Geom_Curve) aCurve;
  Standard_Integer NbExtrema = ESS.NbExtrema();
  for(Standard_Integer k=1;k<=NbExtrema;k++){
    ESS.Points(k,P3,P4);
    aCurve= GC_MakeSegment(P3,P4).Value();
    DisplayCurve(aDoc,aCurve,Quantity_NOC_YELLOW3,false);
}

//==============================================================

    TCollection_AsciiString Message ("                         \n\
GeomFill_FillingStyle Type = GeomFill_StretchStyle;            \n\
                                                               \n\
GeomFill_BSplineCurves aGeomFill1(SPL1,SPL2,Type);             \n\
Handle(Geom_BSplineSurface) aSurf1 = aGeomFill1.Surface();     \n\
                                                               \n\
Handle(Geom_BSplineSurface) aSurf2 =                           \n\
	GeomAPI_PointsToBSplineSurface(array3).Surface();          \n\
                                                               \n\
GeomAPI_ExtremaSurfaceSurface ESS(aSurf1,aSurf2);              \n\
Standard_Real dist = ESS.LowerDistance();                      \n\
gp_Pnt P1,P2;                                                  \n\
ESS.NearestPoints(P1,P2);                                      \n\
                                                               \n"); 

  AddSeparator(aDoc,Message);
  Message += "aSurf1   is Green; \n";
  Message += "aSurf2   is Hot Pink; \n";
  Message += "Nearest points P1 and P2 are shown; \n";

  //--------------------------------------------------------------

//mfa

  DisplaySurface(aDoc,aSurf1,Quantity_NOC_GREEN);
  DisplaySurface(aDoc,aSurf2,Quantity_NOC_HOTPINK);
  DisplayCurve(aDoc,SPL1,Quantity_NOC_RED ,false);
  DisplayCurve(aDoc,SPL2,Quantity_NOC_AZURE ,false);

  DisplayPoint(aDoc,P1,Standard_CString("P1"));
  DisplayPoint(aDoc,P2,Standard_CString("P2"));

  PostProcess(aDoc,ID_BUTTON_Test_47,TheDisplayType,Message);
}

void GeomSources::gpTest48(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = a2DNo3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  Standard_Real radius = 3;
  Handle(Geom2d_Circle) circle =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(-7,2),gp_Dir2d(1,0)),radius);

  Handle(Geom2d_TrimmedCurve) C = new Geom2d_TrimmedCurve(circle,1,5);

  Geom2dAdaptor_Curve GAC (C);


  TColgp_Array1OfPnt2d array (1,5); // sizing array
  array.SetValue(1,gp_Pnt2d (0,0));
  array.SetValue(2,gp_Pnt2d (1,2));
  array.SetValue(3,gp_Pnt2d (2,3));
  array.SetValue(4,gp_Pnt2d (4,3));
  array.SetValue(5,gp_Pnt2d (5,5));
  Handle(Geom2d_BSplineCurve) SPL1 =
    Geom2dAPI_PointsToBSpline(array);


  Handle(TColgp_HArray1OfPnt2d) harray =
    new TColgp_HArray1OfPnt2d (1,5); // sizing harray
  harray->SetValue(1,gp_Pnt2d (13+ 0,0));
  harray->SetValue(2,gp_Pnt2d (13+ 1,2));
  harray->SetValue(3,gp_Pnt2d (13+ 2,3));
  harray->SetValue(4,gp_Pnt2d (13+ 4,3));
  harray->SetValue(5,gp_Pnt2d (13+ 5,5));
  Geom2dAPI_Interpolate anInterpolation(harray,Standard_True,0.01);
  anInterpolation.Perform();
  Handle(Geom2d_BSplineCurve) SPL2 = anInterpolation.Curve();

  Bnd_Box2d aCBox;
  Geom2dAdaptor_Curve GACC (C);
  BndLib_Add2dCurve::Add (GACC,Precision::Approximation(),aCBox);

  Standard_Real aCXmin, aCYmin, aCXmax, aCYmax;
  aCBox.Get( aCXmin, aCYmin, aCXmax,aCYmax);

  Bnd_Box2d aSPL1Box;
  Geom2dAdaptor_Curve GAC1 (SPL1);
  BndLib_Add2dCurve::Add (GAC1,Precision::Approximation(),aSPL1Box);

  Standard_Real aSPL1Xmin,aSPL1Ymin,aSPL1Xmax,aSPL1Ymax;
  aSPL1Box.Get( aSPL1Xmin, aSPL1Ymin, aSPL1Xmax,aSPL1Ymax);

  Bnd_Box2d aSPL2Box;
  Geom2dAdaptor_Curve GAC2 (SPL2);
  BndLib_Add2dCurve::Add (GAC2,Precision::Approximation(),aSPL2Box);

  Standard_Real aSPL2Xmin,aSPL2Ymin,aSPL2Xmax,aSPL2Ymax;
  aSPL2Box.Get( aSPL2Xmin, aSPL2Ymin, aSPL2Xmax,aSPL2Ymax);

  //==============================================================
    TCollection_AsciiString Message (" \
                                                                       \n\
Standard_Real radius = 3;                                              \n\
Handle(Geom2d_Circle) circle =                                         \n\
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(-7,2),gp_Dir2d(1,0)),radius);  \n\
                                                                       \n\
Handle(Geom2d_TrimmedCurve) C = new Geom2d_TrimmedCurve(circle,1,5);   \n\
Geom2dAdaptor_Curve GAC (C);                                           \n\
                                                                       \n\
Handle(Geom2d_BSplineCurve) SPL1 ; // SPL1 = ...                       \n\
                                                                       \n\
Handle(Geom2d_BSplineCurve) SPL2 ; // SPL2 = ...                       \n\
                                                                       \n\
Bnd_Box2d aCBox;                                                       \n\
Geom2dAdaptor_Curve GACC (C);                                          \n\
BndLib_Add2dCurve::Add (GACC,Precision::Approximation(),aCBox);        \n\
                                                                       \n\
Standard_Real aCXmin, aCYmin, aCXmax, aCYmax;                          \n\
aCBox.Get(  aCXmin, aCYmin, aCXmax,aCYmax);                            \n\
                                                                       \n\
Bnd_Box2d aSPL1Box;                                                    \n\
Geom2dAdaptor_Curve GAC1 (SPL1);                                       \n\
BndLib_Add2dCurve::Add (GAC1,Precision::Approximation(),aSPL1Box);     \n\
                                                                       \n\
Standard_Real aSPL1Xmin,aSPL1Ymin,aSPL1Xmax,aSPL1Ymax;                 \n\
aSPL1Box.Get(  aSPL1Xmin, aSPL1Ymin, aSPL1Xmax,aSPL1Ymax);             \n");
Message += "\
                                                                       \n\
Bnd_Box2d aSPL2Box;                                                    \n\
Geom2dAdaptor_Curve GAC2 (SPL2);                                       \n\
BndLib_Add2dCurve::Add (GAC2,Precision::Approximation(),aSPL2Box);     \n\
                                                                       \n\
Standard_Real aSPL2Xmin,aSPL2Ymin,aSPL2Xmax,aSPL2Ymax;                 \n\
aSPL2Box.Get(  aSPL2Xmin, aSPL2Ymin, aSPL2Xmax,aSPL2Ymax);             \n\
                                                                       \n";
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplayCurve(aDoc,C ,5);
  DisplayCurve(aDoc,SPL1,6 );
  DisplayCurve(aDoc,SPL2,7 );

  DisplayPoint(aDoc,gp_Pnt2d(aCXmin,aCYmax),Standard_CString("aCXmin,aCYmax"));
  DisplayPoint(aDoc,gp_Pnt2d(aCXmax,aCYmax),Standard_CString("aCXmax,aCYmax"));
  DisplayPoint(aDoc,gp_Pnt2d(aCXmin,aCYmin),Standard_CString("aCXmin,aCYmin"));
  DisplayPoint(aDoc,gp_Pnt2d(aCXmax,aCYmin),Standard_CString("aCXmax,aCYmin"));

  DisplayCurve(aDoc,GCE2d_MakeSegment(gp_Pnt2d(aCXmin,aCYmax),gp_Pnt2d(aCXmax,aCYmax)).Value() ,4); // X,Ymax
  DisplayCurve(aDoc,GCE2d_MakeSegment(gp_Pnt2d(aCXmin,aCYmin),gp_Pnt2d(aCXmax,aCYmin)).Value() ,4); // X,Ymin
  DisplayCurve(aDoc,GCE2d_MakeSegment(gp_Pnt2d(aCXmin,aCYmin),gp_Pnt2d(aCXmin,aCYmax)).Value() ,4); // Xmin,Y
  DisplayCurve(aDoc,GCE2d_MakeSegment(gp_Pnt2d(aCXmax,aCYmin),gp_Pnt2d(aCXmax,aCYmax)).Value() ,4); // Xmax,Y

  DisplayPoint(aDoc,gp_Pnt2d(aSPL1Xmin,aSPL1Ymax),Standard_CString("aSPL1Xmin,aSPL1Ymax"));
  DisplayPoint(aDoc,gp_Pnt2d(aSPL1Xmax,aSPL1Ymax),Standard_CString("aSPL1Xmax,aSPL1Ymax"));
  DisplayPoint(aDoc,gp_Pnt2d(aSPL1Xmin,aSPL1Ymin),Standard_CString("aSPL1Xmin,aSPL1Ymin"));
  DisplayPoint(aDoc,gp_Pnt2d(aSPL1Xmax,aSPL1Ymin),Standard_CString("aSPL1Xmax,aSPL1Ymin"));

  DisplayCurve(aDoc,GCE2d_MakeSegment(gp_Pnt2d(aSPL1Xmin,aSPL1Ymax),gp_Pnt2d(aSPL1Xmax,aSPL1Ymax)).Value() ,4); // X,Ymax
  DisplayCurve(aDoc,GCE2d_MakeSegment(gp_Pnt2d(aSPL1Xmin,aSPL1Ymin),gp_Pnt2d(aSPL1Xmax,aSPL1Ymin)).Value() ,4); // X,Ymin
  DisplayCurve(aDoc,GCE2d_MakeSegment(gp_Pnt2d(aSPL1Xmin,aSPL1Ymin),gp_Pnt2d(aSPL1Xmin,aSPL1Ymax)).Value() ,4); // Xmin,Y
  DisplayCurve(aDoc,GCE2d_MakeSegment(gp_Pnt2d(aSPL1Xmax,aSPL1Ymin),gp_Pnt2d(aSPL1Xmax,aSPL1Ymax)).Value() ,4); // Xmax,Y

  DisplayPoint(aDoc,gp_Pnt2d(aSPL1Xmin,aSPL1Ymax),Standard_CString("aSPL2Xmin,aSPL2Ymax"));
  DisplayPoint(aDoc,gp_Pnt2d(aSPL1Xmax,aSPL1Ymax),Standard_CString("aSPL2Xmax,aSPL2Ymax"));
  DisplayPoint(aDoc,gp_Pnt2d(aSPL1Xmin,aSPL1Ymin),Standard_CString("aSPL2Xmin,aSPL2Ymin"));
  DisplayPoint(aDoc,gp_Pnt2d(aSPL1Xmax,aSPL1Ymin),Standard_CString("aSPL2Xmax,aSPL2Ymin"));

  DisplayCurve(aDoc,GCE2d_MakeSegment(gp_Pnt2d(aSPL2Xmin,aSPL2Ymax),gp_Pnt2d(aSPL2Xmax,aSPL2Ymax)).Value() ,4); // X,Ymax
  DisplayCurve(aDoc,GCE2d_MakeSegment(gp_Pnt2d(aSPL2Xmin,aSPL2Ymin),gp_Pnt2d(aSPL2Xmax,aSPL2Ymin)).Value() ,4); // X,Ymin
  DisplayCurve(aDoc,GCE2d_MakeSegment(gp_Pnt2d(aSPL2Xmin,aSPL2Ymin),gp_Pnt2d(aSPL2Xmin,aSPL2Ymax)).Value() ,4); // Xmin,Y
  DisplayCurve(aDoc,GCE2d_MakeSegment(gp_Pnt2d(aSPL2Xmax,aSPL2Ymin),gp_Pnt2d(aSPL2Xmax,aSPL2Ymax)).Value() ,4); // Xmax,Y

  PostProcess(aDoc,ID_BUTTON_Test_48,TheDisplayType,Message);
}


void GeomSources::gpTest49(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);

  //==============================================================

  Bnd_Box aBox;
  Standard_Real radius = 100;
  gp_Ax2 anAxis(gp_Pnt(0,0,0),gp_Dir(1,2,-5));

  Handle(Geom_Circle) C =
    new Geom_Circle(anAxis,radius);
  GeomAdaptor_Curve GAC (C);
  BndLib_Add3dCurve::Add (GAC,Precision::Approximation(),aBox);

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax ;
  aBox.Get( aXmin, aYmin,aZmin, aXmax,aYmax,aZmax);

  //==============================================================

    TCollection_AsciiString Message (" \
                                                               \n\
Bnd_Box aBox;                                                  \n\
Standard_Real radius = 100;                                    \n\
gp_Ax2 anAxis(gp_Pnt(0,0,0),gp_Dir(1,2,-5));                   \n\
                                                               \n\
Handle(Geom_Circle) C =                                        \n\
    new Geom_Circle(anAxis,radius);                            \n\
GeomAdaptor_Curve GAC (C);                                     \n\
BndLib_Add3dCurve::Add (GAC,Precision::Approximation(),aBox);  \n\
                                                               \n\
Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax ;       \n\
aBox.Get(  aXmin, aYmin,aZmin, aXmax,aYmax,aZmax);             \n\
                                                               \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  DisplayCurve(aDoc,C,Quantity_NOC_BLUE1 ,false);

  DisplayPoint(aDoc,gp_Pnt(aXmin,aYmax,aZmin),Standard_CString("aXmin,aYmax,aZmin"));
  DisplayPoint(aDoc,gp_Pnt(aXmax,aYmax,aZmin),Standard_CString("aXmax,aYmax,aZmin"));
  DisplayPoint(aDoc,gp_Pnt(aXmin,aYmin,aZmin),Standard_CString("aXmin,aYmin,aZmin"));
  DisplayPoint(aDoc,gp_Pnt(aXmax,aYmin,aZmin),Standard_CString("aXmax,aYmin,aZmin"));

  DisplayPoint(aDoc,gp_Pnt(aXmin,aYmax,aZmax),Standard_CString("aXmin,aYmax,aZmax"));
  DisplayPoint(aDoc,gp_Pnt(aXmax,aYmax,aZmax),Standard_CString("aXmax,aYmax,aZmax"));
  DisplayPoint(aDoc,gp_Pnt(aXmin,aYmin,aZmax),Standard_CString("aXmin,aYmin,aZmax"));
  DisplayPoint(aDoc,gp_Pnt(aXmax,aYmin,aZmax),Standard_CString("aXmax,aYmin,aZmax"));

  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmax,aZmin),
    gp_Pnt(aXmax,aYmax,aZmin)).Value() ,Quantity_NOC_RED); // X,Ymax,ZMin
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmin),
    gp_Pnt(aXmax,aYmin,aZmin)).Value() ,Quantity_NOC_RED); // X,Ymin,ZMin
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmin),
    gp_Pnt(aXmin,aYmax,aZmin)).Value() ,Quantity_NOC_RED); // Xmin,Y,ZMin
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmax,aYmin,aZmin),
    gp_Pnt(aXmax,aYmax,aZmin)).Value() ,Quantity_NOC_RED); // Xmax,Y,ZMin
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmax,aZmax),
    gp_Pnt(aXmax,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // X,Ymax,ZMax
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmax),
    gp_Pnt(aXmax,aYmin,aZmax)).Value() ,Quantity_NOC_RED); // X,Ymin,ZMax
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmax),
    gp_Pnt(aXmin,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmin,Y,ZMax
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmax,aYmin,aZmax),
    gp_Pnt(aXmax,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmax,Y,ZMax
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmin),
    gp_Pnt(aXmin,aYmin,aZmax)).Value() ,Quantity_NOC_RED); // Xmin,Ymin,Z
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmax,aYmin,aZmin),
    gp_Pnt(aXmax,aYmin,aZmax)).Value() ,Quantity_NOC_RED); // Xmax,Ymin,Z
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmax,aZmin),
    gp_Pnt(aXmin,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmin,Ymax,Z
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmax,aYmax,aZmin),
    gp_Pnt(aXmax,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmax,Ymax,Z

  PostProcess(aDoc,ID_BUTTON_Test_49,TheDisplayType,Message);
}

void GeomSources::gpTest50(CGeometryDoc* aDoc)
{
  DisplayType TheDisplayType = No2D3D;
  PreProcess(aDoc,TheDisplayType);
  //==============================================================

  TColgp_Array1OfPnt array1 (1,5);
  array1.SetValue(1,gp_Pnt (-40,00,20 ));
  array1.SetValue(2,gp_Pnt (-70,20,20 ));
  array1.SetValue(3,gp_Pnt (-60,30,10 ));
  array1.SetValue(4,gp_Pnt (-40,30,-10));
  array1.SetValue(5,gp_Pnt (-30,50,-20));
  Handle(Geom_BSplineCurve) SPL1 =
    GeomAPI_PointsToBSpline(array1).Curve();

  TColgp_Array1OfPnt array2 (1,5);
  array2.SetValue(1,gp_Pnt (-40,0, 20));
  array2.SetValue(2,gp_Pnt (-20,20,0 ));
  array2.SetValue(3,gp_Pnt (20 ,30,-10));
  array2.SetValue(4,gp_Pnt (30 ,70,-20));
  array2.SetValue(5,gp_Pnt (40 ,90,-10));
  Handle(Geom_BSplineCurve) SPL2 =
    GeomAPI_PointsToBSpline(array2).Curve();

  GeomFill_FillingStyle Type = GeomFill_StretchStyle;
  GeomFill_BSplineCurves aGeomFill1(SPL1,SPL2,Type);
  Handle(Geom_BSplineSurface) aSurf = aGeomFill1.Surface();
  GeomAdaptor_Surface GAS (aSurf);
  Bnd_Box aBox;
  BndLib_AddSurface::Add (GAS,Precision::Approximation(),aBox);

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax ;
  aBox.Get( aXmin, aYmin,aZmin, aXmax,aYmax,aZmax);

  //==============================================================
  TCollection_AsciiString Message (" \
                                                               \n\
TColgp_Array1OfPnt array1 (1,5);                               \n\
array1.SetValue(1,gp_Pnt (-40,  0, 20));                           \n\
array1.SetValue(2,gp_Pnt (-70, 20, 20));                           \n\
array1.SetValue(3,gp_Pnt (-60, 30, 10));                           \n\
array1.SetValue(4,gp_Pnt (-40, 30,-10));                           \n\
array1.SetValue(5,gp_Pnt (-30, 50,-20));                           \n\
Handle(Geom_BSplineCurve) SPL1 =                               \n\
	GeomAPI_PointsToBSpline(array1).Curve();                   \n\
                                                               \n\
TColgp_Array1OfPnt array2 (1,5);                               \n\
array2.SetValue(1,gp_Pnt (-40,  0, 20));                           \n\
array2.SetValue(2,gp_Pnt (-20, 20,  0));                           \n\
array2.SetValue(3,gp_Pnt ( 20, 30,-10));                           \n\
array2.SetValue(4,gp_Pnt ( 30, 70,-20));                           \n\
array2.SetValue(5,gp_Pnt ( 40, 90,-10));                           \n\
Handle(Geom_BSplineCurve) SPL2 =                               \n\
	GeomAPI_PointsToBSpline(array2).Curve();                   \n\
                                                               \n\
GeomFill_FillingStyle Type = GeomFill_StretchStyle;            \n\
GeomFill_BSplineCurves aGeomFill1(SPL1,SPL2,Type);             \n\
Handle(Geom_BSplineSurface) aSurf = aGeomFill1.Surface();      \n\
GeomAdaptor_Surface GAS (aSurf);                               \n\
Bnd_Box aBox;                                                  \n\
BndLib_AddSurface::Add (GAS,Precision::Approximation(),aBox);  \n\
                                                               \n\
Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax ;       \n\
aBox.Get(  aXmin, aYmin,aZmin, aXmax,aYmax,aZmax);             \n\
                                                               \n");
  AddSeparator(aDoc,Message);
  //--------------------------------------------------------------

  Quantity_NameOfColor aNameOfColor= Quantity_NOC_GREEN;
  Handle(ISession_Surface) aGraphicalSurface = new ISession_Surface(aSurf);
  aDoc->GetAISContext()->SetColor (aGraphicalSurface, aNameOfColor, Standard_False);
  aGraphicalSurface->Attributes()->FreeBoundaryAspect()->SetColor(aNameOfColor);
  aGraphicalSurface->Attributes()->UIsoAspect()->SetColor(aNameOfColor);
  aGraphicalSurface->Attributes()->VIsoAspect()->SetColor(aNameOfColor);

  aDoc->GetAISContext()->SetDisplayMode (aGraphicalSurface, 1, Standard_False);
  aDoc->GetAISContext()->Display(aGraphicalSurface,false);
  //   DisplaySurface(aDoc,aSurf,Quantity_NOC_GREEN);

  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmax,aZmin),
    gp_Pnt(aXmax,aYmax,aZmin)).Value() ,Quantity_NOC_RED); // X,Ymax,ZMin
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmin),
    gp_Pnt(aXmax,aYmin,aZmin)).Value() ,Quantity_NOC_RED); // X,Ymin,ZMin
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmin),
    gp_Pnt(aXmin,aYmax,aZmin)).Value() ,Quantity_NOC_RED); // Xmin,Y,ZMin
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmax,aYmin,aZmin),
    gp_Pnt(aXmax,aYmax,aZmin)).Value() ,Quantity_NOC_RED); // Xmax,Y,ZMin
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmax,aZmax),
    gp_Pnt(aXmax,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // X,Ymax,ZMax
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmax),
    gp_Pnt(aXmax,aYmin,aZmax)).Value() ,Quantity_NOC_RED); // X,Ymin,ZMax
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmax),
    gp_Pnt(aXmin,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmin,Y,ZMax
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmax,aYmin,aZmax),
    gp_Pnt(aXmax,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmax,Y,ZMax
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmin),
    gp_Pnt(aXmin,aYmin,aZmax)).Value() ,Quantity_NOC_RED); // Xmin,Ymin,Z
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmax,aYmin,aZmin),
    gp_Pnt(aXmax,aYmin,aZmax)).Value() ,Quantity_NOC_RED); // Xmax,Ymin,Z
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmin,aYmax,aZmin),
    gp_Pnt(aXmin,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmin,Ymax,Z
  DisplayCurve(aDoc,GC_MakeSegment(gp_Pnt(aXmax,aYmax,aZmin),
    gp_Pnt(aXmax,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmax,Ymax,Z

  PostProcess(aDoc,ID_BUTTON_Test_50,TheDisplayType,Message);
}
