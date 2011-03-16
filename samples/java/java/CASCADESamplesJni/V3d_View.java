// Java Native Class from Cas.Cade
//                     Copyright (C) 1991,1999 by
//  
//                      MATRA DATAVISION, FRANCE
//  
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of the above copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any other person. No title to an ownership of the
// software is hereby transferred.
//  
// At the termination of the contract, the software and all copies of this
// software must be deleted.
//


package CASCADESamplesJni;

import jcas.Object;
import CASCADESamplesJni.Aspect_Window;
import jcas.Standard_Integer;
import jcas.Standard_Boolean;
import CASCADESamplesJni.Quantity_TypeOfColor;
import jcas.Standard_Real;
import jcas.Standard_Short;
import CASCADESamplesJni.Quantity_Color;
import CASCADESamplesJni.Quantity_NameOfColor;
import CASCADESamplesJni.V3d_TypeOfShadingModel;
import CASCADESamplesJni.V3d_TypeOfSurfaceDetail;
import CASCADESamplesJni.Graphic3d_TextureEnv;
import CASCADESamplesJni.V3d_TypeOfVisualization;
import CASCADESamplesJni.V3d_TypeOfZclipping;
import CASCADESamplesJni.V3d_Light;
import CASCADESamplesJni.V3d_Plane;
import CASCADESamplesJni.Aspect_TypeOfTriedronPosition;
import CASCADESamplesJni.Aspect_TypeOfTriedronEcho;
import CASCADESamplesJni.V3d_TypeOfAxe;
import CASCADESamplesJni.V3d_TypeOfOrientation;
import CASCADESamplesJni.V3d_Viewer;
import CASCADESamplesJni.V3d_TypeOfView;
import CASCADESamplesJni.Visual3d_View;
import CASCADESamplesJni.Graphic3d_Plotter;
import CASCADESamplesJni.gp_Ax3;
import CASCADESamplesJni.Aspect_Grid;
import CASCADESamplesJni.PlotMgt_PlotterDriver;
import jcas.Standard_CString;
import CASCADESamplesJni.Aspect_FormatOfSheetPaper;
import CASCADESamplesJni.Aspect_PixMap;
import CASCADESamplesJni.V3d_TypeOfProjectionModel;
import CASCADESamplesJni.V3d_TypeOfBackfacingModel;


