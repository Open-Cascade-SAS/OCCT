
//Title:        Testing viewer
//Version:      
//Copyright:    Copyright (c) 1998
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      EQCC
//Description:  


package util;

import java.io.File;
import java.util.*;
import javax.swing.*;
import javax.swing.filechooser.*;


public class ExtensionFileFilter extends FileFilter
{
  private Hashtable filters = null;
  private String description = null;
  private String fullDescription = null;
  private boolean useExtensionsInDescription = true;


//=======================================================================//
// Constructors
//=======================================================================//
  public ExtensionFileFilter()
  {
  	this.filters = new Hashtable();
  }

  public ExtensionFileFilter(String extension, String description)
  {
  	this();
    if(extension != null) addExtension(extension);
   	if(description != null) setDescription(description);
  }

  public ExtensionFileFilter(String[] filters)
  {
  	this(filters, null);
  }

  public ExtensionFileFilter(String[] filters, String description)
  {
  	this();
    for (int i = 0; i < filters.length; i++)
    {
	    // add filters one by one
	    addExtension(filters[i]);
	  }
    if(description != null) setDescription(description);
  }


//=======================================================================//
// Overriden function
//=======================================================================//
  public boolean accept(File f)
  {
    if (f != null)
    {
	    if(f.isDirectory()) return true;

	    String extension = getExtension(f);
	    if (extension != null && filters.get(getExtension(f)) != null)
    		return true;
  	}

	  return false;
  }

  public String getDescription()
  {
    if (fullDescription == null)
    {
	    if (description == null || isExtensionListInDescription())
      {
     		fullDescription = (description == null) ? "(" : description + " (";

		    // build the description from the extension list
		    Enumeration extensions = filters.keys();
    		if (extensions != null)
        {
  		    fullDescription += "." + (String) extensions.nextElement();
	  	    while (extensions.hasMoreElements())
      			fullDescription += ", " + (String) extensions.nextElement();
		    }

		    fullDescription += ")";
	    }
      else
    		fullDescription = description;
	  }

	  return fullDescription;
  }


  public String[] getExtensions()

  {

    if (filters.size() == 0)

      return null;


    String[] ext = new String[filters.size()];


    Enumeration anEnum = filters.keys();

    int i = 0;

    while (anEnum.hasMoreElements())

      ext[i] = new String((String) anEnum.nextElement());


    return ext;

  }

//=======================================================================//
// Additional function
//=======================================================================//
  public String getExtension(File f)
  {
	  if (f != null)
	    return getExtension(f.getName());
    else
    	return null;
  }

  public String getExtension(String filename)
  {
	  if (filename != null)
    {
      int k = filename.lastIndexOf(System.getProperty("file.separator"));
      String name = filename.substring(k+1);
	    int i = name.lastIndexOf('.');
	    if (i>0 && i<name.length()-1)
    		return name.substring(i+1).toLowerCase();
	  }

  	return null;
  }

  public void addExtension(String extension)
  {
    if (filters == null)
	    filters = new Hashtable(5);

  	filters.put(extension.toLowerCase(), this);
	  fullDescription = null;
  }

  public void setDescription(String description)
  {
	  this.description = description;
  	fullDescription = null;
  }

  public void setExtensionListInDescription(boolean b)
  {
	  useExtensionsInDescription = b;
  	fullDescription = null;
  }

  public boolean isExtensionListInDescription()
  {
  	return useExtensionsInDescription;
  }

}
