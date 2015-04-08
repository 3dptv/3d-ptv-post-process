//////////////////////////////////////////////////////////////////////////////
//
// this code was written by Beat Luthi at IfU, ETH Zürich, Okt 2007
//
// is represents an attempt to have ONE clean non-GUI version of the postPorcessing codes
// that float around in various Borland versions
//
// luethi@ifu.baug.ethz.ch
//
// update/change ...
//
//////////////////////////////////////////////////////////////////////////////

/*

This software links 3D particle positions of consequtivee time steps. 

Copyright (C) 2006 Beat Luthi, Risø, Nat. Lab, Denmark

This program is free software; you can redistribute it and/or modify it under 
the terms of the GNU General Public License v2, as published by the Free 
Software Foundation, provided that the above copyright notices and this 
permission notice appear in all copies of the software and related documentation.
You may charge a fee for the physical act of transferring a copy, and you may at 
your option offer warranty protection in exchange for a fee.

You may not copy, modify, sublicense, or distribute the Program except as 
expressly provided under this License.

This program is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR 
A PARTICULAR PURPOSE.

*/

#pragma GCC diagnostic ignored "-Wformat-security"

#include "stdafx.h"


TpointList pointList;

FILE * input;
FILE *fpp;
int n;
float e;

static void flushline(FILE * fp);
static void readPTVFile(int n, int index);
static void doCubicSplinesTwenty(bool single,int number);
static void setAllMatrixesToZero(int size);
static void makeAT(int n, int m);
static void makeATA(int n, int m);
static void makeATY(int n, int m,int wh);
static bool solve(int n, int m);
static void writeXUAPFile(int t);
static void followTrajPoint(FILE *fpp, int t,int startPoint);
static void followTrajPoint_pressure(FILE *fpp, int t,int startPoint);
static void followTrajPoint_Hessian(FILE *fpp, int t,int startPoint);
static void readXUAPFile(int n, bool firstTime);
static void readXUAGFile(int n, bool firstTime);
static void readXUAGPFile(int n, bool firstTime);


int main(int argc, char *argv[])
{
	char garb[10];
	char pa[256];
	int PLh;
	char name[256];
	int c;

	//begin of read in control parameters
	///////////////////////////////////////////////////////////////////////////////////
	if (argc == 1) {
		//if (NULL == (input = fopen("D:/post_proc_FOR_DEBASHISH.inp","r"))){ //_FOR_MARKUS
		if (NULL == (input = fopen("input.inp","r"))){
		    cout<< "\ndid not find *.inp file";
	    }
	    else{
		    cout<< "\nautomatically and succesfully opened *.inp file \n";
	    }
	}
	else{
	    if (NULL == (input = fopen(argv[1],"r"))){
		    cout<< "\ndid not find *.inp file";
	    }
	    else{
		    cout<< "\nsuccesfully opened *.inp file \n";
	    }
	}
	//what should be done?
	fscanf(input,"%i",&n); flushline(input); if(n==1){pointList.xuap=true;}			else{pointList.xuap=false;}
	fscanf(input,"%i",&n); flushline(input); if(n==1){pointList.traj_point=true;}		else{pointList.traj_point=false;}
	fscanf(input,"%i",&n); flushline(input); if(n==1){pointList.derivatives=true;}		else{pointList.derivatives=false;}
	fscanf(input,"%i",&n); flushline(input); if(n==1){pointList.pressure=true;}		else{pointList.pressure=false;}
	fscanf(input,"%i",&n); flushline(input); if(n==1){pointList.Hessian=true;}		else{pointList.Hessian=false;}


	//data
    fscanf(input,"%s",pa); flushline(input);sprintf (pointList.path,pa);
    fscanf(input,"%i",&n); flushline(input);pointList.firstFile                = n;
	fscanf(input,"%i",&n); flushline(input);pointList.lastFile                 = n;

    //fact
	fscanf(input,"%f",&e); flushline(input);pointList.deltaT                   = e;
	fscanf(input,"%f",&e); flushline(input);pointList.viscosity                = e;

	//controls xuap
	fscanf(input,"%i",&n); flushline(input);pointList.PL                       = n;
	fscanf(input,"%i",&n); flushline(input);pointList.minLeftRight             = n;
	fscanf(input,"%f",&e); flushline(input);pointList.tolMaxVel                = e;

	//controls traj_accc
	fscanf(input,"%f",&e); flushline(input);pointList.maxRadius                = e;
	fscanf(input,"%f",&e); flushline(input);pointList.weDiv                    = e;
	fscanf(input,"%f",&e); flushline(input);pointList.weAcc                    = e;
	flushline(input);
	fscanf(input,"%i",&n); flushline(input);pointList.minTrajLength            = n;
	fscanf(input,"%i",&n); flushline(input);pointList.polyConst                = n;
	fscanf(input,"%f",&e); flushline(input);pointList.c1                       = e;
	fscanf(input,"%f",&e); flushline(input);pointList.c2                       = e;
	fscanf(input,"%i",&n); flushline(input);pointList.maxRank                  = n;
	fscanf(input,"%i",&n); flushline(input);pointList.numOfFrames              = n;
	

	cout << "xuap ... "<<pointList.xuap<<"\n";
    cout << "traj_point ... "<<pointList.traj_point<<"\n";
    cout << "path ... "<<pointList.path<<"\n";
    cout << pointList.pressure;
    
	
	//end of read in control parameters
	///////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////
	if(pointList.xuap){
	   PLh=int((double)pointList.PL/2.);
       pointList.count=0;
       pointList.maxVel=0.;
       pointList.meanVel=0.;
       pointList.meanAcc=0.;

       for (int i=pointList.firstFile;i<pointList.lastFile+1;i++){
	      if(i % 20 == 0){
             cout << "processing file ........."<<i<<"\n";
             cout << "max Vel.................."<<pointList.maxVel<<"\n";
	         cout << "mean Vel................."<<pointList.meanVel<<"\n";
	         cout << "mean Acc................."<<pointList.meanAcc<<"\n\n";
	      }
	      for (int ii=-PLh;ii<PLh+1;ii++){
              readPTVFile(i,ii);
          }
	      doCubicSplinesTwenty(false,0);
          writeXUAPFile(i);
	   }
	}
    ///////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	if(pointList.traj_point){

       pointList.count=0;
	   pointList.count2=0;
       pointList.count3=0;
	   pointList.count4=0;
	   pointList.count5=0;
       pointList.meanDiss=0.;
	   pointList.meanUSq=0.;

       for (int i=pointList.firstFile;i<pointList.lastFile+1;i++){
		  if((double)pointList.count3/(double)pointList.count>0){
             cout << "point per sphere.............."<<(double)pointList.count3/(double)pointList.count<<"\n";
		     cout << "% rel. diva < 0.1............."<<100.*(double)pointList.count4/(double)pointList.count<<"\n";
	         cout << "% rel. acc  < 0.2............."<<100.*(double)pointList.count5/(double)pointList.count6<<"\n";
	         cout << "r.m.s. u [m/s]................"<<pow(pointList.meanUSq,0.5)<<"\n";
		     cout << "mean dissipation [m^2/s^3]...."<<pointList.meanDiss<<"\n\n";
		  }
		  cout << "processing file .............."<<i<<"\n";
		   
          c=sprintf (name, pointList.path);
	      c+=sprintf (name+c, "/n_trajPoint.");
          c+=sprintf (name+c, "%1d", i); 
          fpp = fopen(name,"w");
          followTrajPoint(fpp,i,0);
          fclose (fpp);
      
       }
	}
	if(pointList.pressure){

       pointList.count=0;
	   pointList.count2=0;
       pointList.count3=0;
	   pointList.count4=0;
	   pointList.count5=0;
	   pointList.count6=0;
       pointList.meanDiss=0.;
	   pointList.meanUSq=0.;

	   for(int i=0;i<10000;i++){
		   for(int j=0;j<7000;j++){
               pointList.occ[i][j]=false;
		   }
	   }

       for (int i=pointList.firstFile;i<pointList.lastFile+1;i++){
		  cout << "processing file .............."<<i<<" for pressure gradient\n";
          c=sprintf (name, pointList.path);
	      c+=sprintf (name+c, "/n_trajPoint.");
          c+=sprintf (name+c, "%1d", i); 
          fpp = fopen(name,"w");
		  followTrajPoint_pressure(fpp,i,0);
		  fclose (fpp);
       }
	}
	if(pointList.Hessian){

       pointList.count=0;
	   pointList.count2=0;
       pointList.count3=0;
	   pointList.count4=0;
	   pointList.count5=0;
       pointList.meanDiss=0.;
	   pointList.meanUSq=0.;

	   for(int i=0;i<10000;i++){
		   for(int j=0;j<7000;j++){
               pointList.occ[i][j]=false;
		   }
	   }

       for (int i=pointList.firstFile;i<pointList.lastFile+1;i++){

          if((double)pointList.count2/(double)pointList.count>0){
		     cout << "% rel. sym. of pij < 0.2.................."<<100.*(double)pointList.count2/(double)pointList.count<<"\n\n";
		  }
		  cout << "processing file "<<i<<" for pressure Hessian\n";
          c=sprintf (name, pointList.path);
	      c+=sprintf (name+c, "/H_trajPoint.");
          c+=sprintf (name+c, "%1d", i); 
          fpp = fopen(name,"w");
		  followTrajPoint_Hessian(fpp,i,0);
		  fclose (fpp);
       }
	}
	///////////////////////////////////////////////////////////////////////////////////
	scanf("Please hit a key  %s", garb);

	return 0;
}

void flushline(FILE * fp)
{
    while(fgetc(fp)!='\n' && !feof(fp));
}

void readPTVFile(int n, int index)
{
    FILE *fpp;
	int c;
    int numOfPoints;
    int left,right;
    double x,y,z,rmsDist;

    char name[256];

    if(n+index>pointList.firstFile-1 && n+index<pointList.lastFile+1){
       c=sprintf (name, pointList.path);
	   c+=sprintf (name+c, "/ptv_is.");
       c+=sprintf (name+c, "%d", n+index);
    
     if (NULL == (fpp = fopen(name,"r"))){
		    cout<< "\n did not find "<<name;
	    }
     else{
         cout<< "\n succesfully opened file \n"<<name;
     }
       fscanf (fpp, "%d\0", &numOfPoints);
       pointList.point[index+10][0][0]=numOfPoints;
       for (int i=1; i<numOfPoints+1; i++){
           fscanf (fpp, "%d\0", &left);
           fscanf (fpp, "%d\0", &right);
           fscanf (fpp, "%lf\0", &x);
           fscanf (fpp, "%lf\0", &y);
           fscanf (fpp, "%lf\0", &z);
           rmsDist=0.005;
           pointList.point[index+10][i][0]=left+1;//;//
		   pointList.point[index+10][i][1]=right+1;//;//

           pointList.point[index+10][i][2]=x*0.001;//;//
           pointList.point[index+10][i][3]=y*0.001;//;//
           pointList.point[index+10][i][4]=z*0.001;//;//
           pointList.point[index+10][i][15]=rmsDist;
      }
       fclose (fpp);
    }
    else{
       pointList.point[index+10][0][0]=0;
    }
}

