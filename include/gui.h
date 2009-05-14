#ifndef gui_h
#define gui_h

/**
 * Initializes the GUI elements
 */
void init_gui();

/**
 * Main loop for GUI. It receives the SDL event and does stuff
 */
void gui_loop();

/**
 * Sets the current emulation image as the background of the GUI
 */
void gui_set_background();

#endif /* gui_h */
