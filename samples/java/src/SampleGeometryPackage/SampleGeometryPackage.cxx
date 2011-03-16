#include <SampleGeometryPackage.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ISession2D_InteractiveContext.hxx>
#include <TCollection_AsciiString.hxx>
#include <ISession_Point.hxx>
#include <ISession_Curve.hxx>
#include <ISession_Surface.hxx>
#include <ISession_Text.hxx>
#include <ISession_Direction.hxx>
#include <ISession2D_Curve.hxx>
#include <AIS_Drawer.hxx>
#include <gp.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>
#include <gp_Mat.hxx>
#include <gp_Pln.hxx>
#include <gp_Lin.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gce_MakeLin2d.hxx>
#include <gce_MakeCirc2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2dAPI_PointsToBSpline.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Geom2dAPI_ExtremaCurveCurve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <GCE2d_MakeArcOfCircle.hxx>
#include <GCE2d_MakeArcOfEllipse.hxx>
#include <GCE2d_MakeEllipse.hxx>
#include <GCE2d_MakeParabola.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Transformation.hxx>
#include <GeomAPI.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomAPI_ExtremaSurfaceSurface.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_IntCS.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_CompBezierSurfacesToBSplineSurface.hxx>
#include <GeomFill_FillingStyle.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <GeomFill_SimpleBound.hxx>
#include <GeomFill_ConstrainedFilling.hxx>
#include <GeomFill_Pipe.hxx>
#include <GeomLib.hxx>
#include <GC_MakePlane.hxx>
#include <GC_MakeSegment.hxx>
#include <GC_MakeCircle.hxx>
#include <GC_MakeEllipse.hxx>
#include <GC_MakeConicalSurface.hxx>
#include <GC_MakeTranslation.hxx>
#include <GProp_PEquation.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GccAna_Pnt2dBisec.hxx>
#include <GccAna_Lin2d2Tan.hxx>
#include <GccAna_Circ2d2TanRad.hxx>
#include <GccEnt.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <FairCurve_Batten.hxx>
#include <FairCurve_AnalysisCode.hxx>
#include <FairCurve_MinimalVariation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array2OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColGeom_Array2OfBezierSurface.hxx>
#include <Precision.hxx>
#include <Quantity_Length.hxx>
#include <Bnd_Box2d.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BndLib_AddSurface.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_IsoAspect.hxx>

#include <Aspect_Window.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <V2d_Viewer.hxx>
#include <V2d_View.hxx>

#ifdef WNT
#include <WNT_Window.hxx>
#include <WNT_GraphicDevice.hxx>
#include <WNT_WDriver.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#else
#include <Xw_Window.hxx>
#include <Xw_GraphicDevice.hxx>
#include <Xw_Driver.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#endif




//===============================================================
// Function name: CreateViewer3d
//===============================================================
 Handle(V3d_Viewer) SampleGeometryPackage::CreateViewer3d(const Standard_ExtString aName) 
{
#ifdef WNT
static Handle(Graphic3d_WNTGraphicDevice) defaultDevice;
    
  if (defaultDevice.IsNull()) 
    defaultDevice = new Graphic3d_WNTGraphicDevice();
  return new V3d_Viewer(defaultDevice, aName);
#else
static Handle(Graphic3d_GraphicDevice) defaultDevice;
    
  if (defaultDevice.IsNull()) 
    defaultDevice = new Graphic3d_GraphicDevice("");
  return new V3d_Viewer(defaultDevice, aName);
#endif //WNT
}

//===============================================================
// Function name: SetWindow3d
//===============================================================
void SampleGeometryPackage::SetWindow3d (const Handle(V3d_View)& aView,
					 const Standard_Integer hiwin,
					 const Standard_Integer lowin)
{
#ifdef WNT
  Handle(Graphic3d_WNTGraphicDevice) d = 
    Handle(Graphic3d_WNTGraphicDevice)::DownCast(aView->Viewer()->Device());
  Handle(WNT_Window) w = new WNT_Window(d,hiwin,lowin);
#else
  Handle(Graphic3d_GraphicDevice) d = 
    Handle(Graphic3d_GraphicDevice)::DownCast(aView->Viewer()->Device());
  Handle(Xw_Window) w = new Xw_Window(d,hiwin,lowin,Xw_WQ_3DQUALITY);
#endif
  aView->SetWindow(w);
}


//===============================================================
// Function name: CreateViewer2d
//===============================================================
Handle(V2d_Viewer) SampleGeometryPackage::CreateViewer2d (const Standard_ExtString aName)
{
#ifdef WNT
static Handle(WNT_GraphicDevice) default2dDevice;

  if(default2dDevice.IsNull()) 
    default2dDevice = new WNT_GraphicDevice();
#else
  static Handle(Xw_GraphicDevice) default2dDevice;

  if(default2dDevice.IsNull())  {
    default2dDevice = new Xw_GraphicDevice("",Xw_TOM_READONLY);
  }
#endif
  return new V2d_Viewer(default2dDevice,aName,"");
}


//===============================================================
// Function name: CreateView2d
//===============================================================
Handle(V2d_View) SampleGeometryPackage::CreateView2d (const Handle(V2d_Viewer)& aViewer,
						      const Standard_Integer hiwin,
						      const Standard_Integer lowin)
{
#ifdef WNT
  Handle(WNT_GraphicDevice) GD = Handle(WNT_GraphicDevice)::DownCast(aViewer->Device());
  Handle(WNT_Window) W = new WNT_Window(GD,hiwin,lowin,Quantity_NOC_MATRAGRAY);
  Handle(WNT_WDriver) D = new WNT_WDriver(W);
#else
  Handle(Xw_GraphicDevice) GD = Handle(Xw_GraphicDevice)::DownCast(aViewer->Device());
  Handle(Xw_Window) W = new Xw_Window(GD,hiwin,lowin,Xw_WQ_DRAWINGQUALITY,Quantity_NOC_MATRAGRAY);
  Handle(Xw_Driver) D = new Xw_Driver(W);
#endif
  
  Handle(V2d_View) V = new V2d_View(D,aViewer);
  V->Update();
  return V;
}



//===============================================================

void AddSeparator(TCollection_AsciiString& aMessage)
{
    aMessage+= "------------------------------------------------------------------------\n";
}

void DisplayPoint(const Handle(ISession2D_InteractiveContext)& aContext2D, 
                  const gp_Pnt2d& aPoint, const TCollection_AsciiString& aText, 
                  Standard_Boolean UpdateViewer = Standard_True,
                  Standard_Real anXoffset = 0, Standard_Real anYoffset = 0, 
                  Standard_Real TextScale = 0.05)
{
    Handle(ISession_Point) aGraphicPoint = new ISession_Point(aPoint);
    aContext2D->Display(aGraphicPoint,Standard_False);
    Handle(ISession_Text)  aGraphicText = new ISession_Text(aText,aPoint.X()+anXoffset,aPoint.Y()+anYoffset);
    aGraphicText->SetScale  (TextScale);
    aContext2D->Display(aGraphicText,UpdateViewer);
}

void DisplayPoint(const Handle(AIS_InteractiveContext)& aContext, 
                  const gp_Pnt& aPoint, const TCollection_AsciiString& aText, 
                  Standard_Boolean UpdateViewer = Standard_True,
                  Standard_Real anXoffset = 0, Standard_Real anYoffset = 0, Standard_Real aZoffset = 0,
                  Standard_Real TextScale = 0.05)
{
    Handle(ISession_Point) aGraphicPoint = new ISession_Point(aPoint);
    aContext->Display(aGraphicPoint,Standard_False);
    Handle(ISession_Text)  aGraphicText = new ISession_Text(aText,aPoint.X()+anXoffset,aPoint.Y()+anYoffset,aPoint.Z()+aZoffset);
    aGraphicText->SetScale  (TextScale);
    aContext->Display(aGraphicText,UpdateViewer);
}

void DisplayCurve(const Handle(ISession2D_InteractiveContext)& aContext2D,
                  const Handle(Geom2d_Curve)& aCurve, Standard_Integer aColorIndex = 4,
                  Standard_Boolean UpdateViewer = Standard_False)
{
    Handle(ISession2D_Curve) aGraphicCurve = new ISession2D_Curve(aCurve);
    aGraphicCurve->SetColorIndex(aColorIndex) ;
    aContext2D->Display(aGraphicCurve,UpdateViewer);
}

void DisplayCurveAndCurvature(const Handle(ISession2D_InteractiveContext)& aContext2D,
                              const Handle(Geom2d_Curve)& aCurve, Standard_Integer aColorIndex = 4,
                              Standard_Boolean UpdateViewer = Standard_False)
{
    Handle(ISession2D_Curve) aGraphicCurve = new ISession2D_Curve(aCurve);
    aGraphicCurve->SetDisplayCurbure(Standard_True) ;
    aGraphicCurve->SetDiscretisation(20);
    aGraphicCurve->SetColorIndex(aColorIndex) ;
    aContext2D->Display(aGraphicCurve,UpdateViewer);
}

void DisplayCurve(const Handle(AIS_InteractiveContext)& aContext,
                  const Handle(Geom_Curve)& aCurve, Quantity_NameOfColor aNameOfColor, 
                  Standard_Boolean UpdateViewer = Standard_False)
{
    Handle(ISession_Curve) aGraphicCurve = new ISession_Curve(aCurve);
    aContext->SetColor(aGraphicCurve,aNameOfColor);
    aGraphicCurve->Attributes()->LineAspect()->SetColor(aNameOfColor);
    aContext->Display(aGraphicCurve,UpdateViewer);
}

void DisplayCurve(const Handle(AIS_InteractiveContext)& aContext,
                  const Handle(Geom_Curve)& aCurve, Standard_Boolean UpdateViewer = Standard_False)
{
    Handle(ISession_Curve) aGraphicCurve = new ISession_Curve(aCurve);
    aContext->Display(aGraphicCurve,UpdateViewer);
}


void DisplaySurface(const Handle(AIS_InteractiveContext)& aContext,
                    const Handle(Geom_Surface)& aSurface, Quantity_NameOfColor aNameOfColor, 
                    Standard_Boolean UpdateViewer = Standard_False)
{
    Handle(ISession_Surface) aGraphicalSurface = new ISession_Surface(aSurface);
    aContext->SetColor(aGraphicalSurface,aNameOfColor);
    aGraphicalSurface->Attributes()->FreeBoundaryAspect()->SetColor(aNameOfColor);
    aGraphicalSurface->Attributes()->UIsoAspect()->SetColor(aNameOfColor);
    aGraphicalSurface->Attributes()->VIsoAspect()->SetColor(aNameOfColor);
    aContext->Display(aGraphicalSurface,UpdateViewer);
}

void DisplaySurface(const Handle(AIS_InteractiveContext)& aContext,
                    const Handle(Geom_Surface)& aSurface, Standard_Boolean UpdateViewer = Standard_False)
{
    Handle(ISession_Surface) aGraphicalSurface = new ISession_Surface(aSurface);
    aContext->Display(aGraphicalSurface,UpdateViewer);
}


//===============================================================
// Function name: gpTest1
//===============================================================
 void SampleGeometryPackage::gpTest1(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
    aContext->EraseAll(Standard_False);
//==============================================================

gp_XYZ A(1,2,3);                        
gp_XYZ B(2,2,2);                        
gp_XYZ C(3,2,3);                        
Standard_Real result = A.DotCross(B,C); 
                                        
//==============================================================
    Message = "\
                                        \n\
gp_XYZ A(1,2,3);                        \n\
gp_XYZ B(2,2,2);                        \n\
gp_XYZ C(3,2,3);                        \n\
Standard_Real result = A.DotCross(B,C); \n\
                                        \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    DisplayPoint(aContext,gp_Pnt(A),"A (1,2,3)",Standard_False,0.1);
    DisplayPoint(aContext,gp_Pnt(B),"B (2,2,2)",Standard_False,0.1);
    DisplayPoint(aContext,gp_Pnt(C),"C (3,2,3)",Standard_True,0.1);

    Message+= " result = ";
    Message+= TCollection_AsciiString(result);
    Message+= "  \n";
}

