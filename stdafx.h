// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#pragma once


#include <iostream>
#include <tchar.h>
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <fstream>
#include <string.h>
#include <float.h>
#include <time.h>
using namespace std;

struct TpointList
{

  char experiment[256];

  int firstFile;
  int lastFile;
  int PL,minLeftRight;
  int count,count2,count3,count4,count5;

  double maxVel;
  double meanVel;
  double meanAcc;
  double meanDiss;
  double meanUSq;
  double meanUisq;
  double meanDudxsq;
  double Re;
  
  bool xuap;
  bool traj_point;
  bool pressure;
  bool Hessian;

  char path[256];

  double deltaT;
  double tolMaxVel;
  int polyConst;
  int maxRank;
  double maxRadius;
  double weDiv;
  double weAcc;
  double viscosity;

  double point[200][7000][48];
  bool occ[10000][7000];
  //int fast_search[200][12][12][12][100];

  int numOfFrames;
  bool changed;

  double u,ux,uy,uz;
  double v,vx,vy,vz;
  double w,wx,wy,wz; 
  double ax,axx,axy,axz;
  double ay,ayx,ayy,ayz;
  double az,azx,azy,azz; 

  double meaU[2];
  double meaV[2];
  double meaW[2];
  double meaAx[2];
  double meaAy[2];
  double meaAz[2];

  int maxRowIndex;

  double A  [300][20];
  double AT [20][300];
  double ATA[20][20];
  double B  [300][50];
  double BT [50][300];
  double BTB[50][50];
  double C[300][50];
  double CT [50][300];
  double CTC[50][50];
	
  double Y  [300];
  double y[50][300];
  double yC[5][300];

  double X  [300];
  double ATY[300];
  double BTY[300];
  double CTY[300];
  
  double Yu[300];
  double Yv[300];
  double Yw[300];
    
  double YuB[300];
  double YvB[300];
  double YwB[300];

  double YaxB[300];
  double YayB[300];
  double YazB[300];
    
  double Yaz[300];
  double Yay[300];
  double Yax[300];

  double YuxB[300];
  double YuyB[300];
  double YuzB[300];
  double YvxB[300];
  double YvyB[300];
  double YvzB[300];
  double YwxB[300];
  double YwyB[300];
  double YwzB[300];

  double YpxB[300];
  double YpyB[300];
  double YpzB[300];

  double Aij[3][3];
  double Aaij[3][3];
  double uxx[9];
  double pij[3][3];

  double pointPerRadius[300][2]; //0.1mm resolution up to 10mm.
  int dis[300];
  int disA[300];
  int disB[300];
  int disC[300];
  double traj[300][65];
  double we[300];
  int minTrajLength;
  int noDeriv;
  double c1;
  double c2;


  FILE *fpp;
};



// TODO: reference additional headers your program requires here
