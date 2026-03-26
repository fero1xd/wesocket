#include "server.h"
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

void run_loop(weserver_t server[static 1]);

void wesocket_run(weserver_t w) {
  printf("Running wesocket server on: %s:%d\n", w.config.addr, w.config.port);

  w.fd = socket(AF_INET, SOCK_STREAM, 0);
  assert(w.fd >= 0);

  printf("[+]TCP server socket created.\n");

  struct sockaddr_in server_addr = {.sin_family = AF_INET,
                                    .sin_port = htons(w.config.port),
                                    .sin_addr.s_addr =
                                        inet_addr(w.config.addr)};

  assert(bind(w.fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0);
  printf("[+]Bind to the port number: %s:%d\n", w.config.addr, w.config.port);
  listen(w.fd, 1);
  printf("[+]Listening...\n");

  run_loop(&w);
}

void run_loop(weserver_t server[static 1]) {
  struct sockaddr_in client_addr;
  socklen_t addr_len;

  while (1) {
    addr_len = sizeof(client_addr);

    socket_fd client =
        accept(server->fd, (struct sockaddr *)&client_addr, &addr_len);
    assert(client >= 0);

    printf("Connected client: %d\n", client);
    write(client, "Hello World\n", 12);
    close(client);
  };
}