//===============================================================
// Function name: gpTest2
//===============================================================
 void SampleGeometryPackage::gpTest2(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
    aContext->EraseAll(Standard_False);
//==============================================================

gp_Pnt P1(1,2,3);  
                   
//==============================================================
    Message = "\
                  \n\
gp_Pnt P1(1,2,3); \n\
                  \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    DisplayPoint(aContext,P1,"P1 (1,2,3)",Standard_True,0.5);
}

//===============================================================
// Function name: gpTest3
//===============================================================
 void SampleGeometryPackage::gpTest3(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
    aContext->EraseAll(Standard_False);
//==============================================================

gp_XYZ A(1,2,3);  
gp_Pnt P2(A);     
                  
//==============================================================
    Message = "\
                   \n\
gp_XYZ A(1,2,3);   \n\
gp_Pnt P2(A);      \n\
                   \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    DisplayPoint(aContext,P2,"P2 (1,2,3)",Standard_True,0.5);
}

//===============================================================
// Function name: gpTest4
//===============================================================
 void SampleGeometryPackage::gpTest4(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
    aContext->EraseAll(Standard_False);
//==============================================================
                                              
gp_Pnt P3 = gp::Origin();                     
Standard_Real TheX = P3.X();
Standard_Real TheY = P3.Y();
Standard_Real TheZ = P3.Z();
  
                                              
//==============================================================
    Message = "\
                               \n\
gp_Pnt P3 = gp::Origin();      \n\
Standard_Real TheX = P3.X();   \n\
Standard_Real TheY = P3.Y();   \n\
Standard_Real TheZ = P3.Z();   \n\
                               \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    DisplayPoint(aContext,P3,"P3 = gp::Origin()",Standard_True,0.5);
    Message += " TheX = "; Message += TCollection_AsciiString(TheX);
    Message += " TheY = "; Message += TCollection_AsciiString(TheY);
    Message += " TheZ = "; Message += TCollection_AsciiString(TheZ);
}

//===============================================================
// Function name: gpTest5
//===============================================================
 void SampleGeometryPackage::gpTest5(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
    aContext->EraseAll(Standard_False);
//==============================================================
                               
gp_Pnt P1(1,2,3);              
gp_Pnt P2(3,4,5);              
gp_Pnt PB = P1;                
Standard_Real alpha = 3;       
Standard_Real beta = 7;        
PB.BaryCenter(alpha,P2,beta);  
                               
//==============================================================
    Message = "\
                                 \n\
gp_Pnt P1(1,2,3);                \n\
gp_Pnt P2(3,4,5);                \n\
gp_Pnt PB = P1;                  \n\
Standard_Real alpha = 3;         \n\
Standard_Real beta = 7;          \n\
PB.BaryCenter(alpha,P2,beta);    \n\
                                 \n";

    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayPoint(aContext,P1,"P1",Standard_False,0.2);
    DisplayPoint(aContext,P2,"P2",Standard_False,0.2);
    DisplayPoint(aContext,PB,"PB = barycenter ( 3 * P1 , 7 * P2) ",Standard_True,0.2);

    Message += " PB ( ";
    Message += TCollection_AsciiString(PB.X()); Message += " , ";
    Message += TCollection_AsciiString(PB.Y()); Message += " , ";
    Message += TCollection_AsciiString(PB.Z()); Message += " ); ";
}

//===============================================================
// Function name: gpTest6
//===============================================================
 void SampleGeometryPackage::gpTest6(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
    aContext->EraseAll(Standard_False);
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
if (PE.IsPoint()){IsPoint = Standard_True;                                
                  P = PE .Point();}                              
                  else { IsPoint = Standard_False;  }                     
if (PE.IsLinear()){ /*... */ }  
if (PE.IsPlanar()){ /*... */ }   
if (PE.IsSpace()) { /*... */ }     
                                                                 
//==============================================================
    Message = "\
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
                                                                 \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    TCollection_AsciiString PointName("P");
    for(Standard_Integer i= array.Lower();i <= array.Upper(); i++)
    {
      TCollection_AsciiString TheString (PointName + TCollection_AsciiString(i));
      DisplayPoint(aContext,array(i),TheString,Standard_False,0.5);
    }

    DisplayPoint(aContext,P,"P",Standard_True,0.5);


    Message += " IsPoint = ";  if (IsPoint) {
     Message += "True   -->  ";
     Message += " P ( ";
     Message += TCollection_AsciiString(P.X()); Message += " , ";
     Message += TCollection_AsciiString(P.Y()); Message += " , ";
     Message += TCollection_AsciiString(P.Z()); Message += " ); \n";
    }  else Message += "False\n";
    Message += " IsLinear = ";  if (PE.IsLinear()) Message += "True \n";  else Message += "False\n";
    Message += " IsPlanar = ";  if (PE.IsPlanar()) Message += "True \n";  else Message += "False\n";
    Message += " IsSpace = ";   if (PE.IsSpace() ) Message += "True \n";  else Message += "False\n";
}

//===============================================================
// Function name: gpTest7
//===============================================================
 void SampleGeometryPackage::gpTest7(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
    aContext2D->EraseAll();
//==============================================================
                                           
gp_Pnt2d P1(0,5);                                       
gp_Pnt2d P2(5.5,1);                                     
gp_Pnt2d P3(-2,2);                                      
                                                        
Handle(Geom2d_TrimmedCurve) C =                         
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
    Message = "\
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
                                                    \n";

    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayCurve(aContext2D,C);
    Handle(ISession_Direction) aDirection = new ISession_Direction(P,V);
    aContext2D->Display(aDirection);

    DisplayPoint(aContext2D,P,"P",Standard_True,0.5);
}

//===============================================================
// Function name: gpTest8
//===============================================================
 void SampleGeometryPackage::gpTest8(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
    aContext2D->EraseAll();
//==============================================================
                                                   
Standard_Real radius = 5;                          
Handle(Geom2d_Circle) C =                          
    new Geom2d_Circle(gp::OX2d(),radius);          
Standard_Real param = 1.2*PI;                      
Geom2dLProp_CLProps2d CLP                          
    (C,param,2,Precision::PConfusion());           
    gp_Dir2d D;                                    
CLP.Tangent(D);                                    
// D is the Tangent direction at parameter 1.2*PI  
                                                   
//==============================================================
    Message = " \
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
                                                   \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(C);
    aContext2D->Display(aCurve,Standard_False);
    Handle(ISession_Direction) aDirection = new ISession_Direction(gp_Pnt2d(0,0),D,2);
    aContext2D->Display(aDirection);

     Message += " D ( ";
     Message += TCollection_AsciiString(D.X()); Message += " , ";
     Message += TCollection_AsciiString(D.Y()); Message += " ); \n";
}

//===============================================================
// Function name: gpTest9
//===============================================================
 void SampleGeometryPackage::gpTest9(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
    aContext2D->EraseAll();
//==============================================================
                                                             
Standard_Real radius = 5;                                    
Handle(Geom2d_Circle) C =                                    
    new Geom2d_Circle(gp::OX2d(),radius);                    
Geom2dAdaptor_Curve GAC (C);                                 
Standard_Real startparam = 10*PI180;                                
Standard_Real abscissa = 45*PI180;                                  
gp_Pnt2d P1;
C->D0(startparam,P1);                                                
// abscissa is the distance along the curve from startparam  
GCPnts_AbscissaPoint AP (GAC, abscissa, startparam);         
gp_Pnt2d P2;                                                  
if (AP.IsDone()){C->D0(AP.Parameter(),P2);}                   
// P is now correctly set                                    
                                                             
//==============================================================
    Message = " \n\
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
                                                             \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(C);
    aContext2D->Display(aCurve,Standard_False);

    DisplayPoint(aContext2D,P1,"P1");
    if (AP.IsDone())    DisplayPoint(aContext2D,P2,"P2");

     Message += " P1 ( ";
     Message += TCollection_AsciiString(P1.X()); Message += " , ";
     Message += TCollection_AsciiString(P1.Y()); Message += " ); \n";

     Message += " P2 ( ";
     Message += TCollection_AsciiString(P2.X()); Message += " , ";
     Message += TCollection_AsciiString(P2.Y()); Message += " ); \n";
}

//===============================================================
// Function name: gpTest10
//===============================================================
 void SampleGeometryPackage::gpTest10(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
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
        UA.Parameter(count);  
        // append P in a Sequence                       
        aSequence.Append(P);                            
    }                                                   
}                                                       
Standard_Real Abscissa  = UA.Abscissa();                
                                                        
//==============================================================
    Message = " \
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
                                                        \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(C);
    aContext2D->Display(aCurve,Standard_False);

    TCollection_AsciiString aString;
    for (Standard_Integer i=1;i<= aSequence.Length();i++)
    {
     aString = "P";aString += TCollection_AsciiString(i);
      aString +=": Parameter : "; aString +=TCollection_AsciiString(UA.Parameter(i));
     //   First and Last texts are displayed with an Y offset, point 4 is upper
     Standard_Real YOffset = -0.3;
     YOffset +=  0.2 * ( i == 1 ) ;
     YOffset +=  0.4 * ( i == 4 ) ;
     YOffset += -0.2 * ( i == aSequence.Length() ); 

     DisplayPoint(aContext2D,aSequence(i),aString,Standard_False,0.5,YOffset,0.04);
    }
    Message += "Abscissa  = "; Message += TCollection_AsciiString(Abscissa); Message += " \n";
}

//===============================================================
// Function name: gpTest11
//===============================================================
 void SampleGeometryPackage::gpTest11(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
//==============================================================
                                                          
Standard_Real radius = 5;                                 
Handle(Geom_SphericalSurface) SP =                        
    new Geom_SphericalSurface(gp_Ax3(gp::XOY()),radius);  
Standard_Real u = 2;                                      
Standard_Real v = 3;                                      
gp_Pnt P = SP->Value(u,v);                                
                                                          
//==============================================================
    Message = " \
                                                          \n\
Standard_Real radius = 5;                                 \n\
Handle(Geom_SphericalSurface) SP =                        \n\
    new Geom_SphericalSurface(gp_Ax3(gp::XOY()),radius);  \n\
Standard_Real u = 2;                                      \n\
Standard_Real v = 3;                                      \n\
gp_Pnt P = SP->Value(u,v);                                \n\
                                                          \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplaySurface(aContext,SP);
    DisplayPoint(aContext,P,"P",Standard_True,0.5);

    Message += " P ( ";
    Message += TCollection_AsciiString(P.X()); Message += " , ";
    Message += TCollection_AsciiString(P.Y()); Message += " ); \n";
}

//===============================================================
// Function name: gpTest12
//===============================================================
 void SampleGeometryPackage::gpTest12(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
//==============================================================
                                                            
gp_Pnt N,Q,P(1,2,3);                                        
Standard_Real distance, radius = 5;                         
Handle(Geom_Circle) C = new Geom_Circle(gp::XOY(),radius);  
GeomAPI_ProjectPointOnCurve PPC (P,C);                      
N = PPC.NearestPoint();                                     
Standard_Integer NbResults = PPC.NbPoints();                
                                                            
if(NbResults>0){                                            
    for(Standard_Integer i = 1;i<=NbResults;i++){           
      Q = PPC.Point(i);                                     
      distance = PPC.Distance(i);                           
        // do something with Q or distance here             
      }                                                     
 }                                                          
                                                            
//==============================================================
    Message = " \
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
                                                            \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    TCollection_AsciiString aString;

    DisplayPoint(aContext,P,"P",Standard_True,0.5);

    aString = "N : at Distance : "; aString += TCollection_AsciiString(PPC.LowerDistance());
    DisplayPoint(aContext,N,aString,Standard_False,0.5,0,-0.5);

    DisplayCurve(aContext,C,Standard_False);

	if(NbResults>0){
		for(Standard_Integer i = 1;i<=NbResults;i++){
		Q = PPC.Point(i);
		distance = PPC.Distance(i);	
        
        aString = "Q";aString += TCollection_AsciiString(i); 
        aString +=": at Distance : "; aString += TCollection_AsciiString(distance);
        DisplayPoint(aContext,Q,aString,Standard_False,0.5);
		}
	}
}

