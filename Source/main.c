/*
SDLBlocks
 
Description:
An implementation of tetris using libsdl.
 
Donald E. Llopis 2005 (machinezilla@gmail.com)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
USA
*/

#include <stdlib.h>
#include <stdio.h>
#include "other.h"
#include "API.h"
#include "INPUT.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

#define TETRIS_WIDTH  20
#define TETRIS_HEIGHT 23

#define TETRAD_WIDTH 10
#define TETRAD_HEIGHT 10

#define MAX_TETRAD 7

#define TETRIS_MIN_X (TETRAD_WIDTH-1)
#define TETRIS_MAX_X (TETRIS_MIN_X+(TETRIS_WIDTH*TETRAD_HEIGHT)+1)
#define TETRIS_MIN_Y 0
#define TETRIS_MAX_Y (TETRIS_MIN_Y+(TETRAD_HEIGHT*TETRIS_HEIGHT))

#define START_X (TETRIS_MIN_X+1)+(TETRAD_WIDTH*4) +(TETRAD_WIDTH*6)
#define START_Y TETRIS_MIN_Y

#define TETRIS_OFF_X 0

/* tetrad patterns */

struct TetradMask {
	int w, h;
	unsigned long mask_arr[6];
};


/* ### */
/* #   */

struct TetradMask tetrad_0_mask[4] = {
	
	{ 3, 2, { 
			1, 1, 1, 
			1, 0, 0, 
			} },

	{ 2, 3, { 
			1, 1, 
			0, 1, 
			0, 1,
			} },
	
	{ 3, 2, { 
			0, 0, 1, 
			1, 1, 1, 
			} },
	
	{ 2, 3, { 
			1, 0, 
			1, 0, 
			1, 1,
			} }
};

/* ### */
/*   # */

struct TetradMask tetrad_1_mask[4] = {
	
	{ 3, 2, { 
			1, 1, 1, 
			0, 0, 1, 
			} },

	{ 2, 3, { 
			0, 1, 
			0, 1, 
			1, 1,
			} },
	
	{ 3, 2, { 
			1, 0, 0, 
			1, 1, 1, 
			} },
	
	{ 2, 3, { 
			1, 1,
			1, 0,
			1, 0,
			} }
};

/* ### */
/*  #  */

struct TetradMask tetrad_2_mask[4] = {
	
	{ 3, 2, { 
			1, 1, 1, 
			0, 1, 0, 
			} },

	{ 2, 3, { 
			0, 1,
			1, 1,
			0, 1,
			} },
	
	{ 3, 2, { 
			0, 1, 0, 
			1, 1, 1, 
			} },
	
	{ 2, 3, { 
			1, 0,
			1, 1,
			1, 0,
			} }
};

/*  ## */
/* ##  */

struct TetradMask tetrad_3_mask[2] = {
	
	{ 3, 2, { 
			0, 1, 1, 
			1, 1, 0, 
			} },

	{ 2, 3, { 
			1, 0,
			1, 1,
			0, 1,
			} }
};

/* ##  */
/*  ## */

struct TetradMask tetrad_4_mask[2] = {
	
	{ 3, 2, { 
			1, 1, 0, 
			0, 1, 1, 
			} },

	{ 2, 3, { 
			0, 1, 
			1, 1, 
			1, 0,
			} }
};

/* #### */

struct TetradMask tetrad_5_mask[2] = {
	
	{ 4, 1, { 
			1, 1, 1, 1, 0, 0
			} },
	{ 1, 4, { 
			1, 
			1,
			1,
			1,
			0, 0
			} }
};

/* ## */
/* ## */

struct TetradMask tetrad_6_mask[1] = {
	
	{ 2, 2, { 
			1, 1,
			1, 1,
			0, 0
			} }
};

/* tetrad type */

struct Tetrad {
	int num_patterns;
	unsigned long color;
	struct TetradMask * mask; 
};

