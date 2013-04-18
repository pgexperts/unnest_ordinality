

#include "postgres.h"

#include "fmgr.h"
#include "funcapi.h"
#include "miscadmin.h"
#if PG_VERSION_NUM > 90200
#include "access/htup_details.h"
#endif
#include "catalog/pg_type.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"

PG_MODULE_MAGIC;

/*
 * SQL function unnest_ordinality(anyarray)
 */
extern Datum unnest_ordinality(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(unnest_ordinality);

extern Datum
unnest_ordinality(PG_FUNCTION_ARGS)
{
    ArrayType  *in_array = PG_GETARG_ARRAYTYPE_P(0);
    Oid         element_type = ARR_ELEMTYPE(in_array);
    ReturnSetInfo *rsi;
    TupleDesc   tupdesc, ret_tdesc;
    Datum      *in_datums;
    bool       *in_nulls;
    int         in_count, i;
    int16       typlen;
    bool        typbyval;
    char        typalign,
                typdelim;
    Oid         typioparam;
    Oid         typoutputfunc;
    MemoryContext old_cxt;
	Tuplestorestate *tuple_store;

     rsi = (ReturnSetInfo *) fcinfo->resultinfo;

	 if (!rsi || !IsA(rsi, ReturnSetInfo) ||
        (rsi->allowedModes & SFRM_Materialize) == 0 ||
        rsi->expectedDesc == NULL)
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("set-valued function called in context that "
                        "cannot accept a set")));

    rsi->returnMode = SFRM_Materialize;

	(void) get_call_result_type(fcinfo, NULL, &tupdesc);

    /* make these in a sufficiently long-lived memory context */
    old_cxt = MemoryContextSwitchTo(rsi->econtext->ecxt_per_query_memory);
	ret_tdesc = CreateTupleDescCopy(tupdesc);
    tuple_store =
        tuplestore_begin_heap(rsi->allowedModes & SFRM_Materialize_Random,
                              false, work_mem);
    MemoryContextSwitchTo(old_cxt);

    get_type_io_data(element_type, IOFunc_output,
                     &typlen, &typbyval, &typalign,
                     &typdelim, &typioparam, &typoutputfunc);

    deconstruct_array(in_array,
                      element_type, typlen, typbyval, typalign,
                      &in_datums, &in_nulls, &in_count);

	for (i = 0; i < in_count; i++ )
	{
		Datum       values[2];
		bool        nulls[2] = {false, false};
		HeapTuple   tuple;

		values[0] = Int32GetDatum(i+1);
		
		if (in_nulls[i])
		{
			nulls[1] = true;
			values[1] = (Datum) NULL;
		}
		else
		{
			values[1] = in_datums[i];
		}
		
		
		tuple = heap_form_tuple(ret_tdesc, values, nulls);
		
		tuplestore_puttuple(tuple_store, tuple);
			
	}
	
    rsi->setResult = tuple_store;
    rsi->setDesc = ret_tdesc;

    PG_RETURN_NULL();

	
}