//===============================================================
// Function name: gpTest13
//===============================================================
 void SampleGeometryPackage::gpTest13(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
//==============================================================
                                                          
gp_Pnt N,Q,P(7,8,9);                                      
Standard_Real distance, radius = 5;                       
Handle(Geom_SphericalSurface) SP =                        
    new Geom_SphericalSurface(gp_Ax3(gp::XOY()),radius);  
GeomAPI_ProjectPointOnSurf PPS(P,SP);                     
N = PPS.NearestPoint();                                   
Standard_Integer NbResults = PPS.NbPoints();              
if(NbResults>0){                                          
    for(Standard_Integer i = 1;i<=NbResults;i++){         
    Q = PPS.Point(i);                                     
    distance = PPS.Distance(i);                           
    // do something with Q or distance here               
    }                                                     
}                                                         
                                                          
//==============================================================
    Message = " \
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
                                                          \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    TCollection_AsciiString aString;

    DisplayPoint(aContext,P,"P",Standard_False,0.5);

    aString = "N  : at Distance : "; aString += TCollection_AsciiString(PPS.LowerDistance());
    DisplayPoint(aContext,N,aString,Standard_False,0.5,0,-0.6);



    Handle(ISession_Surface) aSurface = new ISession_Surface(SP);
	Handle (AIS_Drawer) CurDrawer = aSurface->Attributes();
    CurDrawer->UIsoAspect()->SetNumber(10);
    CurDrawer->VIsoAspect()->SetNumber(10);
    aContext->SetLocalAttributes(aSurface, CurDrawer);
    aContext->Display(aSurface);

	if(NbResults>0){
		for(Standard_Integer i = 1;i<=NbResults;i++){
		Q = PPS.Point(i);
		distance = PPS.Distance(i);	
        
        aString = "Q";aString += TCollection_AsciiString(i); 
        aString +=": at Distance : "; aString += TCollection_AsciiString(distance);
        DisplayPoint(aContext,Q,aString,Standard_False,0.5);
		}
	}
}

//===============================================================
// Function name: gpTest14
//===============================================================
 void SampleGeometryPackage::gpTest14(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
//==============================================================
                                                          
gp_Pnt P;                                                 
gp_Ax2 aXOY = gp::XOY();
gp_Ax3 aAx3(aXOY);
gp_Pln PL(aAx3);
//gp_Pln PL (gp_Ax3(gp::XOY()));                            
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
    Message = " \
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
                                                          \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    Handle(Geom_Plane) aPlane = GC_MakePlane(PL).Value();
    Handle(Geom_RectangularTrimmedSurface) aSurface= new Geom_RectangularTrimmedSurface(aPlane,-8.,8.,-12.,12.);
        
    DisplaySurface(aContext,aSurface);


    Handle(Geom_Ellipse) anEllips = GC_MakeEllipse(EL).Value();
    DisplayCurve(aContext,anEllips,Standard_False);

    TCollection_AsciiString aString;

	if (ICQ.IsDone()){
		Standard_Integer NbResults = ICQ.NbPoints();
		if (NbResults>0){
		for(Standard_Integer i = 1;i<=NbResults;i++){
			P = ICQ.Point(i);
            aString = "P";aString += TCollection_AsciiString(i); 
            DisplayPoint(aContext,P,aString,Standard_False,0.5);
			}
		   }
		}
}

//===============================================================
// Function name: gpTest15
//===============================================================
 void SampleGeometryPackage::gpTest15(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
//==============================================================
                     
gp_Pnt P1(1,2,3);    
gp_Pnt P1Copy = P1;  
gp_Pnt P2(5,4,6);    
gp_Trsf TRSF;        
TRSF.SetMirror(P2);  
P1Copy.Transform(TRSF);  
                     
//==============================================================
    Message = " \
                         \n\
gp_Pnt P1(1,2,3);        \n\
gp_Pnt P1Copy = P1;      \n\
gp_Pnt P2(5,4,6);        \n\
gp_Trsf TRSF;            \n\
TRSF.SetMirror(P2);      \n\
P1Copy.Transform(TRSF);  \n\
                         \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayPoint(aContext,P1Copy,"P1Copy",Standard_False,0.5);
    DisplayPoint(aContext,P1,"P1",Standard_False,0.5);
    DisplayPoint(aContext,P2,"P2",Standard_False,0.5);
}

//===============================================================
// Function name: gpTest16
//===============================================================
 void SampleGeometryPackage::gpTest16(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
    Message = " \
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
                                         \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayPoint(aContext,P1,"P1",Standard_False,0.5);
    DisplayPoint(aContext,P2,"P2",Standard_False,0.5);
    DisplayPoint(aContext,P3,"P3",Standard_False,0.5);
    DisplayPoint(aContext,P4,"P4",Standard_False,0.5);

    Handle(ISession_Direction) aDirection1 = new ISession_Direction(P1,V1);
    aContext->Display(aDirection1);

    Handle(ISession_Direction) aDirection2 = new ISession_Direction(P3,V2);
    aContext->Display(aDirection2);

    Message += "result = ";
    if (result) Message += "True \n"; else Message += "False \n";
}

//===============================================================
// Function name: gpTest17
//===============================================================
 void SampleGeometryPackage::gpTest17(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
//==============================================================
                                                 
gp_Dir D1(1,2,3);                                
gp_Dir D2(3,4,5);                                
Standard_Real ang = D1.Angle(D2);                
// the result is in radians in the range [0,PI]  
                                                 
//==============================================================
    Message = " \
                                                 \n\
gp_Dir D1(1,2,3);                                \n\
gp_Dir D2(3,4,5);                                \n\
Standard_Real ang = D1.Angle(D2);                \n\
// the result is in radians in the range [0,PI]  \n\
                                                 \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    Handle(ISession_Direction) aDirection1 = new ISession_Direction(gp_Pnt(0,0,0),D1,3);
    aContext->Display(aDirection1);

    Handle(ISession_Direction) aDirection2 = new ISession_Direction(gp_Pnt(0,0,0),D2,3);
    aContext->Display(aDirection2);
    
    cout<<" D1.Angle(D2) : "<<ang<<endl;

    Message += " ang =  "; Message += TCollection_AsciiString(ang); Message += "   radian \n";
    Message += " ang/PI180 =  "; Message += TCollection_AsciiString(ang/PI180); Message += "   degree \n";
}

//===============================================================
// Function name: gpTest18
//===============================================================
 void SampleGeometryPackage::gpTest18(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
//==============================================================
                                          
gp_Pnt2d P(2,3);                          
gp_Dir2d D(4,5);                          
gp_Ax22d A(P,D);                          
gp_Parab2d Para(A,6);                     
// P is the vertex point                  
// P and D give the axis of symmetry      
// 6 is the focal length of the parabola  
                                          
//==============================================================
    Message = " \
                                          \n\
gp_Pnt2d P(2,3);                          \n\
gp_Dir2d D(4,5);                          \n\
gp_Ax22d A(P,D);                          \n\
gp_Parab2d Para(A,6);                     \n\
// P is the vertex point                  \n\
// P and D give the axis of symmetry      \n\
// 6 is the focal length of the parabola  \n\
                                          \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayPoint(aContext2D,P,"P",Standard_False,0.5,0,3);

    Handle(ISession_Direction) aDirection = new ISession_Direction(P,D,200);
    aContext2D->Display(aDirection,Standard_False);
    Handle(Geom2d_Parabola) aParabola = GCE2d_MakeParabola(Para);
    Handle(Geom2d_TrimmedCurve) aTrimmedCurve = new Geom2d_TrimmedCurve(aParabola,-100,100);
    Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(aTrimmedCurve);
    //aCurve->SetColorIndex(3);
    aContext2D->Display(aCurve);  

    Message += " The entity A of type gp_Ax22d is not displayable \n ";
    Message += " The entity D of type gp_Dir2d is displayed as a vector \n    ( mean with a length != 1 ) \n ";
}

//===============================================================
// Function name: gpTest19
//===============================================================
 void SampleGeometryPackage::gpTest19(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
    Message = " \
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
                                            \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayPoint(aContext,P1,"P1",Standard_False,0.1);
    Handle(ISession_Direction) aDirection = new ISession_Direction(P1,D,2);
    aContext->Display(aDirection);

    Handle(ISession_Direction) aDirection2 = new ISession_Direction(P1,AXDirection,2);
    aDirection2->SetText(TCollection_ExtendedString("A.XDirection"));
    aContext->Display(aDirection2);
    Handle(ISession_Direction) aDirection3 = new ISession_Direction(P1,AYDirection,2);
    aDirection3->SetText(TCollection_ExtendedString("A.YDirection"));
    aContext->Display(aDirection3);

    DisplayPoint(aContext,P2,"P2",Standard_False,0.1);
    Handle(ISession_Direction) aDirection4 = new ISession_Direction(P2,D,2);
    aContext->Display(aDirection4);

    Handle(ISession_Direction) aDirection5 = new ISession_Direction(P2,A2XDirection,2);
    aDirection5->SetText(TCollection_ExtendedString("A2 XDirection"));
    aContext->Display(aDirection5);
    Handle(ISession_Direction) aDirection6 = new ISession_Direction(P2,A2YDirection,2);
    aDirection6->SetText(TCollection_ExtendedString("A2 YDirection"));
    aContext->Display(aDirection6);

    Message += "IsDirectA = ";
    if (IsDirectA) Message += "True = Right Handed \n"; else Message += "False = Left Handed \n";

    Message += "IsDirectA2 = ";
    if (IsDirectA2) Message += "True = Right Handed \n"; else Message += "False = Left Handed  \n";
}

//===============================================================
// Function name: gpTest20
//===============================================================
 void SampleGeometryPackage::gpTest20(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
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
    Message = " \
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
    new TColgp_HArray1OfPnt2d (1,5); // sizing harray                \n";
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
    AddSeparator(Message);
    //--------------------------------------------------------------
    TCollection_AsciiString aString;
    Standard_Integer i;
    for(i = array.Lower();i<=array.Upper();i++){
		gp_Pnt2d P = array(i);
        aString = "array ";aString += TCollection_AsciiString(i); 
        DisplayPoint(aContext2D,P,aString,Standard_False,0.5);
		}
    for( i = harray->Lower();i<=harray->Upper();i++){
		gp_Pnt2d P = harray->Value(i);
        aString = "harray ";aString += TCollection_AsciiString(i); 
        DisplayPoint(aContext2D,P,aString,Standard_False,0.5);
		}
    for( i = harray2->Lower();i<=harray2->Upper();i++){
		gp_Pnt2d P = harray2->Value(i);
        aString = "harray2 ";aString += TCollection_AsciiString(i); 
        DisplayPoint(aContext2D,P,aString,Standard_False,0.5);
		}

    if (!SPL1.IsNull())
    { 
      Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(SPL1);
      aCurve->SetColorIndex(3);
      aContext2D->Display(aCurve);
    }
    
    if (!SPL2.IsNull())
    {
      Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(SPL2);
      aCurve2->SetColorIndex(5);
      aContext2D->Display(aCurve2);  
    }

    if (!SPL3.IsNull())
    {
      Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(SPL3);
      aCurve2->SetColorIndex(6);
      aContext2D->Display(aCurve2);  
    }

    Message += " SPL1  is Red  \n";
    Message += " SPL2  is Blue \n";   
    Message += " SPL3  is Yellow \n";   
}