struct Tetrad tetrad[MAX_TETRAD] = {
	{4, 0xff00ff, &tetrad_0_mask[0]},
	{4, 0xffffff, &tetrad_1_mask[0]},
	{4, 0xffff00, &tetrad_2_mask[0]},
	{2, 0x00ff00, &tetrad_3_mask[0]},
	{2, 0x00ffff, &tetrad_4_mask[0]},
	{2, 0xff0000, &tetrad_5_mask[0]},
	{1, 0x0000ff, &tetrad_6_mask[0]}
};

/* game super type - holds all of the game state variables */

struct Tetris {
	
	/* game state flags */
	int tetrad_drop;
	int tetrad_move;
	int tetrad_skip_move;
	int tetrad_new;
	int tetrad_wait;
	int tetrad_check_fill;
	unsigned long tetrad_drop_rate;

	int cur_tetrad;
	int cur_pattern;
	int prev_pattern;
	int game_over;
	int game_run;
	int game_pause;
	int game_start;
	int game_audio;

	unsigned long game_score;
	unsigned long game_level;
	unsigned long game_total_num_lines_cleared;
	unsigned long game_cur_num_lines_cleared;

	/* tetrad_max_patterns - max patterns of the currently active tetrad */
	int tetrad_max_patterns;

	/* tx, ty -- upper-left position of currently active tetrad relative to the game board */
	int tx;
	int ty;
	int prev_tx;
	int prev_ty;

	/* max_x,max_y -- maximum width and height of currently active tetrad relative to the game board */
	int max_x;
	int max_y;

	/* game clock-tick time variables */
	unsigned long now, next_time;
	
	/* abstract representation of the tetris game board */
	unsigned long board[TETRIS_HEIGHT][TETRIS_WIDTH];
	unsigned long score;

	/* pointer to currently active tetrad */
	struct Tetrad *t;
};

/* function prototypes */

void tetris_initialize( struct Tetris * t );
void tetris_draw_board( unsigned long *board );
void tetris_update( struct Tetris *t );
unsigned long tetris_score( unsigned long level, unsigned long lines );
void tetris_level_up( struct Tetris *t );
void tetrad_draw(int x, int y, struct Tetrad *t, int pattern );
void tetrad_put( unsigned long *board, struct Tetrad *t, int pattern, int tx, int ty );
int  tetrad_move( unsigned long *board, struct Tetrad *t, int pattern, int tx, int ty );
void Draw_game(void);


/*
 * main
 *
 *
 */
 
