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


package SampleViewer3DJni;

import CASCADESamplesJni.V3d_Viewer;
import jcas.Standard_ExtString;
import CASCADESamplesJni.V3d_View;
import jcas.Standard_Integer;
import SampleViewer3DJni.Graphic3d_GraphicDriver;
import CASCADESamplesJni.AIS_InteractiveContext;
import CASCADESamplesJni.TCollection_AsciiString;
import jcas.Standard_Real;
import CASCADESamplesJni.V3d_TypeOfZclipping;
import jcas.Standard_Short;
import jcas.Standard_Boolean;
import CASCADESamplesJni.V3d_TypeOfShadingModel;


public class SampleViewer3DPackage {

 static {
    System.loadLibrary("SampleViewer3DJni");
 }
            
native public static V3d_Viewer CreateViewer(String aName);
native public static void SetWindow(V3d_View aView,int hiwin,int lowin);
native public static Graphic3d_GraphicDriver CreateGraphicDriver();
native public static void DisplayBox(AIS_InteractiveContext aContext);
native public static void DisplayCylinder(AIS_InteractiveContext aContext);
native public static void DisplaySphere(AIS_InteractiveContext aContext);
native public static void EraseAll(AIS_InteractiveContext aContext);
native public static void CreateSpotLight(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void SetSpotLight(AIS_InteractiveContext aContext,V3d_View aView,int X,int Y,TCollection_AsciiString Message);
native public static void DirectingSpotLight(AIS_InteractiveContext aContext,V3d_View aView,int X,int Y);
native public static void DirectSpotLight(V3d_View aView,int X,int Y,TCollection_AsciiString Message);
native public static void ExpandingSpotLight(AIS_InteractiveContext aContext,V3d_View aView,int X,int Y);
native public static void ExpandSpotLight(AIS_InteractiveContext aContext);
native public static void CreatePositionalLight(AIS_InteractiveContext aContext,V3d_View aView,TCollection_AsciiString Message);
native public static void DirectingPositionalLight(V3d_View aView,int X,int Y);
native public static void DirectPositionalLight(AIS_InteractiveContext aContext,V3d_View aView,int X,int Y,TCollection_AsciiString Message);
native public static void CreateDirectionalLight(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void SetDirectionalLight(AIS_InteractiveContext aContext,V3d_View aView,int X,int Y,TCollection_AsciiString Message);
native public static void DirectingDirectionalLight(AIS_InteractiveContext aContext,V3d_View aView,int X,int Y);
native public static void DirectDirectionalLight(AIS_InteractiveContext aContext);
native public static void CreateAmbientLight(AIS_InteractiveContext aContext,V3d_View aView,TCollection_AsciiString Message);
native public static void ClearLights(V3d_View aView,TCollection_AsciiString Message);
native public static void ChangeZClippingDepth(V3d_View aView,double Depth,TCollection_AsciiString Message);
native public static void ChangeZClippingWidth(V3d_View aView,double Width,TCollection_AsciiString Message);
native public static void ChangeZClippingType(V3d_View aView,short Type,TCollection_AsciiString Message);
native public static void ChangeZCueingDepth(V3d_View aView,double Depth,TCollection_AsciiString Message);
native public static void ChangeZCueingWidth(V3d_View aView,double Width,TCollection_AsciiString Message);
native public static void ChangeZCueingOnOff(V3d_View aView,boolean IsOn,TCollection_AsciiString Message);
native public static void ChangeShadingModel(V3d_View aView,short Model);
native public static void ChangeAntialiasing(V3d_View aView,TCollection_AsciiString Message);
native public static void CreateClippingPlane(V3d_Viewer aViewer);
native public static void DisplayClippingPlane(AIS_InteractiveContext aContext,V3d_View aView,Standard_Real Z,Standard_Boolean IsOn);
native public static void ChangeModelClippingZ(AIS_InteractiveContext aContext,V3d_View aView,double Z,boolean IsOn,TCollection_AsciiString Message);
native public static void ChangeModelClippingOnOff(AIS_InteractiveContext aContext,V3d_View aView,boolean IsOn,TCollection_AsciiString Message);
native public static void ClearClippingPlane(AIS_InteractiveContext aContext,V3d_View aView,boolean IsOn);



}