//===============================================================
// Function name: gpTest21
//===============================================================
 void SampleGeometryPackage::gpTest21(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
//==============================================================
                                            
gp_Pnt2d P1(-184, 101);                     
gp_Pnt2d P2(20 ,84);                        
Standard_Real aheight = 1;                  
FairCurve_Batten B (P1,P2,aheight);         
B.SetAngle1(22*PI180);                      
B.SetAngle2(44*PI180);                      
FairCurve_AnalysisCode anAnalysisCode;      
B.Compute(anAnalysisCode);                  
Handle(Geom2d_BSplineCurve) C = B.Curve();  
                                            
//==============================================================
    Message = " \
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
                                            \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayCurveAndCurvature(aContext2D,C,6,Standard_True);               
}

//===============================================================
// Function name: gpTest22
//===============================================================
 void SampleGeometryPackage::gpTest22(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
//==============================================================
                                                
gp_Pnt2d P1(-184, 41);                          
gp_Pnt2d P2(20 ,24);                            
Standard_Real aheight = 1;                      
FairCurve_MinimalVariation MV (P1,P2,aheight);  
MV.SetAngle1(22*PI180);                         
MV.SetAngle2(44*PI180);                         
                                                
FairCurve_AnalysisCode anAnalysisCode;          
MV.Compute(anAnalysisCode);                     
                                                
Handle(Geom2d_BSplineCurve) C = MV.Curve();     
                                                
//==============================================================
    Message = " \
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
                                                \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayCurveAndCurvature(aContext2D,C,7,Standard_True);                   
    DisplayPoint(aContext2D,P1,"P1",Standard_False,0.5);
    DisplayPoint(aContext2D,P2,"P2",Standard_False,0.5);
}

//===============================================================
// Function name: gpTest23
//===============================================================
 void SampleGeometryPackage::gpTest23(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
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
    Message = " \
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
                                                                   \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(E);
    aCurve->SetColorIndex(3); // Red
    aCurve->SetTypeOfLine(Aspect_TOL_DOTDASH);
    aContext2D->Display(aCurve);

    Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(SPL);
    aContext2D->Display(aCurve2);
}

//===============================================================
// Function name: gpTest24
//===============================================================
 void SampleGeometryPackage::gpTest24(const Handle(AIS_InteractiveContext)& aContext,
				      const Handle(ISession2D_InteractiveContext)& aContext2D,
				      TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
  aContext2D->EraseAll();
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
    Message = " \
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
                                                        \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    Handle(Geom_Plane) aPlane = GC_MakePlane(gp_Pln(gp_Ax3(gp::XOY()))).Value();
    Handle(Geom_RectangularTrimmedSurface) aSurface= new Geom_RectangularTrimmedSurface(aPlane,-8.,8.,-12.,12.);   
    DisplaySurface(aContext,aSurface);

    Handle(Geom_Plane) aProjectionPlane = GC_MakePlane(ProjectionPlane).Value();
    Handle(Geom_RectangularTrimmedSurface) aProjectionPlaneSurface=
        new Geom_RectangularTrimmedSurface(aProjectionPlane,-8.,8.,-12.,12.);
        
    DisplaySurface(aContext,aProjectionPlaneSurface);

    Standard_CString aC3DEntityTypeName = C3D->DynamicType()->Name();        
    Standard_CString aC2DEntityTypeName = C2D->DynamicType()->Name();        

    Message += " C3D->DynamicType()->Name() = ";
    Message += aC3DEntityTypeName; Message += " \n";
    Message += " C2D->DynamicType()->Name() = ";
    Message += aC2DEntityTypeName; Message += " \n";

    DisplayCurve(aContext2D,circ2d,4,Standard_False);
    DisplayCurve(aContext,C3D,Standard_False);
    DisplayCurve(aContext2D,C2D,5,Standard_False);

    Handle(ISession_Direction) aC3DCircleXAxisDirection = new ISession_Direction((gp_Pnt)C3DCircleXAxis.Location(),
                                                                                 (gp_Dir)C3DCircleXAxis.Direction(),
                                                                                 5.2);
    aContext->Display(aC3DCircleXAxisDirection);

    Handle(ISession_Direction) acirc2dXAxisDirection = new ISession_Direction((gp_Pnt2d)circ2dXAxis.Location(),
                                                                              (gp_Dir2d)circ2dXAxis.Direction(),
                                                                              5.2);
    aContext2D->Display(acirc2dXAxisDirection);

    Handle(ISession_Direction) aC2DCircleXAxisDirection = new ISession_Direction((gp_Pnt2d)C2DCircleXAxis.Location(),
                                                                              (gp_Dir2d)C2DCircleXAxis.Direction(),
                                                                              5.2);
    aContext2D->Display(aC2DCircleXAxisDirection);
}

//===============================================================
// Function name: gpTest25
//===============================================================
 void SampleGeometryPackage::gpTest25(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
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
Handle(Geom2d_TrimmedCurve) TC1=                                    
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
    Message = " \
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
                                                                    \n";
    AddSeparator(Message);
   //--------------------------------------------------------------

    Handle(ISession2D_Curve) aCurve1 = new ISession2D_Curve(SPL);
    aCurve1->SetDisplayPole(Standard_False);
    aContext2D->Display(aCurve1);
    Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(TC1);
    aContext2D->Display(aCurve2);

    TCollection_AsciiString aString;
	for (Standard_Integer i = 1;i<=NbPoints;i++)
	{
		PK = ICC.Point(i);
		// do something with each intersection point
        aString = "PK_";aString += TCollection_AsciiString(i); 
        DisplayPoint(aContext2D,PK,aString,Standard_False,0.5);
        Message += "PK_"; Message += TCollection_AsciiString(i); Message += " ( ";
        Message += TCollection_AsciiString(PK.X()); Message += " , "; 
        Message += TCollection_AsciiString(PK.Y()); Message += " )\n"; 
	}
}

//===============================================================
// Function name: gpTest26
//===============================================================
 void SampleGeometryPackage::gpTest26(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
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
int i;
for(i=1;i <= NbExtrema; i++)   {                                  
    gp_Pnt2d Ploc1,Ploc2;                                                   
    ECC2.Points(i,Ploc1,Ploc2);                                             
    aSolutionArray(i,1) = Ploc1;  aSolutionArray(i,2) = Ploc2; }            
                                                                      
//==============================================================
    Message = " \
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
Standard_Real FPSPL1 = SPL1->FirstParameter();                        \n";
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
    AddSeparator(Message);
    //--------------------------------------------------------------

    TCollection_AsciiString aString;
    for(i = array.Lower();i<=array.Upper();i++){
		gp_Pnt2d P = array(i);
        aString = "array ";aString += TCollection_AsciiString(i); 
        DisplayPoint(aContext2D,P,aString,Standard_False,0.5);
		}

    if (!SPL1.IsNull())
    { 
      Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(SPL1);
      aCurve->SetDisplayPole(Standard_False);
      aCurve->SetColorIndex(3);
      aContext2D->Display(aCurve);
    }

    Handle(ISession2D_Curve) aCurve1 = new ISession2D_Curve(TC1);
    aCurve1->SetColorIndex(6);
    aContext2D->Display(aCurve1);
    Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(TC2);
    aCurve2->SetColorIndex(5);
    aContext2D->Display(aCurve2);


    for(i=1;i <= NbExtrema; i++)
    {
        gp_Pnt2d Ploc1 =aSolutionArray(i,1);
        aString = "P1_";aString += TCollection_AsciiString(i); 
        DisplayPoint(aContext2D,P1,aString,Standard_False,0.7*i);

        gp_Pnt2d Ploc2 = aSolutionArray(i,2);
        
        Handle(Geom2d_TrimmedCurve) SolutionCurve =            
               GCE2d_MakeSegment(Ploc1,Ploc2);             
        Handle(ISession2D_Curve) aSolutionCurve = new ISession2D_Curve(SolutionCurve);
        aContext2D->Display(aSolutionCurve);
    }

    Message += "TC1 is  Yellow ,TC2 is  Blue ,SPL1 is Red \n";
    Message += "ECC.NbExtrema()  = "; Message += TCollection_AsciiString(ECC.NbExtrema());
    Message += "    shortestdistance = "; Message+= TCollection_AsciiString(shortestdistance); Message += "\n";
    Message += "ECC2.NbExtrema() = "; Message += TCollection_AsciiString(NbExtrema);
    Message += "    SPL1shortestdistance = "; Message+= TCollection_AsciiString(SPL1shortestdistance); Message += "\n";
}

//===============================================================
// Function name: gpTest27
//===============================================================
 void SampleGeometryPackage::gpTest27(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
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
    Message = " \
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
                                                                       \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    Handle(ISession2D_Curve) aCurve1 = new ISession2D_Curve(SPL1);
    aCurve1->SetColorIndex(6);
    aContext2D->Display(aCurve1);
    Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(OC);
    aCurve2->SetColorIndex(5);
    aContext2D->Display(aCurve2);
    Handle(ISession2D_Curve) aCurve3 = new ISession2D_Curve(OC2);
    aCurve3->SetColorIndex(3);
    aContext2D->Display(aCurve3);


    Message += "SPL1 is Yellow \n";
    Message += "OC   is Blue \n";
    Message += "OC2  is Red \n\n";
	Message += "  Warning, Continuity is not guaranteed :  \n ";
	if(result)    Message += " result  = True  \n";
    else          Message += " result  = False \n";
	if(result2)   Message += " result2 = True  \n";
    else          Message += " result2 = False \n";
}

//===============================================================
// Function name: gpTest28
//===============================================================
 void SampleGeometryPackage::gpTest28(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
//==============================================================
                                      
gp_Pnt2d P1(1,2);                     
gp_Pnt2d P2(4,5);                     
gp_Lin2d L = gce_MakeLin2d(P1,P2);    
// assignment by overloaded operator  
                                      
//==============================================================
    Message = " \
                                      \n\
gp_Pnt2d P1(1,2);                     \n\
gp_Pnt2d P2(4,5);                     \n\
gp_Lin2d L = gce_MakeLin2d(P1,P2);    \n\
// assignment by overloaded operator  \n\
                                      \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    
    DisplayPoint(aContext2D,P1,"P1",Standard_False,0.5);
    DisplayPoint(aContext2D,P2,"P2",Standard_False,0.5);

    Handle(Geom2d_TrimmedCurve) aLine = GCE2d_MakeSegment(L,-3,8);
    Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(aLine);
    aContext2D->Display(aCurve);
}

//===============================================================
// Function name: gpTest29
//===============================================================
 void SampleGeometryPackage::gpTest29(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
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
    Message = " \
                             \n\
gp_Pnt2d P1(1,2);            \n\
gp_Pnt2d P2(4,5);            \n\
gp_Lin2d L;                  \n\
GccAna_Pnt2dBisec B(P1,P2);  \n\
if (B.IsDone())              \n\
    {                        \n\
    L = B.ThisSolution();    \n\
    }                        \n\
                             \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayPoint(aContext2D,P1,"P1",Standard_False,0.5);
    DisplayPoint(aContext2D,P2,"P2",Standard_False,0.5);

	if (B.IsDone())
	  {
        Handle(Geom2d_TrimmedCurve) aLine = GCE2d_MakeSegment(L,-8,8);
        Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(aLine);
        aContext2D->Display(aCurve);
      }

	if (B.IsDone()) Message += " \n   B Is Done   ";
    else            Message += " \n   B Is not Done    ";
}

