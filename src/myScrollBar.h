//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         myscrollbar.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for myScrollBar (TScrollBar).
//
//----------------------------------------------------------------------------
#if !defined(myscrollbar_h)              // Sentry, use file only if it's not already included.
#define myscrollbar_h

#include <owl/scrollba.h>

#include "as1.rh"            // Definition of all resources.


//{{TScrollBar = myScrollBar}}
class myScrollBar : public TScrollBar {
  public:
    myScrollBar(TWindow* parent, int id, int x, int y, int w, int h, bool isHScrollBar, TModule* module = 0);
    virtual ~myScrollBar();
};    //{{myScrollBar}}


#endif  // myscrollbar_h sentry.

