#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define DELAY 5
#define INIT_RUNNING 1
#define STOP_RUNNING 0
#define COLOR_BLACK 0x00000000
#define COLOR_WHITE 0xffffffff
#define COLOR_YELLOW 0xffffff00
#define RAYS_NUMBER 120
#define RAY_THICKNESS 2
#define MAX_VERTICES 120
#define M_PI 3.14159265358979323846

#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

typedef struct
{
	SDL_Surface* surface;
	SDL_Window* window;
} App;

struct Circle
{
	double x;
	double y;
	double radius;
};

struct Ray
{
	double x_start, y_start;
	double angle;
};

struct Point
{
	double x;
	double y;
};

struct Polygon
{
	struct Point vertices[MAX_VERTICES];
	int vertex_count;
};

void InitSDL(App* app, int width, int height)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Init error SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	app->window = SDL_CreateWindow("Museum guards", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_SHOWN);
	if (app->window == NULL)
	{
		printf("Init error SDL: %s\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	app->surface = SDL_GetWindowSurface(app->window);
	if (app->surface == NULL)
	{
		printf("Init error surface: %s\n", SDL_GetError());
		SDL_DestroyWindow(app->window);
		SDL_Quit();
		exit(EXIT_FAILURE);
	}
}

void ClearSDL(App* app)
{
	SDL_FreeSurface(app->surface);
	SDL_DestroyWindow(app->window);
	SDL_Quit();
}

void FillCircle(SDL_Surface* surface, const struct Circle circle, Uint32 color)
{
	double radius_squared = pow(circle.radius, 2);
	for (double x = circle.x - circle.radius; x <= circle.x + circle.radius; x++)
	{
		for (double y = circle.y - circle.radius; y <= circle.y + circle.radius; y++)
		{
			double distance_squared = pow(x - circle.x, 2) + pow(y - circle.y, 2);
			if (distance_squared < radius_squared)
			{
				SDL_Rect pixel = (SDL_Rect){ x, y, 1, 1 };
				SDL_FillRect(surface, &pixel, color);
			}
		}
	}
}

void FillPolygon(SDL_Surface* surface, const struct Polygon* polygon, Uint32 color)
{
	for (int i = 0; i < polygon->vertex_count; i++)
	{
		int next_i = (i + 1) % polygon->vertex_count; // Следующая вершина (замыкаем полигон)
		double x1 = polygon->vertices[i].x;
		double y1 = polygon->vertices[i].y;
		double x2 = polygon->vertices[next_i].x;
		double y2 = polygon->vertices[next_i].y;

		// Рисуем линию между вершинами
		double dx = x2 - x1;
		double dy = y2 - y1;
		double steps = fmax(fabs(dx), fabs(dy));
		double x_inc = dx / steps;
		double y_inc = dy / steps;

		double x = x1;
		double y = y1;
		for (int j = 0; j <= steps; j++)
		{
			SDL_Rect pixel = (SDL_Rect){ x, y, 1, 1 };
			SDL_FillRect(surface, &pixel, color);
			x += x_inc;
			y += y_inc;
		}
	}
}

void GenerateRays(struct Circle circle, struct Ray rays[RAYS_NUMBER])
{
	for (int i = 0; i < RAYS_NUMBER; i++)
	{
		double angle = ((double)i / RAYS_NUMBER) * 2 * M_PI;
		struct Ray ray = { circle.x, circle.y, angle };
		rays[i] = ray;
	}
}

int IsPointInsidePolygon(double x, double y, const struct Polygon* polygon)
{
	int inside = 0;
	for (int i = 0, j = polygon->vertex_count - 1; i < polygon->vertex_count; j = i++)
	{
		double xi = polygon->vertices[i].x;
		double yi = polygon->vertices[i].y;
		double xj = polygon->vertices[j].x;
		double yj = polygon->vertices[j].y;

		// Проверяем пересечение луча с ребром полигона
		if ((yi > y) != (yj > y))
		{
			double intersect_x = (xj - xi) * (y - yi) / (yj - yi) + xi;
			if (x < intersect_x)
			{
				inside = !inside;
			}
		}
	}
	return inside;
}

void FillRays(
	SDL_Surface* surface, struct Ray rays[RAYS_NUMBER], Uint32 color, const struct Polygon* polygon)
{
	for (int i = 0; i < RAYS_NUMBER; i++)
	{
		struct Ray ray = rays[i];

		int end_of_screen = 0;
		int inside_polygon = IsPointInsidePolygon(ray.x_start, ray.y_start, polygon);

		double step = 1;
		double x_draw = ray.x_start;
		double y_draw = ray.y_start;

		while (!end_of_screen)
		{
			x_draw += step * cos(ray.angle);
			y_draw += step * sin(ray.angle);

			// Проверка на выход за границы экрана
			if (x_draw < 0 || x_draw > WINDOW_WIDTH || y_draw < 0 || y_draw > WINDOW_HEIGHT)
			{
				end_of_screen = 1;
				break;
			}

			// Проверяем, находится ли текущая точка внутри полигона
			int currently_inside = IsPointInsidePolygon(x_draw, y_draw, polygon);

			// Если луч был снаружи и вошел в полигон, останавливаем его
			if (!inside_polygon && currently_inside)
			{
				end_of_screen = 1;
				break;
			}

			// Рисуем луч только вне полигона
			if (currently_inside)
			{
				SDL_Rect ray_point = (SDL_Rect){ x_draw, y_draw, RAY_THICKNESS, RAY_THICKNESS };
				SDL_FillRect(surface, &ray_point, color);
			}

			// Обновляем состояние inside_polygon
			inside_polygon = currently_inside;
		}
	}
}

int main()
{
	App app;
	memset(&app, 0, sizeof(App));
	InitSDL(&app, WINDOW_WIDTH, WINDOW_HEIGHT);

	SDL_Rect erase_rect = (SDL_Rect){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	struct Circle circle = { 200, 200, 10 };

	struct Polygon polygon = { .vertices
		= { { 578, 316 }, { 607, 419 }, { 585, 427 }, { 601, 454 }, { 566, 452 }, { 428, 366 },
			{ 388, 470 }, { 311, 490 }, { 292, 292 }, { 274, 387 }, { 200, 453 }, { 167, 397 },
			{ 176, 384 }, { 175, 275 }, { 119, 259 }, { 223, 256 }, { 206, 189 }, { 202, 53 },
			{ 216, 49 }, { 366, 381 }, { 466, 132 }, { 517, 120 }, { 480, 187 }, { 499, 198 } },
		.vertex_count = 24 };

	struct Ray rays[RAYS_NUMBER];
	GenerateRays(circle, rays);

	SDL_Event event;
	int running = INIT_RUNNING;

	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = STOP_RUNNING;
			}

			if (event.type == SDL_MOUSEMOTION && event.motion.state != 0)
			{
				circle.x = event.motion.x;
				circle.y = event.motion.y;
				GenerateRays(circle, rays);
			}
		}
		SDL_FillRect(app.surface, &erase_rect, COLOR_BLACK);

		FillPolygon(app.surface, &polygon, COLOR_WHITE);
		FillRays(app.surface, rays, COLOR_YELLOW, &polygon);
		FillCircle(app.surface, circle, COLOR_YELLOW);

		SDL_UpdateWindowSurface(app.window);
		SDL_Delay(DELAY);
	}

	ClearSDL(&app);
	return EXIT_SUCCESS;
}