void doCubicSplinesTwenty(bool single,int number)
{

   int PLh=int((double)pointList.PL/2.);
   int nP=pointList.point[10][0][0];
   int ind[21];
   //double tolerance=0.15;//StrToFloat(paramForm->toleranceEdit->Text);
   double velocity;

   double weight;

   int start,end;
   if(!single){
      start=1;
      end=nP;
   }
   else{
      start=number;
      end=number;
   }

   for(int i=start;i<end+1;i++){
      pointList.point[10][i][14]=0; //can be cubic splined
      int maxIndex=10;
      int minIndex=10;
      int index=10;
      int badCounter=0;
      ind[index]=i;
      bool ok=true;

      while(index>10-PLh && ok){
          if(pointList.point[index][ind[index]][0]>0 && pointList.point[index][0][0]>0){
             ind[index-1]=pointList.point[index][ind[index]][0];
             index--;
             minIndex=index;
          }
          else{
             ok=false;
          }
      }
      index=10;
      ind[index]=i;
      ok=true;
      while(index<10+PLh && ok){
          if(pointList.point[index][ind[index]][1]>0 && pointList.point[index][0][0]>0){
             ind[index+1]=pointList.point[index][ind[index]][1];
             index++;
             maxIndex=index;
          }
          else{
             ok=false;
          }
      }

      //first do for x and u, then do for a
      if(maxIndex-minIndex>2+badCounter && maxIndex>9+pointList.minLeftRight && minIndex<11-pointList.minLeftRight){ 
//    if(maxIndex-minIndex>2+badCounter && maxIndex>9+minLength && minIndex<11-minLength){ 
      
	  //if(maxIndex-minIndex>2+badCounter ){ //ok (minIndex<10 && maxIndex>10){//
          pointList.point[10][i][14]=1;
          //x-Component
          setAllMatrixesToZero(4);
          for(int t=minIndex-10;t<maxIndex-10+1;t++){
              weight     = pointList.point[t+10][ind[t+10]][15];
              weight     = 1.-1./(1.+exp(-300.*(weight-0.015)));
			  weight   = 1.; //Beat March 2009
              pointList.A[t+10][0] = 1.*weight;
              pointList.A[t+10][1] = (double)t*pointList.deltaT*weight;
              pointList.A[t+10][2] = pow((double)t*pointList.deltaT,2.)*weight;
              pointList.A[t+10][3] = pow((double)t*pointList.deltaT,3.)*weight;
              pointList.y[0][t+10] = pointList.point[t+10][ind[t+10]][2]*weight;
          }
          makeAT(21,4);
          makeATA(21,4);
          makeATY(21,4,0);
          solve(21,4);

          pointList.point[10][i][5]=pointList.X[0];//pointList.point[10][ind[10]][2];//
          pointList.point[10][i][8]=pointList.X[1];//(1./(2.*pointList.pointList.deltaT))*(pointList.point[11][ind[11]][2]-pointList.point[9][ind[9]][2]);//
          pointList.point[10][i][11]=2.*pointList.X[2];//(1./(pointList.pointList.deltaT*pointList.pointList.deltaT))*(pointList.point[11][ind[11]][2]-2.*pointList.point[10][ind[10]][2]+pointList.point[9][ind[9]][2]);//
          //y-Component
          setAllMatrixesToZero(4);
          for(int t=minIndex-10;t<maxIndex-10+1;t++){
              weight     = pointList.point[t+10][ind[t+10]][15];
              weight     = 1.-1./(1.+exp(-300.*(weight-0.015)));
              pointList.A[t+10][0] = 1.*weight;
              pointList.A[t+10][1] = (double)t*pointList.deltaT*weight;
              pointList.A[t+10][2] = pow((double)t*pointList.deltaT,2.)*weight;
              pointList.A[t+10][3] = pow((double)t*pointList.deltaT,3.)*weight;
              pointList.y[0][t+10] = pointList.point[t+10][ind[t+10]][3]*weight;
          }
          makeAT(21,4);
          makeATA(21,4);
          makeATY(21,4,0);
          solve(21,4);

          pointList.point[10][i][6]=pointList.X[0]; //pointList.point[10][ind[10]][3];//
          pointList.point[10][i][9]=pointList.X[1]; //(1./(2.*pointList.pointList.deltaT))*(pointList.point[11][ind[11]][3]-pointList.point[9][ind[9]][3]);//
          pointList.point[10][i][12]=2.*pointList.X[2]; //(1./(pointList.pointList.deltaT*pointList.pointList.deltaT))*(pointList.point[11][ind[11]][3]-2.*pointList.point[10][ind[10]][3]+pointList.point[9][ind[9]][3]);//
          //z-Component
          setAllMatrixesToZero(4);
          for(int t=minIndex-10;t<maxIndex-10+1;t++){
              weight     = pointList.point[t+10][ind[t+10]][15];
              weight     = 1.-1./(1.+exp(-300.*(weight-0.015)));
              pointList.A[t+10][0] = 1.*weight;
              pointList.A[t+10][1] = (double)t*pointList.deltaT*weight;
              pointList.A[t+10][2] = pow((double)t*pointList.deltaT,2.)*weight;
              pointList.A[t+10][3] = pow((double)t*pointList.deltaT,3.)*weight;
              pointList.y[0][t+10] = pointList.point[t+10][ind[t+10]][4]*weight;
          }
          makeAT(21,4);
          makeATA(21,4);
          makeATY(21,4,0);
          solve(21,4);
          
          pointList.point[10][i][7]=pointList.X[0]; //pointList.point[10][ind[10]][4];//
          pointList.point[10][i][10]=pointList.X[1];//(1./(2.*pointList.pointList.deltaT))*(pointList.point[11][ind[11]][4]-pointList.point[9][ind[9]][4]);//
          pointList.point[10][i][13]=2.*pointList.X[2]; //(1./(pointList.pointList.deltaT*pointList.pointList.deltaT))*(pointList.point[11][ind[11]][4]-2.*pointList.point[10][ind[10]][4]+pointList.point[9][ind[9]][4]);//
          //max break!
          velocity=pow(pow(pointList.point[10][i][8],2.)+pow(pointList.point[10][i][9],2.)+pow(pointList.point[10][i][10],2.),0.5);
          if(velocity>pointList.tolMaxVel){
             pointList.point[10][i][14]=0;
          }
      }
   }
}

void setAllMatrixesToZero(int size)
{

    for(int i=0;i<300;i++){
       if(i<size){
          pointList.X[i]=0.;
          pointList.ATY[i]=0.;
          pointList.BTY[i]=0.;
          pointList.CTY[i]=0.;
       }
       pointList.Y[i]=0.;
       
       pointList.YuB[i]=0.;
       pointList.YvB[i]=0.;
       pointList.YwB[i]=0.;
      
       pointList.Yaz[i]=0.;
       pointList.Yay[i]=0.;
       pointList.Yax[i]=0.;

       for(int j=0;j<size;j++){
          pointList.A[i][j]=0.;
          pointList.AT[j][i]=0.;
          if(i<size){pointList.ATA[i][j]=0.;}
          pointList.B[i][j]=0.;
          pointList.BT[j][i]=0.;
          if(i<size){pointList.BTB[i][j]=0.;}
          pointList.C[i][j]=0.;
          pointList.CT[j][i]=0.;
          if(i<size){pointList.CTC[i][j]=0.;}
       }
    }
    
}
void makeAT(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
           pointList.AT[i][j]=pointList.A[j][i];
        }
     }
}
void makeATA(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
           pointList.ATA[i][j]=0.;
           for(int k=0;k<n;k++){
              pointList.ATA[i][j]=pointList.ATA[i][j]+pointList.AT[i][k]*pointList.A[k][j];
           }
        }
     }
}
void makeATY(int n, int m,int wh)
{
     for(int i=0;i<m;i++){
           pointList.ATY[i]=0.;
           for(int k=0;k<n;k++){
               pointList.ATY[i]=pointList.ATY[i]+pointList.AT[i][k]*pointList.y[wh][k];
           }
     }
}
bool solve(int n, int m)
{
    double faktor;
    bool ok=true;

    for(int i=1;i<m;i++){
       for(int j=i;j<m;j++){
          if(fabs(pointList.ATA[j][i-1])>0.){
             faktor=pointList.ATA[i-1][i-1]/pointList.ATA[j][i-1];
             for(int k=0;k<m;k++){
                pointList.ATA[j][k]=pointList.ATA[i-1][k]-faktor*pointList.ATA[j][k];
             }
             pointList.ATY[j]=pointList.ATY[i-1]-faktor*pointList.ATY[j];
          }
       }
    }
    for(int i=m-1;i>-1;i--){
       for(int j=i+1;j<m;j++){
          pointList.ATY[i]=pointList.ATY[i]-pointList.ATA[i][j]*pointList.X[j];
       }
       if(fabs(pointList.ATA[i][i])>0.){
          pointList.X[i]=pointList.ATY[i]/pointList.ATA[i][i];
       }
       else{
          ok=false;
       }
    }
    return ok;
}
void makeCT(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
           pointList.CT[i][j]=pointList.C[j][i];
        }
     }
}
void makeCTC(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
           pointList.CTC[i][j]=0.;
           for(int k=0;k<n;k++){
              pointList.CTC[i][j]=pointList.CTC[i][j]+pointList.CT[i][k]*pointList.C[k][j];
           }
        }
     }
}
void makeCTY(int n, int m,int wh)
{
     for(int i=0;i<m;i++){
           pointList.CTY[i]=0.;
           for(int k=0;k<n;k++){
               pointList.CTY[i]=pointList.CTY[i]+pointList.CT[i][k]*pointList.yC[wh][k];
           }
     }
}
bool solveC(int n, int m)
{
    double faktor;
    bool ok=true;

    for(int i=1;i<m;i++){
       for(int j=i;j<m;j++){
          if(fabs(pointList.CTC[j][i-1])>0.){
             faktor=pointList.CTC[i-1][i-1]/pointList.CTC[j][i-1];
             for(int k=0;k<m;k++){
                pointList.CTC[j][k]=pointList.CTC[i-1][k]-faktor*pointList.CTC[j][k];
             }
             pointList.CTY[j]=pointList.CTY[i-1]-faktor*pointList.CTY[j];
          }
       }
    }
    for(int i=m-1;i>-1;i--){
       for(int j=i+1;j<m;j++){
          pointList.CTY[i]=pointList.CTY[i]-pointList.CTC[i][j]*pointList.X[j];
       }
       if(fabs(pointList.CTC[i][i])>0.){
          pointList.X[i]=pointList.CTY[i]/pointList.CTC[i][i];
       }
       else{
          ok=false;
       }
    }
    return ok;
}
void writeXUAPFile(int t)
{

    FILE *fpp;
    char name[256];
    int c;

    c=sprintf (name, pointList.path);
    c+=sprintf (name+c, "/xuap.");
    c+=sprintf (name+c, "%1d", t);

    fpp = fopen(name,"w");
    
    for(int i=1;i<pointList.point[10][0][0];i++){
       if(pointList.point[10][i][14]>0){
           pointList.count++;
           double vel=pow( pow(pointList.point[10][i][8],2.)
                          +pow(pointList.point[10][i][9],2.)
                          +pow(pointList.point[10][i][10],2.),0.5);
           double acc=pow( pow(pointList.point[10][i][11],2.)
                          +pow(pointList.point[10][i][12],2.)
                          +pow(pointList.point[10][i][13],2.),0.5);
           pointList.meanVel=(pointList.meanVel*(double)(pointList.count-1)+vel)/(double)pointList.count;
           pointList.meanAcc=(pointList.meanAcc*(double)(pointList.count-1)+acc)/(double)pointList.count;
           if(vel>pointList.maxVel){
              pointList.maxVel=vel;
           }
        }
        for(int j=0;j<14;j++){
			if(j<5 || pointList.point[10][i][14]>0){
                fprintf(fpp, "%lf\t", pointList.point[10][i][j]);
			}
			else{
                fprintf(fpp, "%lf\t", 0.);
			}
        }
        fprintf(fpp, "%lf\n", pointList.point[10][i][14]);
    }
    fclose (fpp);
}

