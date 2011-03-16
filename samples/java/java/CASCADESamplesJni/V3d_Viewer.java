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
import CASCADESamplesJni.Aspect_GraphicDevice;
import jcas.Standard_ExtString;
import jcas.Standard_CString;
import jcas.Standard_Real;
import CASCADESamplesJni.V3d_TypeOfOrientation;
import CASCADESamplesJni.Quantity_NameOfColor;
import CASCADESamplesJni.V3d_TypeOfVisualization;
import CASCADESamplesJni.V3d_TypeOfShadingModel;
import CASCADESamplesJni.V3d_TypeOfUpdate;
import jcas.Standard_Boolean;
import CASCADESamplesJni.V3d_TypeOfSurfaceDetail;
import jcas.Standard_Short;
import CASCADESamplesJni.V3d_View;
import CASCADESamplesJni.V3d_OrthographicView;
import CASCADESamplesJni.V3d_PerspectiveView;
import CASCADESamplesJni.Quantity_TypeOfColor;
import CASCADESamplesJni.Quantity_Color;
import CASCADESamplesJni.V3d_TypeOfView;
import CASCADESamplesJni.gp_Ax3;
import CASCADESamplesJni.V3d_Light;
import CASCADESamplesJni.V3d_Plane;
import CASCADESamplesJni.Visual3d_ViewManager;
import CASCADESamplesJni.Aspect_GridType;
import CASCADESamplesJni.Aspect_GridDrawMode;
import CASCADESamplesJni.Graphic3d_AspectMarker3d;
import CASCADESamplesJni.Aspect_Grid;
import jcas.Standard_Integer;


