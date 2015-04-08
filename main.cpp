//---------------------------------------------------------------------------
#include <vcl.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>                       
#include <stdio.h>
#pragma hdrstop


#include <io.h>
#include <alloc.h>
#include <fcntl.h>
#include <process.h>
#include <sys\stat.h>

#include "main.h"
#include "parameters.h"
#include "graph.h"
#include "xuap.h"
#include "traj.h"
#include "struct.h"
#include "accuracy.h"
#include "structRepr.h"
#include "kriging.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TmainForm *mainForm;
TpointList *pointList;

static void sort(double minDistB[],int minDistBIndex[], int index);
//---------------------------------------------------------------------------
__fastcall TmainForm::TmainForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::doPointsBtnClick(TObject *Sender)
{
    int PL=StrToInt(xuapForm->maxEdit->Text);
    int PLh=int((double)PL/2.);

    if(!pointList){
       pointList=new TpointList();
    }
    pointList->setPathAndFiles();
    mainForm->Refresh();
    pointList->count=0;
    pointList->maxVel=0.;
    pointList->meanVel=0.;
    pointList->meanAcc=0.;
    for (int i=pointList->firstFile;i<pointList->lastFile+1;i++){
       countEdit->Text=IntToStr(pointList->count);
       maxVelEdit->Text=IntToStr((int)(1.e3*pointList->maxVel+0.5));
       meanVelEdit->Text=IntToStr((int)(1.e3*pointList->meanVel+0.5));
       meanAccEdit->Text=IntToStr((int)(1.e3*pointList->meanAcc+0.5));
       countEdit->Refresh();
       maxVelEdit->Refresh();
       meanVelEdit->Refresh();
       meanAccEdit->Refresh();
       fileNumEdit->Text=IntToStr(i);
       fileNumEdit->Refresh();
       //for (int ii=-10;ii<11;ii++){
       for (int ii=-PLh;ii<PLh+1;ii++){
           pointList->readPTVFile(i,ii);
           //pointList->readAddedFile(i,ii);
       }
       pointList->doCubicSplinesTwenty(false,0);
       pointList->writePTVtoFile(i);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::setPathAndFiles()
{
     AnsiString dummy;
     mainForm->OpenDialog1->Filter="ptv_is files (ptv_is.*)|ptv_is.*";
     //mainForm->OpenDialog1->Filter="added files (added.*)|added.*";
     mainForm->OpenDialog1->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog1->FileName);
     dummy=dummy.Delete(1, 1);
     firstFile= StrToInt(dummy);
     mainForm->OpenDialog2->Filter="ptv_is files (ptv_is.*)|ptv_is.*";
     //mainForm->OpenDialog2->Filter="added files (added.*)|added.*";
     mainForm->OpenDialog2->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog2->FileName);
     dummy=dummy.Delete(1, 1);
     lastFile= StrToInt(dummy);
     int index=mainForm->OpenDialog1->FileName.LastDelimiter("." );
     baseName= mainForm->OpenDialog1->FileName;
     baseName=baseName.SetLength(index);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::setPathAndFiles2()
{
     AnsiString dummy;
     mainForm->OpenDialog3->Filter="xuap files (xuap.*)|xuap.*";
     mainForm->OpenDialog3->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog3->FileName);
     dummy=dummy.Delete(1, 1);
     firstFile= StrToInt(dummy);
     mainForm->OpenDialog4->Filter="xuap files (xuap.*)|xuap.*";
     mainForm->OpenDialog4->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog4->FileName);
     dummy=dummy.Delete(1, 1);
     lastFile= StrToInt(dummy);
     int index=mainForm->OpenDialog3->FileName.LastDelimiter("." );
     baseName= mainForm->OpenDialog3->FileName;
     baseName=baseName.SetLength(index);
     PathSet=true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::setPathAndFiles7()
{

     mainForm->OpenDialog11->Filter="position file (*.pt3)|*.pt3";
     mainForm->OpenDialog11->Execute();
     baseName1= mainForm->OpenDialog11->FileName;
     PathSet=true;
     mainForm->OpenDialog12->Filter="link file (*.pln)|*.pln";
     mainForm->OpenDialog12->Execute();
     baseName2= mainForm->OpenDialog12->FileName;
     PathSet=true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::setPathAndFiles8()
{
     AnsiString dummy;
     mainForm->OpenDialog13->Filter="ptv_is files (ptv_is.*)|ptv_is.*";
     mainForm->OpenDialog13->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog13->FileName);
     dummy=dummy.Delete(1, 1);
     firstFile= StrToInt(dummy);
     mainForm->OpenDialog14->Filter="ptv_is files (ptv_is.*)|ptv_is.*";
     mainForm->OpenDialog14->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog14->FileName);
     dummy=dummy.Delete(1, 1);
     lastFile= StrToInt(dummy);
     int index=mainForm->OpenDialog13->FileName.LastDelimiter("." );
     baseName= mainForm->OpenDialog13->FileName;
     baseName=baseName.SetLength(index);
     index=mainForm->OpenDialog14->FileName.LastDelimiter("." );
     baseNameTwo= mainForm->OpenDialog14->FileName;
     baseNameTwo=baseNameTwo.SetLength(index);
     PathSet=true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::setPathAndFiles9()
{
     AnsiString dummy;
     mainForm->OpenDialog15->Filter="trajAcc files (trajAcc.*)|trajAcc.*";
     mainForm->OpenDialog15->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog15->FileName);
     dummy=dummy.Delete(1, 1);
     firstFile= StrToInt(dummy);
     mainForm->OpenDialog16->Filter="trajAcc files (trajAcc.*)|trajAcc.*";
     mainForm->OpenDialog16->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog16->FileName);
     dummy=dummy.Delete(1, 1);
     lastFile= StrToInt(dummy);
     int index=mainForm->OpenDialog15->FileName.LastDelimiter("." );
     baseName= mainForm->OpenDialog15->FileName;
     baseName=baseName.SetLength(index);
     
     PathSet=true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::setPathAndFiles5()
{
     AnsiString dummy;
     mainForm->OpenDialog3->Filter="grid files (grid.*)|grid.*";
     mainForm->OpenDialog3->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog3->FileName);
     dummy=dummy.Delete(1, 1);
     firstFile= StrToInt(dummy);
     mainForm->OpenDialog4->Filter="grid files (grid.*)|grid.*";
     mainForm->OpenDialog4->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog4->FileName);
     dummy=dummy.Delete(1, 1);
     lastFile= StrToInt(dummy);
     int index=mainForm->OpenDialog3->FileName.LastDelimiter("." );
     baseName= mainForm->OpenDialog3->FileName;
     baseName=baseName.SetLength(index);
     PathSet=true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::setPathAndFiles4()
{
     AnsiString dummy;
     mainForm->OpenDialog3->Filter="xuadP files (xuadP.*)|xuadP.*";
     mainForm->OpenDialog3->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog3->FileName);
     dummy=dummy.Delete(1, 1);
     firstFile= StrToInt(dummy);
     mainForm->OpenDialog4->Filter="xuadP files (xuadP.*)|xuadP.*";
     mainForm->OpenDialog4->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog4->FileName);
     dummy=dummy.Delete(1, 1);
     lastFile= StrToInt(dummy);
     int index=mainForm->OpenDialog3->FileName.LastDelimiter("." );
     baseName= mainForm->OpenDialog3->FileName;
     baseName=baseName.SetLength(index);
     PathSet=true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::setPathAndFiles3()
{
     AnsiString dummy;
     mainForm->OpenDialog5->Filter="trajPoint files (trajPoint.*)|trajPoint.*";
     mainForm->OpenDialog5->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog5->FileName);
     dummy=dummy.Delete(1, 1);
     firstFile= StrToInt(dummy);
     mainForm->OpenDialog6->Filter="trajPoint files (trajPoint.*)|trajPoint.*";
     mainForm->OpenDialog6->Execute();
     dummy=ExtractFileExt(mainForm->OpenDialog6->FileName);
     dummy=dummy.Delete(1, 1);
     lastFile= StrToInt(dummy);
     int index=mainForm->OpenDialog5->FileName.LastDelimiter("." );
     baseName= mainForm->OpenDialog5->FileName;
     baseName=baseName.SetLength(index);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::readPTVFile(int n, int index)
{
    FILE *fpp;
    int numOfPoints;
    int left,right;
    double x,y,z,rmsDist;

    AnsiString name;

    if(n+index>firstFile-1 && n+index<lastFile+1){
       if(n+index<100){
           if(n+index<10){
               name=baseName+IntToStr(0)+IntToStr(0)+IntToStr(n+index);
           }
           else{
               name=baseName+IntToStr(0)+IntToStr(n+index);
           }
       }
       else{
          name=baseName+IntToStr(n+index);
       } 
       //name=baseName+IntToStr(n+index);
       const char *filename;
       filename=name.c_str();

       fpp = fopen(filename,"r");
       fscanf (fpp, "%d\0", &numOfPoints);
       point[index+10][0][0]=numOfPoints;
       for (int i=1; i<numOfPoints+1; i++){
           fscanf (fpp, "%d\0", &left);
           fscanf (fpp, "%d\0", &right);
           fscanf (fpp, "%lf\0", &x);
           fscanf (fpp, "%lf\0", &y);
           fscanf (fpp, "%lf\0", &z);
           rmsDist=0.005;
           point[index+10][i][0]=left+1;
           point[index+10][i][1]=right+1;

           point[index+10][i][2]=x*0.001;
           point[index+10][i][3]=y*0.001;
           point[index+10][i][4]=z*0.001;
           point[index+10][i][15]=rmsDist;
      }
       fclose (fpp);
    }
    else{
       point[index+10][0][0]=0;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::readAddedFile(int n, int index)
{
    FILE *fpp;
    int numOfPoints;
    int left,right,dummy;
    double x,y,z,quality;

    AnsiString name;

    if(n+index>firstFile-1 && n+index<lastFile+1){
       if(n+index<100){
           if(n+index<10){
               name=baseName+IntToStr(0)+IntToStr(0)+IntToStr(n+index);
           }
           else{
               name=baseName+IntToStr(0)+IntToStr(n+index);
           }
       }
       else{
          name=baseName+IntToStr(n+index);
       } 
       //name=baseName+IntToStr(n+index);
       const char *filename;
       filename=name.c_str();

       fpp = fopen(filename,"r");
       fscanf (fpp, "%d\0", &numOfPoints);
       point[index+10][0][0]=numOfPoints;
       for (int i=1; i<numOfPoints+1; i++){
           fscanf (fpp, "%d\0", &left);
           fscanf (fpp, "%d\0", &right);
           fscanf (fpp, "%lf\0", &x);
           fscanf (fpp, "%lf\0", &y);
           fscanf (fpp, "%lf\0", &z);
           fscanf (fpp, "%d\0", &dummy);
           fscanf (fpp, "%lf\0", &quality);

           point[index+10][i][0]=left+1;
           point[index+10][i][1]=right+1;

           point[index+10][i][2]=x*0.001;
           point[index+10][i][3]=y*0.001;
           point[index+10][i][4]=z*0.001;
           point[index+10][i][15]=quality;
      }
       fclose (fpp);
    }
    else{
       point[index+10][0][0]=0;
    }
}
//----------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::writeXUADP(int n)
{
    FILE *fpp;
    //int numOfPoints;
    //int left,right,dummy;
    //double x,y,z,quality;

    AnsiString name;
    name="xuadP."+IntToStr(n);

    const char *filename;
    filename=name.c_str();

    fpp = fopen(filename,"w");
    for(int i=1;i<(int)(point[2][0][0]);i++){
       fprintf(fpp, "%lf\t", point[2][i][0]);
       fprintf(fpp, "%lf\t", point[2][i][1]);
       fprintf(fpp, "%lf\t", point[2][i][2]);
       fprintf(fpp, "%lf\t", point[2][i][3]);
       fprintf(fpp, "%lf\t", point[2][i][4]);
       fprintf(fpp, "%lf\t", point[2][i][2]);
       fprintf(fpp, "%lf\t", point[2][i][3]);
       fprintf(fpp, "%lf\t", point[2][i][4]);
       fprintf(fpp, "%lf\t", point[2][i][5]);
       fprintf(fpp, "%lf\t", point[2][i][6]);
       fprintf(fpp, "%lf\t", point[2][i][7]);
       fprintf(fpp, "%lf\t", point[2][i][8]);
       fprintf(fpp, "%lf\t", point[2][i][9]);
       fprintf(fpp, "%lf\t", point[2][i][10]);
       fprintf(fpp, "%lf\t", point[2][i][11]);
       fprintf(fpp, "%lf\t", point[2][i][12]);
       fprintf(fpp, "%lf\t", point[2][i][13]);
       fprintf(fpp, "%lf\t", point[2][i][14]);
       fprintf(fpp, "%lf\t", point[2][i][15]);
       fprintf(fpp, "%lf\t", point[2][i][16]);
       fprintf(fpp, "%lf\t", point[2][i][17]);
       fprintf(fpp, "%lf\t", point[2][i][18]);
       fprintf(fpp, "%lf\t", point[2][i][19]);
       fprintf(fpp, "%lf\t", point[2][i][20]);
       fprintf(fpp, "%lf\t", point[2][i][21]);
       fprintf(fpp, "%lf\t", point[2][i][22]);
       fprintf(fpp, "%lf\t", point[2][i][23]);
       fprintf(fpp, "%lf\n", point[2][i][24]);
    }
    fclose (fpp);

}
//----------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TpointList::readXUAPFile(int n, bool firstTime, bool estimate)
{
    FILE *fpp;
    int numOfPoints;
    double left,right,x,y,z,u,v,w,ax,ay,az,dummy,cubic;
    int indx,indy,indz;
    double radius=StrToFloat(mainForm->radiusSpatEdit->Text);
    int numSearchBox=(int)(0.022/radius)+1;

    AnsiString name;
    const char *filename;
    if(estimate){
       numOfPoints=0;
       name=baseName+IntToStr(n);
       filename=name.c_str();
       fpp = fopen(filename,"r");
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
          //fscanf (fpp, "%lf\0", &quality);
          point[2][numOfPoints][0]=left;
          point[2][numOfPoints][1]=right;
          point[2][numOfPoints][2]=x;
          point[2][numOfPoints][3]=y;
          point[2][numOfPoints][4]=z;
          point[2][numOfPoints][5]=u;
          point[2][numOfPoints][6]=v;
          point[2][numOfPoints][7]=w;
          point[2][numOfPoints][8]=ax;
          point[2][numOfPoints][9]=ay;
          point[2][numOfPoints][10]=az;
          point[2][numOfPoints][11]=cubic;
          //if(cubic>0){point[2][numOfPoints][11]=1.;}
          //new quality
          //point[2][numOfPoints][12]=quality;
       }
       fclose (fpp);
       point[2][0][0]=numOfPoints++;
    }
    else{
       for(int i=0;i<200;i++){
          if(n-5+i>firstFile-1 && n-5+i<lastFile+1){
             if(i<numOfFrames-1 && !(firstTime)){
                for(int j=0;j<point[i+1][0][0]+1;j++){
                    for(int k=0;k<16;k++){
                        point[i][j][k]=point[i+1][j][k];
                    }
                }
                for(int xx=0;xx<numSearchBox;xx++){for(int yy=0;yy<numSearchBox;yy++){for(int zz=0;zz<numSearchBox;zz++){
                   for(int j=0;j<100;j++){
                      fast_search[i][xx][yy][zz][j]=fast_search[i+1][xx][yy][zz][j];}}}}
             }
             else{
                numOfPoints=0;
                for(int xx=0;xx<numSearchBox;xx++){for(int yy=0;yy<numSearchBox;yy++){for(int zz=0;zz<numSearchBox;zz++){
                   fast_search[i][xx][yy][zz][0]=0;}}}
                name=baseName+IntToStr(n-2+i);
                filename=name.c_str();
                fpp = fopen(filename,"r");
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
                   //fscanf (fpp, "%lf\0", &quality);
                   point[i][numOfPoints][0]=left;
                   point[i][numOfPoints][1]=right;
                   point[i][numOfPoints][2]=x;
                   point[i][numOfPoints][3]=y;
                   point[i][numOfPoints][4]=z;
                   point[i][numOfPoints][5]=u;
                   point[i][numOfPoints][6]=v;
                   point[i][numOfPoints][7]=w;
                   point[i][numOfPoints][8]=ax;
                   point[i][numOfPoints][9]=ay;
                   point[i][numOfPoints][10]=az;
                   point[i][numOfPoints][11]=cubic;
                   ////////////for fast search
                   indx=(int)((x*1000+7.)/(1000*radius));
                   indy=(int)((y*1000+7.)/(1000*radius));
                   indz=(int)((z*1000+15.)/(1000*radius));
                   if(indx<0){indx=0;}if(indx>numSearchBox-1){indx=numSearchBox-1;}
                   if(indy<0){indy=0;}if(indy>numSearchBox-1){indy=numSearchBox-1;}
                   if(indz<0){indz=0;}if(indz>numSearchBox-1){indz=numSearchBox-1;}
                   if(cubic>0 && fast_search[i][indx][indy][indz][0]<99){
                      fast_search[i][indx][indy][indz][0]=fast_search[i][indx][indy][indz][0]+1;
                      fast_search[i][indx][indy][indz][fast_search[i][indx][indy][indz][0]]=numOfPoints;
                   }
                   //////////end for fast search

                }
                fclose (fpp);
                point[i][0][0]=numOfPoints++;
             }
          }
          else{
             point[i][0][0]=0;
          }
       }
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::readWeights()
{
    double value;

    FILE *fpp;
    AnsiString name;
    name="krigingWeights.dat";
    const char *filename;
    filename=name.c_str();
    fpp = fopen(filename,"r");


    for(int i=0;i<30;i++){
       fscanf (fpp, "%lf\0", &value);
       cor[i]=value;
    }
    fclose (fpp);
}
//-----------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::readGridFile(int t, int position)
{
    FILE *fpp;
    numPointInGrid=0;
    int numFields;

       numFields=32;
    

    double value;

    AnsiString name;
    const char *filename;

    name=baseName+IntToStr(t);
    filename=name.c_str();
    fpp = fopen(filename,"r");
    while(!feof(fpp)){
       for(int i=0;i<numFields;i++){
           fscanf (fpp, "%lf\0", &value);
           gridMem[position][numPointInGrid][i]=value;
       }
       numPointInGrid++;
    }
    fclose (fpp);
    numPointInGrid--;

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::readXUAPDFile(int n, bool firstTime, bool estimate)
{
    FILE *fpp;
    int numOfPoints;
    double left,right,x,y,z,u,v,w,ax,ay,az,dummy,cubic,quality,reldiv,index,numInTraj;
    double ux,uy,uz,vx,vy,vz,wx,wy,wz;

    AnsiString name;
    const char *filename;
    if(estimate){
       numOfPoints=0;
       name=baseName+IntToStr(n);
       filename=name.c_str();
       fpp = fopen(filename,"r");
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
          fscanf (fpp, "%lf\0", &quality);
          fscanf (fpp, "%lf\0", &reldiv);
          fscanf (fpp, "%lf\0", &index);
          fscanf (fpp, "%lf\0", &numInTraj);
          fscanf (fpp, "%lf\0", &ux); //ux
          fscanf (fpp, "%lf\0", &uy); //uy
          fscanf (fpp, "%lf\0", &uz); //uz
          fscanf (fpp, "%lf\0", &vx); //vx
          fscanf (fpp, "%lf\0", &vy); //vy
          fscanf (fpp, "%lf\0", &vz); //vz
          fscanf (fpp, "%lf\0", &wx); //wx
          fscanf (fpp, "%lf\0", &wy); //wy
          fscanf (fpp, "%lf\0", &wz); //wz
          point[2][numOfPoints][0]=left;
          point[2][numOfPoints][1]=right;
          point[2][numOfPoints][2]=x;
          point[2][numOfPoints][3]=y;
          point[2][numOfPoints][4]=z;
          point[2][numOfPoints][5]=u;
          point[2][numOfPoints][6]=v;
          point[2][numOfPoints][7]=w;
          point[2][numOfPoints][8]=ax;
          point[2][numOfPoints][9]=ay;
          point[2][numOfPoints][10]=az;
          point[2][numOfPoints][11]=cubic;
          //new quality
          point[2][numOfPoints][12]=quality;
          point[2][numOfPoints][13]=reldiv;
          point[2][numOfPoints][14]=index;
          point[2][numOfPoints][15]=numInTraj;

          point[2][numOfPoints][16]=ux;
          point[2][numOfPoints][17]=uy;
          point[2][numOfPoints][18]=uz;
          point[2][numOfPoints][19]=vx;
          point[2][numOfPoints][20]=vy;
          point[2][numOfPoints][21]=vz;
          point[2][numOfPoints][22]=wx;
          point[2][numOfPoints][23]=wy;
          point[2][numOfPoints][24]=wz;

       }
       fclose (fpp);
       point[2][0][0]=numOfPoints++;
    }
    else{
       for(int i=0;i<200;i++){
          if(n-2+i>firstFile-1 && n-2+i<lastFile+1){
             if(i<numOfFrames-1 && !(firstTime)){
                for(int j=0;j<point[i+1][0][0]+1;j++){
                    for(int k=0;k<12;k++){
                        point[i][j][k]=point[i+1][j][k];
                    }
                }
             }
             else{
                numOfPoints=0;
                name=baseName+IntToStr(n-2+i);
                filename=name.c_str();
                fpp = fopen(filename,"r");
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
                   fscanf (fpp, "%lf\0", &quality);
                   fscanf (fpp, "%lf\0", &reldiv);
                   fscanf (fpp, "%lf\0", &index);
                   fscanf (fpp, "%lf\0", &numInTraj);
                   fscanf (fpp, "%lf\0", &ux); //ux
                   fscanf (fpp, "%lf\0", &uy); //uy
                   fscanf (fpp, "%lf\0", &uz); //uz
                   fscanf (fpp, "%lf\0", &vx); //vx
                   fscanf (fpp, "%lf\0", &vy); //vy
                   fscanf (fpp, "%lf\0", &vz); //vz
                   fscanf (fpp, "%lf\0", &wx); //wx
                   fscanf (fpp, "%lf\0", &wy); //wy
                   fscanf (fpp, "%lf\0", &wz); //wz
                   point[i][numOfPoints][0]=left;
                   point[i][numOfPoints][1]=right;
                   point[i][numOfPoints][2]=x;
                   point[i][numOfPoints][3]=y;
                   point[i][numOfPoints][4]=z;
                   point[i][numOfPoints][5]=u;
                   point[i][numOfPoints][6]=v;
                   point[i][numOfPoints][7]=w;
                   point[i][numOfPoints][8]=ax;
                   point[i][numOfPoints][9]=ay;
                   point[i][numOfPoints][10]=az;
                   point[i][numOfPoints][11]=cubic;
                   //new quality
                   point[i][numOfPoints][12]=quality;
                   point[2][numOfPoints][13]=reldiv;
                   point[2][numOfPoints][14]=index;
                   point[2][numOfPoints][15]=numInTraj;

                   point[2][numOfPoints][16]=ux;
                   point[2][numOfPoints][17]=uy;
                   point[2][numOfPoints][18]=uz;
                   point[2][numOfPoints][19]=vx;
                   point[2][numOfPoints][20]=vy;
                   point[2][numOfPoints][21]=vz;
                   point[2][numOfPoints][22]=wx;
                   point[2][numOfPoints][23]=wy;
                   point[2][numOfPoints][24]=wz;
                }
                fclose (fpp);
                point[i][0][0]=numOfPoints++;
             }
          }
          else{
             point[i][0][0]=0;
          }
       }
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::readTrajPointFile(int n)
{
    FILE *fpp;
    int numOfPoints;
    double x,y,z,u,v,w,ax,ay,az,w1,w2,w3,s11,s12,s13,s22,s23,s33;
    double ww1,ww2,ww3,wwsij,sijsjkski,R,Q,mioDiss,divref,startP,age,ut,vt,wt;

    AnsiString name;
    const char *filename;
    for(int i=0;i<51;i++){
       if(n-50+i>firstFile-1){
          if(n>firstFile && i<50){
             for(int j=0;j<trajec[i+1][0][0]+1;j++){
                for(int k=0;k<32;k++){
                   trajec[i][j][k]=trajec[i+1][j][k];
                }
             }
          }
          else{
             numOfPoints=0;
             name=baseName+IntToStr(n-50+i);
             filename=name.c_str();
             fpp = fopen(filename,"r");
             while(!feof(fpp)){
                numOfPoints++;
                fscanf (fpp, "%lf\0", &x);
                fscanf (fpp, "%lf\0", &y);
                fscanf (fpp, "%lf\0", &z);
                fscanf (fpp, "%lf\0", &u);
                fscanf (fpp, "%lf\0", &v);
                fscanf (fpp, "%lf\0", &w);
                fscanf (fpp, "%lf\0", &ax);
                fscanf (fpp, "%lf\0", &ay);
                fscanf (fpp, "%lf\0", &az);
                fscanf (fpp, "%lf\0", &w1);
                fscanf (fpp, "%lf\0", &w2);
                fscanf (fpp, "%lf\0", &w3);
                fscanf (fpp, "%lf\0", &s11);
                fscanf (fpp, "%lf\0", &s12);
                fscanf (fpp, "%lf\0", &s13);
                fscanf (fpp, "%lf\0", &s22);
                fscanf (fpp, "%lf\0", &s23);
                fscanf (fpp, "%lf\0", &s33);
                fscanf (fpp, "%lf\0", &ww1);
                fscanf (fpp, "%lf\0", &ww2);
                fscanf (fpp, "%lf\0", &ww3);
                fscanf (fpp, "%lf\0", &wwsij);
                fscanf (fpp, "%lf\0", &sijsjkski);
                fscanf (fpp, "%lf\0", &R);
                fscanf (fpp, "%lf\0", &Q);
                fscanf (fpp, "%lf\0", &mioDiss);
                fscanf (fpp, "%lf\0", &divref);
                fscanf (fpp, "%lf\0", &startP);
                fscanf (fpp, "%lf\0", &age);
                fscanf (fpp, "%lf\0", &ut);
                fscanf (fpp, "%lf\0", &vt);
                fscanf (fpp, "%lf\0", &wt);

                trajec[i][numOfPoints][0]=x;
                trajec[i][numOfPoints][1]=y;
                trajec[i][numOfPoints][2]=z;
                trajec[i][numOfPoints][3]=u;
                trajec[i][numOfPoints][4]=v;
                trajec[i][numOfPoints][5]=w;
                trajec[i][numOfPoints][6]=ax;
                trajec[i][numOfPoints][7]=ay;
                trajec[i][numOfPoints][8]=az;
                trajec[i][numOfPoints][9]=w1;
                trajec[i][numOfPoints][10]=w2;
                trajec[i][numOfPoints][11]=w3;
                trajec[i][numOfPoints][12]=s11;
                trajec[i][numOfPoints][13]=s12;
                trajec[i][numOfPoints][14]=s13;
                trajec[i][numOfPoints][15]=s22;
                trajec[i][numOfPoints][16]=s23;
                trajec[i][numOfPoints][17]=s33;
                trajec[i][numOfPoints][18]=ww1;
                trajec[i][numOfPoints][19]=ww2;
                trajec[i][numOfPoints][20]=ww3;
                trajec[i][numOfPoints][21]=wwsij;
                trajec[i][numOfPoints][22]=sijsjkski;
                trajec[i][numOfPoints][23]=R;
                trajec[i][numOfPoints][24]=Q;
                trajec[i][numOfPoints][25]=mioDiss;
                trajec[i][numOfPoints][26]=divref;
                trajec[i][numOfPoints][27]=startP;
                trajec[i][numOfPoints][28]=age;
                trajec[i][numOfPoints][29]=ut;
                trajec[i][numOfPoints][30]=vt;
                trajec[i][numOfPoints][31]=wt;
             }
             fclose (fpp);
             trajec[i][0][0]=numOfPoints++;
          }
       }
       else{
          trajec[i][0][0]=0;
       }
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::doCubicSplines()
{
   int nP=point[2][0][0];
   for(int i=1;i<nP+1;i++){
      point[2][i][14]=0;
      if(point[2][i][0]>0 && point[2][i][1]>0){
         int mOne=(int)point[2][i][0];
         int pOne=(int)point[2][i][1];
         if(point[1][mOne][0]>0 && point[3][pOne][1]>0){
            int mTwo=(int)point[1][mOne][0];
            int pTwo=(int)point[3][pOne][1];
            point[2][i][14]=1;
            //x-Component
            for(int t=-2;t<3;t++){
               A[t+2][0]=1.;
               A[t+2][1]=(double)t*deltaT;
               A[t+2][2]=pow((double)t*deltaT,2.);
               A[t+2][3]=pow((double)t*deltaT,3.);
            }
            Y[0]=point[0][mTwo][2];
            Y[1]=point[1][mOne][2];
            Y[2]=point[2][i][2];
            Y[3]=point[3][pOne][2];
            Y[4]=point[4][pTwo][2];
            makeAT(5,4);
            makeATA(5,4);
            makeATY(5,4,0);
            solve(5,4);
            point[2][i][5]=X[0];
            point[2][i][8]=X[1];
            point[2][i][11]=2.*X[2];
            //y-Component
            for(int t=-2;t<3;t++){
               A[t+2][0]=1.;
               A[t+2][1]=(double)t*deltaT;
               A[t+2][2]=pow((double)t*deltaT,2.);
               A[t+2][3]=pow((double)t*deltaT,3.);
            }
            Y[0]=point[0][mTwo][3];
            Y[1]=point[1][mOne][3];
            Y[2]=point[2][i][3];
            Y[3]=point[3][pOne][3];
            Y[4]=point[4][pTwo][3];
            makeAT(5,4);
            makeATA(5,4);
            makeATY(5,4,0);
            solve(5,4);
            point[2][i][6]=X[0];
            point[2][i][9]=X[1];
            point[2][i][12]=2.*X[2];
            //z-Component
            for(int t=-2;t<3;t++){
               A[t+2][0]=1.;
               A[t+2][1]=(double)t*deltaT;
               A[t+2][2]=pow((double)t*deltaT,2.);
               A[t+2][3]=pow((double)t*deltaT,3.);
            }
            Y[0]=point[0][mTwo][4];
            Y[1]=point[1][mOne][4];
            Y[2]=point[2][i][4];
            Y[3]=point[3][pOne][4];
            Y[4]=point[4][pTwo][4];
            makeAT(5,4);
            makeATA(5,4);
            makeATY(5,4,0);
            solve(5,4);
            point[2][i][7]=X[0];
            point[2][i][10]=X[1];
            point[2][i][13]=2.*X[2];
         }
      }
   }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::doCubicSplinesTwenty(bool single,int number)
{
   int PL=StrToInt(xuapForm->maxEdit->Text);
   int PLacc=StrToInt(xuapForm->maxAccEdit->Text);
   PLacc=int((double)PLacc/2.);
   int minLength=StrToInt(xuapForm->minEdit->Text);
   int minLengthAcc=StrToInt(xuapForm->minAccEdit->Text);

   int PLh=int((double)PL/2.);
   int nP=point[10][0][0];
   int ind[21];
   double tolerance=0.15;//StrToFloat(paramForm->toleranceEdit->Text);
   double tolMaxVel=StrToFloat(mainForm->tolMaxVelEdit->Text);
   double velocity;

   double weight;

   deltaT=StrToFloat(mainForm->deltaTEdit->Text);

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
      point[10][i][14]=0; //can be cubic splined
      int maxIndex=10;
      int minIndex=10;
      int index=10;
      int badCounter=0;
      ind[index]=i;
      bool ok=true;

      while(index>10-PLh && ok){
          if(point[index][ind[index]][0]>0){
             ind[index-1]=point[index][ind[index]][0];
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
      //while(index<PL-1 && ok){
          if(point[index][ind[index]][1]>0 && point[index][0][0]>0){
             ind[index+1]=point[index][ind[index]][1];
             index++;
             maxIndex=index;
          }
          else{
             ok=false;
          }
      }

      //first do for x and u, then do for a
      if(maxIndex-minIndex>2+badCounter && maxIndex>9+minLength && minIndex<11-minLength){ //ok (minIndex<10 && maxIndex>10){//
      //if(maxIndex-minIndex>2+badCounter ){ //ok (minIndex<10 && maxIndex>10){//
          point[10][i][14]=1;
          //x-Component
          setAllMatrixesToZero(4);
          for(int t=minIndex-10;t<maxIndex-10+1;t++){
              weight     = point[t+10][ind[t+10]][15];
              weight     = 1.-1./(1.+exp(-300.*(weight-0.015)));
              A[t+10][0] = 1.*weight;
              A[t+10][1] = (double)t*deltaT*weight;
              A[t+10][2] = pow((double)t*deltaT,2.)*weight;
              A[t+10][3] = pow((double)t*deltaT,3.)*weight;
              y[0][t+10] = point[t+10][ind[t+10]][2]*weight;
          }
          makeAT(21,4);
          makeATA(21,4);
          makeATY(21,4,0);
          solve(21,4);

          point[10][i][5]=X[0];//point[10][ind[10]][2];//
          point[10][i][8]=X[1];//(1./(2.*deltaT))*(point[11][ind[11]][2]-point[9][ind[9]][2]);//
          point[10][i][11]=2.*X[2];//(1./(deltaT*deltaT))*(point[11][ind[11]][2]-2.*point[10][ind[10]][2]+point[9][ind[9]][2]);//
          //y-Component
          setAllMatrixesToZero(4);
          for(int t=minIndex-10;t<maxIndex-10+1;t++){
              weight     = point[t+10][ind[t+10]][15];
              weight     = 1.-1./(1.+exp(-300.*(weight-0.015)));
              A[t+10][0] = 1.*weight;
              A[t+10][1] = (double)t*deltaT*weight;
              A[t+10][2] = pow((double)t*deltaT,2.)*weight;
              A[t+10][3] = pow((double)t*deltaT,3.)*weight;
              y[0][t+10] = point[t+10][ind[t+10]][3]*weight;
          }
          makeAT(21,4);
          makeATA(21,4);
          makeATY(21,4,0);
          solve(21,4);

          point[10][i][6]=X[0]; //point[10][ind[10]][3];//
          point[10][i][9]=X[1]; //(1./(2.*deltaT))*(point[11][ind[11]][3]-point[9][ind[9]][3]);//
          point[10][i][12]=2.*X[2]; //(1./(deltaT*deltaT))*(point[11][ind[11]][3]-2.*point[10][ind[10]][3]+point[9][ind[9]][3]);//
          //z-Component
          setAllMatrixesToZero(4);
          for(int t=minIndex-10;t<maxIndex-10+1;t++){
              weight     = point[t+10][ind[t+10]][15];
              weight     = 1.-1./(1.+exp(-300.*(weight-0.015)));
              A[t+10][0] = 1.*weight;
              A[t+10][1] = (double)t*deltaT*weight;
              A[t+10][2] = pow((double)t*deltaT,2.)*weight;
              A[t+10][3] = pow((double)t*deltaT,3.)*weight;
              y[0][t+10] = point[t+10][ind[t+10]][4]*weight;
          }
          makeAT(21,4);
          makeATA(21,4);
          makeATY(21,4,0);
          solve(21,4);
          
          point[10][i][7]=X[0]; //point[10][ind[10]][4];//
          point[10][i][10]=X[1];//(1./(2.*deltaT))*(point[11][ind[11]][4]-point[9][ind[9]][4]);//
          point[10][i][13]=2.*X[2]; //(1./(deltaT*deltaT))*(point[11][ind[11]][4]-2.*point[10][ind[10]][4]+point[9][ind[9]][4]);//
          //max break!
          velocity=pow(pow(point[10][i][8],2.)+pow(point[10][i][9],2.)+pow(point[10][i][10],2.),0.5);
          if(velocity>tolMaxVel){
             point[10][i][14]=0;
          }
          if(maxIndex<17){
             point[10][i][14]=(maxIndex-10.)/7.+0.1;
          }
      }
      /*
      //now do for a
      if(maxIndex>10+PLacc){
         maxIndex=10+PLacc;
      }
      if(minIndex<10-PLacc){
         minIndex=10-PLacc;
      }
      if(maxIndex-minIndex>2+badCounter && maxIndex>9+minLengthAcc && minIndex<11-minLengthAcc){ //ok (minIndex<10 && maxIndex>10){//
      //if(maxIndex-minIndex>2+badCounter ){ //ok (minIndex<10 && maxIndex>10){//
          //x-Component
          setAllMatrixesToZero(4);
          for(int t=minIndex-10;t<maxIndex-10+1;t++){
              if (PointIsOk[t+10]){
                 A[t+10][0]=1.;
                 A[t+10][1]=(double)t*deltaT;
                 A[t+10][2]=pow((double)t*deltaT,2.);
                 A[t+10][3]=pow((double)t*deltaT,3.);
                 Y[t+10]=point[t+10][ind[t+10]][2];
              }
          }
          makeAT(21,4);
          makeATA(21,4);
          makeATY(21,4,0);
          solve(21,4);

          //point[10][i][5]=X[0];//point[10][ind[10]][2];//
          //point[10][i][8]=X[1];//(1./(2.*deltaT))*(point[11][ind[11]][2]-point[9][ind[9]][2]);//
          point[10][i][11]=2.*X[2];//(1./(deltaT*deltaT))*(point[11][ind[11]][2]-2.*point[10][ind[10]][2]+point[9][ind[9]][2]);//
          //y-Component
          setAllMatrixesToZero(4);
          for(int t=minIndex-10;t<maxIndex-10+1;t++){
              if (PointIsOk[t+10]){
                 A[t+10][0]=1.;
                 A[t+10][1]=(double)t*deltaT;
                 A[t+10][2]=pow((double)t*deltaT,2.);
                 A[t+10][3]=pow((double)t*deltaT,3.);
                 Y[t+10]=point[t+10][ind[t+10]][3];
              }
          }
          makeAT(21,4);
          makeATA(21,4);
          makeATY(21,4,0);
          solve(21,4);

          //point[10][i][6]=X[0]; //point[10][ind[10]][3];//
          //point[10][i][9]=X[1]; //(1./(2.*deltaT))*(point[11][ind[11]][3]-point[9][ind[9]][3]);//
          point[10][i][12]=2.*X[2]; //(1./(deltaT*deltaT))*(point[11][ind[11]][3]-2.*point[10][ind[10]][3]+point[9][ind[9]][3]);//
          //z-Component
          setAllMatrixesToZero(4);
          for(int t=minIndex-10;t<maxIndex-10+1;t++){
              if (PointIsOk[t+10]){
                 A[t+10][0]=1.;
                 A[t+10][1]=(double)t*deltaT;
                 A[t+10][2]=pow((double)t*deltaT,2.);
                 A[t+10][3]=pow((double)t*deltaT,3.);
                 Y[t+10]=point[t+10][ind[t+10]][4];
              }
          }
          makeAT(21,4);
          makeATA(21,4);
          makeATY(21,4,0);
          solve(21,4);
          
          //point[10][i][7]=X[0]; //point[10][ind[10]][4];//
          //point[10][i][10]=X[1];//(1./(2.*deltaT))*(point[11][ind[11]][4]-point[9][ind[9]][4]);//
          point[10][i][13]=2.*X[2]; //(1./(deltaT*deltaT))*(point[11][ind[11]][4]-2.*point[10][ind[10]][4]+point[9][ind[9]][4]);//
          //max break!
      }
      */
   }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::estimateAccuracy(int t)
{
     int pCounterB,startP;
     double centerX,centerY,centerZ,dist,dx,dy,dz;
     double minDist[200];
     int minDistIndex[200],rank;
     int i,indexNo,indexDi,cubicPoints;
     double uMean,vMean,wMean,kFluct,kResid,DuError,DvError,DwError,ratio;
     
     int stepSize=StrToInt(accuracyForm->stepEdit->Text);

     pointList->readXUAPFile(t,true,true);

     for(int n=1;n<point[2][0][0]+1;n=n+stepSize){
         if(point[2][n][11]>0.){
            for(int k=0;k<200;k++){
               minDist[k]=1000;
            }
            cubicPoints=0;
            for(int k=1;k<point[2][0][0]+1;k++){
               if(!(k==n) && point[2][k][11]>0){
                  cubicPoints=cubicPoints+1;
                  dist=pow(pow(point[2][k][2]-point[2][n][2],2.)+pow(point[2][k][3]-point[2][n][3],2.)+pow(point[2][k][4]-point[2][n][4],2.),0.5);
                  bool shift=false;
                  if(dist<minDist[199] && dist>0){
                     shift=true;
                     rank=199;
                     for(int k=199;k>-1;k--){
                        if(dist<minDist[k]){
                           rank=k;
                        }
                     }
                  }
                  if(shift){
                     for(int l=199;l>rank;l--){
                        minDist[l]=minDist[l-1];
                        minDistIndex[l]=minDistIndex[l-1];
                     }
                     minDist[rank]=dist;
                     minDistIndex[rank]=k;
                  }
               }
            }//min dist List for point n is finished
            if(accuracyForm->orderRadioGroup->ItemIndex==0){
               startP=4;
            }
            if(accuracyForm->orderRadioGroup->ItemIndex==1){
               startP=10;
            }
            if(accuracyForm->orderRadioGroup->ItemIndex==2){
               startP=20;
            }
            
            for(int p=startP;p<200;p++){
             if(minDist[p]<100){
               //interpolieren und rausschreiben mit t,n (Zeit und Startpunkt)
               //%Da soll jetzt duidxj linear interpoliert werden
               //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
               //%die nächsten Punkte zu Punkt x,y,z, finden
               pCounterB=0;
               if(accuracyForm->orderRadioGroup->ItemIndex==0){
                  setAllMatrixesToZero(4);
               }
               if(accuracyForm->orderRadioGroup->ItemIndex==1){
                  setAllMatrixesToZero(10);
               }
               if(accuracyForm->orderRadioGroup->ItemIndex==2){
                  setAllMatrixesToZero(20);
               }

               for(int ii=0;ii<arraySize;ii++){
                  dis[ii]=0.;
                  disB[ii]=0.;
               }
               centerX=point[2][n][2];
               centerY=point[2][n][3];
               centerZ=point[2][n][4];

               for(int d=0;d<p;d++){
                  /*i=minDistIndex[d];
                  dist=minDist[d];
                  disB[pCounterB]=(int)(dist*1000.+0.5);
                  dx=point[2][i][2]-centerX;
                  dy=point[2][i][3]-centerY;
                  dz=point[2][i][4]-centerZ;
                  B[pCounterB][0]=1.;
                  B[pCounterB][1]=dx;
                  B[pCounterB][2]=dy;
                  B[pCounterB][3]=dz;
                  if(accuracyForm->orderRadioGroup->ItemIndex==1){
                     B[pCounterB][4]=dx*dx;
                     B[pCounterB][5]=dy*dy;
                     B[pCounterB][6]=dz*dz;
                     B[pCounterB][7]=dx*dy;
                     B[pCounterB][8]=dx*dz;
                     B[pCounterB][9]=dy*dz;
                  }
                  if(accuracyForm->orderRadioGroup->ItemIndex==2){
                     B[pCounterB][4]=dx*dx;
                     B[pCounterB][5]=dy*dy;
                     B[pCounterB][6]=dz*dz;
                     B[pCounterB][7]=dx*dy;
                     B[pCounterB][8]=dx*dz;
                     B[pCounterB][9]=dy*dz;
                     B[pCounterB][10]=dx*dx*dx;
                     B[pCounterB][11]=dy*dy*dy;
                     B[pCounterB][12]=dz*dz*dz;
                     B[pCounterB][13]=dx*dx*dy;
                     B[pCounterB][14]=dx*dx*dz;
                     B[pCounterB][15]=dy*dy*dx;
                     B[pCounterB][16]=dy*dy*dz;
                     B[pCounterB][17]=dz*dz*dx;
                     B[pCounterB][18]=dz*dz*dy;
                     B[pCounterB][19]=dx*dy*dz;
                  }
                  YuB[pCounterB]=point[2][i][5];
                  YvB[pCounterB]=point[2][i][6];
                  YwB[pCounterB]=point[2][i][7];
                  pCounterB++; */
               }
               //here we should be able to get the fluctuation energy whithin the sphere
               uMean=0;
               vMean=0;
               wMean=0;
               for(int d=0;d<pCounterB;d++){
                  uMean=uMean+YuB[d];
                  //vMean=vMean+YvB[d];
                  //wMean=wMean+YwB[d];
               }
               uMean=uMean/(double)pCounterB;
               //vMean=vMean/(double)pCounterB;
               //wMean=wMean/(double)pCounterB;
               kFluct=0;
               /*for(int d=0;d<pCounterB;d++){
                  kFluct=kFluct+pow((YuB[d]-uMean)*(YuB[d]-uMean)+(YvB[d]-vMean)*(YvB[d]-vMean)+(YwB[d]-wMean)*(YwB[d]-wMean),0.5);
                  //kFluct=kFluct+pow((YuB[d]-uMean)*(YuB[d]-uMean),0.5);
               }
               kFluct=kFluct/(double)pCounterB;
               //end of fluctuation energy
               if(pCounterB>3){ // %jetzt wird endlich Punkt1 interpoliert
                  //%correct x,y,z with center of interpolation!

                  if(accuracyForm->orderRadioGroup->ItemIndex==0){
                     makeBT(pCounterB,4);
                     makeBTB(pCounterB,4);
                     makeBTY(pCounterB,4,1);
                     solveB(pCounterB,4);
                  }
                  if(accuracyForm->orderRadioGroup->ItemIndex==1){
                     makeBT(pCounterB,10);
                     makeBTB(pCounterB,10);
                     makeBTY(pCounterB,10,1);
                     solveB(pCounterB,10);
                     uxx=X[4];
                     uyy=X[5];
                     uzz=X[6];
                     uxy=X[7];
                     uxz=X[8];
                     uyz=X[9];
                  }
                  if(accuracyForm->orderRadioGroup->ItemIndex==2){
                     makeBT(pCounterB,20);
                     makeBTB(pCounterB,20);
                     makeBTY(pCounterB,20,1);
                     solveB(pCounterB,20);
                     uxx=X[4];
                     uyy=X[5];
                     uzz=X[6];
                     uxy=X[7];
                     uxz=X[8];
                     uyz=X[9];
                     uxxx=X[10];
                     uyyy=X[11];
                     uzzz=X[12];
                     uxxy=X[13];
                     uxxz=X[14];
                     uyyx=X[15];
                     uyyz=X[16];
                     uzzx=X[17];
                     uzzy=X[18];
                     uxyz=X[19];
                  }

                  u=X[0];
                  ux=X[1];
                  uy=X[2];
                  uz=X[3];

                  if(accuracyForm->orderRadioGroup->ItemIndex==0){
                     makeBT(pCounterB,4);
                     makeBTB(pCounterB,4);
                     makeBTY(pCounterB,4,2);
                     solveB(pCounterB,4);
                  }
                  if(accuracyForm->orderRadioGroup->ItemIndex==1){
                     makeBT(pCounterB,10);
                     makeBTB(pCounterB,10);
                     makeBTY(pCounterB,10,2);
                     solveB(pCounterB,10);
                     vxx=X[4];
                     vyy=X[5];
                     vzz=X[6];
                     vxy=X[7];
                     vxz=X[8];
                     vyz=X[9];
                  }
                  if(accuracyForm->orderRadioGroup->ItemIndex==2){
                     makeBT(pCounterB,20);
                     makeBTB(pCounterB,20);
                     makeBTY(pCounterB,20,2);
                     solveB(pCounterB,20);
                     vxx=X[4];
                     vyy=X[5];
                     vzz=X[6];
                     vxy=X[7];
                     vxz=X[8];
                     vyz=X[9];
                     vxxx=X[10];
                     vyyy=X[11];
                     vzzz=X[12];
                     vxxy=X[13];
                     vxxz=X[14];
                     vyyx=X[15];
                     vyyz=X[16];
                     vzzx=X[17];
                     vzzy=X[18];
                     vxyz=X[19];
                  }

                  v=X[0];
                  vx=X[1];
                  vy=X[2];
                  vz=X[3];

                  if(accuracyForm->orderRadioGroup->ItemIndex==0){
                     makeBT(pCounterB,4);
                     makeBTB(pCounterB,4);
                     makeBTY(pCounterB,4,3);
                     solveB(pCounterB,4);
                  }
                  if(accuracyForm->orderRadioGroup->ItemIndex==1){
                     makeBT(pCounterB,10);
                     makeBTB(pCounterB,10);
                     makeBTY(pCounterB,10,3);
                     solveB(pCounterB,10);
                     wxx=X[4];
                     wyy=X[5];
                     wzz=X[6];
                     wxy=X[7];
                     wxz=X[8];
                     wyz=X[9];
                  }
                  if(accuracyForm->orderRadioGroup->ItemIndex==2){
                     makeBT(pCounterB,20);
                     makeBTB(pCounterB,20);
                     makeBTY(pCounterB,20,3);
                     solveB(pCounterB,20);
                     wxx=X[4];
                     wyy=X[5];
                     wzz=X[6];
                     wxy=X[7];
                     wxz=X[8];
                     wyz=X[9];
                     wxxx=X[10];
                     wyyy=X[11];
                     wzzz=X[12];
                     wxxy=X[13];
                     wxxz=X[14];
                     wyyx=X[15];
                     wyyz=X[16];
                     wzzx=X[17];
                     wzzy=X[18];
                     wxyz=X[19];
                  }

                  w=X[0];
                  wx=X[1];
                  wy=X[2];
                  wz=X[3];

                  //compute Residual stuff
                  if(accuracyForm->orderRadioGroup->ItemIndex==0){
                     kResid=makekResidError(pCounterB);
                     DuError=makeDuError(pCounterB);
                     DvError=makeDvError(pCounterB);
                     DwError=makeDwError(pCounterB);
                  }
                  if(accuracyForm->orderRadioGroup->ItemIndex==1){
                     kResid=makekResidErrorQuad(pCounterB);
                     DuError=makeDuErrorQuad(pCounterB);
                     DvError=makeDvErrorQuad(pCounterB);
                     DwError=makeDwErrorQuad(pCounterB);
                  }
                  if(accuracyForm->orderRadioGroup->ItemIndex==2){
                     kResid=makekResidErrorCube(pCounterB);
                     DuError=makeDuErrorCube(pCounterB);
                     DvError=makeDvErrorCube(pCounterB);
                     DwError=makeDwErrorCube(pCounterB);
                  }

                  //end of Residual stuff

                  //update (global) Energy Curves
                  indexNo=pCounterB;
                  indexDi=(int)(minDist[p-1]*10000+0.5);
                  if(indexNo>0 && indexNo<200){
                     indexCounterNo[indexNo]=indexCounterNo[indexNo]+1;
                     kFluctArrayNo[indexNo]=((indexCounterNo[indexNo]-1)*kFluctArrayNo[indexNo]+kFluct)/indexCounterNo[indexNo];
                     kResidArrayNo[indexNo]=((indexCounterNo[indexNo]-1)*kResidArrayNo[indexNo]+kResid)/indexCounterNo[indexNo];
                     DuErrorArrayNo[indexNo]=((indexCounterNo[indexNo]-1)*DuErrorArrayNo[indexNo]+DuError)/indexCounterNo[indexNo];
                     DvErrorArrayNo[indexNo]=((indexCounterNo[indexNo]-1)*DvErrorArrayNo[indexNo]+DvError)/indexCounterNo[indexNo];
                     DwErrorArrayNo[indexNo]=((indexCounterNo[indexNo]-1)*DwErrorArrayNo[indexNo]+DwError)/indexCounterNo[indexNo];
                     kDistArrayNo[indexNo]=((indexCounterNo[indexNo]-1)*kDistArrayNo[indexNo]+minDist[p-1])/indexCounterNo[indexNo];
                  }
                  if(indexDi>0 && indexDi<200){
                     indexCounterDi[indexDi]=indexCounterDi[indexDi]+1;
                     kFluctArrayDi[indexDi]=((indexCounterDi[indexDi]-1)*kFluctArrayDi[indexDi]+kFluct)/indexCounterDi[indexDi];
                     kResidArrayDi[indexDi]=((indexCounterDi[indexDi]-1)*kResidArrayDi[indexDi]+kResid)/indexCounterDi[indexDi];
                     DuErrorArrayDi[indexDi]=((indexCounterDi[indexDi]-1)*DuErrorArrayDi[indexDi]+DuError)/indexCounterDi[indexDi];
                     DvErrorArrayDi[indexDi]=((indexCounterDi[indexDi]-1)*DvErrorArrayDi[indexDi]+DvError)/indexCounterDi[indexDi];
                     DwErrorArrayDi[indexDi]=((indexCounterDi[indexDi]-1)*DwErrorArrayDi[indexDi]+DwError)/indexCounterDi[indexDi];
                     kNoArrayDi[indexDi]=((indexCounterDi[indexDi]-1)*kNoArrayDi[indexDi]+(double)pCounterB)/indexCounterDi[indexDi];
                  }
                  //end of update global Energy curves!
              }//end of solving*/
             }//end if , does such a point exist at all?
            }//end of loop through closest points
         }//end of if (is it worthwhile to deal with point n?

            accuracyForm->fileEdit->Text=IntToStr(t);
            accuracyForm->pointEdit->Text=IntToStr(n);

            //draw Energy curves
            accuracyForm->Series1->Clear();
            accuracyForm->Series2->Clear();
            accuracyForm->Series3->Clear();
            accuracyForm->Series4->Clear();
            accuracyForm->Series5->Clear();
            accuracyForm->Series6->Clear();
            accuracyForm->Series7->Clear();
            accuracyForm->Series8->Clear();
            accuracyForm->Series9->Clear();
            accuracyForm->Series10->Clear();
            accuracyForm->Series11->Clear();
            accuracyForm->Series12->Clear();
            for(int i=0;i<200;i++){
               accuracyForm->Series1->AddXY((double)i,1e3*kFluctArrayNo[i],'.',clTeeColor);
               accuracyForm->Series2->AddXY((double)i,1e3*kResidArrayNo[i],'.',clTeeColor);
               accuracyForm->Series3->AddXY((double)i/10.,1e3*kFluctArrayDi[i],'.',clTeeColor);
               accuracyForm->Series4->AddXY((double)i/10.,1e3*kResidArrayDi[i],'.',clTeeColor);
               if(kFluctArrayNo[i]>0){
                  ratio=100*kResidArrayNo[i]/kFluctArrayNo[i];
               }
               else{
                  ratio=0;
               }
               accuracyForm->Series5->AddXY((double)i,ratio,'.',clTeeColor);
               if(kFluctArrayDi[i]>0){
               ratio=100*kResidArrayDi[i]/kFluctArrayDi[i];
               }
               else{
                  ratio=0;
               }
               accuracyForm->Series6->AddXY((double)i/10.,ratio,'.',clTeeColor);
               accuracyForm->Series7->AddXY((double)i,DuErrorArrayNo[i],'.',clTeeColor);
               accuracyForm->Series9->AddXY((double)i,DvErrorArrayNo[i],'.',clTeeColor);
               accuracyForm->Series10->AddXY((double)i,DwErrorArrayNo[i],'.',clTeeColor);
               accuracyForm->Series8->AddXY((double)i/10.,DuErrorArrayDi[i],'.',clTeeColor);
               accuracyForm->Series11->AddXY((double)i/10.,DvErrorArrayDi[i],'.',clTeeColor);
               accuracyForm->Series12->AddXY((double)i/10.,DwErrorArrayDi[i],'.',clTeeColor);

           }
           accuracyForm->Refresh();
           //end of draw curves
         
     }//end of loop through points
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::followTrajPointHarmonics(FILE *fpp, int t,int startPoint, bool write)
{
     int pCounterA,pCounterB,pCounterC[11], pCounter, rowIndex;
     int startT, startP,ind;
     double dist,dx,dy,dz,dt;
     double centerX,centerY,centerZ;
     double w1,w2,w3,s11,s12,s13,s22,s23,s33,vel,wsq,twosijsij,Q,NaX,NaY,NaZ,enstrophy;
     double dix,diy,diz,refx,refy,refz,accCriteria,absdi,as11,as22,as33,diva;
     double acx,acy,acz;
     double quality;

     double u_C,v_C,w_C,u_A,v_A,w_A;

     int time;
     double minDistB[200];
     int minDistBIndex[200];
     double minDistC[200][11];
     int minDistCIndex[200][11];

     //////////////////////////////////////////
     /////////////////////////////////////////
     int pm=2;
     int order=2;
     ///////////////////////////////////////////
     ////////////////////////////////////////////

     int indexB;
     int indexC[11];
     double u_local[11],v_local[11],w_local[11];

     double radius=StrToFloat(mainForm->radiusSpatEdit->Text);
     int numSearchBox=(int)(0.022/radius)+1;

     bool contin;

     int rank;

     int start;
     int end;
     double polyConst=StrToFloat(mainForm->polyConstEdit->Text);

     double maxRadiusSpat=StrToFloat(mainForm->radiusSpatEdit->Text);
     int minCounter;

     bool ok;
     startT=t;

     bool continuePast;
     bool continueFuture;
     int step[5];


     double viscosity=StrToFloat(mainForm->viscEdit->Text);
     deltaT=StrToFloat(mainForm->deltaTEdit->Text);
     c1=StrToFloat(mainForm->c1Edit->Text);
     c2=StrToFloat(mainForm->c2Edit->Text);

     double falseDist;
     int indx,indy,indz;
     double low_x,low_y,low_z,high_x,high_y,high_z;

     double su=0.;
     double x[53][300],pC[53][300];
     double xp[300],yp[300],zp[300];
     double up[300],vp[300],wp[300];
     double axp[300],ayp[300],azp[300];
     double dudxp[300],dudyp[300],dudzp[300];
     double dvdxp[300],dvdyp[300],dvdzp[300];
     double dwdxp[300],dwdyp[300],dwdzp[300];
     double dudtp[300],dvdtp[300],dwdtp[300];
     double daxdxp[300],daxdyp[300],daxdzp[300];
     double daydxp[300],daydyp[300],daydzp[300];
     double dazdxp[300],dazdyp[300],dazdzp[300];
     double NXp[300],NYp[300],NZp[300];
     double div,ref;
     double vectorSq;

     if(write){
        if(t==pointList->firstFile){
           pointList->readXUAPFile(t,true,false);
        }
        else{
           pointList->readXUAPFile(t,false,false);
        }
     }
     else{
        if(trajForm->changedFlag->Checked==true){
            pointList->readXUAPFile(t,true,false);
            trajForm->changedFlag->Checked=false;
        }
     }

     if(write){
         start=1;
         end=point[5][0][0]+1;
     }
     else{
         start=startPoint;
         end=startPoint+1;
     }

     int n;
     for(int nn=start;nn<end;nn++){
         time=5;
         if(point[5][nn][11]>0. &&!(occ[t-pointList->firstFile][nn])) {
            startP=nn;
            ok=true;
            numInTraj=0;
            noDeriv=0;
            n=nn;
            while(ok){
               occ[t-pointList->firstFile+time-5][n]=true;
               //interpolieren und rausschreiben mit t,n (Zeit und Startpunkt)
               //%Da soll jetzt duidxj linear interpoliert werden
               //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
               //%die nächsten Punkte zu Punkt x,y,z, finden
               count++;

               centerX=point[time][n][2];
               centerY=point[time][n][3];
               centerZ=point[time][n][4];

               //BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
               indexB=0;
               falseDist=pow(pow(centerX,2.)+pow(centerY,2.)+pow(centerZ,2.),0.5);
               indx=(int)((centerX*1000+7.)/(1000*radius));
               indy=(int)((centerY*1000+7.)/(1000*radius));
               indz=(int)((centerZ*1000+15.)/(1000*radius));
               if(indx<0){indx=0;}if(indx>numSearchBox-1){indx=numSearchBox-1;}
               if(indy<0){indy=0;}if(indy>numSearchBox-1){indy=numSearchBox-1;}
               if(indz<0){indz=0;}if(indz>numSearchBox-1){indz=numSearchBox-1;}
               low_x=indx-1;high_x=indx+1;
               low_y=indy-1;high_y=indy+1;
               low_z=indz-1;high_z=indz+1;
               if(low_x<0){low_x=0;}if(low_y<0){low_y=0;}if(low_z<0){low_z=0;}
               if(high_x>numSearchBox-1){high_x=numSearchBox-1;}if(high_y>numSearchBox-1){high_y=numSearchBox-1;}if(high_z>numSearchBox-1){high_z=numSearchBox-1;}
               for(indx=low_x;indx<high_x+1;indx++){
                   for(indy=low_y;indy<high_y+1;indy++){
                       for(indz=low_z;indz<high_z+1;indz++){
                           if(time<200){
                              for(int i=1;i<fast_search[time][indx][indy][indz][0]+1;i++){
                                  dist=pow( pow(point[time][fast_search[time][indx][indy][indz][i]][2]-centerX,2.)
                                       +pow(point[time][fast_search[time][indx][indy][indz][i]][3]-centerY,2.)
                                       +pow(point[time][fast_search[time][indx][indy][indz][i]][4]-centerZ,2.),0.5);
                                  if(dist<maxRadiusSpat && !(dist==falseDist)){
                                     minDistB[indexB]=dist;
                                     minDistBIndex[indexB]=fast_search[time][indx][indy][indz][i];
                                     indexB++;
                                  }
                              }
                           }
                       }
                   }
               }

               //CCCCCCCCCCCCCCCCCCCCC
               for(int dt=-pm;dt<pm+1;dt++){
                  indexC[dt+pm]=0;
                  falseDist=pow(pow(centerX,2.)+pow(centerY,2.)+pow(centerZ,2.),0.5);
                  indx=(int)((centerX*1000+7.)/(1000*radius));
                  indy=(int)((centerY*1000+7.)/(1000*radius));
                  indz=(int)((centerZ*1000+15.)/(1000*radius));
                  if(indx<0){indx=0;}if(indx>numSearchBox-1){indx=numSearchBox-1;}
                  if(indy<0){indy=0;}if(indy>numSearchBox-1){indy=numSearchBox-1;}
                  if(indz<0){indz=0;}if(indz>numSearchBox-1){indz=numSearchBox-1;}
                  low_x=indx-1;high_x=indx+1;
                  low_y=indy-1;high_y=indy+1;
                  low_z=indz-1;high_z=indz+1;
                  if(low_x<0){low_x=0;}if(low_y<0){low_y=0;}if(low_z<0){low_z=0;}
                  if(high_x>numSearchBox-1){high_x=numSearchBox-1;}if(high_y>numSearchBox-1){high_y=numSearchBox-1;}if(high_z>numSearchBox-1){high_z=numSearchBox-1;}
                  for(indx=low_x;indx<high_x+1;indx++){
                      for(indy=low_y;indy<high_y+1;indy++){
                          for(indz=low_z;indz<high_z+1;indz++){
                             if(time+dt<200){
                                for(int i=1;i<fast_search[time+dt][indx][indy][indz][0]+1;i++){
                                    dist=pow( pow(point[time+dt][fast_search[time+dt][indx][indy][indz][i]][2]-centerX,2.)
                                             +pow(point[time+dt][fast_search[time+dt][indx][indy][indz][i]][3]-centerY,2.)
                                             +pow(point[time+dt][fast_search[time+dt][indx][indy][indz][i]][4]-centerZ,2.),0.5);
                                    if(dist<maxRadiusSpat && !(dist==falseDist)){
                                        minDistC[indexC[dt+pm]][dt+pm]=dist;
                                        minDistCIndex[indexC[dt+pm]][dt+pm]=fast_search[time+dt][indx][indy][indz][i];
                                        indexC[dt+pm]++;
                                    }
                                }
                             }
                             else{
                                int strange=1;
                             }
                          }
                      }
                  }
               }


               pCounterB=0;
               int i;
               for(int pointInd=0;pointInd<indexB;pointInd++){
                  i=minDistBIndex[pointInd];
                  if(point[time][i][11]>0. && pCounterB<300){
                     dx=(point[time][i][2]-centerX)/(1.*maxRadiusSpat);
                     dy=(point[time][i][3]-centerY)/(1.*maxRadiusSpat);
                     dz=(point[time][i][4]-centerZ)/(1.*maxRadiusSpat);

                     /// u,v,w component
                     if(mainForm->RadioGroup->ItemIndex==0){
                        for (int compo=1;compo<4;compo++){
                           B[pCounterB][ 0]=Imp(1,1,0,compo,dx,dy,dz);//ip110
                           B[pCounterB][ 1]=Imp(2,1,0,compo,dx,dy,dz);//ip210
                           B[pCounterB][ 2]=Imp(2,2,0,compo,dx,dy,dz);//ip220

                           B[pCounterB][ 3]=Imq(1,1,0,compo,dx,dy,dz);//iq110

                           B[pCounterB][ 4]=Rep(1,0,0,compo,dx,dy,dz);//rp100
                           B[pCounterB][ 5]=Rep(1,1,0,compo,dx,dy,dz);//rp110
                           B[pCounterB][ 6]=Rep(2,0,0,compo,dx,dy,dz);//rp200
                           B[pCounterB][ 7]=Rep(2,1,0,compo,dx,dy,dz);//rp210
                           B[pCounterB][ 8]=Rep(2,2,0,compo,dx,dy,dz);//rp220

                           B[pCounterB][ 9]=Req(1,0,0,compo,dx,dy,dz);//rq100
                           B[pCounterB][10]=Req(1,1,0,compo,dx,dy,dz);//rq110

                           YuB[pCounterB]=point[time][i][4+compo]/(1.*maxRadiusSpat);
                           pCounterB++;
                        }
                     }
                     else{
                        for (int compo=1;compo<4;compo++){
                           B[pCounterB][ 0]=Imp(1,1,0,compo,dx,dy,dz);//ip110
                           B[pCounterB][ 1]=Imp(1,1,1,compo,dx,dy,dz);//ip111
                           B[pCounterB][ 2]=Imp(2,1,0,compo,dx,dy,dz);//ip210
                           B[pCounterB][ 3]=Imp(2,1,1,compo,dx,dy,dz);//ip211
                           B[pCounterB][ 4]=Imp(2,2,0,compo,dx,dy,dz);//ip220
                           B[pCounterB][ 5]=Imp(2,2,1,compo,dx,dy,dz);//ip221
                           B[pCounterB][ 6]=Imp(3,1,0,compo,dx,dy,dz);//ip310
                           B[pCounterB][ 7]=Imp(3,2,0,compo,dx,dy,dz);//ip320
                           B[pCounterB][ 8]=Imp(3,3,0,compo,dx,dy,dz);//ip330
                           B[pCounterB][ 9]=Imp(4,1,0,compo,dx,dy,dz);//ip410
                           B[pCounterB][10]=Imp(4,2,0,compo,dx,dy,dz);//ip420
                           B[pCounterB][11]=Imp(4,3,0,compo,dx,dy,dz);//ip430
                           B[pCounterB][12]=Imp(4,4,0,compo,dx,dy,dz);//ip440

                           B[pCounterB][13]=Imq(1,1,0,compo,dx,dy,dz);//iq110
                           B[pCounterB][14]=Imq(1,1,1,compo,dx,dy,dz);//iq111
                           B[pCounterB][15]=Imq(2,1,0,compo,dx,dy,dz);//iq210
                           B[pCounterB][16]=Imq(2,2,0,compo,dx,dy,dz);//iq220
                           B[pCounterB][17]=Imq(3,1,0,compo,dx,dy,dz);//iq310
                           B[pCounterB][18]=Imq(3,2,0,compo,dx,dy,dz);//iq320
                           B[pCounterB][19]=Imq(3,3,0,compo,dx,dy,dz);//iq330

                           B[pCounterB][20]=Rep(1,0,0,compo,dx,dy,dz);//rp100
                           B[pCounterB][21]=Rep(1,0,1,compo,dx,dy,dz);//rp101
                           B[pCounterB][22]=Rep(1,1,0,compo,dx,dy,dz);//rp110
                           B[pCounterB][23]=Rep(1,1,1,compo,dx,dy,dz);//rp111
                           B[pCounterB][24]=Rep(2,0,0,compo,dx,dy,dz);//rp200
                           B[pCounterB][25]=Rep(2,0,1,compo,dx,dy,dz);//rp201
                           B[pCounterB][26]=Rep(2,1,0,compo,dx,dy,dz);//rp210
                           B[pCounterB][27]=Rep(2,1,1,compo,dx,dy,dz);//rp211
                           B[pCounterB][28]=Rep(2,2,0,compo,dx,dy,dz);//rp220
                           B[pCounterB][29]=Rep(2,2,1,compo,dx,dy,dz);//rp221

                           B[pCounterB][30]=Rep(3,0,0,compo,dx,dy,dz);//rp300
                           B[pCounterB][31]=Rep(3,1,0,compo,dx,dy,dz);//rp310
                           B[pCounterB][32]=Rep(3,2,0,compo,dx,dy,dz);//rp320
                           B[pCounterB][33]=Rep(3,3,0,compo,dx,dy,dz);//rp330
                           B[pCounterB][34]=Rep(4,0,0,compo,dx,dy,dz);//rp400
                           B[pCounterB][35]=Rep(4,1,0,compo,dx,dy,dz);//rp410
                           B[pCounterB][36]=Rep(4,2,0,compo,dx,dy,dz);//rp420
                           B[pCounterB][37]=Rep(4,3,0,compo,dx,dy,dz);//rp430
                           B[pCounterB][38]=Rep(4,4,0,compo,dx,dy,dz);//rp440

                           B[pCounterB][39]=Req(1,0,0,compo,dx,dy,dz);//rq100
                           B[pCounterB][40]=Req(1,0,1,compo,dx,dy,dz);//rq101
                           B[pCounterB][41]=Req(1,1,0,compo,dx,dy,dz);//rq110
                           B[pCounterB][42]=Req(1,1,1,compo,dx,dy,dz);//rq111
                           B[pCounterB][43]=Req(2,0,0,compo,dx,dy,dz);//rq200
                           B[pCounterB][44]=Req(2,1,0,compo,dx,dy,dz);//rq210
                           B[pCounterB][45]=Req(2,2,0,compo,dx,dy,dz);//rq220

                           B[pCounterB][46]=Req(3,0,0,compo,dx,dy,dz);//rq300
                           B[pCounterB][47]=Req(3,1,0,compo,dx,dy,dz);//rq310
                           B[pCounterB][48]=Req(3,2,0,compo,dx,dy,dz);//rq320
                           B[pCounterB][49]=Req(3,3,0,compo,dx,dy,dz);//rq330

                           YuB[pCounterB]=point[time][i][4+compo]/(1.*maxRadiusSpat);
                           pCounterB++;
                        }
                     }
                  }
               }
               for(int dt=-pm;dt<pm+1;dt++){
                  pCounterC[dt+pm]=0;
                  for(int pointInd=0;pointInd<indexC[dt+pm];pointInd++){
                     i=minDistCIndex[pointInd][dt+pm];
                     if(point[time+dt][i][11]>0. && pCounterC[dt+pm]<300){
                        dx=(point[time+dt][i][2]-centerX)/(1.*maxRadiusSpat);
                        dy=(point[time+dt][i][3]-centerY)/(1.*maxRadiusSpat);
                        dz=(point[time+dt][i][4]-centerZ)/(1.*maxRadiusSpat);

                        /// u,v,w component
                        if(mainForm->RadioGroup->ItemIndex==0){
                           for (int compo=1;compo<4;compo++){
                              C[pCounterC[dt+pm]][ 0][dt+pm]=Imp(1,1,0,compo,dx,dy,dz);//ip110
                              C[pCounterC[dt+pm]][ 1][dt+pm]=Imp(2,1,0,compo,dx,dy,dz);//ip210
                              C[pCounterC[dt+pm]][ 2][dt+pm]=Imp(2,2,0,compo,dx,dy,dz);//ip220

                              C[pCounterC[dt+pm]][ 3][dt+pm]=Imq(1,1,0,compo,dx,dy,dz);//iq110

                              C[pCounterC[dt+pm]][ 4][dt+pm]=Rep(1,0,0,compo,dx,dy,dz);//rp100
                              C[pCounterC[dt+pm]][ 5][dt+pm]=Rep(1,1,0,compo,dx,dy,dz);//rp110
                              C[pCounterC[dt+pm]][ 6][dt+pm]=Rep(2,0,0,compo,dx,dy,dz);//rp200
                              C[pCounterC[dt+pm]][ 7][dt+pm]=Rep(2,1,0,compo,dx,dy,dz);//rp210
                              C[pCounterC[dt+pm]][ 8][dt+pm]=Rep(2,2,0,compo,dx,dy,dz);//rp220

                              C[pCounterC[dt+pm]][ 9][dt+pm]=Req(1,0,0,compo,dx,dy,dz);//rq100
                              C[pCounterC[dt+pm]][10][dt+pm]=Req(1,1,0,compo,dx,dy,dz);//rq110

                              YuC[pCounterC[dt+pm]][dt+pm]=point[time+dt][i][4+compo]/(1.*maxRadiusSpat);
                              pCounterC[dt+pm]++;
                           }
                        }
                        else{
                           for (int compo=1;compo<4;compo++){
                              C[pCounterC[dt+pm]][ 0][dt+pm]=Imp(1,1,0,compo,dx,dy,dz);//ip110
                              C[pCounterC[dt+pm]][ 1][dt+pm]=Imp(1,1,1,compo,dx,dy,dz);//ip111
                              C[pCounterC[dt+pm]][ 2][dt+pm]=Imp(2,1,0,compo,dx,dy,dz);//ip210
                              C[pCounterC[dt+pm]][ 3][dt+pm]=Imp(2,1,1,compo,dx,dy,dz);//ip211
                              C[pCounterC[dt+pm]][ 4][dt+pm]=Imp(2,2,0,compo,dx,dy,dz);//ip220
                              C[pCounterC[dt+pm]][ 5][dt+pm]=Imp(2,2,1,compo,dx,dy,dz);//ip221
                              C[pCounterC[dt+pm]][ 6][dt+pm]=Imp(3,1,0,compo,dx,dy,dz);//ip310
                              C[pCounterC[dt+pm]][ 7][dt+pm]=Imp(3,2,0,compo,dx,dy,dz);//ip320
                              C[pCounterC[dt+pm]][ 8][dt+pm]=Imp(3,3,0,compo,dx,dy,dz);//ip330
                              C[pCounterC[dt+pm]][ 9][dt+pm]=Imp(4,1,0,compo,dx,dy,dz);//ip410
                              C[pCounterC[dt+pm]][10][dt+pm]=Imp(4,2,0,compo,dx,dy,dz);//ip420
                              C[pCounterC[dt+pm]][11][dt+pm]=Imp(4,3,0,compo,dx,dy,dz);//ip430
                              C[pCounterC[dt+pm]][12][dt+pm]=Imp(4,4,0,compo,dx,dy,dz);//ip440

                              C[pCounterC[dt+pm]][13][dt+pm]=Imq(1,1,0,compo,dx,dy,dz);//iq110
                              C[pCounterC[dt+pm]][14][dt+pm]=Imq(1,1,1,compo,dx,dy,dz);//iq111
                              C[pCounterC[dt+pm]][15][dt+pm]=Imq(2,1,0,compo,dx,dy,dz);//iq210
                              C[pCounterC[dt+pm]][16][dt+pm]=Imq(2,2,0,compo,dx,dy,dz);//iq220
                              C[pCounterC[dt+pm]][17][dt+pm]=Imq(3,1,0,compo,dx,dy,dz);//iq310
                              C[pCounterC[dt+pm]][18][dt+pm]=Imq(3,2,0,compo,dx,dy,dz);//iq320
                              C[pCounterC[dt+pm]][19][dt+pm]=Imq(3,3,0,compo,dx,dy,dz);//iq330

                              C[pCounterC[dt+pm]][20][dt+pm]=Rep(1,0,0,compo,dx,dy,dz);//rp100
                              C[pCounterC[dt+pm]][21][dt+pm]=Rep(1,0,1,compo,dx,dy,dz);//rp101
                              C[pCounterC[dt+pm]][22][dt+pm]=Rep(1,1,0,compo,dx,dy,dz);//rp110
                              C[pCounterC[dt+pm]][23][dt+pm]=Rep(1,1,1,compo,dx,dy,dz);//rp111
                              C[pCounterC[dt+pm]][24][dt+pm]=Rep(2,0,0,compo,dx,dy,dz);//rp200
                              C[pCounterC[dt+pm]][25][dt+pm]=Rep(2,0,1,compo,dx,dy,dz);//rp201
                              C[pCounterC[dt+pm]][26][dt+pm]=Rep(2,1,0,compo,dx,dy,dz);//rp210
                              C[pCounterC[dt+pm]][27][dt+pm]=Rep(2,1,1,compo,dx,dy,dz);//rp211
                              C[pCounterC[dt+pm]][28][dt+pm]=Rep(2,2,0,compo,dx,dy,dz);//rp220
                              C[pCounterC[dt+pm]][29][dt+pm]=Rep(2,2,1,compo,dx,dy,dz);//rp221

                              C[pCounterC[dt+pm]][30][dt+pm]=Rep(3,0,0,compo,dx,dy,dz);//rp300
                              C[pCounterC[dt+pm]][31][dt+pm]=Rep(3,1,0,compo,dx,dy,dz);//rp310
                              C[pCounterC[dt+pm]][32][dt+pm]=Rep(3,2,0,compo,dx,dy,dz);//rp320
                              C[pCounterC[dt+pm]][33][dt+pm]=Rep(3,3,0,compo,dx,dy,dz);//rp330
                              C[pCounterC[dt+pm]][34][dt+pm]=Rep(4,0,0,compo,dx,dy,dz);//rp400
                              C[pCounterC[dt+pm]][35][dt+pm]=Rep(4,1,0,compo,dx,dy,dz);//rp410
                              C[pCounterC[dt+pm]][36][dt+pm]=Rep(4,2,0,compo,dx,dy,dz);//rp420
                              C[pCounterC[dt+pm]][37][dt+pm]=Rep(4,3,0,compo,dx,dy,dz);//rp430
                              C[pCounterC[dt+pm]][38][dt+pm]=Rep(4,4,0,compo,dx,dy,dz);//rp440

                              C[pCounterC[dt+pm]][39][dt+pm]=Req(1,0,0,compo,dx,dy,dz);//rq100
                              C[pCounterC[dt+pm]][40][dt+pm]=Req(1,0,1,compo,dx,dy,dz);//rq101
                              C[pCounterC[dt+pm]][41][dt+pm]=Req(1,1,0,compo,dx,dy,dz);//rq110
                              C[pCounterC[dt+pm]][42][dt+pm]=Req(1,1,1,compo,dx,dy,dz);//rq111
                              C[pCounterC[dt+pm]][43][dt+pm]=Req(2,0,0,compo,dx,dy,dz);//rq200
                              C[pCounterC[dt+pm]][44][dt+pm]=Req(2,1,0,compo,dx,dy,dz);//rq210
                              C[pCounterC[dt+pm]][45][dt+pm]=Req(2,2,0,compo,dx,dy,dz);//rq220

                              C[pCounterC[dt+pm]][46][dt+pm]=Req(3,0,0,compo,dx,dy,dz);//rq300
                              C[pCounterC[dt+pm]][47][dt+pm]=Req(3,1,0,compo,dx,dy,dz);//rq310
                              C[pCounterC[dt+pm]][48][dt+pm]=Req(3,2,0,compo,dx,dy,dz);//rq320
                              C[pCounterC[dt+pm]][49][dt+pm]=Req(3,3,0,compo,dx,dy,dz);//rq330

                              YuC[pCounterC[dt+pm]][dt+pm]=point[time+dt][i][4+compo]/(1.*maxRadiusSpat);
                              pCounterC[dt+pm]++;
                           }
                        }
                     }
                  }
               }

               meanPointsInSphereB=(meanPointsInSphereB*(double)(count-1)+(double)pCounterB)/(double)count;
               meanPointsInSphere =(meanPointsInSphere *(double)(count-1)+(double)pCounterC[0])/(double)count;
               if(mainForm->RadioGroup->ItemIndex==0){
                  minCounter=5;
               }
               else{
                  minCounter=50;
               }

               if(pCounterB>minCounter){
                  if(mainForm->RadioGroup->ItemIndex==0){
                     makeBT(pCounterB,11);
                     makeBTB(pCounterB,11);
                     makeBTY(pCounterB,11,1);
                     solveB(pCounterB,11);
                     for(int j=0;j<11;j++){
                        traj[numInTraj][j]=X[j];
                     }
                  }
                  else{
                     makeBT(pCounterB,50);
                     makeBTB(pCounterB,50);
                     makeBTY(pCounterB,50,1);
                     solveB(pCounterB,50);
                     for(int j=0;j<50;j++){
                        traj[numInTraj][j]=X[j];
                     }
                  }

                  //here we put the local derivatives.
                  for(int dt=-pm;dt<pm+1;dt++){
                      if(pCounterC[dt+pm]>minCounter){
                         if(mainForm->RadioGroup->ItemIndex==0){
                            makeCT(pCounterC[dt+pm],11,dt+pm);
                            makeCTC(pCounterC[dt+pm],11,dt+pm);
                            makeCTY(pCounterC[dt+pm],11,1,dt+pm);
                            solveC(pCounterC[dt+pm],11);
                            u_local[dt+pm]=(-0.3454941494713355*X[5])*maxRadiusSpat;
                            v_local[dt+pm]=(-0.3454941494713355*X[0])*maxRadiusSpat;
                            w_local[dt+pm]=( 0.4886025119029199*X[4])*maxRadiusSpat;
                         }
                         else{
                            makeCT(pCounterC[dt+pm],50,dt+pm);
                            makeCTC(pCounterC[dt+pm],50,dt+pm);
                            makeCTY(pCounterC[dt+pm],50,1,dt+pm);
                            solveC(pCounterC[dt+pm],50);
                            u_local[dt+pm]=(-0.24430125595145996*(1.4142135623730951*X[22] + 2.6457513110645907*X[23]))*maxRadiusSpat;
                            v_local[dt+pm]=(-0.24430125595145996*(1.4142135623730951*X[0] + 2.6457513110645907*X[1]))*maxRadiusSpat;
                            w_local[dt+pm]=( 0.24430125595145996*(2.*X[20] + 3.7416573867739413*X[21]))*maxRadiusSpat;
                         }
                      }
                  }
                  for(int j=-pm;j<pm+1;j++){
                     for(int ij=0;ij<order;ij++){////////////ORDER
                        if(u_local[j+pm]==0){
                           A[j+pm][ij]=0.;
                        }
                        else{
                           A[j+pm][ij]=pow((double)j*deltaT+0.000000001,(double)(ij));
                        }
                     }
                     if(u_local[j+pm]==0){
                        y[0][j+pm]=0;
                        y[1][j+pm]=0;
                        y[2][j+pm]=0;
                     }
                     else{
                        y[0][j+pm]=u_local[j+pm];
                        y[1][j+pm]=v_local[j+pm];
                        y[2][j+pm]=w_local[j+pm];
                     }
                  }

                  for(int j=0;j<3;j++){
                     if(pCounterC[pm]>0){
                        makeAT(2*pm+1,order); ////////////ORDER
                        makeATA(2*pm+1,order);////////////ORDER
                        makeATY(2*pm+1,order,j); ////////////ORDER
                        solve(2*pm+1,order);////////////ORDER
                        if(mainForm->RadioGroup->ItemIndex==0){
                           traj[numInTraj][11+j]=X[1];
                        }
                        else{
                           traj[numInTraj][50+j]=X[1];
                        }
                     }
                     else{
                        if(mainForm->RadioGroup->ItemIndex==0){
                           traj[numInTraj][11+j]=10000.;
                        }
                        else{
                           traj[numInTraj][50+j]=10000.;
                        }
                     }
                  }

                  traj[numInTraj][53]=point[time][n][2];//x
                  traj[numInTraj][54]=point[time][n][3];//y
                  traj[numInTraj][55]=point[time][n][4];//z
                  traj[numInTraj][56]=point[time][n][5];//u
                  traj[numInTraj][57]=point[time][n][6];//v
                  traj[numInTraj][58]=point[time][n][7];//w
                  traj[numInTraj][14]=point[time][n][8];//ax
                  traj[numInTraj][15]=point[time][n][9];//ay
                  traj[numInTraj][16]=point[time][n][10];//az

                  //////////////////////////for quality
                  if(mainForm->RadioGroup->ItemIndex==0){
                     up[numInTraj] =traj[numInTraj][56];vp[numInTraj] =traj[numInTraj][57];wp[numInTraj] =traj[numInTraj][58];
                     axp[numInTraj]=traj[numInTraj][14];ayp[numInTraj]=traj[numInTraj][15];azp[numInTraj]=traj[numInTraj][16];

                     dudxp[numInTraj]=0.31539156525252005*(-1.4142135623730951*traj[numInTraj][6]+1.7320508075688772*traj[numInTraj][8]);
                     dudyp[numInTraj]=0.5462742152960396*(traj[numInTraj][2] + 1.4142135623730951*traj[numInTraj][9]);
                     dudzp[numInTraj]=0.5462742152960396*(traj[numInTraj][3] - 1.*traj[numInTraj][7]);
                     dvdxp[numInTraj]=0.5462742152960396*(traj[numInTraj][2] - 1.4142135623730951*traj[numInTraj][9]);
                     dvdyp[numInTraj]=-0.31539156525252005*(1.4142135623730951*traj[numInTraj][6] + 1.7320508075688772*traj[numInTraj][8]);
                     dvdzp[numInTraj]=-0.5462742152960396*(traj[numInTraj][1] + traj[numInTraj][10]);
                     dwdxp[numInTraj]=-0.5462742152960396*(traj[numInTraj][3] + traj[numInTraj][7]);
                     dwdyp[numInTraj]=0.5462742152960396*(-1.*traj[numInTraj][1] + traj[numInTraj][10]);
                     dwdzp[numInTraj]=0.8920620580763856*traj[numInTraj][6];

                     dudtp[numInTraj]=traj[numInTraj][11];
                     dvdtp[numInTraj]=traj[numInTraj][12];
                     dwdtp[numInTraj]=traj[numInTraj][13];
                  }
                  else{
                     up[numInTraj] =traj[numInTraj][56];vp[numInTraj] =traj[numInTraj][57];wp[numInTraj] =traj[numInTraj][58];
                     axp[numInTraj]=traj[numInTraj][59];ayp[numInTraj]=traj[numInTraj][60];azp[numInTraj]=traj[numInTraj][61];

                     dudxp[numInTraj]=0.03526184897173477*(-12.649110640673518*traj[numInTraj][24] - 32.863353450309965*traj[numInTraj][25] \
                               + 15.491933384829668*traj[numInTraj][28] + 40.24922359499622*traj[numInTraj][29]);
                     dudyp[numInTraj]=0.03526184897173477*(15.491933384829668*traj[numInTraj][4] + 40.24922359499622*traj[numInTraj][5] + \
                            21.908902300206645*traj[numInTraj][39] - 73.48469228349533*traj[numInTraj][40]);
                     dudzp[numInTraj]=0.03526184897173477*(15.491933384829668*traj[numInTraj][13] - 51.96152422706631*traj[numInTraj][14] - \
                            15.491933384829668*traj[numInTraj][26] - 40.24922359499622*traj[numInTraj][27]);
                     dvdxp[numInTraj]=0.03526184897173477*(15.491933384829668*traj[numInTraj][4] + 40.24922359499622*traj[numInTraj][5] \
                            - 21.908902300206645*traj[numInTraj][39] + 73.48469228349533*traj[numInTraj][40]);
                     dvdyp[numInTraj]=0.03526184897173477*(-12.649110640673518*traj[numInTraj][24] - 32.863353450309965*traj[numInTraj][25] \
                            - 15.491933384829668*traj[numInTraj][28] - 40.24922359499622*traj[numInTraj][29]);
                     dvdzp[numInTraj]=0.03526184897173477*(-15.491933384829668*traj[numInTraj][2] - 40.24922359499622*traj[numInTraj][3] \
                            - 15.491933384829668*traj[numInTraj][41] + 51.96152422706631*traj[numInTraj][42]);
                     dwdxp[numInTraj]=0.03526184897173477*(-15.491933384829668*traj[numInTraj][13] + 51.96152422706631*traj[numInTraj][14] \
                            - 15.491933384829668*traj[numInTraj][26] - 40.24922359499622*traj[numInTraj][27]);
                     dwdyp[numInTraj]=0.03526184897173477*(-15.491933384829668*traj[numInTraj][2] - 40.24922359499622*traj[numInTraj][3] \
                            + 15.491933384829668*traj[numInTraj][41] - 51.96152422706631*traj[numInTraj][42]);
                     dwdzp[numInTraj]=0.07052369794346953*(12.649110640673518*traj[numInTraj][24] + 32.863353450309965*traj[numInTraj][25]);

                     dudtp[numInTraj]=traj[numInTraj][50];
                     dvdtp[numInTraj]=traj[numInTraj][51];
                     dwdtp[numInTraj]=traj[numInTraj][52];
                  }
                  dix=axp[numInTraj]-dudtp[numInTraj]-up[numInTraj]*dudxp[numInTraj]-vp[numInTraj]*dudyp[numInTraj]-wp[numInTraj]*dudzp[numInTraj];
                  diy=ayp[numInTraj]-dvdtp[numInTraj]-up[numInTraj]*dvdxp[numInTraj]-vp[numInTraj]*dvdyp[numInTraj]-wp[numInTraj]*dvdzp[numInTraj];
                  diz=azp[numInTraj]-dwdtp[numInTraj]-up[numInTraj]*dwdxp[numInTraj]-vp[numInTraj]*dwdyp[numInTraj]-wp[numInTraj]*dwdzp[numInTraj];
                  refx= fabs(axp[numInTraj])
                          +fabs(dudtp[numInTraj])
                          +fabs( up[numInTraj]*dudxp[numInTraj]
                                +vp[numInTraj]*dudyp[numInTraj]
                                +wp[numInTraj]*dudzp[numInTraj]);
                  refy= fabs(ayp[numInTraj])
                          +fabs(dvdtp[numInTraj])
                          +fabs( up[numInTraj]*dvdxp[numInTraj]
                                +vp[numInTraj]*dvdyp[numInTraj]
                                +wp[numInTraj]*dvdzp[numInTraj]);
                  refz= fabs(azp[numInTraj])
                          +fabs(dwdtp[numInTraj])
                          +fabs( up[numInTraj]*dwdxp[numInTraj]
                                +vp[numInTraj]*dwdyp[numInTraj]
                                +wp[numInTraj]*dwdzp[numInTraj]);
                  if(refx>0 && refy>0 && refz>0){
                     traj[numInTraj][62]=(1./3.)*(fabs(dix)/refx+fabs(diy)/refy+fabs(diz)/refz);
                  }
                  else{
                     traj[numInTraj][62]=0.95;
                  }
                  if(traj[numInTraj][62]>0.95){traj[numInTraj][62]=0.95;}
                  //////////////////////////end for quality

               }// end of if pCOunter>3 solve...
               if(!(pCounterB>minCounter )){
                  for(int j=0;j<62;j++){
                      traj[numInTraj][j]=0.;
                  }
                  traj[numInTraj][62]=0.95;
                  noDeriv++;
               }
               numInTraj++;
               if(traj[numInTraj][62]<0.1){
                  count7++;
               }
               count8++;

               //schauen ob's einen nächsten gibt
               if(point[time][n][1]>0 && time<lastFile-firstFile-1){
                   n=point[time][n][1];
                   time++;
                   if( point[time][n][11]==0 || time>197){
                       ok=false;
                   }
               }
               else{
                  ok=false;
               }
            }//end while ok
            //numInTraj++;
            count3++;
            mainForm->meanSuccessDivEdit->Text=IntToStr((int)(100.*(double)pointList->count7/(double)pointList->count8+0.5));
            succIntTraj=(succIntTraj*(double)(count3-1)+(1-(double)noDeriv/(double)numInTraj)*100.)/(double)count3;
            if(numInTraj-noDeriv>minTrajLength-1){   //Wichtig
               /////polynom business////////////////////////////////////////

               su=0.;
               for(int ii=pm;ii<numInTraj-pm;ii++){
                  su=su+(1.-traj[ii][62]);//success=0, bad=1;
               }
               //int orderA=(int)(su/35.+3.5);
               int orderA=(int)(su/polyConst+3.5);

               count6++;
               forMeanNumInTraj=forMeanNumInTraj+numInTraj;
               forEffectiveMeanNumInTraj=forEffectiveMeanNumInTraj+su;
               forTypicalPolyOrder=forTypicalPolyOrder+numInTraj*orderA;

               trajForm->polyOrderEdit->Text=IntToStr(orderA);
               trajForm->polyOrderEdit->Refresh();

               for(int ii=0;ii<numInTraj;ii++){
                  we[ii]=1.-1./(1.+exp(-c1*(traj[ii][62]-c2)));//quality(ii)
               }

               for(int ii=pm;ii<numInTraj-pm;ii++){
                  for(int ij=0;ij<orderA;ij++){
                     A[ii-pm][ij]=we[ii]*pow((double)(ii-pm)*deltaT+0.000000001,(double)(ij));
                  }
                  if(mainForm->RadioGroup->ItemIndex==0){
                     for(int jj=0;jj<17;jj++){
                         y[jj][ii-pm]=we[ii]*traj[ii][jj];
                     }
                  }
                  else{
                     for(int jj=0;jj<53;jj++){
                         y[jj][ii-pm]=we[ii]*traj[ii][jj];
                     }
                  }

               }
               if(mainForm->RadioGroup->ItemIndex==0){
                  for(int jj=0;jj<17;jj++){
                     makeAT(numInTraj-2*pm,orderA);
                     makeATA(numInTraj-2*pm,orderA);
                     makeATY(numInTraj-2*pm,orderA,jj);
                     solve(numInTraj-2*pm,orderA);
                     for(int ii=0;ii<orderA;ii++){
                        x[jj][ii]=X[ii];
                     }
                  }
               }
               else{
                  for(int jj=0;jj<53;jj++){
                     makeAT(numInTraj-2*pm,orderA);
                     makeATA(numInTraj-2*pm,orderA);
                     makeATY(numInTraj-2*pm,orderA,jj);
                     solve(numInTraj-2*pm,orderA);
                     for(int ii=0;ii<orderA;ii++){
                        x[jj][ii]=X[ii];
                     }
                  }
               }

               for(int ii=0;ii<numInTraj-2*pm;ii++){
                  for(int jj=0;jj<53;jj++){
                     pC[jj][ii]=0;
                  }
                  xp[ii]=0;yp[ii]=0;zp[ii]=0;
                  up[ii]=0;vp[ii]=0;wp[ii]=0;
                  axp[ii]=0;ayp[ii]=0;azp[ii]=0;
                  dudxp[ii]=0;dudyp[ii]=0;dudzp[ii]=0;
                  dvdxp[ii]=0;dvdyp[ii]=0;dvdzp[ii]=0;
                  dwdxp[ii]=0;dwdyp[ii]=0;dwdzp[ii]=0;
                  dudtp[ii]=0;dvdtp[ii]=0;dwdtp[ii]=0;
                  NXp[ii]=0;NYp[ii]=0;NZp[ii]=0;
                  daxdxp[ii]=0;daxdyp[ii]=0;daxdzp[ii]=0;
                  daydxp[ii]=0;daydyp[ii]=0;daydzp[ii]=0;
                  dazdxp[ii]=0;dazdyp[ii]=0;dazdzp[ii]=0;

                  if(mainForm->RadioGroup->ItemIndex==0){
                     for(int jj=0;jj<17;jj++){
                        for(int ij=0;ij<orderA;ij++){
                           pC[jj][ii]=pC[jj][ii]+x[jj][ij]*pow((double)ii*deltaT+1e-10,(double)(ij));
                        }
                        //pC[jj][ii]=traj[ii][jj];
                     }
                  }
                  else{
                     for(int jj=0;jj<53;jj++){
                        for(int ij=0;ij<orderA;ij++){
                           pC[jj][ii]=pC[jj][ii]+x[jj][ij]*pow((double)ii*deltaT+1e-10,(double)(ij));
                        }
                        //pC[jj][ii]=traj[ii][jj];
                     }
                  }

                  //and now with the smooth poly constants pC we can build u, ux and whatever
                  xp[ii] =traj[ii+2][53];yp[ii] =traj[ii+2][54];zp[ii] =traj[ii+2][55];
                  up[ii] =traj[ii+2][56];vp[ii] =traj[ii+2][57];wp[ii] =traj[ii+2][58];
                  axp[ii]=traj[ii+2][14];ayp[ii]=traj[ii+2][15];azp[ii]=traj[ii+2][16];

                  if(mainForm->RadioGroup->ItemIndex==0){
                     dudxp[ii]=0.31539156525252005*(-1.4142135623730951*pC[6][ii]+1.7320508075688772*pC[8][ii]);
                     dudyp[ii]=0.5462742152960396*(pC[2][ii] + 1.4142135623730951*pC[9][ii]);
                     dudzp[ii]=0.5462742152960396*(pC[3][ii] - 1.*pC[7][ii]);
                     dvdxp[ii]=0.5462742152960396*(pC[2][ii] - 1.4142135623730951*pC[9][ii]);
                     dvdyp[ii]=-0.31539156525252005*(1.4142135623730951*pC[6][ii] + 1.7320508075688772*pC[8][ii]);
                     dvdzp[ii]=-0.5462742152960396*(pC[1][ii] + pC[10][ii]);
                     dwdxp[ii]=-0.5462742152960396*(pC[3][ii] + pC[7][ii]);
                     dwdyp[ii]=0.5462742152960396*(-1.*pC[1][ii] + pC[10][ii]);
                     dwdzp[ii]=0.8920620580763856*pC[6][ii];

                     dudtp[ii]=pC[11][ii];
                     dvdtp[ii]=pC[12][ii];
                     dwdtp[ii]=pC[13][ii];

                     //axp[ii]=pC[14][ii];
                     //ayp[ii]=pC[15][ii];
                     //azp[ii]=pC[16][ii];

                     NXp[ii]=0;
                     NYp[ii]=0;
                     NZp[ii]=0;

                     daxdxp[ii]=0;
                     daxdyp[ii]=0;
                     daxdzp[ii]=0;
                     daydxp[ii]=0;
                     daydyp[ii]=0;
                     daydzp[ii]=0;
                     dazdxp[ii]=0;
                     dazdyp[ii]=0;
                     dazdzp[ii]=0;
                  }
                  else{
                     dudxp[ii]=0.03526184897173477*(-12.649110640673518*pC[24][ii] - 32.863353450309965*pC[25][ii] \
                            + 15.491933384829668*pC[28][ii] + 40.24922359499622*pC[29][ii]);
                     dudyp[ii]=0.03526184897173477*(15.491933384829668*pC[4][ii] + 40.24922359499622*pC[5][ii] + \
                            21.908902300206645*pC[39][ii] - 73.48469228349533*pC[40][ii]);
                     dudzp[ii]=0.03526184897173477*(15.491933384829668*pC[13][ii] - 51.96152422706631*pC[14][ii] - \
                            15.491933384829668*pC[26][ii] - 40.24922359499622*pC[27][ii]);
                     dvdxp[ii]=0.03526184897173477*(15.491933384829668*pC[4][ii] + 40.24922359499622*pC[5][ii] \
                            - 21.908902300206645*pC[39][ii] + 73.48469228349533*pC[40][ii]);
                     dvdyp[ii]=0.03526184897173477*(-12.649110640673518*pC[24][ii] - 32.863353450309965*pC[25][ii] \
                            - 15.491933384829668*pC[28][ii] - 40.24922359499622*pC[29][ii]);
                     dvdzp[ii]=0.03526184897173477*(-15.491933384829668*pC[2][ii] - 40.24922359499622*pC[3][ii] \
                            - 15.491933384829668*pC[41][ii] + 51.96152422706631*pC[42][ii]);
                     dwdxp[ii]=0.03526184897173477*(-15.491933384829668*pC[13][ii] + 51.96152422706631*pC[14][ii] \
                            - 15.491933384829668*pC[26][ii] - 40.24922359499622*pC[27][ii]);
                     dwdyp[ii]=0.03526184897173477*(-15.491933384829668*pC[2][ii] - 40.24922359499622*pC[3][ii] \
                            + 15.491933384829668*pC[41][ii] - 51.96152422706631*pC[42][ii]);
                     dwdzp[ii]=0.07052369794346953*(12.649110640673518*pC[24][ii] + 32.863353450309965*pC[25][ii]);

                     dudtp[ii]=pC[50][ii];
                     dvdtp[ii]=pC[51][ii];
                     dwdtp[ii]=pC[52][ii];

                     NXp[ii]=51.30326374980659*pC[42][ii]/(maxRadiusSpat*maxRadiusSpat);
                     NYp[ii]=51.30326374980659*pC[14][ii]/(maxRadiusSpat*maxRadiusSpat);
                     NZp[ii]=-72.55377138898045*pC[40][ii]/(maxRadiusSpat*maxRadiusSpat);

                     daxdxp[ii]=0;
                     daxdyp[ii]=0;
                     daxdzp[ii]=0;
                     daydxp[ii]=0;
                     daydyp[ii]=0;
                     daydzp[ii]=0;
                     dazdxp[ii]=0;
                     dazdyp[ii]=0;
                     dazdzp[ii]=0;
                  }

               }// end for loop through traj
               
               if(write){
                  for(int ii=0;ii<numInTraj-2*pm;ii++){
                     fprintf(fpp, "%lf\t", xp[ii]);//1
                     fprintf(fpp, "%lf\t", yp[ii]);//2
                     fprintf(fpp, "%lf\t", zp[ii]);//3
                     fprintf(fpp, "%lf\t", up[ii]);//4
                     fprintf(fpp, "%lf\t", vp[ii]);//5
                     fprintf(fpp, "%lf\t", wp[ii]);//6
                     fprintf(fpp, "%lf\t", axp[ii]);//7
                     fprintf(fpp, "%lf\t", ayp[ii]);//8
                     fprintf(fpp, "%lf\t", azp[ii]);//9
                     
                     fprintf(fpp, "%lf\t", dudxp[ii]);//10
                     fprintf(fpp, "%lf\t", dudyp[ii]);//11
                     fprintf(fpp, "%lf\t", dudzp[ii]);//12
                     fprintf(fpp, "%lf\t", dvdxp[ii]);//13
                     fprintf(fpp, "%lf\t", dvdyp[ii]);//14
                     fprintf(fpp, "%lf\t", dvdzp[ii]);//15
                     fprintf(fpp, "%lf\t", dwdxp[ii]);//16
                     fprintf(fpp, "%lf\t", dwdyp[ii]);//17
                     fprintf(fpp, "%lf\t", dwdzp[ii]);//18
                     fprintf(fpp, "%lf\t", dudtp[ii]);//19
                     fprintf(fpp, "%lf\t", dvdtp[ii]);//20
                     fprintf(fpp, "%lf\t", dwdtp[ii]);//21

                     fprintf(fpp, "%lf\t", daxdxp[ii]);//22
                     fprintf(fpp, "%lf\t", daxdyp[ii]);//23
                     fprintf(fpp, "%lf\t", daxdzp[ii]);//24
                     fprintf(fpp, "%lf\t", daydxp[ii]);//25
                     fprintf(fpp, "%lf\t", daydyp[ii]);//26
                     fprintf(fpp, "%lf\t", daydzp[ii]);//27
                     fprintf(fpp, "%lf\t", dazdxp[ii]);//28
                     fprintf(fpp, "%lf\t", dazdyp[ii]);//29
                     fprintf(fpp, "%lf\t", dazdzp[ii]);//30

                     fprintf(fpp, "%lf\t", NXp[ii]);//31
                     fprintf(fpp, "%lf\t", NYp[ii]);//32
                     fprintf(fpp, "%lf\t", NZp[ii]);//33

                     fprintf(fpp, "%lf\n", (double)(ii));//34

                     //for weighting acceleration
                     //acceleration quality
                     dix=axp[ii]-dudtp[ii]-up[ii]*dudxp[ii]-vp[ii]*dudyp[ii]-wp[ii]*dudzp[ii];
                     diy=ayp[ii]-dvdtp[ii]-up[ii]*dvdxp[ii]-vp[ii]*dvdyp[ii]-wp[ii]*dvdzp[ii];
                     diz=azp[ii]-dwdtp[ii]-up[ii]*dwdxp[ii]-vp[ii]*dwdyp[ii]-wp[ii]*dwdzp[ii];
                     refx= fabs(axp[ii])
                          +fabs(dudtp[ii])
                          +fabs( up[ii]*dudxp[ii]
                                +vp[ii]*dudyp[ii]
                                +wp[ii]*dudzp[ii]);
                     refy= fabs(ayp[ii])
                          +fabs(dvdtp[ii])
                          +fabs( up[ii]*dvdxp[ii]
                                +vp[ii]*dvdyp[ii]
                                +wp[ii]*dvdzp[ii]);
                     refz= fabs(azp[ii])
                          +fabs(dwdtp[ii])
                          +fabs( up[ii]*dwdxp[ii]
                                +vp[ii]*dwdyp[ii]
                                +wp[ii]*dwdzp[ii]);
                     if(refx>0 && refy>0 && refz>0){
                        accCriteria=(1./3.)*(fabs(dix)/refx+fabs(diy)/refy+fabs(diz)/refz);
                     }
                     else{
                        accCriteria=0.95;
                     }
                     if(accCriteria<0.1){
                        count4++;
                     }
                     count9++;

                     //diva quality
                     w1=dwdyp[ii]-dvdzp[ii];
                     w2=dudzp[ii]-dwdxp[ii];
                     w3=dvdxp[ii]-dudyp[ii];
                     enstrophy=w1*w1+w2*w2+w3*w3;
                     s11=dudxp[ii];
                     s22=dvdyp[ii];
                     s33=dwdzp[ii];
                     s12=0.5*(dudyp[ii]+dvdxp[ii]);
                     s13=0.5*(dudzp[ii]+dwdxp[ii]);
                     s23=0.5*(dvdzp[ii]+dwdyp[ii]);
                     twosijsij=2.*(s11*s11+s22*s22+s33*s33
                                   +2.*(s12*s12+s13*s13+s23*s23)
                                   );
                     Q=0.25*(enstrophy-twosijsij);

                     if(ii>0 && ii<numInTraj-1){
                        as11=0.5*(daxdxp[ii]+daxdxp[ii]);
                        as22=0.5*(daydyp[ii]+daydyp[ii]);
                        as33=0.5*(dazdzp[ii]+dazdzp[ii]);
                        diva=as11+as22+as33;
                     }

                     if(twosijsij<10000 && accCriteria<0.1){
                        count2++;
                        meanDiss=meanDiss+viscosity*twosijsij;
                        meanUisq=meanUisq+(1./1.)*( up[ii]*up[ii]
                                                   +vp[ii]*vp[ii]
                                                   +wp[ii]*wp[ii]);
                        meanDudxsq=meanDudxsq+(1./1.)*( dudxp[ii]*dudxp[ii]
                                                       +dvdyp[ii]*dvdyp[ii]
                                                       +dwdzp[ii]*dwdzp[ii]);
                     }

                  }// end for
                  ////end of polynom business
                  mainForm->meanPointsInSphereBEdit->Text=IntToStr((int)(pointList->meanPointsInSphereB/3.+0.5));
                  mainForm->succIntTrajEdit->Text=IntToStr((int)(pointList->succIntTraj+0.5));
                  if(count2>0){
                     mainForm->meanDissEdit->Text=IntToStr((int)(1.e6*pointList->meanDiss/(double)count2+0.5));
                     int Reynolds=(int)((pow(meanUisq/(double)count2,0.5)*pow(meanUisq/meanDudxsq,0.5))/viscosity+0.5);
                     mainForm->reEdit->Text=IntToStr(Reynolds);
                     mainForm->meanSuccessAccEdit->Text=IntToStr((int)(100.*(double)pointList->count4/(double)pointList->count9+0.5));
                     double dummy=(double)forMeanNumInTraj/(double)count6;
                     mainForm->meanEffectiveNumInTrajEdit->Text=IntToStr((int)((double)forEffectiveMeanNumInTraj/(double)count6+0.5));
                     mainForm->meanNumInTrajEdit->Text=IntToStr((int)(dummy+0.5));
                     mainForm->TypicalPolyOrderEdit->Text=FloatToStr((double)forTypicalPolyOrder/((double)count6*dummy));
                     mainForm->Refresh();
                  }
               }// end if write
               else{
                  //draw series
                  trajForm->Series1->Clear();
                  trajForm->Series2->Clear();
                  trajForm->Series3->Clear();
                  //clear and update omega's
                  trajForm->Series31->Clear();
                  for(int iii=0;iii<trajForm->Series4->Count() - 1;iii++){
                     trajForm->Series31->AddXY(trajForm->Series4->XValue[iii],trajForm->Series4->YValue[iii],'.',clTeeColor);
                  }
                  trajForm->Series4->Clear();
                  trajForm->Series32->Clear();
                  for(int iii=0;iii<trajForm->Series5->Count() - 1;iii++){
                     trajForm->Series32->AddXY(trajForm->Series5->XValue[iii],trajForm->Series5->YValue[iii],'.',clTeeColor);
                  }
                  trajForm->Series5->Clear();
                  trajForm->Series33->Clear();
                  for(int iii=0;iii<trajForm->Series6->Count() - 1;iii++){
                     trajForm->Series33->AddXY(trajForm->Series6->XValue[iii],trajForm->Series6->YValue[iii],'.',clTeeColor);
                  }
                  trajForm->Series6->Clear();
                  //end clear and update du/dt
                  trajForm->Series4->Clear();
                  trajForm->Series5->Clear();
                  trajForm->Series6->Clear();
                  trajForm->Series7->Clear();
                  trajForm->Series8->Clear();
                  trajForm->Series9->Clear();
                  trajForm->Series10->Clear();
                  trajForm->Series11->Clear();
                  trajForm->Series12->Clear();
                  //clear amd update du/dt
                  trajForm->Series28->Clear();
                  for(int iii=0;iii<trajForm->Series13->Count() - 1;iii++){
                     trajForm->Series28->AddXY(trajForm->Series13->XValue[iii],trajForm->Series13->YValue[iii],'.',clTeeColor);
                  }
                  trajForm->Series13->Clear();
                  trajForm->Series29->Clear();
                  for(int iii=0;iii<trajForm->Series14->Count() - 1;iii++){
                     trajForm->Series29->AddXY(trajForm->Series14->XValue[iii],trajForm->Series14->YValue[iii],'.',clTeeColor);
                  }
                  trajForm->Series14->Clear();
                  trajForm->Series30->Clear();
                  for(int iii=0;iii<trajForm->Series15->Count() - 1;iii++){
                     trajForm->Series30->AddXY(trajForm->Series15->XValue[iii],trajForm->Series15->YValue[iii],'.',clTeeColor);
                  }
                  trajForm->Series15->Clear();
                  //end clear and update du/dt
                  trajForm->Series16->Clear();
                  trajForm->Series17->Clear();
                  trajForm->Series18->Clear();
                  trajForm->Series19->Clear();
                  trajForm->Series20->Clear();
                  trajForm->Series21->Clear();

                  for(int ii=0;ii<numInTraj-4;ii++){
                     trajForm->Series1->AddXY((double)ii,axp[ii],'.',clTeeColor);
                     trajForm->Series2->AddXY((double)ii,ayp[ii],'.',clTeeColor);
                     trajForm->Series3->AddXY((double)ii,azp[ii],'.',clTeeColor);

                     trajForm->Series4->AddXY((double)ii,dudxp[ii],'.',clTeeColor);
                     trajForm->Series5->AddXY((double)ii,dvdyp[ii],'.',clTeeColor);
                     trajForm->Series6->AddXY((double)ii,dwdzp[ii],'.',clTeeColor);

                     trajForm->Series13->AddXY((double)ii,dudtp[ii],'.',clTeeColor);
                     trajForm->Series14->AddXY((double)ii,dvdtp[ii],'.',clTeeColor);
                     trajForm->Series15->AddXY((double)ii,dwdtp[ii],'.',clTeeColor);

                     acx=up[ii]*dudxp[ii]+vp[ii]*dudyp[ii]+wp[ii]*dudzp[ii];
                     acy=up[ii]*dvdxp[ii]+vp[ii]*dvdyp[ii]+wp[ii]*dvdzp[ii];
                     acz=up[ii]*dwdxp[ii]+vp[ii]*dwdyp[ii]+wp[ii]*dwdzp[ii];

                     trajForm->Series16->AddXY(acx+dudtp[ii],axp[ii],'.',clTeeColor);
                     trajForm->Series17->AddXY(acy+dvdtp[ii],ayp[ii],'.',clTeeColor);
                     trajForm->Series18->AddXY(acz+dwdtp[ii],azp[ii],'.',clTeeColor);

                     trajForm->Series19->AddXY(-dvdyp[ii]-dwdzp[ii],dudxp[ii],'.',clTeeColor);
                     trajForm->Series20->AddXY(-dudxp[ii]-dwdzp[ii],dvdyp[ii],'.',clTeeColor);
                     trajForm->Series21->AddXY(-dudxp[ii]-dvdyp[ii],dwdzp[ii],'.',clTeeColor);

                     
                  }//end for
                  trajForm->Refresh();
               } //end else
            }//end of if enough for traj
                  




         } // end if not occ und central
         else{
            trajForm->Series1->Clear();
            trajForm->Series2->Clear();
            trajForm->Series3->Clear();
            trajForm->Series4->Clear();
            trajForm->Series5->Clear();
            trajForm->Series6->Clear();
            trajForm->Series7->Clear();
            trajForm->Series8->Clear();
            trajForm->Series9->Clear();
            trajForm->Series10->Clear();
            trajForm->Series11->Clear();
            trajForm->Series12->Clear();
            trajForm->Series13->Clear();
            trajForm->Series14->Clear();
            trajForm->Series15->Clear();
            trajForm->Series16->Clear();
            trajForm->Series17->Clear();
            trajForm->Series18->Clear();
            trajForm->Series19->Clear();
            trajForm->Series20->Clear();
            trajForm->Series21->Clear();
            
         }//end else
     }// end haupt for schlaufe
     

}
//----------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::FilterGrid(FILE *fpp,int t)
{
   int filtSize=0;
   int numFields;

      numFields=32;
   
   double viscosity=StrToFloat(mainForm->viscEdit->Text);
   double polyConst=StrToFloat(mainForm->polyConstEdit->Text);
   deltaT=StrToFloat(mainForm->deltaTEdit->Text);
   c1=StrToFloat(mainForm->c1Edit->Text);
   c2=StrToFloat(mainForm->c2Edit->Text);
  
   double su;
   double we[100];
   int orderA;

   double w1,w2,w3,s11,s12,s13,s22,s23,s33,vel,wsq,twosijsij,Q;
   double div,ref,divCriteria;
   double diva,divaCriteria;
   double dix,diy,diz,refx,refy,refz,accCriteria,absdi;
   double acx,acy,acz;
   double cnx,cny,cnz,curvGrad,curv,curvCriteria,Lx,Ly,Lz,gradCriteria;

   //read t+variable (update grid memory +/-variable)
   if(t+filtSize<pointList->lastFile+1){
        pointList->readGridFile(t+filtSize,2*filtSize);
   }
   //if t > filtSize & t < lastFile-filtSize
   if(t > pointList->firstFile+filtSize-1 && t < pointList->lastFile-filtSize+1){
      //center spline all fields, for example
      for(int i=0;i<pointList->numPointInGrid;i++){
         for(int j=0;j<numFields;j++){
            interpolatedGrid[i][j]=0;
            if(!(j==0 || j==1 || j==2 ||j==30 || j==31)){
               su=0;
               for(int k=0;k<2*filtSize+1;k++){
                  //estimate quality with proper weights
                  //%omega,strain,div,ref
                  w1=gridMem[k][i][13]-gridMem[k][i][11];//Liw[2]-Liv[3];
                  w2=gridMem[k][i][ 8]-gridMem[k][i][12];//Liu[3]-Liw[1];
                  w3=gridMem[k][i][ 9]-gridMem[k][i][ 7];//Liv[1]-Liu[2];
                  s11=gridMem[k][i][ 6];//Liu[1];
                  s22=gridMem[k][i][10];//Liv[2];
                  s33=gridMem[k][i][14];//Liw[3];
                  s12=0.5*(gridMem[k][i][ 7]+gridMem[k][i][ 9]);//0.5*(Liu[2]+Liv[1]);
                  s13=0.5*(gridMem[k][i][ 8]+gridMem[k][i][12]);//0.5*(Liu[3]+Liw[1]);
                  s23=0.5*(gridMem[k][i][11]+gridMem[k][i][13]);//0.5*(Liv[3]+Liw[2]);
                  //for weighting divergence
                  div=fabs(s11+s22+s33);
                  ref=fabs(s11)+fabs(s22)+fabs(s33);
                  if(ref>0){
                     divCriteria=div/ref;
                  }
                  else{
                     divCriteria=0.95;
                  }
                  //for weighting acceleration
                  //acceleration quality
                  dix=gridMem[k][i][18]-gridMem[k][i][15]-gridMem[k][i][ 3]*gridMem[k][i][ 6]-gridMem[k][i][ 4]*gridMem[k][i][ 7]-gridMem[k][i][ 5]*gridMem[k][i][ 8];
                  diy=gridMem[k][i][19]-gridMem[k][i][16]-gridMem[k][i][ 3]*gridMem[k][i][ 9]-gridMem[k][i][ 4]*gridMem[k][i][10]-gridMem[k][i][ 5]*gridMem[k][i][11];
                  diz=gridMem[k][i][20]-gridMem[k][i][17]-gridMem[k][i][ 3]*gridMem[k][i][12]-gridMem[k][i][ 4]*gridMem[k][i][13]-gridMem[k][i][ 5]*gridMem[k][i][14];
                  absdi=pow(dix*dix+diy*diy+diz*diz,0.5);
                  refx= fabs(gridMem[k][i][18])
                       +fabs(gridMem[k][i][15])
                       +fabs( gridMem[k][i][ 3]*gridMem[k][i][ 6]
                             +gridMem[k][i][ 4]*gridMem[k][i][ 7]
                             +gridMem[k][i][ 5]*gridMem[k][i][ 8]);
                  refy= fabs(gridMem[k][i][19])
                       +fabs(gridMem[k][i][16])
                       +fabs( gridMem[k][i][ 3]*gridMem[k][i][ 9]
                             +gridMem[k][i][ 4]*gridMem[k][i][10]
                             +gridMem[k][i][ 5]*gridMem[k][i][11]);
                  refz= fabs(gridMem[k][i][20])
                       +fabs(gridMem[k][i][17])
                       +fabs( gridMem[k][i][ 3]*gridMem[k][i][12]
                             +gridMem[k][i][ 4]*gridMem[k][i][13]
                             +gridMem[k][i][ 5]*gridMem[k][i][14]);
                  if(refx>0 && refy>0 && refz>0){
                     accCriteria=(1./3.)*(fabs(dix)/refx+fabs(diy)/refy+fabs(diz)/refz); //20.*absdi;//
                  }
                  else{
                     accCriteria=0.95;
                  }
                  //here we make also a weighting according to diva=4Q
                  wsq=w1*w1+w2*w2+w3*w3;
                  twosijsij=2.*(s11*s11+s22*s22+s33*s33
                                +2.*(s12*s12+s13*s13+s23*s23)
                                );
                  Q=(1./4.)*(wsq-twosijsij);
                  diva=gridMem[k][i][21]+gridMem[k][i][25]+gridMem[k][i][29];
                  if(fabs(diva)+fabs(4*Q)>0){
                     divaCriteria=fabs(diva+4*Q)/(fabs(diva)+fabs(4*Q));
                  }
                  else{
                     divaCriteria=0.95;
                  }
                  gridMem[k][i][30]= 0;
                  //end of estimate quality
                  su=su+1-gridMem[k][i][30];//success=0, bad=1;
               }
               orderA=(int)(su/polyConst+3.5)+1;
               setAllMatrixesToZero(orderA+1);
               for(int k=0;k<2*filtSize+1;k++){
                  we[k]=1.-1./(1.+exp(-c1*(gridMem[k][i][30]-c2)));//reldiv(ii)
               }

               for(int k=0;k<2*filtSize+1;k++){
                  for(int ii=0;ii<orderA;ii++){
                     A[k][ii]=we[k]*pow((double)k*deltaT+0.000000001,(double)(ii));
                  }
                  Y[k]=we[k]*gridMem[k][i][j]; //some field position
               }
               makeAT(2*filtSize+1,orderA);
               makeATA(2*filtSize+1,orderA);
               makeATY(2*filtSize+1,orderA,0);
               solve(2*filtSize+1,orderA);
               for(int ii=0;ii<orderA;ii++){
                  interpolatedGrid[i][j]=interpolatedGrid[i][j]+ X[ii]*pow((double)filtSize*deltaT+0.00001,(double)(ii));
               }
            }
            else{
               interpolatedGrid[i][j]=gridMem[filtSize][i][j];
            }
         }
      }
      //write to 'filename'
      for(int i=0;i<pointList->numPointInGrid;i++){
         for(int j=0;j<numFields-1;j++){
            fprintf(fpp, "%lf\t", interpolatedGrid[i][j]);
         }
         fprintf(fpp, "%lf\n", interpolatedGrid[i][numFields-1]);
         ///////////////quality stuff
         if(fabs(gridMem[filtSize][i][3])>0 && fabs(gridMem[filtSize][i][4])>0 && fabs(gridMem[filtSize][i][5])>0){
         //%omega,strain,div,ref
         w1=interpolatedGrid[i][13]-interpolatedGrid[i][11];//Liw[2]-Liv[3];
         w2=interpolatedGrid[i][ 8]-interpolatedGrid[i][12];//Liu[3]-Liw[1];
         w3=interpolatedGrid[i][ 9]-interpolatedGrid[i][ 7];//Liv[1]-Liu[2];
         s11=interpolatedGrid[i][ 6];//Liu[1];
         s22=interpolatedGrid[i][10];//Liv[2];
         s33=interpolatedGrid[i][14];//Liw[3];
         s12=0.5*(interpolatedGrid[i][ 7]+interpolatedGrid[i][ 9]);//0.5*(Liu[2]+Liv[1]);
         s13=0.5*(interpolatedGrid[i][ 8]+interpolatedGrid[i][12]);//0.5*(Liu[3]+Liw[1]);
         s23=0.5*(interpolatedGrid[i][11]+interpolatedGrid[i][13]);//0.5*(Liv[3]+Liw[2]);
         //for weighting divergence
         div=fabs(s11+s22+s33);
         ref=fabs(s11)+fabs(s22)+fabs(s33);
         if(ref>0){
            divCriteria=div/ref;
         }
         else{
            divCriteria=0.95;
         }
         //for weighting acceleration
         //acceleration quality
         dix=interpolatedGrid[i][18]-interpolatedGrid[i][15]-interpolatedGrid[i][ 3]*interpolatedGrid[i][ 6]-interpolatedGrid[i][ 4]*interpolatedGrid[i][ 7]-interpolatedGrid[i][ 5]*interpolatedGrid[i][ 8];
         diy=interpolatedGrid[i][19]-interpolatedGrid[i][16]-interpolatedGrid[i][ 3]*interpolatedGrid[i][ 9]-interpolatedGrid[i][ 4]*interpolatedGrid[i][10]-interpolatedGrid[i][ 5]*interpolatedGrid[i][11];
         diz=interpolatedGrid[i][20]-interpolatedGrid[i][17]-interpolatedGrid[i][ 3]*interpolatedGrid[i][12]-interpolatedGrid[i][ 4]*interpolatedGrid[i][13]-interpolatedGrid[i][ 5]*interpolatedGrid[i][14];
         absdi=pow(dix*dix+diy*diy+diz*diz,0.5);
         refx= fabs(interpolatedGrid[i][18])
              +fabs(interpolatedGrid[i][15])
              +fabs( interpolatedGrid[i][ 3]*interpolatedGrid[i][ 6]
                    +interpolatedGrid[i][ 4]*interpolatedGrid[i][ 7]
                    +interpolatedGrid[i][ 5]*interpolatedGrid[i][ 8]);
         refy= fabs(interpolatedGrid[i][19])
              +fabs(interpolatedGrid[i][16])
              +fabs( interpolatedGrid[i][ 3]*interpolatedGrid[i][ 9]
                    +interpolatedGrid[i][ 4]*interpolatedGrid[i][10]
                    +interpolatedGrid[i][ 5]*interpolatedGrid[i][11]);
         refz= fabs(interpolatedGrid[i][20])
              +fabs(interpolatedGrid[i][17])
              +fabs( interpolatedGrid[i][ 3]*interpolatedGrid[i][12]
                    +interpolatedGrid[i][ 4]*interpolatedGrid[i][13]
                    +interpolatedGrid[i][ 5]*interpolatedGrid[i][14]);
         if(refx>0 && refy>0 && refz>0){
            accCriteria=(1./3.)*(fabs(dix)/refx+fabs(diy)/refy+fabs(diz)/refz); //20.*absdi;//
         }
         else{
            accCriteria=0.95;
         }
         //here we make also a weighting according to diva=4Q
         wsq=w1*w1+w2*w2+w3*w3;
         twosijsij=2.*(s11*s11+s22*s22+s33*s33
                       +2.*(s12*s12+s13*s13+s23*s23)
                       );
         Q=(1./4.)*(wsq-twosijsij);
         diva=interpolatedGrid[i][21]+interpolatedGrid[i][25]+interpolatedGrid[i][29];
         if(fabs(diva)+fabs(4*Q)>0){
            divaCriteria=fabs(diva+4*Q)/(fabs(diva)+fabs(4*Q));
         }
         else{
            divaCriteria=0.95;
         }
         count++;
         if(divCriteria<0.1){
            count2++;
            meanDiss=meanDiss+viscosity*twosijsij;
            meanUisq=meanUisq+(1./1.)*( interpolatedGrid[i][ 3]*interpolatedGrid[i][ 3]
                                       +interpolatedGrid[i][ 4]*interpolatedGrid[i][ 4]
                                       +interpolatedGrid[i][ 5]*interpolatedGrid[i][ 5]);
            meanDudxsq=meanDudxsq+(1./1.)*( interpolatedGrid[i][ 6]*interpolatedGrid[i][ 6]
                                           +interpolatedGrid[i][10]*interpolatedGrid[i][10]
                                           +interpolatedGrid[i][14]*interpolatedGrid[i][14]);
         }
         if(accCriteria<0.1){
            count4++;
         }
         if(divaCriteria<0.1){
            count5++;
         }
         }
         ///////////////end of quality stuff
      }
   }
   //shift gridMem to left
   for(int i=0;i<2*filtSize;i++){
      for(int j=0;j<pointList->numPointInGrid;j++){
          for(int k=0;k<numFields;k++){
              gridMem[i][j][k]=gridMem[i+1][j][k];
          }
      }
   }
}



//---------------------------------------------------------------------------
void __fastcall TpointList::makeVolumes()
{
     int remain,Jremain,finRemain,ind,candCounter,numVolumes,k1,k2,k3,k4,k5,k6,k7;
     int it1,ii1,it2,ii2,it3,ii3,it4,ii4,it5,ii5,it6,ii6,it7,ii7;
     double x1,x2,x3,y1,y2,y3,z1,z2,z3,d1,d2,d3,vol;
     double dist,bestDist,secondDist,thirdDist,fourthDist,fifthDist,sixthDist,seventhDist,avCand;
     double cand[300][4];
     bool ok;

     double maxDist=StrToFloat(mainForm->maxDistEdit->Text);
     int minRemain=0;

     //setPath and Files
     setPathAndFiles3();
     //loop through files
     for(int i=0;i<51;i++){
        for(int j=0;j<5000;j++){
           for(int k=0;k<32;k++){
              trajec[i][j][k]=0.;    
           }
        }
     }
     numVolumes=0;
     avCand=0;
     for (int i=firstFile;i<lastFile+1;i++){
        mainForm->fileNum2Edit->Text=IntToStr(i);
        mainForm->Refresh();
        readTrajPointFile(i);
        FILE *fpp;
        AnsiString name;
        name="trajVolumes."+IntToStr(i);
        const char *filename;
        filename=name.c_str();
        fpp = fopen(filename,"w");
        for(int j=1;j<trajec[50][0][0]+1;j++){
           candCounter=0;
           if(trajec[50][j][28]==0){
              Jremain=1;
              ind=1;
              ok=true;
              while(ok){
                 if(trajec[50][j][27]==trajec[50][j+ind][27]){
                    Jremain++;
                    ind++;
                 }
                 else{
                    ok=false;
                 }
              }
              if(Jremain>minRemain){
                 //search through t-50 files
                 for(int k=0;k<51;k++){
                    for(int l=1;l<trajec[k][0][0]+1;l++){
                       //if close enough it becomes candidate
                       if(-50+k+trajec[k][l][28]==0 && !(k==50 && j==l)){
                          dist=pow( pow(trajec[50][j][0]-trajec[k][l][0],2.)
                                   +pow(trajec[50][j][1]-trajec[k][l][1],2.)
                                   +pow(trajec[50][j][2]-trajec[k][l][2],2.),0.5);
                          if(dist<maxDist && dist>1e-5){
                             remain=1;
                             ind=1;
                             ok=true;
                             while(ok){
                                 if(trajec[k][l][27]==trajec[k][l+ind][27]){
                                     remain++;
                                     ind++;
                                 }
                                 else{
                                    ok=false;
                                 }
                             }
                             if(remain>minRemain){
                                //t,n,dist,remaining lenght
                                cand[candCounter][0]=(double)k;
                                cand[candCounter][1]=(double)l;
                                cand[candCounter][2]=dist;
                                cand[candCounter][3]=(double)remain;
                                candCounter++;
                             }
                          }
                       }
                    }
                 }
              }
           }
           //make all possible volumes from candidates
           bestDist=1;
           secondDist=1;
           thirdDist=1;
           fourthDist=1;
           fifthDist=1;
           sixthDist=1;
           seventhDist=1;
           finRemain=Jremain;
           for(int k=0;k<candCounter;k++){
              if(cand[k][2]<bestDist){
                 seventhDist=sixthDist;
                 sixthDist=fifthDist;
                 fifthDist=fourthDist;
                 fourthDist=thirdDist;
                 thirdDist=secondDist;
                 secondDist=bestDist;
                 bestDist=cand[k][2];
              }
              else{
                 if(cand[k][2]<secondDist){
                    seventhDist=sixthDist;
                    sixthDist=fifthDist;
                    fifthDist=fourthDist;
                    fourthDist=thirdDist;
                    thirdDist=secondDist;
                    secondDist=cand[k][2];
                 }
                 else{
                    if(cand[k][2]<thirdDist){
                       seventhDist=sixthDist;
                       sixthDist=fifthDist;
                       fifthDist=fourthDist;
                       fourthDist=thirdDist;
                       thirdDist=cand[k][2];
                    }
                    else{
                       if(cand[k][2]<fourthDist){
                           seventhDist=sixthDist;
                           sixthDist=fifthDist;
                           fifthDist=fourthDist;
                           fourthDist=cand[k][2];
                       }
                       else{
                           if(cand[k][2]<fifthDist){
                               seventhDist=sixthDist;
                               sixthDist=fifthDist;
                               fifthDist=cand[k][2];
                           }
                           else{
                               if(cand[k][2]<sixthDist){
                                  seventhDist=sixthDist;
                                  sixthDist=cand[k][2];
                               }
                               else{
                                   if(cand[k][2]<seventhDist){
                                      seventhDist=cand[k][2];
                                   }
                               }
                           }
                       }
                    }
                 }
              }
           }
           if(candCounter>7){
              //store them in trajecVolume files
              numVolumes++;
              avCand=(avCand*((double)numVolumes-1)+(double)candCounter)/(double)numVolumes;
              for(int k=0;k<candCounter;k++){
                  if(cand[k][2]==bestDist){
                     k1=k;
                     if(cand[k][3]<finRemain){
                         finRemain=cand[k][3];
                     }
                  }
                  if(cand[k][2]==secondDist){
                     k2=k;
                     if(cand[k][3]<finRemain){
                         finRemain=cand[k][3];
                     }
                  }
                  if(cand[k][2]==thirdDist){
                     k3=k;
                     if(cand[k][3]<finRemain){
                         finRemain=cand[k][3];
                     }
                  }
                  if(cand[k][2]==fourthDist){
                     k4=k;
                     if(cand[k][3]<finRemain){
                         finRemain=cand[k][3];
                     }
                  }
                  if(cand[k][2]==fifthDist){
                     k5=k;
                     if(cand[k][3]<finRemain){
                         finRemain=cand[k][3];
                     }
                  }
                  if(cand[k][2]==sixthDist){
                     k6=k;
                     if(cand[k][3]<finRemain){
                         finRemain=cand[k][3];
                     }
                  }
                  if(cand[k][2]==seventhDist){
                     k7=k;
                     if(cand[k][3]<finRemain){
                         finRemain=cand[k][3];
                     }
                  }
              }
              for(int k=0;k<finRemain;k++){
                  //volume bestimmen
                  it1=cand[k1][0];
                  ii1=cand[k1][1];
                  it2=cand[k2][0];
                  ii2=cand[k2][1];
                  it3=cand[k3][0];
                  ii3=cand[k3][1];
                  it4=cand[k4][0];
                  ii4=cand[k4][1];
                  it5=cand[k5][0];
                  ii5=cand[k5][1];
                  it6=cand[k6][0];
                  ii6=cand[k6][1];
                  it7=cand[k7][0];
                  ii7=cand[k7][1];
                  x1=-1000.*(trajec[50][j+k][0]-trajec[it1][ii1+k][0]);
                  y1=-1000.*(trajec[50][j+k][1]-trajec[it1][ii1+k][1]);
                  z1=-1000.*(trajec[50][j+k][2]-trajec[it1][ii1+k][2]);
                  x2=-1000.*(trajec[50][j+k][0]-trajec[it2][ii2+k][0]);
                  y2=-1000.*(trajec[50][j+k][1]-trajec[it2][ii2+k][1]);
                  z2=-1000.*(trajec[50][j+k][2]-trajec[it2][ii2+k][2]);
                  x3=-1000.*(trajec[50][j+k][0]-trajec[it3][ii3+k][0]);
                  y3=-1000.*(trajec[50][j+k][1]-trajec[it3][ii3+k][1]);
                  z3=-1000.*(trajec[50][j+k][2]-trajec[it3][ii3+k][2]);
                  d1=pow(x1*x1+y1*y1+z1*z1,0.5);
                  d2=pow(x2*x2+y2*y2+z2*z2,0.5);
                  d3=pow(x3*x3+y3*y3+z3*z3,0.5);
                  vol=x1*(y2*z3-z2*y3)+y1*(z2*x3-x2*z3)+z1*(x2*y3-y2*z3);
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", 1000.*trajec[50][j+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", vol);
                  fprintf(fpp, "%lf\t", d1);
                  fprintf(fpp, "%lf\t", d2);
                  fprintf(fpp, "%lf\n", d3);
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", 1000.*trajec[it1][ii1+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", vol);
                  fprintf(fpp, "%lf\t", d1);
                  fprintf(fpp, "%lf\t", d2);
                  fprintf(fpp, "%lf\n", d3);
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", 1000.*trajec[it2][ii2+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", vol);
                  fprintf(fpp, "%lf\t", d1);
                  fprintf(fpp, "%lf\t", d2);
                  fprintf(fpp, "%lf\n", d3);
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", 1000.*trajec[it3][ii3+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", vol);
                  fprintf(fpp, "%lf\t", d1);
                  fprintf(fpp, "%lf\t", d2);
                  fprintf(fpp, "%lf\n", d3);
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", 1000.*trajec[it4][ii4+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", vol);
                  fprintf(fpp, "%lf\t", d1);
                  fprintf(fpp, "%lf\t", d2);
                  fprintf(fpp, "%lf\n", d3);
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", 1000.*trajec[it5][ii5+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", vol);
                  fprintf(fpp, "%lf\t", d1);
                  fprintf(fpp, "%lf\t", d2);
                  fprintf(fpp, "%lf\n", d3);
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", 1000.*trajec[it6][ii6+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", vol);
                  fprintf(fpp, "%lf\t", d1);
                  fprintf(fpp, "%lf\t", d2);
                  fprintf(fpp, "%lf\n", d3);
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", 1000.*trajec[it7][ii7+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", vol);
                  fprintf(fpp, "%lf\t", d1);
                  fprintf(fpp, "%lf\t", d2);
                  fprintf(fpp, "%lf\n", d3);
              }
           }
        }//end points, j, loop
        fclose (fpp);
        mainForm->volumeEdit->Text=IntToStr(numVolumes);
        mainForm->avCandEdit->Text=FloatToStr(avCand);
        mainForm->Refresh();
     }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeSurfaces()
{
     int remain,Jremain,finRemain,ind,candCounter,numSurfaces,k1,k2;
     int it1,ii1,it2,ii2;
     double x1,x2,y1,y2,z1,z2,d1,d2,nx,ny,nz;
     double dist,bestDist,secondDist,avCand;
     double cand[300][4];
     bool ok;

     double maxDist=StrToFloat(mainForm->maxDistSurfEdit->Text);
     int minRemain=StrToInt(mainForm->minRemainSurfEdit->Text);

     //setPath and Files
     setPathAndFiles3();
     //loop through files
     for(int i=0;i<51;i++){
        for(int j=0;j<5000;j++){
           for(int k=0;k<32;k++){
              trajec[i][j][k]=0.;    
           }
        }
     }
     numSurfaces=0;
     avCand=0;
     for (int i=firstFile;i<lastFile+1;i++){
        mainForm->fileNum2Edit->Text=IntToStr(i);
        mainForm->Refresh();
        readTrajPointFile(i);
        FILE *fpp;
        AnsiString name;
        name="trajSurfaces."+IntToStr(i);
        const char *filename;
        filename=name.c_str();
        fpp = fopen(filename,"w");
        for(int j=1;j<trajec[50][0][0]+1;j++){
           candCounter=0;
           if(trajec[50][j][28]==0){
              Jremain=1;
              ind=1;
              ok=true;
              while(ok){
                 if(trajec[50][j][27]==trajec[50][j+ind][27]){
                    Jremain++;
                    ind++;
                 }
                 else{
                    ok=false;
                 }
              }
              if(Jremain>minRemain){
                 //search through t-50 files
                 for(int k=0;k<51;k++){
                    for(int l=1;l<trajec[k][0][0]+1;l++){
                       //if close enough it becomes candidate
                       if(-50+k+trajec[k][l][28]==0 && !(k==50 && j==l)){
                          dist=pow( pow(trajec[50][j][0]-trajec[k][l][0],2.)
                                   +pow(trajec[50][j][1]-trajec[k][l][1],2.)
                                   +pow(trajec[50][j][2]-trajec[k][l][2],2.),0.5);
                          if(dist<maxDist && dist>1e-5){
                             remain=1;
                             ind=1;
                             ok=true;
                             while(ok){
                                 if(trajec[k][l][27]==trajec[k][l+ind][27]){
                                     remain++;
                                     ind++;
                                 }
                                 else{
                                    ok=false;
                                 }
                             }
                             if(remain>minRemain){
                                //t,n,dist,remaining lenght
                                cand[candCounter][0]=(double)k;
                                cand[candCounter][1]=(double)l;
                                cand[candCounter][2]=dist;
                                cand[candCounter][3]=(double)remain;
                                candCounter++;
                             }
                          }
                       }
                    }
                 }
              }
           }
           //make all possible surfaces from candidates
           bestDist=1;
           secondDist=1;
           finRemain=Jremain;
           for(int k=0;k<candCounter;k++){
              if(cand[k][2]<bestDist){
                 secondDist=bestDist;
                 bestDist=cand[k][2];
              }
              else{
                 if(cand[k][2]<secondDist){
                    secondDist=cand[k][2];
                 }
              }
           }
           if(candCounter>1){
              //store them in trajecVolume files
              numSurfaces++;
              avCand=(avCand*((double)numSurfaces-1)+(double)candCounter)/(double)numSurfaces;
              for(int k=0;k<candCounter;k++){
                  if(cand[k][2]==bestDist){
                     k1=k;
                     if(cand[k][3]<finRemain){
                         finRemain=cand[k][3];
                     }
                  }
                  if(cand[k][2]==secondDist){
                     k2=k;
                     if(cand[k][3]<finRemain){
                         finRemain=cand[k][3];
                     }
                  }
              }
              for(int k=0;k<finRemain;k++){
                  //surface bestimmen
                  it1=cand[k1][0];
                  ii1=cand[k1][1];
                  it2=cand[k2][0];
                  ii2=cand[k2][1];
                  x1=-1000.*(trajec[50][j+k][0]-trajec[it1][ii1+k][0]);
                  y1=-1000.*(trajec[50][j+k][1]-trajec[it1][ii1+k][1]);
                  z1=-1000.*(trajec[50][j+k][2]-trajec[it1][ii1+k][2]);
                  x2=-1000.*(trajec[50][j+k][0]-trajec[it2][ii2+k][0]);
                  y2=-1000.*(trajec[50][j+k][1]-trajec[it2][ii2+k][1]);
                  z2=-1000.*(trajec[50][j+k][2]-trajec[it2][ii2+k][2]);
                  d1=pow(x1*x1+y1*y1+z1*z1,0.5);
                  d2=pow(x2*x2+y2*y2+z2*z2,0.5);
                  nx=y1*z2-z1*y2;
                  ny=z1*x2-x1*z2;
                  nz=x1*y2-y1*x2;
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", trajec[50][j+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", d1);
                  fprintf(fpp, "%lf\t", d2);
                  fprintf(fpp, "%lf\t", nx);
                  fprintf(fpp, "%lf\t", ny);
                  fprintf(fpp, "%lf\n", nz);
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", trajec[it1][ii1+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", d1);
                  fprintf(fpp, "%lf\t", d2);
                  fprintf(fpp, "%lf\t", nx);
                  fprintf(fpp, "%lf\t", ny);
                  fprintf(fpp, "%lf\n", nz);
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", trajec[it2][ii2+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", d1);
                  fprintf(fpp, "%lf\t", d2);
                  fprintf(fpp, "%lf\t", nx);
                  fprintf(fpp, "%lf\t", ny);
                  fprintf(fpp, "%lf\n", nz);
              }
           }
        }//end points, j, loop
        fclose (fpp);
        mainForm->surfaceEdit->Text=IntToStr(numSurfaces);
        mainForm->avCandSurfEdit->Text=FloatToStr(avCand);
        mainForm->Refresh();
     }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeLines()
{
     int remain,Jremain,finRemain,ind,candCounter,numLines,k1;
     int it1,ii1;
     double x1,y1,z1,d;
     double dist,bestDist,avCand;
     double cand[300][4];
     bool ok;

     double maxDist=StrToFloat(mainForm->maxDistLineEdit->Text);
     int minRemain=StrToInt(mainForm->minRemainLineEdit->Text);

     //setPath and Files
     setPathAndFiles3();

     //loop through files
     for(int i=0;i<51;i++){
        for(int j=0;j<5000;j++){
           for(int k=0;k<32;k++){
              trajec[i][j][k]=0.;    
           }
        }
     }
     numLines=0;
     avCand=0;
     for (int i=firstFile;i<lastFile+1;i++){
        mainForm->fileNum2Edit->Text=IntToStr(i);
        mainForm->Refresh();
        readTrajPointFile(i);
        FILE *fpp;
        AnsiString name;
        name="trajLines."+IntToStr(i);
        const char *filename;
        filename=name.c_str();
        fpp = fopen(filename,"w");
        for(int j=1;j<trajec[50][0][0]+1;j++){
           candCounter=0;
           if(trajec[50][j][28]==0){
              Jremain=1;
              ind=1;
              ok=true;
              while(ok){
                 if(trajec[50][j][27]==trajec[50][j+ind][27]){
                    Jremain++;
                    ind++;
                 }
                 else{
                    ok=false;
                 }
              }
              if(Jremain>minRemain){
                 //search through t-50 files
                 for(int k=0;k<51;k++){
                    for(int l=1;l<trajec[k][0][0]+1;l++){
                       //if close enough it becomes candidate
                       if(-50+k+trajec[k][l][28]==0 && !(k==50 && j==l)){
                          dist=pow( pow(trajec[50][j][0]-trajec[k][l][0],2.)
                                   +pow(trajec[50][j][1]-trajec[k][l][1],2.)
                                   +pow(trajec[50][j][2]-trajec[k][l][2],2.),0.5);
                          if(dist<maxDist && dist>1e-5){
                             remain=1;
                             ind=1;
                             ok=true;
                             while(ok){
                                 if(trajec[k][l][27]==trajec[k][l+ind][27]){
                                     remain++;
                                     ind++;
                                 }
                                 else{
                                    ok=false;
                                 }
                             }
                             if(remain>minRemain){
                                //t,n,dist,remaining lenght
                                cand[candCounter][0]=(double)k;
                                cand[candCounter][1]=(double)l;
                                cand[candCounter][2]=dist;
                                cand[candCounter][3]=(double)remain;
                                candCounter++;
                             }
                          }
                       }
                    }
                 }
              }
           }
           //make all possible surfaces from candidates
           bestDist=1;
           finRemain=Jremain;
           for(int k=0;k<candCounter;k++){
              if(cand[k][2]<bestDist){
                 bestDist=cand[k][2];
              }
           }
           if(candCounter>0){
              //store them in trajecVolume files
              numLines++;
              avCand=(avCand*((double)numLines-1)+(double)candCounter)/(double)numLines;
              for(int k=0;k<candCounter;k++){
                  if(cand[k][2]==bestDist){
                     k1=k;
                     if(cand[k][3]<finRemain){
                         finRemain=cand[k][3];
                     }
                  }
              }
              for(int k=0;k<finRemain;k++){
                  //surface bestimmen
                  it1=cand[k1][0];
                  ii1=cand[k1][1];
                  x1=-1000.*(trajec[50][j+k][0]-trajec[it1][ii1+k][0]);
                  y1=-1000.*(trajec[50][j+k][1]-trajec[it1][ii1+k][1]);
                  z1=-1000.*(trajec[50][j+k][2]-trajec[it1][ii1+k][2]);
                  d=pow(x1*x1+y1*y1+z1*z1,0.5);
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", trajec[50][j+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", d);
                  fprintf(fpp, "%lf\t", x1);
                  fprintf(fpp, "%lf\t", y1);
                  fprintf(fpp, "%lf\n", z1);
                  for(int l=0;l<32;l++){
                     fprintf(fpp, "%lf\t", trajec[it1][ii1+k][l]);
                  }
                  fprintf(fpp, "%lf\t", (double)k);
                  fprintf(fpp, "%lf\t", d);
                  fprintf(fpp, "%lf\t", x1);
                  fprintf(fpp, "%lf\t", y1);
                  fprintf(fpp, "%lf\n", z1);
              }
           }
        }//end points, j, loop
        fclose (fpp);
        mainForm->lineEdit->Text=IntToStr(numLines);
        mainForm->avCandLineEdit->Text=FloatToStr(avCand);
        mainForm->Refresh();
     }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeFilesForCorrelations()
{
     double dist;
     bool ok;
     int Jremain,ind,remain;

     //setPath and Files
     setPathAndFiles3();

     //loop through files
     for(int i=0;i<51;i++){
        for(int j=0;j<5000;j++){
           for(int k=0;k<32;k++){
              trajec[i][j][k]=0.;    
           }
        }
     }
     for (int i=firstFile;i<lastFile+1;i++){
        mainForm->fileNum2Edit->Text=IntToStr(i);
        mainForm->Refresh();
        readTrajPointFile(i);
        FILE *fpp;
        AnsiString name;
        name="trajCorrs."+IntToStr(i);
        const char *filename;
        filename=name.c_str();
        fpp = fopen(filename,"w");
        for(int j=1;j<trajec[50][0][0]+1;j++){
           if(trajec[50][j][28]==0){
              Jremain=1;
              ind=1;
              ok=true;
              while(ok){
                 if(trajec[50][j][27]==trajec[50][j+ind][27]){
                    Jremain++;
                    ind++;
                 }
                 else{
                    ok=false;
                 }
              }
              if(Jremain>10){
                 //search through t-50 files
                 for(int k=0;k<51;k++){
                    for(int l=1;l<trajec[k][0][0]+1;l++){
                       //if close enough it becomes candidate
                       if(-50+k+trajec[k][l][28]==0 && !(k==50 && j==l)){
                          dist=pow( pow(trajec[50][j][0]-trajec[k][l][0],2.)
                                   +pow(trajec[50][j][1]-trajec[k][l][1],2.)
                                   +pow(trajec[50][j][2]-trajec[k][l][2],2.),0.5);
                          if(dist>1e-5){
                             remain=1;
                             ind=1;
                             ok=true;
                             while(ok){
                                 if(trajec[k][l][27]==trajec[k][l+ind][27]){
                                     remain++;
                                     ind++;
                                 }
                                 else{
                                    ok=false;
                                 }
                             }
                             if(remain>10){
                                dist=1000*dist;
                                for(int n=0;n<32;n++){
                                   fprintf(fpp, "%lf\t", trajec[50][j][n]);
                                }
                                fprintf(fpp, "%lf\t", (double)k);
                                fprintf(fpp, "%lf\n", dist);
                                for(int n=0;n<32;n++){
                                   fprintf(fpp, "%lf\t", trajec[k][l][n]);
                                }
                                fprintf(fpp, "%lf\t", (double)k);
                                fprintf(fpp, "%lf\n", dist);
                             }
                          }
                       }
                    }
                 }
              }
           }
        }//end points, j, loop
        fclose (fpp);
        mainForm->Refresh();
     }
}

