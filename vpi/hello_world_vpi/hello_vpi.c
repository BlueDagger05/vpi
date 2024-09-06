#include <stdio.h>
#include <stdlib.h>
#include "vpi_user.h"

// Register function, defined in vpi_user.h
// typedef struct t_vpi_systf_data {
//   PLI_INT32 type;
//   PLI_INT32 sysfunctype;
  
//   PLI_BYTE8 *tfname;
  
//   PLI_INT32(*calltf) (PLI_BYTE8 *);
//   PLI_INT32(*compiletf)(PLI_BYTE8 *);
//   PLI_INT32(*sizetf) (PLI_BYTE8 *);
//   PLI_BYTE8 *user_data;
// } s_vpi_systf_data, *p_vpi_systf_data;

int hello (char* user_data)
{
  vpi_printf("\nHello Badhssh World\n");
  return 0;
}

void my_func()
{
  s_vpi_systf_data tf_data;
  p_vpi_systf_data task_data_p = &tf_data;
  tf_data.type         = vpiSysTask;
  tf_data.sysfunctype  = 0;
  tf_data.tfname       = "$hello";
  tf_data.calltf       = (int(*)()) hello;
  tf_data.compiletf    = NULL;
  tf_data.sizetf       = NULL;
  tf_data.user_data    = NULL;
  vpiHandle reg = vpi_register_systf(task_data_p);
  
  if(reg == NULL){
    printf("Error Registering system function \n");
  }
  else
  {
    printf("System function registerd");
  }
    
  vpi_register_systf(&tf_data);
}

void (*vlog_startup_routines[])() =
{
  my_func,
  NULL
};
