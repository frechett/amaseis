//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         myscrollbar.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of myScrollBar (TScrollBar).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>

#include "myscrollbar.h"


//{{myScrollBar Implementation}}


myScrollBar::myScrollBar(TWindow* parent, int id, int x, int y, int w, int h, bool isHScrollBar, TModule* module)
:
    TScrollBar(parent, id, x, y, w, h, isHScrollBar, module)
{
  // Override the default window style for TScrollBar.
  //
      Attr.Style |= WS_CHILD | WS_VISIBLE;
    Attr.Style &= ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

  // INSERT>> Your constructor code here.

}


myScrollBar::~myScrollBar()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}

