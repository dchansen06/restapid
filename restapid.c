/* Exit codes:
 * 0: Everything is ok, no problems
 * -1: The api server had an internal error and could not start
 *
 * The most likely cause of a problem is the port selected being in use
 *
 * This program is designed to setup an api server on port PORT and then
 * when a get request is sent, to reply with the execution of a bash
 * script that's output is less than MAX_BYTES with a http status code.
 *
 * The exit is handeled by a SIGINT being caught by sig_handler() which
 * then releases pause() and continues main() to clean up the instance
 *
 * Dependencies:	(GNU coreutils or dpkg unless otherwise noted)
 *	netstat
 *	grep
 *	wc
 *	cat
 *	awk
 *	ulfius
 *	df
 *	tr
 *	free
 */

#include <stdio.h>
#include <ulfius.h>	// API backend: https://babelouest.github.io/ulfius/
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>

#define PORT 80
#define HOST "127.0.0.1"
#define MAX_BYTES 16

/* Exit the backend loop */
void sig_handler (int code)
{
	printf("Exiting %d\n", code);	// Prints exit signal code for troubleshooting and unpauses the pause() in main to start cleanup
}

/* Allows CORS if OPTIONS method instead of GET */
int callback_options(const struct _u_request *request, struct _u_response *response, void *user_data)
{
	u_map_put(response->map_header, "Access-Control-Allow-Origin", "*");	// Allow access to anyone
	u_map_put(response->map_header, "Access-Control-Allow-Methods", "GET, OPTIONS");
	u_map_put(response->map_header, "Access-Control-Allow-Headers", "Content-Type");	// Allow only Content-Type
	return U_CALLBACK_COMPLETE;
}

/* Connection api */
int callback_connection (const struct _u_request * request, struct _u_response * response, void * user_data)
{
	u_map_put(response->map_header, "Access-Control-Allow-Origin", "*");	// Allow CORS

	FILE * fp;
	char output[MAX_BYTES];
	fp = popen("netstat -an | grep ESTABLISHED | wc -l", "r");	// Connection sh command, parses to number
	fgets(output, MAX_BYTES-1, fp);
	fclose(fp);
	ulfius_set_string_body_response(response, 200, output);	// Response 200
	return U_CALLBACK_CONTINUE;
}

/* CPU api */
int callback_cpu (const struct _u_request * request, struct _u_response * response, void * user_data)
{
	u_map_put(response->map_header, "Access-Control-Allow-Origin", "*");	// Allow CORS

	FILE * fp;
	char output[MAX_BYTES];
	fp = popen("{ cat /proc/stat; sleep 0.06; cat /proc/stat; } | awk '/^cpu / {usr=$2-usr; sys=$4-sys; idle=$5-idle; iow=$6-iow} END {total=usr+sys+idle+iow; printf \"%.2f\\n\", (total-idle)*100/total}'", "r");	// CPU sh command, parses to number
	fgets(output, MAX_BYTES-1, fp);
	fclose(fp);
	ulfius_set_string_body_response(response, 200, output);	// Response 200
	return U_CALLBACK_CONTINUE;
}

/* Disk api */
int callback_disk (const struct _u_request * request, struct _u_response * response, void * user_data)
{
	u_map_put(response->map_header, "Access-Control-Allow-Origin", "*");	// Allow CORS

	FILE * fp;
	char output[MAX_BYTES];
	fp = popen("df --type=ext4 | awk 'FNR == 2 {print $5}' | tr -d '%'", "r");	// Disk sh command, parses to number
	fgets(output, MAX_BYTES-1, fp);
	fclose(fp);
	ulfius_set_string_body_response(response, 200, output);	// Response 200
	return U_CALLBACK_CONTINUE;
}

/* RAM api */
int callback_ram (const struct _u_request * request, struct _u_response * response, void * user_data)
{
	u_map_put(response->map_header, "Access-Control-Allow-Origin", "*");	// Allow CORS

	FILE * fp;
	char output[MAX_BYTES];
	fp = popen("free -m | awk 'FNR == 2 {printf(\"%0.2f%%\", $3/$2*100)}' | tr -d '%'", "r");	// RAM sh command, parses to number
	fgets(output, MAX_BYTES-1, fp);
	fclose(fp);
	ulfius_set_string_body_response(response, 200, output);	// Response 200
	return U_CALLBACK_CONTINUE;
}