public class V3d_View extends CASCADESamplesJni.Viewer_View {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
native public final void SetWindow(Aspect_Window IdWin);
native public final void SetMagnify(Aspect_Window IdWin,V3d_View aPreviousView,int x1,int y1,int x2,int y2);
native public final void Remove();
native public final void Update();
final public void Redraw() {
    V3d_View_Redraw_1();
}

private final native void V3d_View_Redraw_1();

final public void Redraw(int x,int y,int width,int height) {
    V3d_View_Redraw_2(x,y,width,height);
}

private final native void V3d_View_Redraw_2(int x,int y,int width,int height);

native public final void MustBeResized();
native public final void DoMapping();
native public final boolean IsEmpty();
native public final void UpdateLights();
final public void SetBackgroundColor(short Type,double V1,double V2,double V3) {
    V3d_View_SetBackgroundColor_1(Type,V1,V2,V3);
}

private final native void V3d_View_SetBackgroundColor_1(short Type,double V1,double V2,double V3);

final public void SetBackgroundColor(Quantity_Color Color) {
    V3d_View_SetBackgroundColor_2(Color);
}

private final native void V3d_View_SetBackgroundColor_2(Quantity_Color Color);

final public void SetBackgroundColor(short Name) {
    V3d_View_SetBackgroundColor_3(Name);
}

private final native void V3d_View_SetBackgroundColor_3(short Name);

native public final void SetAxis(double X,double Y,double Z,double Vx,double Vy,double Vz);
native public final void SetShadingModel(short Model);
native public final void SetSurfaceDetail(short SurfaceDetail);
native public final void SetTextureEnv(Graphic3d_TextureEnv ATexture);
native public final void SetVisualization(short Mode);
native public final void SetAntialiasingOn();
native public final void SetAntialiasingOff();
native public final void SetZClippingDepth(double Depth);
native public final void SetZClippingWidth(double Width);
native public final void SetZClippingType(short Type);
native public final void SetZCueingDepth(double Depth);
native public final void SetZCueingWidth(double Width);
native public final void SetZCueingOn();
native public final void SetZCueingOff();
final public void SetLightOn(V3d_Light MyLight) {
    V3d_View_SetLightOn_1(MyLight);
}

private final native void V3d_View_SetLightOn_1(V3d_Light MyLight);

final public void SetLightOn() {
    V3d_View_SetLightOn_2();
}

private final native void V3d_View_SetLightOn_2();

final public void SetLightOff(V3d_Light MyLight) {
    V3d_View_SetLightOff_1(MyLight);
}

private final native void V3d_View_SetLightOff_1(V3d_Light MyLight);

final public void SetLightOff() {
    V3d_View_SetLightOff_2();
}

private final native void V3d_View_SetLightOff_2();

native public final boolean IsActiveLight(V3d_Light aLight);
native public final void SetTransparency(boolean AnActivity);
final public void SetPlaneOn(V3d_Plane MyPlane) {
    V3d_View_SetPlaneOn_1(MyPlane);
}

private final native void V3d_View_SetPlaneOn_1(V3d_Plane MyPlane);

final public void SetPlaneOn() {
    V3d_View_SetPlaneOn_2();
}

private final native void V3d_View_SetPlaneOn_2();

final public void SetPlaneOff(V3d_Plane MyPlane) {
    V3d_View_SetPlaneOff_1(MyPlane);
}

private final native void V3d_View_SetPlaneOff_1(V3d_Plane MyPlane);

final public void SetPlaneOff() {
    V3d_View_SetPlaneOff_2();
}

private final native void V3d_View_SetPlaneOff_2();

native public final boolean IsActivePlane(V3d_Plane aPlane);
native public final void TriedronDisplay(short APosition,short AColor,double AScale);
native public final void TriedronErase();
native public final void TriedronEcho(short AType);
native public final void SetFront();
final public void Rotate(double Ax,double Ay,double Az,boolean Start) {
    V3d_View_Rotate_1(Ax,Ay,Az,Start);
}

private final native void V3d_View_Rotate_1(double Ax,double Ay,double Az,boolean Start);

final public void Rotate(double Ax,double Ay,double Az,double X,double Y,double Z,boolean Start) {
    V3d_View_Rotate_2(Ax,Ay,Az,X,Y,Z,Start);
}

private final native void V3d_View_Rotate_2(double Ax,double Ay,double Az,double X,double Y,double Z,boolean Start);

final public void Rotate(short Axe,double Angle,double X,double Y,double Z,boolean Start) {
    V3d_View_Rotate_3(Axe,Angle,X,Y,Z,Start);
}

private final native void V3d_View_Rotate_3(short Axe,double Angle,double X,double Y,double Z,boolean Start);

final public void Rotate(short Axe,double Angle,boolean Start) {
    V3d_View_Rotate_4(Axe,Angle,Start);
}

private final native void V3d_View_Rotate_4(short Axe,double Angle,boolean Start);

final public void Rotate(double Angle,boolean Start) {
    V3d_View_Rotate_5(Angle,Start);
}

private final native void V3d_View_Rotate_5(double Angle,boolean Start);

final public void Move(double Dx,double Dy,double Dz,boolean Start) {
    V3d_View_Move_1(Dx,Dy,Dz,Start);
}

private final native void V3d_View_Move_1(double Dx,double Dy,double Dz,boolean Start);

final public void Move(short Axe,double Length,boolean Start) {
    V3d_View_Move_2(Axe,Length,Start);
}

private final native void V3d_View_Move_2(short Axe,double Length,boolean Start);

final public void Move(double Length,boolean Start) {
    V3d_View_Move_3(Length,Start);
}

private final native void V3d_View_Move_3(double Length,boolean Start);

final public void Translate(double Dx,double Dy,double Dz,boolean Start) {
    V3d_View_Translate_1(Dx,Dy,Dz,Start);
}

private final native void V3d_View_Translate_1(double Dx,double Dy,double Dz,boolean Start);

final public void Translate(short Axe,double Length,boolean Start) {
    V3d_View_Translate_2(Axe,Length,Start);
}

private final native void V3d_View_Translate_2(short Axe,double Length,boolean Start);

final public void Translate(double Length,boolean Start) {
    V3d_View_Translate_3(Length,Start);
}

private final native void V3d_View_Translate_3(double Length,boolean Start);

native public final void Place(int x,int y,double aZoomFactor);
final public void Turn(double Ax,double Ay,double Az,boolean Start) {
    V3d_View_Turn_1(Ax,Ay,Az,Start);
}

private final native void V3d_View_Turn_1(double Ax,double Ay,double Az,boolean Start);

final public void Turn(short Axe,double Angle,boolean Start) {
    V3d_View_Turn_2(Axe,Angle,Start);
}

private final native void V3d_View_Turn_2(short Axe,double Angle,boolean Start);

final public void Turn(double Angle,boolean Start) {
    V3d_View_Turn_3(Angle,Start);
}

private final native void V3d_View_Turn_3(double Angle,boolean Start);

native public final void SetTwist(double Angle);
native public final void SetEye(double X,double Y,double Z);
native public final void SetDepth(double Depth);
final public void SetProj(double Vx,double Vy,double Vz) {
    V3d_View_SetProj_1(Vx,Vy,Vz);
}

private final native void V3d_View_SetProj_1(double Vx,double Vy,double Vz);

final public void SetProj(short Orientation) {
    V3d_View_SetProj_2(Orientation);
}

private final native void V3d_View_SetProj_2(short Orientation);

native public final void SetAt(double X,double Y,double Z);
final public void SetUp(double Vx,double Vy,double Vz) {
    V3d_View_SetUp_1(Vx,Vy,Vz);
}

private final native void V3d_View_SetUp_1(double Vx,double Vy,double Vz);

final public void SetUp(short Orientation) {
    V3d_View_SetUp_2(Orientation);
}

private final native void V3d_View_SetUp_2(short Orientation);

native public final void SetViewOrientationDefault();
native public final void ResetViewOrientation();
native public final void Panning(double Dx,double Dy,double aZoomFactor,boolean Start);
final public void SetCenter(double Xc,double Yc) {
    V3d_View_SetCenter_1(Xc,Yc);
}

private final native void V3d_View_SetCenter_1(double Xc,double Yc);

final public void SetCenter(int X,int Y) {
    V3d_View_SetCenter_2(X,Y);
}

private final native void V3d_View_SetCenter_2(int X,int Y);

native public final void SetSize(double Size);
native public final void SetZSize(double Size);
native public final void SetZoom(double Coef,boolean Start);
native public final void SetScale(double Coef);
final public void FitAll(double Coef,boolean FitZ) {
    V3d_View_FitAll_1(Coef,FitZ,true);
}

private final native void V3d_View_FitAll_1(double Coef,boolean FitZ,boolean Update);

native public final void ZFitAll(double Coef);
final public void FitAll(double Umin,double Vmin,double Umax,double Vmax) {
    V3d_View_FitAll_2(Umin,Vmin,Umax,Vmax);
}

private final native void V3d_View_FitAll_2(double Umin,double Vmin,double Umax,double Vmax);

native public final void WindowFit(int Xmin,int Ymin,int Xmax,int Ymax);
native public final void SetViewMappingDefault();
native public final void ResetViewMapping();
native public final void Reset();
final public double Convert(int Vp) {
   return V3d_View_Convert_1(Vp);
}

private final native double V3d_View_Convert_1(int Vp);

final public void Convert(int Xp,int Yp,Standard_Real Xv,Standard_Real Yv) {
    V3d_View_Convert_2(Xp,Yp,Xv,Yv);
}

private final native void V3d_View_Convert_2(int Xp,int Yp,Standard_Real Xv,Standard_Real Yv);

final public int Convert(double Vv) {
   return V3d_View_Convert_3(Vv);
}

private final native int V3d_View_Convert_3(double Vv);

final public void Convert(double Xv,double Yv,Standard_Integer Xp,Standard_Integer Yp) {
    V3d_View_Convert_4(Xv,Yv,Xp,Yp);
}

private final native void V3d_View_Convert_4(double Xv,double Yv,Standard_Integer Xp,Standard_Integer Yp);

final public void Convert(int Xp,int Yp,Standard_Real X,Standard_Real Y,Standard_Real Z) {
    V3d_View_Convert_5(Xp,Yp,X,Y,Z);
}

private final native void V3d_View_Convert_5(int Xp,int Yp,Standard_Real X,Standard_Real Y,Standard_Real Z);

final public void ConvertToGrid(int Xp,int Yp,Standard_Real Xg,Standard_Real Yg,Standard_Real Zg) {
    V3d_View_ConvertToGrid_1(Xp,Yp,Xg,Yg,Zg);
}

private final native void V3d_View_ConvertToGrid_1(int Xp,int Yp,Standard_Real Xg,Standard_Real Yg,Standard_Real Zg);

final public void ConvertToGrid(double X,double Y,double Z,Standard_Real Xg,Standard_Real Yg,Standard_Real Zg) {
    V3d_View_ConvertToGrid_2(X,Y,Z,Xg,Yg,Zg);
}

private final native void V3d_View_ConvertToGrid_2(double X,double Y,double Z,Standard_Real Xg,Standard_Real Yg,Standard_Real Zg);

final public void Convert(double X,double Y,double Z,Standard_Integer Xp,Standard_Integer Yp) {
    V3d_View_Convert_6(X,Y,Z,Xp,Yp);
}

private final native void V3d_View_Convert_6(double X,double Y,double Z,Standard_Integer Xp,Standard_Integer Yp);

native public final void Project(double X,double Y,double Z,Standard_Real Xp,Standard_Real Yp);
final public void BackgroundColor(short Type,Standard_Real V1,Standard_Real V2,Standard_Real V3) {
    V3d_View_BackgroundColor_1(Type,V1,V2,V3);
}

private final native void V3d_View_BackgroundColor_1(short Type,Standard_Real V1,Standard_Real V2,Standard_Real V3);

final public Quantity_Color BackgroundColor() {
   return V3d_View_BackgroundColor_2();
}

private final native Quantity_Color V3d_View_BackgroundColor_2();

native public final double Scale();
native public final void Center(Standard_Real Xc,Standard_Real Yc);
native public final void Size(Standard_Real Width,Standard_Real Height);
native public final double ZSize();
native public final void Eye(Standard_Real X,Standard_Real Y,Standard_Real Z);
native public final void FocalReferencePoint(Standard_Real X,Standard_Real Y,Standard_Real Z);
native public final void ProjReferenceAxe(int Xpix,int Ypix,Standard_Real XP,Standard_Real YP,Standard_Real ZP,Standard_Real VX,Standard_Real VY,Standard_Real VZ);
native public final double Depth();
native public final void Proj(Standard_Real Vx,Standard_Real Vy,Standard_Real Vz);
native public final void At(Standard_Real X,Standard_Real Y,Standard_Real Z);
native public final void Up(Standard_Real Vx,Standard_Real Vy,Standard_Real Vz);
native public final double Twist();
native public final short ShadingModel();
native public final short SurfaceDetail();
native public final Graphic3d_TextureEnv TextureEnv();
native public final boolean Transparency();
native public final short Visualization();
native public final boolean Antialiasing();
native public final boolean ZCueing(Standard_Real Depth,Standard_Real Width);
native public final short ZClipping(Standard_Real Depth,Standard_Real Width);
native public final boolean IfMoreLights();
native public final void InitActiveLights();
native public final boolean MoreActiveLights();
native public final void NextActiveLights();
native public final V3d_Light ActiveLight();
native public final boolean IfMorePlanes();
native public final void InitActivePlanes();
native public final boolean MoreActivePlanes();
native public final void NextActivePlanes();
native public final V3d_Plane ActivePlane();
native public final V3d_Viewer Viewer();
native public final boolean IfWindow();
native public final Aspect_Window Window();
native public final short Type();
native public final void Pan(int Dx,int Dy,double aZoomFactor);
final public void Zoom(int X1,int Y1,int X2,int Y2) {
    V3d_View_Zoom_1(X1,Y1,X2,Y2);
}

private final native void V3d_View_Zoom_1(int X1,int Y1,int X2,int Y2);

final public void Zoom(int X,int Y) {
    V3d_View_Zoom_2(X,Y);
}

private final native void V3d_View_Zoom_2(int X,int Y);

native public final void StartRotation(int X,int Y,double zRotationThreshold);
native public final void Rotation(int X,int Y);
native public final void SetFocale(double Focale);
native public final double Focale();
native public final Visual3d_View View();
native public final boolean TransientManagerBeginDraw(boolean DoubleBuffer,boolean RetainMode);
native public final void TransientManagerClearDraw();
native public final boolean TransientManagerBeginAddDraw();
native public final void SetAnimationModeOn();
native public final void SetAnimationModeOff();
native public final boolean AnimationModeIsOn();
native public final void SetAnimationMode(boolean anAnimationFlag,boolean aDegenerationFlag);
native public final boolean AnimationMode(Standard_Boolean isDegenerate);
native public final void SetDegenerateModeOn();
native public final void SetDegenerateModeOff();
native public final boolean DegenerateModeIsOn();
native public final void SetComputedMode(boolean aMode);
native public final boolean ComputedMode();
native public final void WindowFitAll(int Xmin,int Ymin,int Xmax,int Ymax);
native public  void SetPlotter(Graphic3d_Plotter aPlotter);
native public final void Plot();
native public final void SetGrid(gp_Ax3 aPlane,Aspect_Grid aGrid);
native public final void SetGridGraphicValues(Aspect_Grid aGrid);
native public final void SetGridActivity(boolean aFlag);
native public final double Tumble(int NbImages,boolean AnimationMode);
native public final void ScreenCopy(PlotMgt_PlotterDriver aPlotterDriver,boolean fWhiteBackground,double aScale);
final public boolean Dump(Standard_CString aFile) {
   return V3d_View_Dump_1(aFile);
}

private final native boolean V3d_View_Dump_1(Standard_CString aFile);

final public boolean Dump(Standard_CString aFile,short aFormat) {
   return V3d_View_Dump_2(aFile,aFormat);
}

private final native boolean V3d_View_Dump_2(Standard_CString aFile,short aFormat);

native public final Aspect_PixMap ToPixMap(int aWidth,int aHeight,int aCDepth);
native public final void SetProjModel(short amOdel);
native public final short ProjModel();
native public final void SetBackFacingModel(short aModel);
native public final short BackFacingModel();
public V3d_View() {
}




}
