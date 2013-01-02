// hdftest.c : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include <direct.h>
#include <stdlib.h>
#include "hdf.h"
// #include "vg.h"
#include "mfhdf.h"


#define FILENAME "trajPoint.hdf"

#define  ORDER            1        /* number of values in the field         */
#define  N_RECORDS        1       /* number of records the vdata contains, 1 for writing in loop  */
#define  N_FIELDS         32        /* number of fields in the vdata         */
#define  NUM_TRAJECTORIES 10
/* FIELD NAMEs */
#define FIELD_NAME1		"x"
#define	FIELD_NAME2		"y"
#define FIELD_NAME3		"z"
#define FIELD_NAME4		"u"
#define FIELD_NAME5		"v"
#define FIELD_NAME6		"w"
#define FIELD_NAME7		"ax"
#define FIELD_NAME8		"ay"
#define FIELD_NAME9		"az"
#define FIELD_NAME10	"w1"
#define FIELD_NAME11	"w2"
#define FIELD_NAME12	"w3"
#define FIELD_NAME13	"s11"
#define FIELD_NAME14	"s12"
#define FIELD_NAME15	"s13"
#define FIELD_NAME16	"s22"
#define FIELD_NAME17	"s23"
#define FIELD_NAME18	"s33"
#define FIELD_NAME19	"ww1"
#define FIELD_NAME20	"ww2"
#define FIELD_NAME21	"ww3"
#define FIELD_NAME22	"wws"
#define FIELD_NAME23	"sss"
#define FIELD_NAME24	"R"
#define FIELD_NAME25	"Q"
#define FIELD_NAME26	"diss"
#define FIELD_NAME27	"div"
#define FIELD_NAME28	"t"
#define FIELD_NAME29 	"age"
#define FIELD_NAME30	"alx"
#define FIELD_NAME31	"aly"
#define FIELD_NAME32	"alz" 

#define  FIELDNAME_LIST   "x,y,z,u,v,w,ax,ay,az,w1,w2,w3,s11,s12,s13,s22,s23,s33,ww1,ww2,ww3,wws,sss,R,Q,diss,div,t,age,alx,aly,alz"  
/* No spaces b/w names */

/* number of values per record */
/* #define  N_VALS_PER_REC   (ORDER_1 + ORDER_2 + ORDER_3) */

#define VDATA_CLASS "Trajectory"


