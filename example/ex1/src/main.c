/*
 * This program demonstrates the use of the allegro_tiled library.
 * Compiling and running can both be handled by make; simply type
 *
 *  $ make run
 *
 * and, assuming the library was built, it should compile and run.
 *
 * Enjoy.
 */

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_tiled.h>

#define FPS 60
#define DEBUG 0
#define MAP_FOLDER "data/maps"

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
 * Application entry point
 */
int main(int argc, char *argv[])
{
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_KEYBOARD_STATE keyboard_state;
	ALLEGRO_MAP *map;

	bool running = true;
	bool redraw = true;
	bool reload = false;

	int map_x = 0, map_y = 0;
	int screen_width = 640;
	int screen_height = 480;

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
	int map_total_width = al_get_map_width(map) * al_get_tile_width(map);
	int map_total_height = al_get_map_height(map) * al_get_tile_height(map);

	// Draw the map
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_map_region(map, map_x, map_y, screen_width, screen_height, 0, 0, 0);
	al_flip_display();

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
						map_x += 5;
						if (map_x > (map_total_width - screen_width))
							map_x = map_total_width - screen_width;
					}
					else if (al_key_down(&keyboard_state, ALLEGRO_KEY_LEFT)) {
						map_x -= 5;
						if (map_x < 0)
							map_x = 0;
					}
					else if (al_key_down(&keyboard_state, ALLEGRO_KEY_UP)) {
						map_y -= 5;
						if (map_y < 0)
							map_y = 0;
					}
					else if (al_key_down(&keyboard_state, ALLEGRO_KEY_DOWN)) {
						map_y += 5;
						if (map_y > (map_total_height - screen_height))
							map_y = map_total_height - screen_height;
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

		if (redraw && al_is_event_queue_empty(event_queue)) {
			// Clear the screen
			al_clear_to_color(al_map_rgb(0, 0, 0));

			// If we need to reload, do it
			// Otherwise, redraw
			if (reload) {
				int x = map_x;
				int y = map_y;
				al_free_map(map);
				map = al_open_map(MAP_FOLDER, "level1.tmx");
				map_x = x;
				map_y = y;
				reload = false;
			}
			else {
				al_draw_map_region(map, map_x, map_y, screen_width, screen_height, 0, 0, 0);
			}

			al_flip_display();
			redraw = false;
		}
	}

	// Clean up and return
	al_free_map(map);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	return 0;
}
