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
import CASCADESamplesJni.Aspect_WindowDriver;
import CASCADESamplesJni.V2d_Viewer;
import jcas.Standard_Real;
import jcas.Standard_Integer;
import jcas.Standard_Boolean;
import jcas.Standard_CString;
import CASCADESamplesJni.Graphic2d_DisplayList;
import CASCADESamplesJni.Graphic2d_PickMode;
import jcas.Standard_Short;
import CASCADESamplesJni.V2d_TypeOfWindowResizingEffect;
import CASCADESamplesJni.PlotMgt_PlotterDriver;
import CASCADESamplesJni.Aspect_TypeOfColorSpace;
import CASCADESamplesJni.Graphic2d_View;
import CASCADESamplesJni.Quantity_NameOfColor;
import CASCADESamplesJni.Aspect_FillMethod;


public class V2d_View extends CASCADESamplesJni.Viewer_View {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public V2d_View(Aspect_WindowDriver aWindowDriver,V2d_Viewer aViewer,double aXCenter,double aYCenter,double aSize) {
   V2d_View_Create_0(aWindowDriver,aViewer,aXCenter,aYCenter,aSize);
}

private final native void V2d_View_Create_0(Aspect_WindowDriver aWindowDriver,V2d_Viewer aViewer,double aXCenter,double aYCenter,double aSize);

native public final void SetDefaultPosition(double aXCenter,double aYCenter,double aSize);
native public final void Fitall();
native public final void WindowFit(int aX1,int aY1,int aX2,int aY2);
final public void Fit(double aX1,double aY1,double aX2,double aY2,boolean UseMinimum) {
    V2d_View_Fit_1(aX1,aY1,aX2,aY2,UseMinimum);
}

private final native void V2d_View_Fit_1(double aX1,double aY1,double aX2,double aY2,boolean UseMinimum);

native public final void SetFitallRatio(double aRatio);
final public void Zoom(double Zoom) {
    V2d_View_Zoom_1(Zoom);
}

private final native void V2d_View_Zoom_1(double Zoom);

final public void Zoom(int aX1,int aY1,int aX2,int aY2,double aCoefficient) {
    V2d_View_Zoom_2(aX1,aY1,aX2,aY2,aCoefficient);
}

private final native void V2d_View_Zoom_2(int aX1,int aY1,int aX2,int aY2,double aCoefficient);

final public void Zoom(int aX,int aY,double aCoefficient) {
    V2d_View_Zoom_3(aX,aY,aCoefficient);
}

private final native void V2d_View_Zoom_3(int aX,int aY,double aCoefficient);

native public final void Magnify(V2d_View anOriginView,int X1,int Y1,int X2,int Y2);
native public final void Translate(double dx,double dy);
native public final void Place(int x,int y,double aZoomFactor);
native public final void ScreenPlace(double x,double y,double aZoomFactor);
native public final void Pan(int dx,int dy);
final public double Convert(int V) {
   return V2d_View_Convert_1(V);
}

private final native double V2d_View_Convert_1(int V);

final public void Convert(int X,int Y,Standard_Real ViewX,Standard_Real ViewY) {
    V2d_View_Convert_2(X,Y,ViewX,ViewY);
}

private final native void V2d_View_Convert_2(int X,int Y,Standard_Real ViewX,Standard_Real ViewY);

final public void Convert(double ViewX,double ViewY,Standard_Integer X,Standard_Integer Y) {
    V2d_View_Convert_3(ViewX,ViewY,X,Y);
}

private final native void V2d_View_Convert_3(double ViewX,double ViewY,Standard_Integer X,Standard_Integer Y);

final public double Convert(double aDriverSize) {
   return V2d_View_Convert_4(aDriverSize);
}

private final native double V2d_View_Convert_4(double aDriverSize);

native public final void Reset();
native public final void Previous();
native public final void DisableStorePrevious();
native public final void EnableStorePrevious();
native public final void Update();
native public final void UpdateNew();
native public final void RestoreArea(int Xc,int Yc,int Width,int Height);
native public final void Restore();
final public void Dump() {
    V2d_View_Dump_1();
}

private final native void V2d_View_Dump_1();

final public void Dump(Standard_CString aFileName) {
    V2d_View_Dump_2(aFileName);
}

private final native void V2d_View_Dump_2(Standard_CString aFileName);

final public Graphic2d_DisplayList Pick(int X,int Y,int aPrecision) {
   return V2d_View_Pick_1(X,Y,aPrecision);
}

private final native Graphic2d_DisplayList V2d_View_Pick_1(int X,int Y,int aPrecision);

final public Graphic2d_DisplayList Pick(int Xmin,int Ymin,int Xmax,int Ymax,short aPickMode) {
   return V2d_View_Pick_2(Xmin,Ymin,Xmax,Ymax,aPickMode);
}

private final native Graphic2d_DisplayList V2d_View_Pick_2(int Xmin,int Ymin,int Xmax,int Ymax,short aPickMode);

native public final void Erase();
native public final void MustBeResized(short anEffect);
native public final void HasBeenMoved();
final public void Plot(PlotMgt_PlotterDriver aPlotterDriver,double aXCenter,double aYCenter,double aScale) {
    V2d_View_Plot_1(aPlotterDriver,aXCenter,aYCenter,aScale);
}

private final native void V2d_View_Plot_1(PlotMgt_PlotterDriver aPlotterDriver,double aXCenter,double aYCenter,double aScale);

final public void Plot(PlotMgt_PlotterDriver aPlotterDriver,double aScale) {
    V2d_View_Plot_2(aPlotterDriver,aScale);
}

private final native void V2d_View_Plot_2(PlotMgt_PlotterDriver aPlotterDriver,double aScale);

native public final void PlotScreen(PlotMgt_PlotterDriver aPlotterDriver);
native public final void ScreenCopy(PlotMgt_PlotterDriver aPlotterDriver,boolean fWhiteBackground,double aScale);
native public final void PostScriptOutput(Standard_CString aFile,double aWidth,double aHeight,double aXCenter,double aYCenter,double aScale,short aTypeOfColorSpace);
native public final void ScreenPostScriptOutput(Standard_CString aFile,double aWidth,double aHeight,short aTypeOfColorSpace);
native public final void Hit(int X,int Y,Standard_Real gx,Standard_Real gy);
native public final void ShowHit(int X,int Y);
native public final void EraseHit();
native public final void SetDefaultHighlightColor(int aColorIndex);
native public final void SetDeflection(double aDeflection);
native public final double Deflection();
native public final Graphic2d_View View();
native public final V2d_Viewer Viewer();
native public final Aspect_WindowDriver Driver();
final public double Zoom() {
   return V2d_View_Zoom_4();
}

private final native double V2d_View_Zoom_4();

native public final void Center(Standard_Real aX,Standard_Real aY);
native public final double Size();
native public final short Color();
native public final void Scroll(Standard_Integer XCenter,Standard_Integer YCenter,Standard_Integer DX,Standard_Integer DY);
native public final int DefaultHighlightColor();
final public void Fit(int aX1,int aY1,int aX2,int aY2) {
    V2d_View_Fit_2(aX1,aY1,aX2,aY2);
}

private final native void V2d_View_Fit_2(int aX1,int aY1,int aX2,int aY2);

final public void SetBackground(short aNameColor) {
    V2d_View_SetBackground_1(aNameColor);
}

private final native void V2d_View_SetBackground_1(short aNameColor);

final public boolean SetBackground(Standard_CString aNameFile,short aMethod) {
   return V2d_View_SetBackground_2(aNameFile,aMethod);
}

private final native boolean V2d_View_SetBackground_2(Standard_CString aNameFile,short aMethod);

public V2d_View() {
}




}
