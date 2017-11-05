#include "ListComponents.h"

#include <iostream>
#include <stdio.h>

#include <IL/OMX_Core.h>

ListComponents::ListComponents()
    : TestCase( TESTCASE_NAME_LIST_COMPONENTS )
{
}

ListComponents::~ListComponents()
{
}

void ListComponents::Run()
{
    TestCase::Run();

    OMX_ERRORTYPE err = OMX_Init();
    if ( err != OMX_ErrorNone ) {
        cout << "OMX_Init() failed" << endl;
        return;
    }

    char componentName[OMX_MAX_STRINGNAME_SIZE];
    err = OMX_ErrorNone;
    for ( int componentID = 0; err != OMX_ErrorNoMore; componentID++ ) {
        err = OMX_ComponentNameEnum( componentName, OMX_MAX_STRINGNAME_SIZE, componentID );
        if ( err == OMX_ErrorNone ) {
            cout << "Component " << componentID << " is " << componentName << endl;
            ListRoles( componentName );
        }
    }
    cout << "No more components\n";
}

void ListComponents::ListRoles( char* componentName )
{
    OMX_U32 numRoles;
    OMX_U8* roles[32];

    /* get the number of roles by passing in a NULL roles param */
    OMX_ERRORTYPE err = OMX_GetRolesOfComponent( componentName, &numRoles, NULL );
    if ( err != OMX_ErrorNone ) {
        cout << "Getting roles failed, error: " << err << endl;
    }
    printf( "  Num roles is %d\n", numRoles );
    if ( numRoles > 32 ) {
        printf( "Too many roles to list\n" );
        return;
    }

    /* now get the roles */
    for ( int roleID = 0; roleID < numRoles; roleID++ ) {
        roles[roleID] = new OMX_U8[OMX_MAX_STRINGNAME_SIZE];
    }

    err = OMX_GetRolesOfComponent( componentName, &numRoles, roles );
    if ( err != OMX_ErrorNone ) {
        fprintf( stderr, "Getting roles failed\n", 0 );
        return;
    }

    for ( int roleID = 0; roleID < numRoles; roleID++ ) {
        printf( "    role: %s\n", roles[roleID] );
        delete[] roles[roleID];
    }

    /* This is in version 1.2
    for (i = 0; OMX_ErrorNoMore != err; i++) {
    err = OMX_RoleOfComponentEnum(role, name, i);
    if (OMX_ErrorNone == err) {
        printf("   Role of omponent is %s\n", role);
    }
    }
    */
}
