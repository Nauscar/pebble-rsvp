#include <pebble.h>
#include "rsvp.h"

Window *window;
TextLayer *text_layer;

static void prv_single_click_cb(void *context) {
	static char test[] = "Hello World.  How are you today?  This is an RSVP test on Pebble. :D";
  	rsvp_display(test);
}

static void prv_click_config_provider(Window *context) {
  	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) prv_single_click_cb);
}

void handle_init(void) {
	// Create a window and text layer
	window = window_create();
	
	Layer *window_layer = window_get_root_layer(window);
	rsvp_init(window_layer, true);

	// Push the window
	window_stack_push(window, true);

	// Subscribe to button clicks
	window_set_click_config_provider(window, (ClickConfigProvider) prv_click_config_provider);
}

void handle_deinit(void) {
	rsvp_deinit();
  	window_destroy(window);
}

int main(void) {
  	handle_init();
  	app_event_loop();
  	handle_deinit();
}
