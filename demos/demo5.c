#include <sofia-sip/nta.h>
#include <sofia-sip/url.h>
#include <sofia-sip/su.h>
#include <sofia-sip/su_tag.h>
#include <sofia-sip/su_alloc.h>
#include <sofia-sip/su_wait.h>
#include <sofia-sip/nua.h>
#include <sofia-sip/sip.h>
#include <sofia-sip/sip_header.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> // Necesario para strcpy

#define SIP_IDENTITY "sip:caller@127.0.0.1"
#define SIP_CONTACT_STR "sip:caller@127.0.0.1"
#define SIP_DEST     "sip:127.0.0.1:5060"
#define SIP_PROXY    "sip:proxy@127.0.0.1"

#ifndef SIP_200_OK
#define SIP_200_OK 200
#endif

// Definición para el Content-Type de los mensajes
#define MESSAGE_CONTENT_TYPE "text/plain"

// Definición de la estructura para el contexto de la aplicación
typedef struct {
    su_home_t home[1];
} app_context_t;

// Función para enviar un mensaje SIP MESSAGE
void send_sip_message(nua_t *nua, su_root_t *root, const char *to_uri, const char *message) {
    app_context_t *app_ctx = (app_context_t *)su_root_magic(root);
    if (app_ctx) {
        url_t *url = url_make(app_ctx->home, (const char *)to_uri);
        if (url) {
            printf("Enviando mensaje a: %s con contenido: %s\n", to_uri, message);
            sip_to_t *sip_to = sip_to_create(app_ctx->home, url);
            if (sip_to) {
                nua_message((nua_handle_t *)nua,
                            SIPTAG_TO(sip_to),
                            SIPTAG_CONTENT_TYPE_STR(MESSAGE_CONTENT_TYPE),
                            SIPTAG_PAYLOAD_STR(message),
                            TAG_END());
                su_free(app_ctx->home, sip_to); // Liberamos la estructura sip_to_t
            } else {
                printf("Error al crear la dirección SIP (To) desde la URL.\n");
            }
            su_free(app_ctx->home, url); // Liberamos la estructura url_t
        } else {
            printf("Error al crear la URL para: %s\n", to_uri);
        }
    } else {
        printf("Error al obtener el contexto de la aplicación.\n");
    }
}

nua_handle_t    *inv_handle = NULL; // Handle para el INVITE

// Callback que maneja los eventos SIP
static void sip_invite_callback(nua_event_t event, int status,
       const char *phrase, nua_t *nua, void *context, nua_handle_t *nh,
       void *param, const struct sip_s *sip, tagi_t *tags)
{
    su_root_t *root = (su_root_t *)context;
    printf("Callback received event: %d, status: %d, phrase: %s\n", event, status, phrase);

    if (event == nua_i_invite) // Evento de INVITE entrante
       printf("INVITE recibido, request-response\n");

    else if (event == nua_r_invite)
    {
        printf("Respuesta agente al INVITE: %d %s\n", status, phrase);
       if (status == 180)
       {
          printf("Ringing...\n");
       }
       else if (status == 200)
       {
            // El INVITE esta ok, info ok
          printf("200 OK recibido. Enviando ACK...\n");
          nua_ack(nh, TAG_END()); // Send ACK for 200 OK
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
        // Potentially initiate shutdown after BYE is handled
        // nua_shutdown(nua);
        su_root_break(root); // Break the event loop after handling BYE
    }
    // Handling for MESSAGE
    else if (event == nua_i_message) {
        const char *from = NULL;
        const char *content_type = NULL;
        const char *payload = NULL;
        // size_t payload_length = 0;

        tl_gets(tags,
                SIPTAG_FROM_STR_REF(from),
                SIPTAG_CONTENT_TYPE_STR_REF(content_type),
                SIPTAG_PAYLOAD_STR_REF(payload),
                TAG_END());

        printf("\n--- Mensaje SIP MESSAGE Recibido ---\n");
        printf("De: %s\n", from);
        printf("Content-Type: %s\n", content_type);
        if (payload) {
            printf("Contenido:\n%s\n", payload);
        } else {
            printf("Contenido vacío.\n");
        }
        printf("--------------------------------------\n");
    } else if (event == nua_r_message) {
        printf("Respuesta al mensaje SIP MESSAGE: %d %s\n", status, phrase);
        // nua_shutdown(nua); // Considerar si esto es apropiado aquí
    }
    else
    {
       printf("Evento SIP: %d, %s\n", event, phrase);
       // nua_shutdown(nua); // Considerar si esto es apropiado aquí
       sleep(1);
       su_root_break(root);
    }
}

int main(void) {
    app_context_t app_ctx;
    su_root_t  *root;
    nua_t     *nua;
    char command[256];
    char to_uri[100];
    char message[156];

    printf("Iniciando el programa...\n");
    su_init();
    su_home_init(app_ctx.home); // Inicializa la memory home
    printf("su_init() completado.\n");
    root = su_root_create(&app_ctx); // Pasa la estructura de contexto a su_root_create
    if (!root) {
       fprintf(stderr, "No se pudo crear el su_root\n");
       return (EXIT_FAILURE);
    }
    printf("su_root_create() completado.\n");
    nua = nua_create(root,
                   sip_invite_callback,
                   root, // El contexto del callback sigue siendo root por ahora
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
    nua_handle_t *invite_handle = nua_handle(nua, NULL, TAG_END()); // Obtain a handle for the INVITE
    if (invite_handle) {
        nua_invite(invite_handle,
                 NUTAG_ALLOW(SIP_IDENTITY),
                 SIPTAG_CONTACT_STR(SIP_CONTACT_STR),
                 SIPTAG_TO_STR(SIP_DEST), // Use the defined SIP_DEST
                 TAG_END());
        printf("nua_invite() llamado.\n");
        inv_handle = invite_handle; // Store the handle
    } else {
        fprintf(stderr, "No se pudo crear el handle para INVITE.\n");
    }


    printf("\n--- Cliente SIP (con Mensajería) ---\n");
    printf("Ingresa 'enviar <uri> <mensaje>' para enviar un mensaje.\n");
    printf("Ingresa 'salir' para salir.\n");
    printf("El programa también intentará enviar un INVITE.\n\n");

    while (1) {
        printf("> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; // Error o EOF
        }
        command[strcspn(command, "\n")] = 0; // Eliminar el newline

        if (strcmp(command, "salir") == 0) {
            break;
        }
        else if (strncmp(command, "enviar ", 7) == 0) {
            if (sscanf(command + 7, "%99s %[^\n]", to_uri, message) == 2) {
                send_sip_message(nua, root, to_uri, message);
            } else {
                printf("Error: Formato incorrecto. Usa 'enviar <uri> <mensaje>'.\n");
            }
        }
    }

    // Ejecuta el loop de eventos SIP (bloqueante)
    su_root_run(root);
    printf("su_root_run() completado.\n");

    // Limpieza
    if (inv_handle) {
        nua_handle_destroy(inv_handle); // Destroy the INVITE handle
    }
    nua_destroy(nua);
    su_root_destroy(root);
    su_deinit();
    printf("Limpieza completada.\n");
    return (EXIT_SUCCESS);
}