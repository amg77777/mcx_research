#include <sofia-sip/nta.h>
#include <sofia-sip/nua.h>
#include <sofia-sip/sip.h>
#include <sofia-sip/su.h>
#include <sofia-sip/su_tag.h>
#include <stdio.h>
#include <stdlib.h>

#define SIP_IDENTITY "sip:caller@192.168.1.10"
#define SIP_CONTACT "sip:caller@127.0.0.1"
#define SIP_DEST "sip:callee@127.0.0.1"
#define SIP_PROXY "sip:proxy@192.168.1.30"

#ifndef SIP_200_OK
#define SIP_200_OK 200
#endif

nua_handle_t	*inv_handle = NULL; // Handle para el INVITE

// Callback que maneja los eventos SIP
static void	sip_invite_callback(nua_event_t event, int status,
		const char *phrase, nua_t *nua, void *context, nua_handle_t *nh,
		void *param, const struct sip_s *sip, tagi_t *tags)
{
	switch (event)
	{
	case nua_i_invite: // Evento de INVITE entrante
		printf("INVITE recibido\n");
		break ;
	case nua_r_invite: // Respuesta al INVITE
		printf("Respuesta INVITE: %d %s\n", status, phrase);
		if (status == 180)
		{
			printf("Ringing...\n");
			// Enviar CANCEL para la llamada en progreso
			printf("Enviando CANCEL...\n");
			nua_cancel(nh, TAG_END());
		}
		else if (status == 200)
		{
			printf("200 OK recibido. Enviando ACK...\n");
			// Enviar ACK usando nua_ack()
			nua_ack(nh, TAG_END());
		}
		break ;
	case nua_i_ack:
		printf("ACK recibido\n");
		break ;
	case nua_i_bye: // Evento de BYE entrante
		printf("BYE recibido, terminando la llamada.\n");
        // Enviar respuesta 200 OK para el BYE
        nua_respond(nh, SIP_200_OK, "OK", TAG_END());
		break ;
	default:
		printf("Evento SIP: %d, %s\n", event, phrase);
		break ;
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
						NULL,
						/*NUTAG_URL(SIP_PROXY),*/ TAG_END());
	if (!nua)
	{
		fprintf(stderr, "No se pudo crear el agente SIP (nua)\n");
		su_root_destroy(root);
		return (EXIT_FAILURE);
	}
	printf("nua_create() completado.\n");
	printf("Intentando enviar el INVITE...\n");
	// Envía un INVITE al destino (SIP_DEST)
	// Se registra la identidad del caller y su contacto
	inv_handle = nua_handle(nua, SIPTAG_TO_STR(SIP_DEST), TAG_END());
	nua_invite(inv_handle, NUTAG_ALLOW(SIP_IDENTITY),
		NTATAG_CONTACT(SIP_CONTACT), SIPTAG_TO_STR(SIP_DEST), TAG_END());
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
niciando el programa...
su_init() completado.
su_root_create() completado.
nua_create() completado.
Intentando enviar el INVITE...
Segmentation fault
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