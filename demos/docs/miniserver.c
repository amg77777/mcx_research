#include <sofia-sip/nta.h>
#include <sofia-sip/su.h>
#include <sofia-sip/su_tag.h>
#include <sofia-sip/nua.h>
#include <sofia-sip/sip.h>
#include <sofia-sip/nua_tag.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void server_message_callback(nua_event_t event, int status,
                                  const char *phrase, nua_t *nua, void *context, nua_handle_t *nh,
                                  void *param, const struct sip_s *sip, tagi_t *tags)
{
    printf("server_message_callback fue llamada con evento: %d\n", event);
    const char *from = NULL;
    const char *content_type = NULL;
    const char *payload = NULL;
    // size_t payload_length = 0;

    if (event == nua_i_message) {
        tl_gets(tags,
                SIPTAG_FROM_STR_REF(from),
                SIPTAG_CONTENT_TYPE_STR_REF(content_type),
                SIPTAG_PAYLOAD_STR_REF(payload),
                TAG_END());

        // payload = (const char *)sip_get_body(sip);
        // payload_length = sip_get_body_length(sip);

        printf("\n--- Mensaje SIP MESSAGE Recibido ---\n");
        printf("De: %s\n", from);
        printf("Content-Type: %s\n", content_type);
        if (payload) {
            printf("Contenido:\n%s\n", payload);
        } else {
            printf("Contenido vacío.\n");
        }
        printf("--------------------------------------\n");
        nua_respond(nh, 200, "OK", TAG_END());
        nua_destroy(nua);
    } else if (event == nua_r_message) {
        printf("Respuesta al mensaje SIP: %d %s\n", status, phrase);
    }
}

int main(void)
{
    su_root_t *root;
    nua_t *nua;

    su_init();
    root = su_root_create(NULL);
    if (root == NULL) {
        fprintf(stderr, "Can't create root object\n");
        return EXIT_FAILURE;
    }

    nua = nua_create(root,
                   server_message_callback,
                   root,
                   NUTAG_URL("sip:127.0.0.1:5060"),
                   TAG_END());

    if (nua == NULL) {
        fprintf(stderr, "Can't create NUA object\n");
        su_root_destroy(root);
        return EXIT_FAILURE;
    }

    printf("Sofia-SIP miniserver started at sip:127.0.0.1:5060\n");

    su_root_run(root);

    nua_destroy(nua);
    su_root_destroy(root);
    su_deinit();

    return EXIT_SUCCESS;
}