//===============================================================
// Function name: gpTest30
//===============================================================
 void SampleGeometryPackage::gpTest30(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
//==============================================================
                                                     
gp_Pnt2d P1 (2,3);                                   
gp_Pnt2d P2 (4,4);                                   
gp_Pnt2d P3 (6,7);                                   
gp_Pnt2d P4 (10,10);                                 
gp_Circ2d C = gce_MakeCirc2d (P1,P2,P3);             
GccEnt_QualifiedCirc QC = GccEnt::Outside(C);        
GccAna_Lin2d2Tan LT (QC,P4,Precision::Confusion());  
Standard_Integer NbSol=0;                              
if (LT.IsDone())                                     
  {                                                  
      NbSol = LT.NbSolutions();                      
      Standard_Integer k;
      for(k=1; k<=NbSol; k++)       
        {                                            
         LT.ThisSolution(k);            
          // do something with L                     
        }                                            
  }                                                  
                                                     
//==============================================================
    Message = " \
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
                                                     \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayPoint(aContext2D,P1,"P1",Standard_False,0.5,-1,0.1);
    DisplayPoint(aContext2D,P2,"P2",Standard_False,0.5,-0.7,0.1);
    DisplayPoint(aContext2D,P3,"P3",Standard_False,0.5,-0.5,0.1);
    DisplayPoint(aContext2D,P4,"P4",Standard_False,0.5,0,0.1);
        
    Handle(Geom2d_Circle) aCircle = new Geom2d_Circle(C);
    Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(aCircle);
    aCurve->SetColorIndex(5);
    aContext2D->Display(aCurve);

	if (LT.IsDone())
		{
		  Standard_Integer NblocSol = LT.NbSolutions();
		  Standard_Integer k;
		  for(k=1; k<=NblocSol; k++)
			{
			  gp_Lin2d L = LT.ThisSolution(k);
			  Handle(Geom2d_TrimmedCurve) alocLine = GCE2d_MakeSegment(L,-10,20);
			  Handle(ISession2D_Curve) alocCurve = new ISession2D_Curve(alocLine);
			  aContext2D->Display(alocCurve);
			}
		}
    Message += " C is Blue \n\n";
    Message += "LT.IsDone() = "; 
    if (LT.IsDone())  Message += "True \n"; else Message += "False \n";
    Message += "NbSol       = "; Message += TCollection_AsciiString(NbSol); Message += "\n";
}

//===============================================================
// Function name: gpTest31
//===============================================================
 void SampleGeometryPackage::gpTest31(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
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
    AddSeparator(Message);
//--------------------------------------------------------------
    DisplayPoint(aContext2D,P1,"P1",Standard_False,0.3);
    DisplayPoint(aContext2D,P2,"P2",Standard_False,0.3);
    DisplayPoint(aContext2D,P3,"P3",Standard_False,0.3);
    DisplayPoint(aContext2D,P4,"P4",Standard_False,0.3);
    DisplayPoint(aContext2D,P5,"P5",Standard_False,0.3);

    Handle(Geom2d_Circle) aCircle = new Geom2d_Circle(C);
    Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(aCircle);
    aCurve->SetColorIndex(3);

    aContext2D->Display(aCurve);
    Handle(Geom2d_TrimmedCurve) aLine = GCE2d_MakeSegment(L,-2,20);
    Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(aLine);
    aCurve2->SetColorIndex(5);
    aContext2D->Display(aCurve2);

	if (TR.IsDone())
		{
		  Standard_Integer NblocSol = TR.NbSolutions();
		  Standard_Integer k;
		  for (k=1; k<=NblocSol; k++)
			{
			  circ = TR.ThisSolution(k);
			  Handle(Geom2d_Circle) alocCircle = new Geom2d_Circle(circ);
			  Handle(ISession2D_Curve) alocCurve = new ISession2D_Curve(alocCircle);
			  aContext2D->Display(alocCurve);

			  // find the solution circle
		 	  TR.Tangency1(k,parsol,pararg,tangentpoint1);
			  // find the first tangent point			 			 			 
			  TR.Tangency2(k,parsol,pararg,tangentpoint2);
			  // find the second tangent point
			  DisplayPoint(aContext2D,tangentpoint1,"tangentpoint1",Standard_False,0.3);
			  DisplayPoint(aContext2D,tangentpoint2,"tangentpoint2",Standard_False,0.3);
			}
		}
    Message += "C is Red \n";
    Message += "L is Blue \n";
}

//===============================================================
// Function name: gpTest32
//===============================================================
 void SampleGeometryPackage::gpTest32(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
//==============================================================
                                                                        
Standard_Real major = 12;                                               
Standard_Real minor = 4;                                                
gp_Ax2d axis = gp::OX2d();                                              
gp_Elips2d EE(axis,major,minor);;                                           
Handle(Geom2d_TrimmedCurve) arc = GCE2d_MakeArcOfEllipse(EE,0.0,PI/4);  
                                                                        
//==============================================================
    Message = " \
                                                                        \n\
Standard_Real major = 12;                                               \n\
Standard_Real minor = 4;                                                \n\
gp_Ax2d axis = gp::OX2d();                                              \n\
gp_Elips2d EE(axis,major,minor);                                        \n\
Handle(Geom2d_TrimmedCurve) arc = GCE2d_MakeArcOfEllipse(EE,0.0,PI/4);  \n\
                                                                        \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    Handle(Geom2d_Curve) E = GCE2d_MakeEllipse(EE).Value();
    Handle(ISession2D_Curve) aCurve = new ISession2D_Curve(E);
    aCurve->SetColorIndex(3);
    aCurve->SetTypeOfLine(Aspect_TOL_DOTDASH);
    //SetWidthOfLine                 
    aContext2D->Display(aCurve);
    Handle(ISession2D_Curve) aCurve2 = new ISession2D_Curve(arc);
    aContext2D->Display(aCurve2);

    Message += " PI = ";Message+= TCollection_AsciiString(PI);
}

//===============================================================
// Function name: gpTest33
//===============================================================
 void SampleGeometryPackage::gpTest33(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
                                                                                        
//==============================================================
    Message = " \
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
    GeomAPI_PointsToBSpline(array).Curve();                                             \n";
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
    AddSeparator(Message);
    //--------------------------------------------------------------
    TCollection_AsciiString aString;
    for(Standard_Integer i = array.Lower();i<=array.Upper();i++){
		gp_Pnt P = array(i);
        aString = "P";aString += TCollection_AsciiString(i); 
        if (i == 1) aString += " (array)  ";
        DisplayPoint(aContext,P,aString,Standard_False,0.5);\
        aString = "P";aString += TCollection_AsciiString(i); 
        if (i == 1) aString += " (harray)  ";
        DisplayPoint(aContext,P.Translated(gp_Vec(4,0,0)),aString,Standard_False,0.5);\
        
		}

    Handle(ISession_Curve) aCurve = new ISession_Curve(SPL1);
    aContext->SetDisplayMode(aCurve,1);
    aContext->Display(aCurve);

    if (anInterpolation.IsDone()) {
        Handle(ISession_Curve) aCurve2 = new ISession_Curve(SPL2);
        aContext->SetDisplayMode(aCurve2,1);
        aContext->Display(aCurve2);
    }
}

//===============================================================
// Function name: gpTest34
//===============================================================
 void SampleGeometryPackage::gpTest34(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
//==============================================================
                                                                 
TColgp_Array1OfPnt array (1,5); // sizing array                  
array.SetValue(1,gp_Pnt(0,0,1));                                 
array.SetValue(2,gp_Pnt(1,2,2));                                 
array.SetValue(3,gp_Pnt(2,3,3));                                 
array.SetValue(4,gp_Pnt(4,4,4));                                 
array.SetValue(5,gp_Pnt(5,5,5));                                 
                                                                 
GProp_PEquation PE (array,1.5 );                                 
                                                                 
if (PE.IsPoint()){ /* ... */  }     
gp_Lin L;                                                        
if (PE.IsLinear()) {  L = PE.Line();    }                      
if (PE.IsPlanar()){ /* ... */  }     
if (PE.IsSpace()) { /* ... */  }     
                                                                 
//==============================================================
    Message = " \
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
                                                \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    TCollection_AsciiString aString;
    for(Standard_Integer i = array.Lower();i<=array.Upper();i++){
		gp_Pnt P = array(i);
        aString = "P";aString += TCollection_AsciiString(i); 
        DisplayPoint(aContext,P,aString,Standard_False,0.5);
		}

    Message += " PE.IsPoint()  = ";  if (PE.IsPoint()) Message += "True \n";  else Message += "False\n";

	if (PE.IsLinear()) { 
         Message += " PE.IsLinear() = True \n";
         L = PE.Line();
         Handle(Geom_Line) aLine = new Geom_Line(L);
         Handle(Geom_TrimmedCurve) aTrimmedCurve = new Geom_TrimmedCurve(aLine,-10,10);
         Handle(ISession_Curve) aCurve = new ISession_Curve(aTrimmedCurve);
         aContext->Display(aCurve);
    }
    else
    Message += "PE.IsLinear() = False \n";

    Message += " PE.IsPlanar() = ";  if (PE.IsPlanar()) Message += "True \n";  else Message += "False\n";
    Message += " PE.IsSpace() = ";   if (PE.IsSpace() ) Message += "True \n";  else Message += "False\n";
}

//===============================================================
// Function name: gpTest35
//===============================================================
 void SampleGeometryPackage::gpTest35(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
                                                            
Standard_Integer NbSeg=0;                                     
Standard_Integer NbPoints=0;                                  
if(CS.IsDone())                                             
    {                                                       
      NbSeg = CS.NbSegments();
      Standard_Integer k;
      for (k=1; k<=NbSeg; k++)             
        {                                                   
          segment = CS.Segment(k);                          
         // do something with the segment                   
        }                                                   
                                                            
      NbPoints = CS.NbPoints();                             
      for (k=1; k<=NbPoints; k++)                           
        {                                                   
//          gp_Pnt aPoint=
	    CS.Point(k);                        
          // do something with the point                    
        }                                                   
    }                                                       
                                                            
//==============================================================
    Message = " \
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
                                                            \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
 
    Handle(ISession_Curve) aCurve2 = new ISession_Curve(aCurve);
    aContext->Display(aCurve2);

    Handle(Geom_RectangularTrimmedSurface) aTrimmedSurface= new Geom_RectangularTrimmedSurface(aSurface,-50.,50.,Standard_False);

    DisplaySurface(aContext,aTrimmedSurface);

    TCollection_AsciiString aString;
    if(CS.IsDone())
		{
		  Standard_Integer NblocSeg = CS.NbSegments();
		  Standard_Integer k;
		  for (k=1; k<=NblocSeg; k++)
			{
			  segment = CS.Segment(k);
			  aString = "S_";aString += TCollection_AsciiString(k);
			  Handle(ISession_Curve) alocCurve = new ISession_Curve(segment);
			  aContext->Display(alocCurve);
			}
        
		  for ( k=1; k<=NbPoints; k++)              
		    {                                                   
		      gp_Pnt aPoint=CS.Point(k);
		      aString = "P_";aString += TCollection_AsciiString(k); 
		      DisplayPoint(aContext,aPoint,aString,Standard_False,0.5);
		      // do something with the point            
		    }                                                  
		}

  Message += "NbSeg       = "; Message += TCollection_AsciiString(NbSeg); Message += "\n";
  Message += "NbPoints  = "; Message += TCollection_AsciiString(NbPoints); Message += "\n";
}

