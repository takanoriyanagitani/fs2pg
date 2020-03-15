#include <string.h>

#include <postgres.h>
#include <fmgr.h>

#include <funcapi.h>
#include <access/htup_details.h>

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(id2path);

Datum id2path(PG_FUNCTION_ARGS){
  text* id = PG_GETARG_TEXT_P(0);

  const char* env_val = getenv("FS2PG_BASE_PATH");
  const char* basedir = NULL == env_val ? "/pgdata/fs2pg" : env_val;
  const uint16_t blen = strlen(basedir);
  const uint16_t ilen = VARSIZE(id)-VARHDRSZ;
  const uint16_t psz  = blen + ilen + 1 + VARHDRSZ + 1; // '/', '\0'
  text* idpath = (text*)palloc(psz);
  SET_VARSIZE(idpath, psz);
  snprintf(VARDATA(idpath), psz-VARHDRSZ, "%s/%.*s", basedir, ilen, VARDATA(id));

  PG_RETURN_TEXT_P(idpath);
}

PG_FUNCTION_INFO_V1(f2p_hsz); Datum f2p_hsz(PG_FUNCTION_ARGS){ PG_RETURN_INT32(VARHDRSZ); }

PG_FUNCTION_INFO_V1(f2p_tsz); Datum f2p_tsz(PG_FUNCTION_ARGS){
  text* t = PG_GETARG_TEXT_P(0);
  int32 i = VARSIZE(t);
  PG_RETURN_INT32(i);
}

PG_FUNCTION_INFO_V1(f2p_txt);

Datum f2p_txt(PG_FUNCTION_ARGS){
  FuncCallContext* ctx = NULL;

  struct {
    FILE* file;
    char* filename16;
    char* buf8;
    char* line;
    uint32_t number;
  } *file_info;

  if(SRF_IS_FIRSTCALL()){
    TupleDesc td = {0};

    ctx = SRF_FIRSTCALL_INIT();
    MemoryContext mc = MemoryContextSwitchTo(ctx->multi_call_memory_ctx);

    switch(get_call_result_type(fcinfo, NULL, &td)){
      case TYPEFUNC_COMPOSITE: break;
      default:
        ereport(ERROR, (
          errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
          errmsg("function returning record called in context that cannot accept type record")
        ));
        break;
    }

    ctx->tuple_desc = BlessTupleDesc(td);

    text* id = PG_GETARG_TEXT_P(0);
    file_info = palloc(sizeof(*file_info));
    file_info->filename16 = (char*) palloc(65536);
    file_info->buf8       = (char*) palloc(256);
    file_info->line       = NULL;
    file_info->number     = 0;
    snprintf(file_info->filename16, 65536, "/pgdata/fs2pg/%.*s", VARSIZE(id)-VARHDRSZ, VARDATA(id));
    file_info->file = fopen(file_info->filename16, "rb");

    ctx->user_fctx = file_info;

    MemoryContextSwitchTo(mc);
  }

  ctx = SRF_PERCALL_SETUP();
  file_info = ctx->user_fctx;

  if(NULL == file_info->file) SRF_RETURN_DONE(ctx);

  file_info->line = fgets(file_info->buf8, 256, file_info->file);
  if(NULL == file_info->line){
    fclose(file_info->file);
    SRF_RETURN_DONE(ctx);
  } else {
    Datum values[2] = {0};
    bool  nulls[2]  = {0};
    nulls[0] = 0; values[0] = UInt32GetDatum(file_info->number++);
    nulls[1] = 0; values[1] = CStringGetDatum(file_info->line);
    HeapTuple ht = heap_form_tuple(ctx->tuple_desc, values, nulls);
    PG_RETURN_DATUM(HeapTupleGetDatum(ht));
  }
}
