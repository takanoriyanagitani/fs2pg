#include <string.h>

#include <postgres.h>
#include <fmgr.h>

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
