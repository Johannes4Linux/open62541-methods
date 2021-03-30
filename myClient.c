#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/plugin/log_stdout.h>

#include <stdlib.h>

int main(void) {
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://192.168.0.225:4840");
    if(retval != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return (int)retval;
    }

	/* Call void/void methods */

	/* First call a method to turn on the LED */
	retval = UA_Client_call(client, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), UA_NODEID_NUMERIC(1, 62541), 0, NULL, NULL, NULL);
	if(retval == UA_STATUSCODE_GOOD)
			printf("Calling the Method 'Turn on LED' was successfull!\n");
	else
			printf("Calling the Method 'Turn on LED' was not successfull!\n");

	/* Wait for user input */
	printf("Press enter to continue...");
	getchar();

	/* Call the method to turn the LED off */
	retval = UA_Client_call(client, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), UA_NODEID_NUMERIC(1, 62542), 0, NULL, NULL, NULL);
	if(retval == UA_STATUSCODE_GOOD)
			printf("Calling the Method 'Turn off LED' was successfull!\n");
	else
			printf("Calling the Method 'Turn off LED' was not successfull!\n");


	/* Wait for user input */
	printf("Press enter to continue...");
	getchar();

	UA_Boolean led_1 = true, led_2 = true;
	UA_Variant input_params[2];

	UA_Variant_init(&input_params[0]);
	UA_Variant_setScalarCopy(&input_params[0], &led_1, &UA_TYPES[UA_TYPES_BOOLEAN]);
	UA_Variant_init(&input_params[1]);
	UA_Variant_setScalarCopy(&input_params[1], &led_2, &UA_TYPES[UA_TYPES_BOOLEAN]);

	size_t output_size;
	UA_Variant *ptr_output;

	retval = UA_Client_call(client, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), UA_NODEID_NUMERIC(1, 62543), 2, input_params, &output_size, &ptr_output);
	if(retval == UA_STATUSCODE_GOOD){
			printf("Calling the Method 'Turn off LED' was successfull!\n");
			/* Evaluate output parameters */
			printf("%d output parameters were returned!\n", output_size);
			UA_Int32 output = * (UA_Int32 *) ptr_output->data;
			printf("output: %d\n", output);
	}
	else
			printf("Calling the Method 'Turn off LED' was not successfull!\n");

    /* Clean up */
    UA_Client_delete(client); /* Disconnects the client internally */
    return EXIT_SUCCESS;
}
