#define LIBSSH_STATIC 1
#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>

static char *_server="u38989.ssh.masterhost.ru";
static char *_user="u38989";
static char *_password="amitin9sti";

int main()
{
	ssh_session my_ssh_session;
  	int rc;

	// Open session and set options
	my_ssh_session = ssh_new();
	if (my_ssh_session == NULL) {
		exit(-1);
	}
	
	ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, _server);
  
	// Connect to server
	rc = ssh_connect(my_ssh_session);
	if (rc != SSH_OK) {
    		fprintf(stderr, "Error connecting to %s: %s\n", _server, ssh_get_error(my_ssh_session));
    		ssh_free(my_ssh_session);
    		exit(-1);
	}

	// Authenticate ourselves
	ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, _user);
	rc = ssh_userauth_password(my_ssh_session, NULL, _password);
	if (rc != SSH_AUTH_SUCCESS) {
		fprintf(stderr, "Error authenticating with password: %s\n", ssh_get_error(my_ssh_session));
		ssh_disconnect(my_ssh_session);
		ssh_free(my_ssh_session);
		exit(-1);
	}
	fprintf(stderr, "Authentification passed Ok.\n");
	// ...
	ssh_disconnect(my_ssh_session);
	ssh_free(my_ssh_session);
}