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
static void doGrid(FILE *fpp, int t,int startPoint);
static void readXUAPFile(int n, bool firstTime);



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
		if (NULL == (input = fopen("D:/post_grid.inp","r"))){
		//if (NULL == (input = fopen("D:/post_proc.inp","r"))){
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

	//data
    fscanf(input,"%s",pa); flushline(input);sprintf (pointList.path,pa);
    fscanf(input,"%i",&n); flushline(input);pointList.firstFile                = n;
	fscanf(input,"%i",&n); flushline(input);pointList.lastFile                 = n;

	//controls
	fscanf(input,"%f",&e); flushline(input);pointList.deltaT                   = e;
	fscanf(input,"%f",&e); flushline(input);pointList.maxRadius                = e;
	fscanf(input,"%f",&e); flushline(input);pointList.minx                     = e;
	fscanf(input,"%f",&e); flushline(input);pointList.maxx                     = e;
	fscanf(input,"%f",&e); flushline(input);pointList.miny                     = e;
	fscanf(input,"%f",&e); flushline(input);pointList.maxy                     = e;
	fscanf(input,"%f",&e); flushline(input);pointList.minz                     = e;
	fscanf(input,"%f",&e); flushline(input);pointList.maxz                     = e;
	fscanf(input,"%f",&e); flushline(input);pointList.dex                      = e;
	fscanf(input,"%f",&e); flushline(input);pointList.dey                      = e;
	fscanf(input,"%f",&e); flushline(input);pointList.dez                      = e;
	flushline(input);
	fscanf(input,"%i",&n); flushline(input);pointList.maxRank                  = n;

		
	
	//end of read in control parameters
	///////////////////////////////////////////////////////////////////////////////////

	

       pointList.count=0;
	   pointList.count2=0;
       pointList.count3=0;
	   pointList.count4=0;
	   pointList.count5=0;
       pointList.meanDiss=0.;
	   pointList.meanUSq=0.;

       for (int i=pointList.firstFile;i<pointList.lastFile+1;i++){
		  if((double)pointList.count3/(double)pointList.count>0){
             cout << "point per sphere.............."<<(double)pointList.count3/(double)pointList.count<<"\n\n";
		  }
		  cout << "processing file .............."<<i<<"\n";
		   
          c=sprintf (name, pointList.path);
	      c+=sprintf (name+c, "/grid_raw.");
          c+=sprintf (name+c, "%1d", i); 
          fpp = fopen(name,"w");
          doGrid(fpp,i,0);
          fclose (fpp);
      
       }
	
	
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
       c+=sprintf (name+c, "%1d", n+index); 
       
       fpp = fopen(name,"r");
       fscanf (fpp, "%d\0", &numOfPoints);
       pointList.point[index+10][0][0]=numOfPoints;
       for (int i=1; i<numOfPoints+1; i++){
           fscanf (fpp, "%d\0", &left);
           fscanf (fpp, "%d\0", &right);
           fscanf (fpp, "%lf\0", &x);
           fscanf (fpp, "%lf\0", &y);
           fscanf (fpp, "%lf\0", &z);
           rmsDist=0.005;
           pointList.point[index+10][i][0]=left+1;
           pointList.point[index+10][i][1]=right+1;

           pointList.point[index+10][i][2]=x*0.001;
           pointList.point[index+10][i][3]=y*0.001;
           pointList.point[index+10][i][4]=z*0.001;
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
    c+=sprintf (name+c, "/g_xuap.");
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

void doGrid(FILE *fpp, int t,int startPoint)
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

     time=2;
     int minCounter;
	 int counter_f;

	 double avU[3];
	 double avA[3];

     bool ok;
     startT=t;

     if(t==pointList.firstFile){
        readXUAPFile(t,true);
     }
     else{
        readXUAPFile(t,false);
     }
          
	 for(centerX=pointList.minx;centerX<pointList.maxx;centerX=centerX+pointList.dex){
         for(centerY=pointList.miny;centerY<pointList.maxy;centerY=centerY+pointList.dey){
             for(centerZ=pointList.minz;centerZ<pointList.maxz;centerZ=centerZ+pointList.dez){
                 
                 setAllMatrixesToZero(4);
			     contin=true;   
                                              
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

                 
                 minCounter=3;
               
                 if(pCounterA>minCounter && pCounterB>minCounter && pCounterC>minCounter){ // %jetzt wird endlich Punkt1 interpoliert
                    pointList.count++;
					pointList.count3=pointList.count3+pCounterB; 
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
                  
				    Liu[0]=avU[0];
                    Liu[1]=pointList.Aij[0][0];
                    Liu[2]=pointList.Aij[0][1];
                    Liu[3]=pointList.Aij[0][2];
				    Liv[0]=avU[1];
                    Liv[1]=pointList.Aij[1][0];
                    Liv[2]=pointList.Aij[1][1];
                    Liv[3]=pointList.Aij[1][2];
				    Liw[0]=avU[2];
                    Liw[1]=pointList.Aij[2][0];
                    Liw[2]=pointList.Aij[2][1];
                    Liw[3]=pointList.Aij[2][2];

				    Liax[0]=avA[0];
				    Liax[1]=pointList.Aaij[0][0];
                    Liax[2]=pointList.Aaij[0][1];
                    Liax[3]=pointList.Aaij[0][2];
                    Liay[0]=avA[1];
				    Liay[1]=pointList.Aaij[1][0];
                    Liay[2]=pointList.Aaij[1][1];
                    Liay[3]=pointList.Aaij[1][2];
                    Liaz[0]=avA[2];
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
				    ref_diva=fabs(1./2.*wsq)+fabs(1./2.*twosijsij)+fabs(Liax[1])+fabs(Liay[2])+fabs(Liaz[3]);

					fprintf(fpp, "%lf\t",centerX);
                    fprintf(fpp, "%lf\t",centerY);
                    fprintf(fpp, "%lf\t",centerZ);
                    fprintf(fpp, "%lf\t",Liu[0]);
                    fprintf(fpp, "%lf\t",Liv[0]);
                    fprintf(fpp, "%lf\t",Liw[0]);
                    fprintf(fpp, "%lf\t",Liax[0]);
                    fprintf(fpp, "%lf\t",Liay[0]);
                    fprintf(fpp, "%lf\t",Liaz[0]);
                    fprintf(fpp, "%lf\t",w1);
                    fprintf(fpp, "%lf\t",w2);
                    fprintf(fpp, "%lf\t",w3);
                    fprintf(fpp, "%lf\t",s11);
                    fprintf(fpp, "%lf\t",s12);
                    fprintf(fpp, "%lf\t",s13);
                    fprintf(fpp, "%lf\t",s22);
                    fprintf(fpp, "%lf\t",s23);
                    fprintf(fpp, "%lf\t",s33);
                    fprintf(fpp, "%lf\t",Liu[4]);//dudt
                    fprintf(fpp, "%lf\t",Liv[4]);//dvdt
                    fprintf(fpp, "%lf\t",Liw[4]);//dwdt

                    fprintf(fpp, "%lf\t",Liax[1]);
                    fprintf(fpp, "%lf\t",Liax[2]);
				    fprintf(fpp, "%lf\t",Liax[3]);
				    fprintf(fpp, "%lf\t",Liay[1]);
                    fprintf(fpp, "%lf\t",Liay[2]);
				    fprintf(fpp, "%lf\t",Liay[3]);
				    fprintf(fpp, "%lf\t",Liaz[1]);
                    fprintf(fpp, "%lf\t",Liaz[2]);
				    fprintf(fpp, "%lf\t",Liaz[3]);

                    fprintf(fpp, "%lf\t",1-fabs(2*Q+diva)/ref_diva);
					fprintf(fpp, "%lf\n",1-absDi);
                 }
                  
                 if(!(pCounterA>minCounter && pCounterB>minCounter && pCounterC>minCounter ) || !(contin)){
				    fprintf(fpp, "%lf\t",centerX);
                    fprintf(fpp, "%lf\t",centerY);
                    fprintf(fpp, "%lf\t",centerZ);
					 for(int i=0;i<31-3;i++){
                         fprintf(fpp, "%lf\t",0.);
				     }
                     fprintf(fpp, "%lf\n",0.);
                 }
			 }//for x
		 }// for y
	 }// for z
                   

}





void readXUAPFile(int n, bool firstTime)
{
    int numOfPoints;
    double left,right,x,y,z,u,v,w,ax,ay,az,dummy,cubic;

    FILE *fpp;
    char name[256];
    int c;

    
       for(int i=0;i<5;i++){
          if(n-2+i>pointList.firstFile-1 && n-2+i<pointList.lastFile+1){
             if(i<5-1 && !(firstTime)){
				 for(int j=0;j<pointList.point[i+1][0][0]+1;j++){           
					for(int k=0;k<15;k++){
                        pointList.point[i][j][k]=pointList.point[i+1][j][k];
                    }
                 }
             }
             else{
                numOfPoints=0;
                c=sprintf (name, pointList.path);
                c+=sprintf (name+c, "/g_xuap.");
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





