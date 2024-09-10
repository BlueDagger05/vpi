#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "vpi_user.h"


/*****************************************************************
 * Function Prototypes                                           *
******************************************************************/
static void vpit_CheckFileExtension(PLI_BYTE8 *, PLI_BYTE8 *);


PLI_INT32 ReadVectorCalltf(PLI_BYTE8 *user_data),
          ReadVectorCompiletf(PLI_BYTE8 *user_data),
          ReadVectorStartOfSim(s_cb_data *callback_data);

static int vpit_CheckError(void)
{
   int error_code;
   s_vpi_error_info error_info;

   error_code = vpi_chk_error( &error_info );
   if(error_code && error_info.message) {
      vpi_printf(" %s \n", error_info.message);
   }
   return error_code;
}
/*****************************************************************
 * Structure definitions                                         *
 *                                                               *
 * Defines storage structure for file pointer and vector handle  *
 *                                                               *
 *****************************************************************/
typedef struct file_struct{
   FILE *file_ptr;               /* test vector file pointer       */
   vpiHandle obj_h;              /* pointer to store vector object */
} file_struct_s, *file_struct_p; /* structure access variables     */

/******************************************************************
 * FileCompiletf()                                                *
 *                                                                *
 * Validate the passed arguments and raise error                  *
 * if any otherwise perform the functionality                     *
 *                                                                *
 ******************************************************************/
PLI_INT32 ReadVectorCompiletf(PLI_BYTE8 *user_data)
{
   s_cb_data  callback_handle;
   vpiHandle  systf_h, arg_itr, arg_h, cb_h;
   PLI_INT32  tfarg_type;
   int        err_flag = 0;
   char       *file_name;

   systf_h = vpi_handle(vpiSysTfCall, NULL);
   arg_itr = vpi_iterate(vpiArgument, systf_h);
   if(arg_itr == NULL) {
      vpi_printf("ERROR :: $read_vector requires two arguments \n");
      vpi_control(vpiFinish, 1); /* abort simulation */
      return(0);
   }

   arg_h = vpi_scan(arg_itr);
   if(vpi_get(vpiType, arg_h) != vpiStringVal) {
      vpi_printf("read_vector arg1 must be a quoted file name \n");
      err_flag = 1;
   }
   else if(vpi_get(vpiType, arg_h) != vpiStringConst) {
      vpi_printf("read_vector arg2 must be a string \n");
      err_flag = 1;
   }

   arg_h = vpi_scan(arg_itr);
   tfarg_type = vpi_get(vpiType, arg_h);
   if(  (tfarg_type != vpiReg)        && 
        (tfarg_type != vpiIntegerVar) &&
        (tfarg_type != vpiTimeVar) ) {
      vpi_printf("read_test_vector arg2 must be a register type \n");
      err_flag = 1;
   }

   if(vpi_scan(arg_itr) != NULL) {
      vpi_printf("read_vector needs only two arguments \n");
      vpi_free_object(arg_itr);
      err_flag = 1;
   }

   if(err_flag == 1){
      vpi_printf("Encountered occured .. Check log files for debugging");
      vpi_control(vpiFinish, 1); /* Abort Simulation */
      return (0);
   }

   /* No syntax errors, setting up a callback for start of simulation */
   callback_handle.reason = cbStartOfSimulation;
   callback_handle.cb_rtn = ReadVectorStartOfSim;
   callback_handle.obj    = NULL;
   callback_handle.time   = NULL;
   callback_handle.value  = NULL;

   /* using user_data to pass systf_h to simulation callback so that the 
    * callback can access the system task arguments                   */
   callback_handle.user_data = (PLI_BYTE8 *)systf_h;
   cb_h = vpi_register_cb(&cb_handle);
}

/****************************************************
 * FileCalltf()                                     *
 *                                                  *
 * Perfoms the task/function as per user definition *
 *                                                  *
 ****************************************************/
