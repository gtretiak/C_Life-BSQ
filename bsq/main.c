#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <limits.h>

typedef struct s_map
{
	int	width;
	int	height;
	char	empty;
	char	full;
	char	obstacle;
	char	**cells;
}	t_map;

bool	free_and_go(t_map *map, char *line)
{
	free(map);
	free(line);
	return (false);
}

bool	freeing_map(t_map *map, bool flag, int num)
{
	if (num > 2)
	{
		for (int i = 0; i < num - 2; i++)
			free(map->cells[i]);
	}
	if (num > 1)
		free(map->cells);
	free(map);
	return (flag);
}

bool	is_first_line_valid(ssize_t read, char *line, t_map *map)
{
	ssize_t	i = 0;
	map->height = 0;
	if (!(line[0] >= '1' && line[0] <= '9') || line[read - 1] != '\n')
		return (false);
	while (i < read && line[i] >= '0' && line[i] <= '9')
	{
		map->height *= 10;
		map->height += line[i] - '0';
		i++;
	}
	if (read < i + 4)
		return (false);
	map->empty = line[i];
	if (map->empty < 32 || map->empty > 126)
		return (false);
	map->obstacle = line[++i];
	if (map->empty == map->obstacle || (map->obstacle < 32 || map->obstacle > 126))
		return (false);
	map->full = line[++i];
	if (line[i] == '\n' || map->full == map->empty || map->full == map->obstacle || (map->full < 32 || map->full > 126))
		return (false);
	return (true);
}

bool	check_chars(t_map *map, int row)
{
	char	c;
	for (int i = 0; map->cells[row][i] != '\n'; i++)
	{
		c = map->cells[row][i];
		if (c != map->empty && c != map->obstacle)
			return (false);
	}	
	return (true);
}

bool	is_map_valid(t_map *map, FILE *f)
{
	size_t	len = 0;
	ssize_t	read = 0;
	map->cells = malloc(sizeof(char *) * map->height);
	if (!map->cells)
		return (freeing_map(map, false, 1));
	map->cells[0] = NULL;
	read = getline(&map->cells[0], &len, f);
	if (read == -1)
		return (freeing_map(map, false, 2));
	if (map->cells[0][read - 1] != '\n' || !check_chars(map, 0))
		return (freeing_map (map, false, 3));
	map->width = read - 1;
	for (int i = 1; i < map->height; i++)
	{
		size_t	len = 0;
		map->cells[i] = NULL;
		read = getline(&map->cells[i], &len, f);
		if (read == -1)
			return (freeing_map(map, false, 2 + i));
		if (read - 1 != map->width || map->cells[i][read - 1] != '\n' || !check_chars(map, i))
			return (freeing_map (map, false, 3 + i));
	}
	return (true);
}

void	fill(t_map *map, int maxS, int maxX, int maxY)
{
	for (int y = maxY; y >= maxY - maxS + 1; y--)
	{
		for (int x = maxX; x >= maxX - maxS + 1; x--)
			map->cells[y][x] = map->full;
	}
}

void	free_dp(int **dp, int n)
{
	for (int i = 0; i < n; i++)
		free(dp[i]);
	free(dp);
}

bool	find_and_fill(t_map *map)
{
	int	**dp = malloc(sizeof(int *) * map->height);
	if (!dp)
		return (freeing_map(map, false, 2 + map->height));
	int	maxS = 0;
	int	maxX = 0;
	int 	maxY = 0;
	for (int y = 0; y < map->height; y++)
	{
		dp[y] = calloc(sizeof(int), map->width);
		if (!dp[y])
		{
			free_dp(dp, y);
			return (freeing_map(map, false, 2 + map->height));
		}
		for (int x = 0; x < map->width; x++)
		{
			if (map->cells[y][x] == map->obstacle)
				dp[y][x] = 0;
			else if (y == 0 || x == 0)
				dp[y][x] = 1;
			else
			{
				int min = dp[y - 1][x];
				if (dp[y][x - 1] < min)
					min = dp[y][x - 1];
				if (dp[y - 1][x - 1] < min)
					min = dp[y - 1][x - 1];
				dp[y][x] = 1 + min;
			}
			if (dp[y][x] > maxS)
			{
				maxS = dp[y][x];
				maxX = x;
				maxY = y;
			}
		}
	}
	fill(map, maxS, maxX, maxY);
	free_dp(dp, map->height);
	return (true);
}

void	print(t_map *map)
{
	for (int y = 0; y < map->height; y++)
	{
		for (int x = 0; x < map->width; x++)
			fprintf(stdout, "%c", map->cells[y][x]);
		fputs("\n", stdout);
	}
}

bool	process_file(FILE *f)
{
	ssize_t	read = 0;
	size_t	len = 0;
	char	*first_line = NULL;
	t_map	*map = malloc(sizeof(t_map));
	if (!map)
		return (NULL);
	read = getline(&first_line, &len, f);
	if (read == -1)
	{
		fprintf(stderr, "%s:%d", "errno", errno);
		return (NULL);
	}
	if (read < 5 || !is_first_line_valid(read, first_line, map))
		return (free_and_go(map, first_line));
	free(first_line);
	if (!is_map_valid(map, f))
		return (NULL);
	if (!find_and_fill(map))
		return (NULL);
	print(map);
	return (freeing_map(map, true, 2 + map->height));
}

int	main(int argc, char **argv)
{
	FILE	*infile = stdin;
	if (argc < 2)
		process_file(infile);
	else
	{
		for (int i = 1; i < argc; i++)
		{
			infile = fopen(argv[i], "r");
			if (!infile)
			{
				fprintf(stderr, "%s:%d", "errno", errno);
				return (1);
			}
			if (!process_file(infile))
				fputs("map error", stderr);
			if (fclose(infile))
			{
				fprintf(stderr, "%s:%d", "errno", errno);
				return (1);
			}
			if (i + 1 < argc)
				fputs("\n", stdout);
		}
	}
}
