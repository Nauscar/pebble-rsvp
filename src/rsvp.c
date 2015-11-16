#include <pebble.h>
#include "rsvp.h"

#define DEFAULT_TIMEOUT_MS 240 //TODO: enum timeouts for WPM setting

#define FONT_TYPE_LOW FONT_KEY_GOTHIC_14
#define FONT_TYPE_MED FONT_KEY_GOTHIC_18
#define FONT_TYPE_HIGH FONT_KEY_GOTHIC_24
#define FONT_TYPE_MAX FONT_KEY_GOTHIC_28

static TextLayer *s_text_layer;
static bool s_center;
static AppTimer *s_timer;
static char* s_token;

enum FONT_PPC_SCALE { /*PPC = Pixels Per Character*/
    FONT_PPC_LOW = 6,
    FONT_PPC_MED = 7,
    FONT_PPC_HIGH = 9,
    FONT_PPC_MAX = 11
};

enum FONT_HEIGHT_SCALE {
    FONT_HEIGHT_LOW = 16,
    FONT_HEIGHT_MED = 20,
    FONT_HEIGHT_HIGH = 28, 
    FONT_HEIGHT_MAX = 34
};

char* strtok(char *s1, const char *delimit)
{
    static char *lastToken = NULL; /* UNSAFE SHARED STATE! */
    char *tmp;

    /* Skip leading delimiters if new string. */
    if ( s1 == NULL ) {
        s1 = lastToken;
        if (s1 == NULL)         /* End of story? */
            return NULL;
    } else {
        s1 += strspn(s1, delimit);
    }

    /* Find end of segment */
    tmp = strpbrk(s1, delimit);
    if (tmp) {
        /* Found another delimiter, split string and save state. */
        *tmp = '\0';
        lastToken = tmp + 1;
    } else {
        /* Last segment, remember that. */
        lastToken = NULL;
    }

    return s1;
}

static void prv_get_scale_params(char *word, char **font, int16_t *height) {
    Layer *layer = text_layer_get_layer(s_text_layer);
    const uint16_t width = layer_get_frame(layer).size.w;

    const uint32_t length = strlen(word);
    if (length * FONT_PPC_MAX <= width){
        *font = FONT_TYPE_MAX;
        *height = FONT_HEIGHT_MAX;
    } 
    else if (length * FONT_PPC_HIGH <= width) {
        *font = FONT_TYPE_HIGH;
        *height = FONT_HEIGHT_HIGH;
    }
    else if (length * FONT_PPC_MED <= width) {
        *font = FONT_TYPE_MED;
        *height = FONT_HEIGHT_MED;
    } else {
        *font = FONT_TYPE_LOW;
        *height = FONT_HEIGHT_LOW;
    }
}
    
static void prv_rsvp_display(char *word) {
    char *font;
    int16_t height;
    prv_get_scale_params(word, &font, &height);
    
    Layer *layer = text_layer_get_layer(s_text_layer);
    GRect frame = layer_get_frame(layer);
    int y_delta = frame.size.h - height;
    frame.size.h = height;
    layer_set_frame(layer, frame);
    
    if(s_center) {
        GRect bounds = layer_get_bounds(layer);
        bounds.size.h += (y_delta / 2);
        layer_set_bounds(layer, bounds);
    }
    
    text_layer_set_font(s_text_layer, fonts_get_system_font(font));
    text_layer_set_text(s_text_layer, word);
}

static void prv_token_cb(void *data) {
    if (s_token == NULL) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Token received: NULL");
        prv_rsvp_display("");
        return;
    }

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Token received: %s", s_token);
    prv_rsvp_display(s_token);

    uint32_t timeout_ms = DEFAULT_TIMEOUT_MS;
    uint32_t length = strlen(s_token);
    char tmp = s_token[length - 1];
    if (tmp == ',' || tmp == '.' || tmp == '!' || tmp == '?' || tmp == ';') {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Punctuation found: %c", tmp);
        timeout_ms += DEFAULT_TIMEOUT_MS;
    }

    s_token = strtok(NULL, " ");
    s_timer = app_timer_register(timeout_ms, prv_token_cb, NULL);
}

void rsvp(char *input, TextLayer *text_layer, bool center) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Input received: %s", input);

    s_text_layer = text_layer;
    s_center = center;
    
    static char string[256];
    strncpy(string, input, sizeof(string));

    s_token = strtok(string, " ");
    prv_token_cb(NULL);
}