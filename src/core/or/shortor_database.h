#ifndef TOR_SHORTOR_DATABASE_H
#define TOR_SHORTOR_DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

/** NOTE(shortor): The client will use a single database connection. */
extern PGconn *shortor_conn;

/** NOTE(shortor): This is the name of the prepared query that shortor will
 * always use. */
extern const char *shortor_statement_name;

/** NOTE(shortor): Initialize database connection. */
void shortor_pg_init(void);

#endif /* !defined(TOR_SHORTOR_DATABASE_H) */
