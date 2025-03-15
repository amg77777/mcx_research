#include <sofia-sip/nta.h>
#include <sofia-sip/su_configure.h>
#include <sofia-sip/nta_tport.h>
#include <sofia-sip/nua.h>
#include <stdio.h>
#include <stdlib.h>
#include <sofia-sip/su_tag.h>

#define SIP_IDENTITY "sip:user@example.com"
#define SIP_CONTACT "sip:user@client.example.com"
#define SIP_PROXY "sip:proxy.example.com"

void sip_event_callback(enum nua_event_e event, int status,
                        char const *phrase, struct nua_s *nua,
                        void *context, struct nua_handle_s *nh,
                        void *data, const struct sip_s *sip,
                        tagi_t *tags) {
    printf("SIP Event: %d (%s) - Status: %d %s\n", event, nua_event_name(event), status, phrase);
}

int main() {
    su_root_t *root;
    nua_t *nua;
    su_init();
    root = su_root_create(NULL);
    if (!root) {
        fprintf(stderr, "Failed to create su_root\n");
        return EXIT_FAILURE;
    }
    // Modified nua_create call
    nua = nua_create(root, sip_event_callback, NULL, /* NUTAG_URL(SIP_PROXY), */ TAG_END());
    if (!nua) {
        fprintf(stderr, "Failed to create SIP agent\n");
        su_root_destroy(root);
        return EXIT_FAILURE;
    }
    nua_register((nua_handle_t *)nua, NUTAG_ALLOW(SIP_IDENTITY), NTATAG_CONTACT(SIP_CONTACT), TAG_END());
    su_root_run(root);
    nua_destroy(nua);
    su_root_destroy(root);
    su_deinit();
    return EXIT_SUCCESS;
}