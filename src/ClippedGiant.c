#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x54, 0x56, 0x7D, 0xFD, 0x2D, 0x59, 0x48, 0xA9, 0x83, 0xB1, 0x61, 0x46, 0x96, 0x03, 0x39, 0x51 }
PBL_APP_INFO(MY_UUID,
             "ClippedGiant", "Jnm",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define CX 72
#define CY 84
#define SCREENW 144
#define HSPACE 16
#define VSPACE 16
#define SCREENH 168

const int hourImage[10] = {
    RESOURCE_ID_IMAGE_H0, RESOURCE_ID_IMAGE_H1, RESOURCE_ID_IMAGE_H2, RESOURCE_ID_IMAGE_H3,
    RESOURCE_ID_IMAGE_H4, RESOURCE_ID_IMAGE_H5, RESOURCE_ID_IMAGE_H6, RESOURCE_ID_IMAGE_H7,
    RESOURCE_ID_IMAGE_H8, RESOURCE_ID_IMAGE_H9
};

Window window;
Layer layer;
TextLayer minuteLayerBlack, minuteLayerWhite[4];
GFont minuteFont;
char minutes[] = "  ";

int h1, h2, m1, m2;

BmpContainer digitBmpContainer[10];

void updateLayer(Layer *me, GContext* ctx) {
    GRect frame1, frame2;
    int width;
    
    frame1 = layer_get_frame(&digitBmpContainer[h1].layer.layer);
    frame2 = layer_get_frame(&digitBmpContainer[h2].layer.layer);
    width = frame1.size.w + frame2.size.w + HSPACE;
    
    // tens hour digit
    frame1.origin.x = CX - width/2;
    frame1.origin.y = CY - frame1.size.h - VSPACE/2;
    graphics_draw_bitmap_in_rect(ctx, &digitBmpContainer[h1].bmp, frame1);
    
    // units hour digit
    frame2.origin.x = frame1.origin.x + frame1.size.w + HSPACE;
    frame2.origin.y = CY - frame2.size.h - VSPACE/2;
    graphics_draw_bitmap_in_rect(ctx, &digitBmpContainer[h2].bmp, frame2);
    
    frame1 = layer_get_frame(&digitBmpContainer[m1].layer.layer);
    frame2 = layer_get_frame(&digitBmpContainer[m2].layer.layer);
    width = frame1.size.w + frame2.size.w + HSPACE;

    // tens minute digit
    frame1.origin.x = CX - width/2;
    frame1.origin.y = CY + VSPACE/2;
    graphics_draw_bitmap_in_rect(ctx, &digitBmpContainer[m1].bmp, frame1);
    
    // units minute digit
    frame2.origin.x = frame1.origin.x + frame1.size.w + HSPACE;
    frame2.origin.y = CY + VSPACE/2;
    graphics_draw_bitmap_in_rect(ctx, &digitBmpContainer[m2].bmp, frame2);
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *evt) {
    PblTm now;
    get_time(&now);

    h1 = now.tm_hour/10;
    h2 = now.tm_hour%10;
    m1 = now.tm_min/10;
    m2 = now.tm_min%10;

    layer_mark_dirty(&layer);
}

void handle_init(AppContextRef ctx) {
    PblTm now;
    int i;
	int dx[4] = { -1, 1, 1, -1 };
	int dy[4] = { -1, -1, 1, 1 };
   
    window_init(&window, "Clipped");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);
    
    resource_init_current_app(&APP_RESOURCES);
    
    get_time(&now);
    
    h1 = now.tm_hour/10;
    h2 = now.tm_hour%10;
    m1 = now.tm_min/10;
    m2 = now.tm_min%10;

    for (i=0; i<10; i++) {
        bmp_init_container(hourImage[i], &digitBmpContainer[i]);
    }
    
    layer_init(&layer, window.layer.frame);
    layer.update_proc = &updateLayer;
    layer_add_child(&window.layer, &layer);
}

void handle_deinit(AppContextRef ctx) {
    int i;
        
    for (i=0; i<10; i++) {
        bmp_deinit_container(&digitBmpContainer[i]);
    }
}

void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,
        .deinit_handler = &handle_deinit,
        
		.tick_info = {
			.tick_handler = &handle_tick,
			.tick_units   = MINUTE_UNIT
		}
    };
    app_event_loop(params, &handlers);
}
