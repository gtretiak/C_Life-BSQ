#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

typedef struct s_board
{
	int	width;
	int	height;
	char	**cells;
}	t_board;

typedef struct s_pen
{
	int	x;
	int	y;
	bool	drawing;
}	t_pen;

void	free_board(t_board *b, int num)
{
	for (int y = 0; y < num; y++)
		free(b->cells[y]);
	free(b->cells);
}

t_board	*create_board(int w, int h)
{
	t_board	*res = malloc(sizeof(t_board));
	if (!res)
		return (NULL);
	res->width = w;
	res->height = h;
	res->cells = malloc(sizeof(char*) * h);
	if (!res->cells)
	{
		free(res);
		return (NULL);
	}
	for (int y = 0; y < h; y++)
	{
		res->cells[y] = malloc(sizeof(char) * w + 1);
		if (!res->cells[y])
		{
			free(res);
			free_board(res, y);
			return (NULL);
		}
		for (int x = 0; x < w; x++)
			res->cells[y][x] = ' ';
		res->cells[y][w] = '\0';
	}
	return (res);
}

void	print_board(t_board *b)
{
	for (int y = 0; y < b->height; y++)
	{
		for (int x = 0; x < b->width; x++)
			putchar(b->cells[y][x]);
		putchar('\n');
	}
}

void	process_input(t_board *b)
{
	t_pen	pen = {0, 0, false};
	char	c;
	while (read(0, &c, 1) > 0)
	{
		switch (c)
		{
			case 's':
			{
				if (pen.y + 1 < b->height)
					pen.y++;
				break ;
			}
			case 'w':
			{
				if (pen.y - 1 >= 0)
					pen.y--;
				break ;
			}
			case 'a':
			{
				if (pen.x - 1 >= 0)
					pen.x--;
				break ;
			}
			case 'd':
			{
				if (pen.x + 1 < b->width)
					pen.x++;
				break ;
			}
			case 'x':
				pen.drawing = !pen.drawing;
				break ;
		}
		if (pen.drawing)
			b->cells[pen.y][pen.x] = 'O';
	}
}

int	count_neighbors(t_board *b, int x, int y)
{
	int	count = 0;
	int	neighbors_x[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
	int	neighbors_y[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
	for (int i = 0; i < 8; i++)
	{
		if (x + neighbors_x[i] >= b->width || x + neighbors_x[i] < 0 || y + neighbors_y[i] >= b->height || y + neighbors_y[i] < 0)
			continue ;
		if (b->cells[y + neighbors_y[i]][x + neighbors_x[i]] == 'O')
			count++;
	}
	return (count);
}

void	perform_action(t_board *curr, t_board *next)
{
	int	neighbors;
	for (int y = 0; y < curr->height; y++)
	{
		for (int x = 0; x < curr->width; x++)
		{
			neighbors = count_neighbors(curr, x, y);
			if (curr->cells[y][x] == 'O')
			{
				if (neighbors == 2 || neighbors == 3)
					next->cells[y][x] = 'O';
				else
					next->cells[y][x] = ' ';
			}
			else
			{
				if (neighbors == 3)
					next->cells[y][x] = 'O';
				else
					next->cells[y][x] = ' ';
			}
		}
	}
}

int	main(int argc, char **argv)
{
	if (argc != 4)
		return (1);
	int w = atoi(argv[1]);
	int h = atoi(argv[2]);
	int iter = atoi(argv[3]);
	t_board	*curr = create_board(w, h);
	if (!curr)
		return (1);
	t_board	*next = create_board(w, h);
	if (!next)
	{
		free_board(curr, h);
		return (1);
	}
	process_input(curr);
	for (int i = 0; i < iter; i++)
	{
		perform_action(curr, next);
		t_board	*temp = curr;
		curr = next;
		next = temp;
	}
	print_board(curr);
	free_board(curr, h);
	free_board(next, h);
	free(curr);
	free(next);
}
