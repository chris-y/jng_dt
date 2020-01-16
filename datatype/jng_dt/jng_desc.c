#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/datatypes.h>
#include <proto/utility.h>

/* we don't have our class library base yet, therefore we use the resources from the given DTHookContext */
#undef SysBase
#undef DOSBase
#undef UtilityBase
#define SysBase     (dthc -> dthc_SysBase)
#define UtilityBase (dthc -> dthc_UtilityBase)
#define DOSBase     (dthc -> dthc_DOSBase)

#undef IExec
#undef IDOS
#undef IUtility
#define IExec     (dthc -> dthc_IExec)
#define IUtility (dthc -> dthc_IUtility)
#define IDOS     (dthc -> dthc_IDOS)

BOOL _start( struct DTHookContext *dthc )  // DTHook
{
	if(IUtility->Strnicmp(dthc->dthc_Buffer,"\213JNG\r\n\032\n",8) == 0) return(TRUE);

    return(FALSE);
}