//===============================================================
// Function name: gpTest36
//===============================================================
 void SampleGeometryPackage::gpTest36(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
//==============================================================
                                                                                    
gp_Pnt centre (5,5,0);  gp_Pnt axispoint (9,9,0);                                   
Standard_Real radius = 3;                                                           
Handle(Geom_Circle) circle =                                                        
    GC_MakeCircle(centre,axispoint,radius);                                         
                                                                                    
Handle(Geom_Geometry) aRotatedEntity  = circle->Rotated(gp::OZ(),PI/4);             
Standard_CString aRotatedEntityTypeName = aRotatedEntity->DynamicType()->Name();        
                                                                                    
Handle(Geom_Geometry) aMirroredEntity = aRotatedEntity->Mirrored(gp::ZOX());        
Standard_CString aMirroredEntityTypeName = aMirroredEntity->DynamicType()->Name();      
                                                                                    
gp_Pnt scalepoint (4,8,0);                                                          
Standard_Real scalefactor = 0.2;                                                    
Handle(Geom_Geometry) aScaledEntity =                                               
     aMirroredEntity->Scaled(scalepoint, scalefactor);                              
Standard_CString aScaledEntityTypeName = aScaledEntity->DynamicType()->Name();          
                                                                                    
Handle (Geom_Transformation) GT =   GC_MakeTranslation  (centre, scalepoint);       
gp_Trsf TR = GT->Trsf();                                                            
                                                                                    
gp_Vec aTranslationVector(TR.TranslationPart ());                                   
Handle(Geom_Geometry) aTranslatedEntity =                                           
       aScaledEntity->Translated(  aTranslationVector  );                           
Standard_CString aTranslatedEntityTypeName = aTranslatedEntity->DynamicType()->Name();  
                                                                                    
gp_Mat matrix = TR.HVectorialPart();                                                
Standard_Real value = matrix.Determinant();                                         
                                                                                    
//==============================================================
    Message = " \
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
                                                                                        \n";
   Message +="\
gp_Vec aTranslationVector(TR.TranslationPart ());                                       \n\
Handle(Geom_Geometry) aTranslatedEntity =                                               \n\
       aScaledEntity->Translated(  aTranslationVector  );                               \n\
Standard_CString aTranslatedEntityTypeName = aTranslatedEntity->DynamicType()->Name();  \n\
                                                                                        \n\
gp_Mat matrix = TR.HVectorialPart();                                                    \n\
Standard_Real value = matrix.Determinant();                                             \n\
                                                                                        \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayPoint(aContext,centre,"centre",Standard_False,0.5);
    DisplayPoint(aContext,axispoint,"axispoint",Standard_False,0.5);
    DisplayPoint(aContext,scalepoint,"scalepoint",Standard_False,0.5);

    DisplayCurve(aContext,circle, Quantity_NOC_RED,Standard_False);
    DisplayCurve(aContext,Handle(Geom_Curve)::DownCast(aRotatedEntity),Quantity_NOC_PEACHPUFF, Standard_False);
    DisplayCurve(aContext,Handle(Geom_Curve)::DownCast(aMirroredEntity), Quantity_NOC_YELLOWGREEN,Standard_False);
    DisplayCurve(aContext,Handle(Geom_Curve)::DownCast(aScaledEntity), Quantity_NOC_GREEN,Standard_False);
    DisplayCurve(aContext,Handle(Geom_Curve)::DownCast(aTranslatedEntity),Quantity_NOC_WHITE,Standard_False);

    Message += " PI = ";Message+= TCollection_AsciiString(PI); Message += "\n";
    Message += " circle is Red;  aRotatedEntity is Peach; aMirroredEntity is Yellow Green\n";
    Message += " aScaleEntity is Green; aTranslatedEntity is White\n\n";

    Message += " aTranslationVector ( "; 
    Message += TCollection_AsciiString(aTranslationVector.X()); Message += " , ";
    Message += TCollection_AsciiString(aTranslationVector.Y()); Message += " , ";
    Message += TCollection_AsciiString(aTranslationVector.Z()); Message += " ); \n";

    Message += " value = ";Message+= TCollection_AsciiString(value); Message += "\n";

    Message += " aRotatedEntityTypeName     = ";Message+= aRotatedEntityTypeName; Message += "\n";
    Message += " aMirroredEntityTypeName     = ";Message+= aMirroredEntityTypeName; Message += "\n";
    Message += " aScaledEntityTypeName       = ";Message+= aScaledEntityTypeName; Message += "\n";
    Message += " aTranslatedEntityTypeName  = ";Message+= aTranslatedEntityTypeName; Message += "\n";
}

//===============================================================
// Function name: gpTest37
//===============================================================
 void SampleGeometryPackage::gpTest37(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
                                                                 
if (PE.IsPoint())  { /* ... */  }     
if (PE.IsLinear()) { /* ... */  }     
if (PE.IsPlanar()) { P = PE.Plane();}                                             
if (PE.IsSpace())  { /* ... */  }     
                                                                 
//==============================================================
    Message = " \
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
                                                                 \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    TCollection_AsciiString aString;
    for(Standard_Integer i = anArrayofPnt.Lower();i<=anArrayofPnt.Upper();i++){
		gp_Pnt P = anArrayofPnt(i);
        aString = "P";aString += TCollection_AsciiString(i); 
        DisplayPoint(aContext,P,aString,Standard_False,0.5);
		}

    Message += " PE.IsPoint()  = ";  if (PE.IsPoint()) Message += "True \n";  else Message += "False\n";
    Message += " PE.IsLinear() = ";  if (PE.IsLinear()) Message += "True \n";  else Message += "False\n";

	if (PE.IsPlanar()) { 
         Message +=  " PE.IsPlanar() = True \n";
         P = PE.Plane();
         Handle(Geom_Plane) aPlane = new Geom_Plane(P);
         Handle(Geom_RectangularTrimmedSurface) aSurface= new Geom_RectangularTrimmedSurface(aPlane,-4.,4.,-4.,4.);

         DisplaySurface(aContext,aSurface);

    }
    else
    Message += " PE.IsPlanar() = False \n";

    Message += " PE.IsSpace() = ";   if (PE.IsSpace() ) Message += "True \n";  else Message += "False\n";
}

