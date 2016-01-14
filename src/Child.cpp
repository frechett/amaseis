//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         child.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of Child (TMDIChild).
//
//----------------------------------------------------------------------------

#include "as1.h"


//{{Child Implementation}}


//--------------------------------------------------------
// Child
// ~~~~~~~~~~
// Construction/Destruction handling.
//
Child::Child(TMDIClient& parent, const char far* title, TWindow* clientWnd,
	bool shrinkToClient, TModule* module) :
  //TMDIChild(parent, title, !clientWnd ? new TAS1EditFile(0, 0, 0) : clientWnd,
  TMDIChild(parent, title, clientWnd,
  	shrinkToClient, module)
{
  // INSERT>> Your constructor code here.

}


Child::~Child()
{
  Destroy();

  // INSERT>> Your destructor code here.

}

void Child::SetupWindow()
{
  TMDIChild::SetupWindow();

  // INSERT>> Your code here.

}