int main( int argc, char *argv[] )
{
	short button_state[18];
	short button_time[18];
	int i, j;
	int x, y;
	int pad;	
	unsigned long ticks;
	unsigned long ticks_ctrl;
	struct Tetris tetris;
	

	/*
	* Initialize the game variables
	*
	*/
	Init_video();

	tetris_initialize( &tetris );
	
	/* map tetrad RGB colors to actual colors */

	tetrad[0].color = 63519;
	tetrad[1].color = 65535;
	tetrad[2].color = 65504;
	tetrad[3].color = 2016;
	tetrad[4].color = 2047;
	tetrad[5].color = 63488;
	tetrad[6].color = 31;
	/*
	 * Main Loop
	 *
	 */
    ticks = 0;
    tetris.next_time = 0;
    
    /*
	Load_Music("music.aiff");
	Play_Music();
	*/
	Load_Image(0, "back.cel");
	Load_Image(1, "score.anim");
	
	for(i=0;i<9;i++)
	{
		Copy_Image(1,2+i);
	}
	
	for(i=0;i<18;i++)
	{
		button_state[i] = 0;
		button_time[i] = 0;
	}

	while ( tetris.game_run ) 
	{
		Controls();
		
		for (i=0;i<5;i++)
		{
			switch(i)
			{
				case 0:
				pad = BUTTON.A;
				break;
				case 1:
				pad = BUTTON.UP;
				break;
				case 2:
				pad = BUTTON.DOWN;
				break;
				case 3:
				pad = BUTTON.LEFT;
				break;
				case 4:
				pad = BUTTON.RIGHT;
				break;
			}
			
			switch (button_state[i])
			{
				case 0:
					if (pad)
					{
						button_state[i] = 1;
						button_time[i] = 0;
					}
				break;
					
				case 1:
					button_time[i]++;
						
					if (button_time[i] > 0)
					{
						button_state[i] = 2;
						button_time[i] = 0;
					}
				break;
					
				case 2:
					if (!(pad))
					{
						button_state[i] = 3;
						button_time[i] = 0;
					}
				break;
				
				case 3:
				
					button_time[i]++;
					if (button_time[i] > 1)
					{
						button_state[i] = 0;
						button_time[i] = 0;
					}
				break;
			}
			
		}
	
		if ( tetris.game_start == 0 ) 
		{	 
			if (BUTTON.LEFT && ticks_ctrl > 1)
			{
						if( tetrad_move( &(tetris.board[0][0]), tetris.t, tetris.cur_pattern, tetris.tx, tetris.ty ) ) 
						{
							tetris.prev_tx = tetris.tx;
												
							tetris.tx -= TETRAD_WIDTH;
												
							if( tetris.tx < (TETRIS_MIN_X+1) )
							{
								tetris.tx = (TETRIS_MIN_X+1);
							}

							/* make sure we can move into this position */
							if( !tetrad_move( &(tetris.board[0][0]), tetris.t, tetris.cur_pattern, tetris.tx, tetris.ty ) ) 
							{
							/* move the tetrad back */
							tetris.tx = tetris.prev_tx;
							}
						}
					tetris.tetrad_wait = 1;
					ticks_ctrl = 0;
			}
			else if (BUTTON.RIGHT && ticks_ctrl > 1)
			{
					if( tetrad_move( &(tetris.board[0][0]), tetris.t, tetris.cur_pattern, tetris.tx, tetris.ty ) ) 
					{
						tetris.prev_tx = tetris.tx;

						tetris.tx += TETRAD_WIDTH;

						if( tetris.tx > tetris.max_x )
							tetris.tx = tetris.max_x;

						/* make sure we can move into this position */
						if( !tetrad_move( &(tetris.board[0][0]), tetris.t, tetris.cur_pattern, tetris.tx, tetris.ty ) ) 
						{
							/* move the tetrad back */
							tetris.tx = tetris.prev_tx;
						}
					}
					tetris.tetrad_wait = 1;
					ticks_ctrl = 0;
			}

			if (button_state[1] == 1)
			{
					tetris.prev_pattern = tetris.cur_pattern;
					tetris.cur_pattern++;

					if( tetris.cur_pattern == tetris.tetrad_max_patterns ) 
						tetris.cur_pattern = 0;

					tetris.max_x = TETRIS_MAX_X - ( tetris.t->mask[tetris.cur_pattern].w * TETRAD_WIDTH );
					tetris.max_y = TETRIS_MAX_Y - ( tetris.t->mask[tetris.cur_pattern].h * TETRAD_HEIGHT );

					/* 
						* check the position of the new tetrad
						* make sure that it can be placed along
						* the right edge of the game board
					*/

					if( tetris.tx > tetris.max_x ) 
					{
							tetris.cur_pattern = tetris.prev_pattern;
							tetris.max_x = TETRIS_MAX_X - ( tetris.t->mask[tetris.cur_pattern].w * TETRAD_WIDTH );
							tetris.max_y = TETRIS_MAX_Y - ( tetris.t->mask[tetris.cur_pattern].h * TETRAD_HEIGHT );
					}

					if( tetris.ty > tetris.max_y ) 
					{
						tetris.cur_pattern = tetris.prev_pattern;
						tetris.max_x = TETRIS_MAX_X - ( tetris.t->mask[tetris.cur_pattern].w * TETRAD_WIDTH );
						tetris.max_y = TETRIS_MAX_Y - ( tetris.t->mask[tetris.cur_pattern].h * TETRAD_HEIGHT );
					}

					if( !tetrad_move( &(tetris.board[0][0]), tetris.t, tetris.cur_pattern, tetris.tx, tetris.ty ) ) 
					{
						/* move the tetrad back */
						tetris.cur_pattern = tetris.prev_pattern;
						tetris.max_x = TETRIS_MAX_X - ( tetris.t->mask[tetris.cur_pattern].w * TETRAD_WIDTH );
						tetris.max_y = TETRIS_MAX_Y - ( tetris.t->mask[tetris.cur_pattern].h * TETRAD_HEIGHT );
					}
			}
			else if (BUTTON.DOWN && ticks_ctrl > 2)
			{
					tetris.prev_ty = tetris.ty;
					tetris.ty += TETRAD_HEIGHT;
					if( tetris.ty > tetris.max_y )
						tetris.ty = tetris.prev_ty;
					/* make sure we can move into this position */
					if( !tetrad_move( &(tetris.board[0][0]), tetris.t, tetris.cur_pattern, tetris.tx, tetris.ty ) ) 
					{
						/* move the tetrad back */
						tetris.ty = tetris.prev_ty;
					}
					ticks_ctrl = 0;
			}
		}


		if (button_state[0] == 1)
		{
			if( tetris.game_over ) 
			{
				tetris_initialize( &tetris );
			}
			else if( tetris.game_start ) 
			{
				ticks = 0;
				tetris.game_start = 0;
				tetris.tetrad_skip_move = 1;
				tetris.tetrad_new = 1;
				/*if( tetris.game_audio ) 
				Mix_PlayMusic( music, -1 );*/
			}
			else 
			{
				tetris.tetrad_drop = 1;
				tetris.tetrad_move = 0;
			}
		}

		if (BUTTON.QUIT)
		{
			tetris.game_run = 0;
		}
		
		ticks = ticks + 16;
		ticks_ctrl++;
		
		/*
		 * Game Logic Section
		 *
		 */

		/* move the currently active tetrad down one row */

		if ( tetris.tetrad_drop || tetris.tetrad_move ) 
		{ 

			tetris.prev_ty = tetris.ty; 

			if ( tetris.tetrad_move ) {
				/*tetris.now = SDL_GetTicks();*/
				tetris.now = ticks;

				if ( tetris.tetrad_wait ) {
					tetris.now = 0;
					tetris.tetrad_wait = 0;
				}

				else if ( ( tetris.now - tetris.next_time ) >= tetris.tetrad_drop_rate ) {
					/*tetris.next_time = SDL_GetTicks();*/
					tetris.next_time = ticks;
					tetris.ty += TETRAD_HEIGHT;
				}
			}
			else 
				tetris.ty += TETRAD_HEIGHT; 

			if( tetris.ty > tetris.max_y ) {
				tetris.ty = tetris.prev_ty;
				tetris.tetrad_drop = 0;
				tetris.tetrad_move = 0;
				tetris.tetrad_new = 1;
				tetris.tetrad_check_fill = 1;
			}
			
			/* make sure we can move into this position */
			if( !tetrad_move( &(tetris.board[0][0]), tetris.t, tetris.cur_pattern, tetris.tx, tetris.ty ) ) {
				if( tetris.ty < 0 ) {
					tetris.game_over = 1;
					tetris.tetrad_new = 0;
					tetris.tetrad_drop = 0;
					tetris.tetrad_move = 0;
					tetris.tetrad_skip_move = 0;

					/*if( tetris.game_audio )
						Mix_HaltMusic();*/
				}
				else {
					/* move the tetrad back */
					tetris.ty = tetris.prev_ty;
					tetris.tetrad_drop = 0;
					tetris.tetrad_move = 0;
					tetris.tetrad_new = 1;
					tetris.tetrad_check_fill = 1;
				}
			}
		}

		/* don't move the tetrad for 1 game-loop */

		if ( tetris.tetrad_skip_move ) 
		{
			tetris.tetrad_skip_move = 0;
			tetris.tetrad_move = 1;
			tetris.tetrad_wait = 1;
		}

		/* spawn a new tetrad */

		if( tetris.tetrad_new ) 
		{
		
			/* place the current tetrad on the board */
			if (!tetris.tetrad_move) tetrad_put( &(tetris.board[0][0]), tetris.t, tetris.cur_pattern, tetris.tx, tetris.ty );

			/* generate a new tetrad */
			tetris.cur_tetrad = rand() % MAX_TETRAD;
			tetris.cur_tetrad = rand() % MAX_TETRAD;
			tetris.cur_tetrad = rand() % MAX_TETRAD; 
			tetris.cur_tetrad = rand() % MAX_TETRAD;
			tetris.cur_tetrad = rand() % MAX_TETRAD;

			tetris.cur_pattern = 0;

			tetris.t = &tetrad[tetris.cur_tetrad];

			tetris.tx = START_X;
			tetris.ty = 0;

			tetris.tetrad_max_patterns = tetris.t->num_patterns;
			tetris.max_x = TETRIS_MAX_X - ( tetris.t->mask[tetris.cur_pattern].w * TETRAD_WIDTH );
			tetris.max_y = TETRIS_MAX_Y - ( tetris.t->mask[tetris.cur_pattern].h * TETRAD_HEIGHT );
			
			/* check for game over */
			if( !tetrad_move( &(tetris.board[0][0]), tetris.t, tetris.cur_pattern, tetris.tx, tetris.ty ) ) 
			{
			#ifdef DEBUG_TETRIS
				fprintf( stderr, "game over...\n" );
			#endif
				tetris.game_over = 1;
				tetris.tetrad_skip_move = 0;

				/*if( tetris.game_audio )
					Mix_HaltMusic();*/
			}
			else 
			{
				tetris.tetrad_skip_move = 1;
			}
			
			tetris.tetrad_new = 0;
			tetris.tetrad_drop = 0;
			tetris.tetrad_move = 0;
		}

		/* check for filled rows and update the score and level */

		if ( tetris.tetrad_check_fill ) 
		{ 
#ifdef DEBUG_TETRIS
			fprintf( stderr, "checking board..\n" );
#endif
			tetris_update( &tetris );
			tetris_level_up( &tetris );
			tetris.tetrad_check_fill = 0;

#ifdef DEBUG_TETRIS
	fprintf( stderr, "\n" );
	fprintf( stderr, "cur level: %d\n", tetris.game_level );
	fprintf( stderr, "cur score: %d\n", tetris.game_score );
	fprintf( stderr, "total num lines cleared: %d\n", tetris.game_total_num_lines_cleared );
	fprintf( stderr, "cur num lines cleared: %d\n", tetris.game_cur_num_lines_cleared );
	fprintf( stderr, "\n" );
#endif

		}
		
		/*
		 * Rendering Section
		 *
		 */
		 
		/* draw the grid */
		Draw_game();
		
		/* draw the tetrominoes already on the matrix */
		tetris_draw_board(&(tetris.board[0][0]) );

		/* draw the currently active tetrominoe */
		tetrad_draw( tetris.tx, tetris.ty, tetris.t, tetris.cur_pattern );
		
		Put_sprite(1, 212, 32, 16, 16, (tetris.game_level / 10) % 10);
		Put_sprite(2, 212+16, 32, 16, 16, (tetris.game_level) % 10);
				
		Put_sprite(3, 212, 112, 16, 16,    (tetris.game_score / 10000) % 10);
		Put_sprite(4, 212+16, 112, 16, 16, (tetris.game_score / 1000) % 10);
		Put_sprite(5, 212+32, 112, 16, 16, (tetris.game_score / 100) % 10);
		Put_sprite(6, 212+48, 112, 16, 16, (tetris.game_score / 10) % 10);
		Put_sprite(7, 212+64, 112, 16, 16, (tetris.game_score) % 10);
		
		Put_sprite(8, 212, 192, 16, 16, (tetris.game_total_num_lines_cleared / 100) % 10);
		Put_sprite(9, 212+16, 192, 16, 16, (tetris.game_total_num_lines_cleared / 10) % 10);
		Put_sprite(10, 212+32, 192, 16, 16, (tetris.game_total_num_lines_cleared) % 10);
		
		/* draw game text */
		
		/*sprintf( &text[0], "SDLBlocks" );
		tetris_draw_text( font, screen, 260, 32, &text[0] );
		sprintf( &text[0], "level: %d", tetris.game_level );
		tetris_draw_text( font, screen, 260, 64, &text[0] );
		sprintf( &text[0], "lines: %d", tetris.game_total_num_lines_cleared );Draw_game();
		tetris_draw_text( font, screen, 260, 96, &text[0] );
		sprintf( &text[0], "score: %d", tetris.game_score );
		tetris_draw_text( font, screen, 260, 128, &text[0] );
		
		if( tetris.game_pause ) {
			sprintf( &text[0], "PAUSE" );
			tetris_draw_text( font, screen, 260, 192, &text[0] );
		}
		else if( tetris.game_start ) {
			sprintf( &text[0], "PRESS SPACE..." );
			tetris_draw_text( font, screen, 260, 192, &text[0] );
		}
		else if( tetris.game_over ) {
			sprintf( &text[0], "GAME OVER" );
			tetris_draw_text( font, screen, 260, 192, &text[0] );
		}*/


		Update_video();
	}

	Clearing();

	return 0;
}

