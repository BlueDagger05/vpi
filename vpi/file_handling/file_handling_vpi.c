#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "vpi_user.h"


/***********************
 * Function Prototypes *
 ***********************/


PLI_INT32 FileCalltf(PLI_BYTE8 *user_data),
          FileCompiletf(PLI_BYTE8 *user_data),
          FileStartOfSim(s_cb_data *callback_data);

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
/*************************************************
 * FileCompiletf()                               *
 *                                               *
 * Validate the passed arguments and raise error *
 * if any otherwise perform the functionality    *
 *                                               *
 *************************************************/
PLI_INT32 FileCompiletf(PLI_BYTE8 *user_data)
{
   vpiHandle systf_handle, arg_itr, arg_handle;

   /* valid files are text files (.txt) */
   PLI_INT32 tfarg_type;

   int err_flag = 0;

   do {
      systf_handle = vpi_handle(vpiArgument, systf_handle);
      tfarg_type   = vpi_get(vpiType, arg_handle);
      arg_handle   = vpi_scan(arg_itr);
      if(arg_itr == NULL) {
         vpi_printf("ERROR: $file_op requires input and output file, has none\n");
         err_flag = 1;
         break;
      }
   }while(0==1);
   if(err_flag == 1) {
      vpi_control(vpiFinish, 1);
      vpi_printf("Error incurred");
   }
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

   vpi_printf("FILE1 name : %s, FILE2 name : %s\n", string_keep1, string_keep2 );
   return(0);

   // Freeing allocated space
   free(string_keep1);
   free(string_keep2);

   // Cleaning VPI handle
   vpi_free_object(arg_itr);
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
   systf_data.tfname      = "$file_op";
   systf_data.calltf      = FileCalltf;
   systf_data.compiletf   = FileCompiletf;
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
