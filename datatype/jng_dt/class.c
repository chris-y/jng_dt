/*
 * pnm.datatype - Chris Young
 */

#include "class.h"
#include <libmng.h>

#define RGB8to32(RGB) (((uint32)(RGB) << 24)|((uint32)(RGB) << 16)|((uint32)(RGB) << 8)|((uint32)(RGB)))

mng_ptr mymng_memalloc(mng_size_t len)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	return(IExec->AllocVec(len,MEMF_CLEAR));
}

void mymng_memfree(mng_ptr ptr,mng_size_t len)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	IExec->FreeVec(ptr);
}

mng_bool mymng_openstream(mng_handle mng)
{
	return MNG_TRUE;
}

mng_bool mymng_closestream(mng_handle mng)
{
	return MNG_TRUE;
}

mng_bool mymng_readdata(mng_handle mng,mng_ptr buf,mng_uint32 len,mng_uint32p pread)
{
	mngstuff	*mymng;
	struct ClassBase *libBase;
	Class *cl;

    mymng = (mngstuff*)mng_get_userdata(mng);

	cl = mymng->cl;
	libBase = (struct ClassBase *)cl->cl_UserData;

	*pread = IDOS->Read(mymng->file,buf,len);

	return MNG_TRUE;
}

mng_bool mymng_processheader(mng_handle mng,mng_uint32 width,mng_uint32 height)
{
	mngstuff	*mymng;
	struct ClassBase *libBase;
	Class *cl;
	struct ExecIFace *IExec;

    mymng = (mngstuff*)mng_get_userdata(mng);

	cl = mymng->cl;
	libBase = (struct ClassBase *)cl->cl_UserData;
	IExec=libBase->IExec;

	mymng->height = height;
	mymng->width = width;

    mymng->bitmap = (UBYTE *)IExec->AllocVec((mymng->height)*(mymng->width)*4,MEMF_CLEAR);
	if(!mymng->bitmap) return MNG_FALSE;

	mng_set_canvasstyle(mng, MNG_CANVAS_ARGB8);

	return MNG_TRUE;
}

mng_ptr mymng_getcanvasline(mng_handle mng,mng_uint32 line)
{
	mngstuff	*mymng;
	struct ClassBase *libBase;
	Class *cl;

    mymng = (mngstuff*)mng_get_userdata(mng);

	cl = mymng->cl;
	libBase = (struct ClassBase *)cl->cl_UserData;

	return(mymng->bitmap+((mymng->width)*4*line));
}

mng_bool mymng_refresh(mng_handle mng,mng_uint32 x,mng_uint32 y,mng_uint32 width,mng_uint32 height)
{
	mngstuff	*mymng;

    mymng = (mngstuff*)mng_get_userdata(mng);

//	mymng->breakout = 0;
	return MNG_TRUE;
}

mng_uint32 mymng_gettickcount(mng_handle mng)
{
	mng_uint32 ticks;

	return(0);
}

mng_bool mymng_settimer(mng_handle mng,mng_uint32 msecs)
{
	mngstuff	*mymng;

    mymng = (mngstuff*)mng_get_userdata(mng);

	mymng->breakout = msecs;

	return MNG_TRUE;
}

mng_bool mymng_processtext(mng_handle mng,mng_uint8 type,mng_pchar keyword,mng_pchar text,mng_pchar lang,mng_pchar trans)
{
	mngstuff	*mymng;
	struct ClassBase *libBase;
	Class *cl;
//struct ExecIFace *IExec;
    mymng = (mngstuff*)mng_get_userdata(mng);

	cl = mymng->cl;
	libBase = (struct ClassBase *)cl->cl_UserData;

/*
	IExec=libBase->IExec;
	IExec->DebugPrintF("%s: %s\n",keyword,text);
*/
	if(!strnicmp(keyword,"Title",strlen(keyword)))
	{
		IDataTypes->SetDTAttrs(mymng->o, NULL, NULL,
				DTA_ObjName,text,
				TAG_END);
	}

	if(!strnicmp(keyword,"Author",strlen(keyword)))
	{
		IDataTypes->SetDTAttrs(mymng->o, NULL, NULL,
				DTA_ObjAuthor,text,
				TAG_END);
	}

	if(!strnicmp(keyword,"Description",strlen(keyword)))
	{
		IDataTypes->SetDTAttrs(mymng->o, NULL, NULL,
				DTA_ObjAnnotation,text,
				TAG_END);
	}

	if(!strnicmp(keyword,"Copyright",strlen(keyword)))
	{
		IDataTypes->SetDTAttrs(mymng->o, NULL, NULL,
				DTA_ObjCopyright,text,
				TAG_END);
	}

	if(!strnicmp(keyword,"Creation Time",strlen(keyword)))
	{
		IDataTypes->SetDTAttrs(mymng->o, NULL, NULL,
				DTA_ObjVersion,text,
				TAG_END);
	}
}


uint32 ClassDispatch (Class *cl, Object *o, Msg msg);

Class *initDTClass (struct ClassBase *libBase) {
	struct ExecIFace *IExec = libBase->IExec;
	Class *cl;
	SuperClassLib = IExec->OpenLibrary("datatypes/picture.datatype", 52);
	if (SuperClassLib) {
		cl = IIntuition->MakeClass(libBase->libNode.lib_Node.ln_Name, PICTUREDTCLASS, NULL, 0, 0);
		if (cl) {
			cl->cl_Dispatcher.h_Entry = (HOOKFUNC)ClassDispatch;
			cl->cl_UserData = (uint32)libBase;
			IIntuition->AddClass(cl);
		} else {
			IExec->CloseLibrary(SuperClassLib);
		}
	}
	return cl;
}