void followTrajPoint(FILE *fpp, int t,int startPoint)
{
     int pCounterA,pCounterB,pCounterC,numInTraj;
     int startT, startP;
     double dist,dx,dy,dz;
     double centerX,centerY,centerZ;
     double Liu[5],Liv[5],Liw[5],Liax[4],Liay[4],Liaz[4];
     double ux,uy,uz,vx,vy,vz,wx,wy,wz;
     double dix,diy,diz,absDi,Dx,Dy,Dz,lx,ly,lz,cx,cy,cz,refx,refy,refz;
     double w1,w2,w3,s11,s12,s13,s22,s23,s33,ww1,ww2,ww3,wwsij;
	 double s111,s222,s333,s112,s113,s221,s223,s331,s332,s123;
     double sijsjkski,wsq,twosijsij,R,Q,div,ref,diss,USq;
     int time;
	 double u[3];
	 double a[3];
	 double ref_diva,diva,reldiva,quality;
    
     double minDistA[300];
     int minDistAIndex[300];
     double minDistB[300];
     int minDistBIndex[300];
     double minDistC[300];
     int minDistCIndex[300];
     
     double um,up,vm,vp,wm,wp;
     bool okc,contin;

     int rank;

     int start;
     int end;
     int minCounter;
	 int counter_f;

	 double avU[3];
	 double avA[3];

	 double distance;

     bool ok;
     startT=t;

     if(t==pointList.firstFile){
        readXUAPFile(t,true);
     }
     else{
        readXUAPFile(t,false);
     }
     

     start=1;
     end=(int)(pointList.point[2][0][0]+0.5);
     

     int n;
     for(int nn=start;nn<end;nn++){
         time=2;

//if((double)pointList.count3/(double)pointList.count>0){
//    cout << "point per sphere.............."<<(double)pointList.count3/(double)pointList.count<<"\n";
//}

         if(pointList.point[2][nn][11]>0. && !(pointList.occ[t-pointList.firstFile][nn]) ){
            startP=nn;
            ok=true;
            numInTraj=0;
            pointList.noDeriv=0;
            n=nn;
            while(ok){
				pointList.occ[t+time-2-pointList.firstFile][n]=true;
               //interpolieren und rausschreiben mit t,n (Zeit und Startpunkt)
               //%Da soll jetzt duidxj linear interpoliert werden
               //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
               //%die nächsten Punkte zu Punkt x,y,z, finden
               pointList.count++;
               setAllMatrixesToZero(4);
			   contin=true;

			      if(pointList.derivatives){ 
				
                              
               centerX=pointList.point[time][n][2];
               centerY=pointList.point[time][n][3];
               centerZ=pointList.point[time][n][4];


               for(int i=0;i<300;i++){
                  minDistA[i]=1000;
                  minDistB[i]=1000;
                  minDistC[i]=1000;
				  minDistAIndex[i]=0;
				  minDistBIndex[i]=0;
				  minDistCIndex[i]=0;
               }
               
               //AAAAAAAAAAAA
               for(int i=1;i<pointList.point[time-1][0][0]+1;i++){
                  dist=pow(pow(pointList.point[time-1][i][2]-centerX,2.)+pow(pointList.point[time-1][i][3]-centerY,2.)+pow(pointList.point[time-1][i][4]-centerZ,2.),0.5);
                  if(dist<minDistA[pointList.maxRank] && pointList.point[time-1][i][11]>0.){
                     rank=pointList.maxRank;
                     for(int k=pointList.maxRank;k>-1;k--){
                        if(dist<minDistA[k]){
                           rank=k;
                        }
                     }
                     for(int l=pointList.maxRank;l>rank;l--){
                        minDistA[l]=minDistA[l-1];
                        minDistAIndex[l]=minDistAIndex[l-1];
                     }
                     minDistA[rank]=dist;
                     minDistAIndex[rank]=i;
                  }
               }
               //BBBBBBBBBBBBBBB
               for(int i=1;i<pointList.point[time][0][0]+1;i++){
                  dist=pow(pow(pointList.point[time][i][2]-centerX,2.)+pow(pointList.point[time][i][3]-centerY,2.)+pow(pointList.point[time][i][4]-centerZ,2.),0.5);
                  if(dist<minDistB[pointList.maxRank] && pointList.point[time][i][11]>0.){
                     rank=pointList.maxRank;
                     for(int k=pointList.maxRank;k>-1;k--){
                        if(dist<minDistB[k]){
                           rank=k;
                        }
                     }
                     for(int l=pointList.maxRank;l>rank;l--){
                        minDistB[l]=minDistB[l-1];
                        minDistBIndex[l]=minDistBIndex[l-1];
                     }
                     minDistB[rank]=dist;
                     minDistBIndex[rank]=i;
                  }
               }
               //CCCCCCCCCCCCCCCCCCCCCCCCCC
               for(int i=1;i<pointList.point[time+1][0][0]+1;i++){
                  dist=pow(pow(pointList.point[time+1][i][2]-centerX,2.)+pow(pointList.point[time+1][i][3]-centerY,2.)+pow(pointList.point[time+1][i][4]-centerZ,2.),0.5);
                  if(dist<minDistC[pointList.maxRank] && pointList.point[time+1][i][11]>0.){
                     rank=pointList.maxRank;
                     for(int k=pointList.maxRank;k>-1;k--){
                        if(dist<minDistC[k]){
                           rank=k;
                        }
                     }
                     for(int l=pointList.maxRank;l>rank;l--){
                        minDistC[l]=minDistC[l-1];
                        minDistCIndex[l]=minDistCIndex[l-1];
                     }
                     minDistC[rank]=dist;
                     minDistCIndex[rank]=i;
                  }
               }
               
               pCounterA=0;
               pCounterB=0;
               pCounterC=0;
               
               int i;
               
               for(int pointInd=0;(pointInd<pointList.maxRank) && (minDistA[pointInd]<pointList.maxRadius);pointInd++){
                  i=minDistAIndex[pointInd];
                  if(pointList.point[time-1][i][11]>0.){
                     dist=pow(pow(pointList.point[time-1][i][2]-centerX,2.)+pow(pointList.point[time-1][i][3]-centerY,2.)+pow(pointList.point[time-1][i][4]-centerZ,2.),0.5);
                     dx=pointList.point[time-1][i][2]-centerX;
                     dy=pointList.point[time-1][i][3]-centerY;
                     dz=pointList.point[time-1][i][4]-centerZ;
                     pointList.A[pCounterA][0]=1.;
                     pointList.A[pCounterA][1]=dx;
                     pointList.A[pCounterA][2]=dy;
                     pointList.A[pCounterA][3]=dz;
                        
                     pointList.y[1][pCounterA]=pointList.point[time-1][i][5];
                     pointList.y[2][pCounterA]=pointList.point[time-1][i][6];
                     pointList.y[3][pCounterA]=pointList.point[time-1][i][7];
                     pCounterA++;
                  }
               }
               for(int pointInd=0;(pointInd<pointList.maxRank) && (minDistB[pointInd]<pointList.maxRadius);pointInd++){
                  i=minDistBIndex[pointInd];
                  if(pointList.point[time][i][11]>0.){
                     dist=pow(pow(pointList.point[time][i][2]-centerX,2.)+pow(pointList.point[time][i][3]-centerY,2.)+pow(pointList.point[time][i][4]-centerZ,2.),0.5);
                     dx=pointList.point[time][i][2]-centerX;
                     dy=pointList.point[time][i][3]-centerY;
                     dz=pointList.point[time][i][4]-centerZ;
                     pointList.B[pCounterB][0]=1.;
                     pointList.B[pCounterB][1]=dx;
                     pointList.B[pCounterB][2]=dy;
                     pointList.B[pCounterB][3]=dz;
                     
                     pointList.YuB[pCounterB]=pointList.point[time][i][5];
                     pointList.YvB[pCounterB]=pointList.point[time][i][6];
                     pointList.YwB[pCounterB]=pointList.point[time][i][7];
					 pointList.YaxB[pCounterB]=pointList.point[time][i][8];
                     pointList.YayB[pCounterB]=pointList.point[time][i][9];
                     pointList.YazB[pCounterB]=pointList.point[time][i][10];
                     pCounterB++;
                  }
               }
               for(int pointInd=0;(pointInd<pointList.maxRank) && (minDistC[pointInd]<pointList.maxRadius);pointInd++){
                  i=minDistCIndex[pointInd];
                  if(pointList.point[time+1][i][11]>0.){
                     dist=pow(pow(pointList.point[time+1][i][2]-centerX,2.)+pow(pointList.point[time+1][i][3]-centerY,2.)+pow(pointList.point[time+1][i][4]-centerZ,2.),0.5);
                     dx=pointList.point[time+1][i][2]-centerX;
                     dy=pointList.point[time+1][i][3]-centerY;
                     dz=pointList.point[time+1][i][4]-centerZ;
                     pointList.C[pCounterC][0]=1.;
                     pointList.C[pCounterC][1]=dx;
                     pointList.C[pCounterC][2]=dy;
                     pointList.C[pCounterC][3]=dz;
                     
                     pointList.yC[1][pCounterC]=pointList.point[time+1][i][5];
                     pointList.yC[2][pCounterC]=pointList.point[time+1][i][6];
                     pointList.yC[3][pCounterC]=pointList.point[time+1][i][7];
                     pCounterC++;
                  }
               }
               //end loop through maxRank

               pointList.count3=pointList.count3+pCounterB;
               minCounter=3;
               
               if(pCounterA>minCounter && pCounterB>minCounter && pCounterC>minCounter){ // %jetzt wird endlich Punkt1 interpoliert
                  //%correct x,y,z with center of interpolation!
                                   
				  //here comes Soren stuff for du/dx AND for da/dx
                  //make avU, make avA
				  for(int i=0;i<3;i++){
                     avU[i]=0;
					 avA[i]=0;
				  }
				  for(int i=0;i<pCounterB;i++){
                     avU[0]=avU[0]+pointList.YuB[i]/(double)pCounterB;
					 avU[1]=avU[1]+pointList.YvB[i]/(double)pCounterB;
					 avU[2]=avU[2]+pointList.YwB[i]/(double)pCounterB;
				     avA[0]=avA[0]+pointList.YaxB[i]/(double)pCounterB;
					 avA[1]=avA[1]+pointList.YayB[i]/(double)pCounterB;
					 avA[2]=avA[2]+pointList.YazB[i]/(double)pCounterB;
				  }
				  //make sum
				  counter_f=0;
		          for(int rr=0;rr<3;rr++){
	                 for(int qq=0;qq<3;qq++){
		                pointList.Aij[rr][qq]=0;
						pointList.Aaij[rr][qq]=0;
	                 }
	              }
				  for(int i=0;i<pCounterB;i++){
				     u[0]=pointList.YuB[i];u[1]=pointList.YvB[i];u[2]=pointList.YwB[i];
					 a[0]=pointList.YaxB[i];a[1]=pointList.YayB[i];a[2]=pointList.YazB[i];
					 counter_f++;
					 for(int rr=0;rr<3;rr++){
			            for(int qq=0;qq<3;qq++){
				           pointList.Aij[rr][qq]=pointList.Aij[rr][qq]
						                        +pointList.B[i][qq+1]/pointList.maxRadius*(u[rr]-avU[rr])/pointList.maxRadius;
						   pointList.Aaij[rr][qq]=pointList.Aaij[rr][qq]
						                        +pointList.B[i][qq+1]/pointList.maxRadius*(a[rr]-avA[rr])/pointList.maxRadius;
			            } 
	                 }
				  }
				  for(int rr=0;rr<3;rr++){
	                 for(int qq=0;qq<3;qq++){
		                pointList.Aij[rr][qq] =pointList.Aij[rr][qq] /((double)counter_f-1)*5;
						pointList.Aaij[rr][qq]=pointList.Aaij[rr][qq]/((double)counter_f-1)*5;
	                 }
	              }
		          double trace=pointList.Aij[0][0]+pointList.Aij[1][1]+pointList.Aij[2][2];
		          pointList.Aij[0][0]=pointList.Aij[0][0]-(1./3.)*trace;
		          pointList.Aij[1][1]=pointList.Aij[1][1]-(1./3.)*trace;
		          pointList.Aij[2][2]=pointList.Aij[2][2]-(1./3.)*trace;
                  
				  Liu[0]=pointList.point[time][n][5];
                  Liu[1]=pointList.Aij[0][0];
                  Liu[2]=pointList.Aij[0][1];
                  Liu[3]=pointList.Aij[0][2];
				  Liv[0]=pointList.point[time][n][6];
                  Liv[1]=pointList.Aij[1][0];
                  Liv[2]=pointList.Aij[1][1];
                  Liv[3]=pointList.Aij[1][2];
				  Liw[0]=pointList.point[time][n][7];
                  Liw[1]=pointList.Aij[2][0];
                  Liw[2]=pointList.Aij[2][1];
                  Liw[3]=pointList.Aij[2][2];

				  Liax[0]=pointList.point[time][n][8];
				  Liax[1]=pointList.Aaij[0][0];
                  Liax[2]=pointList.Aaij[0][1];
                  Liax[3]=pointList.Aaij[0][2];
                  Liay[0]=pointList.point[time][n][9];
				  Liay[1]=pointList.Aaij[1][0];
                  Liay[2]=pointList.Aaij[1][1];
                  Liay[3]=pointList.Aaij[1][2];
                  Liaz[0]=pointList.point[time][n][10];
				  Liaz[1]=pointList.Aaij[2][0];
                  Liaz[2]=pointList.Aaij[2][1];
                  Liaz[3]=pointList.Aaij[2][2];

				  //end of Soren stuff

                  //this is for du/dt                        
                  makeAT(pCounterA,4);
                  makeATA(pCounterA,4);
                  makeATY(pCounterA,4,1);
                  okc=solve(pCounterA,4);
				  if(!okc){contin=false;}
                  um=pointList.X[0];

                  makeCT(pCounterC,4);
                  makeCTC(pCounterC,4);
                  makeCTY(pCounterC,4,1);
                  okc=solveC(pCounterC,4);
				  if(!okc){contin=false;}
                  up=pointList.X[0];
                  
				  Liu[4]=1./(pointList.deltaT)*(0.5*up-0.5*um);

				  //this is for dv/dt
                  makeAT(pCounterA,4);
                  makeATA(pCounterA,4);
                  makeATY(pCounterA,4,2);
                  okc=solve(pCounterA,4);
				  if(!okc){contin=false;}
                  vm=pointList.X[0];

                  makeCT(pCounterC,4);
                  makeCTC(pCounterC,4);
                  makeCTY(pCounterC,4,2);
                  okc=solveC(pCounterC,4);
				  if(!okc){contin=false;}
                  vp=pointList.X[0];
                  
				  Liv[4]=1./(pointList.deltaT)*(0.5*vp-0.5*vm);
                               
                  //this is for dw/dt
				  makeAT(pCounterA,4);
                  makeATA(pCounterA,4);
                  makeATY(pCounterA,4,3);
                  okc=solve(pCounterA,4);
				  if(!okc){contin=false;}
                  wm=pointList.X[0];
                                       
                  makeCT(pCounterC,4);
                  makeCTC(pCounterC,4);
                  makeCTY(pCounterC,4,3);
                  okc=solveC(pCounterC,4);
				  if(!okc){contin=false;}
                  wp=pointList.X[0];
                                          
                  Liw[4]=1./(pointList.deltaT)*(0.5*wp-0.5*wm);

				  ////////////////////

                  

                  //%omega,strain,div,ref
                  w1=Liw[2]-Liv[3];
                  w2=Liu[3]-Liw[1];
                  w3=Liv[1]-Liu[2];
                  s11=Liu[1];
                  s22=Liv[2];
                  s33=Liw[3];
                  s12=0.5*(Liu[2]+Liv[1]);
                  s13=0.5*(Liu[3]+Liw[1]);
                  s23=0.5*(Liv[3]+Liw[2]);
                  div=fabs(trace);
                  ref=fabs(s11)+fabs(s22)+fabs(s33);
                                    
                  //acceleration quality
				  Dx=Liax[0];lx=Liu[4];cx=Liu[0]*Liu[1]+Liv[0]*Liu[2]+Liw[0]*Liu[3];
				  Dy=Liay[0];ly=Liv[4];cy=Liu[0]*Liv[1]+Liv[0]*Liv[2]+Liw[0]*Liv[3];
				  Dz=Liaz[0];lz=Liw[4];cz=Liu[0]*Liw[1]+Liv[0]*Liw[2]+Liw[0]*Liw[3];

                  dix=fabs(Dx-lx-cx);
                  diy=fabs(Dy-ly-cy);
                  diz=fabs(Dz-lz-cz);
				  refx=fabs(Dx)+fabs(lx)+fabs(cx);
				  refy=fabs(Dy)+fabs(ly)+fabs(cy);
				  refz=fabs(Dz)+fabs(lz)+fabs(cz);
                  absDi=(1./3.)*(dix/refx+diy/refy+diz/refz);

				  wsq=w1*w1+w2*w2+w3*w3;
                  twosijsij=2.*(s11*s11+s22*s22+s33*s33
                           +2.*(s12*s12+s13*s13+s23*s23));

                  Q=(1./4.)*(wsq-twosijsij);
				  diva=Liax[1]+Liay[2]+Liaz[3];
				  ref_diva=fabs(wsq)+fabs(twosijsij)+fabs(Liax[1])+fabs(Liay[2])+fabs(Liaz[3]);

                  pointList.traj[numInTraj][ 0]=pointList.point[time][n][5];
                  pointList.traj[numInTraj][ 1]=pointList.point[time][n][6];
                  pointList.traj[numInTraj][ 2]=pointList.point[time][n][7];
                  pointList.traj[numInTraj][ 3]=Liu[0];
                  pointList.traj[numInTraj][ 4]=Liv[0];
                  pointList.traj[numInTraj][ 5]=Liw[0];
                  pointList.traj[numInTraj][ 6]=Liax[0];
                  pointList.traj[numInTraj][ 7]=Liay[0];
                  pointList.traj[numInTraj][ 8]=Liaz[0];
                  pointList.traj[numInTraj][ 9]=w1;
                  pointList.traj[numInTraj][10]=w2;
                  pointList.traj[numInTraj][11]=w3;
                  pointList.traj[numInTraj][12]=s11;
                  pointList.traj[numInTraj][13]=s12;
                  pointList.traj[numInTraj][14]=s13;
                  pointList.traj[numInTraj][15]=s22;
                  pointList.traj[numInTraj][16]=s23;
                  pointList.traj[numInTraj][17]=s33;
                  pointList.traj[numInTraj][18]=Liu[4];//dudt
                  pointList.traj[numInTraj][19]=Liv[4];//dvdt
                  pointList.traj[numInTraj][20]=Liw[4];//dwdt

                  pointList.traj[numInTraj][21]=Liax[1];
                  pointList.traj[numInTraj][22]=Liax[2];
				  pointList.traj[numInTraj][23]=Liax[3];
				  pointList.traj[numInTraj][24]=Liay[1];
                  pointList.traj[numInTraj][25]=Liay[2];
				  pointList.traj[numInTraj][26]=Liay[3];
				  pointList.traj[numInTraj][27]=Liaz[1];
                  pointList.traj[numInTraj][28]=Liaz[2];
				  pointList.traj[numInTraj][29]=Liaz[3];

                  if(ref_diva>0){
					  pointList.traj[numInTraj][30]=pointList.weDiv*fabs(2*Q+diva)/ref_diva+pointList.weAcc*absDi;
                  }
                  else{
                      pointList.traj[numInTraj][30]=0.95;
                  }
                  if(pointList.traj[numInTraj][30]>0.95){
                      pointList.traj[numInTraj][30]=0.95;
                  }
                  pointList.traj[numInTraj][31]=n;
                               
               }// end of if pCOunter>3 solve...
               if(!(pCounterA>minCounter && pCounterB>minCounter && pCounterC>minCounter ) || !(contin)){
                  pointList.traj[numInTraj][ 0]=pointList.point[time][n][2];
                  pointList.traj[numInTraj][ 1]=pointList.point[time][n][3];
                  pointList.traj[numInTraj][ 2]=pointList.point[time][n][4];
                  pointList.traj[numInTraj][ 3]=pointList.point[time][n][5];
                  pointList.traj[numInTraj][ 4]=pointList.point[time][n][6];
                  pointList.traj[numInTraj][ 5]=pointList.point[time][n][7];
                  pointList.traj[numInTraj][ 6]=pointList.point[time][n][8];
                  pointList.traj[numInTraj][ 7]=pointList.point[time][n][9];
                  pointList.traj[numInTraj][ 8]=pointList.point[time][n][10];
                  pointList.traj[numInTraj][ 9]=0.;
                  pointList.traj[numInTraj][10]=0.;
                  pointList.traj[numInTraj][11]=0.;
                  pointList.traj[numInTraj][12]=0.;
                  pointList.traj[numInTraj][13]=0.;
                  pointList.traj[numInTraj][14]=0.;
                  pointList.traj[numInTraj][15]=0.;
                  pointList.traj[numInTraj][16]=0.;
                  pointList.traj[numInTraj][17]=0.;
                  pointList.traj[numInTraj][18]=0.;//dudt
                  pointList.traj[numInTraj][19]=0.;//dvdt
                  pointList.traj[numInTraj][20]=0.;//dwdt

                  pointList.traj[numInTraj][21]=0;
                  pointList.traj[numInTraj][22]=0;
                  pointList.traj[numInTraj][23]=0;
                  pointList.traj[numInTraj][24]=0;
                  pointList.traj[numInTraj][25]=0;
                  pointList.traj[numInTraj][26]=0;
				  pointList.traj[numInTraj][27]=0;
                  pointList.traj[numInTraj][28]=0;
                  pointList.traj[numInTraj][29]=0;

                  pointList.traj[numInTraj][30]=1.;   //Wichtig
                  pointList.traj[numInTraj][31]=(double)n;
                  pointList.noDeriv++;

               }
			      }//end of derivatives
				  else{
                      pointList.traj[numInTraj][ 0]=pointList.point[time][n][2];
                      pointList.traj[numInTraj][ 1]=pointList.point[time][n][3];
                      pointList.traj[numInTraj][ 2]=pointList.point[time][n][4];
                      pointList.traj[numInTraj][ 3]=pointList.point[time][n][5];
                      pointList.traj[numInTraj][ 4]=pointList.point[time][n][6];
                      pointList.traj[numInTraj][ 5]=pointList.point[time][n][7];
                      pointList.traj[numInTraj][ 6]=pointList.point[time][n][8];
                      pointList.traj[numInTraj][ 7]=pointList.point[time][n][9];
                      pointList.traj[numInTraj][ 8]=pointList.point[time][n][10];
				  }

               numInTraj++;

               //schauen ob's einen nächsten gibt
               if(pointList.point[time][n][1]>0 && time<pointList.lastFile){
                   n=pointList.point[time][n][1];
                   time++;
                   if( pointList.point[time][n][11]<1. ){  
                       ok=false;
                   }
               }
               else{
                  ok=false;
               }
            }//end while ok

			     if(pointList.derivatives){
                 

            if(numInTraj-pointList.noDeriv>pointList.minTrajLength-1){   //Wichtig
               /////polynom business////////////////////////////////////////
               double su=0.;
               double x4[300],x5[300],x6[300];
               double x7[300],x8[300],x9[300],x10[300],x11[300];
               double x12[300],x13[300],x14[300];
               double x15[300],x16[300],x17[300];
			   double x18[300],x19[300],x20[300];
			   double x21[300],x22[300],x23[300],x24[300];

               double xp[300],yp[300],zp[300],up[300],vp[300],wp[300];
               double axp[300],ayp[300],azp[300];
               double w1p[300],w2p[300],w3p[300];
               double s11p[300],s12p[300],s13p[300],s22p[300],s23p[300],s33p[300];
               double utp[300],vtp[300],wtp[300];
               double daxdxp[300],daxdyp[300],daxdzp[300];
			   double daydxp[300],daydyp[300],daydzp[300];
			   double dazdxp[300],dazdyp[300],dazdzp[300];

               setAllMatrixesToZero(4);

               for(int ii=0;ii<numInTraj;ii++){
                  su=su+1-pointList.traj[ii][30];//reldiv(ii)
               }
               int order=(int)(su/pointList.polyConst+3.5);
               if(numInTraj<5){
                  order=2;
               }
               if(numInTraj<2){
                  order=1;
               }
                             
               for(int ii=0;ii<numInTraj;ii++){
                  pointList.we[ii]=1.-1./(1.+exp(-pointList.c1*(pointList.traj[ii][30]-pointList.c2)));//reldiv(ii)
               }

               for(int ii=0;ii<numInTraj;ii++){
                  for(int ij=0;ij<order;ij++){
                     pointList.A[ii][ij]=pointList.we[ii]*pow((double)ii*pointList.deltaT+0.000000001,(double)(ij));
                  }
                 

                  pointList.y[4] [ii]=pointList.we[ii]*pointList.traj[ii][ 9];//w1(i)
                  pointList.y[5] [ii]=pointList.we[ii]*pointList.traj[ii][10];//w2(i)
                  pointList.y[6] [ii]=pointList.we[ii]*pointList.traj[ii][11];//w3(i)
                  pointList.y[7] [ii]=pointList.we[ii]*pointList.traj[ii][12];//s11(i)
                  pointList.y[8] [ii]=pointList.we[ii]*pointList.traj[ii][13];//s12(i)
                  pointList.y[9] [ii]=pointList.we[ii]*pointList.traj[ii][14];//s13(i)
                  pointList.y[10][ii]=pointList.we[ii]*pointList.traj[ii][15];//s22(i)
                  pointList.y[11][ii]=pointList.we[ii]*pointList.traj[ii][16];//s23(i)
                  pointList.y[12][ii]=pointList.we[ii]*pointList.traj[ii][17];//s33(i)

                  pointList.y[13][ii]=pointList.we[ii]*pointList.traj[ii][18];//dudt(i)
                  pointList.y[14][ii]=pointList.we[ii]*pointList.traj[ii][19];//dvdt(i)
                  pointList.y[15][ii]=pointList.we[ii]*pointList.traj[ii][20];//dwdt(i)

                  pointList.y[16][ii]=pointList.we[ii]*pointList.traj[ii][21];//daxdx(i)
                  pointList.y[17][ii]=pointList.we[ii]*pointList.traj[ii][22];//daxdy(i)
                  pointList.y[18][ii]=pointList.we[ii]*pointList.traj[ii][23];//daxdz(i)
				  pointList.y[19][ii]=pointList.we[ii]*pointList.traj[ii][24];//daydx(i)
                  pointList.y[20][ii]=pointList.we[ii]*pointList.traj[ii][25];//daydy(i)
                  pointList.y[21][ii]=pointList.we[ii]*pointList.traj[ii][26];//daydz(i)
				  pointList.y[22][ii]=pointList.we[ii]*pointList.traj[ii][27];//dazdx(i)
                  pointList.y[23][ii]=pointList.we[ii]*pointList.traj[ii][28];//dazdy(i)
                  pointList.y[24][ii]=pointList.we[ii]*pointList.traj[ii][29];//dazdz(i)
               }
               
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,4);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x4[ii]=pointList.X[ii];//w1
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,5);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x5[ii]=pointList.X[ii];//w2
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,6);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x6[ii]=pointList.X[ii];//w3
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,7);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x7[ii]=pointList.X[ii];//s11
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,8);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x8[ii]=pointList.X[ii];//s12
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,9);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x9[ii]=pointList.X[ii];//s13
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,10);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x10[ii]=pointList.X[ii];//s22
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,11);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x11[ii]=pointList.X[ii];//s23
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,12);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x12[ii]=pointList.X[ii];//s33
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,13);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x13[ii]=pointList.X[ii];//dudt
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,14);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x14[ii]=pointList.X[ii];//dvdt
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,15);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x15[ii]=pointList.X[ii];//dwdt
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,16);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x16[ii]=pointList.X[ii];//daxdx
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,17);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x17[ii]=pointList.X[ii];//daxdy
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,18);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x18[ii]=pointList.X[ii];//daxdz
               }
			   makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,19);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x19[ii]=pointList.X[ii];//daydx
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,20);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x20[ii]=pointList.X[ii];//daydy
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,21);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x21[ii]=pointList.X[ii];//daydz
               }
			   makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,22);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x22[ii]=pointList.X[ii];//dazdx
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,23);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x23[ii]=pointList.X[ii];//dazdy
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,24);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x24[ii]=pointList.X[ii];//dazdz
               }

               for(int ii=0;ii<numInTraj;ii++){
                  w1p[ii]=0;
                  w2p[ii]=0;
                  w3p[ii]=0;
                  s11p[ii]=0;
                  s12p[ii]=0;
                  s13p[ii]=0;
                  s22p[ii]=0;
                  s23p[ii]=0;
                  s33p[ii]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][12]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][13]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][14]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][15]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][16]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][17]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][18]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][19]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][20]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][21]=0;//grdients?
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][22]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][23]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][24]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][25]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][26]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][27]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][28]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][29]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][30]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][31]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][32]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][33]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][34]=0;
                  utp[ii]=0;
                  vtp[ii]=0;
                  wtp[ii]=0;
                  daxdxp[ii]=0;
                  daxdyp[ii]=0;
                  daxdzp[ii]=0;
				  daydxp[ii]=0;
                  daydyp[ii]=0;
                  daydzp[ii]=0;
				  dazdxp[ii]=0;
                  dazdyp[ii]=0;
                  dazdzp[ii]=0;

                  xp[ii] =pointList.traj[ii][0];
                  yp[ii] =pointList.traj[ii][1];
                  zp[ii] =pointList.traj[ii][2];
                  up[ii] =pointList.traj[ii][3];
                  vp[ii] =pointList.traj[ii][4];
                  wp[ii] =pointList.traj[ii][5];
                  axp[ii]=pointList.traj[ii][6];
                  ayp[ii]=pointList.traj[ii][7];
                  azp[ii]=pointList.traj[ii][8];
                  for(int ij=0;ij<order;ij++){
                     w1p[ii]= w1p[ii]+ x4[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     w2p[ii]= w2p[ii]+ x5[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     w3p[ii]= w3p[ii]+ x6[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     s11p[ii]=s11p[ii]+ x7[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     s12p[ii]=s12p[ii]+ x8[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     s13p[ii]=s13p[ii]+ x9[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     s22p[ii]=s22p[ii]+ x10[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     s23p[ii]=s23p[ii]+x11[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     s33p[ii]=s33p[ii]+x12[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     utp[ii]=utp[ii]+x13[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     vtp[ii]=vtp[ii]+x14[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     wtp[ii]=wtp[ii]+x15[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     daxdxp[ii]=daxdxp[ii]+x16[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     daxdyp[ii]=daxdyp[ii]+x17[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
					 daxdzp[ii]=daxdzp[ii]+x18[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
					 daydxp[ii]=daydxp[ii]+x19[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     daydyp[ii]=daydyp[ii]+x20[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
					 daydzp[ii]=daydzp[ii]+x21[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
					 dazdxp[ii]=dazdxp[ii]+x22[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     dazdyp[ii]=dazdyp[ii]+x23[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
					 dazdzp[ii]=dazdzp[ii]+x24[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                  }
                  
                  
                  
               }// end for loop through traj

			   for(int ii=0;ii<numInTraj;ii++){                   
					 USq=up[ii]*up[ii]+vp[ii]*vp[ii]+wp[ii]*wp[ii];

                     ww1=w1p[ii]*s11p[ii]+w2p[ii]*s12p[ii]+w3p[ii]*s13p[ii];
                     ww2=w1p[ii]*s12p[ii]+w2p[ii]*s22p[ii]+w3p[ii]*s23p[ii];
                     ww3=w1p[ii]*s13p[ii]+w2p[ii]*s23p[ii]+w3p[ii]*s33p[ii];
                     wwsij=w1p[ii]*ww1+w2p[ii]*ww2+w3p[ii]*ww3;

                     s111=s11p[ii]*s11p[ii]*s11p[ii];
                     s222=s22p[ii]*s22p[ii]*s22p[ii];
                     s333=s33p[ii]*s33p[ii]*s33p[ii];
                     s112=s11p[ii]*s12p[ii]*s12p[ii];
                     s113=s11p[ii]*s13p[ii]*s13p[ii];
                     s221=s22p[ii]*s12p[ii]*s12p[ii];
                     s223=s22p[ii]*s23p[ii]*s23p[ii];
                     s331=s33p[ii]*s13p[ii]*s13p[ii];
                     s332=s33p[ii]*s23p[ii]*s23p[ii];
                     s123=s12p[ii]*s23p[ii]*s13p[ii];
                     sijsjkski=s111+s222+s333+3.*(s112+s113+s221+s223+s331+s332)+6.*s123;

                     wsq=w1p[ii]*w1p[ii]+w2p[ii]*w2p[ii]+w3p[ii]*w3p[ii];
                     twosijsij=2.*(s11p[ii]*s11p[ii]+s22p[ii]*s22p[ii]+s33p[ii]*s33p[ii]
                                 +2.*(s12p[ii]*s12p[ii]+s13p[ii]*s13p[ii]+s23p[ii]*s23p[ii]));
			         diss=pointList.viscosity*twosijsij;

                     //rel diva quality
					 Q=(1./4.)*(wsq-twosijsij);
                     R=-(1./3.)*(sijsjkski+(3./4.)*wwsij);
					 diva=daxdxp[ii]+daydyp[ii]+dazdzp[ii];
				     ref_diva=fabs(wsq)+fabs(twosijsij)+fabs(daxdxp[ii])+fabs(daydyp[ii])+fabs(dazdzp[ii]);
                     if(ref_diva>0){
                         reldiva=fabs(2*Q+diva)/ref_diva; 
					 }
					 else{
                         reldiva=1.; 
					 }

                     //acceleration quality
					 ux=s11p[ii];
                     uy=s12p[ii]-0.5*w3p[ii];
                     uz=s13p[ii]+0.5*w2p[ii];
                     vx=s12p[ii]+0.5*w3p[ii];
                     vy=s22p[ii];
                     vz=s23p[ii]-0.5*w1p[ii];
                     wx=s13p[ii]-0.5*w2p[ii];
                     wy=s23p[ii]+0.5*w1p[ii];
                     wz=s33p[ii];
				     Dx=axp[ii];lx=utp[ii];cx=up[ii]*ux+vp[ii]*uy+wp[ii]*uz;
				     Dy=ayp[ii];ly=vtp[ii];cy=up[ii]*vx+vp[ii]*vy+wp[ii]*vz;
				     Dz=azp[ii];lz=wtp[ii];cz=up[ii]*wx+vp[ii]*wy+wp[ii]*wz;

                     dix=fabs(Dx-lx-cx);
                     diy=fabs(Dy-ly-cy);
                     diz=fabs(Dz-lz-cz);
				     refx=fabs(Dx)+fabs(lx)+fabs(cx);
				     refy=fabs(Dy)+fabs(ly)+fabs(cy);
				     refz=fabs(Dz)+fabs(lz)+fabs(cz);
					 if(refx>0 && refy>0 && refz>0){
                        absDi=(1./3.)*(dix/refx+diy/refy+diz/refz);
			         }
			         else{
                        absDi=1.;
			         }
                        
					 //totQuality
					 quality=pointList.weDiv*reldiva+pointList.weAcc*absDi;
					    
					    //prepare for xuag files
                        pointList.point[ii+2][(int)pointList.traj[ii][31]][12]=ux;
						pointList.point[ii+2][(int)pointList.traj[ii][31]][13]=uy;
						pointList.point[ii+2][(int)pointList.traj[ii][31]][14]=uz;
						pointList.point[ii+2][(int)pointList.traj[ii][31]][15]=vx;
						pointList.point[ii+2][(int)pointList.traj[ii][31]][16]=vy;
						pointList.point[ii+2][(int)pointList.traj[ii][31]][17]=vz;
						pointList.point[ii+2][(int)pointList.traj[ii][31]][18]=wx;
						pointList.point[ii+2][(int)pointList.traj[ii][31]][19]=wy;
						pointList.point[ii+2][(int)pointList.traj[ii][31]][20]=wz;
						pointList.point[ii+2][(int)pointList.traj[ii][31]][21]=1;
						pointList.point[ii+2][(int)pointList.traj[ii][31]][22]=utp[ii];
						pointList.point[ii+2][(int)pointList.traj[ii][31]][23]=vtp[ii];
						pointList.point[ii+2][(int)pointList.traj[ii][31]][24]=wtp[ii];
						pointList.point[ii+2][(int)pointList.traj[ii][31]][25]=daxdxp[ii];
						pointList.point[ii+2][(int)pointList.traj[ii][31]][26]=daxdyp[ii];
						pointList.point[ii+2][(int)pointList.traj[ii][31]][27]=daxdxp[ii];
						pointList.point[ii+2][(int)pointList.traj[ii][31]][28]=daydxp[ii];
						pointList.point[ii+2][(int)pointList.traj[ii][31]][29]=daydyp[ii];
						pointList.point[ii+2][(int)pointList.traj[ii][31]][30]=daydzp[ii];
						pointList.point[ii+2][(int)pointList.traj[ii][31]][31]=dazdxp[ii];
						pointList.point[ii+2][(int)pointList.traj[ii][31]][32]=dazdyp[ii];
						pointList.point[ii+2][(int)pointList.traj[ii][31]][33]=dazdzp[ii];
						pointList.point[ii+2][(int)pointList.traj[ii][31]][34]=quality;
					    //end of prepare xuag files
                     
					 if(pointList.weDiv*fabs(2*Q+diva)/ref_diva+pointList.weAcc*absDi){
                        pointList.count2++;
                        pointList.meanDiss=(pointList.meanDiss*(double)(pointList.count2-1)+diss)/(double)pointList.count2;
                        pointList.meanUSq=(pointList.meanUSq*(double)(pointList.count2-1)+USq)/(double)pointList.count2;                        
					 }
					 if(reldiva<0.1){
                        pointList.count4++;
					 }
					 pointList.count6++;
					 if(absDi<0.2){
                        pointList.count5++;
					 }
					 
                     fprintf(fpp, "%lf\t", xp[ii]);//1
                     fprintf(fpp, "%lf\t", yp[ii]);//2
                     fprintf(fpp, "%lf\t", zp[ii]);//3
                     fprintf(fpp, "%lf\t", up[ii]);//4
                     fprintf(fpp, "%lf\t", vp[ii]);//5
                     fprintf(fpp, "%lf\t", wp[ii]);//6
                     fprintf(fpp, "%lf\t", axp[ii]);//7
                     fprintf(fpp, "%lf\t", ayp[ii]);//8
                     fprintf(fpp, "%lf\t", azp[ii]);//9
                     fprintf(fpp, "%lf\t", w1p[ii]);//10
                     fprintf(fpp, "%lf\t", w2p[ii]);//11
                     fprintf(fpp, "%lf\t", w3p[ii]);//12
                     fprintf(fpp, "%lf\t", s11p[ii]);//13
                     fprintf(fpp, "%lf\t", s12p[ii]);//14
                     fprintf(fpp, "%lf\t", s13p[ii]);//15
                     fprintf(fpp, "%lf\t", s22p[ii]);//16
                     fprintf(fpp, "%lf\t", s23p[ii]);//17
                     fprintf(fpp, "%lf\t", s33p[ii]);//18
					 fprintf(fpp, "%lf\t", utp[ii]);//19
                     fprintf(fpp, "%lf\t", vtp[ii]);//20
                     fprintf(fpp, "%lf\t", wtp[ii]);//21
                     fprintf(fpp, "%lf\t", daxdxp[ii]);//22
                     fprintf(fpp, "%lf\t", daxdyp[ii]);//23
                     fprintf(fpp, "%lf\t", daxdzp[ii]);//24
					 fprintf(fpp, "%lf\t", daydxp[ii]);//25
                     fprintf(fpp, "%lf\t", daydyp[ii]);//26
                     fprintf(fpp, "%lf\t", daydzp[ii]);//27
					 fprintf(fpp, "%lf\t", dazdxp[ii]);//28
                     fprintf(fpp, "%lf\t", dazdyp[ii]);//29
                     fprintf(fpp, "%lf\t", dazdzp[ii]);//30
                     fprintf(fpp, "%lf\t", quality);//31 0=good, 1=bad
                     fprintf(fpp, "%lf\n", (double)(ii));//32 age along trajectory
                     
                  }// end for
                  ////end of polynom business
               
            } //end if of polynom buisness
			    }//end of derivatives
				else{
                  double xp[300],yp[300],zp[300],up[300],vp[300],wp[300];
                  double axp[300],ayp[300],azp[300];
                  for(int ii=0;ii<numInTraj;ii++){ 
					  xp[ii] =pointList.traj[ii][0];
                      yp[ii] =pointList.traj[ii][1];
                      zp[ii] =pointList.traj[ii][2];
                      up[ii] =pointList.traj[ii][3];
                      vp[ii] =pointList.traj[ii][4];
                      wp[ii] =pointList.traj[ii][5];
                      axp[ii]=pointList.traj[ii][6];
                      ayp[ii]=pointList.traj[ii][7];
                      azp[ii]=pointList.traj[ii][8];
                     fprintf(fpp, "%lf\t", xp[ii]);//1
                     fprintf(fpp, "%lf\t", yp[ii]);//2
                     fprintf(fpp, "%lf\t", zp[ii]);//3
					 distance=0.;
					 if(ii>9){
                         distance=pow(pow(xp[ii]-xp[ii-1],2.)+pow(yp[ii]-yp[ii-1],2.)+pow(zp[ii]-zp[ii-1],2.),0.5);
						 if(distance>0.002){
							 distance=distance;
						 }
					 }
                     fprintf(fpp, "%lf\t", up[ii]);//4
                     fprintf(fpp, "%lf\t", vp[ii]);//5
                     fprintf(fpp, "%lf\t", wp[ii]);//6
                     fprintf(fpp, "%lf\t", axp[ii]);//7
                     fprintf(fpp, "%lf\t", ayp[ii]);//8
                     fprintf(fpp, "%lf\t", azp[ii]);//9
					 fprintf(fpp, "%lf\n", (double)(ii));//32 age along trajectory
				  }
				}
            
         } // end if not occ und central
         
     }// end haupt for schlaufe
     

}

void followTrajPoint_pressure(FILE *fpp, int t,int startPoint)
{
     int pCounterA,pCounterB,pCounterC,numInTraj;
     int startT, startP;
     double dist,dx,dy,dz;
     double centerX,centerY,centerZ;
     double Liu[5],Liv[5],Liw[5],Liax[4],Liay[4],Liaz[4];
     double ux,uy,uz,vx,vy,vz,wx,wy,wz;
     double dix,diy,diz,absDi,Dx,Dy,Dz,lx,ly,lz,cx,cy,cz,refx,refy,refz;
     double w1,w2,w3,s11,s12,s13,s22,s23,s33,ww1,ww2,ww3,wwsij;
	 double s111,s222,s333,s112,s113,s221,s223,s331,s332,s123;
     double sijsjkski,wsq,twosijsij,R,Q,div,ref,diss,USq;
     int time;
	 double u[3];
	 double a[3];
	 double ref_diva,diva,reldiva,quality;
    
     double minDistA[300];
     int minDistAIndex[300];
     double minDistB[300];
     int minDistBIndex[300];
     double minDistC[300];
     int minDistCIndex[300];
     
     double um,up,vm,vp,wm,wp;
     bool okc,contin;

     int rank;

     int start;
     int end;
     int minCounter;
	 int counter_f;

	 double avU[9];

     bool ok;
     startT=t;

     if(t==pointList.firstFile){
        readXUAGFile(t,true);
     }
     else{
        readXUAGFile(t,false);
     }
     

     start=1;
     end=(int)(pointList.point[2][0][0]+0.5);
     

     int n;
     for(int nn=start;nn<end;nn++){
         time=2;
         if(pointList.point[2][nn][11]>0. && pointList.point[2][nn][21]>0. && !(pointList.occ[t-pointList.firstFile][nn]) ){
            startP=nn;
            ok=true;
            numInTraj=0;
            pointList.noDeriv=0;
            n=nn;
            while(ok){
				pointList.occ[t+time-2-pointList.firstFile][n]=true;
               //interpolieren und rausschreiben mit t,n (Zeit und Startpunkt)
               //%Da soll jetzt Nabla^2 u linear interpoliert werden
               //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
               //%die nächsten Punkte zu Punkt x,y,z, finden
               pointList.count++;
               setAllMatrixesToZero(4);
			   contin=true;
                              
               centerX=pointList.point[time][n][2];
               centerY=pointList.point[time][n][3];
               centerZ=pointList.point[time][n][4];


               for(int i=0;i<300;i++){
                  minDistB[i]=1000;
				  minDistBIndex[i]=0;
               }
			
               //BBBBBBBBBBBBBBB
               for(int i=1;i<pointList.point[time][0][0]+1;i++){
                  dist=pow(pow(pointList.point[time][i][2]-centerX,2.)+pow(pointList.point[time][i][3]-centerY,2.)+pow(pointList.point[time][i][4]-centerZ,2.),0.5);
                  if(dist<minDistB[pointList.maxRank] && pointList.point[time][i][11]>0.){
                     rank=pointList.maxRank;
                     for(int k=pointList.maxRank;k>-1;k--){
                        if(dist<minDistB[k]){
                           rank=k;
                        }
                     }
                     for(int l=pointList.maxRank;l>rank;l--){
                        minDistB[l]=minDistB[l-1];
                        minDistBIndex[l]=minDistBIndex[l-1];
                     }
                     minDistB[rank]=dist;
                     minDistBIndex[rank]=i;
                  }
               }
               
               pCounterB=0;
               
               int i;
               
               for(int pointInd=0;(pointInd<pointList.maxRank) && (minDistB[pointInd]<pointList.maxRadius);pointInd++){
                  i=minDistBIndex[pointInd];
                  if(pointList.point[time][i][11]>0.){
                     dist=pow(pow(pointList.point[time][i][2]-centerX,2.)+pow(pointList.point[time][i][3]-centerY,2.)+pow(pointList.point[time][i][4]-centerZ,2.),0.5);
                     dx=pointList.point[time][i][2]-centerX;
                     dy=pointList.point[time][i][3]-centerY;
                     dz=pointList.point[time][i][4]-centerZ;
                     pointList.B[pCounterB][0]=1.;
                     pointList.B[pCounterB][1]=dx;
                     pointList.B[pCounterB][2]=dy;
                     pointList.B[pCounterB][3]=dz;
                     
					 pointList.YuxB[pCounterB]=pointList.point[time][i][12];
                     pointList.YuyB[pCounterB]=pointList.point[time][i][13];
                     pointList.YuzB[pCounterB]=pointList.point[time][i][14];
					 pointList.YvxB[pCounterB]=pointList.point[time][i][15];
                     pointList.YvyB[pCounterB]=pointList.point[time][i][16];
                     pointList.YvzB[pCounterB]=pointList.point[time][i][17];
					 pointList.YwxB[pCounterB]=pointList.point[time][i][18];
                     pointList.YwyB[pCounterB]=pointList.point[time][i][19];
                     pointList.YwzB[pCounterB]=pointList.point[time][i][20];
                     pCounterB++;
                  }
               }
               //end loop through maxRank

               pointList.count3=pointList.count3+pCounterB;
               minCounter=3;
               
               if(pCounterB>minCounter ){ // %jetzt wird endlich Punkt1 interpoliert
                  //%correct x,y,z with center of interpolation!
                                   
				  //here comes Soren stuff for dux/dx,duy/dy,duz/dz,dvx/dx,dvy/dy,dvz/dz,dwx/dx,dwy/dy,dwz/dz 
                  //make avU, make avA
				  for(int i=0;i<9;i++){
                     avU[i]=0;
				  }
				  for(int i=0;i<pCounterB;i++){
                     avU[0]=avU[0]+pointList.YuxB[i]/(double)pCounterB;
					 avU[1]=avU[1]+pointList.YuyB[i]/(double)pCounterB;
					 avU[2]=avU[2]+pointList.YuzB[i]/(double)pCounterB;
					 avU[3]=avU[3]+pointList.YvxB[i]/(double)pCounterB;
					 avU[4]=avU[4]+pointList.YvyB[i]/(double)pCounterB;
					 avU[5]=avU[5]+pointList.YvzB[i]/(double)pCounterB;
					 avU[6]=avU[6]+pointList.YwxB[i]/(double)pCounterB;
					 avU[7]=avU[7]+pointList.YwyB[i]/(double)pCounterB;
					 avU[8]=avU[8]+pointList.YwzB[i]/(double)pCounterB;
				  }
				  //make sum
				  counter_f=0;
		          for(int rr=0;rr<9;rr++){
		             pointList.uxx[rr]=0;
	              }
				  for(int i=0;i<pCounterB;i++){
					 counter_f++;
				     pointList.uxx[0]=pointList.uxx[0]
						      +pointList.B[i][1]/pointList.maxRadius*(pointList.YuxB[i]-avU[0])/pointList.maxRadius;
				     pointList.uxx[1]=pointList.uxx[1]
						      +pointList.B[i][2]/pointList.maxRadius*(pointList.YuyB[i]-avU[1])/pointList.maxRadius;
				     pointList.uxx[2]=pointList.uxx[2]
						      +pointList.B[i][3]/pointList.maxRadius*(pointList.YuzB[i]-avU[2])/pointList.maxRadius;

				     pointList.uxx[3]=pointList.uxx[3]
						      +pointList.B[i][1]/pointList.maxRadius*(pointList.YvxB[i]-avU[3])/pointList.maxRadius;
				     pointList.uxx[4]=pointList.uxx[4]
						      +pointList.B[i][2]/pointList.maxRadius*(pointList.YvyB[i]-avU[4])/pointList.maxRadius;
				     pointList.uxx[5]=pointList.uxx[5]
						      +pointList.B[i][3]/pointList.maxRadius*(pointList.YvzB[i]-avU[5])/pointList.maxRadius;

				     pointList.uxx[6]=pointList.uxx[6]
						      +pointList.B[i][1]/pointList.maxRadius*(pointList.YwxB[i]-avU[6])/pointList.maxRadius;
				     pointList.uxx[7]=pointList.uxx[7]
						      +pointList.B[i][2]/pointList.maxRadius*(pointList.YwyB[i]-avU[7])/pointList.maxRadius;
				     pointList.uxx[8]=pointList.uxx[8]
						      +pointList.B[i][3]/pointList.maxRadius*(pointList.YwzB[i]-avU[8])/pointList.maxRadius;
				  }
                  for(int i=0;i<9;i++){
				      pointList.uxx[i]=pointList.uxx[i] /((double)counter_f-1)*5;
				  }
                 
                  Liu[0]= -pointList.point[time][i][8]
				          +pointList.viscosity
					     *(pointList.uxx[0]+pointList.uxx[1]+pointList.uxx[2]);
                  Liu[1]= -pointList.point[time][i][9]
				          +pointList.viscosity
					     *(pointList.uxx[3]+pointList.uxx[4]+pointList.uxx[5]);
				  Liu[2]= -pointList.point[time][i][10]
				          +pointList.viscosity
					     *(pointList.uxx[6]+pointList.uxx[7]+pointList.uxx[8]);

				  //end of Soren stuff

                  
                 

                  pointList.traj[numInTraj][ 0]=Liu[0];
                  pointList.traj[numInTraj][ 1]=Liu[1];
                  pointList.traj[numInTraj][ 2]=Liu[2];
                  pointList.traj[numInTraj][31]=n;

                               
               }// end of if pCOunter>3 solve...
               if(!(pCounterB>minCounter) || !(contin)){
                  pointList.traj[numInTraj][ 0]=0;
                  pointList.traj[numInTraj][ 1]=0;
                  pointList.traj[numInTraj][ 2]=0;
                  pointList.traj[numInTraj][31]=n;
                  pointList.noDeriv++;

               }
		   
               numInTraj++;

               //schauen ob's einen nächsten gibt
               if(pointList.point[time][n][1]>0 && time<pointList.lastFile){
                   n=pointList.point[time][n][1];
                   time++;
                   if( pointList.point[time][n][11]<1. ){  
                       ok=false;
                   }
               }
               else{
                  ok=false;
               }
            }//end while ok

			

            if(numInTraj-pointList.noDeriv>pointList.minTrajLength-1){   //Wichtig
               /////polynom business////////////////////////////////////////
               double x4[300],x5[300],x6[300];
               double pxp[300],pyp[300],pzp[300];
            
               setAllMatrixesToZero(4);
               
               int order=(int)(numInTraj/pointList.polyConst+3.5);
               if(numInTraj<5){
                  order=2;
               }
               if(numInTraj<2){
                  order=1;
               }
                             
               for(int ii=0;ii<numInTraj;ii++){
                  for(int ij=0;ij<order;ij++){
                     pointList.A[ii][ij]=pow((double)ii*pointList.deltaT+0.000000001,(double)(ij));
                  }
                 

                  pointList.y[4] [ii]=pointList.traj[ii][0];//px(i)
                  pointList.y[5] [ii]=pointList.traj[ii][1];//py(i)
                  pointList.y[6] [ii]=pointList.traj[ii][2];//pz(i)
               }
               
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,4);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x4[ii]=pointList.X[ii];//px
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,5);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x5[ii]=pointList.X[ii];//py
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,6);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x6[ii]=pointList.X[ii];//pz
               }               

               for(int ii=0;ii<numInTraj;ii++){
                  pxp[ii]=0;
                  pyp[ii]=0;
				  pzp[ii]=0;

				  pointList.point[ii+2][(int)pointList.traj[ii][31]][34]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][35]=0;
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][36]=0;
				  
                  for(int ij=0;ij<order;ij++){
                     pxp[ii]= pxp[ii]+ x4[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     pyp[ii]= pyp[ii]+ x5[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     pzp[ii]= pzp[ii]+ x6[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));                     
                  }
                  
                  
                  
               }// end for loop through traj

			   for(int ii=0;ii<numInTraj;ii++){                   
					 
                  //prepare for xuagp files
                  pointList.point[ii+2][(int)pointList.traj[ii][31]][34]=pxp[ii];
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][35]=pyp[ii];
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][36]=pzp[ii];
				  //end of prepare xuag files
				
				  for(int iii=2;iii<38;iii++){
                     fprintf(fpp, "%lf\t", pointList.point[ii+2][(int)pointList.traj[ii][31]][iii]);
			      }
			      fprintf(fpp, "%lf\n", (double)(ii));
                     
               }// end for
               ////end of polynom business
               
            } //end if of polynom buisness
            
         } // end if not occ und central
         
     }// end haupt for schlaufe
     

}

void followTrajPoint_Hessian(FILE *fpp, int t,int startPoint)
{
     int pCounterA,pCounterB,pCounterC,numInTraj;
     int startT, startP;
     double dist,dx,dy,dz;
     double centerX,centerY,centerZ;
     double Liu[5],Liv[5],Liw[5];
     double ux,uy,uz,vx,vy,vz,wx,wy,wz;
     double dix,diy,diz,absDi,Dx,Dy,Dz,lx,ly,lz,cx,cy,cz,refx,refy,refz;
     double w1,w2,w3,s11,s12,s13,s22,s23,s33,ww1,ww2,ww3,wwsij;
	 double s111,s222,s333,s112,s113,s221,s223,s331,s332,s123;
     double sijsjkski,wsq,twosijsij,R,Q,div,ref,diss,USq;
     int time;
	 double u[3];
	 double a[3];
	 double ref_diva,diva,reldiva,quality;
    
     double minDistB[300];
     int minDistBIndex[300];
     
     double um,up,vm,vp,wm,wp;
     bool okc,contin;

     int rank;

     int start;
     int end;
     int minCounter;
	 int counter_f;

	 double avU[9];

     bool ok;
     startT=t;

     if(t==pointList.firstFile){
        readXUAGPFile(t,true);
     }
     else{
        readXUAGPFile(t,false);
     }
     

     start=1;
     end=(int)(pointList.point[2][0][0]+0.5);
     

     int n;
     for(int nn=start;nn<end;nn++){
         time=2;
         if(pointList.point[2][nn][11]>0. && pointList.point[2][nn][21]>0. && !(pointList.occ[t-pointList.firstFile][nn]) ){
            startP=nn;
            ok=true;
            numInTraj=0;
            pointList.noDeriv=0;
            n=nn;
            while(ok){
				pointList.occ[t+time-2-pointList.firstFile][n]=true;
               //interpolieren und rausschreiben mit t,n (Zeit und Startpunkt)
               //%Da soll jetzt Nabla^2 u linear interpoliert werden
               //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
               //%die nächsten Punkte zu Punkt x,y,z, finden
               pointList.count++;
               setAllMatrixesToZero(4);
			   contin=true;
                              
               centerX=pointList.point[time][n][2];
               centerY=pointList.point[time][n][3];
               centerZ=pointList.point[time][n][4];


               for(int i=0;i<300;i++){
                  minDistB[i]=1000;
				  minDistBIndex[i]=0;
               }
               
               //BBBBBBBBBBBBBBB
               for(int i=1;i<pointList.point[time][0][0]+1;i++){
                  dist=pow(pow(pointList.point[time][i][2]-centerX,2.)+pow(pointList.point[time][i][3]-centerY,2.)+pow(pointList.point[time][i][4]-centerZ,2.),0.5);
                  if(dist<minDistB[pointList.maxRank] && pointList.point[time][i][11]>0.){
                     rank=pointList.maxRank;
                     for(int k=pointList.maxRank;k>-1;k--){
                        if(dist<minDistB[k]){
                           rank=k;
                        }
                     }
                     for(int l=pointList.maxRank;l>rank;l--){
                        minDistB[l]=minDistB[l-1];
                        minDistBIndex[l]=minDistBIndex[l-1];
                     }
                     minDistB[rank]=dist;
                     minDistBIndex[rank]=i;
                  }
               }
               
               pCounterB=0;
               
               int i;
               
               for(int pointInd=0;(pointInd<pointList.maxRank) && (minDistB[pointInd]<pointList.maxRadius);pointInd++){
                  i=minDistBIndex[pointInd];
                  if(pointList.point[time][i][11]>0.){
                     dist=pow(pow(pointList.point[time][i][2]-centerX,2.)+pow(pointList.point[time][i][3]-centerY,2.)+pow(pointList.point[time][i][4]-centerZ,2.),0.5);
                     dx=pointList.point[time][i][2]-centerX;
                     dy=pointList.point[time][i][3]-centerY;
                     dz=pointList.point[time][i][4]-centerZ;
                     pointList.B[pCounterB][0]=1.;
                     pointList.B[pCounterB][1]=dx;
                     pointList.B[pCounterB][2]=dy;
                     pointList.B[pCounterB][3]=dz;
                     
					 pointList.YpxB[pCounterB]=pointList.point[time][i][34];
                     pointList.YpyB[pCounterB]=pointList.point[time][i][35];
                     pointList.YpzB[pCounterB]=pointList.point[time][i][36];

                     pCounterB++;
                  }
               }
               //end loop through maxRank

               pointList.count3=pointList.count3+pCounterB;
               minCounter=3;
               
               if(pCounterB>minCounter ){ // %jetzt wird endlich Punkt1 interpoliert
                  //%correct x,y,z with center of interpolation!
                                   
				  //here comes Soren stuff for dux/dx,duy/dy,duz/dz,dvx/dx,dvy/dy,dvz/dz,dwx/dx,dwy/dy,dwz/dz 
                  //make avU, make avA
				  for(int i=0;i<3;i++){
                     avU[i]=0;
				  }
				  for(int i=0;i<pCounterB;i++){
                     avU[0]=avU[0]+pointList.YpxB[i]/(double)pCounterB;
					 avU[1]=avU[1]+pointList.YpyB[i]/(double)pCounterB;
					 avU[2]=avU[2]+pointList.YpzB[i]/(double)pCounterB;
				  }
				  //make sum
				  counter_f=0;
		          for(int rr=0;rr<3;rr++){
	                 for(int qq=0;qq<3;qq++){
		                pointList.pij[rr][qq]=0;
	                 }
	              }
				  for(int i=0;i<pCounterB;i++){
				     u[0]=pointList.YpxB[i];u[1]=pointList.YpyB[i];u[2]=pointList.YpzB[i];
					 counter_f++;
					 for(int rr=0;rr<3;rr++){
			            for(int qq=0;qq<3;qq++){
				           pointList.pij[rr][qq]=pointList.pij[rr][qq]
						                        +pointList.B[i][qq+1]/pointList.maxRadius*(u[rr]-avU[rr])/pointList.maxRadius;
			            } 
	                 }
				  }
				  for(int rr=0;rr<3;rr++){
	                 for(int qq=0;qq<3;qq++){
		                pointList.pij[rr][qq] =pointList.pij[rr][qq] /((double)counter_f-1)*5;
	                 }
	              }
                 
                  Liu[0]=pointList.pij[0][0];
                  Liu[1]=pointList.pij[0][1];
				  Liu[2]=pointList.pij[0][2];
				  Liv[0]=pointList.pij[1][0];
                  Liv[1]=pointList.pij[1][1];
				  Liv[2]=pointList.pij[1][2];
				  Liw[0]=pointList.pij[2][0];
                  Liw[1]=pointList.pij[2][1];
				  Liw[2]=pointList.pij[2][2];

				  //end of Soren stuff

                  div=(1/3.)*(fabs(Liu[1]-Liv[0]) + fabs(Liu[2]-Liw[0]) + fabs(Liv[2]-Liw[1]));
				  ref=(1/3.)*(fabs(Liu[1])+fabs(Liv[0]) + fabs(Liu[2])+fabs(Liw[0]) + fabs(Liv[2])+fabs(Liw[1]));
				  if(ref>0){
				      quality=div/ref;
				  }
				  else{
					  quality=1;
				  }
                 

                  pointList.traj[numInTraj][ 0]=Liu[0];
                  pointList.traj[numInTraj][ 1]=Liu[1];
                  pointList.traj[numInTraj][ 2]=Liu[2];
				  pointList.traj[numInTraj][ 3]=Liv[0];
                  pointList.traj[numInTraj][ 4]=Liv[1];
                  pointList.traj[numInTraj][ 5]=Liv[2];
				  pointList.traj[numInTraj][ 6]=Liw[0];
                  pointList.traj[numInTraj][ 7]=Liw[1];
                  pointList.traj[numInTraj][ 8]=Liw[2];
				  pointList.traj[numInTraj][30]=quality;
                  pointList.traj[numInTraj][31]=n;

                               
               }// end of if pCOunter>3 solve...
               if(!(pCounterB>minCounter) || !(contin)){
                  pointList.traj[numInTraj][ 0]=0;
                  pointList.traj[numInTraj][ 1]=0;
                  pointList.traj[numInTraj][ 2]=0;
				  pointList.traj[numInTraj][ 3]=0;
                  pointList.traj[numInTraj][ 4]=0;
                  pointList.traj[numInTraj][ 5]=0;
				  pointList.traj[numInTraj][ 6]=0;
                  pointList.traj[numInTraj][ 7]=0;
                  pointList.traj[numInTraj][ 8]=0;
				  pointList.traj[numInTraj][30]=0;
                  pointList.traj[numInTraj][31]=n;
                  pointList.noDeriv++;

               }
               numInTraj++;

               //schauen ob's einen nächsten gibt
               if(pointList.point[time][n][1]>0 && time<pointList.lastFile){
                   n=pointList.point[time][n][1];
                   time++;
                   if( pointList.point[time][n][11]<1. ){  
                       ok=false;
                   }
               }
               else{
                  ok=false;
               }
            }//end while ok

			

            if(numInTraj-pointList.noDeriv>pointList.minTrajLength-1){   //Wichtig
               /////polynom business////////////////////////////////////////
               double x4 [300],x5 [300],x6 [300];
			   double x7 [300],x8 [300],x9 [300];
			   double x10[300],x11[300],x12[300];
               double pxxp[300],pxyp[300],pxzp[300];
			   double pyxp[300],pyyp[300],pyzp[300];
			   double pzxp[300],pzyp[300],pzzp[300];
            
               setAllMatrixesToZero(4);
               
			   double su=0.;
               for(int ii=0;ii<numInTraj;ii++){
                  su=su+1-pointList.traj[ii][30];//quality(ii)
               }
               int order=(int)(su/pointList.polyConst+3.5);
               if(numInTraj<5){
                  order=2;
               }
               if(numInTraj<2){
                  order=1;
               }
                             
               for(int ii=0;ii<numInTraj;ii++){
                  pointList.we[ii]=1.-1./(1.+exp(-pointList.c1*(pointList.traj[ii][30]-pointList.c2)));//quality(ii)
               }

               for(int ii=0;ii<numInTraj;ii++){
                  for(int ij=0;ij<order;ij++){
                     pointList.A[ii][ij]=pointList.we[ii]*pow((double)ii*pointList.deltaT+0.000000001,(double)(ij));
                  }
                 
                  pointList.y[4] [ii]=pointList.we[ii]*pointList.traj[ii][0];//w1(i)
                  pointList.y[5] [ii]=pointList.we[ii]*pointList.traj[ii][1];//w2(i)
                  pointList.y[6] [ii]=pointList.we[ii]*pointList.traj[ii][2];//w3(i)
                  pointList.y[7] [ii]=pointList.we[ii]*pointList.traj[ii][3];//s11(i)
                  pointList.y[8] [ii]=pointList.we[ii]*pointList.traj[ii][4];//s12(i)
                  pointList.y[9] [ii]=pointList.we[ii]*pointList.traj[ii][5];//s13(i)
                  pointList.y[10][ii]=pointList.we[ii]*pointList.traj[ii][6];//s22(i)
                  pointList.y[11][ii]=pointList.we[ii]*pointList.traj[ii][7];//s23(i)
                  pointList.y[12][ii]=pointList.we[ii]*pointList.traj[ii][8];//s33(i)
                  
               }
               
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,4);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x4[ii]=pointList.X[ii];//pxx
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,5);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x5[ii]=pointList.X[ii];//pxy
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,6);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x6[ii]=pointList.X[ii];//pxz
               }
			   makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,7);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x7[ii]=pointList.X[ii];//pyx
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,8);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x8[ii]=pointList.X[ii];//pyy
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,9);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x9[ii]=pointList.X[ii];//pyz
               }
			   makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,10);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x10[ii]=pointList.X[ii];//pzx
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,11);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x11[ii]=pointList.X[ii];//pzy
               }
               makeAT(numInTraj,order);
               makeATA(numInTraj,order);
               makeATY(numInTraj,order,12);
               solve(numInTraj,order);
               for(int ii=0;ii<order;ii++){
                  x12[ii]=pointList.X[ii];//pzz
               }

               for(int ii=0;ii<numInTraj;ii++){
                  pxxp[ii]=0;
                  pxyp[ii]=0;
				  pxzp[ii]=0;
				  pyxp[ii]=0;
                  pyyp[ii]=0;
				  pyzp[ii]=0;
				  pzxp[ii]=0;
                  pzyp[ii]=0;
				  pzzp[ii]=0;
				  
                  for(int ij=0;ij<order;ij++){
                     pxxp[ii]= pxxp[ii]+ x4[ij] *pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     pxyp[ii]= pxyp[ii]+ x5[ij] *pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     pxzp[ii]= pxzp[ii]+ x6[ij] *pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
					 pyxp[ii]= pyxp[ii]+ x7[ij] *pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     pyyp[ii]= pyyp[ii]+ x8[ij] *pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     pyzp[ii]= pyzp[ii]+ x9[ij] *pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
					 pzxp[ii]= pzxp[ii]+ x10[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     pzyp[ii]= pzyp[ii]+ x11[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                     pzzp[ii]= pzzp[ii]+ x12[ij]*pow((double)ii*pointList.deltaT+0.00001,(double)(ij));
                  }
                  
                  
                  
               }// end for loop through traj

			   for(int ii=0;ii<numInTraj;ii++){                   
					 
                  //prepare for xuagph files
                  pointList.point[ii+2][(int)pointList.traj[ii][31]][37]=pxxp[ii];
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][38]=pxyp[ii];
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][39]=pxzp[ii];
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][40]=pyxp[ii];
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][41]=pyyp[ii];
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][42]=pyzp[ii];
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][43]=pzxp[ii];
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][44]=pzyp[ii];
				  pointList.point[ii+2][(int)pointList.traj[ii][31]][45]=pzzp[ii];

				  div=(1/3.)*(fabs(pxyp[ii]-pyxp[ii]) + fabs(pxzp[ii]-pzxp[ii]) + fabs(pyzp[ii]-pzyp[ii]));
				  ref=(1/3.)*(fabs(pxyp[ii])+fabs(pyxp[ii]) + fabs(pxzp[ii])+fabs(pzxp[ii]) + fabs(pyzp[ii])+fabs(pzyp[ii]));
				  if(ref>0){
				      quality=div/ref;
				  }
				  else{
					  quality=1;
				  }
                  pointList.point[ii+2][(int)pointList.traj[ii][31]][47]=quality;
				  //end of prepare xuagph files
				
				  for(int iii=2;iii<48;iii++){
                     fprintf(fpp, "%lf\t", pointList.point[ii+2][(int)pointList.traj[ii][31]][iii]);
			      }
			      fprintf(fpp, "%lf\n", (double)(ii));
				 
				  if(quality<0.2){
                     pointList.count2++;
				  }
                     
               }// end for
               ////end of polynom business
               
            } //end if of polynom buisness
            
         } // end if not occ und central
         
     }// end haupt for schlaufe
     

}

