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


package SampleGeometryJni;

import CASCADESamplesJni.V3d_Viewer;
import jcas.Standard_ExtString;
import CASCADESamplesJni.V3d_View;
import jcas.Standard_Integer;
import CASCADESamplesJni.V2d_Viewer;
import CASCADESamplesJni.V2d_View;
import CASCADESamplesJni.AIS_InteractiveContext;
import CASCADESamplesJni.TCollection_AsciiString;
import CASCADESamplesJni.ISession2D_InteractiveContext;
import jcas.Standard_Boolean;
import jcas.Standard_CString;


public class SampleGeometryPackage {

 static {
    System.loadLibrary("SampleGeometryJni");
 }
            
native public static V3d_Viewer CreateViewer3d(String aName);
native public static void SetWindow3d(V3d_View aView,int hiwin,int lowin);
native public static V2d_Viewer CreateViewer2d(String aName);
native public static V2d_View CreateView2d(V2d_Viewer aViewer,int hiwin,int lowin);
native public static void gpTest1(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest2(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest3(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest4(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest5(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest6(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest7(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest8(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest9(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest10(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest11(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest12(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest13(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest14(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest15(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest16(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest17(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest18(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest19(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest20(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest21(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest22(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest23(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest24(AIS_InteractiveContext aContext,ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest25(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest26(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest27(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest28(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest29(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest30(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest31(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest32(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest33(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest34(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest35(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest36(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest37(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest38(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest39(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest40(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest41(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest42(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest43(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest44(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest45(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest46(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest47(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest48(ISession2D_InteractiveContext aContext2D,TCollection_AsciiString Message);
native public static void gpTest49(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void gpTest50(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static boolean SaveImage(Standard_CString aFileName,Standard_CString aFormat,V3d_View aView);



}