main()
{

	int32		file_id, vdata_id, istat, sd_id;
	int32       vdata_ref = -1;     /* ref number of a vdata, set to -1 to create  */
	int32		offset = 0;			/* later on we will add the option for mutliple files */
	intn		status_n;
	int32		status_32, rec_num, num_of_records, nbytes;
	char		vdata_name[9];		/* we do not expect have more than billion trajectories */
		
	time_t		now;
	char		filename[_MAX_PATH], buffer[_MAX_PATH];

	int32		trajectory = 0;		/* dummy counter for trajectories numbering */
	float32  data_buf[N_RECORDS][N_FIELDS]; /* buffer for vdata values */




	// This part should run once during the post-processing
	// The HDF file is created or gives an error if one already exists
	// Add some atributes to the file: Date, Name of the author, Directory,
	// then you can never mix up different files, always check for the details
	// in the file header.
	// ------------------------------------------------------------------------

	/* Check if the HDF file exists */
	status_n = Hishdf(FILENAME);

	if (status_n == TRUE)
{
	printf("File exists, change file name.\n");
	/* exit (-1);  */
}
     /* Open the HDF file. */
     file_id =  Hopen(FILENAME, DFACC_CREATE, 0);

	/* Initialize the Vset interface. */
     istat = Vstart(file_id);
	 if (istat == FAIL) {
		printf ("Vstart failed.\n");
		HEprint(stdout,0);
		exit (-1);
	}


	/* initialize the SD interface */
	sd_id = SDstart(FILENAME, DFACC_WRITE);
	if (sd_id == FAIL) {
		printf ("SDstart write failed.\n");
		HEprint(stdout,0);
		exit (-1);
	}
	else
		printf ("\n... hdf file opened for WRITE access\n");

	/* Set an attribute that describes the file contents. */
	now = time(NULL); /* initialize time switch */
	istat = SDsetattr(sd_id, "Creation time", DFNT_CHAR8, 16, (VOIDP)asctime(localtime(&now)));
	if (istat == FAIL) {
		printf ("SDsetattr failed.\n"); 
		HEprint(stdout,0);
		exit (-1);
	}
	/* List author */
	istat = SDsetattr(sd_id, "Author", DFNT_CHAR8, 16, (VOIDP)"Alex Liberzon");
	if (istat == FAIL) {
		printf ("SDsetattr failed.\n"); 
		HEprint(stdout,0);
		exit (-1);
	}

	/* Get the current working directory: */
   if( _getcwd( buffer, _MAX_PATH ) == NULL )
      perror( "_getcwd error" );
   else
   {
   /* List current directory */
	istat = SDsetattr(sd_id, "Dataset", DFNT_CHAR8, 16, (VOIDP)buffer);
	if (istat == FAIL) {
		printf ("SDsetattr failed.\n"); 
		HEprint(stdout,0);
		exit (-1);
	} /* if istat */
   }  /* if getwcd */

   // --------------------------------------------------------
   // This part should run for each trajectory.
   // 

   for (trajectory = 0; trajectory < NUM_TRAJECTORIES; trajectory++)
   {
   /*  Create a vdata */
   vdata_id = VSattach (file_id, vdata_ref, "w");
   if (vdata_id == FAIL) {
		printf ("VSattach failed.\n"); 
		HEprint(stdout,0);
		exit (-1);
	} /* if vdata_id */

   /* 
   * Later on we should add the option for the external multiple files
   *
   */
/************************************************************************************/
   /* In a real file, trajectory has to be replaced with the number of the file */
   /*****************************************************************************/

   sprintf(filename,"%s.%d",FILENAME,trajectory);


   status_32 = VSsetexternalfile(vdata_id,filename,offset);
   if (status_32 == FAIL){
        printf ("VSsetexternalfile failed.\n"); 
		HEprint(stdout,0);
		exit (-1);
   }   

   /*
   * Assign a name to the vdata.
   */
   // Each vdata will have its own name, the number of trajectory
   // in this demo we will run some sequential numbering
   
   sprintf(vdata_name,"%u",trajectory);
   status_32 = VSsetname (vdata_id, vdata_name);
   if (status_32 == FAIL) {
		printf ("VSsetname failed.\n"); 
		HEprint(stdout,0);
		exit (-1);
	} /* if status_32 */

   /*
   * Assign a class name to the vdata.
   */
   status_32 = VSsetclass (vdata_id, VDATA_CLASS);
	if (status_32 == FAIL) {
		printf ("VSsetclass failed.\n"); 
		HEprint(stdout,0);
		exit (-1);
	} /* if status_32 */


	/* 
   * Introduce each field's name, data type, and order.  This is the first
   * part in defining a vdata field.
   */


	status_n = VSfdefine (vdata_id, FIELD_NAME1, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME2, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME3, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME4, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME5, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME6, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME7, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME8, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME9, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME10, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME11, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME12, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME13, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME14, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME15, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME16, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME17, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME18, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME19, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME20, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME21, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME22, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME23, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME24, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME25, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME26, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME27, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME28, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME29, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME30, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME31, DFNT_FLOAT32, ORDER);
	status_n = VSfdefine (vdata_id, FIELD_NAME32, DFNT_FLOAT32, ORDER);

	/* status_n = VSfdefine (vdata_id, FIELD2_NAME, DFNT_INT16, ORDER); 
   status_n = VSfdefine (vdata_id, FIELD3_NAME, DFNT_FLOAT32, ORDER); 
   status_n = VSfdefine (vdata_id, FIELD4_NAME, DFNT_CHAR8, ORDER); */

	/*
   * Finalize the definition of the fields of the vdata.
   */
   status_n = VSsetfields (vdata_id, FIELDNAME_LIST);

/* 
   * Buffer the data by the record for fully interlaced mode.  Note that the
   * first three elements contain the three values of the first field, the
   * fourth element contains the value of the second field, and the last two
   * elements contain the two values of the third field.
   */
   for (rec_num = 0; rec_num < N_RECORDS; rec_num++)
   {
	   /* here it will appear like */ 
	   /*
				  data_buf[rec_num][0]  = xp[ii];
                  data_buf[rec_num][1]  = yp[ii];
                  data_buf[rec_num][2]  = zp[ii];
                  data_buf[rec_num][3]  = up[ii];
                  data_buf[rec_num][4]  = vp[ii];
                  data_buf[rec_num][5]  = wp[ii];
                  data_buf[rec_num][6]  = axp[ii];
                  data_buf[rec_num][7]  = ayp[ii];
                  data_buf[rec_num][8]  = azp[ii];
                  data_buf[rec_num][9]  = w1p[ii];
                  data_buf[rec_num][10] = w2p[ii];
                  data_buf[rec_num][11] = w3p[ii];
                  data_buf[rec_num][12] = s11p[ii];
                  data_buf[rec_num][13] = s12p[ii];
                  data_buf[rec_num][14] = s13p[ii];
                  data_buf[rec_num][15] = s22p[ii];
                  data_buf[rec_num][16] = s23p[ii];
                  data_buf[rec_num][17] = s33p[ii];
                  data_buf[rec_num][18] = ww1;
                  data_buf[rec_num][19] = ww2;
                  data_buf[rec_num][20] = ww3;
                  data_buf[rec_num][21] = wwsij;
                  data_buf[rec_num][22] = sijsjkski;
                  data_buf[rec_num][23] = R;
                  data_buf[rec_num][24] = Q;
                  data_buf[rec_num][25] = 1.e6*diss;
                  data_buf[rec_num][26] = div/ref;
                  data_buf[rec_num][27] = (double)startP;
                  data_buf[rec_num][28] = (double)(ii);
                  data_buf[rec_num][29] = utp[ii];
                  data_buf[rec_num][30] = vtp[ii];
                  data_buf[rec_num][31] = wtp[ii];
		*/
				  data_buf[rec_num][0]  = 1.0*trajectory;
                  data_buf[rec_num][1]  = 2.0*trajectory;
                  data_buf[rec_num][2]  = 3.0*trajectory;
                  data_buf[rec_num][3]  = 4.0*trajectory;
                  data_buf[rec_num][4]  = 5.0*trajectory;
                  data_buf[rec_num][5]  = 6.0*trajectory;
                  data_buf[rec_num][6]  = 7.0*trajectory;
                  data_buf[rec_num][7]  = 8.0*trajectory;
                  data_buf[rec_num][8]  = 9.0*trajectory;
                  data_buf[rec_num][9]  = 10.0*trajectory;
                  data_buf[rec_num][10] = 11.0*trajectory;
                  data_buf[rec_num][11] = 12.0*trajectory;
                  data_buf[rec_num][12] = 13.0*trajectory;
                  data_buf[rec_num][13] = 14.0*trajectory;
                  data_buf[rec_num][14] = 15.0*trajectory;
                  data_buf[rec_num][15] = 16.0*trajectory;
                  data_buf[rec_num][16] = 17.0*trajectory;
                  data_buf[rec_num][17] = 18.0*trajectory;
                  data_buf[rec_num][18] = 19.0*trajectory;
                  data_buf[rec_num][19] = 20.0*trajectory;
                  data_buf[rec_num][20] = 21.0*trajectory;
                  data_buf[rec_num][21] = 22.0*trajectory;
                  data_buf[rec_num][22] = 23.0*trajectory;
                  data_buf[rec_num][23] = 24.0*trajectory;
                  data_buf[rec_num][24] = 25.0*trajectory;
                  data_buf[rec_num][25] = 26.0*trajectory;
                  data_buf[rec_num][26] = 27.0*trajectory;
                  data_buf[rec_num][27] = 28.0*trajectory;
                  data_buf[rec_num][28] = 29.0*trajectory;
                  data_buf[rec_num][29] = 30.0*trajectory;
                  data_buf[rec_num][30] = 31.0*trajectory;
                  data_buf[rec_num][31] = 32.0*trajectory;
   }

   /* 
   * Write the data from data_buf to the vdata with full interlacing mode.
   */
   num_of_records = VSwrite (vdata_id, (uint8 *)data_buf, N_RECORDS, 
                             FULL_INTERLACE);
   
   /* Offset for the multiple external files */ 

   nbytes = VSsizeof(vdata_id, FIELDNAME_LIST);
        
   offset = offset + nbytes*num_of_records; // *128;
   fprintf(stdout,"\n%d\n",offset);



    /* 
     * Terminate access to the Vdata 
     */
	 
	   status_32 = VSdetach (vdata_id);
   if (status_32 == FAIL) {
		printf ("VSdetach failed.\n");
		HEprint(stdout,0);
		exit (-1);
	}	
	else
		printf("... Vdata detached \n");

    } /* end of for (trajectory )

   /* Terminate access to the SD interface. */
	istat = SDend(sd_id);
	if (istat == FAIL) {
		printf ("SDend failed.\n");
		HEprint(stdout,0);
		exit (-1);
	}	
	else
		printf("... file closed\n\n");

/* End access to the VData interface */
	status_n  = Vend (file_id);
   if (status_n == FAIL) {
		printf ("Vend failed.\n");
		HEprint(stdout,0);
		exit (-1);
	}	
	else
		printf("... Vdata acess ended \n");

/* Close the file */
     istat = Hclose(file_id);
	 if (istat == FAIL) {
		printf ("Hclose failed.\n");
		HEprint(stdout,0);
		exit (-1);
	}	
	else
		printf("... file closed\n\n");



	// ***************************************************
	// Reading from the HDF file
	// ***************************************************

	/* 
   * Open the HDF file for reading.
   */
   file_id = Hopen (FILENAME, DFACC_READ, 0);

   /* 
   * Initialize the VS interface.
   */
   status_n = Vstart (file_id);

   /*
   * Get the reference number of the vdata, whose name is specified in 
   * VDATA_NAME, using VSfind, which will be discussed in Section 4.7.3.
   */
   for (trajectory = 0; trajectory < NUM_TRAJECTORIES; trajectory++)
   {
	sprintf(vdata_name,"%u",trajectory);
    vdata_ref = VSfind (file_id, vdata_name);

   /* 
   * Attach to the vdata for reading if it is found, otherwise 
   * exit the program.
   */
   if (vdata_ref == 0) exit(-1);
   vdata_id = VSattach (file_id, vdata_ref, "r");

   /* 
   * Specify the fields that will be read.
   */
   status_n = VSsetfields (vdata_id, FIELDNAME_LIST);

   /*
   * Place the current point to the position specified in RECORD_INDEX.
   */
   // record_pos = VSseek (vdata_id, RECORD_INDEX);

   /* 
   * Read the next N_RECORDS records from the vdata and store the data 
   * in the buffer data_buf with fully interlaced mode.
   */
   num_of_records = VSread (vdata_id, (uint8 *)data_buf, N_RECORDS, 
                            FULL_INTERLACE);

   /*
   * Display the read data as many records as the number of records 
   * returned by VSread.
   */
   // printf ("\n       Particle Position        Temperature Range\n\n");
   for (rec_num = 0; rec_num < N_RECORDS; rec_num++)
   {
      printf ("   %6.2f, %6.2f, %6.2f        %6.2f, %6.2f\n", 
        data_buf[rec_num][0], data_buf[rec_num][1], data_buf[rec_num][2], 
        data_buf[rec_num][3], data_buf[rec_num][4]);
	  printf ("   %6.2f, %6.2f, %6.2f        %6.2f, %6.2f\n", 
        data_buf[rec_num][5], data_buf[rec_num][6], data_buf[rec_num][7], 
        data_buf[rec_num][8], data_buf[rec_num][9]);
	  printf ("   %6.2f, %6.2f, %6.2f        %6.2f, %6.2f\n", 
        data_buf[rec_num][10], data_buf[rec_num][11], data_buf[rec_num][12], 
        data_buf[rec_num][13], data_buf[rec_num][14]);
	  printf ("   %6.2f, %6.2f, %6.2f        %6.2f, %6.2f\n", 
        data_buf[rec_num][15], data_buf[rec_num][16], data_buf[rec_num][17], 
        data_buf[rec_num][18], data_buf[rec_num][19]);
	  printf ("   %6.2f, %6.2f, %6.2f        %6.2f, %6.2f\n", 
        data_buf[rec_num][20], data_buf[rec_num][21], data_buf[rec_num][22], 
        data_buf[rec_num][23], data_buf[rec_num][24]);
	  printf ("   %6.2f, %6.2f, %6.2f        %6.2f, %6.2f\n", 
        data_buf[rec_num][25], data_buf[rec_num][26], data_buf[rec_num][27], 
        data_buf[rec_num][28], data_buf[rec_num][29]);
	  printf ("   %6.2f, %6.2f \n", 
        data_buf[rec_num][30], data_buf[rec_num][31]);

   }

   /* 
   * Terminate access to the vdata and to the VS interface, then close 
   * the HDF file.
   */
   status_32 = VSdetach (vdata_id);
   }
   status_n = Vend (file_id);
   status_32 = Hclose (file_id);

	return 0;
}