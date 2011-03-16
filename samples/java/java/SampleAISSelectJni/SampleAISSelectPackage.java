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


package SampleAISSelectJni;

import CASCADESamplesJni.V3d_Viewer;
import jcas.Standard_ExtString;
import CASCADESamplesJni.V3d_View;
import jcas.Standard_Integer;
import CASCADESamplesJni.AIS_InteractiveContext;
import CASCADESamplesJni.TCollection_AsciiString;
import jcas.Standard_Real;
import jcas.Standard_Boolean;
import CASCADESamplesJni.Quantity_Color;


public class SampleAISSelectPackage {

 static {
    System.loadLibrary("SampleAISSelectJni");
 }
            
native public static V3d_Viewer CreateViewer(String aName);
native public static void SetWindow(V3d_View aView,int hiwin,int lowin);
native public static void DisplayBox(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void DisplayCylinder(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void SelectVertices(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void SelectEdges(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void SelectFaces(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void SelectNeutral(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static int MakeFillet(AIS_InteractiveContext aContext,double aValue,TCollection_AsciiString Message);
native public static boolean IsCylinderSelected(AIS_InteractiveContext aContext);
native public static void StartSelectFace(AIS_InteractiveContext aContext);
native public static Quantity_Color GetFaceColor(AIS_InteractiveContext aContext);
native public static void SetFaceColor(AIS_InteractiveContext aContext,Quantity_Color aColor,TCollection_AsciiString Message);
native public static void EndSelectFace(AIS_InteractiveContext aContext);



}
