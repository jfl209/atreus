#include "atreus_trackpoint.h"

/*
 * Trackpoint init tweaks.
 *
 * IBM TrackPoint modules accept a small set of magic PS/2 commands that
 * configure sensitivity, deadband, etc. The defaults are usually OK; tune
 * by uncommenting and adjusting if the cursor feels wrong after you have
 * the basic wiring working.
 */
void ps2_mouse_init_user(void) {
    /* Example knobs — see PS/2 mouse driver docs for the full set.
     *
     *   ps2_mouse_set_resolution(PS2_MOUSE_8_COUNT_PER_MM);
     *   ps2_mouse_set_sample_rate(PS2_MOUSE_100_SAMPLES_PER_SEC);
     */
}
