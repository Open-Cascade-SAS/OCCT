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
import CASCADESamplesJni.Graphic2d_View;
import CASCADESamplesJni.V2d_View;
import CASCADESamplesJni.Aspect_GenericColorMap;
import CASCADESamplesJni.Aspect_TypeMap;
import CASCADESamplesJni.Aspect_WidthMap;
import CASCADESamplesJni.Aspect_FontMap;
import jcas.Standard_Boolean;
import CASCADESamplesJni.Aspect_MarkMap;
import jcas.Standard_Integer;
import CASCADESamplesJni.Quantity_NameOfColor;
import jcas.Standard_Short;
import CASCADESamplesJni.Aspect_GridType;
import jcas.Standard_Real;
import CASCADESamplesJni.Aspect_GridDrawMode;


public class V2d_Viewer extends CASCADESamplesJni.Viewer_Viewer {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public V2d_Viewer(Aspect_GraphicDevice aGraphicDevice,String aName,Standard_CString aDomain) {
   V2d_Viewer_Create_1(aGraphicDevice,aName,aDomain);
}

private final native void V2d_Viewer_Create_1(Aspect_GraphicDevice aGraphicDevice,String aName,Standard_CString aDomain);

public V2d_Viewer(Aspect_GraphicDevice aGraphicDevice,Graphic2d_View aView,String aName,Standard_CString aDomain) {
   V2d_Viewer_Create_2(aGraphicDevice,aView,aName,aDomain);
}

private final native void V2d_Viewer_Create_2(Aspect_GraphicDevice aGraphicDevice,Graphic2d_View aView,String aName,Standard_CString aDomain);

native public final void AddView(V2d_View aView);
native public final void RemoveView(V2d_View aView);
native public final void Update();
native public final void UpdateNew();
native public final void SetColorMap(Aspect_GenericColorMap aColorMap);
native public final void SetTypeMap(Aspect_TypeMap aTypeMap);
native public final void SetWidthMap(Aspect_WidthMap aWidthMap);
native public final void SetFontMap(Aspect_FontMap aFontMap,boolean useMFT);
native public final void SetMarkMap(Aspect_MarkMap aMarkMap);
native public final Aspect_GenericColorMap ColorMap();
native public final Aspect_TypeMap TypeMap();
native public final Aspect_WidthMap WidthMap();
native public final Aspect_FontMap FontMap();
native public final Aspect_MarkMap MarkMap();
native public final boolean UseMFT();
native public final Graphic2d_View View();
native public final int InitializeColor(short aColor);
native public final void InitActiveViews();
native public final boolean MoreActiveViews();
native public final void NextActiveViews();
native public final V2d_View ActiveView();
native public final boolean IsEmpty();
native public final short GridType();
native public final void Hit(double X,double Y,Standard_Real gx,Standard_Real gy);
native public final void ActivateGrid(short aGridType,short aGridDrawMode);
native public final void DeactivateGrid();
native public final boolean IsActive();
native public final void RectangularGridValues(Standard_Real XOrigin,Standard_Real YOrigin,Standard_Real XStep,Standard_Real YStep,Standard_Real RotationAngle);
native public final void SetRectangularGridValues(double XOrigin,double YOrigin,double XStep,double YStep,double RotationAngle);
native public final void CircularGridValues(Standard_Real XOrigin,Standard_Real YOrigin,Standard_Real RadiusStep,Standard_Integer DivisionNumber,Standard_Real RotationAngle);
native public final void SetCircularGridValues(double XOrigin,double YOrigin,double RadiusStep,int DivisionNumber,double RotationAngle);
public V2d_Viewer() {
}




}
