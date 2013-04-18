# unnest_ordinality PostgreSQL Extension

Efficiently turn a Postgres array into a set of 
(element_number, array_element) pairs.

Should work with just about any modern version of PostgreSQL. 
Mainly tested with Release 9.2.

## Building

    make
	make install

## Using

    andrew# create extension unnest_ordinality;

    andrew=# select * from unnest_ordinality('{a,b,c,d,e,f,g}'::text[]);
     element_number | element 
    ----------------+---------
                  1 | a
                  2 | b
                  3 | c
                  4 | d
                  5 | e
                  6 | f
                  7 | g