void Draw_game(void)
{
	Put_image(0, 0, 0);
}


/*
 * tetris_initialize
 *
 * initialize tetris game variables
 *
 * input:
 *
 * struct Tetris * tetris - point to type struct Tetris
 *
 * output: none
 *
 */

void tetris_initialize( struct Tetris *tetris )
{
		unsigned long *board = &(tetris->board[0][0]);
		my_memset( board, 0, (TETRIS_HEIGHT*TETRIS_WIDTH)*sizeof(unsigned long) );
		
		tetris->tetrad_drop = 0;
		tetris->tetrad_new = 0;
		tetris->tetrad_wait = 0;
		tetris->tetrad_check_fill = 0;
		tetris->cur_tetrad = 0;
		/* set cur_pattern to -1 so that the tetrad isn't drawn while
		 * the game hasn't started. */
		tetris->cur_pattern = -1;
		tetris->prev_pattern = 0;
		tetris->tetrad_max_patterns = 0;
		tetris->max_x = 0;
		tetris->max_y = 0;
		tetris->prev_tx = 0;
		tetris->prev_ty = 0;
		tetris->score = 0;
		tetris->game_over = 0;
		tetris->game_pause = 0;
		tetris->game_score = 0;
		tetris->game_level = 0;
		tetris->game_total_num_lines_cleared = 0;
		tetris->game_cur_num_lines_cleared = 0;
		tetris->tetrad_move = 0;
		tetris->tetrad_skip_move = 0;
		tetris->game_run = 1;
		tetris->tx = START_X;
		tetris->ty = START_Y;

		tetris->game_start = 1;
		tetris->game_pause = 0;
		tetris->game_over = 0;

		/* set the first tetrad */
		tetris->tetrad_drop_rate = 500;

		tetris->t = &tetrad[tetris->cur_tetrad];
		tetris->tetrad_max_patterns = tetris->t->num_patterns;
		tetris->max_x = TETRIS_MAX_X - ( tetris->t->mask[tetris->cur_pattern].w * TETRAD_WIDTH );
		tetris->max_y = TETRIS_MAX_Y - ( tetris->t->mask[tetris->cur_pattern].h * TETRAD_HEIGHT );
}


