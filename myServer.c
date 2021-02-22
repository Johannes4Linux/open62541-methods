#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

#include <signal.h>
#include <stdlib.h>

#ifdef gpio
#include <wiringPi.h>
#endif


/** Global variables for led states */
static UA_Boolean led_1 = false, led_2 = false;


/**
 * @brief OPC Ua Method to control two LEDs
 *
 * @param input[0]	new state for LED 1
 * @param input[2]	new state for LED 2
 *
 * @param output	amount of LED states which where changed
 */
static UA_StatusCode
control_leds(UA_Server *server,
                         const UA_NodeId *sessionId, void *sessionHandle,
                         const UA_NodeId *methodId, void *methodContext,
                         const UA_NodeId *objectId, void *objectContext,
                         size_t inputSize, const UA_Variant *input,
                         size_t outputSize, UA_Variant *output) {
	UA_Int32 ret_val = 0;
	/* Let's extract the input parameters */
	UA_Boolean led_1_state = * (UA_Boolean *) input[0].data;
	UA_Boolean led_2_state = * (UA_Boolean *) input[1].data;

	ret_val = (led_1 != led_1_state) + (led_2 != led_2_state);

	led_1 = led_1_state;
	led_2 = led_2_state;

#ifdef gpio
	digitalWrite(24, led_1);
	digitalWrite(25, led_2);
#else
	printf("led_1: %d\n", led_1);
	printf("led_2: %d\n", led_2);
#endif

	/* Set output parameter to desired value */
	UA_Variant_setScalarCopy(output, &ret_val, &UA_TYPES[UA_TYPES_INT32]);
	return UA_STATUSCODE_GOOD;
}

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
	led_2 = true;
#ifdef gpio
	digitalWrite(25, HIGH);
#else
	printf("led_1: now ON\n");
#endif
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
	led_2 = false;
#ifdef gpio
	digitalWrite(25, LOW);
#else
	printf("led_1: now OFF\n");
#endif
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
#ifdef gpio
    wiringPiSetup () ;
    pinMode (24, OUTPUT) ;
    pinMode (25, OUTPUT) ;

	digitalWrite(24, LOW);
	digitalWrite(25, LOW);
#endif

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
	
	/* Add new method */
	UA_Argument inputArguments[2];

	UA_Argument_init(&inputArguments[0]);
    inputArguments[0].description = UA_LOCALIZEDTEXT("en-US", "LED 1 state");
    inputArguments[0].name = UA_STRING("LED 1 state");
    inputArguments[0].dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    inputArguments[0].valueRank = UA_VALUERANK_SCALAR;

	UA_Argument_init(&inputArguments[1]);
    inputArguments[1].description = UA_LOCALIZEDTEXT("en-US", "LED 2 state");
    inputArguments[1].name = UA_STRING("LED 2 state");
    inputArguments[1].dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    inputArguments[1].valueRank = UA_VALUERANK_SCALAR;

	UA_Argument outputArgument;

	UA_Argument_init(&outputArgument);
    outputArgument.description = UA_LOCALIZEDTEXT("en-US", "Amount of LED's states, which changed");
    outputArgument.name = UA_STRING("States changed");
    outputArgument.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    outputArgument.valueRank = UA_VALUERANK_SCALAR;

	UA_MethodAttributes ledstateAttr = UA_MethodAttributes_default;
    ledstateAttr.description = UA_LOCALIZEDTEXT("en-US","Method to control two LEDs");
    ledstateAttr.displayName = UA_LOCALIZEDTEXT("en-US","Method to control two LEDs");
    ledstateAttr.executable = true;
    ledstateAttr.userExecutable = true;
    UA_Server_addMethodNode(server, UA_NODEID_NUMERIC(1,62543),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                            UA_QUALIFIEDNAME(1, "LED control"),
                            ledstateAttr, &control_leds,
                            2, inputArguments, 1, &outputArgument, NULL, NULL);
	
	/* Start Server */
    UA_StatusCode retval = UA_Server_run(server, &running);

    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