void __fastcall TpointList::setAllMatrixesToZero(int size)
{

    for(int i=0;i<arraySize;i++){
       if(i<size){
          X[i]=0.;
          //m0TY[i]=0.;
          ATY[i]=0.;
          BTY[i]=0.;
          CTY[i]=0.;
          //DTY[i]=0.;
       }
       Y[i]=0.;
       /*Yu[i]=0.;
       Yv[i]=0.;
       Yw[i]=0.;
       Yu0[i]=0.;
       Yv0[i]=0.;
       Yw0[i]=0.;
       YuA[i]=0.;
       YvA[i]=0.;
       YwA[i]=0.; */
       YuB[i]=0.;
       /*YvB[i]=0.;
       YwB[i]=0.;
       YuC[i]=0.;
       YvC[i]=0.;
       YwC[i]=0.;
       YuD[i]=0.;
       YvD[i]=0.;
       YwD[i]=0.;
       Yaz[i]=0.;
       Yay[i]=0.;
       Yax[i]=0.; */
       for(int j=0;j<size;j++){
          /*m0[i][j]=0.;
          m0T[j][i]=0.;
          if(i<size){m0T0[i][j]=0.;} */
          A[i][j]=0.;
          AT[j][i]=0.;
          if(i<size){ATA[i][j]=0.;}
          B[i][j]=0.;
          BT[j][i]=0.;
          if(i<size){BTB[i][j]=0.;}
          /*C[i][j]=0.;
          CT[j][i]=0.;
          if(i<size){CTC[i][j]=0.;}
          D[i][j]=0.;
          DT[j][i]=0.;
          if(i<size){DTD[i][j]=0.;} */
       }
    }
    
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TpointList::makeAT(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
           AT[i][j]=A[j][i];
        }
     }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeBT(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
           BT[i][j]=B[j][i];
        }
     }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeCT(int n, int m,int dt)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
           CT[i][j]=C[j][i][dt];
        }
     }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TpointList::makeATA(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
           ATA[i][j]=0.;
           for(int k=0;k<n;k++){
              ATA[i][j]=ATA[i][j]+AT[i][k]*A[k][j];
           }
        }
     }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeBTB(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
           BTB[i][j]=0.;
           for(int k=0;k<n;k++){
              BTB[i][j]=BTB[i][j]+BT[i][k]*B[k][j];
           }
        }
     }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeCTC(int n, int m,int dt)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
           CTC[i][j]=0.;
           for(int k=0;k<n;k++){
              CTC[i][j]=CTC[i][j]+CT[i][k]*C[k][j][dt];
           }
        }
     }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TpointList::makeATY(int n, int m,int wh)
{
     for(int i=0;i<m;i++){
           ATY[i]=0.;
           for(int k=0;k<n;k++){
               ATY[i]=ATY[i]+AT[i][k]*y[wh][k];
           }
     }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
double __fastcall TpointList::makekResidError(int n)
{
     double kResid,BY[300],ResidU[300],ResidV[300],ResidW[300];
     double weightSum;

     //B*u-YuB=Du
     X[0]=u;
     X[1]=ux;
     X[2]=uy;
     X[3]=uz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<4;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           ResidU[i]=BY[i]-YuB[i];
     }
     //B*v-YvB=Dv
     X[0]=v;
     X[1]=vx;
     X[2]=vy;
     X[3]=vz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<4;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           //ResidV[i]=BY[i]-YvB[i];
     }
     //B*w-YwB=Dw
     X[0]=w;
     X[1]=wx;
     X[2]=wy;
     X[3]=wz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<4;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           //ResidW[i]=BY[i]-YwB[i];
     }
     //now calc mean Resiudual Energy
     kResid=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        kResid=kResid+pow(ResidU[i]*ResidU[i]+ResidV[i]*ResidV[i]+ResidW[i]*ResidW[i],0.5);
        weightSum=weightSum+1;;
     }
     kResid=kResid/weightSum;
     return kResid;
}
//--------------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TpointList::makekResidErrorQuad(int n)
{
     double kResid,BY[300],ResidU[300],ResidV[300],ResidW[300];
     double weightSum;

     //B*u-YuB=Du
     X[0]=u;
     X[1]=ux;
     X[2]=uy;
     X[3]=uz;
     X[4]=uxx;
     X[5]=uyy;
     X[6]=uzz;
     X[7]=uxy;
     X[8]=uxz;
     X[9]=uyz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<10;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           ResidU[i]=BY[i]-YuB[i];
     }
     //B*v-YvB=Dv
     X[0]=v;
     X[1]=vx;
     X[2]=vy;
     X[3]=vz;
     X[4]=vxx;
     X[5]=vyy;
     X[6]=vzz;
     X[7]=vxy;
     X[8]=vxz;
     X[9]=vyz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<10;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           //ResidV[i]=BY[i]-YvB[i];
     }
     //B*w-YwB=Dw
     X[0]=w;
     X[1]=wx;
     X[2]=wy;
     X[3]=wz;
     X[4]=wxx;
     X[5]=wyy;
     X[6]=wzz;
     X[7]=wxy;
     X[8]=wxz;
     X[9]=wyz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<10;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           //ResidW[i]=BY[i]-YwB[i];
     }
     //now calc mean Resiudual Energy
     kResid=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        kResid=kResid+pow(ResidU[i]*ResidU[i]+ResidV[i]*ResidV[i]+ResidW[i]*ResidW[i],0.5);
        weightSum=weightSum+1;;
     }
     kResid=kResid/weightSum;
     return kResid;
}
//--------------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TpointList::makekResidErrorCube(int n)
{
     double kResid,BY[300],ResidU[300],ResidV[300],ResidW[300];
     double weightSum;

     //B*u-YuB=Du
     X[0]=u;
     X[1]=ux;
     X[2]=uy;
     X[3]=uz;
     X[4]=uxx;
     X[5]=uyy;
     X[6]=uzz;
     X[7]=uxy;
     X[8]=uxz;
     X[9]=uyz;
     X[10]=uxxx;
     X[11]=uyyy;
     X[12]=uzzz;
     X[13]=uxxy;
     X[14]=uxxz;
     X[15]=uyyx;
     X[16]=uyyz;
     X[17]=uzzx;
     X[18]=uzzy;
     X[19]=uxyz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<20;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           ResidU[i]=BY[i]-YuB[i];
     }
     //B*v-YvB=Dv
     X[0]=v;
     X[1]=vx;
     X[2]=vy;
     X[3]=vz;
     X[4]=vxx;
     X[5]=vyy;
     X[6]=vzz;
     X[7]=vxy;
     X[8]=vxz;
     X[9]=vyz;
     X[10]=vxxx;
     X[11]=vyyy;
     X[12]=vzzz;
     X[13]=vxxy;
     X[14]=vxxz;
     X[15]=vyyx;
     X[16]=vyyz;
     X[17]=vzzx;
     X[18]=vzzy;
     X[19]=vxyz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<20;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           //ResidV[i]=BY[i]-YvB[i];
     }
     //B*w-YwB=Dw
     X[0]=w;
     X[1]=wx;
     X[2]=wy;
     X[3]=wz;
     X[4]=wxx;
     X[5]=wyy;
     X[6]=wzz;
     X[7]=wxy;
     X[8]=wxz;
     X[9]=wyz;
     X[10]=wxxx;
     X[11]=wyyy;
     X[12]=wzzz;
     X[13]=wxxy;
     X[14]=wxxz;
     X[15]=wyyx;
     X[16]=wyyz;
     X[17]=wzzx;
     X[18]=wzzy;
     X[19]=wxyz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<20;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           //ResidW[i]=BY[i]-YwB[i];
     }
     //now calc mean Resiudual Energy
     kResid=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        kResid=kResid+pow(ResidU[i]*ResidU[i]+ResidV[i]*ResidV[i]+ResidW[i]*ResidW[i],0.5);
        weightSum=weightSum+1;;
     }
     kResid=kResid/weightSum;
     return kResid;
}
//---------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TpointList::makeDuError(int n)
{
     double duError,BY[300],ResidU[300];
     double weightSum,dist;


     //B*u-YuB=Du
     X[0]=u;
     X[1]=ux;
     X[2]=uy;
     X[3]=uz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<4;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           ResidU[i]=BY[i]-YuB[i];
     }

     //now calc mean Resiudual Energy
     duError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][1]*B[i][1],0.5);
        if(dist>0){
           duError=duError+pow(ResidU[i]*ResidU[i],0.5)/dist;
           weightSum=weightSum+1;
        }
     }
     if(weightSum>0){
        duError=duError/pow(weightSum,1.5);
     }
     else{
        duError=0.;
     }
     return duError;
}
//--------------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TpointList::makeDuErrorQuad(int n)
{
     double duError,BY[300],ResidU[300];
     double weightSum,dist;


     //B*u-YuB=Du
     X[0]=u;
     X[1]=ux;
     X[2]=uy;
     X[3]=uz;
     X[4]=uxx;
     X[5]=uyy;
     X[6]=uzz;
     X[7]=uxy;
     X[8]=uxz;
     X[9]=uyz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<10;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           ResidU[i]=BY[i]-YuB[i];
     }

     //now calc mean Resiudual Energy
     duError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][1]*B[i][1],0.5);
        if(dist>0){
           duError=duError+pow(ResidU[i]*ResidU[i],0.5)/dist;
           weightSum=weightSum+1;
        }
     }
     if(weightSum>0){
        duError=duError/pow(weightSum,1.5);
     }
     else{
        duError=0.;
     }
     return duError;
}
//--------------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TpointList::makeDuErrorCube(int n)
{
     double duError,BY[300],ResidU[300];
     double weightSum,dist;
  

     //B*u-YuB=Du
     X[0]=u;
     X[1]=ux;
     X[2]=uy;
     X[3]=uz;
     X[4]=uxx;
     X[5]=uyy;
     X[6]=uzz;
     X[7]=uxy;
     X[8]=uxz;
     X[9]=uyz;
     X[10]=uxxx;
     X[11]=uyyy;
     X[12]=uzzz;
     X[13]=uxxy;
     X[14]=uxxz;
     X[15]=uyyx;
     X[16]=uyyz;
     X[17]=uzzx;
     X[18]=uzzy;
     X[19]=uxyz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<20;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           ResidU[i]=BY[i]-YuB[i];
     }

     //now calc mean Resiudual Energy
     duError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][1]*B[i][1],0.5);
        if(dist>0){
           duError=duError+pow(ResidU[i]*ResidU[i],0.5)/dist;
           weightSum=weightSum+1;
        }
     }
     if(weightSum>0){
        duError=duError/pow(weightSum,1.5);
     }
     else{
        duError=0.;
     }
     return duError;
}
//-----------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TpointList::makeDvError(int n)
{
     double dvError,BY[300],ResidV[300];
     double weightSum,dist;


     //B*v-YvB=Dv
     X[0]=v;
     X[1]=vx;
     X[2]=vy;
     X[3]=vz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<4;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           //ResidV[i]=BY[i]-YvB[i];
     }

     //now calc mean Resiudual Energy
     dvError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][2]*B[i][2],0.5);

        if(dist>0){
           dvError=dvError+pow(ResidV[i]*ResidV[i],0.5)/dist;
           weightSum=weightSum+1;
        }
     }
     if(weightSum>0){
        dvError=dvError/pow(weightSum,1.5);
     }
     else{
        dvError=0.;
     }
     return dvError;
}
//--------------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TpointList::makeDvErrorQuad(int n)
{
     double dvError,BY[300],ResidV[300];
     double weightSum,dist;



     //B*v-YvB=Dv
     X[0]=v;
     X[1]=vx;
     X[2]=vy;
     X[3]=vz;
     X[4]=vxx;
     X[5]=vyy;
     X[6]=vzz;
     X[7]=vxy;
     X[8]=vxz;
     X[9]=vyz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<10;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           //ResidV[i]=BY[i]-YvB[i];
     }

     //now calc mean Resiudual Energy
     dvError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][2]*B[i][2],0.5);
        if(dist>0){
           dvError=dvError+pow(ResidV[i]*ResidV[i],0.5)/dist;
           weightSum=weightSum+1;
        }
     }
     if(weightSum>0){
        dvError=dvError/pow(weightSum,1.5);
     }
     else{
        dvError=0.;
     }
     return dvError;
}
//--------------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TpointList::makeDvErrorCube(int n)
{
     double dvError,BY[300],ResidV[300];
     double weightSum,dist;


     //B*v-YvB=Dv
     X[0]=v;
     X[1]=vx;
     X[2]=vy;
     X[3]=vz;
     X[4]=vxx;
     X[5]=vyy;
     X[6]=vzz;
     X[7]=vxy;
     X[8]=vxz;
     X[9]=vyz;
     X[10]=vxxx;
     X[11]=vyyy;
     X[12]=vzzz;
     X[13]=vxxy;
     X[14]=vxxz;
     X[15]=vyyx;
     X[16]=vyyz;
     X[17]=vzzx;
     X[18]=vzzy;
     X[19]=vxyz;

     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<20;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           //ResidV[i]=BY[i]-YvB[i];
     }

     //now calc mean Resiudual Energy
     dvError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][2]*B[i][2],0.5);
        if(dist>0){
           dvError=dvError+pow(ResidV[i]*ResidV[i],0.5)/dist;
           weightSum=weightSum+1;
        }
     }
     if(weightSum>0){
        dvError=dvError/pow(weightSum,1.5);
     }
     else{
        dvError=0.;
     }
     return dvError;
}
//---------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TpointList::makeDwError(int n)
{
     double dwError,BY[300],ResidW[300];
     double weightSum,dist;    


     //B*w-YwB=Dw
     X[0]=w;
     X[1]=wx;
     X[2]=wy;
     X[3]=wz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<4;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           //ResidW[i]=BY[i]-YwB[i];
     }
     //now calc mean Resiudual Energy
     dwError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][3]*B[i][3],0.5);
        if(dist>0){
           dwError=dwError+pow(ResidW[i]*ResidW[i],0.5)/dist;
           weightSum=weightSum+1;
        }
     }
     if(weightSum>0){
        dwError=dwError/pow(weightSum,1.5);
     }
     else{
        dwError=0.;
     }
     return dwError;
}
//--------------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TpointList::makeDwErrorQuad(int n)
{
     double dwError,BY[300],ResidW[300];
     double weightSum,dist;


     //B*w-YwB=Dw
     X[0]=w;
     X[1]=wx;
     X[2]=wy;
     X[3]=wz;
     X[4]=wxx;
     X[5]=wyy;
     X[6]=wzz;
     X[7]=wxy;
     X[8]=wxz;
     X[9]=wyz;
     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<10;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           //ResidW[i]=BY[i]-YwB[i];
     }
     //now calc mean Resiudual Energy
     dwError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][3]*B[i][3],0.5);
        if(dist>0){
           dwError=dwError+pow(ResidW[i]*ResidW[i],0.5)/dist;
           weightSum=weightSum+1;
        }
     }
     if(weightSum>0){
        dwError=dwError/pow(weightSum,1.5);
     }
     else{
        dwError=0.;
     }
     return dwError;
}
//--------------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TpointList::makeDwErrorCube(int n)
{
     double dwError,BY[300],ResidW[300];
     double weightSum,dist;


     //B*w-YwB=Dw
     X[0]=w;
     X[1]=wx;
     X[2]=wy;
     X[3]=wz;
     X[4]=wxx;
     X[5]=wyy;
     X[6]=wzz;
     X[7]=wxy;
     X[8]=wxz;
     X[9]=wyz;
     X[10]=wxxx;
     X[11]=wyyy;
     X[12]=wzzz;
     X[13]=wxxy;
     X[14]=wxxz;
     X[15]=wyyx;
     X[16]=wyyz;
     X[17]=wzzx;
     X[18]=wzzy;
     X[19]=wxyz;

     for(int i=0;i<n;i++){
           BY[i]=0.;
           for(int k=0;k<20;k++){
              BY[i]=BY[i]+B[i][k]*X[k];
           }
           //ResidW[i]=BY[i]-YwB[i];
     }
     //now calc mean Resiudual Energy
     dwError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][3]*B[i][3],0.5);
        if(dist>0){
           dwError=dwError+pow(ResidW[i]*ResidW[i],0.5)/dist;
           weightSum=weightSum+1;
        }
     }
     if(weightSum>0){
        dwError=dwError/pow(weightSum,1.5);
     }
     else{
        dwError=0.;
     }
     return dwError;
}
//--------------------------------------------------------------------------
void __fastcall TpointList::makeBTY(int n, int m,int wh)
{
     for(int i=0;i<m;i++){
           BTY[i]=0.;
           for(int k=0;k<n;k++){
               switch (wh) {
                  case 1 :
                     BTY[i]=BTY[i]+BT[i][k]*YuB[k];
                     break;
                  
               }
           }
     }
}
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------
void __fastcall TpointList::makeCTY(int n, int m,int wh,int dt)
{
     for(int i=0;i<m;i++){
           CTY[i]=0.;
           for(int k=0;k<n;k++){
               switch (wh) {
                  case 1 :
                     CTY[i]=CTY[i]+CT[i][k]*YuC[k][dt];
                     break;
                  
               }
           }
     }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall TpointList::solve(int n, int m)
{
    double faktor;
    bool ok=true;

    for(int i=1;i<m;i++){
       for(int j=i;j<m;j++){
          if(fabs(ATA[j][i-1])>0.){
             faktor=ATA[i-1][i-1]/ATA[j][i-1];
             for(int k=0;k<m;k++){
                ATA[j][k]=ATA[i-1][k]-faktor*ATA[j][k];
             }
             ATY[j]=ATY[i-1]-faktor*ATY[j];
          }
       }
    }
    for(int i=m-1;i>-1;i--){
       for(int j=i+1;j<m;j++){
          ATY[i]=ATY[i]-ATA[i][j]*X[j];
       }
       if(fabs(ATA[i][i])>0.){
          X[i]=ATY[i]/ATA[i][i];
       }
       else{
          ok=false;
       }
    }
    return ok;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::solveB(int n, int m)
{
    double faktor;
    bool ok=true;
    //double Resid[300];
    //double rmsY, rmsR,ratio;

    //bool iterateSolve=false;
    // if(mainForm->iterateCheckBox->Checked){
    //    iterateSolve=true;}

    for(int i=1;i<m;i++){
       for(int j=i;j<m;j++){
          if(fabs(BTB[j][i-1])>0.){
             faktor=BTB[i-1][i-1]/BTB[j][i-1];
             for(int k=0;k<m;k++){
                BTB[j][k]=BTB[i-1][k]-faktor*BTB[j][k];
             }
             BTY[j]=BTY[i-1]-faktor*BTY[j];
          }
       }
    }
    for(int i=m-1;i>-1;i--){
       for(int j=i+1;j<m;j++){
          BTY[i]=BTY[i]-BTB[i][j]*X[j];
       }
       if(fabs(BTB[i][i])>0.){
          X[i]=BTY[i]/BTB[i][i];
       }
       else{
          ok=false;
       }
    }

    /*rmsY=0.; rmsR=0.;
    for(int i=0;i<n;i++){
        Resid[i]=0.;
        for(int j=0;j<m;j++){
           Resid[i] = Resid[i] + B[i][j]*X[j];
        }
        Resid[i] = fabs(Resid[i]-YuB[i]);
        rmsY=rmsY+YuB[i]*YuB[i];
        rmsR=rmsR+Resid[i]*Resid[i];
    }
    rmsY=pow(rmsY/(double)n,0.5);
    rmsR=pow(rmsR/(double)n,0.5);
    if(rmsY>0){
       ratio=rmsR/rmsY;
    }
    else{
    ratio=1.;}

    return ratio; */

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::solveC(int n, int m)
{
    double faktor;
    bool ok=true;


    //bool iterateSolve=false;
    // if(mainForm->iterateCheckBox->Checked){
    //    iterateSolve=true;}

    for(int i=1;i<m;i++){
       for(int j=i;j<m;j++){
          if(fabs(CTC[j][i-1])>0.){
             faktor=CTC[i-1][i-1]/CTC[j][i-1];
             for(int k=0;k<m;k++){
                CTC[j][k]=CTC[i-1][k]-faktor*CTC[j][k];
             }
             CTY[j]=CTY[i-1]-faktor*CTY[j];
          }
       }
    }
    for(int i=m-1;i>-1;i--){
       for(int j=i+1;j<m;j++){
          CTY[i]=CTY[i]-CTC[i][j]*X[j];
       }
       if(fabs(CTC[i][i])>0.){
          X[i]=CTY[i]/CTC[i][i];
       }
       else{
          ok=false;
       }
    }


}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall TpointList::solveA(int n, int m)
{
    double faktor;
    bool ok=true;

    //bool iterateSolve=false;
    // if(mainForm->iterateCheckBox->Checked){
    //    iterateSolve=true;}

    for(int i=1;i<m;i++){
       for(int j=i;j<m;j++){
          if(fabs(ATA[j][i-1])>0.){
             faktor=ATA[i-1][i-1]/ATA[j][i-1];
             for(int k=0;k<m;k++){
                ATA[j][k]=ATA[i-1][k]-faktor*ATA[j][k];
             }
             ATY[j]=ATY[i-1]-faktor*ATY[j];
          }
       }
    }
    for(int i=m-1;i>-1;i--){
       for(int j=i+1;j<m;j++){
          ATY[i]=ATY[i]-ATA[i][j]*X[j];
       }
       if(fabs(ATA[i][i])>0.){
          X[i]=ATY[i]/ATA[i][i];
       }
       else{
          ok=false;
       }
    }
    return ok;

    /*if(iterateSolve){
       // Test C*X - Y = 0 ???
       mini=100000.;maxi=-1000000.;
       for(int i=0;i<n;i++){
           Resid[i]=0.;
           for(int j=0;j<m;j++){
               Resid[i] = Resid[i] + B[i][j]*X[j];
           }
           Resid[i] = fabs(Resid[i]-YuB[i])*1000.;//in mm
           if(Resid[i]<mini){mini=Resid[i];}
           if(Resid[i]>maxi){maxi=Resid[i];}
       }
       for(int i=0;i<n;i++){
           weight[i]=(Resid[i]-mini)/maxi;
           weight[i]=1.-1./(1.+exp(-20.*(weight[i]-0.75)));
       }
    } */

}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void __fastcall TpointList::autoCorrelation()
{
    double dist,co,va,cox,vax,coy,vay,coz,vaz,Aco,Ava,Acox,Avax,Acoy,Avay,Acoz,Avaz;
    double pPerRa[400],minDist[10];
    int index,rank;
    int in[5];
    for (int k=0;k<400;k++){
       pPerRa[k]=0;
    }
    int stepSize=StrToInt(graphForm->stepEdit->Text);


    for(int i=1;i<point[2][0][0];i=i+stepSize){
     if(point[2][i][11]>0.){
       for (int k=0;k<400;k++){
          pPerRa[k]=0;
       }
       for(int j=0;j<10;j++){
          minDist[j]=40.;
       }
       //for(int j=i+1;j<point[2][0][0]+1;j++){
       for(int j=1;j<point[2][0][0]+1;j++){
          if(point[2][j][11]>0.){
             dist=pow( pow(point[2][i][2]-point[2][j][2],2.)
                      +pow(point[2][i][3]-point[2][j][3],2.)
                      +pow(point[2][i][4]-point[2][j][4],2.),0.5);
             if(dist<minDist[9] && dist>0){
                 rank=9;
                 for(int k=9;k>-1;k--){
                    if(dist<minDist[k]){
                       rank=k;
                    }
                 }
                 for(int l=9;l>rank;l--){
                    minDist[l]=minDist[l-1];
                 }
                 minDist[rank]=dist;
             }
             index=(int)(dist*10000.+0.5);
             co=(point[2][i][5]-meaU[1])*(point[2][j][5]-meaU[1]);
             va=(point[2][i][5]-meaU[1])*(point[2][i][5]-meaU[1]);
             cox=(point[2][i][5]-meaU[1])*(point[2][j][5]-meaU[1]);
             vax=(point[2][i][5]-meaU[1])*(point[2][i][5]-meaU[1]);
             coy=(point[2][i][6]-meaV[1])*(point[2][j][6]-meaV[1]);
             vay=(point[2][i][6]-meaV[1])*(point[2][i][6]-meaV[1]);
             coz=(point[2][i][7]-meaW[1])*(point[2][j][7]-meaW[1]);
             vaz=(point[2][i][7]-meaW[1])*(point[2][i][7]-meaW[1]);
             Aco=(point[2][i][8]-meaAx[1])*(point[2][j][5]-meaAx[1]);
             Ava=(point[2][i][8]-meaAx[1])*(point[2][i][5]-meaAx[1]);
             Acox=(point[2][i][8]-meaAx[1])*(point[2][j][8]-meaAx[1]);
             Avax=(point[2][i][8]-meaAx[1])*(point[2][i][8]-meaAx[1]);
             Acoy=(point[2][i][9]-meaAy[1])*(point[2][j][9]-meaAy[1]);
             Avay=(point[2][i][9]-meaAy[1])*(point[2][i][9]-meaAy[1]);
             Acoz=(point[2][i][10]-meaAz[1])*(point[2][j][10]-meaAz[1]);
             Avaz=(point[2][i][10]-meaAz[1])*(point[2][i][10]-meaAz[1]);
             if(index<300){
                cov[index][0]=cov[index][0]+1;
                cov[index][1]=(cov[index][1]*(cov[index][0]-1)+co)/cov[index][0];
                var[index][0]=var[index][0]+1;
                var[index][1]=(var[index][1]*(var[index][0]-1)+va)/var[index][0];
                covx[index][0]=covx[index][0]+1;
                covx[index][1]=(covx[index][1]*(covx[index][0]-1)+cox)/covx[index][0];
                varx[index][0]=varx[index][0]+1;
                varx[index][1]=(varx[index][1]*(varx[index][0]-1)+vax)/varx[index][0];
                covy[index][0]=covy[index][0]+1;
                covy[index][1]=(covy[index][1]*(covy[index][0]-1)+coy)/covy[index][0];
                vary[index][0]=vary[index][0]+1;
                vary[index][1]=(vary[index][1]*(vary[index][0]-1)+vay)/vary[index][0];
                covz[index][0]=covz[index][0]+1;
                covz[index][1]=(covz[index][1]*(covz[index][0]-1)+coz)/covz[index][0];
                varz[index][0]=varz[index][0]+1;
                varz[index][1]=(varz[index][1]*(varz[index][0]-1)+vaz)/varz[index][0];
                Acov[index][0]=Acov[index][0]+1;
                Acov[index][1]=(Acov[index][1]*(Acov[index][0]-1)+Aco)/Acov[index][0];
                Avar[index][0]=Avar[index][0]+1;
                Avar[index][1]=(Avar[index][1]*(Avar[index][0]-1)+Ava)/Avar[index][0];
                Acovx[index][0]=Acovx[index][0]+1;
                Acovx[index][1]=(Acovx[index][1]*(Acovx[index][0]-1)+Acox)/Acovx[index][0];
                Avarx[index][0]=Avarx[index][0]+1;
                Avarx[index][1]=(Avarx[index][1]*(Avarx[index][0]-1)+Avax)/Avarx[index][0];
                Acovy[index][0]=Acovy[index][0]+1;
                Acovy[index][1]=(Acovy[index][1]*(Acovy[index][0]-1)+Acoy)/Acovy[index][0];
                Avary[index][0]=Avary[index][0]+1;
                Avary[index][1]=(Avary[index][1]*(Avary[index][0]-1)+Avay)/Avary[index][0];
                Acovz[index][0]=Acovz[index][0]+1;
                Acovz[index][1]=(Acovz[index][1]*(Acovz[index][0]-1)+Acoz)/Acovz[index][0];
                Avarz[index][0]=Avarz[index][0]+1;
                Avarz[index][1]=(Avarz[index][1]*(Avarz[index][0]-1)+Avaz)/Avarz[index][0];
             }
             //pointPerRadius stuff
             index=(int)(dist*10000.+0.5);
             for (int k=0;k<400;k++){
                if(index<k){
                   pPerRa[k]=pPerRa[k]+1;
                }
             }
          }
       }
       //update of minDist;
       for(int j=0;j<10;j++){
          if(minDist[j]>0 && minDist[j]<0.04){
              index=(int)(minDist[j]*10000.+0.5);
              if(index<400){
                  minDistArray[j][index]=minDistArray[j][index]+1;
              }
          }
       }

       for(int j=0;j<5;j++){
          in[j]=40000;
       }
       if(point[2][i][11]>0.){
           in[2]=i;
       }
       if(in[2]<40000 && point[2][in[2]][1]>0.){
           if(point[3][(int)point[2][in[2]][1]][11]>0){
               in[3]=point[2][in[2]][1];
           }
       }
       if(in[3]<40000 && point[3][in[3]][1]>0.){
           if(point[4][(int)point[3][in[3]][1]][11]>0){
               in[4]=point[3][in[3]][1];
           }
       }
       if(in[2]<40000 && point[2][in[2]][0]>0.){
           if(point[1][(int)point[2][in[2]][0]][11]>0){
               in[1]=point[2][in[2]][0];
           }
       }
       if(in[1]<40000 && point[1][in[1]][0]>0.){
           if(point[0][(int)point[1][in[1]][0]][11]>0){
               in[0]=point[1][in[1]][0];
           }
       }


       //pointPerRadius
       for (int k=0;k<400;k++){
           pointPerRadius[k][0]=pointPerRadius[k][0]+1;
           pointPerRadius[k][1]=(pointPerRadius[k][1]*(pointPerRadius[k][0]-1)+pPerRa[k])/pointPerRadius[k][0];
       }
      }
    }
    cor[0]=1.;

    //draw correlation stuff
    graphForm->Series1->Clear();
    graphForm->Series2->Clear();
    graphForm->Series3->Clear();
    
    for(int i=0;i<300;i++){
       //if(normalizerX>0.){
          //cor[i]=cov[i][1]/var[i][1];
          corx[i]=covx[i][1]/varx[0][1]; ///normalizerX;//
          cory[i]=covy[i][1]/vary[0][1];
          corz[i]=covz[i][1]/varz[0][1];
          graphForm->Series1->AddXY((double)i/10.,corx[i],'.',clTeeColor);
          graphForm->Series2->AddXY((double)i/10.,cory[i],'.',clTeeColor);
          graphForm->Series3->AddXY((double)i/10.,corz[i],'.',clTeeColor);
       //}
    }

    graphForm->Series5->Clear();
    graphForm->Series6->Clear();
    graphForm->Series7->Clear();
   
    for(int i=0;i<300;i++){
       //if(normalizerX>0.){
          //Acor[i]=Acov[i][1]/Avar[i][1];
          Acorx[i]=Acovx[i][1]/Avarx[0][1]; ///normalizerX;//
          Acory[i]=Acovy[i][1]/Avary[0][1];
          Acorz[i]=Acovz[i][1]/Avarz[0][1];
          graphForm->Series5->AddXY((double)i/10.,Acorx[i],'.',clTeeColor);
          graphForm->Series6->AddXY((double)i/10.,Acory[i],'.',clTeeColor);
          graphForm->Series7->AddXY((double)i/10.,Acorz[i],'.',clTeeColor);
       //}
    }

    
   
    //draw points per radius stuff stuff
    graphForm->Series4->Clear();
    for(int i=0;i<400;i++){
       graphForm->Series4->AddXY(((double)i)/10.,pointPerRadius[i][1],'.',clTeeColor);
    }

    graphForm->Series14->Clear();
    graphForm->Series15->Clear();
    graphForm->Series16->Clear();
    graphForm->Series17->Clear();
    graphForm->Series18->Clear();
    graphForm->Series19->Clear();
    graphForm->Series20->Clear();
    graphForm->Series21->Clear();
    graphForm->Series22->Clear();
    graphForm->Series23->Clear();
    for(int i=1;i<400;i++){
       graphForm->Series14->AddXY((double)i/10.,minDistArray[0][i],'.',clTeeColor);
       graphForm->Series15->AddXY((double)i/10.,minDistArray[1][i],'.',clTeeColor);
       graphForm->Series16->AddXY((double)i/10.,minDistArray[2][i],'.',clTeeColor);
       graphForm->Series17->AddXY((double)i/10.,minDistArray[3][i],'.',clTeeColor);
       graphForm->Series18->AddXY((double)i/10.,minDistArray[4][i],'.',clTeeColor);
       graphForm->Series19->AddXY((double)i/10.,minDistArray[5][i],'.',clTeeColor);
       graphForm->Series20->AddXY((double)i/10.,minDistArray[6][i],'.',clTeeColor);
       graphForm->Series21->AddXY((double)i/10.,minDistArray[7][i],'.',clTeeColor);
       graphForm->Series22->AddXY((double)i/10.,minDistArray[8][i],'.',clTeeColor);
       graphForm->Series23->AddXY((double)i/10.,minDistArray[9][i],'.',clTeeColor);
    }

    graphForm->Refresh();


}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::autoCorrelationFast()
{
    double dist,co,va,dup,lx,ly,lz;
    double KolConst=2.;
    int index;
    for(int i=1;i<point[2][0][0];i++){
     if(point[2][i][11]>0.){
       for(int j=i+1;j<point[2][0][0]+1;j++){
          if(point[2][j][11]>0.){
             dist=pow( pow(point[2][i][2]-point[2][j][2],2.)
                      +pow(point[2][i][3]-point[2][j][3],2.)
                      +pow(point[2][i][4]-point[2][j][4],2.),0.5);
             index=(int)(dist*1000.+0.5);
             co=(point[2][i][5]-meaU[1])*(point[2][j][5]-meaU[1]);
             va=(point[2][i][5]-meaU[1])*(point[2][i][5]-meaU[1]);
             if(index<30){
                cov[index][0]=cov[index][0]+1;
                cov[index][1]=(cov[index][1]*(cov[index][0]-1)+co)/cov[index][0];
                var[index][0]=var[index][0]+1;
                var[index][1]=(var[index][1]*(var[index][0]-1)+va)/var[index][0];

                if(dist>0){
                   lx=(point[2][j][2]-point[2][i][2])/dist;
                   ly=(point[2][j][3]-point[2][i][3])/dist;
                   lz=(point[2][j][4]-point[2][i][4])/dist;
                   dup= (point[2][j][5]-point[2][i][5])*lx
                       +(point[2][j][6]-point[2][i][6])*ly
                       +(point[2][j][7]-point[2][i][7])*lz;
                   dup=dup;

                   //update arrays
                   valid[index][0]=valid[index][0]+1;
                   valid[index][1]=  (
                                       valid[index][1]*(valid[index][0]-1)
                                       +pow( dup*dup/KolConst,1.5 )/(dist)
                                     )/ valid[index][0];
                }
             }
          }
       }
      }
    }
    cor[0]=1.;

    double maxi=0;
    for(int i=0;i<30;i++){
       if(valid[i][1]>maxi){
          maxi=valid[i][1];
       }
    }
    maxi=2.2e-5;
    for(int i=0;i<30;i++){
       valid[i][2]=(maxi-valid[i][1])/maxi;
       //valid[i][2]=valid[i][1]/maxi;
    }

    //draw correlation stuff
    krigingForm->Series1->Clear();
    krigingForm->Series2->Clear();
    for(int i=0;i<30;i++){
       if(var[i][1]>0.){
          cor[i]=cov[i][1]/var[i][1];
          krigingForm->Series1->AddXY((double)i,cor[i],'.',clTeeColor);
          krigingForm->Series2->AddXY((double)i,valid[i][2],'.',clTeeColor);
       }
    }
    krigingForm->Refresh();
}
//--------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeKrigingWeightsInnerLoop()
{
    double dist,dup,lx,ly,lz,maxi;
    double KolConst=2.;
    int index;
    for(int i=1;i<point[2][0][0];i++){
     if(point[2][i][11]>0.){
       for(int j=i+1;j<point[2][0][0]+1;j++){
          if(point[2][j][11]>0.){
             dist=pow( pow(point[2][i][2]-point[2][j][2],2.)
                      +pow(point[2][i][3]-point[2][j][3],2.)
                      +pow(point[2][i][4]-point[2][j][4],2.),0.5);
             index=(int)(dist*1000.+0.5);
             if(index<30){
                if(dist>0){
                   lx=(point[2][j][2]-point[2][i][2])/dist;
                   ly=(point[2][j][3]-point[2][i][3])/dist;
                   lz=(point[2][j][4]-point[2][i][4])/dist;
                   dup= (point[2][j][5]-point[2][i][5])*lx
                       +(point[2][j][6]-point[2][i][6])*ly
                       +(point[2][j][7]-point[2][i][7])*lz;
                   dup=dup;

                   //update arrays
                   valid[index][0]=valid[index][0]+1;
                   valid[index][1]=  (
                                       valid[index][1]*(valid[index][0]-1)
                                       +pow( dup*dup/KolConst,1.5 )/(dist)
                                     )/ valid[index][0];
                }
             }
          }
       }
      }
    }


    maxi=StrToFloat(krigingForm->maxiEdit->Text);//2.2e-5;
    bool zeroCross=false;
    for(int i=0;i<30;i++){
       valid[i][2]=(maxi-valid[i][1])/maxi;
       if(valid[i][2]<0.01 || zeroCross){
          zeroCross=true;
          valid[i][2]=0.01;
       }
    }

    //draw kriging stuff
    krigingForm->Series2->Clear();
    for(int i=0;i<30;i++){
       krigingForm->Series2->AddXY((double)i,valid[i][2],'.',clTeeColor);
    }
    krigingForm->Refresh();
}
//----------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::getMean()
{
    double mU=0,mV=0,mW=0,mAx=0,mAy=0,mAz=0;
    double count=0;

    for(int i=1;i<point[2][0][0];i++){
       if(point[2][i][11]>0.){
          count++;
          mU=mU+point[2][i][5];
          mV=mV+point[2][i][6];
          mW=mW+point[2][i][7];
          mAx=mAx+point[2][i][8];
          mAy=mAy+point[2][i][9];
          mAz=mAz+point[2][i][10];
       }
    }
    if(count>0){
    mU=mU/count;
    mV=mV/count;
    mW=mW/count;
    mAx=mAx/count;
    mAy=mAy/count;
    mAz=mAz/count;

    meaU[0]=meaU[0]+1;
    meaU[1]=(meaU[1]*(meaU[0]-1)+mU)/meaU[0];
    meaV[0]=meaV[0]+1;
    meaV[1]=(meaV[1]*(meaV[0]-1)+mV)/meaV[0];
    meaW[0]=meaW[0]+1;
    meaW[1]=(meaW[1]*(meaW[0]-1)+mW)/meaW[0];

    meaAx[0]=meaAx[0]+1;
    meaAx[1]=(meaAx[1]*(meaAx[0]-1)+mAx)/meaAx[0];
    meaAy[0]=meaAy[0]+1;
    meaAy[1]=(meaAy[1]*(meaAy[0]-1)+mAy)/meaAy[0];
    meaAz[0]=meaAz[0]+1;
    meaAz[1]=(meaAz[1]*(meaAz[0]-1)+mAz)/meaAz[0];
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::readXUAPFileOld(int n, bool shift)
{
    FILE *fpp;
    int numOfPoints;
    double left,right,x,y,z,u,v,w,ax,ay,az,dummy,cubic,quality;

    AnsiString name;
    const char *filename;
    for(int i=0;i<5;i++){
       if(n-2+i>firstFile-1 && n-2+i<lastFile+1){
          if(i<4 && shift){
             for(int j=0;j<point[i+1][0][0]+1;j++){
                 for(int k=0;k<12;k++){
                     point[i][j][k]=point[i+1][j][k];
                 }
             }
          }
          else{
             numOfPoints=0;
             name=baseName+IntToStr(n-2+i);
             filename=name.c_str();
             fpp = fopen(filename,"r");
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
                //fscanf (fpp, "%lf\0", &quality);
                point[i][numOfPoints][0]=left;
                point[i][numOfPoints][1]=right;
                point[i][numOfPoints][2]=x;
                point[i][numOfPoints][3]=y;
                point[i][numOfPoints][4]=z;
                point[i][numOfPoints][5]=u;
                point[i][numOfPoints][6]=v;
                point[i][numOfPoints][7]=w;
                point[i][numOfPoints][8]=ax;
                point[i][numOfPoints][9]=ay;
                point[i][numOfPoints][10]=az;
                point[i][numOfPoints][11]=cubic;
                //new quality
                //point[i][numOfPoints][12]=quality;
             }
             fclose (fpp);
             point[i][0][0]=numOfPoints++;
          }

       }
       else{
          point[i][0][0]=0;
       }
    }
}
//-----------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeCor()
{
     for(int j=0;j<10;j++){
        for(int i=0;i<100;i++){
           minDistArray[j][i]=0;
        }
     }
     for(int i=0;i<300;i++){
        cov[i][0]=0.;
        cov[i][1]=0.;
        var[i][0]=0.;
        var[i][1]=0.;
        covx[i][0]=0.;
        covx[i][1]=0.;
        varx[i][0]=0.;
        varx[i][1]=0.;
        covy[i][0]=0.;
        covy[i][1]=0.;
        vary[i][0]=0.;
        vary[i][1]=0.;
        covz[i][0]=0.;
        covz[i][1]=0.;
        varz[i][0]=0.;
        varz[i][1]=0.;
        Acov[i][0]=0.;
        Acov[i][1]=0.;
        Avar[i][0]=0.;
        Avar[i][1]=0.;
        Acovx[i][0]=0.;
        Acovx[i][1]=0.;
        Avarx[i][0]=0.;
        Avarx[i][1]=0.;
        Acovy[i][0]=0.;
        Acovy[i][1]=0.;
        Avary[i][0]=0.;
        Avary[i][1]=0.;
        Acovz[i][0]=0.;
        Acovz[i][1]=0.;
        Avarz[i][0]=0.;
        Avarz[i][1]=0.;
     }

     

     int stepSizeFile=StrToInt(graphForm->stepFileEdit->Text);
     for (int i=firstFile;i<lastFile+1;i=i+stepSizeFile){
        mainForm->fileNum2Edit->Text=IntToStr(i);
        mainForm->fileNum2Edit->Refresh();
        graphForm->fileNum2Edit->Text=IntToStr(i);
        graphForm->fileNum2Edit->Refresh();
        if(i>firstFile){
           readXUAPFileOld(i,true);
        }
        else{
           readXUAPFileOld(i,false);
        }
        getMean();
     }
     for (int i=firstFile;i<lastFile+1;i=i+stepSizeFile){
        mainForm->fileNum2Edit->Text=IntToStr(i);
        mainForm->fileNum2Edit->Refresh();
        graphForm->fileNum2Edit->Text=IntToStr(i);
        graphForm->fileNum2Edit->Refresh();
        if(i>firstFile){
           readXUAPFileOld(i,true);
        }
        else{
           readXUAPFileOld(i,false);
        }
        autoCorrelation();
     }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeStruct()
{

     double dist,lx,ly,lz,dup,duda;
     int index;
     double ua,uaMean,counter,counterArray[201],secondArray[201],thirdArray[201],thirdArrayYeung[201],dudaArray[201];
     double KolConst=StrToFloat(structForm->KolConstEdit->Text);
     int stepSizeFile=StrToInt(structForm->stepFileEdit->Text);
     int stepSize=StrToInt(structForm->stepEdit->Text);

     double meanU=0,meanV=0,meanW=0,meanAx=0,meanAy=0,meanAz=0;

     counter=0;
     uaMean=0;
     for(int i=0;i<201;i++){
        counterArray[i]=0;
        secondArray[i]=0;
        thirdArray[i]=0;
        thirdArrayYeung[i]=0; //K=0.4 not 0.8
        dudaArray[i]=0;
     }


     for (int i=firstFile;i<lastFile+1;i=i+stepSizeFile){
        structForm->fileNumEdit->Text=IntToStr(i);
        structForm->fileNumEdit->Refresh();
        readXUAPFile(i,true,true);
        
        //update structures, 2nd, 3d, du da
        for(int j=1;j<point[2][0][0]-1;j=j+stepSize){
          if(point[2][j][11]>0.){
             ua= point[2][j][5]*point[2][j][8]
                +point[2][j][6]*point[2][j][9]
                +point[2][j][7]*point[2][j][10];

             meanU=meanU+point[2][j][5];
             meanV=meanV+point[2][j][6];
             meanW=meanW+point[2][j][7];
             meanAx=meanAx+point[2][j][8];
             meanAy=meanAy+point[2][j][9];
             meanAz=meanAz+point[2][j][10];

             counter=counter+1;
             for(int k=j+1;k<point[2][0][0];k++){
                if(point[2][k][11]>0.){
                   dist=pow( pow(point[2][k][2]-point[2][j][2],2.)
                            +pow(point[2][k][3]-point[2][j][3],2.)
                            +pow(point[2][k][4]-point[2][j][4],2.),0.5);
                   // 1/10 of mm resolution up to 20mm, say
                   index=(int)(dist*10000.+0.5);
                   if(index<201 && dist>0){
                      //delta u ||
                         // ||l||
                      lx=(point[2][k][2]-point[2][j][2])/dist;
                      ly=(point[2][k][3]-point[2][j][3])/dist;
                      lz=(point[2][k][4]-point[2][j][4])/dist;
                      dup= (point[2][k][5]-point[2][j][5])*lx
                          +(point[2][k][6]-point[2][j][6])*ly
                          +(point[2][k][7]-point[2][j][7])*lz;
                      dup=dup;
                      //du * da
                      duda= (point[2][k][5]-point[2][j][5])*(point[2][k][8]-point[2][j][8])
                           +(point[2][k][6]-point[2][j][6])*(point[2][k][9]-point[2][j][9])
                           +(point[2][k][7]-point[2][j][7])*(point[2][k][10]-point[2][j][10]);
                      //update arrays
                      counterArray[index]=counterArray[index]+1;
                      secondArray[index]=  (
                                              secondArray[index]*(counterArray[index]-1)
                                             +pow( dup*dup/KolConst,1.5 )/(dist)
                                            )/ counterArray[index];
                      thirdArray[index] =  (
                                              thirdArray[index]*(counterArray[index]-1)
                                             +(-dup*dup*dup*1.25)/dist
                                            )/ counterArray[index];
                      thirdArrayYeung[index] =  (
                                              thirdArrayYeung[index]*(counterArray[index]-1)
                                             +(-dup*dup*dup*2.5)/dist
                                            )/ counterArray[index];
                      dudaArray[index]  =  (
                                              dudaArray[index]*(counterArray[index]-1)
                                             -0.5*duda
                                            )/ counterArray[index];
                   }
                }
             }
             uaMean=uaMean-ua;
          }
        }
        //draw structure stuff
        structForm->Series1->Clear(); //second
        structForm->Series2->Clear(); //third
        structForm->Series3->Clear(); //third, Yeung
        structForm->Series4->Clear(); //du * da
        structForm->Series5->Clear(); //u*a
        for(int j=0;j<200;j++){
           structForm->Series1->AddXY((double)j/10.,1e6*secondArray[j],'.',clTeeColor);
           structForm->Series2->AddXY((double)j/10.,1e6*thirdArray[j],'.',clTeeColor);
           structForm->Series3->AddXY((double)j/10.,1e6*thirdArrayYeung[j],'.',clTeeColor);
           structForm->Series4->AddXY((double)j/10.,1e6*dudaArray[j],'.',clTeeColor);
        }
        structForm->Series5->AddXY((double)0/10.,1e6*(uaMean/counter),'.',clTeeColor);
        structForm->Series5->AddXY((double)200/10.,1e6*(uaMean/counter),'.',clTeeColor);

        structForm->uEdit->Text=FloatToStr(meanU/counter);
        structForm->vEdit->Text=FloatToStr(meanV/counter);
        structForm->wEdit->Text=FloatToStr(meanW/counter);
        structForm->axEdit->Text=FloatToStr(meanAx/counter);
        structForm->AyEdit->Text=FloatToStr(meanAy/counter);
        structForm->AzEdit->Text=FloatToStr(meanAz/counter);

        structForm->Refresh();
     }

    meanU=meanU/counter;
    meanV=meanV/counter;
    meanW=meanW/counter;
    meanAx=meanAx/counter;
    meanAy=meanAy/counter;
    meanAz=meanAz/counter;

    counter=0;
     uaMean=0;
     for(int i=0;i<201;i++){
        counterArray[i]=0;
        secondArray[i]=0;
        thirdArray[i]=0;
        thirdArrayYeung[i]=0; //K=0.4 not 0.8
        dudaArray[i]=0;
     }

    /*for (int i=firstFile;i<lastFile+1;i=i+stepSizeFile){
        structForm->fileNumEdit->Text=IntToStr(i);
        structForm->fileNumEdit->Refresh();
        readXUAPFile(i,true,true);
        
        //update structures, 2nd, 3d, du da
        for(int j=1;j<point[2][0][0]-1;j=j+stepSize){
          if(point[2][j][11]>0.){
             ua= (point[2][j][5]-meanU)*(point[2][j][8]-meanAx)
                +(point[2][j][6]-meanV)*(point[2][j][9]-meanAy)
                +(point[2][j][7]-meanW)*(point[2][j][10]-meanAz);

             counter=counter+1;
             for(int k=j+1;k<point[2][0][0];k++){
                if(point[2][k][11]>0.){
                   dist=pow( pow(point[2][k][2]-point[2][j][2],2.)
                            +pow(point[2][k][3]-point[2][j][3],2.)
                            +pow(point[2][k][4]-point[2][j][4],2.),0.5);
                   // 1/10 of mm resolution up to 20mm, say
                   index=(int)(dist*10000.+0.5);
                   if(index<201 && dist>0){
                      //delta u ||
                         // ||l||
                      lx=(point[2][k][2]-point[2][j][2])/dist;
                      ly=(point[2][k][3]-point[2][j][3])/dist;
                      lz=(point[2][k][4]-point[2][j][4])/dist;
                      dup= ((point[2][k][5]-meanU)-(point[2][j][5]-meanU))*lx
                          +((point[2][k][6]-meanV)-(point[2][j][6]-meanV))*ly
                          +((point[2][k][7]-meanW)-(point[2][j][7]-meanW))*lz;
                      dup=dup;
                      //du * da
                      duda= ((point[2][k][5]-meanU)-(point[2][j][5]-meanU))*((point[2][k][8]-meanAx)-(point[2][j][8]-meanAx))
                           +((point[2][k][6]-meanV)-(point[2][j][6]-meanV))*((point[2][k][9]-meanAy)-(point[2][j][9]-meanAy))
                           +((point[2][k][7]-meanW)-(point[2][j][7]-meanW))*((point[2][k][10]-meanAz)-(point[2][j][10]-meanAz));
                      //update arrays
                      counterArray[index]=counterArray[index]+1;
                      secondArray[index]=  (
                                              secondArray[index]*(counterArray[index]-1)
                                             +pow( dup*dup/KolConst,1.5 )/(dist)
                                            )/ counterArray[index];
                      thirdArray[index] =  (
                                              thirdArray[index]*(counterArray[index]-1)
                                             +(-dup*dup*dup*1.25)/dist
                                            )/ counterArray[index];
                      thirdArrayYeung[index] =  (
                                              thirdArrayYeung[index]*(counterArray[index]-1)
                                             +(-dup*dup*dup*2.5)/dist
                                            )/ counterArray[index];
                      dudaArray[index]  =  (
                                              dudaArray[index]*(counterArray[index]-1)
                                             -0.5*duda
                                            )/ counterArray[index];
                   }
                }
             }
             uaMean=uaMean-ua;
          }
        }
        //draw structure stuff
        structForm->Series1->Clear(); //second
        structForm->Series2->Clear(); //third
        structForm->Series3->Clear(); //third, Yeung
        structForm->Series4->Clear(); //du * da
        structForm->Series5->Clear(); //u*a
        for(int j=0;j<200;j++){
           structForm->Series1->AddXY((double)j/10.,1e6*secondArray[j],'.',clTeeColor);
           structForm->Series2->AddXY((double)j/10.,1e6*thirdArray[j],'.',clTeeColor);
           structForm->Series3->AddXY((double)j/10.,1e6*thirdArrayYeung[j],'.',clTeeColor);
           structForm->Series4->AddXY((double)j/10.,1e6*dudaArray[j],'.',clTeeColor);
        }
        structForm->Series5->AddXY((double)0/10.,1e6*(uaMean/counter),'.',clTeeColor);
        structForm->Series5->AddXY((double)200/10.,1e6*(uaMean/counter),'.',clTeeColor);

        structForm->uEdit->Text=FloatToStr(meanU);
        structForm->vEdit->Text=FloatToStr(meanV);
        structForm->wEdit->Text=FloatToStr(meanW);
        structForm->axEdit->Text=FloatToStr(meanAx);
        structForm->AyEdit->Text=FloatToStr(meanAy);
        structForm->AzEdit->Text=FloatToStr(meanAz);

        structForm->Refresh();
     }*/

    FILE *fpp;
    AnsiString name;
    name="structures.res";
    const char *filename;
    filename=name.c_str();
    fpp = fopen(filename,"w");
    
    for(int i=0;i<201;i++){

        fprintf(fpp, "%lf\t", 1e6*secondArray[i]);
        fprintf(fpp, "%lf\t", 1e6*thirdArray[i]);
        fprintf(fpp, "%lf\t", 1e6*thirdArrayYeung[i]);
        fprintf(fpp, "%lf\n", 1e6*dudaArray[i]);

    }
    fclose (fpp);
}
//---------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeStructRepr()
{

     double dist,lx,ly,lz,du,dup,duo,duda;
     int index;
     double ua,uaMean,counter,counterArray[201],secondArray[201],thirdArray[201],thirdArrayYeung[201],dudaArray[201];
     double belowCounterArray[201],aboveCounterArray[201];
     double secondParalBelowArray[201],secondParalAboveArray[201],secondOrthoBelowArray[201],secondOrthoAboveArray[201];
     double dudaBelowArray[201],dudaAboveArray[201];
     double uKinBelowArray[1001],uKinAboveArray[1001],aSqBelowArray[1001],aSqAboveArray[1001];
     double uKinBelowArrayPlot[1001],uKinAboveArrayPlot[1001],aSqBelowArrayPlot[1001],aSqAboveArrayPlot[1001];
     double KolConst=StrToFloat(structReprForm->KolConstEdit->Text);
     double tail=8;//StrToFloat(paramForm->tailCutEdit->Text);

     double upArray[201],deltaVelBelowArray[201],deltaVelAboveArray[201];

     counter=0;
     uaMean=0;

     int pdfSize=1001;
     double uKin;
     double aSqu;
     double uKinMax=1e-3;
     double aSquMax=1e-2;
     //double sumUkin;
     //double sumaSqu;
     double sumUkinBelow;
     double sumUkinAbove;
     double sumaSquBelow;
     double sumaSquAbove;

     double dx,dy,dz,deltaU,deltaV,deltaW,deltaVel;

     for(int i=0;i<201;i++){
        counterArray[i]=0;
        belowCounterArray[i]=0;
        aboveCounterArray[i]=0;

        secondArray[i]=0;
        thirdArray[i]=0;
        thirdArrayYeung[i]=0; //K=0.4 not 0.8
        dudaArray[i]=0;

        secondParalBelowArray[i]=0;
        secondParalAboveArray[i]=0;
        secondOrthoBelowArray[i]=0;
        secondOrthoAboveArray[i]=0;

        dudaBelowArray[i]=0;
        dudaAboveArray[i]=0;

        upArray[i]=0;
        deltaVelBelowArray[i]=0;
        deltaVelAboveArray[i]=0;

     }
     for(int i=0;i<pdfSize;i++){
        uKinBelowArray[i]=0;
        uKinAboveArray[i]=0;
        aSqBelowArray[i]=0;
        aSqAboveArray[i]=0;
     }


     for (int i=firstFile;i<lastFile+1;i++){
        structReprForm->fileNumEdit->Text=IntToStr(i);
        structReprForm->fileNumEdit->Refresh();
        readXUAPDFile(i,true,true);
        
        //update structures, 2nd, 3d, du da
        for(int j=1;j<point[2][0][0]-1;j++){
          if(point[2][j][11]>0. && point[2][j][14]>tail && point[2][j][14]<point[2][j][15]-tail){
             ua= point[2][j][5]*point[2][j][8]
                +point[2][j][6]*point[2][j][9]
                +point[2][j][7]*point[2][j][10];
             counter=counter+1;

             uKin= point[2][j][5]*point[2][j][5]
                  +point[2][j][6]*point[2][j][6]
                  +point[2][j][7]*point[2][j][7];
             aSqu= point[2][j][8]*point[2][j][8]
                  +point[2][j][9]*point[2][j][9]
                  +point[2][j][10]*point[2][j][10];
             index=(int)(uKin*pdfSize/uKinMax+0.5);
             if(index>0 && index<pdfSize){
                if(point[2][j][13]<0.1){
                   uKinBelowArray[index]=uKinBelowArray[index]+1;
                }
                //if(1<2){//(point[2][j][13]>0.1){
                   uKinAboveArray[index]=uKinAboveArray[index]+1;
                //}
             }
             index=(int)(aSqu*pdfSize/aSquMax+0.5);
             if(index>0 && index<pdfSize){
                if(point[2][j][13]<0.1){
                   aSqBelowArray[index]=aSqBelowArray[index]+1;
                }
                //if(1<2){//(point[2][j][13]>0.1){
                   aSqAboveArray[index]=aSqAboveArray[index]+1;
                //}
             }

             for(int k=j+1;k<point[2][0][0];k++){
                if(point[2][k][11]>0. && point[2][k][14]>tail && point[2][k][14]<point[2][k][15]-tail){
                   dist=pow( pow(point[2][k][2]-point[2][j][2],2.)
                            +pow(point[2][k][3]-point[2][j][3],2.)
                            +pow(point[2][k][4]-point[2][j][4],2.),0.5);
                   // 1/10 of mm resolution up to 20mm, say
                   index=(int)(dist*10000.+0.5);
                   if(index<201 && dist>0){
                      //delta u ||
                         // ||l||
                      lx=(point[2][k][2]-point[2][j][2])/dist;
                      ly=(point[2][k][3]-point[2][j][3])/dist;
                      lz=(point[2][k][4]-point[2][j][4])/dist;
                      dup= (point[2][k][5]-point[2][j][5])*lx
                          +(point[2][k][6]-point[2][j][6])*ly
                          +(point[2][k][7]-point[2][j][7])*lz;
                      du=  pow( pow(point[2][k][5]-point[2][j][5],2.)
                               +pow(point[2][k][6]-point[2][j][6],2.)
                               +pow(point[2][k][7]-point[2][j][7],2.),0.5);
                      duo=pow(du*du-dup*dup,0.5);
                      dup=dup;
                      //du * da
                      duda= (point[2][k][5]-point[2][j][5])*(point[2][k][8]-point[2][j][8])
                           +(point[2][k][6]-point[2][j][6])*(point[2][k][9]-point[2][j][9])
                           +(point[2][k][7]-point[2][j][7])*(point[2][k][10]-point[2][j][10]);

                      // ms u'
                      //=du*du;
                      // ms u(x+dx)-u(x)-du/dx*dx
                      dx= 1.*(point[2][k][2]-point[2][j][2]);
                      dy= 1.*(point[2][k][3]-point[2][j][3]);
                      dz= 1.*(point[2][k][4]-point[2][j][4]);
                      deltaU=point[2][k][5]-(point[2][j][5]+point[2][j][16]*dx
                                                           +point[2][j][17]*dy
                                                           +point[2][j][18]*dz);
                      deltaV=point[2][k][6]-(point[2][j][6]+point[2][j][19]*dx
                                                           +point[2][j][20]*dy
                                                           +point[2][j][21]*dz);
                      deltaW=point[2][k][7]-(point[2][j][7]+point[2][j][22]*dx
                                                           +point[2][j][23]*dy
                                                           +point[2][j][24]*dz);
                      deltaVel=deltaU*deltaU+deltaV*deltaV+deltaW*deltaW;
                      //update arrays
                      counterArray[index]=counterArray[index]+1;
                      secondArray[index]=  (
                                              secondArray[index]*(counterArray[index]-1)
                                             +pow( dup*dup/KolConst,1.5 )/(dist)
                                            )/ counterArray[index];
                      thirdArray[index] =  (
                                              thirdArray[index]*(counterArray[index]-1)
                                             +(-dup*dup*dup*1.25)/dist
                                            )/ counterArray[index];
                      thirdArrayYeung[index] =  (
                                              thirdArrayYeung[index]*(counterArray[index]-1)
                                             +(-dup*dup*dup*2.5)/dist
                                            )/ counterArray[index];
                      dudaArray[index]  =  (
                                              dudaArray[index]*(counterArray[index]-1)
                                             -0.5*duda
                                            )/ counterArray[index];
                      upArray[index]=  (
                                         upArray[index]*(counterArray[index]-1)
                                        +du*du
                                       )/ counterArray[index];
                      if(point[2][j][13]<0.1 && point[2][k][13]<0.1){
                         belowCounterArray[index]=belowCounterArray[index]+1;
                         secondParalBelowArray[index]=  (
                                                           secondParalBelowArray[index]*(belowCounterArray[index]-1)
                                                          //+pow( dup*dup/KolConst,1.5 )/(dist)
                                                          +dup*dup
                                                        )/ belowCounterArray[index];
                         secondOrthoBelowArray[index]=  (
                                                           secondOrthoBelowArray[index]*(belowCounterArray[index]-1)
                                                          //+pow( 0.75*duo*duo/KolConst,1.5 )/(dist)
                                                          +duo*duo
                                                        )/ belowCounterArray[index];


                         dudaBelowArray[index]=(
                                                 dudaBelowArray[index]*(belowCounterArray[index]-1)

                                                 -0.5*duda
                                                )/ belowCounterArray[index];
                         deltaVelBelowArray[index]=(
                                                 deltaVelBelowArray[index]*(belowCounterArray[index]-1)
                                                 +deltaVel
                                                )/ belowCounterArray[index];


                      }
                      //if(1<2){//(point[2][j][13]>0.1 && point[2][k][13]>0.1){
                         aboveCounterArray[index]=aboveCounterArray[index]+1;
                         secondParalAboveArray[index]=  (
                                                           secondParalAboveArray[index]*(aboveCounterArray[index]-1)
                                                          //+pow( dup*dup/KolConst,1.5 )/(dist)
                                                          +dup*dup
                                                        )/ aboveCounterArray[index];
                         secondOrthoAboveArray[index]=  (
                                                           secondOrthoAboveArray[index]*(aboveCounterArray[index]-1)
                                                          //+pow( 0.75*duo*duo/KolConst,1.5 )/(dist)
                                                          +duo*duo
                                                        )/ aboveCounterArray[index];
                         dudaAboveArray[index]=(
                                                 dudaAboveArray[index]*(aboveCounterArray[index]-1)

                                                 -0.5*duda
                                                )/ aboveCounterArray[index];
                         deltaVelAboveArray[index]=(
                                                 deltaVelAboveArray[index]*(aboveCounterArray[index]-1)
                                                 +deltaVel
                                                )/ aboveCounterArray[index];
                      //}
                   }
                }
             }
             uaMean=(uaMean*(counter-1)-ua)/counter;
          }
        }
        //draw structure stuff
        structReprForm->Series1->Clear(); //secondParalBelow
        structReprForm->Series2->Clear(); //secondParalAbove
        structReprForm->Series3->Clear(); //secondOrthoBelowArray
        structReprForm->Series4->Clear(); //secondOrthoAbove
        structReprForm->Series5->Clear(); //uKinBelow
        structReprForm->Series6->Clear(); //uKinAbove
        structReprForm->Series7->Clear(); //aSqBelow
        structReprForm->Series8->Clear(); //aSqAbove
        structReprForm->Series9->Clear(); //dudaBelow
        structReprForm->Series10->Clear(); //dudaAbove
        structReprForm->Series11->Clear(); //up
        structReprForm->Series12->Clear(); //deltaVelBelow
        structReprForm->Series13->Clear(); //deltaVelAbove

        for(int j=0;j<200;j++){
           structReprForm->Series1->AddXY((double)j/10.,1e6*secondParalBelowArray[j],'.',clTeeColor);
           structReprForm->Series2->AddXY((double)j/10.,1e6*secondParalAboveArray[j],'.',clTeeColor);
           structReprForm->Series3->AddXY((double)j/10.,1e6*secondOrthoBelowArray[j],'.',clTeeColor);
           structReprForm->Series4->AddXY((double)j/10.,1e6*secondOrthoAboveArray[j],'.',clTeeColor);
           structReprForm->Series9->AddXY((double)j/10.,1e6*dudaBelowArray[j],'.',clTeeColor);
           structReprForm->Series10->AddXY((double)j/10.,1e6*dudaAboveArray[j],'.',clTeeColor);

           structReprForm->Series11->AddXY((double)j/10.,1e3*pow(upArray[j],0.5),'.',clTeeColor);
           structReprForm->Series12->AddXY((double)j/10.,1e3*pow(deltaVelBelowArray[j],0.5),'.',clTeeColor);
           structReprForm->Series13->AddXY((double)j/10.,1e3*pow(deltaVelAboveArray[j],0.5),'.',clTeeColor);
        }
        sumUkinBelow=1e-9;
        sumUkinAbove=1e-9;
        sumaSquBelow=1e-9;
        sumaSquAbove=1e-9;
        for(int j=0;j<pdfSize;j++){
           sumUkinBelow=sumUkinBelow+uKinBelowArray[j];
           sumUkinAbove=sumUkinAbove+uKinAboveArray[j];
           sumaSquBelow=sumaSquBelow+aSqBelowArray[j];
           sumaSquAbove=sumaSquAbove+aSqAboveArray[j];
        }
        for(int j=0;j<pdfSize;j++){
           uKinBelowArrayPlot[j]=uKinBelowArray[j]/(sumUkinBelow*(uKinMax/pdfSize));
           uKinAboveArrayPlot[j]=uKinAboveArray[j]/(sumUkinAbove*(uKinMax/pdfSize));;
           aSqBelowArrayPlot[j]=aSqBelowArray[j]/(sumaSquBelow*(aSquMax/pdfSize));;
           aSqAboveArrayPlot[j]=aSqAboveArray[j]/(sumaSquAbove*(aSquMax/pdfSize));;
           structReprForm->Series5->AddXY(1e6*(double)j*uKinMax/pdfSize,uKinBelowArrayPlot[j],'.',clTeeColor);
           structReprForm->Series6->AddXY(1e6*(double)j*uKinMax/pdfSize,uKinAboveArrayPlot[j],'.',clTeeColor);
           structReprForm->Series7->AddXY(1e6*(double)j*aSquMax/pdfSize,aSqBelowArrayPlot[j],'.',clTeeColor);
           structReprForm->Series8->AddXY(1e6*(double)j*aSquMax/pdfSize,aSqAboveArrayPlot[j],'.',clTeeColor);
        }
        structReprForm->Refresh();
     }

    FILE *fpp;
    AnsiString name;
    name="structures.res";
    const char *filename;
    filename=name.c_str();
    fpp = fopen(filename,"w");
    for(int j=0;j<201;j++){
        fprintf(fpp, "%lf\t",(double)j/10.);
        fprintf(fpp, "%lf\t",1e6*secondParalBelowArray[j]);
        fprintf(fpp, "%lf\t",1e6*secondParalAboveArray[j]);
        fprintf(fpp, "%lf\t",1e6*secondOrthoBelowArray[j]);
        fprintf(fpp, "%lf\t",1e6*secondOrthoAboveArray[j]);
        fprintf(fpp, "%lf\t",1e6*dudaBelowArray[j]);
        fprintf(fpp, "%lf\t",1e6*dudaAboveArray[j]);

        fprintf(fpp, "%lf\t",1e3*pow(upArray[j],0.5));
        fprintf(fpp, "%lf\t",1e3*pow(deltaVelBelowArray[j],0.5));
        fprintf(fpp, "%lf\n",1e3*pow(deltaVelAboveArray[j],0.5));
    }
    fclose (fpp);

    name="pdfs.res";
    filename=name.c_str();
    fpp = fopen(filename,"w");
    for(int j=0;j<pdfSize;j++){
        fprintf(fpp, "%lf\t",1e6*(double)j*uKinMax/pdfSize);
        fprintf(fpp, "%lf\t",uKinBelowArrayPlot[j]);
        fprintf(fpp, "%lf\t",uKinAboveArrayPlot[j]);
        fprintf(fpp, "%lf\t",1e6*(double)j*aSquMax/pdfSize);
        fprintf(fpp, "%lf\t",aSqBelowArrayPlot[j]);
        fprintf(fpp, "%lf\n",aSqAboveArrayPlot[j]);

    }
    fclose (fpp);
}
//---------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeCorFast()
{
     for(int i=0;i<30;i++){
        cov[i][0]=0.;
        cov[i][1]=0.;
        var[i][0]=0.;
        var[i][1]=0.;
        cor[i]=0.;
     }
     int end=lastFile;
     if(end>firstFile+995){
        end =firstFile+995;
     }
     for (int i=firstFile;i<end+1;i++){
        mainForm->fileNum2Edit->Text=IntToStr(i);
        mainForm->fileNum2Edit->Refresh();
        if(i>firstFile){
           readXUAPFileOld(i,true);
        }
        else{
           readXUAPFileOld(i,false);
        }
        getMean();
     }
     for(int i=0;i<30;i++){
        valid[i][0]=0;
        valid[i][1]=0;
     }
     krigingForm->Show();
     krigingForm->Refresh();
     for (int i=firstFile;i<end+1;i++){
        mainForm->fileNum2Edit->Text=IntToStr(i);
        mainForm->fileNum2Edit->Refresh();
        if(i>firstFile){
           readXUAPFileOld(i,true);
        }
        else{
           readXUAPFileOld(i,false);
        }
        autoCorrelationFast();
     }
     kriging=true;
}
//--------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeKrigingWeights()
{

     int stepSize=StrToInt(krigingForm->stepEdit->Text);
     int end=lastFile;

     for(int i=0;i<30;i++){
        valid[i][0]=0;
        valid[i][1]=0;
     }
     for (int i=firstFile;i<end+1;i=i+stepSize){
        mainForm->fileNum2Edit->Text=IntToStr(i);
        mainForm->fileNum2Edit->Refresh();
        if(i>firstFile){
           readXUAPFileOld(i,true);
        }
        else{
           readXUAPFileOld(i,false);
        }
        makeKrigingWeightsInnerLoop();
     }
}
//-----------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::readWriteRisoETH()
{
    const char *filename1;
    filename1=baseName1.c_str();
    const char *filename2;
    filename2=baseName2.c_str();
    AnsiString name;
    //AnsiString name_rt;
    AnsiString name_ptv;
    const char *filenamePTV;
    bool risoFileNotFinished=true;
    FILE *fPtsInput, *fLinkInput;
    int nopart,ok;
    short int left,right;
    float x,y,z;

    int frame=10000;
    double risoPoint[2000][5];
    //FILE *fpp1;
    FILE *fpp2;

    fPtsInput  = fopen(filename1,"rb");
    fLinkInput = fopen(filename2,"rb");
    while (risoFileNotFinished){
       ok=fread(&nopart,4,1,fPtsInput);
       risoPoint[0][0]=nopart;
       if(!(ok==1)){
       risoFileNotFinished=false;}
       if(risoFileNotFinished){
          for(int i=1;i<risoPoint[0][0]+1;i++){
             ok=fread(&x,sizeof(float),1,fPtsInput);
             ok=fread(&y,sizeof(float),1,fPtsInput);
             ok=fread(&z,sizeof(float),1,fPtsInput);
             //ok=fread(&link,2,1,fPtsInput);
             //ok=fread(&link,2,1,fPtsInput);
             //ok=fread(&link,2,1,fPtsInput);
             //ok=fread(&link,2,1,fPtsInput);
             risoPoint[i][2]=(double)x;
             risoPoint[i][3]=(double)y;
             risoPoint[i][4]=(double)z;
          }
          ok=fread(&nopart,sizeof(int),1,fLinkInput);
          for(int i=1;i<risoPoint[0][0]+1;i++){
             ok=fread(&left,sizeof(short int),1,fLinkInput);
             if((double)left>-1){
                risoPoint[i][0]=(double)left;
             }
             else{
                risoPoint[i][0]=-1;
             }

          }
          for(int i=1;i<risoPoint[0][0]+1;i++){
             ok=fread(&right,sizeof(short int),1,fLinkInput);
             if((double)right>-1){
                risoPoint[i][1]=(double)right;
             }
             else{
                risoPoint[i][1]=-1;
             }
          }
          mainForm->fileNumEdit->Text=IntToStr(frame-1);
          name_ptv = "ptv_is."+IntToStr(frame);
          filenamePTV=name_ptv.c_str();
          fpp2 = fopen(filenamePTV,"w");
          fprintf(fpp2, "%d\n", (int)risoPoint[0][0]);
          for(int i=1;i<risoPoint[0][0]+1;i++){
             fprintf(fpp2, "%d\t", (int)risoPoint[i][0]);
             fprintf(fpp2, "%d\t", (int)risoPoint[i][1]);
             fprintf(fpp2, "%lf\t", risoPoint[i][2]);
             fprintf(fpp2, "%lf\t", risoPoint[i][3]);
             fprintf(fpp2, "%lf\n", risoPoint[i][4]);
          }
          fclose (fpp2);
          frame++;
          mainForm->Refresh();
       }

       /*mainForm->fileNumEdit->Text=IntToStr(frame-1);
       //name_rt  = "rt_is."+IntToStr(frame);
       name_ptv = "ptv_is."+IntToStr(frame);
       const char *filenameRT;
       const char *filenamePTV;
       //filenameRT=name_rt.c_str();
       filenamePTV=name_ptv.c_str();
       //fpp1 = fopen(filenameRT,"w");
       fpp2 = fopen(filenamePTV,"w");
       //fprintf(fpp1, "%d\n", (int)risoPoint[0][0]);
       fprintf(fpp2, "%d\n", (int)risoPoint[0][0]);
       for(int i=1;i<risoPoint[0][0]+1;i++){
          //fprintf(fpp1, "%d\t", i);
          //fprintf(fpp1, "%lf\t", risoPoint[i][2]);
          //fprintf(fpp1, "%lf\t", risoPoint[i][3]);
          //fprintf(fpp1, "%lf\t", risoPoint[i][4]);
          //fprintf(fpp1, "%d\t", 500);
          //fprintf(fpp1, "%d\t", 500);
          //fprintf(fpp1, "%d\t", 500);
          //fprintf(fpp1, "%d\n", 500);

          fprintf(fpp2, "%d\t", (int)risoPoint[i][0]);
          fprintf(fpp2, "%d\t", (int)risoPoint[i][1]);
          fprintf(fpp2, "%lf\t", risoPoint[i][2]);
          fprintf(fpp2, "%lf\t", risoPoint[i][3]);
          fprintf(fpp2, "%lf\n", risoPoint[i][4]);
       }
       //fclose (fpp1);
       fclose (fpp2);

       frame++;
       mainForm->Refresh();*/
   }
   fclose(fPtsInput);
   fclose(fLinkInput);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::readWriteETHRiso()
{

    const char *filenamePTV;
    filenamePTV=baseNameTwo.c_str();

    FILE *fPtsOutput, *fLinkOutput;
    int nopart,left,right;

    double risoPoint[2000][5],dummy;
    float risoPointFloat[2000][5];
    //FILE *fpp1;
    FILE *fpp2;

    AnsiString name1;
    AnsiString name2;
    const char *filename1;
    const char *filename2;
    //AnsiString name_rt;
    AnsiString name_ptv;

    name1="rec//////.pt3";
    name2="rec/////.pln";

    filename1=name1.c_str();
    filename2=name2.c_str();

    fPtsOutput  = fopen(filename1,"wb");
    fLinkOutput = fopen(filename2,"wb");
    for(int frame=firstFile;frame<lastFile+1;frame++){

       mainForm->fileNumEdit->Text=IntToStr(frame);
       //name_rt  = baseName+IntToStr(frame);
       name_ptv = baseNameTwo+IntToStr(frame);
       //filenameRT=name_rt.c_str();
       filenamePTV=name_ptv.c_str();
       //fpp1 = fopen(filenameRT,"r");
       fpp2 = fopen(filenamePTV,"r");
       //fscanf(fpp1, "%d\n", &nopart);
       fscanf(fpp2, "%d\n", &nopart);
       for(int i=1;i<nopart+1;i++){
          /*fscanf(fpp1, "%d\t", &dummy);
          fscanf(fpp1, "%lf\t", &risoPoint[i][2]);
          fscanf(fpp1, "%lf\t", &risoPoint[i][3]);
          fscanf(fpp1, "%lf\t", &risoPoint[i][4]);
          fscanf(fpp1, "%d\t", &dummy);
          fscanf(fpp1, "%d\t", &dummy);
          fscanf(fpp1, "%d\t", &dummy);
          fscanf(fpp1, "%d\n", &dummy); */

          fscanf(fpp2, "%d\t", &left);
          fscanf(fpp2, "%d\t", &right);
          fscanf(fpp2, "%lf\t", &risoPoint[i][2]);
          fscanf(fpp2, "%lf\t", &risoPoint[i][3]);
          fscanf(fpp2, "%lf\n", &risoPoint[i][4]);
          fscanf(fpp2, "%lf\n", &dummy);

          risoPoint[i][0]=(double)left;
          risoPoint[i][1]=(double)right;
          risoPointFloat[i][2]=(float)risoPoint[i][2];
          risoPointFloat[i][3]=(float)risoPoint[i][3];
          risoPointFloat[i][4]=(float)risoPoint[i][4];
       }
       //fclose (fpp1);
       fclose (fpp2);


       fwrite(&nopart,sizeof(int),1,fPtsOutput);

          for(int i=1;i<nopart+1;i++){
             fwrite(&risoPointFloat[i][2],sizeof(float),1,fPtsOutput);
             fwrite(&risoPointFloat[i][3],sizeof(float),1,fPtsOutput);
             fwrite(&risoPointFloat[i][4],sizeof(float),1,fPtsOutput);
          }
          fwrite(&nopart,sizeof(int),1,fLinkOutput);
          for(int i=1;i<nopart+1;i++){
             left=(int)risoPoint[i][0];
             fwrite(&left,sizeof(short int),1,fLinkOutput);
          }
          for(int i=1;i<nopart+1;i++){
             right=(int)risoPoint[i][1];
             fwrite(&right,sizeof(short int),1,fLinkOutput);
          }

       mainForm->Refresh();
   }
   fclose(fPtsOutput);
   fclose(fLinkOutput);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::writeBinary()
{

    const char *filenameTrajAcc;
    filenameTrajAcc=baseName.c_str();
    FILE *fBinaryData;

    double x,y,z,u,v,w,ax,ay,az,ux,uy,uz,vx,vy,vz,wx,wy,wz,ut,vt,wt,dummy;
    double dix,diy,diz,refx,refy,refz,qual;
    FILE *fpp;
    int howMany=0;

    AnsiString name1;
    const char *filename1;
    AnsiString name_traj;

    name1="data.dat";
    filename1=name1.c_str();
    fBinaryData  = fopen(filename1,"wb");


    for(int frame=firstFile;frame<lastFile+1;frame++){

       mainForm->fileNumEdit->Text=IntToStr(frame);

       name_traj = baseName+IntToStr(frame);
       filenameTrajAcc=name_traj.c_str();
       fpp = fopen(filenameTrajAcc,"r");
       while(!feof(fpp)){
          fscanf(fpp, "%lf\0", &x);//1
          fscanf(fpp, "%lf\0", &y);//2
          fscanf(fpp, "%lf\0", &z);//3
          fscanf(fpp, "%lf\0", &u);//4
          fscanf(fpp, "%lf\0", &v);//5
          fscanf(fpp, "%lf\0", &w);//6
          fscanf(fpp, "%lf\0", &ax);//7
          fscanf(fpp, "%lf\0", &ay);//8
          fscanf(fpp, "%lf\0", &az);//9
                     
          fscanf(fpp, "%lf\0", &ux);//10
          fscanf(fpp, "%lf\0", &uy);//11
          fscanf(fpp, "%lf\0", &uz);//12
          fscanf(fpp, "%lf\0", &vx);//13
          fscanf(fpp, "%lf\0", &vy);//14
          fscanf(fpp, "%lf\0", &vz);//15
          fscanf(fpp, "%lf\0", &wx);//16
          fscanf(fpp, "%lf\0", &wy);//17
          fscanf(fpp, "%lf\0", &wz);//18
          fscanf(fpp, "%lf\0", &ut);//19
          fscanf(fpp, "%lf\0", &vt);//20
          fscanf(fpp, "%lf\0", &wt);//21

          fscanf(fpp, "%lf\0", &dummy);//22
          fscanf(fpp, "%lf\0", &dummy);//23
          fscanf(fpp, "%lf\0", &dummy);//24
          fscanf(fpp, "%lf\0", &dummy);//25
          fscanf(fpp, "%lf\0", &dummy);//26
          fscanf(fpp, "%lf\0", &dummy);//27
          fscanf(fpp, "%lf\0", &dummy);//28
          fscanf(fpp, "%lf\0", &dummy);//29
          fscanf(fpp, "%lf\0", &dummy);//30

          fscanf(fpp, "%lf\0", &dummy);//31
          fscanf(fpp, "%lf\0", &dummy);//32
          fscanf(fpp, "%lf\0", &dummy);//33

          fscanf(fpp, "%lf\n", &dummy);//34

          //check for quality
          dix=ax-ut-u*ux-v*uy-w*uz;
          diy=ay-vt-u*vx-v*vy-w*vz;
          diz=az-wt-u*wx-v*wy-w*wz;
          refx= fabs(ax)+fabs(ut)+fabs(u*ux+v*uy+w*uz);
          refy= fabs(ay)+fabs(vt)+fabs(u*vx+v*vy+w*vz);
          refz= fabs(az)+fabs(wt)+fabs(u*wx+v*wy+w*wz);
          if(refx>0 && refy>0 && refz>0){
                     qual=(1./3.)*(fabs(dix)/refx+fabs(diy)/refy+fabs(diz)/refz);
          }
          else{
                     qual=0.95;
          }
          //end check for quality
          //if(qual<0.1 && fabs(u)>0 && fabs(v)>0 && fabs(w)>0){
          if(fabs(u)>0 && fabs(v)>0 && fabs(w)>0){
               howMany++;
               fwrite(&u,sizeof(double),1,fBinaryData);
               fwrite(&v,sizeof(double),1,fBinaryData);
               fwrite(&w,sizeof(double),1,fBinaryData);
               fwrite(&ax,sizeof(double),1,fBinaryData);
               fwrite(&ay,sizeof(double),1,fBinaryData);
               fwrite(&az,sizeof(double),1,fBinaryData);

               fwrite(&ux,sizeof(double),1,fBinaryData);
               fwrite(&uy,sizeof(double),1,fBinaryData);
               fwrite(&uz,sizeof(double),1,fBinaryData);
               fwrite(&vx,sizeof(double),1,fBinaryData);
               fwrite(&vy,sizeof(double),1,fBinaryData);
               fwrite(&vz,sizeof(double),1,fBinaryData);
               fwrite(&wx,sizeof(double),1,fBinaryData);
               fwrite(&wy,sizeof(double),1,fBinaryData);
               fwrite(&wz,sizeof(double),1,fBinaryData);

               fwrite(&ut,sizeof(double),1,fBinaryData);
               fwrite(&vt,sizeof(double),1,fBinaryData);
               fwrite(&wt,sizeof(double),1,fBinaryData);

          }
       }
       fclose (fpp);
       mainForm->numEdit->Text=IntToStr(howMany);
       mainForm->Refresh();
   }
   fclose(fBinaryData);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::writePTVtoFile(int t)
{

    FILE *fpp;
    AnsiString name;
    name="xuap."+IntToStr(t);
    const char *filename;
    filename=name.c_str();
    fpp = fopen(filename,"w");
    
    for(int i=1;i<point[10][0][0];i++){
       if(point[10][i][14]>0){
           count++;
           double vel=pow( pow(point[10][i][8],2.)
                          +pow(point[10][i][9],2.)
                          +pow(point[10][i][10],2.),0.5);
           double acc=pow( pow(point[10][i][11],2.)
                          +pow(point[10][i][12],2.)
                          +pow(point[10][i][13],2.),0.5);
           meanVel=(meanVel*(double)(count-1)+vel)/(double)count;
           meanAcc=(meanAcc*(double)(count-1)+acc)/(double)count;
           if(vel>maxVel){
              maxVel=vel;
           }
        }
        for(int j=0;j<14;j++){
        //for(int j=0;j<15;j++){
           fprintf(fpp, "%lf\t", point[10][i][j]);
        }
        fprintf(fpp, "%lf\n", point[10][i][14]);
    }
    fclose (fpp);
}
//---------------------------------------------------------------------------
double __fastcall TpointList::Sqrt(double x)
{
	return sqrt(x);
}
 

double __fastcall TpointList::Power(double x,int n)
{
	return pow(x,n);
/*	switch (n) {
	case 2:
		return x*x;
		break;
	case 3:
		return x*x*x;
		break;
	default:
		nrerror("???? Error in Power");
		break;
	}*/
}

double __fastcall TpointList::Rep(int j, int m, int n, int i, double x, double y, double z)
{
	double ex;
	int code;
        double Pi=3.141592653589793;

	code = i*1000+j*100+m*10+n;

/* Re p */
switch (code) {
case 1100 :
ex = 0;
break;
case 2100 :
ex = 0;
break;
case 3100 :
ex = Sqrt(3/Pi)/2.;
break;
case 1101 :
ex = (Sqrt(21/(2.*Pi))*x*z)/2.;
break;
case 2101 :
ex = (Sqrt(21/(2.*Pi))*y*z)/2.;
break;
case 3101 :
ex = -(Sqrt(21/(2.*Pi))*(-1 + 2*Power(x,2) + 2*Power(y,2) + Power(z,2)))/2.;
break;
case 1110 :
ex = -Sqrt(3/(2.*Pi))/2.;
break;
case 2110 :
ex = 0;
break;
case 3110 :
ex = 0;
break;
case 1111 :
ex = (Sqrt(21/Pi)*(-1 + Power(x,2) + 2*Power(y,2) + 2*Power(z,2)))/4.;
break;
case 2111 :
ex = -(Sqrt(21/Pi)*x*y)/4.;
break;
case 3111 :
ex = -(Sqrt(21/Pi)*x*z)/4.;
break;
case 1200 :
ex = -(Sqrt(5/(2.*Pi))*x)/2.;
break;
case 2200 :
ex = -(Sqrt(5/(2.*Pi))*y)/2.;
break;
case 3200 :
ex = Sqrt(5/(2.*Pi))*z;
break;
case 1201 :
ex = (3*Sqrt(15/(2.*Pi))*x*(-1 + Power(x,2) + Power(y,2) + \
3*Power(z,2)))/4.;
break;
case 2201 :
ex = (3*Sqrt(15/(2.*Pi))*y*(-1 + Power(x,2) + Power(y,2) + \
3*Power(z,2)))/4.;
break;
case 3201 :
ex = (-3*Sqrt(15/(2.*Pi))*z*(-1 + 2*Power(x,2) + 2*Power(y,2) + \
Power(z,2)))/2.;
break;
case 1210 :
ex = -(Sqrt(15/Pi)*z)/4.;
break;
case 2210 :
ex = 0;
break;
case 3210 :
ex = -(Sqrt(15/Pi)*x)/4.;
break;
case 1211 :
ex = (3*Sqrt(5/Pi)*z*(-3 + Power(x,2) + 5*Power(y,2) + 5*Power(z,2)))/8.;
break;
case 2211 :
ex = (-3*Sqrt(5/Pi)*x*y*z)/2.;
break;
case 3211 :
ex = (3*Sqrt(5/Pi)*x*(-3 + 5*Power(x,2) + 5*Power(y,2) + Power(z,2)))/8.;
break;
case 1220 :
ex = (Sqrt(15/Pi)*x)/4.;
break;
case 2220 :
ex = -(Sqrt(15/Pi)*y)/4.;
break;
case 3220 :
ex = 0;
break;
case 1221 :
ex = (-3*Sqrt(5/Pi)*x*(-3 + 3*Power(x,2) + 7*Power(y,2) + 5*Power(z,2)))/8.;
break;
case 2221 :
ex = (3*Sqrt(5/Pi)*y*(-3 + 7*Power(x,2) + 3*Power(y,2) + 5*Power(z,2)))/8.;
break;
case 3221 :
ex = (3*Sqrt(5/Pi)*(Power(x,2) - Power(y,2))*z)/4.;
break;
case 1300 :
ex = -(Sqrt(21/Pi)*x*z)/2.;
break;
case 2300 :
ex = -(Sqrt(21/Pi)*y*z)/2.;
break;
case 3300 :
ex = -(Sqrt(21/Pi)*(Power(x,2) + Power(y,2) - 2*Power(z,2)))/4.;
break;
case 1301 :
ex = (Sqrt(231/Pi)*x*z*(-4 + 3*Power(x,2) + 3*Power(y,2) + \
8*Power(z,2)))/8.;
break;
case 2301 :
ex = (Sqrt(231/Pi)*y*z*(-4 + 3*Power(x,2) + 3*Power(y,2) + \
8*Power(z,2)))/8.;
break;
case 3301 :
ex = (Sqrt(231/Pi)*(3*Power(x,4) + 3*Power(y,4) + Power(x,2)*(-2 + \
6*Power(y,2) - 6*Power(z,2)) - 4*Power(z,2)*(-1 + Power(z,2)) - \
2*Power(y,2)*(1 + 3*Power(z,2))))/8.;
break;
case 1310 :
ex = (Sqrt(7/Pi)*(3*Power(x,2) + Power(y,2) - 4*Power(z,2)))/8.;
break;
case 2310 :
ex = (Sqrt(7/Pi)*x*y)/4.;
break;
case 3310 :
ex = -(Sqrt(7/Pi)*x*z);
break;
case 1311 :
ex = -(Sqrt(77/Pi)*(6*Power(x,4) + 3*Power(y,4) + 8*Power(z,2) - \
12*Power(z,4) - Power(y,2)*(2 + 9*Power(z,2)) + Power(x,2)*(-6 + \
9*Power(y,2) + 9*Power(z,2))))/16.;
break;
case 2311 :
ex = -(Sqrt(77/Pi)*x*y*(-4 + 3*Power(x,2) + 3*Power(y,2) + \
18*Power(z,2)))/16.;
break;
case 3311 :
ex = (Sqrt(77/Pi)*x*z*(-16 + 27*Power(x,2) + 27*Power(y,2) + \
12*Power(z,2)))/16.;
break;
case 1320 :
ex = (Sqrt(35/(2.*Pi))*x*z)/2.;
break;
case 2320 :
ex = -(Sqrt(35/(2.*Pi))*y*z)/2.;
break;
case 3320 :
ex = (Sqrt(35/(2.*Pi))*(Power(x,2) - Power(y,2)))/4.;
break;
case 1321 :
ex = -(Sqrt(385/(2.*Pi))*x*z*(-4 + 3*Power(x,2) + 9*Power(y,2) + \
6*Power(z,2)))/8.;
break;
case 2321 :
ex = (Sqrt(385/(2.*Pi))*y*z*(-4 + 9*Power(x,2) + 3*Power(y,2) + \
6*Power(z,2)))/8.;
break;
case 3321 :
ex = (Sqrt(385/(2.*Pi))*(2*Power(x,2) - 3*Power(x,4) - 2*Power(y,2) + \
3*Power(y,4)))/8.;
break;
case 1330 :
ex = (Sqrt(105/Pi)*(-Power(x,2) + Power(y,2)))/8.;
break;
case 2330 :
ex = (Sqrt(105/Pi)*x*y)/4.;
break;
case 3330 :
ex = 0;
break;
case 1331 :
ex = (Sqrt(1155/Pi)*(2*Power(x,4) + Power(y,2)*(2 - 3*Power(y,2) - \
3*Power(z,2)) + Power(x,2)*(-2 + 3*Power(y,2) + 3*Power(z,2))))/16.;
break;
case 2331 :
ex = -(Sqrt(1155/Pi)*x*y*(-4 + 7*Power(x,2) + 3*Power(y,2) + \
6*Power(z,2)))/16.;
break;
case 3331 :
ex = -(Sqrt(1155/Pi)*x*(Power(x,2) - 3*Power(y,2))*z)/16.;
break;
case 1400 :
ex = (9*x*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/(8.*Sqrt(Pi));
break;
case 2400 :
ex = (9*y*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/(8.*Sqrt(Pi));
break;
case 3400 :
ex = (3*z*(-3*Power(x,2) - 3*Power(y,2) + 2*Power(z,2)))/(2.*Sqrt(Pi));
break;
case 1401 :
ex = (-3*Sqrt(65/Pi)*x*(3*Power(x,4) + 3*Power(y,4) + 4*Power(z,2)*(3 - \
5*Power(z,2)) - 3*Power(y,2)*(1 + Power(z,2)) + Power(x,2)*(6*Power(y,2) - \
3*(1 + Power(z,2)))))/16.;
break;
case 2401 :
ex = (-3*Sqrt(65/Pi)*y*(3*Power(x,4) + 3*Power(y,4) + 4*Power(z,2)*(3 - \
5*Power(z,2)) - 3*Power(y,2)*(1 + Power(z,2)) + Power(x,2)*(6*Power(y,2) - \
3*(1 + Power(z,2)))))/16.;
break;
case 3401 :
ex = (-3*Sqrt(65/Pi)*z*(-9*Power(x,4) - 9*Power(y,4) + 4*Power(z,2)*(-1 + \
Power(z,2)) + 2*Power(y,2)*(3 + Power(z,2)) + 2*Power(x,2)*(3 - 9*Power(y,2) \
+ Power(z,2))))/8.;
break;
case 1410 :
ex = (3*Sqrt(5/Pi)*z*(9*Power(x,2) + 3*Power(y,2) - 4*Power(z,2)))/16.;
break;
case 2410 :
ex = (9*Sqrt(5/Pi)*x*y*z)/8.;
break;
case 3410 :
ex = (9*Sqrt(5/Pi)*x*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/16.;
break;
case 1411 :
ex = (-3*Sqrt(13/Pi)*z*(39*Power(x,4) + 21*Power(y,4) + 4*Power(z,2)*(5 - \
7*Power(z,2)) - Power(y,2)*(15 + 7*Power(z,2)) + Power(x,2)*(-45 + \
60*Power(y,2) + 67*Power(z,2))))/32.;
break;
case 2411 :
ex = (-3*Sqrt(13/Pi)*x*y*z*(-15 + 9*Power(x,2) + 9*Power(y,2) + \
37*Power(z,2)))/16.;
break;
case 3411 :
ex = (-3*Sqrt(13/Pi)*x*(21*Power(x,4) + 21*Power(y,4) + 60*Power(z,2) - \
52*Power(z,4) + 3*Power(x,2)*(-5 + 14*Power(y,2) - 29*Power(z,2)) - \
3*Power(y,2)*(5 + 29*Power(z,2))))/32.;
break;
case 1420 :
ex = (-3*Sqrt(5/(2.*Pi))*x*(Power(x,2) - 3*Power(z,2)))/4.;
break;
case 2420 :
ex = (3*Sqrt(5/(2.*Pi))*y*(Power(y,2) - 3*Power(z,2)))/4.;
break;
case 3420 :
ex = (9*Sqrt(5/(2.*Pi))*(Power(x,2) - Power(y,2))*z)/4.;
break;
case 1421 :
ex = (3*Sqrt(13/(2.*Pi))*x*(5*Power(x,4) + 2*Power(y,4) - \
33*Power(y,2)*Power(z,2) + 3*Power(z,2)*(5 - 7*Power(z,2)) + Power(x,2)*(-5 \
+ 7*Power(y,2) - 2*Power(z,2))))/8.;
break;
case 2421 :
ex = (-3*Sqrt(13/(2.*Pi))*y*(2*Power(x,4) + 5*Power(y,4) + \
Power(x,2)*(7*Power(y,2) - 33*Power(z,2)) + 3*Power(z,2)*(5 - 7*Power(z,2)) \
- Power(y,2)*(5 + 2*Power(z,2))))/8.;
break;
case 3421 :
ex = (-3*Sqrt(13/(2.*Pi))*(Power(x,2) - Power(y,2))*z*(-15 + 23*Power(x,2) + \
23*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 1430 :
ex = (-9*Sqrt(35/Pi)*(Power(x,2) - Power(y,2))*z)/16.;
break;
case 2430 :
ex = (9*Sqrt(35/Pi)*x*y*z)/8.;
break;
case 3430 :
ex = (-3*Sqrt(35/Pi)*x*(Power(x,2) - 3*Power(y,2)))/16.;
break;
case 1431 :
ex = (3*Sqrt(91/Pi)*z*(13*Power(x,4) - 3*Power(y,2)*(-5 + 7*Power(y,2) + \
7*Power(z,2)) + 3*Power(x,2)*(-5 + 8*Power(y,2) + 7*Power(z,2))))/32.;
break;
case 2431 :
ex = (-3*Sqrt(91/Pi)*x*y*z*(-15 + 25*Power(x,2) + 9*Power(y,2) + \
21*Power(z,2)))/16.;
break;
case 3431 :
ex = (3*Sqrt(91/Pi)*x*(Power(x,2) - 3*Power(y,2))*(-5 + 7*Power(x,2) + \
7*Power(y,2) - Power(z,2)))/32.;
break;
case 1440 :
ex = (3*Sqrt(35/(2.*Pi))*x*(Power(x,2) - 3*Power(y,2)))/8.;
break;
case 2440 :
ex = (3*Sqrt(35/(2.*Pi))*y*(-3*Power(x,2) + Power(y,2)))/8.;
break;
case 3440 :
ex = 0;
break;
case 1441 :
ex = (-3*Sqrt(91/(2.*Pi))*x*(5*Power(x,4) + Power(y,2)*(15 - 23*Power(y,2) - \
21*Power(z,2)) + Power(x,2)*(-5 - 2*Power(y,2) + 7*Power(z,2))))/16.;
break;
case 2441 :
ex = (3*Sqrt(91/(2.*Pi))*y*(23*Power(x,4) + Power(y,2)*(5 - 5*Power(y,2) - \
7*Power(z,2)) + Power(x,2)*(-15 + 2*Power(y,2) + 21*Power(z,2))))/16.;
break;
case 3441 :
ex = (3*Sqrt(91/(2.*Pi))*(Power(x,4) - 6*Power(x,2)*Power(y,2) + \
Power(y,4))*z)/8.;
break;
default:
fprintf(stderr,"not implemented");
break;
}
return ex;

}

double __fastcall TpointList::Imp(int j, int m, int n, int i, double x, double y, double z)
{
	double ex;
	int code;
        double Pi=3.141592653589793;

	code = i*1000+j*100+m*10+n;

/* Im p */
switch (code) {
case 1110 :
ex = 0;
break;
case 2110 :
ex = -Sqrt(3/(2.*Pi))/2.;
break;
case 3110 :
ex = 0;
break;
case 1111 :
ex = -(Sqrt(21/Pi)*x*y)/4.;
break;
case 2111 :
ex = (Sqrt(21/Pi)*(-1 + 2*Power(x,2) + Power(y,2) + 2*Power(z,2)))/4.;
break;
case 3111 :
ex = -(Sqrt(21/Pi)*y*z)/4.;
break;
case 1210 :
ex = 0;
break;
case 2210 :
ex = -(Sqrt(15/Pi)*z)/4.;
break;
case 3210 :
ex = -(Sqrt(15/Pi)*y)/4.;
break;
case 1211 :
ex = (-3*Sqrt(5/Pi)*x*y*z)/2.;
break;
case 2211 :
ex = (3*Sqrt(5/Pi)*z*(-3 + 5*Power(x,2) + Power(y,2) + 5*Power(z,2)))/8.;
break;
case 3211 :
ex = (3*Sqrt(5/Pi)*y*(-3 + 5*Power(x,2) + 5*Power(y,2) + Power(z,2)))/8.;
break;
case 1220 :
ex = (Sqrt(15/Pi)*y)/4.;
break;
case 2220 :
ex = (Sqrt(15/Pi)*x)/4.;
break;
case 3220 :
ex = 0;
break;
case 1221 :
ex = (-3*Sqrt(5/Pi)*y*(-3 + Power(x,2) + 5*Power(y,2) + 5*Power(z,2)))/8.;
break;
case 2221 :
ex = (-3*Sqrt(5/Pi)*x*(-3 + 5*Power(x,2) + Power(y,2) + 5*Power(z,2)))/8.;
break;
case 3221 :
ex = (3*Sqrt(5/Pi)*x*y*z)/2.;
break;
case 1310 :
ex = (Sqrt(7/Pi)*x*y)/4.;
break;
case 2310 :
ex = (Sqrt(7/Pi)*(Power(x,2) + 3*Power(y,2) - 4*Power(z,2)))/8.;
break;
case 3310 :
ex = -(Sqrt(7/Pi)*y*z);
break;
case 1311 :
ex = -(Sqrt(77/Pi)*x*y*(-4 + 3*Power(x,2) + 3*Power(y,2) + \
18*Power(z,2)))/16.;
break;
case 2311 :
ex = -(Sqrt(77/Pi)*(3*Power(x,4) + 6*Power(y,4) + 8*Power(z,2) - \
12*Power(z,4) + Power(x,2)*(-2 + 9*Power(y,2) - 9*Power(z,2)) + \
Power(y,2)*(-6 + 9*Power(z,2))))/16.;
break;
case 3311 :
ex = (Sqrt(77/Pi)*y*z*(-16 + 27*Power(x,2) + 27*Power(y,2) + \
12*Power(z,2)))/16.;
break;
case 1320 :
ex = (Sqrt(35/(2.*Pi))*y*z)/2.;
break;
case 2320 :
ex = (Sqrt(35/(2.*Pi))*x*z)/2.;
break;
case 3320 :
ex = (Sqrt(35/(2.*Pi))*x*y)/2.;
break;
case 1321 :
ex = -(Sqrt(385/(2.*Pi))*y*z*(-2 + 3*Power(y,2) + 3*Power(z,2)))/4.;
break;
case 2321 :
ex = -(Sqrt(385/(2.*Pi))*x*z*(-2 + 3*Power(x,2) + 3*Power(z,2)))/4.;
break;
case 3321 :
ex = -(Sqrt(385/(2.*Pi))*x*y*(-2 + 3*Power(x,2) + 3*Power(y,2)))/4.;
break;
case 1330 :
ex = -(Sqrt(105/Pi)*x*y)/4.;
break;
case 2330 :
ex = (Sqrt(105/Pi)*(-Power(x,2) + Power(y,2)))/8.;
break;
case 3330 :
ex = 0;
break;
case 1331 :
ex = (Sqrt(1155/Pi)*x*y*(-4 + 3*Power(x,2) + 7*Power(y,2) + \
6*Power(z,2)))/16.;
break;
case 2331 :
ex = (Sqrt(1155/Pi)*(3*Power(x,4) + Power(y,2)*(2 - 2*Power(y,2) - \
3*Power(z,2)) + Power(x,2)*(-2 - 3*Power(y,2) + 3*Power(z,2))))/16.;
break;
case 3331 :
ex = (Sqrt(1155/Pi)*y*(-3*Power(x,2) + Power(y,2))*z)/16.;
break;
case 1410 :
ex = (9*Sqrt(5/Pi)*x*y*z)/8.;
break;
case 2410 :
ex = (3*Sqrt(5/Pi)*z*(3*Power(x,2) + 9*Power(y,2) - 4*Power(z,2)))/16.;
break;
case 3410 :
ex = (9*Sqrt(5/Pi)*y*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/16.;
break;
case 1411 :
ex = (-3*Sqrt(13/Pi)*x*y*z*(-15 + 9*Power(x,2) + 9*Power(y,2) + \
37*Power(z,2)))/16.;
break;
case 2411 :
ex = (-3*Sqrt(13/Pi)*z*(21*Power(x,4) + 39*Power(y,4) + 4*Power(z,2)*(5 - \
7*Power(z,2)) + Power(x,2)*(-15 + 60*Power(y,2) - 7*Power(z,2)) + \
Power(y,2)*(-45 + 67*Power(z,2))))/32.;
break;
case 3411 :
ex = (-3*Sqrt(13/Pi)*y*(21*Power(x,4) + 21*Power(y,4) + 60*Power(z,2) - \
52*Power(z,4) + 3*Power(x,2)*(-5 + 14*Power(y,2) - 29*Power(z,2)) - \
3*Power(y,2)*(5 + 29*Power(z,2))))/32.;
break;
case 1420 :
ex = (-3*Sqrt(5/(2.*Pi))*y*(3*Power(x,2) + Power(y,2) - 6*Power(z,2)))/8.;
break;
case 2420 :
ex = (-3*Sqrt(5/(2.*Pi))*x*(Power(x,2) + 3*Power(y,2) - 6*Power(z,2)))/8.;
break;
case 3420 :
ex = (9*Sqrt(5/(2.*Pi))*x*y*z)/2.;
break;
case 1421 :
ex = (3*Sqrt(13/(2.*Pi))*y*(13*Power(x,4) + 7*Power(y,4) + 6*Power(z,2)*(5 - \
7*Power(z,2)) - 5*Power(y,2)*(1 + 7*Power(z,2)) + Power(x,2)*(-15 + \
20*Power(y,2) + 27*Power(z,2))))/16.;
break;
case 2421 :
ex = (3*Sqrt(13/(2.*Pi))*x*(7*Power(x,4) + 13*Power(y,4) + 6*Power(z,2)*(5 - \
7*Power(z,2)) + 5*Power(x,2)*(-1 + 4*Power(y,2) - 7*Power(z,2)) + \
3*Power(y,2)*(-5 + 9*Power(z,2))))/16.;
break;
case 3421 :
ex = (-3*Sqrt(13/(2.*Pi))*x*y*z*(-15 + 23*Power(x,2) + 23*Power(y,2) + \
9*Power(z,2)))/4.;
break;
case 1430 :
ex = (-9*Sqrt(35/Pi)*x*y*z)/8.;
break;
case 2430 :
ex = (-9*Sqrt(35/Pi)*(Power(x,2) - Power(y,2))*z)/16.;
break;
case 3430 :
ex = (3*Sqrt(35/Pi)*y*(-3*Power(x,2) + Power(y,2)))/16.;
break;
case 1431 :
ex = (3*Sqrt(91/Pi)*x*y*z*(-15 + 9*Power(x,2) + 25*Power(y,2) + \
21*Power(z,2)))/16.;
break;
case 2431 :
ex = (3*Sqrt(91/Pi)*z*(21*Power(x,4) + Power(y,2)*(15 - 13*Power(y,2) - \
21*Power(z,2)) - 3*Power(x,2)*(5 + 8*Power(y,2) - 7*Power(z,2))))/32.;
break;
case 3431 :
ex = (-3*Sqrt(91/Pi)*y*(-3*Power(x,2) + Power(y,2))*(-5 + 7*Power(x,2) + \
7*Power(y,2) - Power(z,2)))/32.;
break;
case 1440 :
ex = (-3*Sqrt(35/(2.*Pi))*y*(-3*Power(x,2) + Power(y,2)))/8.;
break;
case 2440 :
ex = (3*Sqrt(35/(2.*Pi))*x*(Power(x,2) - 3*Power(y,2)))/8.;
break;
case 3440 :
ex = 0;
break;
case 1441 :
ex = (-3*Sqrt(91/(2.*Pi))*y*(13*Power(x,4) + Power(y,2)*(5 - 7*Power(y,2) - \
7*Power(z,2)) + Power(x,2)*(-15 + 22*Power(y,2) + 21*Power(z,2))))/16.;
break;
case 2441 :
ex = (-3*Sqrt(91/(2.*Pi))*x*(7*Power(x,4) + Power(y,2)*(15 - 13*Power(y,2) - \
21*Power(z,2)) + Power(x,2)*(-5 - 22*Power(y,2) + 7*Power(z,2))))/16.;
break;
case 3441 :
ex = (3*Sqrt(91/(2.*Pi))*x*y*(Power(x,2) - Power(y,2))*z)/2.;
break;
default:
fprintf(stderr,"not implemented");
break;
}
return ex;

}

double __fastcall TpointList::Req(int j, int m, int n, int i, double x, double y, double z)
{
	double ex;
	int code;
        double Pi=3.141592653589793;

	code = i*1000+j*100+m*10+n;

/* Re q */
switch (code) {
case 1100 :
ex = (Sqrt(15/(2.*Pi))*y)/2.;
break;
case 2100 :
ex = -(Sqrt(15/(2.*Pi))*x)/2.;
break;
case 3100 :
ex = 0;
break;
case 1101 :
ex = (3*Sqrt(3/(2.*Pi))*y*(-5 + 7*Power(x,2) + 7*Power(y,2) + \
7*Power(z,2)))/4.;
break;
case 2101 :
ex = (-3*Sqrt(3/(2.*Pi))*x*(-5 + 7*Power(x,2) + 7*Power(y,2) + \
7*Power(z,2)))/4.;
break;
case 3101 :
ex = 0;
break;
case 1110 :
ex = 0;
break;
case 2110 :
ex = -(Sqrt(15/Pi)*z)/4.;
break;
case 3110 :
ex = (Sqrt(15/Pi)*y)/4.;
break;
case 1111 :
ex = 0;
break;
case 2111 :
ex = (-3*Sqrt(3/Pi)*z*(-5 + 7*Power(x,2) + 7*Power(y,2) + 7*Power(z,2)))/8.;
break;
case 3111 :
ex = (3*Sqrt(3/Pi)*y*(-5 + 7*Power(x,2) + 7*Power(y,2) + 7*Power(z,2)))/8.;
break;
case 1200 :
ex = (Sqrt(105/(2.*Pi))*y*z)/2.;
break;
case 2200 :
ex = -(Sqrt(105/(2.*Pi))*x*z)/2.;
break;
case 3200 :
ex = 0;
break;
case 1201 :
ex = (Sqrt(165/(2.*Pi))*y*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + \
9*Power(z,2)))/4.;
break;
case 2201 :
ex = -(Sqrt(165/(2.*Pi))*x*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + \
9*Power(z,2)))/4.;
break;
case 3201 :
ex = 0;
break;
case 1210 :
ex = -(Sqrt(35/Pi)*x*y)/4.;
break;
case 2210 :
ex = (Sqrt(35/Pi)*(Power(x,2) - Power(z,2)))/4.;
break;
case 3210 :
ex = (Sqrt(35/Pi)*y*z)/4.;
break;
case 1211 :
ex = -(Sqrt(55/Pi)*x*y*(-7 + 9*Power(x,2) + 9*Power(y,2) + \
9*Power(z,2)))/8.;
break;
case 2211 :
ex = (Sqrt(55/Pi)*(Power(x,2) - Power(z,2))*(-7 + 9*Power(x,2) + \
9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 3211 :
ex = (Sqrt(55/Pi)*y*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 1220 :
ex = (Sqrt(35/Pi)*y*z)/4.;
break;
case 2220 :
ex = (Sqrt(35/Pi)*x*z)/4.;
break;
case 3220 :
ex = -(Sqrt(35/Pi)*x*y)/2.;
break;
case 1221 :
ex = (Sqrt(55/Pi)*y*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 2221 :
ex = (Sqrt(55/Pi)*x*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 3221 :
ex = -(Sqrt(55/Pi)*x*y*(-7 + 9*Power(x,2) + 9*Power(y,2) + \
9*Power(z,2)))/4.;
break;
case 1300 :
ex = (-3*Sqrt(21/Pi)*y*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/8.;
break;
case 2300 :
ex = (3*Sqrt(21/Pi)*x*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/8.;
break;
case 3300 :
ex = 0;
break;
case 1301 :
ex = (Sqrt(273/Pi)*y*(-11*Power(x,4) - 11*Power(y,4) + 4*Power(z,2)*(-9 + \
11*Power(z,2)) + Power(y,2)*(9 + 33*Power(z,2)) + Power(x,2)*(9 - \
22*Power(y,2) + 33*Power(z,2))))/16.;
break;
case 2301 :
ex = (Sqrt(273/Pi)*x*(11*Power(x,4) + 11*Power(y,4) + 36*Power(z,2) - \
44*Power(z,4) + Power(x,2)*(-9 + 22*Power(y,2) - 33*Power(z,2)) - \
3*Power(y,2)*(3 + 11*Power(z,2))))/16.;
break;
case 3301 :
ex = 0;
break;
case 1310 :
ex = (-15*Sqrt(7/Pi)*x*y*z)/8.;
break;
case 2310 :
ex = (3*Sqrt(7/Pi)*z*(11*Power(x,2) + Power(y,2) - 4*Power(z,2)))/16.;
break;
case 3310 :
ex = (-3*Sqrt(7/Pi)*y*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/16.;
break;
case 1311 :
ex = (-5*Sqrt(91/Pi)*x*y*z*(-9 + 11*Power(x,2) + 11*Power(y,2) + \
11*Power(z,2)))/16.;
break;
case 2311 :
ex = (Sqrt(91/Pi)*z*(121*Power(x,4) + 11*Power(y,4) + 36*Power(z,2) - \
44*Power(z,4) + 11*Power(x,2)*(-9 + 12*Power(y,2) + 7*Power(z,2)) - \
3*Power(y,2)*(3 + 11*Power(z,2))))/32.;
break;
case 3311 :
ex = (Sqrt(91/Pi)*y*(-11*Power(x,4) - 11*Power(y,4) + 4*Power(z,2)*(-9 + \
11*Power(z,2)) + Power(y,2)*(9 + 33*Power(z,2)) + Power(x,2)*(9 - \
22*Power(y,2) + 33*Power(z,2))))/32.;
break;
case 1320 :
ex = (3*Sqrt(35/(2.*Pi))*y*(Power(x,2) - Power(y,2) + 2*Power(z,2)))/8.;
break;
case 2320 :
ex = (-3*Sqrt(35/(2.*Pi))*x*(Power(x,2) - Power(y,2) - 2*Power(z,2)))/8.;
break;
case 3320 :
ex = (-3*Sqrt(35/(2.*Pi))*x*y*z)/2.;
break;
case 1321 :
ex = (Sqrt(455/(2.*Pi))*y*(11*Power(x,4) - 11*Power(y,4) + 2*Power(z,2)*(-9 \
+ 11*Power(z,2)) + Power(y,2)*(9 + 11*Power(z,2)) + Power(x,2)*(-9 + \
33*Power(z,2))))/16.;
break;
case 2321 :
ex = (Sqrt(455/(2.*Pi))*x*(-11*Power(x,4) + 11*Power(y,4) + 2*Power(z,2)*(-9 \
+ 11*Power(z,2)) + Power(x,2)*(9 + 11*Power(z,2)) + Power(y,2)*(-9 + \
33*Power(z,2))))/16.;
break;
case 3321 :
ex = -(Sqrt(455/(2.*Pi))*x*y*z*(-9 + 11*Power(x,2) + 11*Power(y,2) + \
11*Power(z,2)))/4.;
break;
case 1330 :
ex = (-3*Sqrt(105/Pi)*x*y*z)/8.;
break;
case 2330 :
ex = (-3*Sqrt(105/Pi)*(Power(x,2) - Power(y,2))*z)/16.;
break;
case 3330 :
ex = (-3*Sqrt(105/Pi)*y*(-3*Power(x,2) + Power(y,2)))/16.;
break;
case 1331 :
ex = -(Sqrt(1365/Pi)*x*y*z*(-9 + 11*Power(x,2) + 11*Power(y,2) + \
11*Power(z,2)))/16.;
break;
case 2331 :
ex = -(Sqrt(1365/Pi)*(Power(x,2) - Power(y,2))*z*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/32.;
break;
case 3331 :
ex = -(Sqrt(1365/Pi)*y*(-3*Power(x,2) + Power(y,2))*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/32.;
break;
case 1400 :
ex = (-3*Sqrt(55/Pi)*y*z*(3*Power(x,2) + 3*Power(y,2) - 4*Power(z,2)))/8.;
break;
case 2400 :
ex = (3*Sqrt(55/Pi)*x*z*(3*Power(x,2) + 3*Power(y,2) - 4*Power(z,2)))/8.;
break;
case 3400 :
ex = 0;
break;
case 1401 :
ex = (-15*Sqrt(3/Pi)*y*z*(39*Power(x,4) + 39*Power(y,4) + 44*Power(z,2) - \
52*Power(z,4) + Power(x,2)*(-33 + 78*Power(y,2) - 13*Power(z,2)) - \
Power(y,2)*(33 + 13*Power(z,2))))/16.;
break;
case 2401 :
ex = (15*Sqrt(3/Pi)*x*z*(39*Power(x,4) + 39*Power(y,4) + 44*Power(z,2) - \
52*Power(z,4) + Power(x,2)*(-33 + 78*Power(y,2) - 13*Power(z,2)) - \
Power(y,2)*(33 + 13*Power(z,2))))/16.;
break;
case 3401 :
ex = 0;
break;
case 1410 :
ex = (9*Sqrt(11/Pi)*x*y*(Power(x,2) + Power(y,2) - 6*Power(z,2)))/16.;
break;
case 2410 :
ex = (-3*Sqrt(11/Pi)*(3*Power(x,4) - 3*Power(y,2)*Power(z,2) + 4*Power(z,4) \
+ 3*Power(x,2)*(Power(y,2) - 7*Power(z,2))))/16.;
break;
case 3410 :
ex = (-3*Sqrt(11/Pi)*y*z*(3*Power(x,2) + 3*Power(y,2) - 4*Power(z,2)))/16.;
break;
case 1411 :
ex = (9*Sqrt(15/Pi)*x*y*(13*Power(x,4) + 13*Power(y,4) + 66*Power(z,2) - \
78*Power(z,4) + Power(x,2)*(-11 + 26*Power(y,2) - 65*Power(z,2)) - \
Power(y,2)*(11 + 65*Power(z,2))))/32.;
break;
case 2411 :
ex = (-3*Sqrt(15/Pi)*(39*Power(x,6) + 3*Power(x,4)*(-11 + 26*Power(y,2) - \
78*Power(z,2)) + Power(z,2)*(-39*Power(y,4) - 44*Power(z,2) + 52*Power(z,4) \
+ Power(y,2)*(33 + 13*Power(z,2))) + Power(x,2)*(39*Power(y,4) + \
231*Power(z,2) - 221*Power(z,4) - 3*Power(y,2)*(11 + 91*Power(z,2)))))/32.;
break;
case 3411 :
ex = (-3*Sqrt(15/Pi)*y*z*(39*Power(x,4) + 39*Power(y,4) + 44*Power(z,2) - \
52*Power(z,4) + Power(x,2)*(-33 + 78*Power(y,2) - 13*Power(z,2)) - \
Power(y,2)*(33 + 13*Power(z,2))))/32.;
break;
case 1420 :
ex = (3*Sqrt(11/(2.*Pi))*y*z*(3*Power(x,2) - 4*Power(y,2) + \
3*Power(z,2)))/4.;
break;
case 2420 :
ex = (-3*Sqrt(11/(2.*Pi))*x*z*(4*Power(x,2) - 3*(Power(y,2) + \
Power(z,2))))/4.;
break;
case 3420 :
ex = (3*Sqrt(11/(2.*Pi))*x*y*(Power(x,2) + Power(y,2) - 6*Power(z,2)))/4.;
break;
case 1421 :
ex = (3*Sqrt(15/(2.*Pi))*y*z*(39*Power(x,4) - 52*Power(y,4) - 33*Power(z,2) \
+ 39*Power(z,4) + Power(y,2)*(44 - 13*Power(z,2)) + Power(x,2)*(-33 - \
13*Power(y,2) + 78*Power(z,2))))/8.;
break;
case 2421 :
ex = (-3*Sqrt(15/(2.*Pi))*x*z*(52*Power(x,4) - 39*Power(y,4) + 33*Power(z,2) \
- 39*Power(z,4) + Power(y,2)*(33 - 78*Power(z,2)) + Power(x,2)*(-44 + \
13*Power(y,2) + 13*Power(z,2))))/8.;
break;
case 3421 :
ex = (3*Sqrt(15/(2.*Pi))*x*y*(13*Power(x,4) + 13*Power(y,4) + 66*Power(z,2) \
- 78*Power(z,4) + Power(x,2)*(-11 + 26*Power(y,2) - 65*Power(z,2)) - \
Power(y,2)*(11 + 65*Power(z,2))))/8.;
break;
case 1430 :
ex = (-3*Sqrt(77/Pi)*x*y*(Power(x,2) - 3*Power(y,2) + 6*Power(z,2)))/16.;
break;
case 2430 :
ex = (3*Sqrt(77/Pi)*(Power(x,4) + 3*Power(y,2)*Power(z,2) - \
3*Power(x,2)*(Power(y,2) + Power(z,2))))/16.;
break;
case 3430 :
ex = (-9*Sqrt(77/Pi)*y*(-3*Power(x,2) + Power(y,2))*z)/16.;
break;
case 1431 :
ex = (-3*Sqrt(105/Pi)*x*y*(13*Power(x,4) - 39*Power(y,4) - 66*Power(z,2) + \
78*Power(z,4) + Power(y,2)*(33 + 39*Power(z,2)) + Power(x,2)*(-11 - \
26*Power(y,2) + 91*Power(z,2))))/32.;
break;
case 2431 :
ex = (3*Sqrt(105/Pi)*(13*Power(x,6) + 3*Power(y,2)*Power(z,2)*(-11 + \
13*Power(y,2) + 13*Power(z,2)) - Power(x,4)*(11 + 26*Power(y,2) + \
26*Power(z,2)) + Power(x,2)*(-39*Power(y,4) + 33*Power(z,2) - 39*Power(z,4) \
+ Power(y,2)*(33 - 39*Power(z,2)))))/32.;
break;
case 3431 :
ex = (-9*Sqrt(105/Pi)*y*(-3*Power(x,2) + Power(y,2))*z*(-11 + 13*Power(x,2) \
+ 13*Power(y,2) + 13*Power(z,2)))/32.;
break;
case 1440 :
ex = (-3*Sqrt(77/(2.*Pi))*y*(-3*Power(x,2) + Power(y,2))*z)/8.;
break;
case 2440 :
ex = (3*Sqrt(77/(2.*Pi))*x*(Power(x,2) - 3*Power(y,2))*z)/8.;
break;
case 3440 :
ex = (-3*Sqrt(77/(2.*Pi))*x*y*(Power(x,2) - Power(y,2)))/2.;
break;
case 1441 :
ex = (-3*Sqrt(105/(2.*Pi))*y*(-3*Power(x,2) + Power(y,2))*z*(-11 + \
13*Power(x,2) + 13*Power(y,2) + 13*Power(z,2)))/16.;
break;
case 2441 :
ex = (3*Sqrt(105/(2.*Pi))*x*(Power(x,2) - 3*Power(y,2))*z*(-11 + \
13*Power(x,2) + 13*Power(y,2) + 13*Power(z,2)))/16.;
break;
case 3441 :
ex = (-3*Sqrt(105/(2.*Pi))*x*y*(Power(x,2) - Power(y,2))*(-11 + \
13*Power(x,2) + 13*Power(y,2) + 13*Power(z,2)))/4.;
break;
default:
fprintf(stderr,"not implemented");
break;
}
return ex;

}

double __fastcall TpointList::Imq(int j, int m, int n, int i, double x, double y, double z)
{
	double ex;
	int code;
        double Pi=3.141592653589793;

	code = i*1000+j*100+m*10+n;

/* Im q */
switch (code) {
case 1110 :
ex = (Sqrt(15/Pi)*z)/4.;
break;
case 2110 :
ex = 0;
break;
case 3110 :
ex = -(Sqrt(15/Pi)*x)/4.;
break;
case 1111 :
ex = (3*Sqrt(3/Pi)*z*(-5 + 7*Power(x,2) + 7*Power(y,2) + 7*Power(z,2)))/8.;
break;
case 2111 :
ex = 0;
break;
case 3111 :
ex = (-3*Sqrt(3/Pi)*x*(-5 + 7*Power(x,2) + 7*Power(y,2) + 7*Power(z,2)))/8.;
break;
case 1210 :
ex = (Sqrt(35/Pi)*(-Power(y,2) + Power(z,2)))/4.;
break;
case 2210 :
ex = (Sqrt(35/Pi)*x*y)/4.;
break;
case 3210 :
ex = -(Sqrt(35/Pi)*x*z)/4.;
break;
case 1211 :
ex = -(Sqrt(55/Pi)*(Power(y,2) - Power(z,2))*(-7 + 9*Power(x,2) + \
9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 2211 :
ex = (Sqrt(55/Pi)*x*y*(-7 + 9*Power(x,2) + 9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 3211 :
ex = -(Sqrt(55/Pi)*x*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + \
9*Power(z,2)))/8.;
break;
case 1220 :
ex = -(Sqrt(35/Pi)*x*z)/4.;
break;
case 2220 :
ex = (Sqrt(35/Pi)*y*z)/4.;
break;
case 3220 :
ex = (Sqrt(35/Pi)*(Power(x,2) - Power(y,2)))/4.;
break;
case 1221 :
ex = -(Sqrt(55/Pi)*x*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + \
9*Power(z,2)))/8.;
break;
case 2221 :
ex = (Sqrt(55/Pi)*y*z*(-7 + 9*Power(x,2) + 9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 3221 :
ex = (Sqrt(55/Pi)*(Power(x,2) - Power(y,2))*(-7 + 9*Power(x,2) + \
9*Power(y,2) + 9*Power(z,2)))/8.;
break;
case 1310 :
ex = (-3*Sqrt(7/Pi)*z*(Power(x,2) + 11*Power(y,2) - 4*Power(z,2)))/16.;
break;
case 2310 :
ex = (15*Sqrt(7/Pi)*x*y*z)/8.;
break;
case 3310 :
ex = (3*Sqrt(7/Pi)*x*(Power(x,2) + Power(y,2) - 4*Power(z,2)))/16.;
break;
case 1311 :
ex = (Sqrt(91/Pi)*z*(-11*Power(x,4) - 121*Power(y,4) + Power(y,2)*(99 - \
77*Power(z,2)) + 4*Power(z,2)*(-9 + 11*Power(z,2)) + Power(x,2)*(9 - \
132*Power(y,2) + 33*Power(z,2))))/32.;
break;
case 2311 :
ex = (5*Sqrt(91/Pi)*x*y*z*(-9 + 11*Power(x,2) + 11*Power(y,2) + \
11*Power(z,2)))/16.;
break;
case 3311 :
ex = (Sqrt(91/Pi)*x*(11*Power(x,4) + 11*Power(y,4) + 36*Power(z,2) - \
44*Power(z,4) + Power(x,2)*(-9 + 22*Power(y,2) - 33*Power(z,2)) - \
3*Power(y,2)*(3 + 11*Power(z,2))))/32.;
break;
case 1320 :
ex = (3*Sqrt(35/(2.*Pi))*x*(Power(y,2) - Power(z,2)))/4.;
break;
case 2320 :
ex = (-3*Sqrt(35/(2.*Pi))*y*(Power(x,2) - Power(z,2)))/4.;
break;
case 3320 :
ex = (3*Sqrt(35/(2.*Pi))*(Power(x,2) - Power(y,2))*z)/4.;
break;
case 1321 :
ex = (Sqrt(455/(2.*Pi))*x*(Power(y,2) - Power(z,2))*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/8.;
break;
case 2321 :
ex = -(Sqrt(455/(2.*Pi))*y*(Power(x,2) - Power(z,2))*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/8.;
break;
case 3321 :
ex = (Sqrt(455/(2.*Pi))*(Power(x,2) - Power(y,2))*z*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/8.;
break;
case 1330 :
ex = (3*Sqrt(105/Pi)*(Power(x,2) - Power(y,2))*z)/16.;
break;
case 2330 :
ex = (-3*Sqrt(105/Pi)*x*y*z)/8.;
break;
case 3330 :
ex = (-3*Sqrt(105/Pi)*x*(Power(x,2) - 3*Power(y,2)))/16.;
break;
case 1331 :
ex = (Sqrt(1365/Pi)*(Power(x,2) - Power(y,2))*z*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/32.;
break;
case 2331 :
ex = -(Sqrt(1365/Pi)*x*y*z*(-9 + 11*Power(x,2) + 11*Power(y,2) + \
11*Power(z,2)))/16.;
break;
case 3331 :
ex = -(Sqrt(1365/Pi)*x*(Power(x,2) - 3*Power(y,2))*(-9 + 11*Power(x,2) + \
11*Power(y,2) + 11*Power(z,2)))/32.;
break;
case 1410 :
ex = (3*Sqrt(11/Pi)*(3*Power(y,4) - 21*Power(y,2)*Power(z,2) + 4*Power(z,4) \
+ 3*Power(x,2)*(Power(y,2) - Power(z,2))))/16.;
break;
case 2410 :
ex = (-9*Sqrt(11/Pi)*x*y*(Power(x,2) + Power(y,2) - 6*Power(z,2)))/16.;
break;
case 3410 :
ex = (3*Sqrt(11/Pi)*x*z*(3*Power(x,2) + 3*Power(y,2) - 4*Power(z,2)))/16.;
break;
case 1411 :
ex = (3*Sqrt(15/Pi)*(39*Power(y,6) - 44*Power(z,4) + 52*Power(z,6) + \
39*Power(x,4)*(Power(y,2) - Power(z,2)) - 3*Power(y,4)*(11 + 78*Power(z,2)) \
+ Power(y,2)*(231*Power(z,2) - 221*Power(z,4)) + Power(x,2)*(78*Power(y,4) + \
Power(z,2)*(33 + 13*Power(z,2)) - 3*Power(y,2)*(11 + 91*Power(z,2)))))/32.;
break;
case 2411 :
ex = (-9*Sqrt(15/Pi)*x*y*(13*Power(x,4) + 13*Power(y,4) + 66*Power(z,2) - \
78*Power(z,4) + Power(x,2)*(-11 + 26*Power(y,2) - 65*Power(z,2)) - \
Power(y,2)*(11 + 65*Power(z,2))))/32.;
break;
case 3411 :
ex = (3*Sqrt(15/Pi)*x*z*(39*Power(x,4) + 39*Power(y,4) + 44*Power(z,2) - \
52*Power(z,4) + Power(x,2)*(-33 + 78*Power(y,2) - 13*Power(z,2)) - \
Power(y,2)*(33 + 13*Power(z,2))))/32.;
break;
case 1420 :
ex = (3*Sqrt(11/(2.*Pi))*x*z*(Power(x,2) + 15*Power(y,2) - \
6*Power(z,2)))/8.;
break;
case 2420 :
ex = (-3*Sqrt(11/(2.*Pi))*y*z*(15*Power(x,2) + Power(y,2) - \
6*Power(z,2)))/8.;
break;
case 3420 :
ex = (-3*Sqrt(11/(2.*Pi))*(Power(x,2) - Power(y,2))*(Power(x,2) + Power(y,2) \
- 6*Power(z,2)))/8.;
break;
case 1421 :
ex = (3*Sqrt(15/(2.*Pi))*x*z*(13*Power(x,4) + Power(x,2)*(-11 + \
208*Power(y,2) - 65*Power(z,2)) + 3*(65*Power(y,4) + 22*Power(z,2) - \
26*Power(z,4) + Power(y,2)*(-55 + 39*Power(z,2)))))/16.;
break;
case 2421 :
ex = (-3*Sqrt(15/(2.*Pi))*y*z*(195*Power(x,4) + 13*Power(y,4) + \
66*Power(z,2) - 78*Power(z,4) - Power(y,2)*(11 + 65*Power(z,2)) + \
Power(x,2)*(-165 + 208*Power(y,2) + 117*Power(z,2))))/16.;
break;
case 3421 :
ex = (-3*Sqrt(15/(2.*Pi))*(Power(x,2) - Power(y,2))*(13*Power(x,4) + \
13*Power(y,4) + 66*Power(z,2) - 78*Power(z,4) + Power(x,2)*(-11 + \
26*Power(y,2) - 65*Power(z,2)) - Power(y,2)*(11 + 65*Power(z,2))))/16.;
break;
case 1430 :
ex = (3*Sqrt(77/Pi)*(Power(y,4) - 3*Power(y,2)*Power(z,2) - \
3*Power(x,2)*(Power(y,2) - Power(z,2))))/16.;
break;
case 2430 :
ex = (3*Sqrt(77/Pi)*x*y*(3*Power(x,2) - Power(y,2) - 6*Power(z,2)))/16.;
break;
case 3430 :
ex = (-9*Sqrt(77/Pi)*x*(Power(x,2) - 3*Power(y,2))*z)/16.;
break;
case 1431 :
ex = (-3*Sqrt(105/Pi)*(39*Power(x,4)*(Power(y,2) - Power(z,2)) + \
Power(y,2)*(-13*Power(y,4) - 33*Power(z,2) + 39*Power(z,4) + Power(y,2)*(11 \
+ 26*Power(z,2))) + Power(x,2)*(26*Power(y,4) + 33*Power(z,2) - \
39*Power(z,4) + Power(y,2)*(-33 + 39*Power(z,2)))))/32.;
break;
case 2431 :
ex = (3*Sqrt(105/Pi)*x*y*(39*Power(x,4) - 13*Power(y,4) + 66*Power(z,2) - \
78*Power(z,4) + Power(y,2)*(11 - 91*Power(z,2)) + Power(x,2)*(-33 + \
26*Power(y,2) - 39*Power(z,2))))/32.;
break;
case 3431 :
ex = (-9*Sqrt(105/Pi)*x*(Power(x,2) - 3*Power(y,2))*z*(-11 + 13*Power(x,2) + \
13*Power(y,2) + 13*Power(z,2)))/32.;
break;
case 1440 :
ex = (-3*Sqrt(77/(2.*Pi))*x*(Power(x,2) - 3*Power(y,2))*z)/8.;
break;
case 2440 :
ex = (-3*Sqrt(77/(2.*Pi))*y*(-3*Power(x,2) + Power(y,2))*z)/8.;
break;
case 3440 :
ex = (3*Sqrt(77/(2.*Pi))*(Power(x,4) - 6*Power(x,2)*Power(y,2) + \
Power(y,4)))/8.;
break;
case 1441 :
ex = (-3*Sqrt(105/(2.*Pi))*x*(Power(x,2) - 3*Power(y,2))*z*(-11 + \
13*Power(x,2) + 13*Power(y,2) + 13*Power(z,2)))/16.;
break;
case 2441 :
ex = (-3*Sqrt(105/(2.*Pi))*y*(-3*Power(x,2) + Power(y,2))*z*(-11 + \
13*Power(x,2) + 13*Power(y,2) + 13*Power(z,2)))/16.;
break;
case 3441 :
ex = (3*Sqrt(105/(2.*Pi))*(Power(x,4) - 6*Power(x,2)*Power(y,2) + \
Power(y,4))*(-11 + 13*Power(x,2) + 13*Power(y,2) + 13*Power(z,2)))/16.;
break;
default:
fprintf(stderr,"not implemented");
break;
}
return ex;

}
//----------------------constructor-----------------------------------------
__fastcall TpointList::TpointList(void)
    : TObject()
{
        
        arraySize=300;
        maxRowIndex=300;
        kriging=false;
        //c1=50.;//10.; //weight of rel.divergence
        //c2=0.15;//0.25;//
        minTrajLength=10;
        PathSet=false;
        numOfFrames=200;
        changed=true;
}
//----------------------constructor-----------------------------------------


void __fastcall TmainForm::doGridBtnClick(TObject *Sender)
{
    if(!pointList){
       pointList=new TpointList();
    }
    pointList->setPathAndFiles2();
    mainForm->Refresh();

    //if(1<2){
       pointList->makeCor();
    //}
    
    

    pointList->count=0;
    pointList->meanPointsInSphere=0;
    pointList->count2=0;
    pointList->meanDiss=0.;
    for (int i=pointList->firstFile;i<pointList->lastFile+1;i++){
       fileNum2Edit->Text=IntToStr(i);
       fileNum2Edit->Refresh();
       meanDissEdit->Text=FloatToStr(pointList->meanDiss);
       meanDissEdit->Refresh();
       pointList->readXUAPFileOld(i,false);
       FILE *fpp;
       AnsiString name;
       name="grid."+IntToStr(i);
       const char *filename;
       filename=name.c_str();
       fpp = fopen(filename,"w");
       //pointList->doLinearInterp(fpp);
       fclose (fpp);
       meanSuccessDivEdit->Text=FloatToStr(100.*(double)pointList->count2/(double)pointList->count);
       meanSuccessDivEdit->Refresh();
    }
}
//---------------------------------------------------------------------------


void __fastcall TmainForm::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        Refresh();        
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::FormClick(TObject *Sender)
{
        Show();
        Refresh();        
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::volumeBtnClick(TObject *Sender)
{
    if(!pointList){
       pointList=new TpointList();
    }
    pointList->makeVolumes();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::surfaceBtnClick(TObject *Sender)
{
     if(!pointList){
       pointList=new TpointList();
    }
    pointList->makeSurfaces();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::lineBtnClick(TObject *Sender)
{
    if(!pointList){
       pointList=new TpointList();
    }
    pointList->makeLines();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::linkBtnClick(TObject *Sender)
{
    if(!pointList){
       pointList=new TpointList();
    }
    pointList->setPathAndFiles2();
    mainForm->Refresh();
    //if(1<2){
       pointList->makeCor();
    //}
    //pointList->reLink();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void __fastcall TmainForm::corrBtnClick(TObject *Sender)
{
    if(!pointList){
       pointList=new TpointList();
    }
    pointList->makeFilesForCorrelations();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::correlBtnClick(TObject *Sender)
{
    if(!pointList){
       pointList=new TpointList();
    }
    graphForm->Show();
    graphForm->Refresh();

    if(graphForm->readyCheck->Checked){
       pointList->setPathAndFiles2();
       mainForm->Refresh();

       pointList->makeCor();
    }


}
//---------------------------------------------------------------------------

void __fastcall TmainForm::paramBtnClick(TObject *Sender)
{
     //Edit1->Text=paramForm->splineEdit->Text;
     paramForm->Show();
}
//---------------------------------------------------------------------------



void __fastcall TmainForm::prevXUPBtnClick(TObject *Sender)
{
    int startFile=StrToInt(xuapForm->startFileEdit->Text);
    int startPoint=StrToInt(xuapForm->startPointEdit->Text);

    int fileInd;
    int pointInd;
    int numOnTrack=0;
    int counter=0;
    bool onTrack=true;
    double diffXsq,diffYsq,diffZsq;
    double rmsChangeX=0,rmsChangeY=0,rmsChangeZ=0;
    


    int PL=StrToInt(xuapForm->maxEdit->Text);
    int PLh=int((double)PL/2.);

    if(!pointList){
       pointList=new TpointList();
       pointList->setPathAndFiles();
       mainForm->Refresh();
    }
    if(startFile<pointList->firstFile){
       startFile=pointList->firstFile+10;
       xuapForm->startFileEdit->Text=IntToStr(startFile);
    }
    xuapForm->Show();
    xuapForm->Series1->Clear();
    xuapForm->Series2->Clear();
    xuapForm->Series3->Clear();
    xuapForm->Series4->Clear();
    xuapForm->Series5->Clear();
    xuapForm->Series6->Clear();
    xuapForm->Series7->Clear();
    xuapForm->Series8->Clear();
    xuapForm->Series9->Clear();
    xuapForm->Series10->Clear();
    xuapForm->Series11->Clear();
    xuapForm->Series12->Clear();
    xuapForm->Refresh();

    pointInd=startPoint;
    fileInd=startFile;
    while(onTrack){

       for (int ii=-PLh;ii<PLh+1;ii++){
          pointList->readPTVFile(fileInd,ii);
          //pointList->readAddedFile(fileInd,ii);
       }
       pointList->doCubicSplinesTwenty(true,pointInd);
       if(pointList->point[10][pointInd][14]>0){
          xuapForm->Series10->AddXY(numOnTrack,pointList->point[10][pointInd][2]*1000,'.',clTeeColor);
          xuapForm->Series11->AddXY(numOnTrack,pointList->point[10][pointInd][3]*1000,'.',clTeeColor);
          xuapForm->Series12->AddXY(numOnTrack,pointList->point[10][pointInd][4]*1000,'.',clTeeColor);
          xuapForm->Series1->AddXY(numOnTrack,pointList->point[10][pointInd][5]*1000,'.',clTeeColor);
          xuapForm->Series2->AddXY(numOnTrack,pointList->point[10][pointInd][6]*1000,'.',clTeeColor);
          xuapForm->Series3->AddXY(numOnTrack,pointList->point[10][pointInd][7]*1000,'.',clTeeColor);
          xuapForm->Series4->AddXY(numOnTrack,pointList->point[10][pointInd][8]*1000,'.',clTeeColor);
          xuapForm->Series5->AddXY(numOnTrack,pointList->point[10][pointInd][9]*1000,'.',clTeeColor);
          xuapForm->Series6->AddXY(numOnTrack,pointList->point[10][pointInd][10]*1000,'.',clTeeColor);
          xuapForm->Series7->AddXY(numOnTrack,pointList->point[10][pointInd][11]*1000,'.',clTeeColor);
          xuapForm->Series8->AddXY(numOnTrack,pointList->point[10][pointInd][12]*1000,'.',clTeeColor);
          xuapForm->Series9->AddXY(numOnTrack,pointList->point[10][pointInd][13]*1000,'.',clTeeColor);
       }

       diffXsq=pow(pointList->point[10][pointInd][2]-pointList->point[10][pointInd][5],2.);
       diffYsq=pow(pointList->point[10][pointInd][3]-pointList->point[10][pointInd][6],2.);
       diffZsq=pow(pointList->point[10][pointInd][4]-pointList->point[10][pointInd][7],2.);

       if(pointList->point[10][pointInd][14]>0){
           counter++;
              rmsChangeX=(rmsChangeX*((double)counter-1.)+diffXsq)/(double)counter;
              rmsChangeY=(rmsChangeY*((double)counter-1.)+diffYsq)/(double)counter;
              rmsChangeZ=(rmsChangeZ*((double)counter-1.)+diffZsq)/(double)counter;
       }
       xuapForm->avXEdit->Text=(FloatToStr(1000*pow(rmsChangeX,0.5)));
       xuapForm->avYEdit->Text=(FloatToStr(1000*pow(rmsChangeY,0.5)));
       xuapForm->avZEdit->Text=(FloatToStr(1000*pow(rmsChangeZ,0.5)));
       xuapForm->Refresh();

       if(pointList->point[10][pointInd][1]>0){
           fileInd++;
           pointInd=pointList->point[10][pointInd][1];
           numOnTrack++;
       }
       else{
           onTrack=false;
       }
    }
    xuapForm->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::prevTrajBtnClick(TObject *Sender)
{
    int startFile=StrToInt(trajForm->startFileEdit->Text);
    int startPoint;
    if(!pointList){
       pointList=new TpointList();
    }
    if(!pointList->PathSet){
       pointList->setPathAndFiles2();
       mainForm->Refresh();
    }

    for(int i=pointList->firstFile;i<pointList->lastFile+1;i++){
       for(int j=0;j<1000;j++){
           //pointList->occ[i][j]=false;
       }
    }

    FILE *fpp;
    AnsiString name;
    name="trajPoint."+IntToStr(startFile);
    const char *filename;
    filename=name.c_str();
    fpp = fopen(filename,"w");

    fileNum2Edit->Text=IntToStr(startFile);
    startPoint=StrToInt(trajForm->startPointEdit->Text);
    mainForm->Refresh();
    trajForm->Show();
    trajForm->Refresh();
    bool cont=true;
    while(cont){
       pointList->followTrajPointHarmonics(fpp,startFile,startPoint,false);
       if(pointList->numInTraj>19){
          cont=false;
       }
       else{
          startPoint++;
          trajForm->startPointEdit->Text=IntToStr(startPoint);
          trajForm->startPointEdit->Refresh();
       }
    }


}
//---------------------------------------------------------------------------

void __fastcall TmainForm::structBtnClick(TObject *Sender)
{
    if(!pointList){
       pointList=new TpointList();
    }
    structForm->Show();
    structForm->Refresh();
    if(structForm->readyCheck->Checked){
       pointList->setPathAndFiles2();
       mainForm->Refresh();
       structForm->Refresh();
       pointList->makeStruct();
    }
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::accuracyBtnClick(TObject *Sender)
{
    int startFile=StrToInt(trajForm->startFileEdit->Text);
    int stepFileSize=StrToInt(accuracyForm->stepFileEdit->Text);

    if(!(accuracyForm->Visible)){
       accuracyForm->Show();
    }
    else{
       accuracyForm->Show();
       if(!pointList){
          pointList=new TpointList();
       }
       if(!pointList->PathSet){
          pointList->setPathAndFiles2();
          mainForm->Refresh();
       }
       accuracyForm->Refresh();
       //set accuracyArrays to Zero
       for(int i=0;i<200;i++){
           pointList->indexCounterNo[i]=0.;
           pointList->kFluctArrayNo[i]=0.;
           pointList->kResidArrayNo[i]=0.;
           pointList->DuErrorArrayNo[i]=0.;
           pointList->DvErrorArrayNo[i]=0.;
           pointList->DwErrorArrayNo[i]=0.;
           pointList->kDistArrayNo[i]=0.;
           pointList->indexCounterDi[i]=0.;
           pointList->kFluctArrayDi[i]=0.;
           pointList->kResidArrayDi[i]=0.;
           pointList->DuErrorArrayDi[i]=0.;
           pointList->DvErrorArrayDi[i]=0.;
           pointList->DwErrorArrayDi[i]=0.;
           pointList->kNoArrayDi[i]=0.;
       }
       //end of set acuracyArrays to Zero

       for (int i=pointList->firstFile;i<pointList->lastFile+1;i=i+stepFileSize){
          fileNum2Edit->Text=IntToStr(startFile);
          mainForm->Refresh();
          pointList->estimateAccuracy(i);
          accuracyForm->Show();
          accuracyForm->Refresh();
       }
       //store in accuracy.res
       FILE *fpp;
       AnsiString name;
       name="accuracy.res";
       const char *filename;
       filename=name.c_str();
       fpp = fopen(filename,"w");
       for(int i=0;i<200;i++){
          fprintf(fpp, "%lf\t", 1e3*pointList->kFluctArrayDi[i]);
          fprintf(fpp, "%lf\t", 1e3*pointList->kResidArrayDi[i]);
          if(pointList->kFluctArrayDi[i]>0){
             fprintf(fpp, "%lf\n", 100*pointList->kResidArrayDi[i]/pointList->kFluctArrayDi[i]);
          }
          else{
             fprintf(fpp, "%lf\n", 0);
          }
       }
       fclose (fpp);
    }
}
//---------------------------------------------------------------------------




void __fastcall TmainForm::reprBtnClick(TObject *Sender)
{

    if(!pointList){
       pointList=new TpointList();
    }
    pointList->setPathAndFiles4();
    mainForm->Refresh();
    structReprForm->Show();
    structReprForm->Refresh();
    pointList->makeStructRepr();

}









void __fastcall TmainForm::rotaBtnClick(TObject *Sender)
{
    if(!pointList){
       pointList=new TpointList();
    }
    pointList->setPathAndFiles2();
    mainForm->Caption=pointList->baseName;
    mainForm->Refresh();

    pointList->count=0;
    pointList->meanPointsInSphere=0;
    pointList->meanPointsInSphereB=0;
    pointList->succIntTraj=0;
    pointList->count2=0;
    pointList->forMeanNumInTraj=0;
    pointList->forEffectiveMeanNumInTraj=0;
    pointList->forTypicalPolyOrder=0;

    pointList->count3=0;
    pointList->count4=0;
    pointList->count5=0;
    pointList->count6=0;
    pointList->count7=0;
    pointList->count8=0;
    pointList->count9=0;

    for(int i=0;i<10001;i++){
       for(int j=0;j<1500;j++){
           pointList->occ[i][j]=false;
       }
    }

    for (int i=pointList->firstFile;i<pointList->lastFile+1;i++){

       fileNum2Edit->Text=IntToStr(i);
       mainForm->Refresh();
       FILE *fpp;
       AnsiString name;
       name="trajAcc."+IntToStr(i);
       const char *filename;
       filename=name.c_str();
       fpp = fopen(filename,"w");
       pointList->followTrajPointHarmonics(fpp,i,0,true);
       fclose (fpp);
      
       
       mainForm->Refresh();
    }

    //exit(0 - '0');

}
//---------------------------------------------------------------------------





void __fastcall TmainForm::gridBtnClick(TObject *Sender)
{
    if(!pointList){
       pointList=new TpointList();
    }
    pointList->setPathAndFiles2();
    mainForm->Caption=pointList->baseName;
    mainForm->Refresh();

    for(int i=pointList->firstFile;i<pointList->lastFile+1;i++){
       for(int j=0;j<1000;j++){
           //pointList->occ[i][j]=false;
       }
    }

    pointList->count=0;
    pointList->meanPointsInSphere=0;
    pointList->meanPointsInSphereB=0;
    pointList->meanDiss=0.;
    pointList->meanUisq=0.;
    pointList->meanDudxsq=0.;
   
    pointList->count2=0;


    pointList->count3=0;
    pointList->count4=0;
    pointList->count5=0;
    pointList->count6=0;
    pointList->count7=0;
    pointList->count8=0;

    for (int i=pointList->firstFile;i<pointList->lastFile+1;i++){
       fileNum2Edit->Text=IntToStr(i);
       mainForm->Refresh();
       FILE *fpp;
       AnsiString name;
       name="grid."+IntToStr(i);
       const char *filename;
       filename=name.c_str();
       fpp = fopen(filename,"w");
       //pointList->followTrajPointLinQuadforAccDerivGrid(fpp,i,0,true);
       fclose (fpp);
      
       
       mainForm->Refresh();
    }      
}

//---------------------------------------------------------------------------


void __fastcall TmainForm::filterGridBtnClick(TObject *Sender)
{
   if(!pointList){
       pointList=new TpointList();
    }
    pointList->setPathAndFiles5();
    mainForm->Caption=pointList->baseName;
    mainForm->Refresh();


    pointList->count=0;
    pointList->meanDiss=0.;
    pointList->meanUisq=0.;
    pointList->meanDudxsq=0.;
   
    pointList->count2=0;

    pointList->count3=0;
    pointList->count4=0;
    pointList->count5=0;
    pointList->count6=0;
    pointList->count7=0;
    pointList->count8=0;
    pointList->count9=0;
    
    int filtSize=0;
    double viscosity=StrToFloat(mainForm->viscEdit->Text);
    int Reynolds;

    for (int i=pointList->firstFile-filtSize;i<pointList->lastFile+1;i++){
       fileNum2Edit->Text=IntToStr(i);
       mainForm->Refresh();
       FILE *fpp;
       AnsiString name;
       name="gridFilt."+IntToStr(i);
       const char *filename;
       filename=name.c_str();
       fpp = fopen(filename,"w");
       pointList->FilterGrid(fpp,i);
       fclose (fpp);
       if(pointList->count>0){
          
          meanDissEdit->Text=IntToStr((int)(1.e6*pointList->meanDiss/(double)pointList->count2+0.5));
          Reynolds=(int)((pow(pointList->meanUisq/(double)pointList->count2,0.5)*pow(pointList->meanUisq/pointList->meanDudxsq,0.5))/viscosity+0.5);
          mainForm->reEdit->Text=IntToStr(Reynolds);
         
       }
       ///end urvature and grad kinetic energy stuff
       mainForm->Refresh();
    }        
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::risoETHBtnClick(TObject *Sender)
{
   pointList=new TpointList();
   pointList->setPathAndFiles7();
   pointList->readWriteRisoETH();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::ETHRisoeBtnClick(TObject *Sender)
{
   pointList=new TpointList();
   pointList->setPathAndFiles8();
   pointList->readWriteETHRiso();
}
//---------------------------------------------------------------------------
static void sort(double minDistB[],int minDistBIndex[],int index)
{
   bool done=false;
   double hlpVal;
   int hlpInt;

   //bubble sorting
   while(!done){
       done=true;
       for(int i=index-1;i>0;i--){
           if(minDistB[i]<minDistB[i-1]){
              //exchange
              done=false;
              hlpVal=minDistB[i];
              hlpInt=minDistBIndex[i];
              minDistB[i]=minDistB[i-1];
              minDistBIndex[i]=minDistBIndex[i-1];
              minDistB[i-1]=hlpVal;
              minDistBIndex[i-1]=hlpInt;
           }
       }
   }
}
//---------------------------------------------------------------------------










void __fastcall TmainForm::Button1Click(TObject *Sender)
{
     pointList=new TpointList();
     pointList->setPathAndFiles9();
     pointList->writeBinary();
}
//---------------------------------------------------------------------------

