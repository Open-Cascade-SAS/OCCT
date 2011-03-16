
//Title:        Geological editor
//Version:      
//Copyright:    Copyright (c) 1998
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      EQCC
//Description:  Prototype of BRGM project


package util.win32;

import sun.awt.DrawingSurface;
import sun.awt.Win32DrawingSurface;
import sun.awt.DrawingSurfaceInfo;
import java.awt.peer.ComponentPeer;
import java.awt.Component;


public class WinHandleAccess implements util.handleAccess
{

  protected DrawingSurfaceInfo dsi;
  protected Win32DrawingSurface wds;
  protected int window, depth;

  protected void achieveData(java.awt.Component c, java.awt.Graphics g)
  {
//    System.out.println(">>> achieveData: Start...");
//    System.out.println(">>> achieveData: Component = " + c);
//    System.out.println(">>> achieveData: Graphics = " + g);

    dsi=null;
    wds=null;
    window=0; depth=0;

    ComponentPeer peer = c.getPeer();
//    System.out.println(">>> achieveData: Peer = " + peer);

    DrawingSurface ds = (DrawingSurface) peer;
//    System.out.println(">>> achieveData: DrawingSurface = " + ds);

    dsi = ds.getDrawingSurfaceInfo();
//    System.out.println(">>> achieveData: DrawingSurface = " + dsi);

    if(dsi!=null)
    {
      dsi.lock();
      wds = (Win32DrawingSurface)dsi.getSurface();
      dsi.unlock();
    }
//    System.out.println(">>> achieveData: Win32DrawingSurface = " + wds);

    if(wds!=null)
    {
      dsi.lock();

      //window = wds.getHDC();
      window = wds.getHWnd();
      depth = wds.getDepth();

/*
      System.out.println("WinHandleAccess:");
      System.out.println("\t wds = " + wds);
      System.out.println("\t wds.Depth = " + wds.getDepth());
      System.out.println("\t wds.HDC = " + wds.getHDC());
      System.out.println("\t wds.HWnd = " + wds.getHWnd());
*/

      dsi.unlock();
    }

    if (wds==null)
      System.out.println("Win32HandleAccess.getWinHandle failed, because the given Component is NOT a Window-Component\n");
  }

  public int getWinHandle(java.awt.Component c, java.awt.Graphics g)
  {
    achieveData(c, g);
    return window;
  }

  public int getWinDepth(java.awt.Component c, java.awt.Graphics g)
  {
    achieveData(c, g);
    return depth;
  }

  public int getVisualId(java.awt.GraphicsConfiguration gc)
  {
    return -1;
  } // Not avalaible under Windows

  public WinHandleAccess()
  {
  }

}
