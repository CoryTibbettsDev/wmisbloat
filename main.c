#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include <xcb/xcb.h>

static xcb_connection_t *conn;
static xcb_screen_t *screen;

int
main(int argc, char*argv[])
{
	int screen_num;
	conn = xcb_connect(NULL, &screen_num);

	if (xcb_connection_has_error(conn)) {
		fprintf(stderr, "Cannot open display, xcb connection has error!\n");
		exit(EXIT_FAILURE);
	}

	/* Get first screen */
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(conn));

	for (int i = 0; i < screen_num; i++)
		xcb_screen_next(&iter);

	screen = iter.data;

	/* Define the application as window manager */
	const uint32_t masks[] = {
		XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_PROPERTY_CHANGE |
		XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
		XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_EXPOSURE |
		XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
		XCB_EVENT_MASK_KEY_PRESS
	};
	xcb_change_window_attributes(conn, screen->root, XCB_CW_EVENT_MASK, masks);

	/* Mouse buttons */
	xcb_grab_button(conn, 0, screen->root,
		XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
		XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, screen->root, XCB_NONE, 1, XCB_MOD_MASK_1);
	xcb_grab_button(conn, 0, screen->root,
		XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
		XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, screen->root, XCB_NONE, 3, XCB_MOD_MASK_1);
	xcb_grab_button(conn, 0, screen->root,
		XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
		XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, screen->root, XCB_NONE, 1, XCB_NONE);

	/* Keyboard keys */
	// xcb_grab_key(conn, 0, screen->root,
	// 	XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
	// 	XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, screen->root, XCB_NONE, 1, XCB_NONE);

	/* Flush all requests */
	xcb_flush(conn);

	xcb_generic_event_t *ev;
	/* Enter the main loop */
	while((ev = xcb_wait_for_event(conn))) {
		printf("loop\n");
		switch (ev->response_type & ~0x80) {
			case XCB_EXPOSE:
				printf("EXPOSE\n");
				break;
			/* Keyboard Keys */
			case XCB_KEY_PRESS:
				printf("KEY PRESS\n");
				xcb_key_press_event_t *key;
				key = (xcb_key_press_event_t *) ev;
				printf("Button press = %d, Modifier = %d\n", key->detail, key->state);
				break;
			/* Mouse Buttons */
			case XCB_BUTTON_PRESS:
				printf("BUTTON PRESS\n");
				xcb_button_press_event_t *e;
				e = (xcb_button_press_event_t *) ev;
				printf("Button press = %d, Modifier = %d\n", e->detail, e->state);
				break;
			default:
				printf("Default\n");
		}
		free(ev);
	}
	/* Close connection to server */
	xcb_disconnect(conn);
	return 0;
}
