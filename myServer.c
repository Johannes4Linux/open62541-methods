#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

#include <signal.h>
#include <stdlib.h>
#include <wiringPi.h>

/**
 * @brief OPC Ua Method to turn LED on
 */
static UA_StatusCode
turn_led_on(UA_Server *server,
                         const UA_NodeId *sessionId, void *sessionHandle,
                         const UA_NodeId *methodId, void *methodContext,
                         const UA_NodeId *objectId, void *objectContext,
                         size_t inputSize, const UA_Variant *input,
                         size_t outputSize, UA_Variant *output) {
	digitalWrite(25, HIGH);
	return UA_STATUSCODE_GOOD;
}

/**
 * @brief OPC Ua Method to turn LED off
 */
static UA_StatusCode
turn_led_off(UA_Server *server,
                         const UA_NodeId *sessionId, void *sessionHandle,
                         const UA_NodeId *methodId, void *methodContext,
                         const UA_NodeId *objectId, void *objectContext,
                         size_t inputSize, const UA_Variant *input,
                         size_t outputSize, UA_Variant *output) {
	digitalWrite(25, LOW);
	return UA_STATUSCODE_GOOD;
}

static volatile UA_Boolean running = true;
static void stopHandler(int sig) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "received ctrl-c");
    running = false;
}

int main(int argc, char *argv[]) {

    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    /* Initialize GPIO */
    wiringPiSetup () ;
    pinMode (25, OUTPUT) ;
    pinMode (24, OUTPUT) ;

	digitalWrite(25, LOW);
	digitalWrite(24, LOW);

    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

	if(argc == 2){	/* IP Address available */
		/* copy the hostname from char * to an open62541 variable */
		UA_String hostname;
		UA_String_init(&hostname);
		hostname.length = strlen(argv[1]);
		hostname.data = (UA_Byte *) argv[1];

		/* Change the configuration */
		UA_ServerConfig_setCustomHostname(UA_Server_getConfig(server), hostname);
	}

	/* Add methods to server */
	UA_MethodAttributes ledOnAttr = UA_MethodAttributes_default;
    ledOnAttr.description = UA_LOCALIZEDTEXT("en-US","Method to turn a LED on");
    ledOnAttr.displayName = UA_LOCALIZEDTEXT("en-US","Turn LED on");
    ledOnAttr.executable = true;
    ledOnAttr.userExecutable = true;
    UA_Server_addMethodNode(server, UA_NODEID_NUMERIC(1,62541),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                            UA_QUALIFIEDNAME(1, "Turn LED on"),
                            ledOnAttr, &turn_led_on,
                            0, NULL, 0, NULL, NULL, NULL);

	UA_MethodAttributes ledOffAttr = UA_MethodAttributes_default;
    ledOffAttr.description = UA_LOCALIZEDTEXT("en-US","Method to turn a LED off");
    ledOffAttr.displayName = UA_LOCALIZEDTEXT("en-US","Turn LED off");
    ledOffAttr.executable = true;
    ledOffAttr.userExecutable = true;
    UA_Server_addMethodNode(server, UA_NODEID_NUMERIC(1,62542),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                            UA_QUALIFIEDNAME(1, "Turn LED on"),
                            ledOffAttr, &turn_led_off,
                            0, NULL, 0, NULL, NULL, NULL);

	/* Start Server */
    UA_StatusCode retval = UA_Server_run(server, &running);

    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
