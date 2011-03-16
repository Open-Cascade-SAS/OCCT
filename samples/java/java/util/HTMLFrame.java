
//Title:        Geological editor
//Version:      
//Copyright:    Copyright (c) 1998
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      EQCC
//Description:  Prototype of BRGM project


package util;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;
import java.net.*;
import java.io.IOException;
import javax.swing.text.html.*;
import javax.swing.border.*;


public class HTMLFrame extends JFrame
                       implements HyperlinkListener
{
  JEditorPane html;
  JViewport vp;
  String myPage;

//=======================================================================//
// Constructor
//=======================================================================//
  public HTMLFrame(String name)
  {
//    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    myPage = name;
    try
    {
      jbInit();
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
  }

//=======================================================================//
// Component initialization
//=======================================================================//
  private void jbInit() throws Exception
  {
    try
    {
      URL url = null;
      String prefix = "file:";

      try
      {
        url = new URL(prefix + myPage);
      }
      catch (MalformedURLException exc)
      {
        System.err.println("Attempted to open file "
                           + "with a bad URL: " + url);
        url = null;
      }

      if(url != null)
      {
        html = new JEditorPane(url);
        html.setEditable(false);
      	html.setContentType("text/html");
        html.addHyperlinkListener(this);

        JScrollPane scroller = new JScrollPane();
        scroller.setBorder(new SoftBevelBorder(BevelBorder.LOWERED));

        vp = scroller.getViewport();
        vp.add(html);
        vp.setBackingStoreEnabled(true);

        this.getContentPane().add(scroller, BorderLayout.CENTER);
        this.setSize(800, 600);
      }
    }
    catch (MalformedURLException e)
    {
      System.out.println("Malformed URL: " + e);
    }
    catch (IOException e)
    {
      System.out.println("IOException: " + e);
    }
  }


//=======================================================================//
//                          HyperlinkListener                            //
//=======================================================================//
  public void hyperlinkUpdate(HyperlinkEvent e)
  {
    if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED)
    {
      JEditorPane pane = (JEditorPane) e.getSource();
      if (e instanceof HTMLFrameHyperlinkEvent)
      {
        HTMLFrameHyperlinkEvent evt = (HTMLFrameHyperlinkEvent) e;
        HTMLDocument doc = (HTMLDocument)pane.getDocument();
        doc.processHTMLFrameHyperlinkEvent(evt);
      }
      else
      {
        try
        {
          pane.setPage(e.getURL());
      	  Point p = vp.getViewPosition();
      	  if (p.y < 0)
      	  {
      	    p.y = 0;
      	    vp.setViewPosition(p);
      	  }
        }
        catch (Throwable t)
        {
          t.printStackTrace();
        }
      }
    }
  }

}