BOOL freeDTClass (struct ClassBase *libBase, Class *cl) {
	struct ExecIFace *IExec = libBase->IExec;
	BOOL res = TRUE;
	if (cl) {
		res = IIntuition->FreeClass(cl);
		if (res) {
			IExec->CloseLibrary(SuperClassLib);
		}
	}
	return res;
}

static int32 WriteICO (Class *cl, Object *o, struct dtWrite *msg);
static int32 ConvertICO (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh); //, uint32 index, uint32 *total);
static int32 GetICO (Class *cl, Object *o, struct TagItem *tags);

uint32 ClassDispatch (Class *cl, Object *o, Msg msg) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;

	uint32 ret;

	switch (msg->MethodID) {

		case OM_NEW:
			ret = IIntuition->IDoSuperMethodA(cl, o, msg);
			if (ret) {
				int32 error;
				error = GetICO(cl, (Object *)ret, ((struct opSet *)msg)->ops_AttrList);
				if (error != OK) {
					IIntuition->ICoerceMethod(cl, (Object *)ret, OM_DISPOSE);
					ret = (uint32)NULL;
					IDOS->SetIoErr(error);
				}
			}
			break;

/*
		case DTM_WRITE:
			// check if dt's native format should be used
			if (((struct dtWrite *)msg)->dtw_Mode == DTWM_RAW) {
				int32 error;
				error = WriteICO(cl, o, (struct dtWrite *)msg);
				if (error == OK) {
					ret = TRUE;
				} else {
					IDOS->SetIoErr(error);
					ret = FALSE;
				}
				break;
			}
*/
			/* fall through and let superclass deal with it */

		default:
			ret = IIntuition->IDoSuperMethodA(cl, o, msg);
			break;

	}

	return ret;
}

static int32 WriteICO (Class *cl, Object *o, struct dtWrite *msg) {
	return ERROR_NOT_IMPLEMENTED;
}

static int32 ConvertICO (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh) //, uint32 index, uint32 *total)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	struct ExecIFace *IExec = libBase->IExec;
	int32 status, level, error = OK;
	unsigned long width=0,height=0;
	int bpp=4;
	long pixformat;
	mngstuff *mymng;
	mng_handle	mng;

	mymng = (mngstuff*)IExec->AllocVec(sizeof(*mymng),MEMF_CLEAR);

	mymng->cl = cl;
	mymng->file = file;
	mymng->o = o;

	mng = mng_initialize(mymng, mymng_memalloc, mymng_memfree, MNG_NULL);
	mng_setcb_openstream(mng, mymng_openstream);
	mng_setcb_closestream(mng, mymng_closestream);
	mng_setcb_readdata(mng, mymng_readdata);
	mng_setcb_gettickcount(mng, mymng_gettickcount);
	mng_setcb_settimer(mng, mymng_settimer);
	mng_setcb_processheader(mng, mymng_processheader);
	mng_setcb_getcanvasline(mng, mymng_getcanvasline);
	mng_setcb_refresh(mng, mymng_refresh);

	IDOS->ChangeFilePosition(file,0,OFFSET_BEGINNING);

	mng_readdisplay(mng);

	while((mymng->breakout) && (mng_get_currentframe(mng) <= 1))
	{
		mymng->breakout = 0;
		mng_display_resume(mng);
	}

	mng_display_freeze(mng);

		pixformat = PBPAFMT_ARGB;
		bpp=4;

		bmh->bmh_Width = mymng->width;
		bmh->bmh_Height = mymng->height;
		bmh->bmh_Depth = 32;
		bmh->bmh_Masking = mskHasAlpha;

//	uint32 bpr = (bmh->bmh_Width + 3) & ~3;

	IDataTypes->SetDTAttrs(o, NULL, NULL,
	DTA_NominalHoriz,	bmh->bmh_Width,
	DTA_NominalVert,	bmh->bmh_Height,
	PDTA_SourceMode,	PMODE_V43,
	DTA_ErrorLevel,	&level,
	DTA_ErrorNumber,	&error,
	TAG_END);

	IIntuition->IDoSuperMethod(cl, o,
	PDTM_WRITEPIXELARRAY,mymng->bitmap, pixformat,
	bmh->bmh_Width*4, 0, 0, bmh->bmh_Width, bmh->bmh_Height);

	IExec->FreeVec(mymng->bitmap);

	mng_cleanup(&mng);

	return error;
}

static int32 GetICO (Class *cl, Object *o, struct TagItem *tags) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	struct BitMapHeader *bmh = NULL;
	char *filename;
	int32 srctype;
	int32 error = ERROR_OBJECT_NOT_FOUND;
	BPTR file = (BPTR)NULL;
	uint32 whichpic, *numpics;

	filename = (char *)IUtility->GetTagData(DTA_Name, (uint32)"Untitled", tags);
//	whichpic = IUtility->GetTagData(PDTA_WhichPicture, 0, tags);
//	numpics = (uint32 *)IUtility->GetTagData(PDTA_GetNumPictures, (uint32)NULL, tags);

	IDataTypes->GetDTAttrs(o,
		PDTA_BitMapHeader,	&bmh,
		DTA_Handle,			&file,
		DTA_SourceType,		&srctype,
		TAG_END);

	/* Do we have everything we need? */
	if (bmh && file && srctype == DTST_FILE) {

		IDataTypes->SetDTAttrs(o, NULL, NULL,
				DTA_ObjName,IDOS->FilePart(filename),
				TAG_END);
		error = ConvertICO(cl, o, file, bmh); //, whichpic, numpics);

	}

	return error;
}
