#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>

static xcb_connection_t *conn;
static xcb_screen_t *screen;

int
main(int argc, char*argv[])
{
	int screen_num;
	conn = xcb_connect(NULL, &screen_num);

	if (xcb_connection_has_error(conn)) {
		fprintf(stderr, "Cannot open display\n");
		exit(-1);
	}

	/* Get first screen */
	// screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(conn));

	for (int i = 0; i < screen_num; i++)
		xcb_screen_next(&iter);

	screen = iter.data;

	xcb_grab_key(conn, 1, screen->root, XCB_MOD_MASK_1, XCB_NO_SYMBOL,
		XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);

	/* Define the application as window manager */
	const uint32_t masks[] = {
		XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
		XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_PROPERTY_CHANGE |
		XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
		XCB_EVENT_MASK_FOCUS_CHANGE
	};
	xcb_change_window_attributes(conn, screen->root, XCB_CW_EVENT_MASK, masks);

	/* Flush all request */
	xcb_flush(conn);

	/* Need to xcb_flush to validate error handler */
	xcb_aux_sync(conn);

	if (xcb_poll_for_event(conn) != NULL) {
		fprintf(stderr, "Another window manager is already running!\n");
		exit(1);
	}

	/* Flush all requests */
	xcb_flush(conn);

	xcb_generic_event_t *ev;
	int done;
	/* Enter the main loop */
	done = 0;
	while (!done && (ev = xcb_wait_for_event(conn))) {
		printf("loop\n");
		switch(ev->response_type) {
		/* (re)draw the window */
		case XCB_EXPOSE:
			printf("EXPOSE\n");
			break;
		/* Exit on keypress */
		case XCB_KEY_PRESS:
			printf("KEY PRESS\n");
			done = 1;
			break;
		case XCB_BUTTON_PRESS:
			printf("BUTTON PRESS\n");
			done = 1;
			break;
		default:
			//event_management(ev);
			//printf("The events = %s\n",xcb_event_get_label(ev->response_type));
		}
		free(ev);
	}
	/* Close connection to server */
	xcb_disconnect(conn);
	return 0;
}