void readXUAPFile(int n, bool firstTime)
{
    int numOfPoints;
    double left,right,x,y,z,u,v,w,ax,ay,az,dummy,cubic;

    FILE *fpp;
    char name[256];
    int c;

	
    FILE *fpp_xuag;
    char name_xuag[256];
    int c_xuag;
    c_xuag=sprintf (name_xuag, pointList.path);
    c_xuag+=sprintf (name_xuag+c_xuag, "/xuag.");
    c_xuag+=sprintf (name_xuag+c_xuag, "%1d", n-1);

    
    
	
    
       for(int i=0;i<pointList.numOfFrames;i++){
          if(n-2+i>pointList.firstFile-1 && n-2+i<pointList.lastFile+1){
             if(i<pointList.numOfFrames-1 && !(firstTime)){
                 //write xuag
				 if(i==2){
				    fpp_xuag = fopen(name_xuag,"w");
				    for(int j=1;j<pointList.point[2][0][0]+1;j++){           
                       for(int arg=0;arg<34;arg++){
                          fprintf(fpp_xuag, "%lf\t", pointList.point[2][j][arg]);
                       }
                       fprintf(fpp_xuag, "%lf\n", pointList.point[2][j][34]);                        	
                    }
				    fclose (fpp_xuag);
				 }
			     //end write xuag
				 for(int j=0;j<pointList.point[i+1][0][0]+1;j++){           
					for(int k=0;k<35;k++){
                        pointList.point[i][j][k]=pointList.point[i+1][j][k];
                    }
                 }
             }
             else{
                numOfPoints=0;
                c=sprintf (name, pointList.path);
                c+=sprintf (name+c, "/xuap.");
                c+=sprintf (name+c, "%1d", n-2+i);
                fpp = fopen(name,"r");
                while(!feof(fpp)){
                   numOfPoints++;
                   fscanf (fpp, "%lf\0", &left);
                   fscanf (fpp, "%lf\0", &right);
                   fscanf (fpp, "%lf\0", &dummy); //measured x
                   fscanf (fpp, "%lf\0", &dummy); //measured y
                   fscanf (fpp, "%lf\0", &dummy); //measured z
                   fscanf (fpp, "%lf\0", &x); //cubic spline x
                   fscanf (fpp, "%lf\0", &y); //cubic spline y
                   fscanf (fpp, "%lf\0", &z); //cubic spline z
                   fscanf (fpp, "%lf\0", &u);
                   fscanf (fpp, "%lf\0", &v);
                   fscanf (fpp, "%lf\0", &w);
                   fscanf (fpp, "%lf\0", &ax);
                   fscanf (fpp, "%lf\0", &ay);
                   fscanf (fpp, "%lf\0", &az);
                   fscanf (fpp, "%lf\0", &cubic);
                   pointList.point[i][numOfPoints][0]=left;
                   pointList.point[i][numOfPoints][1]=right;
                   pointList.point[i][numOfPoints][2]=x;
                   pointList.point[i][numOfPoints][3]=y;
                   pointList.point[i][numOfPoints][4]=z;
                   pointList.point[i][numOfPoints][5]=u;
                   pointList.point[i][numOfPoints][6]=v;
                   pointList.point[i][numOfPoints][7]=w;
                   pointList.point[i][numOfPoints][8]=ax;
                   pointList.point[i][numOfPoints][9]=ay;
                   pointList.point[i][numOfPoints][10]=az;
                   pointList.point[i][numOfPoints][11]=cubic;
                }
                fclose (fpp);
                pointList.point[i][0][0]=numOfPoints;
             }
          }
          else{
             pointList.point[i][0][0]=0;
          }
       }
}

