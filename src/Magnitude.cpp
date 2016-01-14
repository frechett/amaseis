//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         magnitude.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of Magnitude (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"
#include "HelpManual\as1.h"

//Richter mb Q table
static float q[108][17] = {
{5.6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{5.8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{6.1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{6.4,6.3,6,6,6,6,5.9,5.7,5.6,5.7,5.7,5.8,5.8,5.8,5.7,5.7,5.7},
{6.5,6.5,6.2,6.1,6,6,6,5.7,5.7,5.7,5.8,5.9,5.8,5.8,5.8,5.7,5.7},
{7,6.8,6.5,6.4,6.3,6.2,6,5.8,5.7,5.8,5.9,5.9,5.9,5.9,5.8,5.7,5.7},
{7,7,6.6,6.5,6.4,6.4,6,5.8,5.8,5.9,6,6.1,6,5.9,5.8,5.8,5.7},
{7.2,7,6.8,6.6,6.6,6.4,6.1,5.8,5.8,6,6.1,6.1,6.1,6,5.9,5.8,5.7},
{7.3,7.1,6.9,6.8,6.7,6.4,6.1,5.9,5.9,6.1,6.2,6.2,6.2,6.1,5.8,5.8,5.7},
{7.2,7,6.9,6.7,6.6,6.4,6.2,5.9,6,6.2,6.2,6.2,6.2,6.1,5.9,5.8,5.8},
{7.1,7,6.8,6.7,6.5,6.3,6.2,6,6,6.2,6.2,6.2,6.2,6.2,6,5.8,5.8},
{7,6.9,6.7,6.5,6.4,6.2,6.2,6,6.1,6.2,6.2,6.2,6.3,6.2,6,5.9,5.8},
{6.6,6.5,6.5,6.1,6,6.1,6.2,6.1,6.1,6.2,6.2,6.3,6.3,6.2,6.1,5.9,5.8},
{6.3,6.1,6,6,6,6.1,6.2,6.2,6.1,6.1,6.2,6.3,6.4,6.3,6.1,5.9,5.8},
{5.9,5.9,5.9,5.9,6,6.1,6.2,6.2,6.2,6.1,6.2,6.3,6.4,6.3,6.2,6,5.9},
{5.9,5.9,5.9,6,6,6.1,6.2,6.2,6.2,6.1,6.1,6.3,6.4,6.4,6.2,6.1,5.9},
{5.9,5.9,5.9,6,6,6.1,6.2,6.3,6.2,6.1,6.1,6.3,6.4,6.4,6.3,6.1,6},
{6,6,6,6,6.1,6.1,6.3,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.3,6.1,6},
{6.1,6.1,6.1,6.1,6.1,6.2,6.3,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.4,6.2,6},
{6.1,6.2,6.1,6.1,6.1,6.2,6.3,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.4,6.2,6},
{6.2,6.2,6.2,6.2,6.1,6.2,6.3,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.4,6.3,6.1},
{6.3,6.3,6.2,6.2,6.1,6.2,6.4,6.3,6.2,6.1,6.2,6.3,6.4,6.4,6.4,6.3,6.1},
{6.4,6.3,6.3,6.2,6.2,6.3,6.4,6.3,6.2,6.1,6.2,6.3,6.3,6.4,6.4,6.4,6.1},
{6.5,6.4,6.3,6.2,6.2,6.3,6.4,6.3,6.2,6.1,6.2,6.3,6.3,6.4,6.4,6.4,6.2},
{6.5,6.4,6.3,6.3,6.3,6.4,6.5,6.4,6.2,6.1,6.2,6.2,6.3,6.4,6.4,6.4,6.2},
{6.5,6.4,6.4,6.3,6.3,6.4,6.5,6.4,6.2,6.1,6.2,6.2,6.3,6.4,6.4,6.4,6.3},
{6.6,6.5,6.4,6.4,6.4,6.5,6.5,6.4,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.4,6.3},
{6.6,6.5,6.4,6.4,6.4,6.5,6.5,6.4,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.4,6.3},
{6.6,6.6,6.5,6.5,6.5,6.5,6.5,6.4,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.4,6.3},
{6.7,6.6,6.5,6.5,6.5,6.5,6.5,6.4,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.4,6.3},
{6.7,6.7,6.6,6.6,6.5,6.6,6.4,6.4,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.4,6.4},
{6.7,6.7,6.6,6.6,6.6,6.5,6.4,6.4,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.4,6.4},
{6.7,6.7,6.7,6.7,6.6,6.5,6.4,6.4,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.4,6.3},
{6.6,6.7,6.7,6.7,6.7,6.5,6.4,6.3,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.3,6.3},
{6.6,6.7,6.7,6.7,6.7,6.5,6.4,6.3,6.3,6.1,6.1,6.2,6.3,6.4,6.4,6.3,6.3},
{6.5,6.6,6.7,6.7,6.7,6.5,6.4,6.3,6.2,6.1,6.1,6.2,6.3,6.4,6.4,6.3,6.3},
{6.5,6.6,6.7,6.7,6.7,6.5,6.4,6.3,6.2,6.1,6.1,6.2,6.3,6.4,6.3,6.3,6.3},
{6.4,6.5,6.6,6.7,6.6,6.5,6.4,6.3,6.1,6,6.1,6.2,6.3,6.4,6.3,6.3,6.3},
{6.4,6.5,6.6,6.7,6.6,6.5,6.3,6.2,6.1,6,6.1,6.2,6.3,6.4,6.3,6.2,6.3},
{6.5,6.5,6.5,6.6,6.6,6.4,6.3,6.2,6,6,6.1,6.2,6.3,6.3,6.3,6.2,6.3},
{6.5,6.5,6.5,6.6,6.6,6.4,6.3,6.2,6,6,6.1,6.2,6.3,6.3,6.3,6.2,6.3},
{6.5,6.5,6.5,6.6,6.6,6.4,6.3,6.1,6,6,6.1,6.2,6.3,6.3,6.3,6.2,6.3},
{6.6,6.6,6.5,6.6,6.6,6.4,6.3,6.1,6.1,6,6.1,6.2,6.3,6.3,6.3,6.2,6.2},
{6.7,6.7,6.6,6.6,6.6,6.4,6.2,6.1,6.1,6,6.1,6.2,6.3,6.3,6.3,6.2,6.2},
{6.8,6.7,6.7,6.7,6.6,6.4,6.2,6.1,6.1,6,6.1,6.2,6.3,6.3,6.3,6.2,6.2},
{6.9,6.8,6.7,6.7,6.6,6.4,6.2,6.1,6.1,6,6.1,6.2,6.3,6.3,6.3,6.2,6.2},
{6.9,6.8,6.8,6.7,6.6,6.5,6.2,6.1,6.1,6,6.1,6.2,6.2,6.3,6.3,6.2,6.2},
{6.8,6.8,6.8,6.8,6.7,6.5,6.2,6.2,6.1,6.1,6.1,6.2,6.2,6.3,6.3,6.2,6.2},
{6.7,6.8,6.8,6.8,6.8,6.5,6.3,6.2,6.1,6.1,6.1,6.1,6.2,6.3,6.3,6.1,6.1},
{6.7,6.7,6.8,6.8,6.8,6.5,6.3,6.2,6.2,6.1,6.1,6.1,6.2,6.2,6.2,6.1,6.1},
{6.7,6.7,6.8,6.8,6.8,6.5,6.4,6.2,6.2,6.1,6.1,6.1,6.1,6.2,6.2,6.1,6.1},
{6.7,6.7,6.8,6.8,6.8,6.6,6.4,6.2,6.2,6.1,6.1,6.1,6.1,6.1,6.2,6.1,6.1},
{6.8,6.8,6.8,6.8,6.8,6.6,6.4,6.3,6.2,6.1,6.1,6.1,6.1,6.1,6.1,6.1,6},
{6.8,6.8,6.8,6.8,6.8,6.6,6.5,6.3,6.2,6.2,6.1,6.1,6.1,6.1,6.1,6,6},
{6.8,6.8,6.8,6.8,6.8,6.7,6.5,6.3,6.2,6.2,6.1,6.1,6.1,6.1,6.1,6,6},
{6.8,6.8,6.8,6.9,6.8,6.7,6.5,6.4,6.2,6.2,6.2,6.2,6.1,6.1,6,6,6},
{6.8,6.8,6.9,6.9,6.8,6.7,6.5,6.4,6.3,6.2,6.2,6.2,6.1,6.1,6,6,6},
{6.9,6.9,6.9,6.9,6.9,6.7,6.5,6.4,6.3,6.2,6.2,6.2,6.2,6.1,6,6,6},
{6.9,6.9,6.9,6.9,6.9,6.7,6.5,6.4,6.3,6.3,6.2,6.2,6.2,6.1,6,6,6},
{6.9,6.9,6.9,6.9,6.8,6.7,6.5,6.4,6.3,6.3,6.3,6.3,6.2,6.2,6.1,6,6},
{7,6.9,6.9,6.9,6.8,6.7,6.6,6.4,6.4,6.3,6.3,6.3,6.3,6.2,6.1,6.1,6},
{7,6.9,6.9,6.8,6.7,6.7,6.6,6.5,6.4,6.4,6.4,6.3,6.3,6.2,6.2,6.1,6},
{7,6.9,6.8,6.7,6.7,6.7,6.6,6.5,6.5,6.4,6.4,6.4,6.4,6.3,6.2,6.1,6.1},
{7,6.9,6.8,6.7,6.7,6.7,6.6,6.5,6.5,6.5,6.4,6.4,6.4,6.3,6.2,6.1,6.1},
{7,6.9,6.8,6.7,6.7,6.7,6.5,6.5,6.5,6.5,6.5,6.4,6.4,6.3,6.2,6.2,6.1},
{7,6.9,6.8,6.7,6.7,6.6,6.5,6.5,6.5,6.5,6.5,6.4,6.4,6.3,6.3,6.2,6.1},
{7,6.9,6.8,6.7,6.7,6.6,6.5,6.5,6.5,6.5,6.5,6.4,6.4,6.3,6.3,6.2,6.2},
{7,6.9,6.7,6.7,6.6,6.6,6.5,6.5,6.5,6.5,6.4,6.4,6.4,6.3,6.3,6.2,6.2},
{6.9,6.9,6.7,6.7,6.6,6.6,6.5,6.5,6.5,6.5,6.4,6.4,6.3,6.3,6.3,6.2,6.2},
{6.9,6.9,6.7,6.7,6.6,6.6,6.5,6.5,6.5,6.5,6.4,6.4,6.3,6.3,6.3,6.3,6.2},
{6.9,6.8,6.7,6.7,6.6,6.5,6.5,6.5,6.5,6.5,6.4,6.4,6.3,6.3,6.3,6.3,6.2},
{6.9,6.8,6.7,6.7,6.6,6.5,6.5,6.5,6.5,6.5,6.4,6.4,6.3,6.3,6.3,6.3,6.3},
{6.8,6.8,6.7,6.7,6.6,6.5,6.5,6.5,6.5,6.5,6.4,6.4,6.3,6.3,6.3,6.3,6.3},
{6.8,6.8,6.7,6.7,6.6,6.5,6.5,6.5,6.5,6.5,6.5,6.4,6.3,6.2,6.3,6.3,6.3},
{6.9,6.8,6.7,6.7,6.6,6.5,6.5,6.5,6.5,6.5,6.5,6.4,6.3,6.2,6.3,6.3,6.3},
{6.9,6.8,6.8,6.7,6.6,6.5,6.5,6.5,6.5,6.6,6.5,6.4,6.2,6.2,6.2,6.3,6.3},
{6.9,6.8,6.8,6.7,6.6,6.5,6.5,6.5,6.5,6.6,6.5,6.4,6.2,6.2,6.2,6.3,6.3},
{6.8,6.8,6.7,6.7,6.6,6.5,6.5,6.5,6.6,6.6,6.5,6.4,6.2,6.2,6.2,6.3,6.3},
{6.7,6.8,6.7,6.7,6.6,6.5,6.5,6.5,6.6,6.6,6.5,6.4,6.2,6.2,6.2,6.3,6.3},
{6.8,6.8,6.7,6.7,6.6,6.5,6.5,6.5,6.6,6.6,6.5,6.4,6.3,6.3,6.3,6.3,6.3},
{6.9,6.8,6.8,6.7,6.6,6.5,6.5,6.5,6.6,6.6,6.5,6.4,6.3,6.3,6.3,6.3,6.3},
{7,6.9,6.8,6.7,6.7,6.6,6.5,6.5,6.6,6.6,6.5,6.5,6.3,6.3,6.3,6.4,6.3},
{7,7,6.8,6.8,6.7,6.6,6.5,6.6,6.6,6.6,6.5,6.5,6.4,6.4,6.4,6.4,6.3},
{7,7,6.9,6.8,6.7,6.6,6.5,6.6,6.6,6.6,6.6,6.5,6.4,6.4,6.4,6.4,6.4},
{6.9,7,7,6.8,6.8,6.6,6.6,6.6,6.6,6.7,6.6,6.5,6.5,6.5,6.5,6.5,6.4},
{7,7,7,6.9,6.8,6.7,6.6,6.6,6.7,6.7,6.6,6.5,6.5,6.5,6.5,6.5,6.4},
{7.1,7.1,7,6.9,6.8,6.8,6.6,6.6,6.7,6.7,6.6,6.6,6.6,6.6,6.6,6.5,6.4},
{7,7.1,7.1,7,6.9,6.8,6.7,6.7,6.7,6.7,6.6,6.6,6.6,6.7,6.7,6.6,6.5},
{7,7,7.1,7,6.9,6.8,6.7,6.7,6.7,6.7,6.6,6.7,6.7,6.7,6.7,6.7,6.5},
{7.1,7.1,7.2,7.1,7,6.9,6.8,6.7,6.7,6.7,6.7,6.7,6.7,6.8,6.8,6.7,6.6},
{7.1,7.2,7.2,7.2,7.1,6.9,6.8,6.8,6.7,6.8,6.7,6.8,6.8,6.8,6.8,6.8,6.7},
{7.2,7.2,7.2,7.2,7.1,7,6.9,6.8,6.8,6.8,6.8,6.8,6.8,6.9,6.8,6.9,6.7},
{7.1,7.2,7.2,7.2,7.2,7,6.9,6.9,6.9,6.9,6.9,6.9,6.9,6.9,7,6.9,6.8},
{7.2,7.2,7.2,7.2,7.2,7.1,7,7,6.9,6.9,6.9,6.9,6.9,7,7,7,6.9},
{7.3,7.2,7.3,7.3,7.3,7.2,7.1,7,7,7,6.9,7,7,7,7,7,6.9},
{7.4,7.3,7.3,7.3,7.3,7.2,7.1,7.1,7,7,7,7,7.1,7.1,7.1,7,7},
{7.5,7.3,7.3,7.3,7.3,7.3,7.2,7.1,7.1,7.1,7.1,7.1,7.1,7.1,7.1,7.1,7},
{7.5,7.3,7.3,7.3,7.4,7.3,7.2,7.2,7.2,7.1,7.1,7.2,7.2,7.2,7.2,7.1,7},
{7.3,7.3,7.3,7.4,7.4,7.3,7.2,7.2,7.2,7.2,7.2,7.2,7.2,7.2,7.2,7.2,7.1},
{7.4,7.3,7.4,7.4,7.4,7.4,7.3,7.3,7.3,7.3,7.3,7.3,7.3,7.3,7.3,7.2,7.1},
{7.4,7.4,7.4,7.5,7.5,7.5,7.4,7.3,7.3,7.3,7.4,7.4,7.4,7.4,7.3,7.3,7.2},
{7.5,7.5,7.5,7.5,7.6,7.6,7.5,7.4,7.4,7.4,7.5,7.5,7.5,7.5,7.4,7.3,7.2},
{7.6,7.6,7.6,7.7,7.7,7.7,7.7,7.6,7.6,7.6,7.6,7.6,7.6,7.6,7.5,7.4,7.3},
{7.7,7.7,7.7,7.8,7.8,7.8,7.8,7.8,7.8,7.8,7.7,7.7,7.7,7.6,7.6,7.5,7.4},
{7.8,7.8,7.8,7.8,7.8,7.9,7.9,7.9,7.9,7.9,7.8,7.8,7.7,7.7,7.6,7.5,7.4},
{7.8,7.8,7.9,7.9,7.9,7.9,7.9,8,8,8,7.9,7.8,7.7,7.7,7.6,7.5,7.4},
{7.9,7.9,7.9,7.9,7.9,8,8,8,8,8,8,7.8,7.8,7.7,7.7,7.6,7.5},
{8,8,8,8,8,8,8,8,8,8,8,7.9,7.8,7.7,7.7,7.6,7.5},
};

/***********************
// Routine from Ray Buland to compute mb
//
// Input:
// amp: amplitude in micrometers
// t: period in seconds
// delta: great-circle arc distance in degrees
// depth: depth of event in km
// q: Richter's Q factor (returned)
// ll: log(amp/t) (returned)
***********************/
static float mbmag(float amp, float t, float delta, float depth,
	float &qval, float &ll) throw (char *)
{
	qval = 0;
  ll = 0;
  //    if(t.lt..1.or.t.gt.9.) go to 11
	if(t < 0.1) throw "Period must be greater than 0.1 seconds.";
 	//     if(amp.eq.0.) go to 11
  if(amp == 0) return 0;
	//      if(delta.lt.5..or.delta.gt.109.) go to 11
  if(delta < 5 || delta > 109) throw "Great circle arc distance must\n"
  									"be between 5 and 109 degrees.";
	//c     if(delta.lt.5..and.depth.ne.0.) go to 11
  //    if(abs(res).gt.10..or.depth.gt.800.) go to 11
  if(depth > 800) throw "Depth must be less than 800 km.";
	//c
  //    do 7 k=1,17
  float dep, s1, s2;
  int k, j;
  for(k = 1;k<=17;k++) {
  	//    if(k.gt.5) go to 6
  	if(k <= 5) {
			//      dep=(k-1)*25
    	dep = (k-1)*25;
      //if(depth.ge.dep) go to 7
      if(depth >= dep) continue;
      //	s1=(depth+25.0-dep)*0.04
      s1 = (depth+25-dep)*0.04;
      //go to 8
      break;
    } else {
    	//6 dep=(k-3)*50
      dep = (k-3)*50;
      //if(depth.gt.dep) go to 7
      if(depth > dep) continue;
      //s1=(depth+50.0-dep)*0.02
      s1 = (depth+50-dep)*0.02;
    }
  }
  //go to 8
  //7 continue
  if(k >= 17) {
    k=17;
		//c     s1=0.0
    s1=(depth+50.0-700.)*0.02;
  }
  //8 do 9 j=1,108
  for(j=1;j<=108;j++) {
   	// del = j+1
    float del=j+1;
    //if(delta.ge.del) go to 9
    if(delta >= del) continue;
    //s2=delta+1.0-del
    s2=delta+1.0-del;
    //go to 10
    break;
  }
  //9 continue
  if(j >= 108) {
    j=108;
    //s2=0.0
    s2 = 0.0;
  }
 	//  10 q1=q(j-1,k-1)+s1*(q(j-1,k)-q(j-1,k-1))
  float q1=q[j-2][k-2]+s1*(q[j-2][k-1]-q[j-2][k-2]);
  //  q2=q(j,k-1)  +s1*(q(j,k)  -q(j,k-1))
  float q2=q[j-1][k-2]+s1*(q[j-1][k-1] -q[j-1][k-2]);
  //  qval=q1+s2*(q2-q1)
  qval=q1+s2*(q2-q1);
  //  mb=alog10(amp*.001/t)+qval
  ll = log10(amp/t);
  return ll + qval;
	//c     if((t.gt.3..or.(delta.lt.15..and.depth.lt.100.)).and.
	//c    1 mbflg.ne.'R') mbflg='r'
  //  if((t.gt.3..or.(delta.lt.15..and.depth.lt.100.)).and.
  // 1 mbflg.ne.'R'.and.mbflg.ne.'U') mbflg='r'
  //  return
	//c
  //11 mbflg='n'
  //   return
  //   end
}
//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(Magnitude, TDialog)
//{{MagnitudeRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDHELP, HandleHelp),
  EV_BN_CLICKED(IDOK, HandleOK),
  EV_BN_CLICKED(IDC_COMPUTE, Process),
//{{MagnitudeRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{Magnitude Implementation}}


//static MagnitudeXfer MagnitudeData;
//static    float raw_amp;
//static    float per;
//static    float inst_res;

Magnitude::Magnitude(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{MagnitudeXFER_USE}}
  raw_amplitude = new TEdit(this, IDC_RAW_AMPLITUDE, 16);
  period = new TEdit(this, IDC_PERIOD, 16);
  inst_response = new TStatic(this, IDC_INST_RESPONSE, 16);
  calibration_file = new TEdit(this, IDC_CALIBRATION_FILE, 48);
  amplitude_microns = new TStatic(this, IDC_AMPLITUDE_MICRONS, 16);
  gca = new TEdit(this, IDC_GCARC, 16);
  depth = new TEdit(this, IDC_DEPTH, 16);
  magnitude = new TStatic(this, IDC_MB, 16);
  q_field = new TStatic(this, IDC_Q, 16);
  log_field = new TStatic(this,IDC_LOG,16);
  response_table = new TStatic(this,IDC_RESPONSE_TABLE,255);

  //SetTransferBuffer(&MagnitudeData);
//{{MagnitudeXFER_USE_END}}

}


Magnitude::~Magnitude()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete raw_amplitude;
  delete period;
  delete inst_response;
  delete calibration_file;
  delete amplitude_microns;
  delete gca;
  delete depth;
  delete magnitude;
  delete q_field;
  delete log_field;
  delete response_table;
}

char * DisplayResponseTable(char *filename) {
  float period, response;
  char buf[48];
  FILE *fp = fopen(filename,"rt");
  static char line[1024];
  line[0] = 0;
  if(fp) {
  	while(fgets(buf,47,fp)) {
  		sscanf(buf,"%f %f",&period,&response);
    	sprintf(line+strlen(line),"%-7.3g\t%-7.3g\t%-7g\r",period,1./period,response);
  	}
  	fclose(fp);
  } else
  	return 0;
  return line;
}

float InterpolateResponse(char *cal_file, float per) {
  float inst_response1, inst_response2;
  float period1, period2;
  char buf[48];
  FILE *fp = fopen(cal_file,"rt");
  float inst_res = 0;
  if(fp) {
  	while(fgets(buf,47,fp)) {
			if(2 == sscanf(buf,"%f %f",&period1, &inst_response1)) {
      	if(period1 < per)
        	continue;
        else {
        	if(fgets(buf,47,fp)) {
          	if(2 == sscanf(buf,"%f %f",&period2, &inst_response2)) {
            	// Interpolate
              inst_res = inst_response1*(period2 - per);
              inst_res += inst_response2*(per - period1);
              inst_res = inst_res/(period2 - period1);
              break;
            }
            else inst_res = 0;
          }
        }
      } else continue;
    }
  	fclose(fp);
  }
  return inst_res;
}


void Magnitude::SetupWindow()
{
  TDialog::SetupWindow();

  // INSERT>> Your code here.
  //TransferData(tdSetData);
  char buf[48];
  sprintf(buf,"%g",profile->raw_amp);
  raw_amplitude->SetText(buf);
  sprintf(buf,"%g",profile->per);
  period->SetText(buf);
  sprintf(buf,"%g",profile->inst_res);
  inst_response->SetText(buf);
  sprintf(buf,"%g",profile->gca);
  gca->SetText(buf);
  sprintf(buf,"%g",profile->depth);
  depth->SetText(buf);
  calibration_file->SetText(profile->calibration_file);
  profile->inst_res = 0;
  char *line = DisplayResponseTable(profile->calibration_file);
  if(line) {
  	response_table->SetText(line);
  } else {
  	MessageBox("Can't open calibration file.");
  }

  Process();

}


void Magnitude::HandleOK()
{
  // INSERT>> Your code here.
  Process();
  CloseWindow();

}


void Magnitude::HandleHelp()
{
  // INSERT>> Your code here.
  //WinHelp("AS1.HLP", HELP_CONTEXT, Calibration);
  myHtmlHelp(Calibration);

}


void Magnitude::HandleCancel()
{
  // INSERT>> Your code here.
	CloseWindow();
}

void Magnitude::Process()
{
	// TransferData doesn't work.  That is, it doesn't hold the data in the
  // static structure.
  //TransferData(tdGetData);
  char buf[48];
  char cal_file[48];
  raw_amplitude->GetText(buf,15);
  profile->raw_amp = atof(buf);
  period->GetText(buf,15);
  profile->per = atof(buf);
  // Read calibration file
  calibration_file->GetText(cal_file,47);
  profile->inst_res = InterpolateResponse(cal_file,profile->per);
  if(profile->inst_res == 0) {
  	sprintf(buf,"Calibration file \"%s\" not found.",cal_file);
  	MessageBox(buf,"File not found");
    return;
  }
  sprintf(buf,"%g",profile->inst_res);
  inst_response->SetText(buf);

  gca->GetText(buf,15);
  profile->gca = atof(buf);
  depth->GetText(buf,15);
  profile->depth = atof(buf);
  if(profile->inst_res != 0) {
  	float am = profile->raw_amp/profile->inst_res;
    sprintf(buf,"%.3g",am);
    amplitude_microns->SetText(buf);
    float mb = 0;
    float q = 0;
    float ll = 0;
    try {
    	mb = mbmag(am,profile->per,profile->gca,profile->depth,q,ll);
    } catch (char *p) {
    	MessageBox(p);
    }
    sprintf(buf,"%.1f",mb);
    magnitude->SetText(buf);
    sprintf(buf,"%.1f",q);
    q_field->SetText(buf);
    sprintf(buf,"%.1f",ll);
    log_field->SetText(buf);
  }
}


