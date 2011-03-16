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


package SampleDisplayAnimationJni;

import CASCADESamplesJni.V3d_Viewer;
import jcas.Standard_ExtString;
import CASCADESamplesJni.V3d_View;
import jcas.Standard_Integer;
import jcas.Standard_Boolean;
import CASCADESamplesJni.AIS_InteractiveContext;
import jcas.Standard_CString;
import jcas.Standard_Real;


public class SampleDisplayAnimationPackage {

 static {
    System.loadLibrary("SampleDisplayAnimationJni");
 }
            
native public static V3d_Viewer CreateViewer(String aName);
native public static void SetWindow(V3d_View aView,int hiwin,int lowin);
native public static boolean LoadData(AIS_InteractiveContext aContext,Standard_CString aPath);
native public static void ChangePosition(AIS_InteractiveContext aContext);
native public static double GetDeviationCoefficient();
native public static void SetDeviationCoefficient(AIS_InteractiveContext aContext,double aValue);
native public static int GetAngleIncrement();
native public static void SetAngleIncrement(int aValue);
native public static boolean SaveImage(Standard_CString aFileName,Standard_CString aFormat,V3d_View aView);



}
