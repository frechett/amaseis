//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         child.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for Child (TMDIChild).
//
//----------------------------------------------------------------------------
#if !defined(child_h)              // Sentry, use file only if it's not already included.
#define child_h

#include "as1.rh"            // Definition of all resources.


//{{TMDIChild = Child}}
class Child : public TMDIChild {
  public:
    Child(TMDIClient& parent, const char far* title, TWindow* clientWnd, bool shrinkToClient = false, TModule* module = 0);
    virtual ~Child();

//{{ChildVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{ChildVIRTUAL_END}}
};    //{{Child}}


#endif  // child_h sentry.