void readXUAGFile(int n, bool firstTime)
{
    int numOfPoints;
    double left,right,x,y,z,u,v,w,ax,ay,az,dummy,cubic;
	double ux,uy,uz,vx,vy,vz,wx,wy,wz,grad,ut,vt,wt,axx,axy,axz,ayx,ayy,ayz,azx,azy,azz,quality;

    FILE *fpp;
    char name[256];
    int c;

	
    FILE *fpp_xuagp;
    char name_xuagp[256];
    int c_xuagp;
    c_xuagp=sprintf (name_xuagp, pointList.path);
    c_xuagp+=sprintf (name_xuagp+c_xuagp, "/xuagp.");
    c_xuagp+=sprintf (name_xuagp+c_xuagp, "%1d", n-1);

    
    
	
    
       for(int i=0;i<pointList.numOfFrames;i++){
          if(n-2+i>pointList.firstFile-1 && n-2+i<pointList.lastFile){
             if(i<pointList.numOfFrames-1 && !(firstTime)){
                 //write xuagp
				 if(i==2){
				    fpp_xuagp = fopen(name_xuagp,"w");
				    for(int j=1;j<pointList.point[2][0][0]+1;j++){           
                       for(int arg=0;arg<37;arg++){
                          fprintf(fpp_xuagp, "%lf\t", pointList.point[2][j][arg]);
                       }
                       fprintf(fpp_xuagp, "%lf\n", pointList.point[2][j][37]);                        	
                    }
				    fclose (fpp_xuagp);
				 }
			     //end write xuagp
				 for(int j=0;j<pointList.point[i+1][0][0]+1;j++){           
					for(int k=0;k<38;k++){
                        pointList.point[i][j][k]=pointList.point[i+1][j][k];
                    }
                 }
             }
             else{
                numOfPoints=0;
                c=sprintf (name, pointList.path);
                c+=sprintf (name+c, "/xuag.");
                c+=sprintf (name+c, "%1d", n-2+i);
                fpp = fopen(name,"r");
                while(!feof(fpp)){
                   numOfPoints++;
                   fscanf (fpp, "%lf\0", &left);
                   fscanf (fpp, "%lf\0", &right);
                   fscanf (fpp, "%lf\0", &x); //cubic spline x
                   fscanf (fpp, "%lf\0", &y); //cubic spline y
                   fscanf (fpp, "%lf\0", &z); //cubic spline z
                   fscanf (fpp, "%lf\0", &u);
                   fscanf (fpp, "%lf\0", &v);
                   fscanf (fpp, "%lf\0", &w);
                   fscanf (fpp, "%lf\0", &ax);
                   fscanf (fpp, "%lf\0", &ay);
                   fscanf (fpp, "%lf\0", &az);
                   fscanf (fpp, "%lf\0", &cubic);
				   fscanf (fpp, "%lf\0", &ux);
                   fscanf (fpp, "%lf\0", &uy);
                   fscanf (fpp, "%lf\0", &uz);
				   fscanf (fpp, "%lf\0", &vx);
                   fscanf (fpp, "%lf\0", &vy);
                   fscanf (fpp, "%lf\0", &vz);
				   fscanf (fpp, "%lf\0", &wx);
                   fscanf (fpp, "%lf\0", &wy);
                   fscanf (fpp, "%lf\0", &wz);
				   fscanf (fpp, "%lf\0", &grad);
				   fscanf (fpp, "%lf\0", &ut);
				   fscanf (fpp, "%lf\0", &vt);
				   fscanf (fpp, "%lf\0", &wt);
				   fscanf (fpp, "%lf\0", &axx);
				   fscanf (fpp, "%lf\0", &axy);
				   fscanf (fpp, "%lf\0", &axz);
				   fscanf (fpp, "%lf\0", &ayx);
				   fscanf (fpp, "%lf\0", &ayy);
				   fscanf (fpp, "%lf\0", &ayz);
				   fscanf (fpp, "%lf\0", &azx);
				   fscanf (fpp, "%lf\0", &azy);
				   fscanf (fpp, "%lf\0", &azz);
				   fscanf (fpp, "%lf\0", &quality);
                   pointList.point[i][numOfPoints][0]=left;//1
                   pointList.point[i][numOfPoints][1]=right;//2
                   pointList.point[i][numOfPoints][2]=x;//3
                   pointList.point[i][numOfPoints][3]=y;//4
                   pointList.point[i][numOfPoints][4]=z;//5
                   pointList.point[i][numOfPoints][5]=u;//6
                   pointList.point[i][numOfPoints][6]=v;//7
                   pointList.point[i][numOfPoints][7]=w;//8
                   pointList.point[i][numOfPoints][8]=ax;//9
                   pointList.point[i][numOfPoints][9]=ay;//10
                   pointList.point[i][numOfPoints][10]=az;//11
                   pointList.point[i][numOfPoints][11]=cubic;//12
				   pointList.point[i][numOfPoints][12]=ux;//13
                   pointList.point[i][numOfPoints][13]=uy;//14
                   pointList.point[i][numOfPoints][14]=uz;//15
				   pointList.point[i][numOfPoints][15]=vx;//16
                   pointList.point[i][numOfPoints][16]=vy;//17
                   pointList.point[i][numOfPoints][17]=vz;//18
				   pointList.point[i][numOfPoints][18]=wx;//19
                   pointList.point[i][numOfPoints][19]=wy;//20
                   pointList.point[i][numOfPoints][20]=wz;//21
				   pointList.point[i][numOfPoints][21]=grad;//22
				   pointList.point[i][numOfPoints][22]=ut;//23
                   pointList.point[i][numOfPoints][23]=vt;//24
                   pointList.point[i][numOfPoints][24]=wt;//25
				   pointList.point[i][numOfPoints][25]=axx;//26
				   pointList.point[i][numOfPoints][26]=axy;//27
				   pointList.point[i][numOfPoints][27]=axz;//28
				   pointList.point[i][numOfPoints][28]=ayx;//29
				   pointList.point[i][numOfPoints][29]=ayy;//30
				   pointList.point[i][numOfPoints][30]=ayz;//31
				   pointList.point[i][numOfPoints][31]=azx;//32
				   pointList.point[i][numOfPoints][32]=azy;//33
				   pointList.point[i][numOfPoints][33]=azz;//34
				   pointList.point[i][numOfPoints][34]=0;//35
				   pointList.point[i][numOfPoints][35]=0;//36
				   pointList.point[i][numOfPoints][36]=0;//37
				   pointList.point[i][numOfPoints][37]=quality;//38;
                }
                fclose (fpp);
                pointList.point[i][0][0]=numOfPoints;
             }
          }
          else{
             pointList.point[i][0][0]=0;
          }
       }
}

