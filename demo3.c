#include <sofia-sip/nta.h>
#include <sofia-sip/su.h>
#include <sofia-sip/su_tag.h>
#include <sofia-sip/nua.h>
#include <sofia-sip/sip.h>
#include <stdio.h>
#include <stdlib.h>

#define SIP_IDENTITY "sip:caller@192.168.1.10"
#define SIP_CONTACT   "sip:caller@127.0.0.1"
#define SIP_DEST    "sip:callee@127.0.0.1"
#define SIP_PROXY    "sip:proxy@192.168.1.30"


// Callback que maneja los eventos SIP
static void sip_invite_callback(nua_event_t event, int status,
                                const char *phrase, nua_t *nua,
                                void *context, nua_handle_t *nh,
                                void *param, const struct sip_s *sip, tagi_t *tags) {
    switch(event) {
        case nua_i_invite: // Evento de INVITE saliente
            printf("INVITE enviado\n");
            break;
        case nua_r_invite: // Respuesta al INVITE
            printf("Respuesta INVITE: %d %s\n", status, phrase);
            if(status == 180) {
                printf("Ringing...\n");
            } else if(status == 200) {
                printf("200 OK recibido. Enviando ACK...\n");
                // Enviar ACK usando nua_ack()
                nua_ack(nh, TAG_END());
            }
            break;
        case nua_i_ack:
            printf("ACK recibido\n");
            break;
        default:
            printf("Evento SIP: %d, %s\n", event, phrase);
            break;
    }
}

int main(void) {
    su_root_t *root;
    nua_t *nua;

    printf("Iniciando el programa...\n");

    // Inicializa la librería
    su_init();
    printf("su_init() completado.\n");

    root = su_root_create(NULL);
    if (!root) {
        fprintf(stderr, "No se pudo crear el su_root\n");
        return EXIT_FAILURE;
    }
    printf("su_root_create() completado.\n");

    // Crea el agente SIP (NUA) usando el callback definido
    nua = nua_create(root, sip_invite_callback, NULL,
                     /*NUTAG_URL(SIP_PROXY),*/ TAG_END());
    if (!nua) {
        fprintf(stderr, "No se pudo crear el agente SIP (nua)\n");
        su_root_destroy(root);
        return EXIT_FAILURE;
    }
    printf("nua_create() completado.\n");

    printf("Intentando enviar el INVITE...\n");
    // Envía un INVITE al destino (SIP_DEST)
    // Se registra la identidad del caller y su contacto
    nua_invite((nua_handle_t *)nua,
               NUTAG_ALLOW(SIP_IDENTITY),
               NTATAG_CONTACT(SIP_CONTACT),
               SIPTAG_TO_STR(SIP_DEST),
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

    return EXIT_SUCCESS;
}