/*
 * tetris_update
 *
 * check the tetris game board for filled rows and remove them if they are found
 * and accumulate points.
 *
 */
void tetris_update( struct Tetris *t )
{
	unsigned long board_tmp[TETRIS_HEIGHT][TETRIS_WIDTH];
	unsigned long *bptr;
	unsigned long num_lines_cleared;
	int i, j;
	int n;
	int m;

	my_memset( &board_tmp[0][0], 0, (TETRIS_HEIGHT*TETRIS_WIDTH)*sizeof(unsigned long) );

	num_lines_cleared = 0;

	/* check the board for filled rows from the bottom-up */
	i = TETRIS_HEIGHT - 1;

	for( i=TETRIS_HEIGHT-1; i>-1; i-- ) {
		bptr = (i * TETRIS_WIDTH) + &(t->board[0][0]);
		n = 0;
		for ( j=0; j<TETRIS_WIDTH; j++ ) {
				if ( *bptr++ )
						n++;
		}

#ifdef DEBUG_TETRIS
		fprintf( stderr, "n: %d\n", n );
#endif

		/* is there a fully filled row? */
		if( n == TETRIS_WIDTH ) 
		{ 

			/* yes, then shift the board down to the current level */

			num_lines_cleared++;

			bptr = &(t->board[0][0]);

			m = i * TETRIS_WIDTH;
			
			my_memcpy( &board_tmp[0][0], bptr, m*sizeof(unsigned long) );
			
			my_memcpy( bptr+TETRIS_WIDTH, &board_tmp[0][0], m*sizeof(unsigned long) );

			/* push the row counter back up to the previous line */
			i++;
		}
	}

	/* update game score */
	t->game_score += tetris_score( t->game_level, num_lines_cleared );
	t->game_total_num_lines_cleared += num_lines_cleared;
	t->game_cur_num_lines_cleared += num_lines_cleared;

	/* check for tilt, if so then reset the score */
	if( t->game_score > 9999999 ) 
	{
		t->game_score = 0;
	}
}

