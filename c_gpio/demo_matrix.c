#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include "../BBBIOlib/BBBio_lib/BBBiolib.h"
#include "../LVA/pins.h"
#include "../LVA/utils.h"
#include "matrix.h"

static int running = 1;
static int PINS_LEVELS[7][5] = {
	{3, 3, 3, 3, 3},
	{3, 1, 2, 3, 1},
	{3, 1, 2, 3, 1},
	{3, 2, 1, 0, 2},
	{3, 1, 2, 3, 1},
	{3, 1, 2, 3, 1},
	{3, 1, 2, 3, 1},
};

static const char* pins_rows_names[] = {"P8_36", "P8_37", "P8_38", "P8_39", "P8_40", "P8_41", "P8_42"};
static const char* pins_cols_names[] = {"P8_11", "P8_12", "P8_15", "P8_16", "P8_26"};

static const pin_t *pins_rows; 
static const pin_t *pins_cols; 

void set_pins_values(void)
{
	static int first_run = 1;
	static int counter_row = 0;
	static int counter_col = 0;

	if (first_run) {
		srand(time(NULL));
		first_run = 0;
	}

	for (int i = 0; i < N_ROWS; ++i) {
		for (int j = 0; j < N_COLS; ++j) {
			/*PINS_LEVELS[i][j] = rand() % 4;*/
			PINS_LEVELS[i][j] = 3;
		}
	}

	PINS_LEVELS[counter_row][counter_col] = 0;
	counter_col++;

	if (counter_col%N_COLS == 0) {
		counter_row++;
		counter_col = 0;
	}
	if (counter_row%N_ROWS == 0) {
		counter_row = 0;
	}

}

void handler(int sig)
{
	/* yes i know, we only catch SIGINT ATM.. */
	if (sig == SIGINT) {
		running = 0;
		printf("Exiting...\n");
	}
}

void setup(int *ctrl)
{
	int p;
	int row_ctrl;

	/* init gpio */
	iolib_init();
	BBBIO_sys_Enable_GPIO(BBBIO_GPIO1);

	/* rows gpio */
	BBBIO_sys_Enable_GPIO(BBBIO_GPIO2);

	/* get pins ctrl */
	for (p = 0; p < N_COLS; ++p) {
		*ctrl |= (pins_cols[p]).id;
	}

	for (p = 0; p < N_ROWS; ++p) {
		row_ctrl |= (pins_rows[p]).id;
	}

	/* set pins as output */
	BBBIO_GPIO_set_dir(BBBIO_GPIO1, 0, *ctrl);
	BBBIO_GPIO_set_dir(BBBIO_GPIO2, 0, row_ctrl);

	/* set all pins to low */
	set_pins_row_off_by_gpio(BBBIO_GPIO1, *ctrl);
	unselect_row_by_id_and_gpio(BBBIO_GPIO2, row_ctrl);
}

void cleanup(void)
{
	iolib_free();
}

int get_pins_to_set_on(int row_id, int level_id)
{
	int out, p;
	out = 0;

	/* loop over pins */
	for (p = 0; p < N_COLS; ++p) {
		if (PINS_LEVELS[row_id][p] <= level_id) {
			out |= (pins_cols[p]).id;
		}
	}

	return out;
}

void switch_leds(int ctrl)
{
	int l, out, row;
	
	while (running) {
		
		for (row = 0; row < N_ROWS; ++row) {

			select_row_by_id_and_gpio(BBBIO_GPIO2, pins_rows[row].id);

			/* loop over levels */
			/* N_LEVELS-2, indeed no need to check level 3 (off)*/
			for (l = N_LEVELS - 2; l >= 0 ; --l) {
	
				/* todo: fct to set on/off independent of gpios */
				out = get_pins_to_set_on(row, l);
	
				/* set pins values */
				set_pins_row_on_by_gpio(BBBIO_GPIO1, out);
				set_pins_row_off_by_gpio(BBBIO_GPIO1, ctrl & (~out));
	
				/*
					wait X us 
				 	we already waited for T/LEVELS[l-1], so let's subtract it
				*/
	
				level_sleep(l);
	
			}
			set_pins_row_off_by_gpio(BBBIO_GPIO1, ctrl);

			unselect_row_by_id_and_gpio(BBBIO_GPIO2, pins_rows[row].id);
		}

		//set_pins_values();
	}

	/* end of loop, exit */
	cleanup();
}

int main()
{
	int ctrl;

	for (int i = 0; i < N_ROWS; ++i) {
		for (int j = 0; j < N_COLS; ++j) {
			PINS_LEVELS[i][j]= 3;
		}
	}
	PINS_LEVELS[5][2] = 0;

	pins_rows = get_pins_by_names(pins_rows_names, ARRAY_SIZE(pins_rows_names));
	pins_cols = get_pins_by_names(pins_cols_names, ARRAY_SIZE(pins_cols_names));

	setup(&ctrl);
	switch_leds(ctrl);

	signal(SIGINT, handler);

	return 0;
}
