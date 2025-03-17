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

// Callback que maneja los eventos SIP
static void sip_invite_callback(nua_event_t event, int status,
	const char *phrase, nua_t *nua,
	void *context, nua_handle_t *nh,
	void *param, const struct sip_s *sip, tagi_t *tags) {
su_root_t *root = (su_root_t *)context;
printf("Callback received event: %d, status: %d, phrase: %s\n", event, status, phrase);

if (event == nua_i_invite) {
printf("INVITE enviado\n");
} else if (event == 10) { // nua_ok
printf("Evento OK recibido: %d %s\n", status, phrase);
if (status >= 200) {
printf("Final OK response received. Initiating NUA shutdown.\n");
nua_shutdown(nua); // Iniciar el apagado del NUA
}
} else if (event == nua_i_ack) {
printf("ACK recibido\n");
} else {
printf("Evento SIP: %d, %s\n", event, phrase);
nua_shutdown(nua);
sleep(1);
su_root_break(root);
}
}
int main(void) {
    su_root_t *root;
    nua_t *nua;

    // Inicializa la librería
    su_init();
    root = su_root_create(NULL);
    if (!root) {
        fprintf(stderr, "No se pudo crear el su_root\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "su_root creado\n");

    // Crea el agente SIP (NUA) usando el callback definido, pasando 'root' como contexto
    nua = nua_create(root, sip_invite_callback, root,
                     /*NUTAG_URL(SIP_PROXY),*/ TAG_END());
    if (!nua) {
        fprintf(stderr, "No se pudo crear el agente SIP (nua)\n");
        su_root_destroy(root);
        return EXIT_FAILURE;
    }
    fprintf(stdout, "NUA creado\n");

    // Envía un INVITE al destino (SIP_DEST)
    // Se registra la identidad del caller y su contacto
    nua_invite((nua_handle_t *)nua,
               NUTAG_ALLOW(SIP_IDENTITY),
               SIPTAG_CONTACT_STR(SIP_CONTACT_STR),
               SIPTAG_TO_STR("sip:callee@127.0.0.1:5060"),
               TAG_END());
    fprintf(stdout, "nua invite\n");
    // Ejecuta el loop de eventos SIP (bloqueante)
    su_root_run(root);
    fprintf(stdout, "loop finalize\n");

    sleep(1);

    // Limpieza
    nua_destroy(nua);
    su_root_destroy(root);
    su_deinit();

    return EXIT_SUCCESS;
}