//===============================================================
// Function name: gpTest38
//===============================================================
 void SampleGeometryPackage::gpTest38(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
Handle(Geom_BSplineSurface)    aBSplineSurface1 = aGeomFill1.Surface();               
                                                                                      
Type = GeomFill_CoonsStyle;                                                           
GeomFill_BSplineCurves aGeomFill2(                                                    
        Handle(Geom_BSplineCurve)::DownCast(SPL1->Translated(gp_Vec(10,0,0))),        
        Handle(Geom_BSplineCurve)::DownCast(SPL2->Translated(gp_Vec(10,0,0))),Type);  
Handle(Geom_BSplineSurface)    aBSplineSurface2 = aGeomFill2.Surface();               
Type = GeomFill_CurvedStyle;                                                          
GeomFill_BSplineCurves aGeomFill3(                                                    
        Handle(Geom_BSplineCurve)::DownCast(SPL1->Translated(gp_Vec(20,0,0))),        
        Handle(Geom_BSplineCurve)::DownCast(SPL2->Translated(gp_Vec(20,0,0))),Type);  
Handle(Geom_BSplineSurface)    aBSplineSurface3 = aGeomFill3.Surface();               
                                                                                      
//==============================================================
    Message = " \
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
GeomFill_BSplineCurves aGeomFill2(                                                    \n";
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
    AddSeparator(Message);
    //--------------------------------------------------------------

    // Trace des frontieres.  -> FreeBoundaryAspect
    // Trace des isoparametriques.  --> UIsoAspect()
    
    DisplaySurface(aContext,aBSplineSurface1,Quantity_NOC_YELLOW);
    DisplaySurface(aContext,aBSplineSurface2,Quantity_NOC_SALMON);
    DisplaySurface(aContext,aBSplineSurface3,Quantity_NOC_HOTPINK);

    for (int i=0;i<=2;i++)
    {
        DisplayCurve(aContext,Handle(Geom_BSplineCurve)::DownCast(SPL1->Translated(gp_Vec(i*10,0,0))), Quantity_NOC_RED,Standard_False);
        DisplayCurve(aContext,Handle(Geom_BSplineCurve)::DownCast(SPL2->Translated(gp_Vec(i*10,0,0))), Quantity_NOC_GREEN,Standard_False);
    }

    Message += "SPL1                      is Red; \n";
    Message += "SPL2                      is Green; \n";
    Message += "aBSplineSurface1  is Yellow;       ( GeomFill_StretchStyle )\n";   
    Message += "aBSplineSurface2  is Salmon;     ( GeomFill_CoonsStyle ) \n";
    Message += "aBSplineSurface3  is Hot pink;   ( GeomFill_CurvedStyle ) \n";
}

//===============================================================
// Function name: gpTest39
//===============================================================
 void SampleGeometryPackage::gpTest39(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
    Message = " \
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
Standard_Integer MaxDeg = 8;                                                               \n";
Message += "\
Standard_Integer MaxSeg = 2;                                                               \n\
GeomFill_ConstrainedFilling aConstrainedFilling(MaxDeg, MaxSeg);                           \n\
                                                                                           \n\
aConstrainedFilling.Init(B1,B2,B3,NoCheck);                                                \n\
                                                                                           \n\
Handle(Geom_BSplineSurface) aBSplineSurface = aConstrainedFilling.Surface();               \n\
                                                                                           \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    TCollection_AsciiString aString;

    DisplaySurface(aContext,aBSplineSurface,Quantity_NOC_YELLOW);

    DisplayCurve(aContext,SPL1,Quantity_NOC_RED ,Standard_False);
    DisplayCurve(aContext,SPL2,Quantity_NOC_GREEN ,Standard_False);
    DisplayCurve(aContext,SPL3,Quantity_NOC_BLUE1 ,Standard_False);

    Message += "SPL1                      is Red; \n";
    Message += "SPL2                      is Green; \n";
    Message += "SPL3                      is Blue; \n";

    Message += "aBSplineSurface  is Yellow; \n";   
}

//===============================================================
// Function name: gpTest40
//===============================================================
 void SampleGeometryPackage::gpTest40(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
//==============================================================
                                                                   
TColgp_Array1OfPnt array1 (1,5); // sizing array                   
array1.SetValue(1,gp_Pnt (-4,0,2 ));                               
array1.SetValue(2,gp_Pnt (-5,1,0 ));                               
array1.SetValue(3,gp_Pnt (-6,2,-2 ));                              
array1.SetValue(4,gp_Pnt (-5,4,-7));                               
array1.SetValue(5,gp_Pnt (-3,5,-12));                              
                                                                   
Handle(Geom_BSplineCurve) SPL1 =                                   
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
if (!aSurface2.IsNull())  {                                        
    aSurfaceEntityTypeName2 = aSurface2->DynamicType()->Name();    
    aSurface2->Translate(gp_Vec(5,0,0));  }                        
                                                                   
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
    Message = " \
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
aPipe2.Perform();                                                  \n";
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
    AddSeparator(Message);
    //--------------------------------------------------------------


    if (!aSurface.IsNull())
    {
        DisplaySurface(aContext,aSurface,Quantity_NOC_YELLOW);
    }
    if (!aSurface2.IsNull())
    {
        DisplaySurface(aContext,aSurface2,Quantity_NOC_YELLOW);
    }
    if (!aSurface3.IsNull())
    {
        DisplaySurface(aContext,aSurface3,Quantity_NOC_YELLOW);
    }

    DisplayCurve(aContext,SPL1,Quantity_NOC_RED ,Standard_False);

    Message += "SPL1                      is Red; \n";

    Message += " aSurfaceEntityTypeName     = ";Message+= aSurfaceEntityTypeName; Message += "\n";
    Message += " aSurfaceEntityTypeName2     = ";Message+= aSurfaceEntityTypeName2; Message += "\n";
    Message += " aSurfaceEntityTypeName3     = ";Message+= aSurfaceEntityTypeName3; Message += "\n";
}

//===============================================================
// Function name: gpTest41
//===============================================================
 void SampleGeometryPackage::gpTest41(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
    Message = " \
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
                                                                   \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

                                                                                        
    if (!aPipeSurface.IsNull())
    {
        DisplaySurface(aContext,aPipeSurface,Quantity_NOC_YELLOW);
    }

    if (!anotherBSplineSurface.IsNull())
    {
        DisplaySurface(aContext,anotherBSplineSurface,Quantity_NOC_HOTPINK);
    }

    DisplayCurve(aContext,SPL1,Quantity_NOC_RED ,Standard_False);
    DisplayCurve(aContext,FirstSect,Quantity_NOC_GREEN ,Standard_False);

    Message += "SPL1                              is Red; \n";
    Message += "SPL2                              is Green; \n";
    Message += "SPL3                              is Blue; \n";
    Message += "aBSplineSurface            is Yellow; \n";   
    Message += "anotherBSplineSurface  is Hot Pink; \n";   
}

//===============================================================
// Function name: gpTest42
//===============================================================
 void SampleGeometryPackage::gpTest42(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
if (BB.IsDone()){                                                 
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
    Message = " \
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
        BB.UKnots()->Array1(),                                    \n";
        Message += "\
        BB.VKnots()->Array1(),                                    \n\
        BB.UMultiplicities()->Array1(),                           \n\
        BB.VMultiplicities()->Array1(),                           \n\
        BB.UDegree(),                                             \n\
        BB.VDegree() );                                           \n\
    BSPLSURF->Translate(gp_Vec(0,0,2));                           \n\
    }                                                             \n\
                                                                  \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
        
    DisplaySurface(aContext,BZ1,Quantity_NOC_RED);
    DisplaySurface(aContext,BZ2,Quantity_NOC_GREEN);
    DisplaySurface(aContext,BZ3,Quantity_NOC_BLUE1);
    DisplaySurface(aContext,BZ4,Quantity_NOC_BROWN);

    if (BB.IsDone()){
        DisplaySurface(aContext,BSPLSURF,Quantity_NOC_HOTPINK);
    }

    Message += "BZ1              is Red; \n";
    Message += "BZ2              is Green; \n";
    Message += "BZ3              is Blue; \n";
    Message += "BZ4              is Brown; \n";
    Message += "BSPLSURF  is Hot Pink; \n";   
}

//===============================================================
// Function name: gpTest43
//===============================================================
 void SampleGeometryPackage::gpTest43(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
    Message = " \
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
                                                                                 \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplaySurface(aContext,aGeomSurface,Quantity_NOC_BLUE1);
    DisplaySurface(aContext,GOS,Quantity_NOC_GREEN);
    DisplaySurface(aContext,GOS1,Quantity_NOC_GREEN);
    DisplaySurface(aContext,GOS2,Quantity_NOC_GREEN);

    DisplayCurve(aContext,SPL1,Quantity_NOC_RED ,Standard_False);
    DisplayCurve(aContext,SPL2,Quantity_NOC_HOTPINK ,Standard_False);

    Message += "aGeomSurface  is Blue; \n";
    Message += "GOS              are Green; \n";
}

//===============================================================
// Function name: gpTest44
//===============================================================
 void SampleGeometryPackage::gpTest44(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
   new Geom_RectangularTrimmedSurface(SOLE,-10,10,Standard_False);  
                                                           
Standard_CString SOLEEntityTypeName="Not Computed";        
if (!SOLE.IsNull())                                        
  {                                                        
    SOLEEntityTypeName = SOLE->DynamicType()->Name();      
  }                                                        
                                                           
//==============================================================
    Message = " \
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
                                                           \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplaySurface(aContext,aTrimmedSurface,Quantity_NOC_GREEN);
    DisplayCurve(aContext,aCurve,Quantity_NOC_RED ,Standard_False);

    Message += "aCurve   is Red; \n";
    Message += "aTrimmedSurface       is Green; \n";

    Message += " SOLEEntityTypeName     = ";Message+= SOLEEntityTypeName; Message += "\n";
}

//===============================================================
// Function name: gpTest45
//===============================================================
 void SampleGeometryPackage::gpTest45(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
    Message = " \
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
                                                     \n";
    AddSeparator(Message);
    //--------------------------------------------------------------
    DisplaySurface(aContext,SOR,Quantity_NOC_GREEN);
    DisplayCurve(aContext,aCurve,Quantity_NOC_RED ,Standard_False);

    Message += "aCurve   is Red; \n";
    Message += "SOR       is Green; \n";

    Message += " SOREntityTypeName     = ";Message+= SOREntityTypeName; Message += "\n";
}

//===============================================================
// Function name: gpTest46
//===============================================================
 void SampleGeometryPackage::gpTest46(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
                                                                  
Handle(Geom_BSplineSurface) aTranslatedGeomSurface =              
   Handle(Geom_BSplineSurface)::DownCast(aGeomSurface->Copy());   
                                                                  
Standard_Real extension = 3;                                      
Standard_Integer continuity = 2;                                  
Standard_Boolean Udirection = Standard_True;                      
Standard_Boolean after = Standard_True;                           
GeomLib::ExtendSurfByLength (aTranslatedGeomSurface,              
	extension,continuity,Udirection,after);                       
                                                                 
//==============================================================
    Message = " \
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
Handle(Geom_BSplineSurface) aTranslatedGeomSurface =              \n\
   Handle(Geom_BSplineSurface)::DownCast(aGeomSurface->Copy());   \n\
                                                                  \n\
Standard_Real extension = 3;                                      \n\
Standard_Integer continuity = 2;                                  \n\
Standard_Boolean Udirection = Standard_True;                      \n\
Standard_Boolean after = Standard_True;                           \n\
GeomLib::ExtendSurfByLength (aTranslatedGeomSurface,              \n\
	extension,continuity,Udirection,after);                       \n\
                                                                  \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    if (!aGeomSurface.IsNull())
    {
        DisplaySurface(aContext,aGeomSurface,Quantity_NOC_HOTPINK);
    }

    if (!aTranslatedGeomSurface.IsNull())
    {
        DisplaySurface(aContext,aTranslatedGeomSurface,Quantity_NOC_BLUE1);
    }

    DisplayCurve(aContext,SPL1,Quantity_NOC_RED ,Standard_False);
    DisplayCurve(aContext,SPL2,Quantity_NOC_GREEN ,Standard_False);

    Message += "aGeomSurface                    is Hot Pink; \n";
    Message += "aTranslatedGeomSurface   is Blue; \n";
    Message += "SPL1                                   is Red; \n";
    Message += "SPL2                                   is Green; \n";
}

//===============================================================
// Function name: gpTest47
//===============================================================
 void SampleGeometryPackage::gpTest47(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
//Quantity_Length dist = 
  ESS.LowerDistance();
gp_Pnt P1,P2;	
ESS.NearestPoints(P1,P2);

gp_Pnt P3,P4;
Handle(Geom_Curve) aCurve;
Standard_Integer NbExtrema = ESS.NbExtrema();
for(Standard_Integer k=1;k<=NbExtrema;k++){
    ESS.Points(k,P3,P4);                                      
    aCurve= GC_MakeSegment(P3,P4).Value();
    DisplayCurve(aContext,aCurve,Quantity_NOC_YELLOW3,Standard_False);
}

                                                                 
//==============================================================
    Message = "                         \n\
GeomFill_FillingStyle Type = GeomFill_StretchStyle;            \n\
                                                               \n\
GeomFill_BSplineCurves aGeomFill1(SPL1,SPL2,Type);             \n\
Handle(Geom_BSplineSurface) aSurf1 = aGeomFill1.Surface();     \n\
                                                               \n\
Handle(Geom_BSplineSurface) aSurf2 =                           \n\
	GeomAPI_PointsToBSplineSurface(array3).Surface();          \n\
                                                               \n\
GeomAPI_ExtremaSurfaceSurface ESS(aSurf1,aSurf2);              \n\
Quantity_Length dist = ESS.LowerDistance();                    \n\
gp_Pnt P1,P2;                                                  \n\
ESS.NearestPoints(P1,P2);                                      \n\
                                                               \n"; 

    AddSeparator(Message);
    Message += "aSurf1   is Green; \n";
    Message += "aSurf2   is Hot Pink; \n";
    Message += "Nearest points P1 and P2 are shown; \n";

    //--------------------------------------------------------------

    DisplaySurface(aContext,aSurf1,Quantity_NOC_GREEN);
    DisplaySurface(aContext,aSurf2,Quantity_NOC_HOTPINK);
    DisplayCurve(aContext,SPL1,Quantity_NOC_RED ,Standard_False);
    DisplayCurve(aContext,SPL2,Quantity_NOC_AZURE ,Standard_False);
 
    DisplayPoint(aContext,P1,"P1");
    DisplayPoint(aContext,P2,"P2");
}

//===============================================================
// Function name: gpTest48
//===============================================================
 void SampleGeometryPackage::gpTest48(const Handle(ISession2D_InteractiveContext)& aContext2D,
                                     TCollection_AsciiString& Message) 
{
  aContext2D->EraseAll();
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
aCBox.Get(  aCXmin, aCYmin, aCXmax,aCYmax);                         
                                                                    
Bnd_Box2d aSPL1Box;                                                 
Geom2dAdaptor_Curve GAC1 (SPL1);                                    
BndLib_Add2dCurve::Add (GAC1,Precision::Approximation(),aSPL1Box);  
                                                                    
Standard_Real aSPL1Xmin,aSPL1Ymin,aSPL1Xmax,aSPL1Ymax;              
aSPL1Box.Get(  aSPL1Xmin, aSPL1Ymin, aSPL1Xmax,aSPL1Ymax);          
                                                                    
Bnd_Box2d aSPL2Box;                                                 
Geom2dAdaptor_Curve GAC2 (SPL2);                                    
BndLib_Add2dCurve::Add (GAC2,Precision::Approximation(),aSPL2Box);  
                                                                    
Standard_Real aSPL2Xmin,aSPL2Ymin,aSPL2Xmax,aSPL2Ymax;              
aSPL2Box.Get(  aSPL2Xmin, aSPL2Ymin, aSPL2Xmax,aSPL2Ymax);          
                                                                    
//==============================================================
    Message = " \
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
aSPL1Box.Get(  aSPL1Xmin, aSPL1Ymin, aSPL1Xmax,aSPL1Ymax);             \n";
Message += "\
                                                                       \n\
Bnd_Box2d aSPL2Box;                                                    \n\
Geom2dAdaptor_Curve GAC2 (SPL2);                                       \n\
BndLib_Add2dCurve::Add (GAC2,Precision::Approximation(),aSPL2Box);     \n\
                                                                       \n\
Standard_Real aSPL2Xmin,aSPL2Ymin,aSPL2Xmax,aSPL2Ymax;                 \n\
aSPL2Box.Get(  aSPL2Xmin, aSPL2Ymin, aSPL2Xmax,aSPL2Ymax);             \n\
                                                                       \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayCurve(aContext2D,C ,5);
    DisplayCurve(aContext2D,SPL1,6 );
    DisplayCurve(aContext2D,SPL2,7 );

    DisplayPoint(aContext2D,gp_Pnt2d(aCXmin,aCYmax),"aCXmin,aCYmax");
    DisplayPoint(aContext2D,gp_Pnt2d(aCXmax,aCYmax),"aCXmax,aCYmax");
    DisplayPoint(aContext2D,gp_Pnt2d(aCXmin,aCYmin),"aCXmin,aCYmin");
    DisplayPoint(aContext2D,gp_Pnt2d(aCXmax,aCYmin),"aCXmax,aCYmin");

    DisplayCurve(aContext2D,GCE2d_MakeSegment(gp_Pnt2d(aCXmin,aCYmax),gp_Pnt2d(aCXmax,aCYmax)).Value() ,4); // X,Ymax
    DisplayCurve(aContext2D,GCE2d_MakeSegment(gp_Pnt2d(aCXmin,aCYmin),gp_Pnt2d(aCXmax,aCYmin)).Value() ,4); // X,Ymin
    DisplayCurve(aContext2D,GCE2d_MakeSegment(gp_Pnt2d(aCXmin,aCYmin),gp_Pnt2d(aCXmin,aCYmax)).Value() ,4); // Xmin,Y
    DisplayCurve(aContext2D,GCE2d_MakeSegment(gp_Pnt2d(aCXmax,aCYmin),gp_Pnt2d(aCXmax,aCYmax)).Value() ,4); // Xmax,Y

    DisplayPoint(aContext2D,gp_Pnt2d(aSPL1Xmin,aSPL1Ymax),"aSPL1Xmin,aSPL1Ymax");
    DisplayPoint(aContext2D,gp_Pnt2d(aSPL1Xmax,aSPL1Ymax),"aSPL1Xmax,aSPL1Ymax");
    DisplayPoint(aContext2D,gp_Pnt2d(aSPL1Xmin,aSPL1Ymin),"aSPL1Xmin,aSPL1Ymin");
    DisplayPoint(aContext2D,gp_Pnt2d(aSPL1Xmax,aSPL1Ymin),"aSPL1Xmax,aSPL1Ymin");

    DisplayCurve(aContext2D,GCE2d_MakeSegment(gp_Pnt2d(aSPL1Xmin,aSPL1Ymax),gp_Pnt2d(aSPL1Xmax,aSPL1Ymax)).Value() ,4); // X,Ymax
    DisplayCurve(aContext2D,GCE2d_MakeSegment(gp_Pnt2d(aSPL1Xmin,aSPL1Ymin),gp_Pnt2d(aSPL1Xmax,aSPL1Ymin)).Value() ,4); // X,Ymin
    DisplayCurve(aContext2D,GCE2d_MakeSegment(gp_Pnt2d(aSPL1Xmin,aSPL1Ymin),gp_Pnt2d(aSPL1Xmin,aSPL1Ymax)).Value() ,4); // Xmin,Y
    DisplayCurve(aContext2D,GCE2d_MakeSegment(gp_Pnt2d(aSPL1Xmax,aSPL1Ymin),gp_Pnt2d(aSPL1Xmax,aSPL1Ymax)).Value() ,4); // Xmax,Y

    DisplayPoint(aContext2D,gp_Pnt2d(aSPL2Xmin,aSPL2Ymax),"aSPL2Xmin,aSPL2Ymax");
    DisplayPoint(aContext2D,gp_Pnt2d(aSPL2Xmax,aSPL2Ymax),"aSPL2Xmax,aSPL2Ymax");
    DisplayPoint(aContext2D,gp_Pnt2d(aSPL2Xmin,aSPL2Ymin),"aSPL2Xmin,aSPL2Ymin");
    DisplayPoint(aContext2D,gp_Pnt2d(aSPL2Xmax,aSPL2Ymin),"aSPL2Xmax,aSPL2Ymin");

    DisplayCurve(aContext2D,GCE2d_MakeSegment(gp_Pnt2d(aSPL2Xmin,aSPL2Ymax),gp_Pnt2d(aSPL2Xmax,aSPL2Ymax)).Value() ,4); // X,Ymax
    DisplayCurve(aContext2D,GCE2d_MakeSegment(gp_Pnt2d(aSPL2Xmin,aSPL2Ymin),gp_Pnt2d(aSPL2Xmax,aSPL2Ymin)).Value() ,4); // X,Ymin
    DisplayCurve(aContext2D,GCE2d_MakeSegment(gp_Pnt2d(aSPL2Xmin,aSPL2Ymin),gp_Pnt2d(aSPL2Xmin,aSPL2Ymax)).Value() ,4); // Xmin,Y
    DisplayCurve(aContext2D,GCE2d_MakeSegment(gp_Pnt2d(aSPL2Xmax,aSPL2Ymin),gp_Pnt2d(aSPL2Xmax,aSPL2Ymax)).Value() ,4); // Xmax,Y
}

//===============================================================
// Function name: gpTest49
//===============================================================
 void SampleGeometryPackage::gpTest49(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
//==============================================================
                                                               
Bnd_Box aBox;                                                  
Standard_Real radius = 100;                                      
gp_Ax2 anAxis(gp_Pnt(0,0,0),gp_Dir(1,2,-5));                   
                                                               
Handle(Geom_Circle) C =                                        
    new Geom_Circle(anAxis,radius);                            
GeomAdaptor_Curve GAC (C);                                     
BndLib_Add3dCurve::Add (GAC,Precision::Approximation(),aBox);  
                                                               
Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax ;       
aBox.Get(  aXmin, aYmin,aZmin, aXmax,aYmax,aZmax);             
                                                               
//==============================================================
    Message = " \
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
                                                               \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    DisplayCurve(aContext,C,Quantity_NOC_BLUE1 ,Standard_False);

    DisplayPoint(aContext,gp_Pnt(aXmin,aYmax,aZmin),"aXmin,aYmax,aZmin");
    DisplayPoint(aContext,gp_Pnt(aXmax,aYmax,aZmin),"aXmax,aYmax,aZmin");
    DisplayPoint(aContext,gp_Pnt(aXmin,aYmin,aZmin),"aXmin,aYmin,aZmin");
    DisplayPoint(aContext,gp_Pnt(aXmax,aYmin,aZmin),"aXmax,aYmin,aZmin");

    DisplayPoint(aContext,gp_Pnt(aXmin,aYmax,aZmax),"aXmin,aYmax,aZmax");
    DisplayPoint(aContext,gp_Pnt(aXmax,aYmax,aZmax),"aXmax,aYmax,aZmax");
    DisplayPoint(aContext,gp_Pnt(aXmin,aYmin,aZmax),"aXmin,aYmin,aZmax");
    DisplayPoint(aContext,gp_Pnt(aXmax,aYmin,aZmax),"aXmax,aYmin,aZmax");

    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmax,aZmin),
                                     gp_Pnt(aXmax,aYmax,aZmin)).Value() ,Quantity_NOC_RED); // X,Ymax,ZMin
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmin),
                                     gp_Pnt(aXmax,aYmin,aZmin)).Value() ,Quantity_NOC_RED); // X,Ymin,ZMin
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmin),
                                     gp_Pnt(aXmin,aYmax,aZmin)).Value() ,Quantity_NOC_RED); // Xmin,Y,ZMin
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmax,aYmin,aZmin),
                                     gp_Pnt(aXmax,aYmax,aZmin)).Value() ,Quantity_NOC_RED); // Xmax,Y,ZMin
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmax,aZmax),
                                     gp_Pnt(aXmax,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // X,Ymax,ZMax
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmax),
                                     gp_Pnt(aXmax,aYmin,aZmax)).Value() ,Quantity_NOC_RED); // X,Ymin,ZMax
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmax),
                                     gp_Pnt(aXmin,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmin,Y,ZMax
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmax,aYmin,aZmax),
                                     gp_Pnt(aXmax,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmax,Y,ZMax
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmin),
                                     gp_Pnt(aXmin,aYmin,aZmax)).Value() ,Quantity_NOC_RED); // Xmin,Ymin,Z
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmax,aYmin,aZmin),
                                     gp_Pnt(aXmax,aYmin,aZmax)).Value() ,Quantity_NOC_RED); // Xmax,Ymin,Z
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmax,aZmin),
                                     gp_Pnt(aXmin,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmin,Ymax,Z
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmax,aYmax,aZmin),
                                     gp_Pnt(aXmax,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmax,Ymax,Z
/*
	Handle(AIS_Trihedron) aTrihedron;
	Handle(Geom_Axis2Placement) aTrihedronAxis=new Geom_Axis2Placement(gp::XOY());
	aTrihedron=new AIS_Trihedron(aTrihedronAxis);
	aContext->Display(aTrihedron);
*/
}

