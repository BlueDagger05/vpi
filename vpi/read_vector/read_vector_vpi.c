#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "vpi_user.h"


/*****************************************************************
 * Function Prototypes                                           *
******************************************************************/
//static void vpit_CheckFileExtension(PLI_BYTE8 *, PLI_BYTE8 *);
//
//
PLI_INT32 ReadVectorCalltf(PLI_BYTE8 *user_data),
          ReadVectorCompiletf(PLI_BYTE8 *user_data),
          ReadVectorStartOfSim(s_cb_data *cb_data_s);

//static int vpit_CheckError(void)
//{
//   int error_code;
//   s_vpi_error_info error_info;
//
//   error_code = vpi_chk_error( &error_info );
//   if(error_code && error_info.message) {
//      vpi_printf(" %s \n", error_info.message);
//   }
//   return error_code;
//}
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

/****************************************************************************
 * VPI Registration Data
****************************************************************************/
void vpit_RegisterTfs()
{
   s_vpi_systf_data tf_data;

   tf_data.type        = vpiSysTask;
   tf_data.sysfunctype = 0;
   tf_data.tfname      = "$read_vector";
   tf_data.calltf      = ReadVectorCalltf;
   tf_data.compiletf   = ReadVectorCompiletf;
   tf_data.sizetf      = 0;
   tf_data.user_data   = 0;

   vpi_register_systf(&tf_data);
}
/******************************************************************
 * FileCompiletf()                                                *
 *                                                                *
 * Validate the passed arguments and raise error                  *
 * if any otherwise perform the functionality                     *
 *                                                                *
 ******************************************************************/
PLI_INT32 ReadVectorCompiletf(PLI_BYTE8 *user_data)
{
   s_cb_data  cb_data_s;
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
   if(vpi_get(vpiType, arg_h) != vpiConstant) {
      vpi_printf("read_vector arg1 must be a quoted file name \n");
      err_flag = 1;
   }
   else if(vpi_get(vpiConstType, arg_h) != vpiStringConst) {
      vpi_printf("read_vector arg1 must be a string \n");
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

   if(err_flag){
      vpi_printf("Encountered occured .. Check log files for debugging");
      vpi_control(vpiFinish, 1); /* Abort Simulation */
      return (0);
   }

   /* No syntax errors, setting up a callback for start of simulation */
   /* setup a callback for start of simulation */
   cb_data_s.reason          = cbStartOfSimulation;
   cb_data_s.cb_rtn          = ReadVectorStartOfSim;
   cb_data_s.obj             = NULL;
   cb_data_s.time            = NULL;
   cb_data_s.value           = NULL;
   cb_data_s.user_data       = (PLI_BYTE8 *)systf_h; /* pass systf_h */
   cb_h                      = vpi_register_cb(&cb_data_s);
   vpi_free_object(cb_h);                            /* dont need a callback handle */
   return(0);
}

/******************************************************************
 * StartOfSim() Callback -- opens the test vector file and saves
 * the pointer and the system task handle in the work area storage
 *                                                  
 * Routine which gets invoked before 0 sim time     
 *                                                  
 ******************************************************************/
PLI_INT32 ReadVectorStartOfSim(p_cb_data cb_data)
{
   s_vpi_value    argVal;
   char           *file_name;
   FILE           *vector_file;
   vpiHandle      systf_h, arg_itr, arg1_h, arg2_h;

   file_struct_p vector_data; /* Pointer to a ReadVecData structure */
   vector_data  = (file_struct_p)malloc(sizeof(file_struct_s));

   /* retrieving system task handle from user_data */
   systf_h = (vpiHandle)cb_data->user_data;

   /* get argument handles */
   arg_itr = vpi_iterate(vpiArgument, systf_h);
   arg1_h  = vpi_scan(arg_itr);
   arg2_h  = vpi_scan(arg_itr);
   vpi_free_object(arg_itr);

   /* read file name from first tfarg*/
   argVal.format = vpiStringVal;
   vpi_get_value(arg1_h, &argVal);
   if(vpi_chk_error(NULL)) {
      vpi_printf("ERROR: $read_vector could not get file name\n");
      vpi_control(vpiFinish, 1); /*abort simulation */
      return(0);
   }
   file_name = argVal.value.str;
   if(!(vector_file = fopen(file_name, "r"))) {
      vpi_printf("$read_vector could not open file %s \n", file_name);
      vpi_control(vpiFinish, 1); /*abort simulation */
      return(0);
   }

   /* store file pointer and tfarg2_h in work area for this instance */
   vector_data->file_ptr = vector_file;
   vector_data->obj_h    = arg2_h;
   vpi_put_userdata(systf_h, (PLI_BYTE8 *)vector_data);

   return(0);
}
/****************************************************
 * ReadVectorCalltf()                               *
 *                                                  *
 * Perfoms the task/function as per user definition *
 *                                                  *
 ****************************************************/
PLI_INT32 ReadVectorCalltf(PLI_BYTE8 *user_data)   
{
   s_cb_data       data_s;
   s_vpi_time      time_s;
   s_vpi_value     value_s;
   FILE           *vector_file;
   vpiHandle       systf_h, arg2_h;
   file_struct_p   vector_data;   /* pointer to a file_struct structure */
   char            vector[1024];  /* could use malloc                   */

   systf_h     = vpi_handle(vpiSysTfCall, NULL);
   vector_data = (file_struct_p)vpi_get_userdata(systf_h);

   /* ger ReadVecData pointer from work area for this task instance   */
   /* the data in the work area was loaded at the start of simulation */
   vector_file = vector_data->file_ptr;
   arg2_h      = vector_data->obj_h;

   /* read next line from the file */
   if( (fscanf(vector_file, "%s\n", vector)) == EOF) {
      vpi_printf("$read_vector reached End-Of-File\n");
      fclose(vector_data->file_ptr);
      vpi_control(vpiFinish,1);
      return(0);
   }

   /* write the vector to the second system task argument */
   value_s.format    = vpiHexStrVal;
   value_s.value.str = vector;
   vpi_put_value(arg2_h, &value_s, NULL, vpiNoDelay);
   vpi_printf("FROM VPI :: input = %s\n", value_s.value.str);

   return(0);
}



/****************************************************
 * Required Structure for initializing VPI routines *
 ****************************************************/

void (*vlog_startup_routines[]) () = {
   vpit_RegisterTfs,
   0
};