/*
 * tetris_score
 *
 * calculate game score based on NES scoring algorithm
 * see wikipedia.org entry for tetris for an explaination.
 *
 */
unsigned long tetris_score( unsigned long level, unsigned long lines )
{
	switch(lines)
	{
		case 0:
			return 0;
		break;		
		case 1:
			return ( level + 1 ) * 40;
		break;	
		case 2:
			return ( level + 1 ) * 100;
		break;	
		case 3:
			return ( level + 1 ) * 300;
		break;		
		default:
			return ( level + 1 ) * 1200;
		break;	
	}
}

/*
 * tetris_level_up
 *
 * increase the game level every 10 lines cleared
 * stop at level 20
 *
 * increase tetrad_drop_rate by 20ms per level
 *
 */
void tetris_level_up( struct Tetris *t )
{
	if( ( t->game_cur_num_lines_cleared > 9 ) && ( t->game_level < 20 ) ) {
		t->game_level++;
		t->tetrad_drop_rate -= 20;
		t->game_cur_num_lines_cleared -= 10;
	}
	else if( ( t->game_cur_num_lines_cleared >= 10 ) && ( t->game_level >= 20 ) ) {
		t->game_level++;
		t->game_cur_num_lines_cleared -= 10;
	}
}

/*
 * tetrad_draw
 *
 * draw a tetrad on the board
 *
 */
