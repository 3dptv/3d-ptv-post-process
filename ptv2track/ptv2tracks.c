/***************************************************************************
**
** Title: ptv2tracks
**
** Autor: Jochen Willneff
**
** Created: 25.05.2000
**
** Changes:
**
** Description:
**		
** follows tracks from ascii-file and writes each track in a file.
** This file is in a  subdirectory with name tracks/
**
** input : ptv2tracks <prefixfilname> <firstframe> <lastframe>
**
** output : tracks/________.track
**
** (___ = counter number)
**
** Output data includes the following:
**
** first place number of timesteps for this track
** 1. framenumber 
** 2, 3, 4 Coordinate vector in mm
** gcc ptv2tracks.c -o ptv2tracks
***************************************************************************/
#include	<stdio.h>
#include	<math.h>
#include 	<string.h>
#include 	<stdlib.h>
#define 	nmax 20000
#define		datei 100000

int main(int argc, char **argv)
{
  int	j, i_seq;
  int	firstframe, lastframe, file, vectors, dummy = 10000, minitrack=0, count[datei];
  int next[nmax], last[nmax], folderlast[nmax], foldernext[nmax];
  float xcor[nmax], ycor[nmax], zcor[nmax];
  
  FILE	*FILEOUT, *FILEIN;
  char	filein[100], fileout[100], prefix[50];
  
  if(argc != 4)
    {
      printf("\n Usage: ptv2tracks <fileprefix> <firstdatasetnum.> <lastdatasetnum.> !!!\n");
      exit(1);
    }
  strcpy (prefix, argv[1]);
  firstframe = atoi(argv[2]);
  lastframe  = atoi(argv[3]);
  
  file=0;
  
  for (j = 0; j < nmax; j++)
    foldernext[j] = -1;
  
  for (j = 0; j < datei; j++)
    count[j]=0;
  
  for (i_seq=firstframe; i_seq <= lastframe; i_seq++)
    {    
      if (i_seq < 10) sprintf (filein,"%s.00%d", prefix, i_seq);
      else if (i_seq < 100) sprintf (filein,"%s.0%d", prefix, i_seq);
      else  sprintf (filein,"%s.%d", prefix, i_seq);
      printf("filein: %s\n",  filein);     
      FILEIN = fopen (filein, "r");	if (! FILEIN) exit(1);
      fscanf(FILEIN, "%i", &vectors);
      for (j = 0; j < vectors; j++)
	{
	  fscanf(FILEIN, "%d %d %f %f %f\n", 
		 &last[j], &next[j], &xcor[j], &ycor[j], &zcor[j]);
	}
      
      fclose(FILEIN);
      for (j = 0; j < vectors; j++)
	{
	  if (last[j] < 0)
	    {
	      folderlast[j] = -1;
	    }
	  else
	    {
	      folderlast[j] = foldernext[j];
	    }
	}
     
      for (j = 0; j < vectors; j++)
	{
	  if (next[j] >= 0)
	    {
	      /*test if track is known or create new file*/
	      if (folderlast[j] == -1) {	
		sprintf (fileout,"tracks/%d.track", file);
		FILEOUT = fopen(fileout, "w");
		fprintf(FILEOUT, "\n%4i\n", dummy);
		foldernext[next[j]] = file;
		folderlast[j] = file;	
		fprintf(FILEOUT, "%4i %7.3f %7.3f %7.3f 0.0\n",
			i_seq, xcor[j] ,ycor[j] ,zcor[j]);
		fclose(FILEOUT);
		count[file]++;
		file++; 		
	      } 
	      else {		  
		sprintf (fileout,"tracks/%d.track", folderlast[j]);
		FILEOUT = fopen(fileout , "a");
		fprintf(FILEOUT, "%4i %7.3f %7.3f %7.3f 0.0\n",
			i_seq, xcor[j] ,ycor[j] ,zcor[j]);
		fclose(FILEOUT);
		count[folderlast[j]]++;
		foldernext[next[j]] = folderlast[j];
	      }
	    }	  
	  else if ((last[j] >= 0) && (folderlast[j] != -1))
	    {	
      	      sprintf (fileout,"tracks/%d.track", folderlast[j]);
	      FILEOUT = fopen(fileout , "a");
	      fprintf(FILEOUT, "%4i %7.3f %7.3f %7.3f 0.0\n",
		      i_seq, xcor[j] ,ycor[j] ,zcor[j]);
	      fclose(FILEOUT);
	      count[folderlast[j]]++;
	    }
	  else { minitrack++; }
	}
    }
  
  printf("There were %d tracks over only one timestep\n", minitrack); 
  
  for (j=0 ;j<file ; j++)
    {
      sprintf (fileout,"tracks/%d.track", j);
      FILEOUT = fopen(fileout , "r+");
      rewind(FILEOUT);
	fprintf(FILEOUT, "\n%4i\n", j);
	/*      fprintf(FILEOUT, "%6i", count[j]); 
printf("%5d %6i\n", j, count[j]); */
      fclose(FILEOUT);

      FILEOUT = fopen(fileout , "a");
	fprintf(FILEOUT, "-1\n");
      fclose(FILEOUT);
    }

  system("cat tracks/*.track > data.trk");
  sprintf (fileout,"data.trk");
  FILEOUT = fopen(fileout , "a");
  fprintf(FILEOUT, "-1\n");
  fclose(FILEOUT);
  
}



