// compile with g++ -o odbc_sample.exe odbc_sample.cpp -lodbc32 -lodbccp32
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <sql.h>
#include <sqlext.h>

void HandleDiagnosticRecord (SQLHANDLE      hHandle,
                             SQLSMALLINT    hType,
                             RETCODE        RetCode)
{
    SQLSMALLINT iRec = 0;
    SQLINTEGER  iError;
    WCHAR       wszMessage[1000];
    WCHAR       wszState[1000];


    if (RetCode == SQL_INVALID_HANDLE)
    {
        printf("Invalid handle!\n");
        return;
    }

    while (SQLGetDiagRecW(hType,
                         hHandle,
                         ++iRec,
                         (SQLWCHAR*)wszState,
                         &iError,
                         (SQLWCHAR*)wszMessage,
                         (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)),
                         (SQLSMALLINT *)NULL) == SQL_SUCCESS)
    {
        // Hide data truncated..
        const WCHAR* val = L"01004";
        if (memcmp(wszState, val, sizeof(val)))
        {
            printf("[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
        }
    }
}

void toUtf8(const wchar_t* w1, char* o) {
    if (!w1) return;
    // int len = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);
    // WideCharToMultiByte(CP_UTF8, 0, w, -1, o, len, nullptr, nullptr);
    char* w = (char*)w1;
    int pos1 = 0;
    int pos2 = 0;
    while(true) {
        if (w[pos1] == '\0' && w[pos1+1] == '\0') {
            o[pos2] = '\0';
            break;
        }
        if (((char*)w)[pos1] & 128) o[pos2] = '?';
        else o[pos2] = ((char*)w)[pos1];
        // o[pos2] = ((char*)w)[pos1] & 127;
        // o[pos2+1] = ((char*)w)[pos1+1] & 127;
        pos1 += 2;
        pos2 += 1;
    }
}

void toWideChar(char* input, wchar_t* output) {
    int i = 0;
    while(true) {
        output[i] = (wchar_t) btowc((int)input[i]);
        if (input[i] == '\0') {
            break;
        }
    }
}

void showDataSourcesFrom(SQLHENV* env, SQLUSMALLINT dir) {
    SQLRETURN ret;
    const int BUFFER_SIZE = 100;
    WCHAR dataSourceName[BUFFER_SIZE];
    int extraLenghtUnused;
    WCHAR description[BUFFER_SIZE];
    ret = SQLDataSourcesW(
        *env,
        dir,
        (SQLWCHAR*)dataSourceName,
        BUFFER_SIZE,
        (SQLSMALLINT*)&extraLenghtUnused,
        (SQLWCHAR*)description,
        BUFFER_SIZE,
        (SQLSMALLINT*)&extraLenghtUnused
    );
    if (ret == SQL_ERROR) {
        printf("Error on data sources\n");
        HandleDiagnosticRecord(*env, SQL_HANDLE_ENV, ret);
        return;
    } else if (ret == SQL_NO_DATA) {
    } else {
        char b1[BUFFER_SIZE];
        char b2[BUFFER_SIZE];
        toUtf8(dataSourceName, b1);
        toUtf8(description, b2);
        printf("ret: %d Ds name: %s Ds desc: %s\n", ret, b1, b2);
    }
    while(true) {
        ret = SQLDataSourcesW(
            *env,
            SQL_FETCH_NEXT,
            (SQLWCHAR*)dataSourceName,
            BUFFER_SIZE,
            (SQLSMALLINT*)&extraLenghtUnused,
            (SQLWCHAR*)description,
            BUFFER_SIZE,
            (SQLSMALLINT*)&extraLenghtUnused
        );
        if (ret == SQL_ERROR) {
            printf("Error on data sources\n");
            HandleDiagnosticRecord(*env, SQL_HANDLE_ENV, ret);
            return;
        } else if (ret == SQL_NO_DATA) {
            break;
        }
    
        char b1[BUFFER_SIZE];
        char b2[BUFFER_SIZE];
        toUtf8(dataSourceName, b1);
        toUtf8(description, b2);
        printf("ret: %d Ds name: %s Ds desc: %s\n", ret, b1, b2);
    }
}

