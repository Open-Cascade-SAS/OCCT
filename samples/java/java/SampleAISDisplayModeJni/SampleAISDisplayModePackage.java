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


package SampleAISDisplayModeJni;

import CASCADESamplesJni.V3d_Viewer;
import jcas.Standard_ExtString;
import CASCADESamplesJni.V3d_View;
import jcas.Standard_Integer;
import CASCADESamplesJni.AIS_InteractiveContext;
import CASCADESamplesJni.TCollection_AsciiString;
import CASCADESamplesJni.AIS_DisplayMode;
import jcas.Standard_Short;
import CASCADESamplesJni.Graphic3d_NameOfMaterial;
import CASCADESamplesJni.Quantity_Color;
import jcas.Standard_Real;


public class SampleAISDisplayModePackage {

 static {
    System.loadLibrary("SampleAISDisplayModeJni");
 }
            
native public static V3d_Viewer CreateViewer(String aName);
native public static void SetWindow(V3d_View aView,int hiwin,int lowin);
native public static void DisplayBox(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void DisplaySphere(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void DisplayCylinder(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void EraseAll(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void InitContext(AIS_InteractiveContext aContext);
native public static void GetIsosNumber(AIS_InteractiveContext aContext,Standard_Integer u,Standard_Integer v);
native public static void SetIsosNumber(AIS_InteractiveContext aContext,int u,int v,TCollection_AsciiString Message);
native public static void SetDisplayMode(AIS_InteractiveContext aContext,short aMode,TCollection_AsciiString Message);
native public static void SetObjectDisplayMode(AIS_InteractiveContext aContext,short aMode,TCollection_AsciiString Message);
native public static void SetObjectMaterial(AIS_InteractiveContext aContext,short aName,TCollection_AsciiString Message);
native public static Quantity_Color GetObjectColor(AIS_InteractiveContext aContext);
native public static void SetObjectColor(AIS_InteractiveContext aContext,Quantity_Color aColor,TCollection_AsciiString Message);
native public static double GetObjectTransparency(AIS_InteractiveContext aContext);
native public static void SetObjectTransparency(AIS_InteractiveContext aContext,double aValue,TCollection_AsciiString Message);
native public static void CreateLight(AIS_InteractiveContext aContext);
native public static void SetFirstPointOfLight(AIS_InteractiveContext aContext,V3d_View aView,int X,int Y);
native public static void MoveSecondPointOfLight(AIS_InteractiveContext aContext,V3d_View aView,int X,int Y);
native public static void SetSecondPointOfLight(AIS_InteractiveContext aContext);



}