//===============================================================
// Function name: gpTest50
//===============================================================
 void SampleGeometryPackage::gpTest50(const Handle(AIS_InteractiveContext)& aContext,
                                     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
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
aBox.Get(  aXmin, aYmin,aZmin, aXmax,aYmax,aZmax);             
                                                               
//==============================================================
    Message = " \
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
                                                               \n";
    AddSeparator(Message);
    //--------------------------------------------------------------

    Quantity_NameOfColor aNameOfColor= Quantity_NOC_GREEN;
    Handle(ISession_Surface) aGraphicalSurface = new ISession_Surface(aSurf);
    aContext->SetColor(aGraphicalSurface,aNameOfColor);
    aGraphicalSurface->Attributes()->FreeBoundaryAspect()->SetColor(aNameOfColor);
    aGraphicalSurface->Attributes()->UIsoAspect()->SetColor(aNameOfColor);
    aGraphicalSurface->Attributes()->VIsoAspect()->SetColor(aNameOfColor);

    aContext->SetDisplayMode(aGraphicalSurface,1);
    aContext->Display(aGraphicalSurface,Standard_False);
    //   DisplaySurface(aContext,aSurf,Quantity_NOC_GREEN);

    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmax,aZmin),
                                     gp_Pnt(aXmax,aYmax,aZmin)).Value() ,Quantity_NOC_RED); // X,Ymax,ZMin
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmin),
                                     gp_Pnt(aXmax,aYmin,aZmin)).Value() ,Quantity_NOC_RED); // X,Ymin,ZMin
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmin),
                                     gp_Pnt(aXmin,aYmax,aZmin)).Value() ,Quantity_NOC_RED); // Xmin,Y,ZMin
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmax,aYmin,aZmin),
                                     gp_Pnt(aXmax,aYmax,aZmin)).Value() ,Quantity_NOC_RED); // Xmax,Y,ZMin
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmax,aZmax),
                                     gp_Pnt(aXmax,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // X,Ymax,ZMax
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmax),
                                     gp_Pnt(aXmax,aYmin,aZmax)).Value() ,Quantity_NOC_RED); // X,Ymin,ZMax
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmax),
                                     gp_Pnt(aXmin,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmin,Y,ZMax
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmax,aYmin,aZmax),
                                     gp_Pnt(aXmax,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmax,Y,ZMax
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmin,aZmin),
                                     gp_Pnt(aXmin,aYmin,aZmax)).Value() ,Quantity_NOC_RED); // Xmin,Ymin,Z
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmax,aYmin,aZmin),
                                     gp_Pnt(aXmax,aYmin,aZmax)).Value() ,Quantity_NOC_RED); // Xmax,Ymin,Z
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmin,aYmax,aZmin),
                                     gp_Pnt(aXmin,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmin,Ymax,Z
    DisplayCurve(aContext,GC_MakeSegment(gp_Pnt(aXmax,aYmax,aZmin),
                                     gp_Pnt(aXmax,aYmax,aZmax)).Value() ,Quantity_NOC_RED); // Xmax,Ymax,Z
/*
	Handle(AIS_Trihedron) aTrihedron;
	Handle(Geom_Axis2Placement) aTrihedronAxis=new Geom_Axis2Placement(gp::XOY());
	aTrihedron=new AIS_Trihedron(aTrihedronAxis);
	aContext->Display(aTrihedron);
*/
}


//===============================================================
// Function name: SaveImage
//===============================================================
#ifndef WNT
 Standard_Boolean SampleGeometryPackage::SaveImage(const Standard_CString ,
						   const Standard_CString ,
						   const Handle(V3d_View)& ) 
{
#else
 Standard_Boolean SampleGeometryPackage::SaveImage(const Standard_CString aFileName,
						   const Standard_CString aFormat,
						   const Handle(V3d_View)& aView) 
{
  Handle(Aspect_Window) anAspectWindow = aView->Window();
  Handle(WNT_Window) aWNTWindow = Handle(WNT_Window)::DownCast(anAspectWindow);

  if (aFormat == "bmp") aWNTWindow->SetOutputFormat(WNT_TOI_BMP);
  if (aFormat == "gif") aWNTWindow->SetOutputFormat(WNT_TOI_GIF);
  if (aFormat == "xwd") aWNTWindow->SetOutputFormat(WNT_TOI_XWD);

  aWNTWindow->Dump(aFileName);
#endif
  return Standard_True;
}