PLI_INT32 FileCalltf(PLI_BYTE8 *user_data)   
{
   s_vpi_value value_s;
   vpiHandle systf_handle, arg_itr, arg_handle;
   PLI_BYTE8 *arg1, *arg2;
   char *string_keep1, *string_keep2;

   systf_handle = vpi_handle(vpiSysTfCall, NULL);
   arg_itr      = vpi_iterate(vpiArgument, systf_handle);
   if(arg_itr == NULL) {
      vpi_printf("ERROR : $file_o failed to obtain file arguments \n");
      return(0);
   }
   // Retrieving name of first handle
   arg_handle     = vpi_scan(arg_itr);
   value_s.format = vpiStringVal; 
   vpi_get_value(arg_handle, &value_s);
   arg1         = value_s.value.str; 

   string_keep1 = malloc(strlen((char*)arg1)+1);
   if(string_keep1 == NULL) {
      vpi_printf("ERROR : Failed to allocate memory for string_keep1");
      return(1);
   }
   strcpy(string_keep1, arg1);

   // Retrieving name of second handle
   arg_handle     = vpi_scan(arg_itr);
   value_s.format = vpiStringVal;
   vpi_get_value(arg_handle, &value_s);
   arg2           = value_s.value.str;

   string_keep2   = malloc(strlen((char*)arg2)+1);
   if(string_keep2 == NULL) {
      vpi_printf("ERROR : Failed to allocate memory for string_keep2");
      return(1);
   }
   strcpy(string_keep2, arg2);

   vpit_CheckFileExtension(string_keep1, string_keep2);

   //vpi_printf("FILE1 name : %s, FILE2 name : %s\n", string_keep1, string_keep2 );

   // Freeing allocated space
   free(string_keep1);
   free(string_keep2);

   // Cleaning VPI handle
   vpi_free_object(arg_itr);

   return(0);
}

/* Function definition for vpit_CheckFileExtension */
void vpit_CheckFileExtension(PLI_BYTE8 *arg1, PLI_BYTE8 *arg2)
{
   char *compare1 = strrchr(arg1, '.');
   char *compare2 = strrchr(arg2, '.');
   char str_compare[4] = ".txt";
   if ( strcmp(str_compare, compare1) == 0)  
      vpi_printf("valid extension for file1\n");
   else {
      vpi_printf("ERROR: Invalid extension... File1 must be .txt file\n");
      vpi_control(vpiFinish, 1);
   }
   if ( strcmp(str_compare, compare2) == 0)
      vpi_printf("valid extension for file2\n");
   else
   {
      vpi_printf("ERROR: Invalid extension... File2 must be .txt file\n");
      vpi_control(vpiFinish, 1);
   }
}

PLI_INT32 FileStartOfSim(s_cb_data *callback_data)
{
   vpi_printf("\n $file_o operation in use\n");
   return(0);
}


void vpit_RegisterTfs(void)
{
   s_vpi_systf_data systf_data;
   vpiHandle        callback_handle;
   s_cb_data        systf_cb_data;

   systf_data.type        = vpiSysTask;
   systf_data.sysfunctype = 0;
   systf_data.tfname      = "$read_vector";
   systf_data.calltf      = ReadVectorCalltf;
   systf_data.compiletf   = ReadVectorCompiletf;
   systf_data.sizetf      = 0;
   systf_data.user_data   = 0;

   vpi_register_systf(&systf_data);
   systf_cb_data.reason = cbStartOfSimulation;
   systf_cb_data.cb_rtn = FileStartOfSim;

   systf_cb_data.obj       = NULL;
   systf_cb_data.time      = NULL;
   systf_cb_data.value     = NULL;
   systf_cb_data.user_data = NULL;
   callback_handle         = vpi_register_cb( &systf_cb_data );

   vpi_free_object( callback_handle);
}
/****************************************************
 * Required Structure for initializing VPI routines *
 ****************************************************/

void (*vlog_startup_routines[]) () = {
   vpit_RegisterTfs,
   0
};
