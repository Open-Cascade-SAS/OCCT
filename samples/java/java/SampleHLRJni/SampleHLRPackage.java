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


package SampleHLRJni;

import CASCADESamplesJni.V3d_Viewer;
import jcas.Standard_ExtString;
import CASCADESamplesJni.V3d_View;
import jcas.Standard_Integer;
import CASCADESamplesJni.V2d_Viewer;
import CASCADESamplesJni.V2d_View;
import CASCADESamplesJni.AIS_InteractiveContext;
import jcas.Standard_Boolean;
import CASCADESamplesJni.ISession2D_InteractiveContext;
import jcas.Standard_CString;


public class SampleHLRPackage {

 static {
    System.loadLibrary("SampleHLRJni");
 }
            
native public static V3d_Viewer CreateViewer3d(String aName);
native public static void SetWindow3d(V3d_View aView,int hiwin,int lowin);
native public static V2d_Viewer CreateViewer2d(String aName);
native public static V2d_View CreateView2d(V2d_Viewer aViewer,int hiwin,int lowin);
native public static void InitMaps(V2d_Viewer aViewer);
native public static void DisplayTrihedron(AIS_InteractiveContext aContext);
native public static boolean GetShapes(AIS_InteractiveContext aSrcContext,AIS_InteractiveContext aDestContext);
native public static void Apply(ISession2D_InteractiveContext aContext2d,int aDisplayMode);
native public static void UpdateProjector(V3d_View aView);
native public static void SetNbIsos(int aNbIsos);
native public static boolean ReadBRep(Standard_CString aFileName,AIS_InteractiveContext aContext);
native public static boolean SaveBRep(Standard_CString aFileName,AIS_InteractiveContext aContext);
static public boolean SaveImage(Standard_CString aFileName,Standard_CString aFormat,V3d_View aView) {
   return SampleHLRPackage_SaveImage_1(aFileName,aFormat,aView);
}

private static native boolean SampleHLRPackage_SaveImage_1(Standard_CString aFileName,Standard_CString aFormat,V3d_View aView);

static public boolean SaveImage(Standard_CString aFileName,Standard_CString aFormat,V2d_View aView) {
   return SampleHLRPackage_SaveImage_2(aFileName,aFormat,aView);
}

private static native boolean SampleHLRPackage_SaveImage_2(Standard_CString aFileName,Standard_CString aFormat,V2d_View aView);




}
