#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "vpi_user.h"

// PLI_INT32 is the return type of the below mentioned functions
PLI_INT32 PowSizetf(PLI_BYTE8 *user_data),         // user_data is a pointer of type PLI_BYTE8 passed as argument
          PowCalltf(PLI_BYTE8 *user_data),
          PowCompiletf(PLI_BYTE8 *user_data),
          PowStartOfSim(s_cb_data *callback_data); // callback_data is a pointer of type s_cb_data passed as argument

void pow_register()
{
   s_vpi_systf_data tf_data;         // s_vpi_systf_data is a structure defined in vpi_user
                                     // and tf_data is its handle
   s_cb_data        cb_data_s;       // s_cb_data is a normal callback structure, handle is cb_data_s
   vpiHandle        callback_handle; // vpiHandle is a datatype which stores pointers to information about objects

   tf_data.type        = vpiSysFunc;
   tf_data.sysfunctype = vpiSysFuncSized;
   tf_data.tfname      = "$pow_x";
   tf_data.calltf      = PowCalltf;
   tf_data.compiletf   = PowCompiletf;
   tf_data.sizetf      = PowSizetf;
   tf_data.user_data   = NULL;

   // vpi_register_systf is a VPI function used to register
   // user-defined function/task with the simulator
   vpi_register_systf(&tf_data); // tf_data passes address of s_vpi_systf_data to the vpi_register_systf function

   cb_data_s.reason    = cbStartOfSimulation; // Defining reason as cbStartOfSimulation
   cb_data_s.cb_rtn    = PowStartOfSim;       // PowStartOfSim will be the pointer of size PLI_INT32 pointing
                                              // to a function which accepts s_cb_data structure
   cb_data_s.obj       = NULL; // Object field pointing to NULL
   cb_data_s.time      = NULL; // time field pointing to NULL
   cb_data_s.value     = NULL; // value field pointing to NULL
   cb_data_s.user_data = NULL; // user_data field pointing to NULL
   callback_handle     = vpi_register_cb(&cb_data_s);

   // frees the memory allocated to callback_handle
   vpi_free_object(callback_handle);
}

PLI_INT32 PowSizetf(PLI_BYTE8 *user_data)
{
   return(32); // returns 32-bit values
}

// compiletf application to verify valid systf args
PLI_INT32 PowCompiletf(PLI_BYTE8 *user_data) // takes argument a pointer user_data which points to variable of
                                             // type PLI_BYTE8
{
   // creating handles of type vpiHandle
   vpiHandle systf_handle, arg_itr, arg_handle;

   // variable of size PLI_INT32 for checking argument type
   PLI_INT32 tfarg_type;

   // flag for raising error
   int       err_flag = 0;

   do{
      // retrieves the handle of vpiArgument and stores it into systf_handle
      systf_handle = vpi_handle(vpiArgument, systf_handle);
      tfarg_type = vpi_get(vpiType, arg_handle);
      arg_handle = vpi_scan(arg_itr); // arg_itr is an iterator used to iterate over arguments
      if(arg_itr == NULL)
      {
         vpi_printf("ERROR: $pow requires two arguments; has none\n");
         err_flag = 1;
         break;
      }
      if( (tfarg_type != vpiReg)        && 
          (tfarg_type != vpiIntegerVar) &&
          (tfarg_type != vpiConstant) ) 
      {
         vpi_printf("ERROR: $pow arg1 must be number, variable or net\n");
         err_flag = 0; // Need to check
         break;
      }
      //  Retrieves the next VPI object
      // Checks for the presence of next argument
      // if the vpi_scan returns zero, argument is missing
      if(arg_handle == NULL)
      {
         vpi_printf("ERROR: $pow requires 2nd argument\n");
         err_flag = 1;
         break;
      }

      // retrieves type of argument passed to the function
      // calls the arg_handle which calls iterator
      if(   (tfarg_type != vpiReg)        &&
            (tfarg_type != vpiIntegerVar) &&
            (tfarg_type != vpiConstant) )
      {
         vpi_printf("ERROR: $pow arg2 must be number, variable or net\n ");
         err_flag = 1;
         break;
      }

      // raises error if iterator returns null after two iterations
      if(vpi_scan(arg_itr) != NULL) {
         vpi_printf("ERROR: $pow requires two arguments; has too many \n");
         vpi_free_object(arg_itr);
         err_flag = 1;
         break;
      }
   } while (0 == 1);
   if(err_flag){
      vpi_control(vpiFinish, 1); // Aborting the simulation
      vpi_printf("Aborting the simulation");
   }
   return(0);
}

PLI_INT32 PowCalltf(PLI_BYTE8 *user_data)
{
   s_vpi_value value_s;
   vpiHandle systf_handle, arg_itr, arg_handle;
   PLI_INT32 base, exp;
   double result;

   systf_handle = vpi_handle(vpiSysTfCall, NULL);
   arg_itr = vpi_iterate(vpiArgument, systf_handle);
   if(arg_itr == NULL) {
      vpi_printf("ERROR: $pow failed to obtain systf arg handles\n");
      return(0);
   }

   arg_handle = vpi_scan(arg_itr);
   // feteches value in int format
   value_s.format = vpiIntVal;
   // retrieves value of arg_handle pointed by value_s
   vpi_get_value(arg_handle, &value_s);
   base = value_s.value.integer;

   arg_handle = vpi_scan(arg_itr);
   vpi_free_object(arg_itr);
   vpi_get_value(arg_handle, &value_s);
   exp = value_s.value.integer;

   result = pow((double)base, (double)exp );

   // converting it into 32-bit value
   value_s.value.integer = (PLI_INT32)result;

   vpi_put_value(systf_handle, &value_s, NULL, vpiNoDelay);
   return(0);
}

PLI_INT32 PowStartOfSim(s_cb_data *callback_data)
{
   vpi_printf("\n $pow PLI application is being used. \n\n");
   return(0);
}

void (*vlog_startup_routines[])() = 
{
   pow_register,
   NULL
};