public class V3d_Viewer extends CASCADESamplesJni.Viewer_Viewer {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public V3d_Viewer(Aspect_GraphicDevice Device,String aName,Standard_CString aDomain,double ViewSize,short ViewProj,short ViewBackground,short Visualization,short ShadingModel,short UpdateMode,boolean ComputedMode,boolean DefaultComputedMode,short SurfaceDetail) {
   V3d_Viewer_Create_0(Device,aName,aDomain,ViewSize,ViewProj,ViewBackground,Visualization,ShadingModel,UpdateMode,ComputedMode,DefaultComputedMode,SurfaceDetail);
}

private final native void V3d_Viewer_Create_0(Aspect_GraphicDevice Device,String aName,Standard_CString aDomain,double ViewSize,short ViewProj,short ViewBackground,short Visualization,short ShadingModel,short UpdateMode,boolean ComputedMode,boolean DefaultComputedMode,short SurfaceDetail);

native public final V3d_View CreateView();
native public final V3d_OrthographicView DefaultOrthographicView();
native public final V3d_PerspectiveView DefaultPerspectiveView();
final public void SetViewOn() {
    V3d_Viewer_SetViewOn_1();
}

private final native void V3d_Viewer_SetViewOn_1();

final public void SetViewOn(V3d_View View) {
    V3d_Viewer_SetViewOn_2(View);
}

private final native void V3d_Viewer_SetViewOn_2(V3d_View View);

final public void SetViewOff() {
    V3d_Viewer_SetViewOff_1();
}

private final native void V3d_Viewer_SetViewOff_1();

final public void SetViewOff(V3d_View View) {
    V3d_Viewer_SetViewOff_2(View);
}

private final native void V3d_Viewer_SetViewOff_2(V3d_View View);

native public final void Update();
native public final void UpdateLights();
native public final void Redraw();
native public final void Remove();
native public final void Erase();
native public final void UnHighlight();
final public void SetDefaultBackgroundColor(short Type,double V1,double V2,double V3) {
    V3d_Viewer_SetDefaultBackgroundColor_1(Type,V1,V2,V3);
}

private final native void V3d_Viewer_SetDefaultBackgroundColor_1(short Type,double V1,double V2,double V3);

final public void SetDefaultBackgroundColor(short Name) {
    V3d_Viewer_SetDefaultBackgroundColor_2(Name);
}

private final native void V3d_Viewer_SetDefaultBackgroundColor_2(short Name);

final public void SetDefaultBackgroundColor(Quantity_Color Color) {
    V3d_Viewer_SetDefaultBackgroundColor_3(Color);
}

private final native void V3d_Viewer_SetDefaultBackgroundColor_3(Quantity_Color Color);

native public final void SetDefaultViewSize(double Size);
native public final void SetDefaultViewProj(short Orientation);
native public final void SetDefaultVisualization(short Type);
native public final void SetZBufferManagment(boolean Automatic);
native public final boolean ZBufferManagment();
native public final void SetDefaultShadingModel(short Type);
native public final void SetDefaultSurfaceDetail(short Type);
native public final void SetDefaultAngle(double Angle);
native public final void SetUpdateMode(short Mode);
native public final void SetDefaultTypeOfView(short Type);
native public final void SetPrivilegedPlane(gp_Ax3 aPlane);
native public final gp_Ax3 PrivilegedPlane();
native public final void DisplayPrivilegedPlane(boolean OnOff,double aSize);
final public void SetLightOn(V3d_Light MyLight) {
    V3d_Viewer_SetLightOn_1(MyLight);
}

private final native void V3d_Viewer_SetLightOn_1(V3d_Light MyLight);

final public void SetLightOn() {
    V3d_Viewer_SetLightOn_2();
}

private final native void V3d_Viewer_SetLightOn_2();

final public void SetLightOff(V3d_Light MyLight) {
    V3d_Viewer_SetLightOff_1(MyLight);
}

private final native void V3d_Viewer_SetLightOff_1(V3d_Light MyLight);

final public void SetLightOff() {
    V3d_Viewer_SetLightOff_2();
}

private final native void V3d_Viewer_SetLightOff_2();

native public final void DelLight(V3d_Light MyLight);
native public final void SetCurrentSelectedLight(V3d_Light TheLight);
native public final void ClearCurrentSelectedLight();
final public void DefaultBackgroundColor(short Type,Standard_Real V1,Standard_Real V2,Standard_Real V3) {
    V3d_Viewer_DefaultBackgroundColor_1(Type,V1,V2,V3);
}

private final native void V3d_Viewer_DefaultBackgroundColor_1(short Type,Standard_Real V1,Standard_Real V2,Standard_Real V3);

final public Quantity_Color DefaultBackgroundColor() {
   return V3d_Viewer_DefaultBackgroundColor_2();
}

private final native Quantity_Color V3d_Viewer_DefaultBackgroundColor_2();

native public final double DefaultViewSize();
native public final short DefaultViewProj();
native public final short DefaultVisualization();
native public final short DefaultShadingModel();
native public final short DefaultSurfaceDetail();
native public final double DefaultAngle();
native public final short UpdateMode();
native public final boolean IfMoreViews();
native public final void InitActiveViews();
native public final boolean MoreActiveViews();
native public final void NextActiveViews();
native public final V3d_View ActiveView();
native public final boolean LastActiveView();
native public final void InitDefinedViews();
native public final boolean MoreDefinedViews();
native public final void NextDefinedViews();
native public final V3d_View DefinedView();
native public final void InitActiveLights();
native public final boolean MoreActiveLights();
native public final void NextActiveLights();
native public final V3d_Light ActiveLight();
native public final void InitDefinedLights();
native public final boolean MoreDefinedLights();
native public final void NextDefinedLights();
native public final V3d_Light DefinedLight();
native public final void InitDefinedPlanes();
native public final boolean MoreDefinedPlanes();
native public final void NextDefinedPlanes();
native public final V3d_Plane DefinedPlane();
native public final Visual3d_ViewManager Viewer();
native public final V3d_Light CurrentSelectedLight();
native public final boolean IsGlobalLight(V3d_Light TheLight);
native public final boolean ComputedMode();
native public final boolean DefaultComputedMode();
native public final void ActivateGrid(short aGridType,short aGridDrawMode);
native public final void DeactivateGrid();
final public void SetGridEcho(boolean showGrid) {
    V3d_Viewer_SetGridEcho_1(showGrid);
}

private final native void V3d_Viewer_SetGridEcho_1(boolean showGrid);

final public void SetGridEcho(Graphic3d_AspectMarker3d aMarker) {
    V3d_Viewer_SetGridEcho_2(aMarker);
}

private final native void V3d_Viewer_SetGridEcho_2(Graphic3d_AspectMarker3d aMarker);

native public final boolean GridEcho();
final public boolean IsActive() {
   return V3d_Viewer_IsActive_1();
}

private final native boolean V3d_Viewer_IsActive_1();

native public final Aspect_Grid Grid();
native public final short GridType();
native public final short GridDrawMode();
native public final void RectangularGridValues(Standard_Real XOrigin,Standard_Real YOrigin,Standard_Real XStep,Standard_Real YStep,Standard_Real RotationAngle);
native public final void SetRectangularGridValues(double XOrigin,double YOrigin,double XStep,double YStep,double RotationAngle);
native public final void CircularGridValues(Standard_Real XOrigin,Standard_Real YOrigin,Standard_Real RadiusStep,Standard_Integer DivisionNumber,Standard_Real RotationAngle);
native public final void SetCircularGridValues(double XOrigin,double YOrigin,double RadiusStep,int DivisionNumber,double RotationAngle);
native public final void CircularGridGraphicValues(Standard_Real Radius,Standard_Real OffSet);
native public final void SetCircularGridGraphicValues(double Radius,double OffSet);
native public final void RectangularGridGraphicValues(Standard_Real XSize,Standard_Real YSize,Standard_Real OffSet);
native public final void SetRectangularGridGraphicValues(double XSize,double YSize,double OffSet);
native public final void SetDefaultLights();
native public final void Init();
public V3d_Viewer() {
}




}
