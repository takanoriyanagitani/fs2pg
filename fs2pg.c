#include <string.h>

#include <postgres.h>
#include <fmgr.h>

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(id2path);

Datum id2path(PG_FUNCTION_ARGS){
  text* id = PG_GETARG_TEXT_P(0);

  text* path = (text*)palloc(VARSIZE(id));
  SET_VARSIZE(path, VARSIZE(id));

  memcpy(VARDATA(path), VARDATA(id), VARSIZE(id)-VARHDRSZ);

  PG_RETURN_TEXT_P(path);
}

PG_FUNCTION_INFO_V1(f2p_hsz); Datum f2p_hsz(PG_FUNCTION_ARGS){ PG_RETURN_INT32(VARHDRSZ); }

PG_FUNCTION_INFO_V1(f2p_tsz); Datum f2p_tsz(PG_FUNCTION_ARGS){
  text* t = PG_GETARG_TEXT_P(0);
  int32 i = VARSIZE(t);
  PG_RETURN_INT32(i);
}