/* Swap api */
int callback_swap (const struct _u_request * request, struct _u_response * response, void * user_data)
{
	u_map_put(response->map_header, "Access-Control-Allow-Origin", "*");	// Allow CORS

	FILE * fp;
	char output[MAX_BYTES];
	fp = popen("free -m | awk 'FNR == 3 {printf(\"%0.2f%%\", $3/$2*100)}' | tr -d '%'", "r");	// Swap sh command, parses to number
	fgets(output, MAX_BYTES-1, fp);
	fclose(fp);
	ulfius_set_string_body_response(response, 200, output);	// Response 200
	return U_CALLBACK_CONTINUE;
}

/* Temp api */
int callback_temp (const struct _u_request * request, struct _u_response * response, void * user_data)
{
	u_map_put(response->map_header, "Access-Control-Allow-Origin", "*");	// Allow CORS

	FILE * fp;
	char output[MAX_BYTES];
	fp = popen("cat /sys/class/thermal/thermal_zone0/temp | awk '{printf(\"%0.2f\", $1/1000)}'", "r");	// Works on a Raspberry Pi only, modify to work on your device as needed
	fgets(output, MAX_BYTES-1, fp);
	fclose(fp);
	ulfius_set_string_body_response(response, 200, output);	// Response 200
	return U_CALLBACK_CONTINUE;
}

/* Root response */
int callback_root (const struct _u_request * request, struct _u_response * response, void * user_data)
{
	u_map_put(response->map_header, "Access-Control-Allow-Origin", "*");	// Allow CORS

	ulfius_set_string_body_response(response, 501, "Error 501: Not Implemented\nThe server lacks the ability to fulfull the request at this time.");	// Prints info, response 501
	return U_CALLBACK_CONTINUE;
}

/* Index.html response */
int callback_index (const struct _u_request * request, struct _u_response * response, void * user_data)
{
	u_map_put(response->map_header, "Access-Control-Allow-Origin", "*");	// Allow CORS

	ulfius_set_string_body_response(response, 418, "Error 418: I'm a teapot\nThe server refuses the attempt to brew coffee (serve HTML) with a teapot (REST API server).");	// Prints info, response 418
	return U_CALLBACK_CONTINUE;
}

/* Handles setup and take down */
int main (int argc, char *argv[])	// Accepts arguments
{
	struct _u_instance instance;	// Set up ulfius struct

	struct sockaddr_in sockaddr;
	sockaddr.sin_port = htons(PORT);
	sockaddr.sin_family = AF_INET;
	inet_pton(AF_INET, HOST, (struct in_addr *)&sockaddr.sin_addr.s_addr);	// Select specific address to bind too

	if (ulfius_init_instance(&instance, PORT, &sockaddr, NULL) != U_OK)	// Confirm can get port allocation
	{
		fprintf(stderr, "Error ulfius_init_instance, abort\n");
		return(-1);
	}

	ulfius_add_endpoint_by_val(&instance, "OPTIONS", NULL, "*", 0, &callback_options, NULL);	// Add CORS information if OPTIONS method

	ulfius_add_endpoint_by_val(&instance, "GET", "/connection", NULL, 0, &callback_connection, NULL);	// Add connection api
	ulfius_add_endpoint_by_val(&instance, "GET", "/cpu", NULL, 0, &callback_cpu, NULL);	// Add cpu api
	ulfius_add_endpoint_by_val(&instance, "GET", "/disk", NULL, 0, &callback_disk, NULL);	// Add disk api
	ulfius_add_endpoint_by_val(&instance, "GET", "/ram", NULL, 0, &callback_ram, NULL);	// Add ram api
	ulfius_add_endpoint_by_val(&instance, "GET", "/swap", NULL, 0, &callback_swap, NULL);	// Add swap api
	ulfius_add_endpoint_by_val(&instance, "GET", "/temp", NULL, 0, &callback_temp, NULL);	// Add temp api
	ulfius_add_endpoint_by_val(&instance, "GET", "/", NULL, 0, &callback_root, NULL);	// Add root response
	ulfius_add_endpoint_by_val(&instance, "GET", "/index.html", NULL, 0, &callback_index, NULL);	// Add index.html response

	signal(SIGINT, sig_handler);	// Initialize exit catcher

	if (ulfius_start_framework(&instance) == U_OK)	// Confirm everything works
	{
		printf("Started API on http:%s:%d\n", HOST, PORT);
		pause();	// When SIGINT recieved, pause() will exit and continue in main()
	} else
	{
		fprintf(stderr, "Error starting API\n");
	}

	printf("Ended API\n");

	ulfius_stop_framework(&instance);	// Shutdown api
	ulfius_clean_instance(&instance);	// Clean up api

	return 0;	// Exit
}
