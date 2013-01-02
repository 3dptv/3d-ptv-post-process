//---------------------------------------------------------------------------
#include <vcl.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>                       
#include <stdio.h>
#pragma hdrstop

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
    double x,y,z;

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

           point[index+10][i][0]=left+1;
           point[index+10][i][1]=right+1;

           point[index+10][i][2]=x*0.001;
           point[index+10][i][3]=y*0.001;
           point[index+10][i][4]=z*0.001;
           point[index+10][i][15]=0.01;
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
void __fastcall TpointList::readXUAPFile(int n, bool firstTime, bool estimate)
{
    FILE *fpp;
    int numOfPoints;
    double left,right,x,y,z,u,v,w,ax,ay,az,dummy,cubic,quality;

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
          //new quality
          //point[2][numOfPoints][12]=quality;
       }
       fclose (fpp);
       point[2][0][0]=numOfPoints++;
    }
    else{
       for(int i=0;i<200;i++){
          if(n-2+i>firstFile-1 && n-2+i<lastFile+1){
             if(i<numOfFrames-1 && !(firstTime)){
                for(int j=0;j<point[i+1][0][0]+1;j++){
                    for(int k=0;k<16;k++){
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
    if(mainForm->interpolRadioGroup->ItemIndex==0){
       numFields=32;
    }
    else{
       numFields=47;
    }

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
   bool PointIsOk[21];
   double tolerance=0.15;//StrToFloat(paramForm->toleranceEdit->Text);
   double tolMaxVel=StrToFloat(mainForm->tolMaxVelEdit->Text);
   double velocity;

   deltaT=StrToFloat(mainForm->deltaTEdit->Text);

   int start,end;
   if(!single){
      start=1;
      end=nP;
   }
   else{
      start=number;
      end=number+1;
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
      //check for bad quality contributions and mark them!
      for(int t=minIndex-10;t<maxIndex-10+1;t++){
          PointIsOk[t+10]=true;
          if(point[t+10][ind[t+10]][15]>tolerance){
              PointIsOk[t+10]=false;
              badCounter++;
          }
      }
      //end check for bad quality check
      //first do for x and u, then do for a
      if(maxIndex-minIndex>2+badCounter && maxIndex>9+minLength && minIndex<11-minLength){ //ok (minIndex<10 && maxIndex>10){//
      //if(maxIndex-minIndex>2+badCounter ){ //ok (minIndex<10 && maxIndex>10){//
          point[10][i][14]=1;
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

          point[10][i][5]=X[0];//point[10][ind[10]][2];//
          point[10][i][8]=X[1];//(1./(2.*deltaT))*(point[11][ind[11]][2]-point[9][ind[9]][2]);//
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

          point[10][i][6]=X[0]; //point[10][ind[10]][3];//
          point[10][i][9]=X[1]; //(1./(2.*deltaT))*(point[11][ind[11]][3]-point[9][ind[9]][3]);//
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
          
          point[10][i][7]=X[0]; //point[10][ind[10]][4];//
          point[10][i][10]=X[1];//(1./(2.*deltaT))*(point[11][ind[11]][4]-point[9][ind[9]][4]);//
          point[10][i][13]=2.*X[2]; //(1./(deltaT*deltaT))*(point[11][ind[11]][4]-2.*point[10][ind[10]][4]+point[9][ind[9]][4]);//
          //max break!
          velocity=pow(pow(point[10][i][8],2.)+pow(point[10][i][9],2.)+pow(point[10][i][10],2.),0.5);
          if(velocity>tolMaxVel){
             point[10][i][14]=0;
          }
      }
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
                  i=minDistIndex[d];
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
                  pCounterB++;
               }
               //here we should be able to get the fluctuation energy whithin the sphere
               uMean=0;
               vMean=0;
               wMean=0;
               for(int d=0;d<pCounterB;d++){
                  uMean=uMean+YuB[d];
                  vMean=vMean+YvB[d];
                  wMean=wMean+YwB[d];
               }
               uMean=uMean/(double)pCounterB;
               vMean=vMean/(double)pCounterB;
               wMean=wMean/(double)pCounterB;
               kFluct=0;
               for(int d=0;d<pCounterB;d++){
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
              }//end of solving
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
void __fastcall TpointList::followTrajPointLinQuadforAccDeriv(FILE *fpp, int t,int startPoint, bool write)
{
     int pCounterB, pCounter, rowIndex;
     int startT, startP,ind;
     double dist,dx,dy,dz,dt;
     double centerX,centerY,centerZ;
     double Liu[5],Liv[5],Liw[5],Liax[4],Liay[4],Liaz[4];
     double Linx[4],Liny[4],Linz[4],Lik[4];
     double w1,w2,w3,s11,s12,s13,s22,s23,s33,vel,wsq,twosijsij,Q;
     double div,ref,divCriteria;
     double diva,divaCriteria;
     double dix,diy,diz,refx,refy,refz,accCriteria,absdi;
     double acx,acy,acz;
     double cnx,cny,cnz,curvGrad,curv,curvCriteria,Lx,Ly,Lz,gradCriteria;

     int time;
     double minDistB[200];
     int minDistBIndex[200];
     bool contin;

     int rank;

     int start;
     int end;
     double polyConst=StrToFloat(mainForm->polyConstEdit->Text);

     double maxRadiusSpat=StrToFloat(mainForm->radiusSpatEdit->Text);
     double maxRadiusTemp=StrToFloat(mainForm->radiusTempEdit->Text);
     int minCounter;

     bool ok;
     startT=t;

     bool continuePast;
     bool continueFuture;
     int step[5];

     double weightDivU=StrToFloat(mainForm->divEdit->Text);
     double weightAcc=StrToFloat(mainForm->accEdit->Text);
     double weightDivA=StrToFloat(mainForm->divaEdit->Text);
     double weightCurv=StrToFloat(mainForm->curvEdit->Text);
     double weightGradK=StrToFloat(mainForm->gradKEdit->Text);

     double viscosity=StrToFloat(mainForm->viscEdit->Text);
     deltaT=StrToFloat(mainForm->deltaTEdit->Text);
     c1=StrToFloat(mainForm->c1Edit->Text);
     c2=StrToFloat(mainForm->c2Edit->Text);


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
         end=point[2][0][0]+1;
     }
     else{
         start=startPoint;
         end=startPoint+1;
     }

     int n;
     for(int nn=start;nn<end;nn++){
         time=2;
         if(point[2][nn][11]>0. && !(occ[t][nn]) ){
            startP=nn;
            ok=true;
            numInTraj=0;
            noDeriv=0;
            n=nn;
            while(ok){
               occ[t+time-2][n]=true;
               //interpolieren und rausschreiben mit t,n (Zeit und Startpunkt)
               //%Da soll jetzt duidxj linear interpoliert werden
               //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
               //%die nächsten Punkte zu Punkt x,y,z, finden
               count++;
               if(mainForm->spatialRadioGroup->ItemIndex==0){
                  setAllMatrixesToZero(4);
               }
               if(mainForm->spatialRadioGroup->ItemIndex==1){
                  setAllMatrixesToZero(10);
               }
               if(mainForm->spatialRadioGroup->ItemIndex==2){
                  setAllMatrixesToZero(20);
               }
               for(int i=0;i<arraySize;i++){
                  dis0[i]=0.;
                  disA[i]=0.;
                  disB[i]=0.;
                  disC[i]=0.;
                  disD[i]=0.;
               }
               centerX=point[time][n][2];
               centerY=point[time][n][3];
               centerZ=point[time][n][4];


               for(int i=0;i<200+1;i++){
                  minDistB[i]=1000+i;
                  minDistBIndex[i]=1000+i;
               }


               //BBBBBBBBBBBBBBB
               int index=0;
               for(int i=1;i<point[time][0][0]+1;i++){
                  dist=pow(pow(point[time][i][2]-centerX,2.)+pow(point[time][i][3]-centerY,2.)+pow(point[time][i][4]-centerZ,2.),0.5);
                  if(dist<maxRadiusSpat){
                      minDistB[index]=dist;
                      minDistBIndex[index]=i;
                      index++;
                  }
               }

               pCounterB=0;
               pCounter=0;
               int i;
               for(int pointInd=0;pointInd<index;pointInd++){
                  i=minDistBIndex[pointInd];
                  if(point[time][i][11]>0.){
                     dist=pow(pow(point[time][i][2]-centerX,2.)+pow(point[time][i][3]-centerY,2.)+pow(point[time][i][4]-centerZ,2.),0.5);
                     disB[pCounterB]=(int)(dist*1000.+0.5);
                     dx=point[time][i][2]-centerX;
                     dy=point[time][i][3]-centerY;
                     dz=point[time][i][4]-centerZ;
                     B[pCounterB][0]=1.;
                     B[pCounterB][1]=dx;
                     B[pCounterB][2]=dy;
                     B[pCounterB][3]=dz;
                     if(mainForm->spatialRadioGroup->ItemIndex==1){
                        B[pCounterB][4]=dx*dx;
                        B[pCounterB][5]=dy*dy;
                        B[pCounterB][6]=dz*dz;
                        B[pCounterB][7]=dx*dy;
                        B[pCounterB][8]=dx*dz;
                        B[pCounterB][9]=dy*dz;
                     }
                     if(mainForm->spatialRadioGroup->ItemIndex==2){
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
                     YuB[pCounterB]=point[time][i][5];
                     YvB[pCounterB]=point[time][i][6];
                     YwB[pCounterB]=point[time][i][7];
                     y0[pCounterB]=point[time][i][8];
                     y1[pCounterB]=point[time][i][9];
                     y2[pCounterB]=point[time][i][10];
                     ///curvature and grad kinetic energy stuff
                     if(write && mainForm->interpolRadioGroup->ItemIndex==1){
                        vel=pow( pow(point[time][i][5],2.)
                                +pow(point[time][i][6],2.)
                                +pow(point[time][i][7],2.),0.5);
                        y3[pCounterB]=point[time][i][5]/vel;
                        y4[pCounterB]=point[time][i][6]/vel;
                        y5[pCounterB]=point[time][i][7]/vel;
                        y6[pCounterB]=0.5*vel*vel;//kinetic energy
                     }
                     ///end urvature and grad kinetic energy stuff
                     pCounterB++;
                     if(minDistB[pointInd]<maxRadiusTemp){
                     /////////for du/dt likeold scheme!
                     /////////////////////////////////
                     step[0]=0;
                     step[1]=-1;
                     step[2]=-2;
                     step[3]= 1;
                     step[4]= 2;
                     ind=i;
                     continuePast=true;
                     continueFuture=true;
                     for(int j=0;j<5;j++){
                        if((j<3 && continuePast) || (j>2 && continueFuture)){
                           dx=point[time+step[j]][ind][2]-centerX;
                           dy=point[time+step[j]][ind][3]-centerY;
                           dz=point[time+step[j]][ind][4]-centerZ;
                           A[pCounter][0]=1.;
                           A[pCounter][1]=dx;
                           A[pCounter][2]=dy;
                           A[pCounter][3]=dz;
                           A[pCounter][4]=(double)step[j]*deltaT;

                           YuA[pCounter]=point[time+step[j]][ind][5];
                           YvA[pCounter]=point[time+step[j]][ind][6];
                           YwA[pCounter]=point[time+step[j]][ind][7];
                           pCounter++;
                           if(j<2){
                              if(point[time+step[j]][ind][0]>0){
                                 ind=point[time+step[j]][ind][0];
                              }
                              else{
                                  continuePast=false;
                              }
                           }
                           if(j>2){
                              if(point[time+step[j]][ind][1]>0){
                                 ind=point[time+step[j]][ind][1];
                              }
                              else{
                                  continueFuture=false;
                              }
                           }
                        }
                        if(j==2){
                           ind=i;
                           step[2]=0;
                           if(point[time+step[j]][ind][1]>0){
                              ind=point[time+step[j]][ind][1];
                           }
                           else{
                              continueFuture=false;
                           }
                        }
                     }
                     /////////////////////////////////
                     }
                  }
               }

               
               meanPointsInSphereB=(meanPointsInSphereB*(double)(count-1)+(double)pCounterB)/(double)count;
               meanPointsInSphere =(meanPointsInSphere *(double)(count-1)+(double)pCounter )/(double)count;
               if(mainForm->spatialRadioGroup->ItemIndex==0){
                  minCounter=3;
               }
               if(mainForm->spatialRadioGroup->ItemIndex==1){
                  minCounter=9;
               }
               if(mainForm->spatialRadioGroup->ItemIndex==2){
                  minCounter=19;
               }
               if(pCounterB>minCounter ){ // %jetzt wird endlich Punkt1 interpoliert
                  //%correct x,y,z with center of interpolation!

                  contin=true;
                  if(mainForm->spatialRadioGroup->ItemIndex==0){
                     makeBT(pCounterB,4);
                     makeBTB(pCounterB,4);
                     makeBTY(pCounterB,4,1);
                     contin=solveB(pCounterB,4);
                  }
                  if(mainForm->spatialRadioGroup->ItemIndex==1){
                     makeBT(pCounterB,10);
                     makeBTB(pCounterB,10);
                     makeBTY(pCounterB,10,1);
                     contin=solveB(pCounterB,10);
                  }
                  if(mainForm->spatialRadioGroup->ItemIndex==2){
                     makeBT(pCounterB,20);
                     makeBTB(pCounterB,20);
                     makeBTY(pCounterB,20,1);
                     contin=solveB(pCounterB,20);
                  }
                  if(contin){
                     Liu[0]=point[time][n][5];//[8];
                     Liu[1]=X[1];
                     Liu[2]=X[2];
                     Liu[3]=X[3];
                     if(pCounter>4){
                        makeAT(pCounter,5);
                        makeATA(pCounter,5);
                        makeATY(pCounter,5,1);
                        solve(pCounter,5);
                        Liu[4]=X[4];
                     }
                     else{
                        contin=false;
                     }
                     if(contin){
                        if(mainForm->spatialRadioGroup->ItemIndex==0){
                           makeBT(pCounterB,4);
                           makeBTB(pCounterB,4);
                           makeBTY(pCounterB,4,2);
                           contin=solveB(pCounterB,4);
                        }
                        if(mainForm->spatialRadioGroup->ItemIndex==1){
                           makeBT(pCounterB,10);
                           makeBTB(pCounterB,10);
                           makeBTY(pCounterB,10,2);
                           contin=solveB(pCounterB,10);
                        }
                        if(mainForm->spatialRadioGroup->ItemIndex==2){
                           makeBT(pCounterB,20);
                           makeBTB(pCounterB,20);
                           makeBTY(pCounterB,20,2);
                           contin=solveB(pCounterB,20);
                        }
                        if(contin){
                           Liv[0]=point[time][n][6];//[9];
                           Liv[1]=X[1];
                           Liv[2]=X[2];
                           Liv[3]=X[3];
                           if(pCounter>4){
                              makeAT(pCounter,5);
                              makeATA(pCounter,5);
                              makeATY(pCounter,5,2);
                              solve(pCounter,5);
                              Liv[4]=X[4];
                           }
                           else{
                              contin=false;
                           }
                           if(contin){
                              if(mainForm->spatialRadioGroup->ItemIndex==0){
                                 makeBT(pCounterB,4);
                                 makeBTB(pCounterB,4);
                                 makeBTY(pCounterB,4,3);
                                 contin=solveB(pCounterB,4);
                              }
                              if(mainForm->spatialRadioGroup->ItemIndex==1){
                                 makeBT(pCounterB,10);
                                 makeBTB(pCounterB,10);
                                 makeBTY(pCounterB,10,3);
                                 contin=solveB(pCounterB,10);
                              }
                              if(mainForm->spatialRadioGroup->ItemIndex==2){
                                 makeBT(pCounterB,20);
                                 makeBTB(pCounterB,20);
                                 makeBTY(pCounterB,20,3);
                                 contin=solveB(pCounterB,20);
                              }
                              if(contin){
                                 Liw[0]=point[time][n][7];//[10];
                                 Liw[1]=X[1];
                                 Liw[2]=X[2];
                                 Liw[3]=X[3];
                                 if(pCounter>4){
                                    makeAT(pCounter,5);
                                    makeATA(pCounter,5);
                                    makeATY(pCounter,5,3);
                                    solve(pCounter,5);
                                    Liw[4]=X[4];
                                 }
                                 else{
                                    contin=false;
                                 }
                                 if(contin){
                                    if(mainForm->spatialRadioGroup->ItemIndex==0){
                                       makeBT(pCounterB,4);
                                       makeBTB(pCounterB,4);
                                       makeBTY(pCounterB,4,4);
                                       contin=solveB(pCounterB,4);
                                    }
                                    if(mainForm->spatialRadioGroup->ItemIndex==1){
                                       makeBT(pCounterB,10);
                                       makeBTB(pCounterB,10);
                                       makeBTY(pCounterB,10,4);
                                       contin=solveB(pCounterB,10);
                                    }
                                    if(mainForm->spatialRadioGroup->ItemIndex==2){
                                       makeBT(pCounterB,20);
                                       makeBTB(pCounterB,20);
                                       makeBTY(pCounterB,20,4);
                                       contin=solveB(pCounterB,20);
                                    }
                                    if(contin){
                                       Liax[0]=X[0];//point[time][n][8];
                                       Liax[1]=X[1];
                                       Liax[2]=X[2];
                                       Liax[3]=X[3];
                                       if(contin){
                                          if(mainForm->spatialRadioGroup->ItemIndex==0){
                                             makeBT(pCounterB,4);
                                             makeBTB(pCounterB,4);
                                             makeBTY(pCounterB,4,5);
                                             contin=solveB(pCounterB,4);
                                          }
                                          if(mainForm->spatialRadioGroup->ItemIndex==1){
                                             makeBT(pCounterB,10);
                                             makeBTB(pCounterB,10);
                                             makeBTY(pCounterB,10,5);
                                             contin=solveB(pCounterB,10);
                                          }
                                          if(mainForm->spatialRadioGroup->ItemIndex==2){
                                             makeBT(pCounterB,20);
                                             makeBTB(pCounterB,20);
                                             makeBTY(pCounterB,20,5);
                                             contin=solveB(pCounterB,20);
                                          }
                                          if(contin){
                                             Liay[0]=X[0];//point[time][n][9];
                                             Liay[1]=X[1];
                                             Liay[2]=X[2];
                                             Liay[3]=X[3];
                                             if(contin){
                                                if(mainForm->spatialRadioGroup->ItemIndex==0){
                                                   makeBT(pCounterB,4);
                                                   makeBTB(pCounterB,4);
                                                   makeBTY(pCounterB,4,6);
                                                   contin=solveB(pCounterB,4);
                                                }
                                                if(mainForm->spatialRadioGroup->ItemIndex==1){
                                                   makeBT(pCounterB,10);
                                                   makeBTB(pCounterB,10);
                                                   makeBTY(pCounterB,10,6);
                                                   contin=solveB(pCounterB,10);
                                                }
                                                if(mainForm->spatialRadioGroup->ItemIndex==2){
                                                   makeBT(pCounterB,20);
                                                   makeBTB(pCounterB,20);
                                                   makeBTY(pCounterB,20,6);
                                                   contin=solveB(pCounterB,20);
                                                }
                                                if(contin){
                                                   Liaz[0]=X[0];//point[time][n][10];
                                                   Liaz[1]=X[1];
                                                   Liaz[2]=X[2];
                                                   Liaz[3]=X[3];
                                                   if(contin){
                                                      ///curvature and grad kinetic energy stuff
                                                      if(mainForm->interpolRadioGroup->ItemIndex==1){
                                                         //nx
                                                         if(mainForm->spatialRadioGroup->ItemIndex==0){
                                                            makeBT(pCounterB,4);
                                                            makeBTB(pCounterB,4);
                                                            makeBTY(pCounterB,4,7);
                                                            contin=solveB(pCounterB,4);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==1){
                                                            makeBT(pCounterB,10);
                                                            makeBTB(pCounterB,10);
                                                            makeBTY(pCounterB,10,7);
                                                            contin=solveB(pCounterB,10);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==2){
                                                            makeBT(pCounterB,20);
                                                            makeBTB(pCounterB,20);
                                                            makeBTY(pCounterB,20,7);
                                                            contin=solveB(pCounterB,20);
                                                         }
                                                         vel=pow( pow(point[time][n][5],2.)
                                                                 +pow(point[time][n][6],2.)
                                                                 +pow(point[time][n][7],2.),0.5);
                                                         Linx[0]=point[time][n][5]/vel;
                                                         Linx[1]=X[1];
                                                         Linx[2]=X[2];
                                                         Linx[3]=X[3];
                                                         //ny
                                                         if(mainForm->spatialRadioGroup->ItemIndex==0){
                                                            makeBT(pCounterB,4);
                                                            makeBTB(pCounterB,4);
                                                            makeBTY(pCounterB,4,8);
                                                            contin=solveB(pCounterB,4);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==1){
                                                            makeBT(pCounterB,10);
                                                            makeBTB(pCounterB,10);
                                                            makeBTY(pCounterB,10,8);
                                                            contin=solveB(pCounterB,10);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==2){
                                                            makeBT(pCounterB,20);
                                                            makeBTB(pCounterB,20);
                                                            makeBTY(pCounterB,20,8);
                                                            contin=solveB(pCounterB,20);
                                                         }
                                                         vel=pow( pow(point[time][n][5],2.)
                                                                 +pow(point[time][n][6],2.)
                                                                 +pow(point[time][n][7],2.),0.5);
                                                         Liny[0]=point[time][n][6]/vel;
                                                         Liny[1]=X[1];
                                                         Liny[2]=X[2];
                                                         Liny[3]=X[3];
                                                         //nz
                                                         if(mainForm->spatialRadioGroup->ItemIndex==0){
                                                            makeBT(pCounterB,4);
                                                            makeBTB(pCounterB,4);
                                                            makeBTY(pCounterB,4,9);
                                                            contin=solveB(pCounterB,4);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==1){
                                                            makeBT(pCounterB,10);
                                                            makeBTB(pCounterB,10);
                                                            makeBTY(pCounterB,10,9);
                                                            contin=solveB(pCounterB,10);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==2){
                                                            makeBT(pCounterB,20);
                                                            makeBTB(pCounterB,20);
                                                            makeBTY(pCounterB,20,9);
                                                            contin=solveB(pCounterB,20);
                                                         }
                                                         vel=pow( pow(point[time][n][5],2.)
                                                                 +pow(point[time][n][6],2.)
                                                                 +pow(point[time][n][7],2.),0.5);
                                                         Linz[0]=point[time][n][7]/vel;
                                                         Linz[1]=X[1];
                                                         Linz[2]=X[2];
                                                         Linz[3]=X[3];
                                                         //grad k
                                                         if(mainForm->spatialRadioGroup->ItemIndex==0){
                                                            makeBT(pCounterB,4);
                                                            makeBTB(pCounterB,4);
                                                            makeBTY(pCounterB,4,10);
                                                            contin=solveB(pCounterB,4);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==1){
                                                            makeBT(pCounterB,10);
                                                            makeBTB(pCounterB,10);
                                                            makeBTY(pCounterB,10,10);
                                                            contin=solveB(pCounterB,10);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==2){
                                                            makeBT(pCounterB,20);
                                                            makeBTB(pCounterB,20);
                                                            makeBTY(pCounterB,20,10);
                                                            contin=solveB(pCounterB,20);
                                                         }
                                                         vel=pow( pow(Liu[0],2.)
                                                                 +pow(Liv[0],2.)
                                                                 +pow(Liw[0],2.),0.5);
                                                         Lik[0]=0.5*vel*vel;
                                                         Lik[1]=X[1];
                                                         Lik[2]=X[2];
                                                         Lik[3]=X[3];
                                                      }
                                                      ///end urvature and grad kinetic energy stuff
                                                      traj[numInTraj][ 0]=point[time][n][2];//x
                                                      traj[numInTraj][ 1]=point[time][n][3];//y
                                                      traj[numInTraj][ 2]=point[time][n][4];//z
                                                      traj[numInTraj][ 3]=Liu[0]; //u
                                                      traj[numInTraj][ 4]=Liv[0]; //v
                                                      traj[numInTraj][ 5]=Liw[0]; //w
                                                      traj[numInTraj][ 6]=Liu[1]; //du/dx
                                                      traj[numInTraj][ 7]=Liu[2]; //du/dy
                                                      traj[numInTraj][ 8]=Liu[3]; //du/dz
                                                      traj[numInTraj][ 9]=Liv[1]; //dv/dx
                                                      traj[numInTraj][10]=Liv[2]; //dv/dy
                                                      traj[numInTraj][11]=Liv[3]; //dv/dz
                                                      traj[numInTraj][12]=Liw[1]; //dw/dx
                                                      traj[numInTraj][13]=Liw[2]; //dw/dy
                                                      traj[numInTraj][14]=Liw[3]; //dw/dz
                                                      traj[numInTraj][15]=Liu[4]; //du/dt
                                                      traj[numInTraj][16]=Liv[4]; //dv/dt
                                                      traj[numInTraj][17]=Liw[4]; //dw/dt
                                                      traj[numInTraj][18]=Liax[0]; //ax
                                                      traj[numInTraj][19]=Liay[0]; //ay
                                                      traj[numInTraj][20]=Liaz[0]; //az
                                                      traj[numInTraj][21]=Liax[1]; //da_x/dx
                                                      traj[numInTraj][22]=Liax[2]; //da_x/dy
                                                      traj[numInTraj][23]=Liax[3]; //da_x/dz
                                                      traj[numInTraj][24]=Liay[1]; //da_y/dx
                                                      traj[numInTraj][25]=Liay[2]; //da_y/dy
                                                      traj[numInTraj][26]=Liay[3]; //da_y/dz
                                                      traj[numInTraj][27]=Liaz[1]; //da_z/dx
                                                      traj[numInTraj][28]=Liaz[2]; //da_z/dy
                                                      traj[numInTraj][29]=Liaz[3]; //da_z/dz


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
                                                      dix=Liax[0]-Liu[4]-Liu[0]*Liu[1]-Liv[0]*Liu[2]-Liw[0]*Liu[3];
                                                      diy=Liay[0]-Liv[4]-Liu[0]*Liv[1]-Liv[0]*Liv[2]-Liw[0]*Liv[3];
                                                      diz=Liaz[0]-Liw[4]-Liu[0]*Liw[1]-Liv[0]*Liw[2]-Liw[0]*Liw[3];
                                                      absdi=pow(dix*dix+diy*diy+diz*diz,0.5);
                                                      refx= fabs(Liax[0])
                                                           +fabs(Liu[4])
                                                           +fabs( Liu[0]*Liu[1]
                                                                 +Liv[0]*Liu[2]
                                                                 +Liw[0]*Liu[3]);
                                                      refy= fabs(Liay[0])
                                                           +fabs(Liv[4])
                                                           +fabs( Liu[0]*Liv[1]
                                                                 +Liv[0]*Liv[2]
                                                                 +Liw[0]*Liv[3]);
                                                      refz= fabs(Liaz[0])
                                                           +fabs(Liw[4])
                                                           +fabs( Liu[0]*Liw[1]
                                                                 +Liv[0]*Liw[2]
                                                                 +Liw[0]*Liw[3]);
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
                                                      diva=Liax[1]+Liay[2]+Liaz[3];
                                                      if(fabs(diva)+fabs(4*Q)>0){
                                                         divaCriteria=fabs(diva+4*Q)/(fabs(diva)+fabs(4*Q));
                                                      }
                                                      else{
                                                         divaCriteria=0.95;
                                                      }
                                                      if(mainForm->interpolRadioGroup->ItemIndex==0){
                                                         traj[numInTraj][30]= weightDivU*divCriteria
                                                                             +weightAcc*accCriteria
                                                                             +weightDivA*divaCriteria;
                                                      }
                                                      if(traj[numInTraj][30]>0.95){
                                                         traj[numInTraj][30]=0.95;
                                                      }
                                                      traj[numInTraj][31]=n;
                                                      ///curvature and grad kinetic energy stuff
                                                      if(mainForm->interpolRadioGroup->ItemIndex==1){
                                                          ///traj[numInTraj][33..47]
                                                          traj[numInTraj][33]=Linx[0]; //nx
                                                          traj[numInTraj][34]=Liny[0]; //ny
                                                          traj[numInTraj][35]=Linz[0]; //nz
                                                          traj[numInTraj][36]=Linx[1]; //dn_x/dx
                                                          traj[numInTraj][37]=Linx[2]; //dn_x/dy
                                                          traj[numInTraj][38]=Linx[3]; //dn_x/dz
                                                          traj[numInTraj][39]=Liny[1]; //dn_y/dx
                                                          traj[numInTraj][40]=Liny[2]; //dn_y/dy
                                                          traj[numInTraj][41]=Liny[3]; //dn_y/dz
                                                          traj[numInTraj][42]=Linz[1]; //dn_z/dx
                                                          traj[numInTraj][43]=Linz[2]; //dn_z/dy
                                                          traj[numInTraj][44]=Linz[3]; //dn_z/dz
                                                          traj[numInTraj][45]=Lik[1]; //dk/dx
                                                          traj[numInTraj][46]=Lik[2]; //dk/dy
                                                          traj[numInTraj][47]=Lik[3]; //dk/dz
                                                          //weighting according curvature check
                                                          cnx=Linx[0]*Linx[1]+Liny[0]*Linx[2]+Linz[0]*Linx[3];
                                                          cny=Linx[0]*Liny[1]+Liny[0]*Liny[2]+Linz[0]*Liny[3];
                                                          cnz=Linx[0]*Linz[1]+Liny[0]*Linz[2]+Linz[0]*Linz[3];
                                                          curvGrad=pow(cnx*cnx+cny*cny+cnz*cnz,0.5);
                                                          vel=pow( pow(Liu[0],2.)
                                                                  +pow(Liv[0],2.)
                                                                  +pow(Liw[0],2.),0.5);
                                                          curv= pow( pow(Liv[0]*Liaz[0]-Liw[0]*Liay[0],2.)
                                                                    +pow(Liw[0]*Liax[0]-Liu[0]*Liaz[0],2.)
                                                                    +pow(Liu[0]*Liay[0]-Liv[0]*Liax[0],2.)
                                                                    ,0.5)
                                                               /(vel*vel*vel);
                                                          /*if(fabs(curvGrad)+fabs(curv)>0){
                                                             curvCriteria=fabs(curvGrad-curv)/(fabs(curvGrad)+fabs(curv));
                                                          }
                                                          else{
                                                             curvCriteria=0.95;
                                                          }*/
                                                          if(fabs(curvGrad)<fabs(curv)){
                                                             curvCriteria=1.-fabs(curvGrad)/fabs(curv);
                                                          }
                                                          else{
                                                             curvCriteria=1.-fabs(curv)/fabs(curvGrad);
                                                          }
                                                          //weighting according to grad k check
                                                          acx=Liu[0]*Liu[1]+Liv[0]*Liu[2]+Liw[0]*Liu[3];
                                                          acy=Liu[0]*Liv[1]+Liv[0]*Liv[2]+Liw[0]*Liv[3];
                                                          acz=Liu[0]*Liw[1]+Liv[0]*Liw[2]+Liw[0]*Liw[3];
                                                          w1=Liw[2]-Liv[3];
                                                          w2=Liu[3]-Liw[1];
                                                          w3=Liv[1]-Liu[2];
                                                          Lx=w2*Liw[0]-w3*Liv[0];
                                                          Ly=w3*Liu[0]-w1*Liw[0];
                                                          Lz=w1*Liv[0]-w2*Liu[0];
                                                          dix=acx-Lx-Lik[1];
                                                          diy=acy-Ly-Lik[2];
                                                          diz=acz-Lz-Lik[3];
                                                          refx=fabs(acx)+fabs(Lx)+fabs(Lik[1]);
                                                          refy=fabs(acy)+fabs(Ly)+fabs(Lik[2]);
                                                          refz=fabs(acz)+fabs(Lz)+fabs(Lik[3]);
                                                          if(refx>0 && refy>0 && refz>0){
                                                             gradCriteria=(1./3.)*(fabs(dix)/refx+fabs(diy)/refy+fabs(diz)/refz); //20.*absdi;//
                                                          }
                                                          else{
                                                             gradCriteria=0.95;
                                                          }
                                                          traj[numInTraj][30]= weightDivU*divCriteria
                                                                              +weightAcc*accCriteria
                                                                              +weightDivA*divaCriteria
                                                                              +weightCurv*curvCriteria
                                                                              +weightGradK*gradCriteria;
                                                      }
                                                      ///end curvature and grad kinetic energy stuff
                                                   }
                                                }
                                             }
                                          }
                                       }
                                    }
                                 }
                              }
                           }
                        }
                     }
                  }
               }// end of if pCOunter>3 solve...
               if(!(pCounterB>minCounter ) || !(contin)){
                  traj[numInTraj][ 0]=point[time][n][2];
                  traj[numInTraj][ 1]=point[time][n][3];
                  traj[numInTraj][ 2]=point[time][n][4];
                  traj[numInTraj][ 3]=point[time][n][5];
                  traj[numInTraj][ 4]=point[time][n][6];
                  traj[numInTraj][ 5]=point[time][n][7];
                  traj[numInTraj][ 6]=0;
                  traj[numInTraj][ 7]=0;
                  traj[numInTraj][ 8]=0;
                  traj[numInTraj][ 9]=0.;
                  traj[numInTraj][10]=0.;
                  traj[numInTraj][11]=0.;
                  traj[numInTraj][12]=0.;
                  traj[numInTraj][13]=0.;
                  traj[numInTraj][14]=0.;
                  traj[numInTraj][15]=0.;
                  traj[numInTraj][16]=0.;
                  traj[numInTraj][17]=0.;
                  traj[numInTraj][18]=point[time][n][8];
                  traj[numInTraj][19]=point[time][n][9];
                  traj[numInTraj][20]=point[time][n][10];
                  traj[numInTraj][21]=0.;
                  traj[numInTraj][22]=0.;
                  traj[numInTraj][23]=0.;
                  traj[numInTraj][24]=0;
                  traj[numInTraj][25]=0;
                  traj[numInTraj][26]=0;
                  traj[numInTraj][27]=0;
                  traj[numInTraj][28]=0;
                  traj[numInTraj][29]=0;

                  traj[numInTraj][30]=1.;   //Wichtig
                  traj[numInTraj][31]=(double)n;

                  ///curvature and grad kinetic energy stuff
                  if(write && mainForm->interpolRadioGroup->ItemIndex==1){
                     ///traj[numInTraj][33..47]
                     traj[numInTraj][33]=0.;
                     traj[numInTraj][34]=0.;
                     traj[numInTraj][35]=0.;
                     traj[numInTraj][36]=0.;
                     traj[numInTraj][37]=0.;
                     traj[numInTraj][38]=0.;
                     traj[numInTraj][39]=0.;
                     traj[numInTraj][40]=0.;
                     traj[numInTraj][41]=0.;
                     traj[numInTraj][42]=0.;
                     traj[numInTraj][43]=0.;
                     traj[numInTraj][44]=0;
                     traj[numInTraj][45]=0;
                     traj[numInTraj][46]=0;
                     traj[numInTraj][47]=0;
                  }
                  ///end urvature and grad kinetic energy stuff
                  noDeriv++;
               }
               numInTraj++;

               //schauen ob's einen nächsten gibt
               if(point[time][n][1]>0 && time<lastFile-firstFile+1){
                   n=point[time][n][1];
                   time++;
                   if( point[time][n][11]<1. ){  // && !(occ[t+time-2][n]) ){
                   //if( point[time][n][11]<1. || (occ[time][n]) ){
                       ok=false;
                   }
               }
               else{
                  ok=false;
               }
            }//end while ok
            //numInTraj++;
            count3++;
            succIntTraj=(succIntTraj*(double)(count3-1)+(1-(double)noDeriv/(double)numInTraj)*100.)/(double)count3;
            if(numInTraj-noDeriv>minTrajLength-1){   //Wichtig
               /////polynom business////////////////////////////////////////
               double su=0.;
               double x0[300],x1[300],x2[300],x3[300],x4[300],x5[300],x6[300];
               double x7[300],x8[300],x9[300],x10[300],x11[300];
               double x12[300],x13[300],x14[300];
               double x15[300],x16[300],x17[300];
               double x18[300],x19[300],x20[300];
               double x21[300],x22[300],x23[300];
               double x24[300],x25[300],x26[300];
               double x27[300],x28[300],x29[300];
               double x30[300],x31[300],x32[300];
               double x33[300],x34[300],x35[300];
               double x36[300],x37[300],x38[300];
               double x39[300],x40[300],x41[300];
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
               double nxp[300],nyp[300],nzp[300];
               double dnxdxp[300],dnxdyp[300],dnxdzp[300];
               double dnydxp[300],dnydyp[300],dnydzp[300];
               double dnzdxp[300],dnzdyp[300],dnzdzp[300];
               double dkdxp[300],dkdyp[300],dkdzp[300];
               double div,ref;
               double vectorSq;

               double fP;
               if(trajForm->polyCheck->Checked){
                  fP=1;
               }
               else{
                  fP=0;
               }

               setAllMatrixesToZero(4);

               for(int ii=0;ii<numInTraj;ii++){
                  su=su+1-traj[ii][30];//success=0, bad=1;
               }
               //int orderA=(int)(su/35.+3.5);
               int orderA=(int)(su/polyConst+3.5);

               count6++;
               forMeanNumInTraj=forMeanNumInTraj+numInTraj;
               forEffectiveMeanNumInTraj=forEffectiveMeanNumInTraj+su;
               forTypicalPolyOrder=forTypicalPolyOrder+numInTraj*orderA;

               if(numInTraj<5){
                  orderA=2;
               }
               if(numInTraj<2){
                  orderA=1;
               }
               trajForm->polyOrderEdit->Text=IntToStr(orderA);
               trajForm->polyOrderEdit->Refresh();

               for(int ii=0;ii<numInTraj;ii++){
                  we[ii]=1.-1./(1.+exp(-c1*(traj[ii][30]-c2)));//reldiv(ii)
               }

               for(int ii=0;ii<numInTraj;ii++){
                  for(int ij=0;ij<orderA;ij++){
                     A[ii][ij]=we[ii]*pow((double)ii*deltaT+0.000000001,(double)(ij));
                  }

                  y3 [ii]=we[ii]*traj[ii][ 6]; //du/dx
                  y4 [ii]=we[ii]*traj[ii][ 7]; //du/dy
                  y5 [ii]=we[ii]*traj[ii][ 8]; //du/dz
                  y6 [ii]=we[ii]*traj[ii][ 9]; //dv/dx
                  y7 [ii]=we[ii]*traj[ii][10]; //dv/dy
                  y8 [ii]=we[ii]*traj[ii][11]; //dv/dz
                  y9 [ii]=we[ii]*traj[ii][12]; //dw/dx
                  y10[ii]=we[ii]*traj[ii][13]; //dw/dy
                  y11[ii]=we[ii]*traj[ii][14]; //dw/dz

                  y12[ii]=we[ii]*traj[ii][15]; //du/dt
                  y13[ii]=we[ii]*traj[ii][16]; //dv/dt
                  y14[ii]=we[ii]*traj[ii][17]; //dw/dt

                  y39[ii]=we[ii]*traj[ii][18]; //ax
                  y40[ii]=we[ii]*traj[ii][19]; //ay
                  y41[ii]=we[ii]*traj[ii][20]; //az

                  y15[ii]=we[ii]*traj[ii][21]; //da_x/dx
                  y16[ii]=we[ii]*traj[ii][22]; //da_x/dy
                  y17[ii]=we[ii]*traj[ii][23]; //da_x/dz
                  y18[ii]=we[ii]*traj[ii][24]; //da_y/dx
                  y19[ii]=we[ii]*traj[ii][25]; //da_y/dy
                  y20[ii]=we[ii]*traj[ii][26]; //da_y/dz
                  y21[ii]=we[ii]*traj[ii][27]; //da_z/dx
                  y22[ii]=we[ii]*traj[ii][28]; //da_z/dy
                  y23[ii]=we[ii]*traj[ii][29]; //da_z/dz

                  ///curvature and grad kinetic energy stuff
                  if(write && mainForm->interpolRadioGroup->ItemIndex==1){
                     ///traj[numInTraj][32..47]
                     y24[ii]=we[ii]*traj[ii][33]; //n_x
                     y25[ii]=we[ii]*traj[ii][34]; //n_y
                     y26[ii]=we[ii]*traj[ii][35]; //n_z
                     y27[ii]=we[ii]*traj[ii][36]; //dn_x/dx
                     y28[ii]=we[ii]*traj[ii][37]; //dn_x/dy
                     y29[ii]=we[ii]*traj[ii][38]; //dn_x/dz
                     y30[ii]=we[ii]*traj[ii][39]; //dn_y/dx
                     y31[ii]=we[ii]*traj[ii][40]; //dn_y/dy
                     y32[ii]=we[ii]*traj[ii][41]; //dn_y/dz
                     y33[ii]=we[ii]*traj[ii][42]; //dn_z/dx
                     y34[ii]=we[ii]*traj[ii][43]; //dn_z/dy
                     y35[ii]=we[ii]*traj[ii][44]; //dn_z/dz

                     y36[ii]=we[ii]*traj[ii][45]; //dk/dx
                     y37[ii]=we[ii]*traj[ii][46]; //dk/dy
                     y38[ii]=we[ii]*traj[ii][47]; //dk/dz
                  }
                  ///end urvature and grad kinetic energy stuff

               }
               
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,7);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x3[ii]=X[ii];//du/dx
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,8);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x4[ii]=X[ii];//du/dy
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,9);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x5[ii]=X[ii];//du/dz
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,10);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x6[ii]=X[ii];//dv/dx
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,11);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x7[ii]=X[ii];//dv/dy
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,12);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x8[ii]=X[ii];//dv/dz
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,13);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x9[ii]=X[ii];//dw/dx
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,14);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x10[ii]=X[ii];//dw/dy
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,15);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x11[ii]=X[ii];//dw/dz
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,16);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x12[ii]=X[ii];//du/dt
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,17);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x13[ii]=X[ii];//dv/dt
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,18);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x14[ii]=X[ii];//dw/dt
               }

               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,43);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x39[ii]=X[ii];//a_x
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,44);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x40[ii]=X[ii];//a_y
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,45);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x41[ii]=X[ii];//a_z
               }

               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,19);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x15[ii]=X[ii];//da_x/dx
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,20);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x16[ii]=X[ii];//da_x/dy
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,21);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x17[ii]=X[ii];//da_x/dz
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,22);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x18[ii]=X[ii];//da_y/dx
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,23);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x19[ii]=X[ii];//da_y/dy
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,24);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x20[ii]=X[ii];//da_y/dz
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,25);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x21[ii]=X[ii];//da_z/dx
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,26);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x22[ii]=X[ii];//da_z/dy
               }
               makeAT(numInTraj,orderA);
               makeATA(numInTraj,orderA);
               makeATY(numInTraj,orderA,27);
               solve(numInTraj,orderA);
               for(int ii=0;ii<orderA;ii++){
                  x23[ii]=X[ii];//da_z/dz
               }

               ///curvature and grad kinetic energy stuff
               if(mainForm->interpolRadioGroup->ItemIndex==1){
                  ///ATY( , ,28..42)
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,28);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x24[ii]=X[ii];//n_x
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,29);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x25[ii]=X[ii];//n_y
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,30);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x26[ii]=X[ii];//n_z
                  }

                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,31);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x27[ii]=X[ii];//dn_x/dx
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,32);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x28[ii]=X[ii];//dn_x/dy
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,33);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x29[ii]=X[ii];//dn_x/dz
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,34);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x30[ii]=X[ii];//dn_y/dx
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,35);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x31[ii]=X[ii];//dn_y/dy
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,36);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x32[ii]=X[ii];//dn_y/dz
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,37);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x33[ii]=X[ii];//dn_z/dx
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,38);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x34[ii]=X[ii];//dn_z/dy
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,39);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x35[ii]=X[ii];//dn_z/dz
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,40);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x36[ii]=X[ii];//dk/dx
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,41);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x37[ii]=X[ii];//dk/dy
                  }
                  makeAT(numInTraj,orderA);
                  makeATA(numInTraj,orderA);
                  makeATY(numInTraj,orderA,42);
                  solve(numInTraj,orderA);
                  for(int ii=0;ii<orderA;ii++){
                     x38[ii]=X[ii];//dk/dz
                  }
               }
               ///end curvature and grad kinetic energy stuff


               for(int ii=0;ii<numInTraj;ii++){
               
                  up[ii]=0;
                  vp[ii]=0;
                  wp[ii]=0;
                  dudxp[ii]=0;
                  dudyp[ii]=0;
                  dudzp[ii]=0;
                  dvdxp[ii]=0;
                  dvdyp[ii]=0;
                  dvdzp[ii]=0;
                  dwdxp[ii]=0;
                  dwdyp[ii]=0;
                  dwdzp[ii]=0;
                  dudtp[ii]=0;
                  dvdtp[ii]=0;
                  dwdtp[ii]=0;
                  axp[ii]=0;
                  ayp[ii]=0;
                  azp[ii]=0;
                  daxdxp[ii]=0;
                  daxdyp[ii]=0;
                  daxdzp[ii]=0;
                  daydxp[ii]=0;
                  daydyp[ii]=0;
                  daydzp[ii]=0;
                  dazdxp[ii]=0;
                  dazdyp[ii]=0;
                  dazdzp[ii]=0;

                  ///curvature and grad kinetic energy stuff
                  if(write && mainForm->interpolRadioGroup->ItemIndex==1){
                     nxp[ii]=0;
                     nyp[ii]=0;
                     nzp[ii]=0;
                     dnxdxp[ii]=0;
                     dnxdyp[ii]=0;
                     dnxdzp[ii]=0;
                     dnydxp[ii]=0;
                     dnydyp[ii]=0;
                     dnydzp[ii]=0;
                     dnzdxp[ii]=0;
                     dnzdyp[ii]=0;
                     dnzdzp[ii]=0;
                     dkdxp[ii]=0;
                     dkdyp[ii]=0;
                     dkdzp[ii]=0;
                  }
                  ///end urvature and grad kinetic energy stuff

                  for(int ij=0;ij<orderA;ij++){
                     dudxp[ii]=dudxp[ii]+ x3[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dudyp[ii]=dudyp[ii]+ x4[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dudzp[ii]=dudzp[ii]+ x5[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dvdxp[ii]=dvdxp[ii]+ x6[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dvdyp[ii]=dvdyp[ii]+ x7[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dvdzp[ii]=dvdzp[ii]+ x8[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dwdxp[ii]=dwdxp[ii]+ x9[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dwdyp[ii]=dwdyp[ii]+x10[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dwdzp[ii]=dwdzp[ii]+x11[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));

                     dudtp[ii]=dudtp[ii]+x12[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dvdtp[ii]=dvdtp[ii]+x13[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dwdtp[ii]=dwdtp[ii]+x14[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));

                     axp[ii]=axp[ii]+x39[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     ayp[ii]=ayp[ii]+x40[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     azp[ii]=azp[ii]+x41[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));

                     daxdxp[ii]=daxdxp[ii]+x15[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     daxdyp[ii]=daxdyp[ii]+x16[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     daxdzp[ii]=daxdzp[ii]+x17[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     daydxp[ii]=daydxp[ii]+x18[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     daydyp[ii]=daydyp[ii]+x19[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     daydzp[ii]=daydzp[ii]+x20[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dazdxp[ii]=dazdxp[ii]+x21[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dazdyp[ii]=dazdyp[ii]+x22[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     dazdzp[ii]=dazdzp[ii]+x23[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));

                     ///curvature and grad kinetic energy stuff
                     if(write && mainForm->interpolRadioGroup->ItemIndex==1){
                        nxp[ii]=nxp[ii]+x24[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        nyp[ii]=nyp[ii]+x25[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        nzp[ii]=nzp[ii]+x26[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        dnxdxp[ii]=dnxdxp[ii]+x27[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        dnxdyp[ii]=dnxdyp[ii]+x28[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        dnxdzp[ii]=dnxdzp[ii]+x29[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        dnydxp[ii]=dnydxp[ii]+x30[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        dnydyp[ii]=dnydyp[ii]+x31[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        dnydzp[ii]=dnydzp[ii]+x32[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        dnzdxp[ii]=dnzdxp[ii]+x33[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        dnzdyp[ii]=dnzdyp[ii]+x34[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        dnzdzp[ii]=dnzdzp[ii]+x35[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));

                        dkdxp[ii]=dkdxp[ii]+x36[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        dkdyp[ii]=dkdyp[ii]+x37[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                        dkdzp[ii]=dkdzp[ii]+x38[ij]*pow((double)ii*deltaT+0.00001,(double)(ij));
                     }
                     ///end urvature and grad kinetic energy stuff
                  }
                  
                  double fR=1.-fP;
                  xp [ii]=traj[ii][ 0];
                  yp [ii]=traj[ii][ 1];
                  zp [ii]=traj[ii][ 2];
                  up [ii]=traj[ii][ 3];
                  vp [ii]=traj[ii][ 4];
                  wp [ii]=traj[ii][ 5];
                  /*if(ii>0 && ii<numInTraj-1){
                     axp[ii]=30.*(traj[ii+1][ 3]-traj[ii-1][ 3]);
                     ayp[ii]=30.*(traj[ii+1][ 4]-traj[ii-1][ 4]);
                     azp[ii]=30.*(traj[ii+1][ 5]-traj[ii-1][ 5]);
                  }
                  else{
                     axp[ii]=traj[ii][18];
                     ayp[ii]=traj[ii][19];
                     azp[ii]=traj[ii][20];
                  }*/


                  dudxp[ii]=fP*dudxp[ii]+fR*traj[ii][ 6];
                  dudyp[ii]=fP*dudyp[ii]+fR*traj[ii][ 7];
                  dudzp[ii]=fP*dudzp[ii]+fR*traj[ii][ 8];
                  dvdxp[ii]=fP*dvdxp[ii]+fR*traj[ii][ 9];
                  dvdyp[ii]=fP*dvdyp[ii]+fR*traj[ii][10];
                  dvdzp[ii]=fP*dvdzp[ii]+fR*traj[ii][11];
                  dwdxp[ii]=fP*dwdxp[ii]+fR*traj[ii][12];
                  dwdyp[ii]=fP*dwdyp[ii]+fR*traj[ii][13];
                  dwdzp[ii]=fP*dwdzp[ii]+fR*traj[ii][14];

                  dudtp[ii]=fP*dudtp[ii]+fR*traj[ii][15];
                  dvdtp[ii]=fP*dvdtp[ii]+fR*traj[ii][16];
                  dwdtp[ii]=fP*dwdtp[ii]+fR*traj[ii][17];

                  axp[ii]=fP*axp[ii]+fR*traj[ii][18];
                  ayp[ii]=fP*ayp[ii]+fR*traj[ii][19];
                  azp[ii]=fP*azp[ii]+fR*traj[ii][20];

                  daxdxp[ii]=fP*daxdxp[ii]+fR*traj[ii][21];
                  daxdyp[ii]=fP*daxdyp[ii]+fR*traj[ii][22];
                  daxdzp[ii]=fP*daxdzp[ii]+fR*traj[ii][23];
                  daydxp[ii]=fP*daydxp[ii]+fR*traj[ii][24];
                  daydyp[ii]=fP*daydyp[ii]+fR*traj[ii][25];
                  daydzp[ii]=fP*daydzp[ii]+fR*traj[ii][26];
                  dazdxp[ii]=fP*dazdxp[ii]+fR*traj[ii][27];
                  dazdyp[ii]=fP*dazdyp[ii]+fR*traj[ii][28];
                  dazdzp[ii]=fP*dazdzp[ii]+fR*traj[ii][29];

                  ///curvature and grad kinetic energy stuff
                  if(mainForm->interpolRadioGroup->ItemIndex==1){
                     /*nxp[ii]=fP*nxp[ii]+fR*traj[ii][33];
                     nyp[ii]=fP*nyp[ii]+fR*traj[ii][34];
                     nzp[ii]=fP*nzp[ii]+fR*traj[ii][35];*/
                     nxp[ii]=traj[ii][33];
                     nyp[ii]=traj[ii][34];
                     nzp[ii]=traj[ii][35];

                     dnxdxp[ii]=fP*dnxdxp[ii]+fR*traj[ii][36];
                     dnxdyp[ii]=fP*dnxdyp[ii]+fR*traj[ii][37];
                     dnxdzp[ii]=fP*dnxdzp[ii]+fR*traj[ii][38];
                     dnydxp[ii]=fP*dnydxp[ii]+fR*traj[ii][39];
                     dnydyp[ii]=fP*dnydyp[ii]+fR*traj[ii][40];
                     dnydzp[ii]=fP*dnydzp[ii]+fR*traj[ii][41];
                     dnzdxp[ii]=fP*dnzdxp[ii]+fR*traj[ii][42];
                     dnzdyp[ii]=fP*dnzdyp[ii]+fR*traj[ii][43];
                     dnzdzp[ii]=fP*dnzdzp[ii]+fR*traj[ii][44];

                     dkdxp[ii]=fP*dkdxp[ii]+fR*traj[ii][45];
                     dkdyp[ii]=fP*dkdyp[ii]+fR*traj[ii][46];
                     dkdzp[ii]=fP*dkdzp[ii]+fR*traj[ii][47];
                  }
                  ///end urvature and grad kinetic energy stuff

               }// end for loop through traj
               if(write){
                  for(int ii=0;ii<numInTraj;ii++){

                     fprintf(fpp, "%lf\t", xp[ii]);//1
                     fprintf(fpp, "%lf\t", yp[ii]);//2
                     fprintf(fpp, "%lf\t", zp[ii]);//3
                     fprintf(fpp, "%lf\t", up[ii]);//4
                     fprintf(fpp, "%lf\t", vp[ii]);//5
                     fprintf(fpp, "%lf\t", wp[ii]);//6
                     fprintf(fpp, "%lf\t", dudxp[ii]);//7
                     fprintf(fpp, "%lf\t", dudyp[ii]);//8
                     fprintf(fpp, "%lf\t", dudzp[ii]);//9
                     fprintf(fpp, "%lf\t", dvdxp[ii]);//10
                     fprintf(fpp, "%lf\t", dvdyp[ii]);//11
                     fprintf(fpp, "%lf\t", dvdzp[ii]);//12
                     fprintf(fpp, "%lf\t", dwdxp[ii]);//13
                     fprintf(fpp, "%lf\t", dwdyp[ii]);//14
                     fprintf(fpp, "%lf\t", dwdzp[ii]);//15
                     fprintf(fpp, "%lf\t", dudtp[ii]);//16
                     fprintf(fpp, "%lf\t", dvdtp[ii]);//17
                     fprintf(fpp, "%lf\t", dwdtp[ii]);//18

                     fprintf(fpp, "%lf\t", axp[ii]);//19
                     fprintf(fpp, "%lf\t", ayp[ii]);//20
                     fprintf(fpp, "%lf\t", azp[ii]);//21

                     fprintf(fpp, "%lf\t", daxdxp[ii]);//22
                     fprintf(fpp, "%lf\t", daxdyp[ii]);//23
                     fprintf(fpp, "%lf\t", daxdzp[ii]);//24
                     fprintf(fpp, "%lf\t", daydxp[ii]);//25
                     fprintf(fpp, "%lf\t", daydyp[ii]);//26
                     fprintf(fpp, "%lf\t", daydzp[ii]);//27
                     fprintf(fpp, "%lf\t", dazdxp[ii]);//28
                     fprintf(fpp, "%lf\t", dazdyp[ii]);//29
                     fprintf(fpp, "%lf\t", dazdzp[ii]);//30

                     fprintf(fpp, "%lf\t", (double)startP);//31
                     ///curvature and grad kinetic energy stuff
                     if(mainForm->interpolRadioGroup->ItemIndex==0){
                        fprintf(fpp, "%lf\n", (double)(ii));//32

                     }
                     else{
                        fprintf(fpp, "%lf\t", (double)(ii));//32
                        fprintf(fpp, "%lf\t", nxp[ii]);//33
                        fprintf(fpp, "%lf\t", nyp[ii]);//34
                        fprintf(fpp, "%lf\t", nzp[ii]);//35
                        fprintf(fpp, "%lf\t", dnxdxp[ii]);//36
                        fprintf(fpp, "%lf\t", dnxdyp[ii]);//37
                        fprintf(fpp, "%lf\t", dnxdzp[ii]);//38
                        fprintf(fpp, "%lf\t", dnydxp[ii]);//39
                        fprintf(fpp, "%lf\t", dnydyp[ii]);//40
                        fprintf(fpp, "%lf\t", dnydzp[ii]);//41
                        fprintf(fpp, "%lf\t", dnzdxp[ii]);//42
                        fprintf(fpp, "%lf\t", dnzdyp[ii]);//43
                        fprintf(fpp, "%lf\t", dnzdzp[ii]);//44
                        fprintf(fpp, "%lf\t", dkdxp[ii]);//45
                        fprintf(fpp, "%lf\t", dkdyp[ii]);//46
                        fprintf(fpp, "%lf\n", dkdzp[ii]);//47
                     }
                     ///end urvature and grad kinetic energy stuff


                     //%omega,strain,div,ref
                     w1=dwdyp[ii]-dvdzp[ii];
                     w2=dudzp[ii]-dwdxp[ii];
                     w3=dvdxp[ii]-dudyp[ii];
                     s11=dudxp[ii];
                     s22=dvdyp[ii];
                     s33=dwdzp[ii];
                     s12=0.5*(dudyp[ii]+dvdxp[ii]);
                     s13=0.5*(dudzp[ii]+dwdxp[ii]);
                     s23=0.5*(dvdzp[ii]+dwdyp[ii]);
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
                     //here we make also a weighting according to diva=4Q
                     wsq=w1*w1+w2*w2+w3*w3;
                     twosijsij=2.*(s11*s11+s22*s22+s33*s33
                                   +2.*(s12*s12+s13*s13+s23*s23)
                                   );
                     Q=(1./4.)*(wsq-twosijsij);
                     diva=daxdxp[ii]+daydyp[ii]+dazdzp[ii];
                     if(fabs(diva)+fabs(4*Q)>0){
                        divaCriteria=fabs(diva+4*Q)/(fabs(diva)+fabs(4*Q));
                     }
                     else{
                        divaCriteria=0.95;
                     }
                     ///curvature and grad kinetic energy stuff
                     if(mainForm->interpolRadioGroup->ItemIndex==1){
                        //weighting according curvature check
                        cnx=nxp[ii]*dnxdxp[ii]+nyp[ii]*dnxdyp[ii]+nzp[ii]*dnxdzp[ii];
                        cny=nxp[ii]*dnydxp[ii]+nyp[ii]*dnydyp[ii]+nzp[ii]*dnydzp[ii];
                        cnz=nxp[ii]*dnzdxp[ii]+nyp[ii]*dnzdyp[ii]+nzp[ii]*dnzdzp[ii];
                        curvGrad=pow(cnx*cnx+cny*cny+cnz*cnz,0.5);
                        vel=pow( pow(up[ii],2.)
                                +pow(vp[ii],2.)
                                +pow(wp[ii],2.),0.5);
                        if(vel>0){
                           vectorSq= pow(vp[ii]*azp[ii]-wp[ii]*ayp[ii],2.)
                                    +pow(wp[ii]*axp[ii]-up[ii]*azp[ii],2.)
                                    +pow(up[ii]*ayp[ii]-vp[ii]*axp[ii],2.);
                           if(vectorSq>0){
                              curv= pow(vectorSq,0.5)
                                   /(vel*vel*vel);
                           }
                           else{
                              curv=0.;
                              curvGrad=0.;
                           }
                        }
                        else{
                           curv=0.;
                           curvGrad=0.;
                        }
                        /*if(fabs(curvGrad)+fabs(curv)>0){
                           curvCriteria=fabs(curvGrad-curv)/(fabs(curvGrad)+fabs(curv));
                        }
                        else{
                           curvCriteria=0.95;
                        }*/
                        if(fabs(curvGrad)<fabs(curv)){
                           curvCriteria=1.-fabs(curvGrad)/fabs(curv);
                        }
                        else{
                           curvCriteria=1.-fabs(curv)/fabs(curvGrad);
                        }
                        //weighting according to grad k check
                        acx=up[ii]*dudxp[ii]+vp[ii]*dudyp[ii]+wp[ii]*dudzp[ii];
                        acy=up[ii]*dvdxp[ii]+vp[ii]*dvdyp[ii]+wp[ii]*dvdzp[ii];
                        acz=up[ii]*dwdxp[ii]+vp[ii]*dwdyp[ii]+wp[ii]*dwdzp[ii];

                        w1=dwdyp[ii]-dvdzp[ii];
                        w2=dudzp[ii]-dwdxp[ii];
                        w3=dvdxp[ii]-dudyp[ii];

                        Lx=w2*wp[ii]-w3*vp[ii];
                        Ly=w3*up[ii]-w1*wp[ii];
                        Lz=w1*vp[ii]-w2*up[ii];
                        dix=acx-Lx-dkdxp[ii];
                        diy=acy-Ly-dkdyp[ii];
                        diz=acz-Lz-dkdzp[ii];
                        refx=fabs(acx)+fabs(Lx)+fabs(dkdxp[ii]);
                        refy=fabs(acy)+fabs(Ly)+fabs(dkdyp[ii]);
                        refz=fabs(acz)+fabs(Lz)+fabs(dkdzp[ii]);
                        if(refx>0 && refy>0 && refz>0){
                           gradCriteria=(1./3.)*(fabs(dix)/refx+fabs(diy)/refy+fabs(diz)/refz); //20.*absdi;//
                        }
                        else{
                           gradCriteria=0.95;
                        }

                     }
                     ///end urvature and grad kinetic energy stuff

                     if(divCriteria<0.1){
                        count2++;
                        meanDiss=meanDiss+viscosity*twosijsij;
                        meanUisq=meanUisq+(1./1.)*( up[ii]*up[ii]
                                                   +vp[ii]*vp[ii]
                                                   +wp[ii]*wp[ii]);
                        meanDudxsq=meanDudxsq+(1./1.)*( dudxp[ii]*dudxp[ii]
                                                       +dvdyp[ii]*dvdyp[ii]
                                                       +dwdzp[ii]*dwdzp[ii]);
                     }
                     if(accCriteria<0.1){
                        count4++;
                     }
                     if(divaCriteria<0.1){
                        count5++;
                     }
                     ///curvature and grad kinetic energy stuff
                     if(mainForm->interpolRadioGroup->ItemIndex==1){
                        if(curvCriteria<0.1){
                           count7++;
                        }
                        if(gradCriteria<0.1){
                           count8++;
                        }
                     }
                     ///end urvature and grad kinetic energy stuff
                  }// end for
                  ////end of polynom business
                  mainForm->meanPointsInSphereBEdit->Text=IntToStr((int)(pointList->meanPointsInSphereB+0.5));
                  mainForm->meanPointsInSphereEdit->Text =IntToStr((int)(pointList->meanPointsInSphere+0.5));
                  mainForm->succIntTrajEdit->Text=IntToStr((int)(pointList->succIntTraj+0.5));
                  if(count2>0){
                     mainForm->meanDissEdit->Text=IntToStr((int)(1.e6*pointList->meanDiss/(double)count2+0.5));
                     int Reynolds=(int)((pow(meanUisq/(double)count2,0.5)*pow(meanUisq/meanDudxsq,0.5))/viscosity+0.5);
                     mainForm->reEdit->Text=IntToStr(Reynolds);
                     mainForm->meanSuccessDivEdit->Text=IntToStr((int)(100.*(double)pointList->count2/(double)pointList->count+0.5));
                     mainForm->meanSuccessAccEdit->Text=IntToStr((int)(100.*(double)pointList->count4/(double)pointList->count+0.5));
                     mainForm->meanSuccessDivAEdit->Text=IntToStr((int)(100.*(double)pointList->count5/(double)pointList->count+0.5));
                     ///curvature and grad kinetic energy stuff
                     if(mainForm->interpolRadioGroup->ItemIndex==1){
                        mainForm->meanSuccessCurvEdit->Text=IntToStr((int)(100.*(double)pointList->count7/(double)pointList->count+0.5));
                        mainForm->meanSuccessGradKEdit->Text=IntToStr((int)(100.*(double)pointList->count8/(double)pointList->count+0.5));
                     }
                     ///end urvature and grad kinetic energy stuff
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
                  
                  

                  FILE *fppD;
                  AnsiString name;
                  name="trajDeriv.res";
                  const char *filename;
                  filename=name.c_str();
                  fppD = fopen(filename,"w");
                  for(int ii=0;ii<numInTraj;ii++){
                     //%omega,strain,div,ref
                     w1=dwdyp[ii]-dvdzp[ii];
                     w2=dudzp[ii]-dwdxp[ii];
                     w3=dvdxp[ii]-dudyp[ii];
                     s11=dudxp[ii];
                     s22=dvdyp[ii];
                     s33=dwdzp[ii];
                     s12=0.5*(dudyp[ii]+dvdxp[ii]);
                     s13=0.5*(dudzp[ii]+dwdxp[ii]);
                     s23=0.5*(dvdzp[ii]+dwdyp[ii]);

                     trajForm->Series1->AddXY((double)ii,axp[ii],'.',clTeeColor);
                     trajForm->Series2->AddXY((double)ii,ayp[ii],'.',clTeeColor);
                     trajForm->Series3->AddXY((double)ii,azp[ii],'.',clTeeColor);


                     trajForm->Series4->AddXY((double)ii,dudxp[ii],'.',clTeeColor);
                     trajForm->Series5->AddXY((double)ii,dvdyp[ii],'.',clTeeColor);
                     trajForm->Series6->AddXY((double)ii,dwdzp[ii],'.',clTeeColor);

                     //store in trajecDeriv.res

                     fprintf(fppD, "%lf\t", dudxp[ii]);
                     fprintf(fppD, "%lf\t", dvdyp[ii]);
                     fprintf(fppD, "%lf\n", dwdzp[ii]);

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
                  fclose (fppD);
                  trajForm->Refresh();
               } //end else
            } //end if of polynom buisness
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
   int filtSize=(int)(StrToFloat(mainForm->filtEdit->Text)/2);
   int numFields;
   if(mainForm->interpolRadioGroup->ItemIndex==0){
      numFields=32;
   }
   else{
      numFields=47;
   }
   double viscosity=StrToFloat(mainForm->viscEdit->Text);
   double polyConst=StrToFloat(mainForm->polyConstEdit->Text);
   deltaT=StrToFloat(mainForm->deltaTEdit->Text);
   c1=StrToFloat(mainForm->c1Edit->Text);
   c2=StrToFloat(mainForm->c2Edit->Text);
   double weightDivU=StrToFloat(mainForm->divEdit->Text);
   double weightAcc=StrToFloat(mainForm->accEdit->Text);
   double weightDivA=StrToFloat(mainForm->divaEdit->Text);
   double weightCurv=StrToFloat(mainForm->curvEdit->Text);
   double weightGradK=StrToFloat(mainForm->gradKEdit->Text);
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
                  gridMem[k][i][30]= weightDivU*divCriteria
                                    +weightAcc*accCriteria
                                    +weightDivA*divaCriteria;
                                    //+weightCurv*curvCriteria
                                    //+weightGradK*gradCriteria;
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
void __fastcall TpointList::followTrajPointLinQuadforAccDerivGrid(FILE *fpp, int t,int startPoint, bool write)
{
     int pCounterB, pCounter, rowIndex;
     int numInTraj=0;
     int startT, startP,ind;
     double dist,dx,dy,dz,dt;
     double Liu[5],Liv[5],Liw[5],Liax[4],Liay[4],Liaz[4];
     double Linx[4],Liny[4],Linz[4],Lik[4];
     double w1,w2,w3,s11,s12,s13,s22,s23,s33,vel,wsq,twosijsij,Q;
     double div,ref,divCriteria;
     double diva,divaCriteria;
     double dix,diy,diz,refx,refy,refz,accCriteria,absdi;
     double acx,acy,acz;
     double cnx,cny,cnz,curvGrad,curv,curvCriteria,Lx,Ly,Lz,gradCriteria;

     int time;
     double minDistB[200];
     int minDistBIndex[200];
     bool contin;

     int rank;

     int start;
     int end;
     double polyConst=StrToFloat(mainForm->polyConstEdit->Text);

     double maxRadiusSpat=StrToFloat(mainForm->radiusSpatEdit->Text);
     double maxRadiusTemp=StrToFloat(mainForm->radiusTempEdit->Text);
     int minCounter;

     bool ok;
     startT=t;

     bool continuePast;
     bool continueFuture;
     int step[5];

     double weightDivU=StrToFloat(mainForm->divEdit->Text);
     double weightAcc=StrToFloat(mainForm->accEdit->Text);
     double weightDivA=StrToFloat(mainForm->divaEdit->Text);
     double weightCurv=StrToFloat(mainForm->curvEdit->Text);
     double weightGradK=StrToFloat(mainForm->gradKEdit->Text);

     double viscosity=StrToFloat(mainForm->viscEdit->Text);
     deltaT=StrToFloat(mainForm->deltaTEdit->Text);
     c1=StrToFloat(mainForm->c1Edit->Text);
     c2=StrToFloat(mainForm->c2Edit->Text);

     minX=0.001*StrToFloat(mainForm->minXEdit->Text);
     minY=0.001*StrToFloat(mainForm->minYEdit->Text);
     minZ=0.001*StrToFloat(mainForm->minZEdit->Text);
     deltaX=0.001*StrToFloat(mainForm->deltaXEdit->Text);
     deltaY=0.001*StrToFloat(mainForm->deltaYEdit->Text);
     deltaZ=0.001*StrToFloat(mainForm->deltaZEdit->Text);
     maxX=minX+deltaX*StrToFloat(mainForm->numXEdit->Text);
     maxY=minY+deltaY*StrToFloat(mainForm->numYEdit->Text);
     maxZ=minZ+deltaZ*StrToFloat(mainForm->numZEdit->Text);


     if(t==pointList->firstFile){
        pointList->readXUAPFile(t,true,false);
     }
     else{
        pointList->readXUAPFile(t,false,false);
     }

     start=1;
     end=point[2][0][0]+1;

     int n;
     for (double centerX=minX;centerX<maxX;centerX=centerX+deltaX){
        for (double centerY=minY;centerY<maxY;centerY=centerY+deltaY){
           for (double centerZ=minZ;centerZ<maxZ;centerZ=centerZ+deltaZ){
              count++;
              if(mainForm->spatialRadioGroup->ItemIndex==0){
                 setAllMatrixesToZero(4);
              }
              if(mainForm->spatialRadioGroup->ItemIndex==1){
                 setAllMatrixesToZero(10);
              }
              if(mainForm->spatialRadioGroup->ItemIndex==2){
                 setAllMatrixesToZero(20);
              }
              for(int i=0;i<arraySize;i++){
                 dis0[i]=0.;
                 disA[i]=0.;
                 disB[i]=0.;
                 disC[i]=0.;
                 disD[i]=0.;
              }
              for(int i=0;i<200+1;i++){
                 minDistB[i]=1000+i;
                 minDistBIndex[i]=1000+i;
              }
              int index=0;
              for(int i=start;i<end;i++){
                  time=2;
                  if(point[2][i][11]>0.){
                     //interpolieren und rausschreiben mit t,n (Zeit und Startpunkt)
                     //%Da soll jetzt duidxj linear interpoliert werden
                     //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                     //%die nächsten Punkte zu Punkt x,y,z, finden

                     //BBBBBBBBBBBBBBB
                     dist=pow(pow(point[time][i][2]-centerX,2.)+pow(point[time][i][3]-centerY,2.)+pow(point[time][i][4]-centerZ,2.),0.5);
                     if(dist<maxRadiusSpat){
                        minDistB[index]=dist;
                        minDistBIndex[index]=i;
                        index++;
                     }
                  }
               }//for i loop
               pCounterB=0;
               pCounter=0;
               int i;
               for(int pointInd=0;pointInd<index;pointInd++){
                  i=minDistBIndex[pointInd];
                  if(point[time][i][11]>0.){
                     dist=pow(pow(point[time][i][2]-centerX,2.)+pow(point[time][i][3]-centerY,2.)+pow(point[time][i][4]-centerZ,2.),0.5);
                     disB[pCounterB]=(int)(dist*1000.+0.5);
                     dx=point[time][i][2]-centerX;
                     dy=point[time][i][3]-centerY;
                     dz=point[time][i][4]-centerZ;
                     B[pCounterB][0]=1.;
                     B[pCounterB][1]=dx;
                     B[pCounterB][2]=dy;
                     B[pCounterB][3]=dz;
                     if(mainForm->spatialRadioGroup->ItemIndex==1){
                        B[pCounterB][4]=dx*dx;
                        B[pCounterB][5]=dy*dy;
                        B[pCounterB][6]=dz*dz;
                        B[pCounterB][7]=dx*dy;
                        B[pCounterB][8]=dx*dz;
                        B[pCounterB][9]=dy*dz;
                     }
                     if(mainForm->spatialRadioGroup->ItemIndex==2){
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
                     YuB[pCounterB]=point[time][i][5];
                     YvB[pCounterB]=point[time][i][6];
                     YwB[pCounterB]=point[time][i][7];
                     y0[pCounterB]=point[time][i][8];
                     y1[pCounterB]=point[time][i][9];
                     y2[pCounterB]=point[time][i][10];
                     ///curvature and grad kinetic energy stuff
                     if(write && mainForm->interpolRadioGroup->ItemIndex==1){
                        vel=pow( pow(point[time][i][5],2.)
                                +pow(point[time][i][6],2.)
                                +pow(point[time][i][7],2.),0.5);
                        y3[pCounterB]=point[time][i][5]/vel;
                        y4[pCounterB]=point[time][i][6]/vel;
                        y5[pCounterB]=point[time][i][7]/vel;
                        y6[pCounterB]=0.5*vel*vel;//kinetic energy
                     }
                     ///end urvature and grad kinetic energy stuff
                     pCounterB++;
                     if(minDistB[pointInd]<maxRadiusTemp){
                     /////////for du/dt likeold scheme!
                     /////////////////////////////////
                     step[0]=0;
                     step[1]=-1;
                     step[2]=-2;
                     step[3]= 1;
                     step[4]= 2;
                     ind=i;
                     continuePast=true;
                     continueFuture=true;
                     for(int j=0;j<5;j++){
                        if((j<3 && continuePast) || (j>2 && continueFuture)){
                           dx=point[time+step[j]][ind][2]-centerX;
                           dy=point[time+step[j]][ind][3]-centerY;
                           dz=point[time+step[j]][ind][4]-centerZ;
                           A[pCounter][0]=1.;
                           A[pCounter][1]=dx;
                           A[pCounter][2]=dy;
                           A[pCounter][3]=dz;
                           A[pCounter][4]=(double)step[j]*deltaT;

                           YuA[pCounter]=point[time+step[j]][ind][5];
                           YvA[pCounter]=point[time+step[j]][ind][6];
                           YwA[pCounter]=point[time+step[j]][ind][7];
                           pCounter++;
                           if(j<2){
                              if(point[time+step[j]][ind][0]>0){
                                 ind=point[time+step[j]][ind][0];
                              }
                              else{
                                  continuePast=false;
                              }
                           }
                           if(j>2){
                              if(point[time+step[j]][ind][1]>0){
                                 ind=point[time+step[j]][ind][1];
                              }
                              else{
                                  continueFuture=false;
                              }
                           }
                        }
                        if(j==2){
                           ind=i;
                           step[2]=0;
                           if(point[time+step[j]][ind][1]>0){
                              ind=point[time+step[j]][ind][1];
                           }
                           else{
                              continueFuture=false;
                           }
                        }
                     }
                     /////////////////////////////////
                     }
                  }
               }


               meanPointsInSphereB=(meanPointsInSphereB*(double)(count-1)+(double)pCounterB)/(double)count;
               meanPointsInSphere =(meanPointsInSphere *(double)(count-1)+(double)pCounter )/(double)count;
               if(mainForm->spatialRadioGroup->ItemIndex==0){
                  minCounter=3;
               }
               if(mainForm->spatialRadioGroup->ItemIndex==1){
                  minCounter=9;
               }
               if(mainForm->spatialRadioGroup->ItemIndex==2){
                  minCounter=19;
               }
               if(pCounterB>minCounter ){ // %jetzt wird endlich Punkt1 interpoliert
                  //%correct x,y,z with center of interpolation!

                  contin=true;
                  if(mainForm->spatialRadioGroup->ItemIndex==0){
                     makeBT(pCounterB,4);
                     makeBTB(pCounterB,4);
                     makeBTY(pCounterB,4,1);
                     contin=solveB(pCounterB,4);
                  }
                  if(mainForm->spatialRadioGroup->ItemIndex==1){
                     makeBT(pCounterB,10);
                     makeBTB(pCounterB,10);
                     makeBTY(pCounterB,10,1);
                     contin=solveB(pCounterB,10);
                  }
                  if(mainForm->spatialRadioGroup->ItemIndex==2){
                     makeBT(pCounterB,20);
                     makeBTB(pCounterB,20);
                     makeBTY(pCounterB,20,1);
                     contin=solveB(pCounterB,20);
                  }
                  if(contin){
                     Liu[0]=X[0];
                     Liu[1]=X[1];
                     Liu[2]=X[2];
                     Liu[3]=X[3];
                     if(pCounter>4){
                        makeAT(pCounter,5);
                        makeATA(pCounter,5);
                        makeATY(pCounter,5,1);
                        solve(pCounter,5);
                        Liu[4]=X[4];
                     }
                     else{
                        contin=false;
                     }
                     if(contin){
                        if(mainForm->spatialRadioGroup->ItemIndex==0){
                           makeBT(pCounterB,4);
                           makeBTB(pCounterB,4);
                           makeBTY(pCounterB,4,2);
                           contin=solveB(pCounterB,4);
                        }
                        if(mainForm->spatialRadioGroup->ItemIndex==1){
                           makeBT(pCounterB,10);
                           makeBTB(pCounterB,10);
                           makeBTY(pCounterB,10,2);
                           contin=solveB(pCounterB,10);
                        }
                        if(mainForm->spatialRadioGroup->ItemIndex==2){
                           makeBT(pCounterB,20);
                           makeBTB(pCounterB,20);
                           makeBTY(pCounterB,20,2);
                           contin=solveB(pCounterB,20);
                        }
                        if(contin){
                           Liv[0]=X[0];
                           Liv[1]=X[1];
                           Liv[2]=X[2];
                           Liv[3]=X[3];
                           if(pCounter>4){
                              makeAT(pCounter,5);
                              makeATA(pCounter,5);
                              makeATY(pCounter,5,2);
                              solve(pCounter,5);
                              Liv[4]=X[4];
                           }
                           else{
                              contin=false;
                           }
                           if(contin){
                              if(mainForm->spatialRadioGroup->ItemIndex==0){
                                 makeBT(pCounterB,4);
                                 makeBTB(pCounterB,4);
                                 makeBTY(pCounterB,4,3);
                                 contin=solveB(pCounterB,4);
                              }
                              if(mainForm->spatialRadioGroup->ItemIndex==1){
                                 makeBT(pCounterB,10);
                                 makeBTB(pCounterB,10);
                                 makeBTY(pCounterB,10,3);
                                 contin=solveB(pCounterB,10);
                              }
                              if(mainForm->spatialRadioGroup->ItemIndex==2){
                                 makeBT(pCounterB,20);
                                 makeBTB(pCounterB,20);
                                 makeBTY(pCounterB,20,3);
                                 contin=solveB(pCounterB,20);
                              }
                              if(contin){
                                 Liw[0]=X[0];
                                 Liw[1]=X[1];
                                 Liw[2]=X[2];
                                 Liw[3]=X[3];
                                 if(pCounter>4){
                                    makeAT(pCounter,5);
                                    makeATA(pCounter,5);
                                    makeATY(pCounter,5,3);
                                    solve(pCounter,5);
                                    Liw[4]=X[4];
                                 }
                                 else{
                                    contin=false;
                                 }
                                 if(contin){
                                    if(mainForm->spatialRadioGroup->ItemIndex==0){
                                       makeBT(pCounterB,4);
                                       makeBTB(pCounterB,4);
                                       makeBTY(pCounterB,4,4);
                                       contin=solveB(pCounterB,4);
                                    }
                                    if(mainForm->spatialRadioGroup->ItemIndex==1){
                                       makeBT(pCounterB,10);
                                       makeBTB(pCounterB,10);
                                       makeBTY(pCounterB,10,4);
                                       contin=solveB(pCounterB,10);
                                    }
                                    if(mainForm->spatialRadioGroup->ItemIndex==2){
                                       makeBT(pCounterB,20);
                                       makeBTB(pCounterB,20);
                                       makeBTY(pCounterB,20,4);
                                       contin=solveB(pCounterB,20);
                                    }
                                    if(contin){
                                       Liax[0]=X[0];
                                       Liax[1]=X[1];
                                       Liax[2]=X[2];
                                       Liax[3]=X[3];
                                       if(contin){
                                          if(mainForm->spatialRadioGroup->ItemIndex==0){
                                             makeBT(pCounterB,4);
                                             makeBTB(pCounterB,4);
                                             makeBTY(pCounterB,4,5);
                                             contin=solveB(pCounterB,4);
                                          }
                                          if(mainForm->spatialRadioGroup->ItemIndex==1){
                                             makeBT(pCounterB,10);
                                             makeBTB(pCounterB,10);
                                             makeBTY(pCounterB,10,5);
                                             contin=solveB(pCounterB,10);
                                          }
                                          if(mainForm->spatialRadioGroup->ItemIndex==2){
                                             makeBT(pCounterB,20);
                                             makeBTB(pCounterB,20);
                                             makeBTY(pCounterB,20,5);
                                             contin=solveB(pCounterB,20);
                                          }
                                          if(contin){
                                             Liay[0]=X[0];
                                             Liay[1]=X[1];
                                             Liay[2]=X[2];
                                             Liay[3]=X[3];
                                             if(contin){
                                                if(mainForm->spatialRadioGroup->ItemIndex==0){
                                                   makeBT(pCounterB,4);
                                                   makeBTB(pCounterB,4);
                                                   makeBTY(pCounterB,4,6);
                                                   contin=solveB(pCounterB,4);
                                                }
                                                if(mainForm->spatialRadioGroup->ItemIndex==1){
                                                   makeBT(pCounterB,10);
                                                   makeBTB(pCounterB,10);
                                                   makeBTY(pCounterB,10,6);
                                                   contin=solveB(pCounterB,10);
                                                }
                                                if(mainForm->spatialRadioGroup->ItemIndex==2){
                                                   makeBT(pCounterB,20);
                                                   makeBTB(pCounterB,20);
                                                   makeBTY(pCounterB,20,6);
                                                   contin=solveB(pCounterB,20);
                                                }
                                                if(contin){
                                                   Liaz[0]=X[0];
                                                   Liaz[1]=X[1];
                                                   Liaz[2]=X[2];
                                                   Liaz[3]=X[3];
                                                   if(contin){
                                                      ///curvature and grad kinetic energy stuff
                                                      if(mainForm->interpolRadioGroup->ItemIndex==1){
                                                         //nx
                                                         if(mainForm->spatialRadioGroup->ItemIndex==0){
                                                            makeBT(pCounterB,4);
                                                            makeBTB(pCounterB,4);
                                                            makeBTY(pCounterB,4,7);
                                                            contin=solveB(pCounterB,4);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==1){
                                                            makeBT(pCounterB,10);
                                                            makeBTB(pCounterB,10);
                                                            makeBTY(pCounterB,10,7);
                                                            contin=solveB(pCounterB,10);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==2){
                                                            makeBT(pCounterB,20);
                                                            makeBTB(pCounterB,20);
                                                            makeBTY(pCounterB,20,7);
                                                            contin=solveB(pCounterB,20);
                                                         }
                                                         vel=pow( pow(Liu[0],2.)
                                                                 +pow(Liv[0],2.)
                                                                 +pow(Liw[0],2.),0.5);
                                                         Linx[0]=Liu[0]/vel;
                                                         Linx[1]=X[1];
                                                         Linx[2]=X[2];
                                                         Linx[3]=X[3];
                                                         //ny
                                                         if(mainForm->spatialRadioGroup->ItemIndex==0){
                                                            makeBT(pCounterB,4);
                                                            makeBTB(pCounterB,4);
                                                            makeBTY(pCounterB,4,8);
                                                            contin=solveB(pCounterB,4);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==1){
                                                            makeBT(pCounterB,10);
                                                            makeBTB(pCounterB,10);
                                                            makeBTY(pCounterB,10,8);
                                                            contin=solveB(pCounterB,10);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==2){
                                                            makeBT(pCounterB,20);
                                                            makeBTB(pCounterB,20);
                                                            makeBTY(pCounterB,20,8);
                                                            contin=solveB(pCounterB,20);
                                                         }

                                                         Liny[0]=Liv[0]/vel;
                                                         Liny[1]=X[1];
                                                         Liny[2]=X[2];
                                                         Liny[3]=X[3];
                                                         //nz
                                                         if(mainForm->spatialRadioGroup->ItemIndex==0){
                                                            makeBT(pCounterB,4);
                                                            makeBTB(pCounterB,4);
                                                            makeBTY(pCounterB,4,9);
                                                            contin=solveB(pCounterB,4);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==1){
                                                            makeBT(pCounterB,10);
                                                            makeBTB(pCounterB,10);
                                                            makeBTY(pCounterB,10,9);
                                                            contin=solveB(pCounterB,10);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==2){
                                                            makeBT(pCounterB,20);
                                                            makeBTB(pCounterB,20);
                                                            makeBTY(pCounterB,20,9);
                                                            contin=solveB(pCounterB,20);
                                                         }

                                                         Linz[0]=Liw[0]/vel;
                                                         Linz[1]=X[1];
                                                         Linz[2]=X[2];
                                                         Linz[3]=X[3];
                                                         //grad k
                                                         if(mainForm->spatialRadioGroup->ItemIndex==0){
                                                            makeBT(pCounterB,4);
                                                            makeBTB(pCounterB,4);
                                                            makeBTY(pCounterB,4,10);
                                                            contin=solveB(pCounterB,4);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==1){
                                                            makeBT(pCounterB,10);
                                                            makeBTB(pCounterB,10);
                                                            makeBTY(pCounterB,10,10);
                                                            contin=solveB(pCounterB,10);
                                                         }
                                                         if(mainForm->spatialRadioGroup->ItemIndex==2){
                                                            makeBT(pCounterB,20);
                                                            makeBTB(pCounterB,20);
                                                            makeBTY(pCounterB,20,10);
                                                            contin=solveB(pCounterB,20);
                                                         }
                                                         
                                                         Lik[0]=0.5*vel*vel;
                                                         Lik[1]=X[1];
                                                         Lik[2]=X[2];
                                                         Lik[3]=X[3];
                                                      }
                                                      ///end urvature and grad kinetic energy stuff
                                                      traj[0][ 0]=centerX;//x
                                                      traj[0][ 1]=centerY;//y
                                                      traj[0][ 2]=centerZ;//z
                                                      traj[0][ 3]=Liu[0]; //u
                                                      traj[0][ 4]=Liv[0]; //v
                                                      traj[0][ 5]=Liw[0]; //w
                                                      traj[0][ 6]=Liu[1]; //du/dx
                                                      traj[0][ 7]=Liu[2]; //du/dy
                                                      traj[0][ 8]=Liu[3]; //du/dz
                                                      traj[0][ 9]=Liv[1]; //dv/dx
                                                      traj[0][10]=Liv[2]; //dv/dy
                                                      traj[0][11]=Liv[3]; //dv/dz
                                                      traj[0][12]=Liw[1]; //dw/dx
                                                      traj[0][13]=Liw[2]; //dw/dy
                                                      traj[0][14]=Liw[3]; //dw/dz
                                                      traj[0][15]=Liu[4]; //du/dt
                                                      traj[0][16]=Liv[4]; //dv/dt
                                                      traj[0][17]=Liw[4]; //dw/dt
                                                      traj[0][18]=Liax[0]; //ax
                                                      traj[0][19]=Liay[0]; //ay
                                                      traj[0][20]=Liaz[0]; //az
                                                      traj[0][21]=Liax[1]; //da_x/dx
                                                      traj[0][22]=Liax[2]; //da_x/dy
                                                      traj[0][23]=Liax[3]; //da_x/dz
                                                      traj[0][24]=Liay[1]; //da_y/dx
                                                      traj[0][25]=Liay[2]; //da_y/dy
                                                      traj[0][26]=Liay[3]; //da_y/dz
                                                      traj[0][27]=Liaz[1]; //da_z/dx
                                                      traj[0][28]=Liaz[2]; //da_z/dy
                                                      traj[0][29]=Liaz[3]; //da_z/dz


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
                                                      dix=Liax[0]-Liu[4]-Liu[0]*Liu[1]-Liv[0]*Liu[2]-Liw[0]*Liu[3];
                                                      diy=Liay[0]-Liv[4]-Liu[0]*Liv[1]-Liv[0]*Liv[2]-Liw[0]*Liv[3];
                                                      diz=Liaz[0]-Liw[4]-Liu[0]*Liw[1]-Liv[0]*Liw[2]-Liw[0]*Liw[3];
                                                      absdi=pow(dix*dix+diy*diy+diz*diz,0.5);
                                                      refx= fabs(Liax[0])
                                                           +fabs(Liu[4])
                                                           +fabs( Liu[0]*Liu[1]
                                                                 +Liv[0]*Liu[2]
                                                                 +Liw[0]*Liu[3]);
                                                      refy= fabs(Liay[0])
                                                           +fabs(Liv[4])
                                                           +fabs( Liu[0]*Liv[1]
                                                                 +Liv[0]*Liv[2]
                                                                 +Liw[0]*Liv[3]);
                                                      refz= fabs(Liaz[0])
                                                           +fabs(Liw[4])
                                                           +fabs( Liu[0]*Liw[1]
                                                                 +Liv[0]*Liw[2]
                                                                 +Liw[0]*Liw[3]);
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
                                                      diva=Liax[1]+Liay[2]+Liaz[3];
                                                      if(fabs(diva)+fabs(4*Q)>0){
                                                         divaCriteria=fabs(diva+4*Q)/(fabs(diva)+fabs(4*Q));
                                                      }
                                                      else{
                                                         divaCriteria=0.95;
                                                      }
                                                      if(mainForm->interpolRadioGroup->ItemIndex==0){
                                                         traj[0][30]= weightDivU*divCriteria
                                                                             +weightAcc*accCriteria
                                                                             +weightDivA*divaCriteria;
                                                      }
                                                      if(traj[0][30]>0.95){
                                                         traj[0][30]=0.95;
                                                      }
                                                      traj[0][31]=divCriteria;
                                                      ///curvature and grad kinetic energy stuff
                                                      if(mainForm->interpolRadioGroup->ItemIndex==1){
                                                          ///traj[0][33..47]
                                                          traj[0][33]=Linx[0]; //nx
                                                          traj[0][34]=Liny[0]; //ny
                                                          traj[0][35]=Linz[0]; //nz
                                                          traj[0][36]=Linx[1]; //dn_x/dx
                                                          traj[0][37]=Linx[2]; //dn_x/dy
                                                          traj[0][38]=Linx[3]; //dn_x/dz
                                                          traj[0][39]=Liny[1]; //dn_y/dx
                                                          traj[0][40]=Liny[2]; //dn_y/dy
                                                          traj[0][41]=Liny[3]; //dn_y/dz
                                                          traj[0][42]=Linz[1]; //dn_z/dx
                                                          traj[0][43]=Linz[2]; //dn_z/dy
                                                          traj[0][44]=Linz[3]; //dn_z/dz
                                                          traj[0][45]=Lik[1]; //dk/dx
                                                          traj[0][46]=Lik[2]; //dk/dy
                                                          traj[0][47]=Lik[3]; //dk/dz
                                                          //weighting according curvature check
                                                          cnx=Linx[0]*Linx[1]+Liny[0]*Linx[2]+Linz[0]*Linx[3];
                                                          cny=Linx[0]*Liny[1]+Liny[0]*Liny[2]+Linz[0]*Liny[3];
                                                          cnz=Linx[0]*Linz[1]+Liny[0]*Linz[2]+Linz[0]*Linz[3];
                                                          curvGrad=pow(cnx*cnx+cny*cny+cnz*cnz,0.5);
                                                          vel=pow( pow(Liu[0],2.)
                                                                  +pow(Liv[0],2.)
                                                                  +pow(Liw[0],2.),0.5);
                                                          curv= pow( pow(Liv[0]*Liaz[0]-Liw[0]*Liay[0],2.)
                                                                    +pow(Liw[0]*Liax[0]-Liu[0]*Liaz[0],2.)
                                                                    +pow(Liu[0]*Liay[0]-Liv[0]*Liax[0],2.)
                                                                    ,0.5)
                                                               /(vel*vel*vel);
                                                          /*if(fabs(curvGrad)+fabs(curv)>0){
                                                             curvCriteria=fabs(curvGrad-curv)/(fabs(curvGrad)+fabs(curv));
                                                          }
                                                          else{
                                                             curvCriteria=0.95;
                                                          }*/
                                                          if(fabs(curvGrad)<fabs(curv)){
                                                             curvCriteria=1.-fabs(curvGrad)/fabs(curv);
                                                          }
                                                          else{
                                                             curvCriteria=1.-fabs(curv)/fabs(curvGrad);
                                                          }
                                                          //weighting according to grad k check
                                                          acx=Liu[0]*Liu[1]+Liv[0]*Liu[2]+Liw[0]*Liu[3];
                                                          acy=Liu[0]*Liv[1]+Liv[0]*Liv[2]+Liw[0]*Liv[3];
                                                          acz=Liu[0]*Liw[1]+Liv[0]*Liw[2]+Liw[0]*Liw[3];
                                                          w1=Liw[2]-Liv[3];
                                                          w2=Liu[3]-Liw[1];
                                                          w3=Liv[1]-Liu[2];
                                                          Lx=w2*Liw[0]-w3*Liv[0];
                                                          Ly=w3*Liu[0]-w1*Liw[0];
                                                          Lz=w1*Liv[0]-w2*Liu[0];
                                                          dix=acx-Lx-Lik[1];
                                                          diy=acy-Ly-Lik[2];
                                                          diz=acz-Lz-Lik[3];
                                                          refx=fabs(acx)+fabs(Lx)+fabs(Lik[1]);
                                                          refy=fabs(acy)+fabs(Ly)+fabs(Lik[2]);
                                                          refz=fabs(acz)+fabs(Lz)+fabs(Lik[3]);
                                                          if(refx>0 && refy>0 && refz>0){
                                                             gradCriteria=(1./3.)*(fabs(dix)/refx+fabs(diy)/refy+fabs(diz)/refz); //20.*absdi;//
                                                          }
                                                          else{
                                                             gradCriteria=0.95;
                                                          }
                                                          traj[0][30]= weightDivU*divCriteria
                                                                              +weightAcc*accCriteria
                                                                              +weightDivA*divaCriteria
                                                                              +weightCurv*curvCriteria
                                                                              +weightGradK*gradCriteria;
                                                      }
                                                      ///end curvature and grad kinetic energy stuff
                                                   }
                                                }
                                             }
                                          }
                                       }
                                    }
                                 }
                              }
                           }
                        }
                     }
                  }
               }// end of if pCOunter>3 solve...
               if(!(pCounterB>minCounter ) || !(contin)){
                  traj[0][ 0]=centerX;
                  traj[0][ 1]=centerY;
                  traj[0][ 2]=centerZ;
                  traj[0][ 3]=0;
                  traj[0][ 4]=0;
                  traj[0][ 5]=0;
                  traj[0][ 6]=0;
                  traj[0][ 7]=0;
                  traj[0][ 8]=0;
                  traj[0][ 9]=0.;
                  traj[0][10]=0.;
                  traj[0][11]=0.;
                  traj[0][12]=0.;
                  traj[0][13]=0.;
                  traj[0][14]=0.;
                  traj[0][15]=0.;
                  traj[0][16]=0.;
                  traj[0][17]=0.;
                  traj[0][18]=0;
                  traj[0][19]=0;
                  traj[0][20]=0;
                  traj[0][21]=0.;
                  traj[0][22]=0.;
                  traj[0][23]=0.;
                  traj[0][24]=0;
                  traj[0][25]=0;
                  traj[0][26]=0;
                  traj[0][27]=0;
                  traj[0][28]=0;
                  traj[0][29]=0;

                  traj[0][30]=1.;   //Wichtig
                  traj[0][31]=(double)pCounterB;

                  ///curvature and grad kinetic energy stuff
                  if(mainForm->interpolRadioGroup->ItemIndex==1){
                     ///traj[0][33..47]
                     traj[0][33]=0.;
                     traj[0][34]=0.;
                     traj[0][35]=0.;
                     traj[0][36]=0.;
                     traj[0][37]=0.;
                     traj[0][38]=0.;
                     traj[0][39]=0.;
                     traj[0][40]=0.;
                     traj[0][41]=0.;
                     traj[0][42]=0.;
                     traj[0][43]=0.;
                     traj[0][44]=0;
                     traj[0][45]=0;
                     traj[0][46]=0;
                     traj[0][47]=0;
                  }
                  ///end urvature and grad kinetic energy stuff
                  divCriteria=1.;
                  accCriteria=1.;
                  divaCriteria=1.;
                  curvCriteria=1.;
                  gradCriteria=1.;
                  twosijsij=0;
               }
               //end of if(pCounterB>minCounter ){


               fprintf(fpp, "%lf\t", traj[0][0]);//1
               fprintf(fpp, "%lf\t", traj[0][1]);//2
               fprintf(fpp, "%lf\t", traj[0][2]);//3
               fprintf(fpp, "%lf\t", traj[0][3]);//4
               fprintf(fpp, "%lf\t", traj[0][4]);//5
               fprintf(fpp, "%lf\t", traj[0][5]);//6
               fprintf(fpp, "%lf\t", traj[0][6]);//7
               fprintf(fpp, "%lf\t", traj[0][7]);//8
               fprintf(fpp, "%lf\t", traj[0][8]);//9
               fprintf(fpp, "%lf\t", traj[0][9]);//10
               fprintf(fpp, "%lf\t", traj[0][10]);//11
               fprintf(fpp, "%lf\t", traj[0][11]);//12
               fprintf(fpp, "%lf\t", traj[0][12]);//13
               fprintf(fpp, "%lf\t", traj[0][13]);//14
               fprintf(fpp, "%lf\t", traj[0][14]);//15
               fprintf(fpp, "%lf\t", traj[0][15]);//16
               fprintf(fpp, "%lf\t", traj[0][16]);//17
               fprintf(fpp, "%lf\t", traj[0][17]);//18

               fprintf(fpp, "%lf\t", traj[0][18]);//19
               fprintf(fpp, "%lf\t", traj[0][19]);//20
               fprintf(fpp, "%lf\t", traj[0][20]);//21

               fprintf(fpp, "%lf\t", traj[0][21]);//22
               fprintf(fpp, "%lf\t", traj[0][22]);//23
               fprintf(fpp, "%lf\t", traj[0][23]);//24
               fprintf(fpp, "%lf\t", traj[0][24]);//25
               fprintf(fpp, "%lf\t", traj[0][25]);//26
               fprintf(fpp, "%lf\t", traj[0][26]);//27
               fprintf(fpp, "%lf\t", traj[0][27]);//28
               fprintf(fpp, "%lf\t", traj[0][28]);//29
               fprintf(fpp, "%lf\t", traj[0][29]);//30

               fprintf(fpp, "%lf\t", traj[0][30]);//31allcrieteria
               ///curvature and grad kinetic energy stuff
               if(mainForm->interpolRadioGroup->ItemIndex==0){
                  fprintf(fpp, "%lf\n", (double)pCounterB);//32reldiv
               }
               else{
                  fprintf(fpp, "%lf\t", (double)pCounterB);//32//reldiv
                  fprintf(fpp, "%lf\t", traj[0][33]);//33
                  fprintf(fpp, "%lf\t", traj[0][34]);//34
                  fprintf(fpp, "%lf\t", traj[0][35]);//35
                  fprintf(fpp, "%lf\t", traj[0][36]);//36
                  fprintf(fpp, "%lf\t", traj[0][37]);//37
                  fprintf(fpp, "%lf\t", traj[0][38]);//38
                  fprintf(fpp, "%lf\t", traj[0][39]);//39
                  fprintf(fpp, "%lf\t", traj[0][40]);//40
                  fprintf(fpp, "%lf\t", traj[0][41]);//41
                  fprintf(fpp, "%lf\t", traj[0][42]);//42
                  fprintf(fpp, "%lf\t", traj[0][43]);//43
                  fprintf(fpp, "%lf\t", traj[0][44]);//44
                  fprintf(fpp, "%lf\t", traj[0][45]);//45
                  fprintf(fpp, "%lf\t", traj[0][46]);//46
                  fprintf(fpp, "%lf\n", traj[0][47]);//47
               }
               ///end urvature and grad kinetic energy stuff

               if(divCriteria<0.1){
                  count2++;
                  meanDiss=meanDiss+viscosity*twosijsij;
                  meanUisq=meanUisq+(1./1.)*( traj[0][3]*traj[0][3]
                                             +traj[0][4]*traj[0][4]
                                             +traj[0][5]*traj[0][5]);
                  meanDudxsq=meanDudxsq+(1./1.)*( traj[0][6]*traj[0][6]
                                                 +traj[0][10]*traj[0][10]
                                                 +traj[0][14]*traj[0][14]);
               }
               if(accCriteria<0.1){
                  count4++;
               }
               if(divaCriteria<0.1){
                  count5++;
               }
               ///curvature and grad kinetic energy stuff
               if(mainForm->interpolRadioGroup->ItemIndex==1){
                  if(curvCriteria<0.1){
                     count7++;
                  }
                  if(gradCriteria<0.1){
                     count8++;
                  }
               }
           }//z loop
        }//y loop
     }//x loop



     mainForm->meanPointsInSphereBEdit->Text=IntToStr((int)(pointList->meanPointsInSphereB+0.5));
     mainForm->meanPointsInSphereEdit->Text =IntToStr((int)(pointList->meanPointsInSphere+0.5));
     if(count2>0){
        mainForm->meanDissEdit->Text=IntToStr((int)(1.e6*pointList->meanDiss/(double)count2+0.5));
        int Reynolds=(int)((pow(meanUisq/(double)count2,0.5)*pow(meanUisq/meanDudxsq,0.5))/viscosity+0.5);
        mainForm->reEdit->Text=IntToStr(Reynolds);
        mainForm->meanSuccessDivEdit->Text=IntToStr((int)(100.*(double)pointList->count2/(double)pointList->count+0.5));
        mainForm->meanSuccessAccEdit->Text=IntToStr((int)(100.*(double)pointList->count4/(double)pointList->count+0.5));
        mainForm->meanSuccessDivAEdit->Text=IntToStr((int)(100.*(double)pointList->count5/(double)pointList->count+0.5));
        ///curvature and grad kinetic energy stuff
        if(mainForm->interpolRadioGroup->ItemIndex==1){
           mainForm->meanSuccessCurvEdit->Text=IntToStr((int)(100.*(double)pointList->count7/(double)pointList->count+0.5));
           mainForm->meanSuccessGradKEdit->Text=IntToStr((int)(100.*(double)pointList->count8/(double)pointList->count+0.5));
        }
     }
     mainForm->Refresh();
}
//----------------------------------------------------------


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
     int minRemain=StrToInt(mainForm->minRemainEdit->Text);

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
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::reLink()
{
     double dist,radius,centerX,centerY,centerZ,u,v,w,x,y,z,vel,velNew;
     int pCounter,gain;

     radius=1.;//StrToFloat(mainForm->sphereEdit->Text);

     //loop through files
     for(int i=0;i<51;i++){
        for(int j=0;j<5000;j++){
           for(int k=0;k<32;k++){
              trajec[i][j][k]=0.;
           }
        }
     }
     for (int i=firstFile;i<lastFile+1;i++){
        gain=0;
        mainForm->fileNum2Edit->Text=IntToStr(i);
        mainForm->Refresh();
        baseName="trajPoint.";
        readTrajPointFile(i);
        baseName="ptv_is.";
        readPTVFile(i,-1);
        readPTVFile(i,0);
        readPTVFile(i,1);
        
        int nP=point[10][0][0];
        for(int i=1;i<nP+1;i++){
           if(point[10][i][0]<1 || point[10][i][1]<1){
              //search through t-50 files
              setAllMatrixesToZero(4);
              pCounter=0;
              centerX=point[10][i][2];
              centerY=point[10][i][3];
              centerZ=point[10][i][4];
              for(int k=0;k<51;k++){
                 for(int l=1;l<trajec[k][0][0]+1;l++){
                    //if close enough it becomes candidate
                    if(-50+k+trajec[k][l][28]==0){
                       dist=pow( pow(point[10][i][2]-trajec[k][l][0],2.)
                                +pow(point[10][i][3]-trajec[k][l][1],2.)
                                +pow(point[10][i][4]-trajec[k][l][2],2.),0.5);
                       if(dist<radius && pCounter<arraySize){
                          dis[pCounter]=(int)(dist*1000.+0.5);
                          A[pCounter][0]=1.;
                          A[pCounter][1]=trajec[k][l][0];
                          A[pCounter][2]=trajec[k][l][1];
                          A[pCounter][3]=trajec[k][l][2];
                          Yu[pCounter]=trajec[k][l][3];
                          Yv[pCounter]=trajec[k][l][4];
                          Yw[pCounter]=trajec[k][l][5];
                          pCounter++;
                       }
                    }
                 }
              }
              if(pCounter>3){ // %jetzt wird endlich Punkt1 interpoliert
                  //%correct x,y,z with center of interpolation!
                  for(int m=0;m<pCounter;m++){
                     A[m][1]=A[m][1]-centerX;
                     A[m][2]=A[m][2]-centerY;
                     A[m][3]=A[m][3]-centerZ;
                     if(1<2){
                        A[m][0]=A[m][0]*cor[dis[m]];
                        A[m][1]=A[m][1]*cor[dis[m]];
                        A[m][2]=A[m][2]*cor[dis[m]];
                        A[m][3]=A[m][3]*cor[dis[m]];
                        Yu[m]=Yu[m]*cor[dis[m]];
                        Yv[m]=Yv[m]*cor[dis[m]];
                        Yw[m]=Yw[m]*cor[dis[m]];
                     }
                  }
                  makeAT(pCounter,4);
                  makeATA(pCounter,4);
                  makeATY(pCounter,4,1);
                  solve(pCounter,4);
                  u=X[0];
                  makeAT(pCounter,4);
                  makeATA(pCounter,4);
                  makeATY(pCounter,4,2);
                  solve(pCounter,4);
                  v=X[0];
                  makeAT(pCounter,4);
                  makeATA(pCounter,4);
                  makeATY(pCounter,4,3);
                  solve(pCounter,4);
                  w=X[0];
                  vel=pow(u*u+v*v+w*w,0.5);
                  if(point[10][i][0]<1){
                     x=point[10][i][2]-deltaT*u;
                     y=point[10][i][3]-deltaT*v;
                     z=point[10][i][4]-deltaT*w;
                     int nPm=point[9][0][0];
                     for(int i=1;i<nPm+1;i++){
                        if(point[9][i][1]<1){
                           dist=pow( pow(point[9][i][2]-x,2.)
                                    +pow(point[9][i][3]-y,2.)
                                    +pow(point[9][i][4]-z,2.),0.5);
                           if(dist<0.0005){
                              velNew=dist/deltaT;
                              gain++;
                           }
                        }

                     }
                  }
                  if(point[10][i][1]<1){
                     x=point[10][i][2]+deltaT*u;
                     y=point[10][i][3]+deltaT*v;
                     z=point[10][i][4]+deltaT*w;
                     int nPp=point[9][0][0];
                     for(int i=1;i<nPp+1;i++){
                        if(point[11][i][0]<1){
                           dist=pow( pow(point[11][i][2]-x,2.)
                                    +pow(point[11][i][3]-y,2.)
                                    +pow(point[11][i][4]-z,2.),0.5);
                           if(dist<0.0005){
                              velNew=dist/deltaT;
                              gain++;
                           }
                        }
                     }
                  }
              }
           }
        }
        gain=gain;
     }
}
//---------------------------------------------------------------------------
void __fastcall TpointList::doLinearInterp(FILE *fpp)
{
     int pCounter;
     double gridSize,radius,centerX,centerY,centerZ,dist;
     double Liu[4],Liv[4],Liw[4],Liax[4],Liay[4],Liaz[4];
     double w1,w2,w3,s11,s12,s13,s22,s23,s33,ww1,ww2,ww3,wwsij;
     double s111,s222,s333,s112,s113,s221,s223,s331,s332,s123;
     double sijsjkski,wsq,twosijsij,R,Q,div,ref,diss;

     gridSize=1.;//StrToFloat(mainForm->gridSizeEdit->Text);
     radius=1.;//StrToFloat(mainForm->sphereEdit->Text);

     for(double x=minX;x<maxX+gridSize;x=x+gridSize){
        for(double y=minY;y<maxY+gridSize;y=y+gridSize){
           for(double z=minZ;z<maxZ+gridSize;z=z+gridSize){
               //%Da soll jetzt duidxj linear interpoliert werden
               //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
               //%die nächsten Punkte zu Punkt xr,yr,zr, finden
               pCounter=0;
               count++;
               setAllMatrixesToZero(4);
               centerX=0.;
               centerY=0.;
               centerZ=0.;
               for(int i=0;i<arraySize;i++){
                  dis[i]=0.;
               }
               
               for(int i=1;i<point[0][0][0]+1;i++){
                  if(point[0][i][11]>0.){
                     dist=pow(pow(point[0][i][2]-x,2.)+pow(point[0][i][3]-y,2.)+pow(point[0][i][4]-z,2.),0.5);
                     if(dist<radius && pCounter<arraySize){
                        dis[pCounter]=(int)(dist*1000.+0.5);
                        centerX=centerX+point[0][i][2];
                        centerY=centerY+point[0][i][3];
                        centerZ=centerZ+point[0][i][4];
                        A[pCounter][0]=1.;
                        A[pCounter][1]=point[0][i][2];
                        A[pCounter][2]=point[0][i][3];
                        A[pCounter][3]=point[0][i][4];
                        Yu[pCounter]=point[0][i][5];
                        Yv[pCounter]=point[0][i][6];
                        Yw[pCounter]=point[0][i][7];
                        Yax[pCounter]=point[0][i][8];
                        Yay[pCounter]=point[0][i][9];
                        Yaz[pCounter]=point[0][i][10];
                        pCounter++;
                     }
                  }
               }
               meanPointsInSphere=(meanPointsInSphere*(double)(count-1)+(double)pCounter)/(double)count;
               if(pCounter>3){ // %jetzt wird endlich Punkt1 interpoliert
                  centerX=centerX/(double)pCounter;
                  centerY=centerY/(double)pCounter;
                  centerZ=centerZ/(double)pCounter;
                  //%correct x,y,z with center of interpolation!
                  for(int m=0;m<pCounter;m++){
                     A[m][1]=A[m][1]-centerX;
                     A[m][2]=A[m][2]-centerY;
                     A[m][3]=A[m][3]-centerZ;
                     if(1<2){
                         A[m][0]=A[m][0]*cor[dis[m]];
                         A[m][1]=A[m][1]*cor[dis[m]];
                         A[m][2]=A[m][2]*cor[dis[m]];
                         A[m][3]=A[m][3]*cor[dis[m]];
                         Yu[m]=Yu[m]*cor[dis[m]];
                         Yv[m]=Yv[m]*cor[dis[m]];
                         Yw[m]=Yw[m]*cor[dis[m]];
                         Yax[m]=Yax[m]*cor[dis[m]];
                         Yay[m]=Yay[m]*cor[dis[m]];
                         Yaz[m]=Yaz[m]*cor[dis[m]];
                     }
                  }
                  makeAT(pCounter,4);
                  makeATA(pCounter,4);
                  makeATY(pCounter,4,1);
                  solve(pCounter,4);
                  Liu[0]=X[0];
                  Liu[1]=X[1];
                  Liu[2]=X[2];
                  Liu[3]=X[3];
                  makeAT(pCounter,4);
                  makeATA(pCounter,4);
                  makeATY(pCounter,4,2);
                  solve(pCounter,4);
                  Liv[0]=X[0];
                  Liv[1]=X[1];
                  Liv[2]=X[2];
                  Liv[3]=X[3];
                  makeAT(pCounter,4);
                  makeATA(pCounter,4);
                  makeATY(pCounter,4,3);
                  solve(pCounter,4);
                  Liw[0]=X[0];
                  Liw[1]=X[1];
                  Liw[2]=X[2];
                  Liw[3]=X[3];
                  makeAT(pCounter,4);
                  makeATA(pCounter,4);
                  makeATY(pCounter,4,4);
                  solve(pCounter,4);
                  Liax[0]=X[0];
                  makeAT(pCounter,4);
                  makeATA(pCounter,4);
                  makeATY(pCounter,4,5);
                  solve(pCounter,4);
                  Liay[0]=X[0];
                  makeAT(pCounter,4);
                  makeATA(pCounter,4);
                  makeATY(pCounter,4,6);
                  solve(pCounter,4);
                  Liaz[0]=X[0];

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
                  div=fabs(s11+s22+s33);
                  ref=fabs(s11)+fabs(s22)+fabs(s33);

                  ww1=w1*s11+w2*s12+w3*s13;
                  ww2=w1*s12+w2*s22+w3*s23;
                  ww3=w1*s13+w2*s23+w3*s33;
                  wwsij=w1*ww1+w2*ww2+w3*ww3;

                  s111=s11*s11*s11;
                  s222=s22*s22*s22;
                  s333=s33*s33*s33;
                  s112=s11*s12*s12;
                  s113=s11*s13*s13;
                  s221=s22*s12*s12;
                  s223=s22*s23*s23;
                  s331=s33*s13*s13;
                  s332=s33*s23*s23;
                  s123=s12*s23*s13;
                  sijsjkski=s111+s222+s333+3.*(s112+s113+s221+s223+s331+s332)+6.*s123;

                  wsq=w1*w1+w2*w2+w3*w3;
                  twosijsij=2.*(s11*s11+s22*s22+s33*s33+2.*(s12*s12+s13*s13+s23*s23));
                  diss=StrToFloat(mainForm->viscEdit->Text)*twosijsij;

                  Q=(1./4.)*(wsq-twosijsij);
                  R=-(1./3.)*(sijsjkski+(3./4.)*wwsij);

                  if(div/ref<0.1){
                      count2++;
                      meanDiss=(meanDiss*(double)(count2-1)+(double)diss)/(double)count2;
                      fprintf(fpp, "%lf\t", x);
                      fprintf(fpp, "%lf\t", y);
                      fprintf(fpp, "%lf\t", z);
                      fprintf(fpp, "%lf\t", Liu[0]);
                      fprintf(fpp, "%lf\t", Liv[0]);
                      fprintf(fpp, "%lf\t", Liw[0]);
                      fprintf(fpp, "%lf\t", Liax[0]);
                      fprintf(fpp, "%lf\t", Liay[0]);
                      fprintf(fpp, "%lf\t", Liaz[0]);
                      fprintf(fpp, "%lf\t", w1);
                      fprintf(fpp, "%lf\t", w2);
                      fprintf(fpp, "%lf\t", w3);
                      fprintf(fpp, "%lf\t", s11);
                      fprintf(fpp, "%lf\t", s12);
                      fprintf(fpp, "%lf\t", s13);
                      fprintf(fpp, "%lf\t", s22);
                      fprintf(fpp, "%lf\t", s23);
                      fprintf(fpp, "%lf\t", s33);
                      fprintf(fpp, "%lf\t", ww1);
                      fprintf(fpp, "%lf\t", ww2);
                      fprintf(fpp, "%lf\t", ww3);
                      fprintf(fpp, "%lf\t", wwsij);
                      fprintf(fpp, "%lf\t", sijsjkski);
                      fprintf(fpp, "%lf\t", R);
                      fprintf(fpp, "%lf\t", Q);
                      fprintf(fpp, "%lf\n", diss);
                  }
               }
           }
        }
     }
}
//---------------------------------------------------------------------------
void __fastcall TpointList::setAllMatrixesToZero(int size)
{

    for(int i=0;i<arraySize;i++){
       if(i<size){
          X[i]=0.;
          m0TY[i]=0.;
          ATY[i]=0.;
          BTY[i]=0.;
          CTY[i]=0.;
          DTY[i]=0.;
       }
       Y[i]=0.;
       Yu[i]=0.;
       Yv[i]=0.;
       Yw[i]=0.;
       Yu0[i]=0.;
       Yv0[i]=0.;
       Yw0[i]=0.;
       YuA[i]=0.;
       YvA[i]=0.;
       YwA[i]=0.;
       YuB[i]=0.;
       YvB[i]=0.;
       YwB[i]=0.;
       YuC[i]=0.;
       YvC[i]=0.;
       YwC[i]=0.;
       YuD[i]=0.;
       YvD[i]=0.;
       YwD[i]=0.;
       Yaz[i]=0.;
       Yay[i]=0.;
       Yax[i]=0.;
       for(int j=0;j<size;j++){
          m0[i][j]=0.;
          m0T[j][i]=0.;
          if(i<size){m0T0[i][j]=0.;}
          A[i][j]=0.;
          AT[j][i]=0.;
          if(i<size){ATA[i][j]=0.;}
          B[i][j]=0.;
          BT[j][i]=0.;
          if(i<size){BTB[i][j]=0.;}
          C[i][j]=0.;
          CT[j][i]=0.;
          if(i<size){CTC[i][j]=0.;}
          D[i][j]=0.;
          DT[j][i]=0.;
          if(i<size){DTD[i][j]=0.;}
       }
    }
    
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::make0T(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
           m0T[i][j]=m0[j][i];
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
void __fastcall TpointList::makeDT(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
           DT[i][j]=D[j][i];
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
void __fastcall TpointList::makeCT(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
           CT[i][j]=C[j][i];
        }
     }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TpointList::make0T0(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
           m0T0[i][j]=0.;
           for(int k=0;k<n;k++){
              m0T0[i][j]=m0T0[i][j]+m0T[i][k]*m0[k][j];
           }
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
void __fastcall TpointList::makeDTD(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
           DTD[i][j]=0.;
           for(int k=0;k<n;k++){
              DTD[i][j]=DTD[i][j]+DT[i][k]*D[k][j];
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
void __fastcall TpointList::makeCTC(int n, int m)
{
     for(int i=0;i<m;i++){
        for(int j=0;j<m;j++){
           CTC[i][j]=0.;
           for(int k=0;k<n;k++){
              CTC[i][j]=CTC[i][j]+CT[i][k]*C[k][j];
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
               switch (wh) {
                  case 0 :
                     ATY[i]=ATY[i]+AT[i][k]*Y[k];
                     break;
                  case 1 :
                     ATY[i]=ATY[i]+AT[i][k]*YuA[k];
                     break;
                  case 2 :
                     ATY[i]=ATY[i]+AT[i][k]*YvA[k];
                     break;
                  case 3 :
                     ATY[i]=ATY[i]+AT[i][k]*YwA[k];
                     break;
                  case 7 :
                     ATY[i]=ATY[i]+AT[i][k]*y3[k];
                     break;
                  case 8 :
                     ATY[i]=ATY[i]+AT[i][k]*y4[k];
                     break;
                  case 9 :
                     ATY[i]=ATY[i]+AT[i][k]*y5[k];
                     break;
                  case 10 :
                     ATY[i]=ATY[i]+AT[i][k]*y6[k];
                     break;
                  case 11 :
                     ATY[i]=ATY[i]+AT[i][k]*y7[k];
                     break;
                  case 12 :
                     ATY[i]=ATY[i]+AT[i][k]*y8[k];
                     break;
                  case 13 :
                     ATY[i]=ATY[i]+AT[i][k]*y9[k];
                     break;
                  case 14 :
                     ATY[i]=ATY[i]+AT[i][k]*y10[k];
                     break;
                  case 15 :
                     ATY[i]=ATY[i]+AT[i][k]*y11[k];
                     break;
                  case 16 :
                     ATY[i]=ATY[i]+AT[i][k]*y12[k];
                     break;
                  case 17 :
                     ATY[i]=ATY[i]+AT[i][k]*y13[k];
                     break;
                  case 18 :
                     ATY[i]=ATY[i]+AT[i][k]*y14[k];
                     break;
                  case 19 :
                     ATY[i]=ATY[i]+AT[i][k]*y15[k];
                     break;
                  case 20 :
                     ATY[i]=ATY[i]+AT[i][k]*y16[k];
                     break;
                  case 21 :
                     ATY[i]=ATY[i]+AT[i][k]*y17[k];
                     break;
                  case 22 :
                     ATY[i]=ATY[i]+AT[i][k]*y18[k];
                     break;
                  case 23 :
                     ATY[i]=ATY[i]+AT[i][k]*y19[k];
                     break;
                  case 24 :
                     ATY[i]=ATY[i]+AT[i][k]*y20[k];
                     break;
                  case 25 :
                     ATY[i]=ATY[i]+AT[i][k]*y21[k];
                     break;
                  case 26 :
                     ATY[i]=ATY[i]+AT[i][k]*y22[k];
                     break;
                  case 27 :
                     ATY[i]=ATY[i]+AT[i][k]*y23[k];
                     break;
                  case 28 :
                     ATY[i]=ATY[i]+AT[i][k]*y24[k];
                     break;
                  case 29 :
                     ATY[i]=ATY[i]+AT[i][k]*y25[k];
                     break;
                  case 30 :
                     ATY[i]=ATY[i]+AT[i][k]*y26[k];
                     break;
                  case 31 :
                     ATY[i]=ATY[i]+AT[i][k]*y27[k];
                     break;
                  case 32 :
                     ATY[i]=ATY[i]+AT[i][k]*y28[k];
                     break;
                  case 33 :
                     ATY[i]=ATY[i]+AT[i][k]*y29[k];
                     break;
                  case 34 :
                     ATY[i]=ATY[i]+AT[i][k]*y30[k];
                     break;
                  case 35 :
                     ATY[i]=ATY[i]+AT[i][k]*y31[k];
                     break;
                  case 36 :
                     ATY[i]=ATY[i]+AT[i][k]*y32[k];
                     break;
                  case 37 :
                     ATY[i]=ATY[i]+AT[i][k]*y33[k];
                     break;
                  case 38 :
                     ATY[i]=ATY[i]+AT[i][k]*y34[k];
                     break;
                  case 39 :
                     ATY[i]=ATY[i]+AT[i][k]*y35[k];
                     break;
                  case 40 :
                     ATY[i]=ATY[i]+AT[i][k]*y36[k];
                     break;
                  case 41 :
                     ATY[i]=ATY[i]+AT[i][k]*y37[k];
                     break;
                  case 42 :
                     ATY[i]=ATY[i]+AT[i][k]*y38[k];
                     break;
                  case 43 :
                     ATY[i]=ATY[i]+AT[i][k]*y39[k];
                     break;
                  case 44 :
                     ATY[i]=ATY[i]+AT[i][k]*y40[k];
                     break;
                  case 45 :
                     ATY[i]=ATY[i]+AT[i][k]*y41[k];
                     break;
                  
               }
           }
     }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::make0TY(int n, int m,int wh)
{
     for(int i=0;i<m;i++){
           m0TY[i]=0.;
           for(int k=0;k<n;k++){
               switch (wh) {
                  case 1 :
                     m0TY[i]=m0TY[i]+m0T[i][k]*Yu0[k];
                     break;
                  case 2 :
                     m0TY[i]=m0TY[i]+m0T[i][k]*Yv0[k];
                     break;
                  case 3 :
                     m0TY[i]=m0TY[i]+m0T[i][k]*Yw0[k];
                     break;
               }
           }
     }
}
//----------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeCTY(int n, int m,int wh)
{
     for(int i=0;i<m;i++){
           CTY[i]=0.;
           for(int k=0;k<n;k++){
               switch (wh) {
                  case 1 :
                     CTY[i]=CTY[i]+CT[i][k]*YuC[k];
                     break;
                  case 2 :
                     CTY[i]=CTY[i]+CT[i][k]*YvC[k];
                     break;
                  case 3 :
                     CTY[i]=CTY[i]+CT[i][k]*YwC[k];
                     break;
               }
           }
     }
}
//----------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeDTY(int n, int m,int wh)
{
     for(int i=0;i<m;i++){
           DTY[i]=0.;
           for(int k=0;k<n;k++){
               switch (wh) {
                  case 1 :
                     DTY[i]=DTY[i]+DT[i][k]*YuD[k];
                     break;
                  case 2 :
                     DTY[i]=DTY[i]+DT[i][k]*YvD[k];
                     break;
                  case 3 :
                     DTY[i]=DTY[i]+DT[i][k]*YwD[k];
                     break;
               }
           }
     }
}
//----------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeCXmY(int n, int m)
{
     for(int i=0;i<n;i++){
           CY[i]=0.;
           for(int k=0;k<m;k++){
              CY[i]=CY[i]+C[i][k]*X[k];
           }
           Resid[i]=CY[i]-Y[i];
     }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TpointList::makekResidError(int n)
{
     double kResid,BY[300],ResidU[300],ResidV[300],ResidW[300];
     double weightSum,dist;
     int distIndex;

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
           ResidV[i]=BY[i]-YvB[i];
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
           ResidW[i]=BY[i]-YwB[i];
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
     int distIndex;

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
           ResidV[i]=BY[i]-YvB[i];
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
           ResidW[i]=BY[i]-YwB[i];
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
     int distIndex;

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
           ResidV[i]=BY[i]-YvB[i];
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
           ResidW[i]=BY[i]-YwB[i];
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
     int distIndex;

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
     int distIndex;

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
     int distIndex;

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
     int distIndex;


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
           ResidV[i]=BY[i]-YvB[i];
     }

     //now calc mean Resiudual Energy
     dvError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][2]*B[i][2],0.5);
        distIndex=(int)(dist*1000+0.5);
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
     int distIndex;


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
           ResidV[i]=BY[i]-YvB[i];
     }

     //now calc mean Resiudual Energy
     dvError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][2]*B[i][2],0.5);
        distIndex=(int)(dist*1000+0.5);
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
     int distIndex;


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
           ResidV[i]=BY[i]-YvB[i];
     }

     //now calc mean Resiudual Energy
     dvError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][2]*B[i][2],0.5);
        distIndex=(int)(dist*1000+0.5);
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
     int distIndex;


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
           ResidW[i]=BY[i]-YwB[i];
     }
     //now calc mean Resiudual Energy
     dwError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][3]*B[i][3],0.5);
        distIndex=(int)(dist*1000+0.5);
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
     int distIndex;


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
           ResidW[i]=BY[i]-YwB[i];
     }
     //now calc mean Resiudual Energy
     dwError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][3]*B[i][3],0.5);
        distIndex=(int)(dist*1000+0.5);
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
     int distIndex;


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
           ResidW[i]=BY[i]-YwB[i];
     }
     //now calc mean Resiudual Energy
     dwError=0;
     weightSum=0;
     for(int i=0;i<n;i++){
        dist=pow(B[i][3]*B[i][3],0.5);
        distIndex=(int)(dist*1000+0.5);
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
//----------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::makeBTY(int n, int m,int wh)
{
     for(int i=0;i<m;i++){
           BTY[i]=0.;
           for(int k=0;k<n;k++){
               switch (wh) {
                  case 1 :
                     BTY[i]=BTY[i]+BT[i][k]*YuB[k];
                     break;
                  case 2 :
                     BTY[i]=BTY[i]+BT[i][k]*YvB[k];
                     break;
                  case 3 :
                     BTY[i]=BTY[i]+BT[i][k]*YwB[k];
                     break;
                  case 4 :
                     BTY[i]=BTY[i]+BT[i][k]*y0[k];
                     break;
                  case 5 :
                     BTY[i]=BTY[i]+BT[i][k]*y1[k];
                     break;
                  case 6 :
                     BTY[i]=BTY[i]+BT[i][k]*y2[k];
                     break;
                  case 7 :
                     BTY[i]=BTY[i]+BT[i][k]*y3[k];
                     break;
                  case 8 :
                     BTY[i]=BTY[i]+BT[i][k]*y4[k];
                     break;
                  case 9 :
                     BTY[i]=BTY[i]+BT[i][k]*y5[k];
                     break;
                  case 10 :
                     BTY[i]=BTY[i]+BT[i][k]*y6[k];
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
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall TpointList::solveB(int n, int m)
{
    double faktor;
    bool ok=true;

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
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall TpointList::solve0(int n, int m)
{
    double faktor;
    bool ok=true;
    int indMax;
    double s[45],q[45],max;
    double dummy[45],dummyCTY;

    //mit relativer Kolonnenmaximierungsstrategie, Schwarz, p.23


    for(int i=1;i<m;i++){
       for(int j=i;j<m;j++){
          if(fabs(m0T0[j][i-1])>0.){
             faktor=m0T0[i-1][i-1]/m0T0[j][i-1];
             for(int k=0;k<m;k++){
                m0T0[j][k]=m0T0[i-1][k]-faktor*m0T0[j][k];
             }
             m0TY[j]=m0TY[i-1]-faktor*m0TY[j];
          }
       }
    }
    for(int i=m-1;i>-1;i--){
       for(int j=i+1;j<m;j++){
          m0TY[i]=m0TY[i]-m0T0[i][j]*X[j];
       }
       if(fabs(m0T0[i][i])>0.){
          X[i]=m0TY[i]/m0T0[i][i];
       }
       else{
           ok=false;
       }

    }
    return ok;
}
//------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall TpointList::solveC(int n, int m)
{
    double faktor;
    bool ok=true;
    //int indMax;
    //double s[45],q[45],max;
    //double dummy[45],dummyCTY;

    //mit relativer Kolonnenmaximierungsstrategie, Schwarz, p.23


    for(int i=1;i<m;i++){
       /*
       //look which lines are to change
       for(int j=i-1;j<m;j++){
           s[j]=1.;
           for(int k=0;k<m;k++){
              s[j]=s[j]+fabs(CTC[j][k]);
           }
       }
       for(int j=0;j<m;j++){
          q[j]=0;
       }
       for(int j=i-1;j<m;j++){
           q[j]=CTC[j][i-1]/s[j];
       }
         //find max
       max=0;
       indMax=i-1;
       for(int j=i-1;j<m;j++){
          if(fabs(q[j])>max){
             max=fabs(q[j]);
             indMax=j;
          }
       }
       //change lines
       if(indMax>i-1){
           for(int k=0;k<m;k++){
              dummy[k]=CTC[i-1][k];
              dummyCTY=CTY[i-1];
           }
           for(int k=0;k<m;k++){
              CTC[i-1][k]=CTC[indMax][k];
              CTY[i-1]=CTY[indMax];
           }
           for(int k=0;k<m;k++){
              CTC[indMax][k]=dummy[k];
              CTY[indMax]=dummyCTY;
           }
       } 
       //go
       */
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
    return ok;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall TpointList::solveD(int n, int m)
{
    double faktor;
    bool ok=true;

    //mit relativer Kolonnenmaximierungsstrategie, Schwarz, p.23


    for(int i=1;i<m;i++){
       for(int j=i;j<m;j++){
          if(fabs(DTD[j][i-1])>0.){
             faktor=DTD[i-1][i-1]/DTD[j][i-1];
             for(int k=0;k<m;k++){
                DTD[j][k]=DTD[i-1][k]-faktor*DTD[j][k];
             }
             DTY[j]=DTY[i-1]-faktor*DTY[j];
          }
       }
    }
    for(int i=m-1;i>-1;i--){
       for(int j=i+1;j<m;j++){
          DTY[i]=DTY[i]-DTD[i][j]*X[j];
       }
       if(fabs(DTD[i][i])>0.){
          X[i]=DTY[i]/DTD[i][i];
       }
       else{
           ok=false;
       }

    }
    return ok;
}
//------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TpointList::autoCorrelation()
{
    double dist,co,va,cox,vax,coy,vay,coz,vaz,Aco,Ava,Acox,Avax,Acoy,Avay,Acoz,Avaz;
    double pPerRa[100],minDist[10];
    int index,rank;
    int in[5];
    for (int k=0;k<100;k++){
       pPerRa[k]=0;
    }
    int stepSize=StrToInt(graphForm->stepEdit->Text);

    double normalizerX;
    double normalizerY;
    double normalizerZ;

    for(int i=1;i<point[2][0][0];i=i+stepSize){
     if(point[2][i][11]>0.){
       for (int k=0;k<100;k++){
          pPerRa[k]=0;
       }
       for(int j=0;j<10;j++){
          minDist[j]=10.;
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
             for (int k=0;k<100;k++){
                if(index<k){
                   pPerRa[k]=pPerRa[k]+1;
                }
             }
          }
       }
       //update of minDist;
       for(int j=0;j<10;j++){
          if(minDist[j]>0 && minDist[j]<0.01){
              index=(int)(minDist[j]*10000.+0.5);
              if(index<100){
                  minDistArray[j][index]=minDistArray[j][index]+1;
              }
          }
       }

       for(int j=0;j<5;j++){
          in[j]=10000;
       }
       if(point[2][i][11]>0.){
           in[2]=i;
       }
       if(in[2]<10000 && point[2][in[2]][1]>0.){
           if(point[3][(int)point[2][in[2]][1]][11]>0){
               in[3]=point[2][in[2]][1];
           }
       }
       if(in[3]<10000 && point[3][in[3]][1]>0.){
           if(point[4][(int)point[3][in[3]][1]][11]>0){
               in[4]=point[3][in[3]][1];
           }
       }
       if(in[2]<10000 && point[2][in[2]][0]>0.){
           if(point[1][(int)point[2][in[2]][0]][11]>0){
               in[1]=point[2][in[2]][0];
           }
       }
       if(in[1]<10000 && point[1][in[1]][0]>0.){
           if(point[0][(int)point[1][in[1]][0]][11]>0){
               in[0]=point[1][in[1]][0];
           }
       }


       //pointPerRadius
       for (int k=0;k<100;k++){
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
    normalizerX=covx[0][1];
    normalizerY=covy[0][1];
    normalizerZ=covz[0][1];
    for(int i=0;i<300;i++){
       if(normalizerX>0.){
          //cor[i]=cov[i][1]/var[i][1];
          corx[i]=covx[i][1]/normalizerX;//varx[i][1];
          cory[i]=covy[i][1]/normalizerY;//vary[i][1];
          corz[i]=covz[i][1]/normalizerZ;//varz[i][1];
          graphForm->Series1->AddXY((double)i/10.,corx[i],'.',clTeeColor);
          graphForm->Series2->AddXY((double)i/10.,cory[i],'.',clTeeColor);
          graphForm->Series3->AddXY((double)i/10.,corz[i],'.',clTeeColor);
       }
    }

    graphForm->Series5->Clear();
    graphForm->Series6->Clear();
    graphForm->Series7->Clear();
    normalizerX=Acovx[0][1];
    normalizerY=Acovy[0][1];
    normalizerZ=Acovz[0][1];
    for(int i=0;i<300;i++){
       if(normalizerX>0.){
          //Acor[i]=Acov[i][1]/Avar[i][1];
          Acorx[i]=Acovx[i][1]/normalizerX;//Avarx[i][1];
          Acory[i]=Acovy[i][1]/normalizerY;//Avary[i][1];
          Acorz[i]=Acovz[i][1]/normalizerZ;//Avarz[i][1];
          graphForm->Series5->AddXY((double)i/10.,Acorx[i],'.',clTeeColor);
          graphForm->Series6->AddXY((double)i/10.,Acory[i],'.',clTeeColor);
          graphForm->Series7->AddXY((double)i/10.,Acorz[i],'.',clTeeColor);
       }
    }

    
   
    //draw points per radius stuff stuff
    graphForm->Series4->Clear();
    for(int i=0;i<100;i++){
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
    for(int i=1;i<100;i++){
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
             uaMean=(uaMean*(counter-1)-ua)/counter;
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
        structForm->Series5->AddXY((double)0/10.,1e6*uaMean,'.',clTeeColor);
        structForm->Series5->AddXY((double)200/10.,1e6*uaMean,'.',clTeeColor);
        structForm->Refresh();
     }

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
     double sumUkin;
     double sumaSqu;
     double sumUkinBelow;
     double sumUkinAbove;
     double sumaSquBelow;
     double sumaSquAbove;

     double dx,dy,dz,deltaU,deltaV,deltaW,deltaVel,deltaVelTrue;

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
                if(1<2){//(point[2][j][13]>0.1){
                   uKinAboveArray[index]=uKinAboveArray[index]+1;
                }
             }
             index=(int)(aSqu*pdfSize/aSquMax+0.5);
             if(index>0 && index<pdfSize){
                if(point[2][j][13]<0.1){
                   aSqBelowArray[index]=aSqBelowArray[index]+1;
                }
                if(1<2){//(point[2][j][13]>0.1){
                   aSqAboveArray[index]=aSqAboveArray[index]+1;
                }
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
                      deltaVelTrue=du*du;
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
                      if(1<2){//(point[2][j][13]>0.1 && point[2][k][13]>0.1){
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
                      }
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
//----------------------constructor-----------------------------------------
__fastcall TpointList::TpointList(void)
    : TObject()
{
        
        arraySize=300;
        maxRowIndex=300;
        kriging=false;
        //c1=50.;//10.; //weight of rel.divergence
        //c2=0.15;//0.25;//
        minTrajLength=3;
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

    if(1<2){
       pointList->makeCor();
    }
    pointList->minX=StrToFloat(minXEdit->Text);
    pointList->minY=StrToFloat(minYEdit->Text);
    pointList->minZ=StrToFloat(minZEdit->Text);
    

    pointList->count=0;
    pointList->meanPointsInSphere=0;
    pointList->count2=0;
    pointList->meanDiss=0.;
    for (int i=pointList->firstFile;i<pointList->lastFile+1;i++){
       fileNum2Edit->Text=IntToStr(i);
       fileNum2Edit->Refresh();
       meanPointsInSphereEdit->Text=FloatToStr(pointList->meanPointsInSphere);
       meanPointsInSphereEdit->Refresh();
       meanDissEdit->Text=FloatToStr(pointList->meanDiss);
       meanDissEdit->Refresh();
       pointList->readXUAPFileOld(i,false);
       FILE *fpp;
       AnsiString name;
       name="grid."+IntToStr(i);
       const char *filename;
       filename=name.c_str();
       fpp = fopen(filename,"w");
       pointList->doLinearInterp(fpp);
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
    if(1<2){
       pointList->makeCor();
    }
    pointList->reLink();
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
          //xuapForm->Series10->AddXY(numOnTrack,pointList->point[10][pointInd][2]*1000,'.',clTeeColor);
          //xuapForm->Series11->AddXY(numOnTrack,pointList->point[10][pointInd][3]*1000,'.',clTeeColor);
          xuapForm->Series12->AddXY(numOnTrack,pointList->point[10][pointInd][4]*1000,'.',clTeeColor);
          //xuapForm->Series1->AddXY(numOnTrack,pointList->point[10][pointInd][5]*1000,'.',clTeeColor);
          //xuapForm->Series2->AddXY(numOnTrack,pointList->point[10][pointInd][6]*1000,'.',clTeeColor);
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
           pointList->occ[i][j]=false;
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
       pointList->followTrajPointLinQuadforAccDeriv(fpp,startFile,startPoint,false);
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

    if(1>2){
       pointList->readWeights();
    }

    for(int i=pointList->firstFile;i<pointList->lastFile+1;i++){
       for(int j=0;j<1000;j++){
           pointList->occ[i][j]=false;
       }
    }

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

    for (int i=pointList->firstFile;i<pointList->lastFile+1;i++){
       fileNum2Edit->Text=IntToStr(i);
       mainForm->Refresh();
       FILE *fpp;
       AnsiString name;
       name="trajAcc."+IntToStr(i);
       const char *filename;
       filename=name.c_str();
       fpp = fopen(filename,"w");
       pointList->followTrajPointLinQuadforAccDeriv(fpp,i,0,true);
       fclose (fpp);
       mainForm->meanSuccessDivEdit->Text=IntToStr((int)(100.*(double)pointList->count2/(double)pointList->count+0.5));
       mainForm->meanSuccessAccEdit->Text=IntToStr((int)(100.*(double)pointList->count4/(double)pointList->count+0.5));
       mainForm->meanSuccessDivAEdit->Text=IntToStr((int)(100.*(double)pointList->count5/(double)pointList->count+0.5));
       ///curvature and grad kinetic energy stuff
       if(mainForm->interpolRadioGroup->ItemIndex==1){
          mainForm->meanSuccessCurvEdit->Text=IntToStr((int)(100.*(double)pointList->count7/(double)pointList->count+0.5));
          mainForm->meanSuccessGradKEdit->Text=IntToStr((int)(100.*(double)pointList->count8/(double)pointList->count+0.5));
       }
       ///end urvature and grad kinetic energy stuff
       mainForm->Refresh();
    }      
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
           pointList->occ[i][j]=false;
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
       pointList->followTrajPointLinQuadforAccDerivGrid(fpp,i,0,true);
       fclose (fpp);
       mainForm->meanSuccessDivEdit->Text=IntToStr((int)(100.*(double)pointList->count2/(double)pointList->count+0.5));
       mainForm->meanSuccessAccEdit->Text=IntToStr((int)(100.*(double)pointList->count4/(double)pointList->count+0.5));
       mainForm->meanSuccessDivAEdit->Text=IntToStr((int)(100.*(double)pointList->count5/(double)pointList->count+0.5));
       ///curvature and grad kinetic energy stuff
       if(mainForm->interpolRadioGroup->ItemIndex==1){
          mainForm->meanSuccessCurvEdit->Text=IntToStr((int)(100.*(double)pointList->count7/(double)pointList->count+0.5));
          mainForm->meanSuccessGradKEdit->Text=IntToStr((int)(100.*(double)pointList->count8/(double)pointList->count+0.5));
       }
       ///end urvature and grad kinetic energy stuff
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
    
    int filtSize=(int)(StrToFloat(mainForm->filtEdit->Text)/2);
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
          mainForm->meanSuccessDivEdit->Text=IntToStr((int)(100.*(double)pointList->count2/(double)pointList->count+0.5));
          mainForm->meanSuccessAccEdit->Text=IntToStr((int)(100.*(double)pointList->count4/(double)pointList->count+0.5));
          mainForm->meanSuccessDivAEdit->Text=IntToStr((int)(100.*(double)pointList->count5/(double)pointList->count+0.5));
          meanDissEdit->Text=IntToStr((int)(1.e6*pointList->meanDiss/(double)pointList->count2+0.5));
          Reynolds=(int)((pow(pointList->meanUisq/(double)pointList->count2,0.5)*pow(pointList->meanUisq/pointList->meanDudxsq,0.5))/viscosity+0.5);
          mainForm->reEdit->Text=IntToStr(Reynolds);
          ///curvature and grad kinetic energy stuff
          if(mainForm->interpolRadioGroup->ItemIndex==1){
             mainForm->meanSuccessCurvEdit->Text=IntToStr((int)(100.*(double)pointList->count7/(double)pointList->count+0.5));
             mainForm->meanSuccessGradKEdit->Text=IntToStr((int)(100.*(double)pointList->count8/(double)pointList->count+0.5));
          }
       }
       ///end urvature and grad kinetic energy stuff
       mainForm->Refresh();
    }        
}
//---------------------------------------------------------------------------

