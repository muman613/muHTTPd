#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sqlite3.h>

using namespace std;

string session_id(int length=16);

char charSet[] = {
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
};

void session_id_init() {
	srandom(time(0L));
	return;
}

string session_id(int length) {
	string sRes;
	
	for (int x = 0 ; x < length ; x++) {
		int n = random() % sizeof(charSet);
		sRes += charSet[n];
	}
	
	return sRes;
}


int main(int argc, char* argv[]) {
	string sSess1, sSQL; //, sSess2;
	sqlite3	*db;
	int rc;
	int num = 20;
 	char *zErrMsg = 0;

	if (argc > 1) {
		num = atoi(argv[1]);
	}
	
	rc = sqlite3_open("session.db", &db);
	if (rc) {
		fprintf(stderr, "ERROR: Can't open database - %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	
	printf("Total entries in table = %ld\n", sizeof(charSet));

	session_id_init();
	
	for (int count = 0 ; count < num ; count++) {
		sSess1 = session_id();
		sSQL = "INSERT INTO session (ID) VALUES ('" + sSess1 + "')";
		printf("session ID = %s\n", sSess1.c_str());
//		printf("SQL %s\n", sSQL.c_str());
		rc = sqlite3_exec(db, sSQL.c_str(), 0, 0, &zErrMsg);

		if (rc != SQLITE_OK) {
		    fprintf(stderr, "SQL error: %s\n", zErrMsg);   
			sqlite3_free(zErrMsg);
		}
	}
	
	
	sqlite3_close(db);
	
	return 0;
}