void _showDrivers(SQLHENV* env, SQLUSMALLINT dir) {
    SQLRETURN ret;
    const int BUFFER_SIZE = 200;
    int unused;
    WCHAR description[BUFFER_SIZE];
    // attributes returns in a key value par format sepparated by \0 and ending with \0\0
    WCHAR attributes[BUFFER_SIZE];
    attributes[BUFFER_SIZE-1] = L'\0';
    memset(attributes, '\n', BUFFER_SIZE);
    while(true) {
        ret = SQLDriversW(
            *env,
            dir,
            description,
            BUFFER_SIZE,
            (SQLSMALLINT*)&unused,
            attributes,
            BUFFER_SIZE,
            (SQLSMALLINT*)&unused
        );
        if (ret == SQL_ERROR) {
            printf("Error on drivers\n");
            HandleDiagnosticRecord(*env, SQL_HANDLE_ENV, ret);
            return;
        } else if (ret == SQL_NO_DATA) {
            break;
        } else {
            char b1[BUFFER_SIZE];
            toUtf8(description, b1);
            char b2[BUFFER_SIZE];
            for(int i = 0, pos2 = 0; i < BUFFER_SIZE - 1; i += 1, pos2++) {
                if(attributes[i] == '\0') {
                    if (attributes[i+1] == '\0') {
                        b2[pos2] = '\0';
                        break;
                    }
                    b2[pos2] = '|';
                } else if (attributes[i] & 128) {
                    b2[pos2] = '?';
                } else {
                    b2[pos2] = attributes[i];
                }
            }
            b2[BUFFER_SIZE-1] = '\0';
            printf("ret: %d | description: %s | attributes: {%s}\n", ret, b1, b2);
            // to ensure no loop
            if (dir == SQL_FETCH_FIRST) break;
        }
    }
}
void showDrivers(SQLHENV* env) {
    _showDrivers(env, SQL_FETCH_FIRST);
    _showDrivers(env, SQL_FETCH_NEXT);
}

void showDriverInfo(SQLHENV *env, char* connectionString) {
    SQLRETURN ret;
    SQLHDBC hConn;
    ret = SQLAllocHandle(SQL_HANDLE_DBC, *env, &hConn);
    if (ret == SQL_ERROR) {
        printf("Error on handle\n");
        return;
    }
    const int BUFFER_SIZE = 1024;
    int unused;
    WCHAR connectionStringW[BUFFER_SIZE];
    toWideChar(connectionString, connectionStringW);
    WCHAR outConnectionStringW[BUFFER_SIZE];
    ret = SQLDriverConnectW(
        hConn,
        NULL,
        (SQLWCHAR*) connectionStringW,
        (SQLSMALLINT) strlen(connectionString),
        outConnectionStringW,
        (SQLSMALLINT) BUFFER_SIZE,
        (SQLSMALLINT*) &unused,
        SQL_DRIVER_NOPROMPT
    );
    if (ret == SQL_ERROR) {
        printf("Error on drivers\n");
        HandleDiagnosticRecord(*env, SQL_HANDLE_ENV, ret);
        return;
    }
    if (hConn) {
        SQLDisconnect(hConn);
        SQLFreeHandle(SQL_HANDLE_DBC, hConn);
    }
}

int main() {
    printf("Running\n");

    SQLHENV env;
    SQLRETURN ret;
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    if (ret == SQL_ERROR) {
        printf("Error on handle\n");
        return(-1);
    }
    SQLSetEnvAttr(env,
                SQL_ATTR_ODBC_VERSION,
                (SQLPOINTER)SQL_OV_ODBC3,
                0);

    printf("User data sources:\n");
    showDataSourcesFrom(&env, SQL_FETCH_FIRST_USER);
    printf("System data sources:\n");
    showDataSourcesFrom(&env, SQL_FETCH_FIRST_SYSTEM);

    printf("Drivers:\n");
    showDrivers(&env);

    char* driverName = (char*) "gopipePostgres";
    printf("Conneting to %s\n", driverName);
    showDriverInfo(&env, driverName);

    SQLFreeHandle(SQL_HANDLE_ENV, env);
    printf("Ending\n");
    return 0;
}