void tetrad_draw( int x, int y, struct Tetrad *t, int pattern )
{
	unsigned long *mask;
	
	int new_x, new_y, new_h, new_w;
	
	/*SDL_Rect rect;*/
	int i, j;
	int w, h;
	
	x = x + TETRIS_OFF_X;

	if ( pattern < 0 )
		return;
	
	new_x = x;
	new_y = y;
	new_h = TETRAD_HEIGHT - 1;
	new_w = TETRAD_WIDTH - 1;

	mask = t->mask[pattern].mask_arr;
	w = t->mask[pattern].w;
	h = t->mask[pattern].h;

	for( i=0; i<h; i++ ) 
	{

		new_y = y + (i*TETRAD_HEIGHT) + 1;

		if ( y > -1 ) 
		{
			for( j=0; j<w; j++ ) 
			{
				new_x = x + (j*TETRAD_WIDTH) + 1;
				if ( *mask++ )
				{
					Draw_Rect_noRGB(new_x, new_y, new_w, new_h, t->color);
				}
			}
		}
	}
}

/*
 * tetrad_move
 *
 */
int tetrad_move( unsigned long *board, struct Tetrad *t, int pattern, int tx, int ty )
{
	unsigned long *mask;
	unsigned long *bptr;
	int i, j;
	int bx;
	int by;
	int w, h;

	bx = (tx / TETRAD_WIDTH)-1;
	by = ty / TETRAD_HEIGHT;
	w = t->mask[pattern].w;
	h = t->mask[pattern].h;

	mask = t->mask[pattern].mask_arr;

	for( i=0; i<h; i++ ) {
		if ( by > -1 ) {
			bptr = board + ( by * TETRIS_WIDTH ) + bx;
			for( j=0; j<w; j++ ) {
				if( *mask && *bptr ) {
					return 0;
				}
				mask++;
				bptr++;
			}
		}
		by++;
	}
	
	return 1;
}