void readXUAGPFile(int n, bool firstTime)
{
    int numOfPoints;
    double left,right,x,y,z,u,v,w,ax,ay,az,cubic;
	double ux,uy,uz,vx,vy,vz,wx,wy,wz,grad,ut,vt,wt,axx,axy,axz,ayx,ayy,ayz,azx,azy,azz,px,py,pz,quality;

    FILE *fpp;
    char name[256];
    int c;

	
    FILE *fpp_xuagph;
    char name_xuagph[256];
    int c_xuagph;
    c_xuagph=sprintf (name_xuagph, pointList.path);
    c_xuagph+=sprintf (name_xuagph+c_xuagph, "/xuagph.");
    c_xuagph+=sprintf (name_xuagph+c_xuagph, "%1d", n-1);

    
    
	
    
       for(int i=0;i<pointList.numOfFrames;i++){
          if(n-2+i>pointList.firstFile-1 && n-2+i<pointList.lastFile){
             if(i<pointList.numOfFrames-1 && !(firstTime)){
                 //write xuagp
				 if(i==2){
				    fpp_xuagph = fopen(name_xuagph,"w");
				    for(int j=1;j<pointList.point[2][0][0]+1;j++){           
						if(pointList.point[2][j][47]==1){
                            pointList.point[2][j][46]=1;
						}
					    for(int arg=0;arg<47;arg++){
                            fprintf(fpp_xuagph, "%lf\t", pointList.point[2][j][arg]);
                        }
                        fprintf(fpp_xuagph, "%lf\n", pointList.point[2][j][47]);                        	
                    }
				    fclose (fpp_xuagph);
				 }
			     //end write xuagp
				 for(int j=0;j<pointList.point[i+1][0][0]+1;j++){           
					for(int k=0;k<48;k++){
                        pointList.point[i][j][k]=pointList.point[i+1][j][k];
                    }
                 }
             }
             else{
                numOfPoints=0;
                c=sprintf (name, pointList.path);
                c+=sprintf (name+c, "/xuagp.");
                c+=sprintf (name+c, "%1d", n-2+i);
                fpp = fopen(name,"r");
                while(!feof(fpp)){
                   numOfPoints++;
                   fscanf (fpp, "%lf\0", &left);
                   fscanf (fpp, "%lf\0", &right);
                   fscanf (fpp, "%lf\0", &x); //cubic spline x
                   fscanf (fpp, "%lf\0", &y); //cubic spline y
                   fscanf (fpp, "%lf\0", &z); //cubic spline z
                   fscanf (fpp, "%lf\0", &u);
                   fscanf (fpp, "%lf\0", &v);
                   fscanf (fpp, "%lf\0", &w);
                   fscanf (fpp, "%lf\0", &ax);
                   fscanf (fpp, "%lf\0", &ay);
                   fscanf (fpp, "%lf\0", &az);
                   fscanf (fpp, "%lf\0", &cubic);
				   fscanf (fpp, "%lf\0", &ux);
                   fscanf (fpp, "%lf\0", &uy);
                   fscanf (fpp, "%lf\0", &uz);
				   fscanf (fpp, "%lf\0", &vx);
                   fscanf (fpp, "%lf\0", &vy);
                   fscanf (fpp, "%lf\0", &vz);
				   fscanf (fpp, "%lf\0", &wx);
                   fscanf (fpp, "%lf\0", &wy);
                   fscanf (fpp, "%lf\0", &wz);
				   fscanf (fpp, "%lf\0", &grad);
				   fscanf (fpp, "%lf\0", &ut);
				   fscanf (fpp, "%lf\0", &vt);
				   fscanf (fpp, "%lf\0", &wt);
				   fscanf (fpp, "%lf\0", &axx);
				   fscanf (fpp, "%lf\0", &axy);
				   fscanf (fpp, "%lf\0", &axz);
				   fscanf (fpp, "%lf\0", &ayx);
				   fscanf (fpp, "%lf\0", &ayy);
				   fscanf (fpp, "%lf\0", &ayz);
				   fscanf (fpp, "%lf\0", &azx);
				   fscanf (fpp, "%lf\0", &azy);
				   fscanf (fpp, "%lf\0", &azz);
				   fscanf (fpp, "%lf\0", &px);
				   fscanf (fpp, "%lf\0", &py);
				   fscanf (fpp, "%lf\0", &pz);
				   fscanf (fpp, "%lf\0", &quality);
                   pointList.point[i][numOfPoints][0]=left;//1
                   pointList.point[i][numOfPoints][1]=right;//2
                   pointList.point[i][numOfPoints][2]=x;//3
                   pointList.point[i][numOfPoints][3]=y;//4
                   pointList.point[i][numOfPoints][4]=z;//5
                   pointList.point[i][numOfPoints][5]=u;//6
                   pointList.point[i][numOfPoints][6]=v;//7
                   pointList.point[i][numOfPoints][7]=w;//8
                   pointList.point[i][numOfPoints][8]=ax;//9
                   pointList.point[i][numOfPoints][9]=ay;//10
                   pointList.point[i][numOfPoints][10]=az;//11
                   pointList.point[i][numOfPoints][11]=cubic;//12
				   pointList.point[i][numOfPoints][12]=ux;//13
                   pointList.point[i][numOfPoints][13]=uy;//14
                   pointList.point[i][numOfPoints][14]=uz;//15
				   pointList.point[i][numOfPoints][15]=vx;//16
                   pointList.point[i][numOfPoints][16]=vy;//17
                   pointList.point[i][numOfPoints][17]=vz;//18
				   pointList.point[i][numOfPoints][18]=wx;//19
                   pointList.point[i][numOfPoints][19]=wy;//20
                   pointList.point[i][numOfPoints][20]=wz;//21
				   pointList.point[i][numOfPoints][21]=grad;//22
				   pointList.point[i][numOfPoints][22]=ut;//23
                   pointList.point[i][numOfPoints][23]=vt;//24
                   pointList.point[i][numOfPoints][24]=wt;//25
				   pointList.point[i][numOfPoints][25]=axx;//26
				   pointList.point[i][numOfPoints][26]=axy;//27
				   pointList.point[i][numOfPoints][27]=axz;//28
				   pointList.point[i][numOfPoints][28]=ayx;//29
				   pointList.point[i][numOfPoints][29]=ayy;//30
				   pointList.point[i][numOfPoints][30]=ayz;//31
				   pointList.point[i][numOfPoints][31]=azx;//32
				   pointList.point[i][numOfPoints][32]=azy;//33
				   pointList.point[i][numOfPoints][33]=azz;//34
				   pointList.point[i][numOfPoints][34]=px;//35
				   pointList.point[i][numOfPoints][35]=py;//36
				   pointList.point[i][numOfPoints][36]=pz;//37
				   pointList.point[i][numOfPoints][37]=0;//38
				   pointList.point[i][numOfPoints][38]=0;//39
				   pointList.point[i][numOfPoints][39]=0;//40
				   pointList.point[i][numOfPoints][40]=0;//41
				   pointList.point[i][numOfPoints][41]=0;//42
				   pointList.point[i][numOfPoints][42]=0;//43
				   pointList.point[i][numOfPoints][43]=0;//44
				   pointList.point[i][numOfPoints][44]=0;//45
				   pointList.point[i][numOfPoints][45]=0;//46
				   pointList.point[i][numOfPoints][46]=quality;//47;
				   pointList.point[i][numOfPoints][47]=1;//48;
                }
                fclose (fpp);
                pointList.point[i][0][0]=numOfPoints;
             }
          }
          else{
             pointList.point[i][0][0]=0;
          }
       }
}