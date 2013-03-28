-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION unnest_ordinality" to load this file. \quit

CREATE FUNCTION unnest_ordinality(the_array IN anyarray)
RETURNS TABLE (element_number int, element  anyelement)
AS 'MODULE_PATHNAME' , 'unnest_ordinality'
LANGUAGE C STRICT IMMUTABLE;
