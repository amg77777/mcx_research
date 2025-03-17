#include <sofia-sip/nta.h>
#include <sofia-sip/su.h>
#include <sofia-sip/su_tag.h>
#include <sofia-sip/nua.h>
#include <sofia-sip/sip.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> // Necesario para strcpy

#define SIP_IDENTITY "sip:caller@127.0.0.1"
#define SIP_CONTACT_STR "sip:caller@127.0.0.1"
#define SIP_DEST     "sip:callee@127.0.0.1"
#define SIP_PROXY    "sip:proxy@127.0.0.1"

#ifndef SIP_200_OK
#define SIP_200_OK 200
#endif

nua_handle_t	*inv_handle = NULL; // Handle para el INVITE

// Callback que maneja los eventos SIP
static void	sip_invite_callback(nua_event_t event, int status,
		const char *phrase, nua_t *nua, void *context, nua_handle_t *nh,
		void *param, const struct sip_s *sip, tagi_t *tags)
{
	su_root_t *root = (su_root_t *)context;
	printf("Callback received event: %d, status: %d, phrase: %s\n", event, status, phrase);

	if (event == nua_i_invite) // Evento de INVITE entrante
		printf("INVITE recibido, request-response\n");

	else if (event == 10) // Respuesta al INVITE
	{
        printf("Respuesta agente al INVITE: %d %s\n", status, phrase);
		if (status == 180)
		{
			printf("Ringing...\n");
			// Enviar CANCEL para la llamada en progreso
			// printf("Enviando CANCEL...\n");
            // y colgamos
			// nua_cancel(nh, TAG_END());
		}
		else if (status == 200)
		{
            // El INVITE esta ok, info ok
			printf("200 OK recibido. Enviando ACK...\n");
			// Enviar ACK usando nua_ack()
			nua_shutdown(nua);
		}
	}
	else if (event == nua_i_ack)
    {
        // Ya te conozco
		printf("ACK recibido\n");
	}
	else if (event == nua_i_bye) // Evento de BYE entrante
    {
		printf("BYE recibido, terminando la llamada.\n");
        // Enviar respuesta 200 OK para el BYE
        nua_respond(nh, SIP_200_OK, "OK", TAG_END());
	}
	else
    {
		printf("Evento SIP: %d, %s\n", event, phrase);
		nua_shutdown(nua);
		sleep(1);
		su_root_break(root);
    }
}

int	main(void)
{
	su_root_t	*root;
	nua_t		*nua;

	printf("Iniciando el programa...\n");
	// Inicializa la librería
	su_init();
	printf("su_init() completado.\n");
	root = su_root_create(NULL);
	if (!root)
	{
		fprintf(stderr, "No se pudo crear el su_root\n");
		return (EXIT_FAILURE);
	}
	printf("su_root_create() completado.\n");
	// Crea el agente SIP (NUA) usando el callback definido
	nua = nua_create(root,
						sip_invite_callback,
						root,
						/*NUTAG_URL(SIP_PROXY),*/ TAG_END());
	if (!nua)
	{
		fprintf(stderr, "No se pudo crear el agente SIP (nua)\n");
		su_root_destroy(root);
		return (EXIT_FAILURE);
	}
	printf("nua_create() completado.\n");
	printf("Intentando enviar el INVITE...\n");

    // Llamada a INVITE
	nua_invite((nua_handle_t *)nua,
			   NUTAG_ALLOW(SIP_IDENTITY),
			   SIPTAG_CONTACT_STR(SIP_CONTACT_STR),
			   SIPTAG_TO_STR("sip:callee@127.0.0.1:5060"),
			   TAG_END());
	printf("nua_invite() llamado.\n");

	// Ejecuta el loop de eventos SIP (bloqueante)
	su_root_run(root);
	printf("su_root_run() completado (esto podría no alcanzarse hasta recibir una respuesta).\n");

    // Limpieza
	nua_destroy(nua);
	su_root_destroy(root);
	su_deinit();
	printf("Limpieza completada.\n");
	return (EXIT_SUCCESS);
}

/* PARA COMPILAR:
gcc -o demo4 demo4.c $(pkg-config --cflags --libs sofia-sip-ua)
./demo4 
*/

/* RESPUESTA:
vlorenzo:~/SIP-MCPTT-Core/demos $ ./demo4
Iniciando el programa...
su_init() completado.
su_root_create() completado.
nua_create() completado.
Intentando enviar el INVITE...
nua: nua_r_invite with invalid handle 0x55564bbc8fa0
nua_invite() llamado.
Callback received event: 10, status: 200, phrase: OK
Respuesta agente al INVITE: 200 OK
200 OK recibido. Enviando ACK...
Callback received event: 25, status: 200, phrase: Shutdown successful
Evento SIP:: 25, Shutdown successful
su_root_run() completado (esto podría no alcanzarse hasta recibir una respuesta).
Limpieza completada.
 */

/*
>> BYE : Agregar manejo del evento nua_i_bye: caso en el switch para el evento nua_i_bye
que maneja el mensaje BYE.

>> RESPUESTA 200 OK : Enviar respuesta 200 OK para el BYE: Dentro del caso nua_i_bye,
se envía una respuesta 200 OK usando nua_respond(nh, SIP_200_OK, TAG_END());

>> Handle para el INVITE: Se ha agregado una variable inv_handle para almacenar el handle
del INVITE.

>>Enviar CANCEL en respuesta a 180 Ringing: Dentro del caso nua_r_invite,
se envía un mensaje CANCEL cuando el estado es 180 Ringing usando nua_cancel(nh,
TAG_END());.

*/