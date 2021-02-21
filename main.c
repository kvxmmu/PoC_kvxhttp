#include <stdio.h>

#include "kvxhttp/server.h"

int main() {
    KString path = kstring_create("./html/", 0, 0);

    HTTPServer server = create_server(8080, INADDR_ANY,
            &path);

    start_server(&server);

    kstring_free(&path);
}
