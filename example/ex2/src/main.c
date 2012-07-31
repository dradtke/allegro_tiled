
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_tiled.h>

#define FPS 60
#define DEBUG 0
#define MAP_FOLDER "data/maps"

int screen_width, screen_height;

TILED_MAP *map;
ALLEGRO_BITMAP *egg;
float egg_x, egg_y;
float map_x, map_y;
int egg_width, egg_height;
float move_speed;
float fall_speed;
float jump_start;
float gravity;
bool jumping;

/*
 * Utility debug method
 * Used like printf, but only prints if DEBUG is set
 */
void debug(const char *format, ...)
{
	if (!DEBUG)
		return;

	va_list argptr;
	va_start(argptr, format);
	vprintf(format, argptr);
	printf("\n");
}

/*
 * Draw the map and egg.
 */
void flip()
{
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_map(map, map_x, map_y, screen_width, screen_height);
	al_draw_bitmap(egg, egg_x, egg_y, 0);
	al_flip_display();
}

bool collide(TILED_MAP *map, char id)
{
	char *p = al_get_tile_property(al_get_tile_for_id(map, id), "collide", "false");
	if (!strcmp(p, "true")) {
		return true;
	} else {
		return false;
	}
}

/*
 * Application entry point
 */
int main(int argc, char *argv[])
{
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_KEYBOARD_STATE keyboard_state;

	bool running = true;
	bool redraw = true;
	bool reload = false;

	screen_width = 640;
	screen_height = 480;

	move_speed = 6;

	//{{{ initialization
	
	// Initialize allegro
	if (!al_init()) {
		fprintf(stderr, "Failed to initialize allegro.\n");
		return 1;
	}

	// Initialize allegro_image addon
	if (!al_init_image_addon()) {
		fprintf(stderr, "Failed to initialize image addon.\n");
		return 1;
	}

	// Initialize the timer
	timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		fprintf(stderr, "Failed to create timer.\n");
		return 1;
	}

	// Install the keyboard
	if (!al_install_keyboard()) {
		fprintf(stderr, "Failed to install keyboard.\n");
		return 1;
	}

	// Create the display
	display = al_create_display(screen_width, screen_height);
	if (!display) {
		fprintf(stderr, "Failed to create display.\n");
		return 1;
	}

	// Create the event queue
	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "Failed to create event queue.\n");
		return 1;
	}

	// Register event sources
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	//}}}
	
	// Start the timer
	al_start_timer(timer);

	// Parse the map
	map = al_open_map(MAP_FOLDER, "level1.tmx");
	map_x = 0, map_y = 0;

	if (!(egg = al_load_bitmap("data/sprites/egg.png"))) {
		fprintf(stderr, "Failed to load egg sprite.\n");
		return 1;
	}

	egg_x = 50;
	egg_y = 200;
	egg_width = al_get_bitmap_width(egg);
	egg_height = al_get_bitmap_height(egg);
	gravity = 0.2;
	jumping = true;
	fall_speed = 0;
	jump_start = -6;

	// Draw the map
	flip();

	// Main loop
	while (running) {
		ALLEGRO_EVENT event;
		ALLEGRO_TIMEOUT	timeout;

		// Initialize the timeout (not the game's clock, I think)
		al_init_timeout(&timeout, 0.06);

		// Fetch the event (if one exists)
		bool get_event = al_wait_for_event_until(event_queue, &event, &timeout);

		// Handle the event
		if (get_event) {
			switch (event.type) {
				case ALLEGRO_EVENT_TIMER:
					// is an arrow key being held?
					al_get_keyboard_state(&keyboard_state);
					if (al_key_down(&keyboard_state, ALLEGRO_KEY_RIGHT)) {
						egg_x += 2;
					}
					else if (al_key_down(&keyboard_state, ALLEGRO_KEY_LEFT)) {
						egg_x -= 2;
					}

					if (al_key_down(&keyboard_state, ALLEGRO_KEY_SPACE)) {
						if (!jumping) {
							jumping = true;
							fall_speed = jump_start;
						}
					}

					redraw = true;
					break;
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
					running = false;
					break;
				case ALLEGRO_EVENT_KEY_DOWN:
					// ignore
					break;
				case ALLEGRO_EVENT_KEY_UP:
					// ignore
					break;
				case ALLEGRO_EVENT_KEY_CHAR:
					if (event.keyboard.keycode == ALLEGRO_KEY_SPACE)
						reload = true;
					break;
				default:
					fprintf(stderr, "Unsupported event received: %d\n", event.type);
					break;
			}
		}

		// non-event logic
		if (jumping) {
			fall_speed += gravity;
			if (fall_speed > 6)
				fall_speed = 6;

			float new_y = egg_y + fall_speed;

			if (fall_speed > 0) {
				int tile_x = egg_x / map->tile_width;
				int tile_bottom = (new_y + egg_height) / map->tile_height;
				char *tiles = al_get_tiles(map, tile_x, tile_bottom);
				int i;
				for (i = 0; tiles[i] > -1; i++) {
					if (collide(map, tiles[i])) {
						jumping = false;
						egg_y = (tile_bottom * map->tile_height) - egg_height;
					}
				}

				al_free(tiles);
			}

			if (jumping)
				egg_y = new_y;
		}
		else {
			int right = egg_x + egg_width;
			int left = egg_x;
			int bottom = egg_y + egg_height + 1;

			int i;
			char *tiles_botright = al_get_tiles(map, right/map->tile_width, bottom/map->tile_height);
			bool botright_floating = true;
			for (i = 0; tiles_botright[i] > -1; i++) {
				if (collide(map, tiles_botright[i])) {
					botright_floating = false;
					break;
				}
			}

			char *tiles_botleft = al_get_tiles(map, left/map->tile_width, bottom/map->tile_height);
			bool botleft_floating = true;
			for (i = 0; tiles_botleft[i] > -1; i++) {
				if (collide(map, tiles_botleft[i])) {
					botleft_floating = false;
					break;
				}
			}

			if (botright_floating && botleft_floating) {
				jumping = true;
				fall_speed = 0;
			}

			al_free(tiles_botright);
			al_free(tiles_botleft);
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {
			// Clear the screen
			flip();
			redraw = false;
		}
	}

	// Clean up and return
	al_destroy_bitmap(egg);
	al_free_map(map);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	return 0;
}
