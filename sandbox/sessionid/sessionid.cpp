#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sqlite3.h>

using namespace std;

string session_id(int length=16);


static char charSet[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '@', '#', '$', '%',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
};

/* global storage */

int newCnt = 10;
bool bList = false;
bool bAdd  = true;

/**
 * Init session id.
 */

void session_id_init() {
    srandom(time(0L));
    return;
}

/**
 * Generate a session id.
 */

string session_id(int length) {
    string sRes;

    for (int x = 0 ; x < length ; x++) {
        int n = random() % sizeof(charSet);
        sRes += charSet[n];
    }

    return sRes;
}

bool parse_options(int argc, char* const * argv) {
    bool bRes = false;
    int  c;

    while ((c = getopt(argc, argv, "n:la")) != -1) {
        switch (c) {
        case 'n':
            newCnt = atoi( optarg );
            break;

        case 'l':
            bList = true;
            bAdd = false;
            bRes = true;
            break;

        case 'a':
            bList = false;
            bAdd = true;
            bRes = true;
            break;
        }
    }

    if (!bRes) {
        fprintf(stderr, "Must specify either -l (list) or -a (add)!\n");
    }

    return bRes;
}

/**
 *
 */

bool create_tables(sqlite3* db) {
    string sSQL;
    int rc;
    char* zErrMsg = 0;

    printf("Re-creating table!\n");

    sSQL = "CREATE TABLE IF NOT EXISTS session (id integer primary key autoincrement, sessid text unique, sessstart text default CURRENT_TIMESTAMP, sessend text)";

    rc = sqlite3_exec(db, sSQL.c_str(), 0, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        printf("ERROR: %s\n", zErrMsg);
    }

    return (rc == SQLITE_OK);
}


/**
 *	Check if a table exists in the sqlite3 database.
 */

bool check_table_exists(sqlite3* db, const char* szTableName) {
    bool 			bRes = false;
    string			sSQL;
    sqlite3_stmt*	stmt;
    int				rc;

    sSQL = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='" + string(szTableName) + "'";

#ifdef	DEBUG
    fprintf(stderr, "check_table_exists > SQL : %s\n", sSQL.c_str());
#endif

    rc = sqlite3_prepare_v2(db, sSQL.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "ERROR: Unable to prepare statement [%s]\n", sSQL.c_str());
        return false;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (sqlite3_column_int(stmt, 0) == 1) {
            printf("table exists!\n");
            bRes = true;
        }
    }

    rc = sqlite3_finalize(stmt);

    return bRes;
}

int get_row_count(sqlite3* db, const char* szTableName) {
    int nRes = -1;
    string			sSQL;
    sqlite3_stmt*	stmt;
    int				rc;

    sSQL = "SELECT count(*) FROM " + string(szTableName);

    rc = sqlite3_prepare_v2(db, sSQL.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "ERROR: Unable to prepare statement [%s]\n", sSQL.c_str());
        return -1;
    }

    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        nRes = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return nRes;
}

/**
 *	List all sessions in the database.
 */

void list_sessions(sqlite3* db) {
    string 			sSQL;
    sqlite3_stmt*	stmt;
    int				rc;

    sSQL = "SELECT id, sessid, datetime(sessstart, 'localtime') FROM session";

    rc = sqlite3_prepare_v2(db, sSQL.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "ERROR: Unable to prepare statement [%s]\n", sSQL.c_str());
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int							nSesID  = sqlite3_column_int(stmt, 0);
        const unsigned char*		szSesID = sqlite3_column_text(stmt, 1);
        const unsigned char*		szSesBe = sqlite3_column_text(stmt, 2);

        printf("%-4d : %-30s | %s\n", nSesID, szSesID, szSesBe);
    }

    sqlite3_finalize(stmt);

    return;
}

/**
 *	Main entry point
 */

int main(int argc, char* argv[]) {
    string 		sSess1, sSQL; //, sSess2;
    sqlite3*	db;
    int 		rc;
    char*		zErrMsg = 0;

    if (!parse_options(argc, argv)) {
        return -1;
    }

    /* Open the SQLite database */
    rc = sqlite3_open("session.db", &db);
    if (rc) {
        fprintf(stderr, "ERROR: Can't open database - %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    if (!check_table_exists(db, "session")) {
        create_tables(db);
    }

#ifdef	DEBUG
    fprintf(stderr, "Total entries in sessid table = %ld\n", sizeof(charSet));
#endif

    if (bAdd) {
        session_id_init();

        for (int count = 0 ; count < newCnt ; count++) {
            sSess1 = session_id(24);
            sSQL = "INSERT INTO session (sessid) VALUES ('" + sSess1 + "')";
            printf("session ID = %s\n", sSess1.c_str());
#ifdef	DEBUG
            fprintf(stderr, "SQL : %s\n", sSQL.c_str());
#endif
            rc = sqlite3_exec(db, sSQL.c_str(), 0, 0, &zErrMsg);

            if (rc != SQLITE_OK) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
        }
    } else if (bList) {
        /* */
        printf("Dumping %d session ids!\n", get_row_count(db, "session"));
        list_sessions(db);
    }

    sqlite3_close(db);

    return 0;
}
