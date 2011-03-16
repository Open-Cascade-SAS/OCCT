
import java.awt.*;

// cascade includes 
import jcas.*;
import CASCADESamplesJni.*;
import util.*;
import SampleGeometryJni.*;

//****************************************************************************************************************
/** */
public class SimpleViewer extends Canvas {
//****************************************************************************************************************
  private boolean hasWindow = false;
  V3d_Viewer myViewer = null;
  V3d_View myView = null;
  static{
    System.out.println("Info: Load NativePaint lib...");
    System.loadLibrary("TKNativePaint");
    System.out.println("Info: Loading NativePaint lib done");
  }

//================================================================================================================
  /** */
  public SimpleViewer(){
//================================================================================================================
    myViewer = SampleGeometryPackage.CreateViewer3d("v3d");
    myView = myViewer.CreateView();
  }

//================================================================================================================
  /**
    */
  public native void paint(Graphics g);
//================================================================================================================

}
