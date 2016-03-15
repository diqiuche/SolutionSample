/*
This exmple program provides a trivial server program that listens for TCP
connections on port 9995.  When they arrive, it writes a short message to
each client connection, and closes each connection once it is flushed.

Where possible, it exits cleanly in response to a SIGINT (ctrl-c).
*/
#ifndef HELLO_WORLD_TEST_H_
#define HELLO_WORLD_TEST_H_

#include <string.h>
#include <errno.h>
#include <stdio.h>
#ifndef WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#define MAX_BUF_SIZE 1024
static const char MESSAGE[] = "Hello, World!\n";

static const int PORT = 61616;

static void listener_cb(struct evconnlistener *, evutil_socket_t,
struct sockaddr *, int socklen, void *);
static void conn_readcb(struct bufferevent *, void *);
static void conn_writecb(struct bufferevent *, void *);
static void conn_eventcb(struct bufferevent *, short, void *);

int
	TestHelloWorld(int argc, char **argv)
{
	struct event_base *base;
	struct evconnlistener *listener;
	struct event *signal_event;

	struct sockaddr_in sin;
#ifdef WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);

	listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
		LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
		(struct sockaddr*)&sin,
		sizeof(sin));

	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}


	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_base_free(base);

	printf("done\n");
	return 0;
}

static void
	listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = (struct event_base *)user_data;
	struct bufferevent *bev;

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, NULL);
	bufferevent_disable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);

	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}


//bufferevent *bev满时的写回调函数
static void
	conn_readcb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output;    
	output = bufferevent_get_output(bev);//其实就是取出bufferevent中的output  

	int now_read = 0;
	char buf[MAX_BUF_SIZE];
	do{
		memset(buf, 0, MAX_BUF_SIZE);
		now_read = bufferevent_read(bev, buf, MAX_BUF_SIZE-1);
		//buf[MAX_BUF_SIZE] = '\0';
		cout << buf << endl;
	} while (now_read == MAX_BUF_SIZE-1);

	if (string(buf) == "start")
	{

	}

}

static void
	conn_writecb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
		bufferevent_free(bev);
	}
}

static void
	conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
			strerror(errno));/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	* timeouts */
	bufferevent_free(bev);
}

#endif