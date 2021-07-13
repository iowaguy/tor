
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>

#include "lib/log/log.h"

#include "core/or/shortor_database.h"

PGconn *shortor_conn;
const char *shortor_statement_name = "shortorFastest";

/** NOTE(shortor): By default, do not use shortor routing. */
int use_shortor_routing = 0;

/** NOTE(shortor): Initialize postgres connection. */
void
shortor_pg_init(void)
{
  /* NOTE(shortor): Initialize database connection. */
  log_notice(LD_CIRC, "SHORTOR Initializing database connection.");
  shortor_conn = PQconnectdb("hostaddr=10.233.1.2 user=postgres dbname=shortor");

  /* NOTE(shortor): This can only happen if there is not enough memory
   * to allocate the PGconn structure.
   */
  if (shortor_conn == NULL) {
    log_err(LD_CIRC, "SHORTOR Out of memory connecting to PostgreSQL.");
    exit(1);
  }

  /* NOTE(shortor): check if the connection attempt worked */
  log_notice(LD_CIRC, "SHORTOR confirming successful connection...");
  if (PQstatus(shortor_conn) != CONNECTION_OK)
  {
    log_err(LD_CIRC, "SHORTOR database connection failed with: %s",
            PQerrorMessage(shortor_conn));

    /* NOTE(shortor): Even if the connection failed, the PGconn structure has
     * been allocated and must be freed. */
    PQfinish(shortor_conn);
    exit(1);
  }

  /* NOTE(shortor): Prepare the query so it only needs to be parsed and planned
   * once. */
  log_notice(LD_CIRC, "SHORTOR preparing query...");
  PQprepare(shortor_conn, shortor_statement_name,
            "SELECT nickname, fingerprint, latency FROM chutney_test WHERE "
            "latency = (SELECT MIN(latency) FROM chutney_test WHERE fingerprint "
            "NOT IN ($1, $2, $3, $4));",
            4, // Number of query params
            NULL); // passing NULL forces the server to infer the types.

  const char *err_message = (const char *) PQerrorMessage(shortor_conn);
  if (strcmp(err_message, "") != 0) {
    log_notice(LD_CIRC, "SHORTOR connection error message: %s", err_message);
    PQfinish(shortor_conn);
    exit(1);
  }
}
