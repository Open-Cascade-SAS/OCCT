
//Title:        Geological editor
//Version:      
//Copyright:    Copyright (c) 1998
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      EQCC
//Description:  Prototype of BRGM project


package util.x11;

//import sun.awt.DrawingSurface;
//import sun.awt.X11DrawingSurface;
//import sun.awt.X11GraphicsConfig;
//import sun.awt.DrawingSurfaceInfo;
//import java.awt.peer.ComponentPeer;
import java.awt.Component;

public class X11HandleAccess implements util.handleAccess
{

//  protected DrawingSurfaceInfo dsi;
//  protected X11DrawingSurface wds;
  protected int window, depth;
  protected int visualID;

  protected void achieveData(java.awt.Component c, java.awt.Graphics g)
  {
//    System.out.println(">>> achieveData: Start...");
//    System.out.println(">>> achieveData: Component = " + c);
//    System.out.println(">>> achieveData: Graphics = " + g);
/*
    dsi = null;
    wds = null;
    window = 0; depth = 0;
    visualID = 0;

    ComponentPeer peer = c.getPeer();
//    System.out.println(">>> achieveData: Peer = " + peer);

    DrawingSurface ds = (DrawingSurface) peer;
//    System.out.println(">>> achieveData: DrawingSurface = " + ds);

    dsi = ds.getDrawingSurfaceInfo();
//    System.out.println(">>> achieveData: DrawingSurface = " + dsi);

    if(dsi!=null)
    {
      dsi.lock();
      wds = (X11DrawingSurface)dsi.getSurface();
      dsi.unlock();
    }
//    System.out.println(">>> achieveData: X11DrawingSurface = " + wds);

    if(wds!=null)
    {
      dsi.lock();

      window = wds.getDrawable();
//      window = wds.getDisplay();
      depth = wds.getDepth();
      visualID = wds.getVisualID();
*/
/*
      System.out.println("X11HandleAccess:");
      System.out.println("\t wds ="+wds);
      System.out.println("\t wds.Depth ="+wds.getDepth());
      System.out.println("\t wds.Drawable ="+wds.getDrawable());
      System.out.println("\t wds.Display ="+wds.getDisplay());
      System.out.println("\t wds.VisualID ="+wds.getVisualID());
*/
/*
      dsi.unlock();
    }*/

//    if(wds==null)
//      System.out.println("X11HandleAccess.getWinHandle failed, because the given Component is NOT a Window-Component\n");
  }

  public int getWinHandle(java.awt.Component c, java.awt.Graphics g)
  {
    //achieveData(c, g);
    return 0; //window;
  }

  public int getWinDepth(java.awt.Component c, java.awt.Graphics g)
  {
    //achieveData(c, g);
    return 1;// depth;
  }

  public int getVisualId(java.awt.GraphicsConfiguration gc)
  {
//    return visualID;

/*    X11GraphicsConfig X11GC = (X11GraphicsConfig) gc;
    if (X11GC != null)
      return X11GC.getVisual();
    else*/
      return -1;
  }

  public X11HandleAccess()
  {
  }

}