/*
 * tetrad_put
 *
 */
void tetrad_put( unsigned long *board, struct Tetrad *t, int pattern, int tx, int ty )
{
	unsigned long *mask;
	unsigned long *bptr = board;
	int i, j;
	int bx;
	int by;
	int w, h;

	if ( ty < 0 )
		return;

	bx = (tx / TETRAD_WIDTH)-1;
	by = ty / TETRAD_HEIGHT;
	w = t->mask[pattern].w;
	h = t->mask[pattern].h;

	mask = t->mask[pattern].mask_arr;

	for( i=0; i<h; i++ ) {
		if ( by > -1 ) {
			bptr = board + ( by * TETRIS_WIDTH ) + bx;
			for( j=0; j<w; j++ ) {
				if( *mask ) {
					*bptr = t->color;
				}
				mask++;
				bptr++;
			}
		}
		by++;
	}

#ifdef DEBUG_TETRIS

	bptr = board;

	fprintf( stderr, "\n\n");

	for( i=0; i<TETRIS_HEIGHT; i++ ) {

		for( j=0; j<TETRIS_WIDTH; j++ ) {

			if ( *bptr++ ) {
				fprintf( stderr, "1 " );
			}
			else
				fprintf( stderr, "0 " );
		}
		fprintf( stderr, "\n");
	}

#endif 
}

/*
 * tetris_draw_board
 *
 */
void tetris_draw_board( unsigned long *board ) 
{
	unsigned long *bptr = board;
	int i, j;
	
	int nw, nh, nx, ny;

	nw = TETRAD_HEIGHT - 1;
	nh = TETRAD_WIDTH - 1;
	
	for( i=0; i<TETRIS_HEIGHT; i++ ) {
		ny = (i * TETRAD_HEIGHT) + TETRIS_MIN_Y + 1;
		nx = 1+TETRIS_OFF_X;
		for( j=0; j<TETRIS_WIDTH; j++ ) {
			nx += TETRAD_WIDTH;
			if ( *bptr ) {
				Draw_Rect_noRGB(nx, ny, nw, nh, *bptr);
			}
			bptr++;
		}
	}
}

/* vim: set ci ai ts=4 sw=4: */
