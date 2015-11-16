#include <pebble.h>
#include "rsvp.h"

Window *window;
TextLayer *text_layer;

static void prv_single_click_cb(void *context) {
	static char test[] = "Hello World.  How are you today?  This is an RSVP test on Pebble. :D";
  	rsvp(test, text_layer, true);
}

static void prv_click_config_provider(Window *context) {
  	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) prv_single_click_cb);
}

void handle_init(void) {
	// Create a window and text layer
	window = window_create();
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
  	uint32_t text_width = bounds.size.w;
  	uint32_t text_height = 34;
  	text_layer = text_layer_create(GRect(0, bounds.size.h/2 - text_height/2,
        text_width, text_height));
	
	// Set the text, font, and text alignment
	text_layer_set_text(text_layer, "Press Select");
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));

	// Push the window
	window_stack_push(window, true);

	// Subscribe to button clicks
	window_set_click_config_provider(window, (ClickConfigProvider) prv_click_config_provider);
}

void handle_deinit(void) {
  	text_layer_destroy(text_layer);
  	window_destroy(window);
}

int main(void) {
  	handle_init();
  	app_event_loop();
  	handle_deinit();
}
