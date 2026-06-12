#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define WIDTH 60
#define HEIGHT 20
#define MAX_OBJECTS 100

typedef enum {
    SHAPE_LINE = 1,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

typedef struct {
    int x1, y1, x2, y2;
} LineData;

typedef struct {
    int x, y, width, height;
} RectData;

typedef struct {
    int xc, yc, r;
} CircleData;

typedef struct {
    int x1, y1, x2, y2, x3, y3;
} TriangleData;

typedef struct {
    int id;
    ShapeType type;
    int is_active;
    union {
        LineData line;
        RectData rect;
        CircleData circle;
        TriangleData triangle;
    } data;
} GraphicObject;

// Global state
GraphicObject objects[MAX_OBJECTS];
int object_count = 0;
int next_id = 1;

// Drawing function declarations
void draw_line(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2);
void draw_rectangle(char canvas[HEIGHT][WIDTH], int x, int y, int w, int h);
void draw_circle(char canvas[HEIGHT][WIDTH], int xc, int yc, int r);
void draw_triangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int x3, int y3);

// Helper for robust integer input
int read_int(const char* prompt, int min_val, int max_val) {
    char buffer[100];
    int value;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error reading input. Exiting.\n");
            exit(1);
        }
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        if (sscanf(buffer, "%d", &value) == 1) {
            if (value >= min_val && value <= max_val) {
                return value;
            } else {
                printf("Error: Input must be between %d and %d.\n", min_val, max_val);
            }
        } else {
            printf("Error: Invalid integer input.\n");
        }
    }
}

// Initialize canvas with background character '_'
void clear_canvas(char canvas[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Render all active objects onto canvas
void render_scene(char canvas[HEIGHT][WIDTH]) {
    clear_canvas(canvas);
    for (int i = 0; i < object_count; i++) {
        if (objects[i].is_active) {
            switch (objects[i].type) {
                case SHAPE_LINE:
                    draw_line(canvas, objects[i].data.line.x1, objects[i].data.line.y1,
                              objects[i].data.line.x2, objects[i].data.line.y2);
                    break;
                case SHAPE_RECTANGLE:
                    draw_rectangle(canvas, objects[i].data.rect.x, objects[i].data.rect.y,
                                   objects[i].data.rect.width, objects[i].data.rect.height);
                    break;
                case SHAPE_CIRCLE:
                    draw_circle(canvas, objects[i].data.circle.xc, objects[i].data.circle.yc,
                                objects[i].data.circle.r);
                    break;
                case SHAPE_TRIANGLE:
                    draw_triangle(canvas, objects[i].data.triangle.x1, objects[i].data.triangle.y1,
                                  objects[i].data.triangle.x2, objects[i].data.triangle.y2,
                                  objects[i].data.triangle.x3, objects[i].data.triangle.y3);
                    break;
            }
        }
    }
}

// Display the rendered canvas with axes coordinate helpers
void display_picture(char canvas[HEIGHT][WIDTH]) {
    printf("\n");
    // Column header: tens digit
    printf("     ");
    for (int x = 0; x < WIDTH; x++) {
        if (x >= 10) {
            printf("%d ", (x / 10) % 10);
        } else {
            printf("  ");
        }
    }
    printf("\n");
    // Column header: units digit
    printf("     ");
    for (int x = 0; x < WIDTH; x++) {
        printf("%d ", x % 10);
    }
    printf("\n");
    // Top border line
    printf("   +-");
    for (int x = 0; x < WIDTH; x++) {
        printf("--");
    }
    printf("+\n");
    // Canvas contents
    for (int y = 0; y < HEIGHT; y++) {
        printf("%2d | ", y);
        for (int x = 0; x < WIDTH; x++) {
            printf("%c ", canvas[y][x]);
        }
        printf("|\n");
    }
    // Bottom border line
    printf("   +-");
    for (int x = 0; x < WIDTH; x++) {
        printf("--");
    }
    printf("+\n");
}

// List all active objects
void list_objects() {
    int active_found = 0;
    printf("\n=================== ACTIVE OBJECTS ===================\n");
    for (int i = 0; i < object_count; i++) {
        if (objects[i].is_active) {
            active_found = 1;
            printf("ID %d: ", objects[i].id);
            switch (objects[i].type) {
                case SHAPE_LINE:
                    printf("[Line]      from (%d, %d) to (%d, %d)\n",
                           objects[i].data.line.x1, objects[i].data.line.y1,
                           objects[i].data.line.x2, objects[i].data.line.y2);
                    break;
                case SHAPE_RECTANGLE:
                    printf("[Rectangle] top-left (%d, %d), width %d, height %d\n",
                           objects[i].data.rect.x, objects[i].data.rect.y,
                           objects[i].data.rect.width, objects[i].data.rect.height);
                    break;
                case SHAPE_CIRCLE:
                    printf("[Circle]    center (%d, %d), radius %d\n",
                           objects[i].data.circle.xc, objects[i].data.circle.yc,
                           objects[i].data.circle.r);
                    break;
                case SHAPE_TRIANGLE:
                    printf("[Triangle]  vertices (%d, %d), (%d, %d), (%d, %d)\n",
                           objects[i].data.triangle.x1, objects[i].data.triangle.y1,
                           objects[i].data.triangle.x2, objects[i].data.triangle.y2,
                           objects[i].data.triangle.x3, objects[i].data.triangle.y3);
                    break;
            }
        }
    }
    if (!active_found) {
        printf("(None - Add some shapes to get started!)\n");
    }
    printf("======================================================\n");
}

// Find object index by ID
int find_object_index(int id) {
    for (int i = 0; i < object_count; i++) {
        if (objects[i].id == id && objects[i].is_active) {
            return i;
        }
    }
    return -1;
}

// Prompt and fill shape data
void prompt_shape_data(ShapeType type, GraphicObject *obj) {
    obj->type = type;
    switch (type) {
        case SHAPE_LINE:
            printf("\nEnter coordinates for the Line:\n");
            obj->data.line.x1 = read_int("Start X (0 to 59): ", 0, WIDTH - 1);
            obj->data.line.y1 = read_int("Start Y (0 to 19): ", 0, HEIGHT - 1);
            obj->data.line.x2 = read_int("End X (0 to 59):   ", 0, WIDTH - 1);
            obj->data.line.y2 = read_int("End Y (0 to 19):   ", 0, HEIGHT - 1);
            break;
        case SHAPE_RECTANGLE:
            printf("\nEnter properties for the Rectangle:\n");
            obj->data.rect.x = read_int("Top-left X (0 to 59): ", 0, WIDTH - 1);
            obj->data.rect.y = read_int("Top-left Y (0 to 19): ", 0, HEIGHT - 1);
            obj->data.rect.width = read_int("Width (1 to 60):      ", 1, WIDTH);
            obj->data.rect.height = read_int("Height (1 to 20):     ", 1, HEIGHT);
            break;
        case SHAPE_CIRCLE:
            printf("\nEnter properties for the Circle:\n");
            obj->data.circle.xc = read_int("Center X (0 to 59):   ", 0, WIDTH - 1);
            obj->data.circle.yc = read_int("Center Y (0 to 19):   ", 0, HEIGHT - 1);
            obj->data.circle.r = read_int("Radius (0 to 60):     ", 0, WIDTH);
            break;
        case SHAPE_TRIANGLE:
            printf("\nEnter vertices for the Triangle:\n");
            obj->data.triangle.x1 = read_int("Vertex 1 X (0 to 59): ", 0, WIDTH - 1);
            obj->data.triangle.y1 = read_int("Vertex 1 Y (0 to 19): ", 0, HEIGHT - 1);
            obj->data.triangle.x2 = read_int("Vertex 2 X (0 to 59): ", 0, WIDTH - 1);
            obj->data.triangle.y2 = read_int("Vertex 2 Y (0 to 19): ", 0, HEIGHT - 1);
            obj->data.triangle.x3 = read_int("Vertex 3 X (0 to 59): ", 0, WIDTH - 1);
            obj->data.triangle.y3 = read_int("Vertex 3 Y (0 to 19): ", 0, HEIGHT - 1);
            break;
    }
}

// Add a new object
void add_object() {
    if (object_count >= MAX_OBJECTS) {
        printf("\nError: Maximum object limit reached (%d objects max).\n", MAX_OBJECTS);
        return;
    }
    printf("\nSelect shape to add:\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    int choice = read_int("Choice (1-4): ", 1, 4);
    GraphicObject new_obj;
    new_obj.id = next_id++;
    new_obj.is_active = 1;
    prompt_shape_data((ShapeType)choice, &new_obj);
    objects[object_count++] = new_obj;
    printf("\nSuccessfully added object with ID %d!\n", new_obj.id);
}

// Delete an object
void delete_object() {
    list_objects();
    printf("\nEnter the ID of the object to delete:\n");
    int id = read_int("ID: ", 1, 99999);
    int index = find_object_index(id);
    if (index == -1) {
        printf("\nError: Object with ID %d not found or already deleted.\n", id);
    } else {
        objects[index].is_active = 0;
        printf("\nSuccessfully deleted object with ID %d!\n", id);
    }
}

// Modify an existing object
void modify_object() {
    list_objects();
    printf("\nEnter the ID of the object to modify:\n");
    int id = read_int("ID: ", 1, 99999);
    int index = find_object_index(id);
    if (index == -1) {
        printf("\nError: Object with ID %d not found.\n", id);
    } else {
        printf("\nModifying object ID %d...\n", id);
        prompt_shape_data(objects[index].type, &objects[index]);
        printf("\nSuccessfully modified object with ID %d!\n", id);
    }
}

int main() {
    char canvas[HEIGHT][WIDTH];
    printf("==========================================\n");
    printf("     ASCII 2D Graphics Editor (C)         \n");
    printf("==========================================\n");
    while (1) {
        printf("\nMain Menu:\n");
        printf("1. Display Canvas / Picture\n");
        printf("2. Add Object\n");
        printf("3. Delete Object\n");
        printf("4. Modify Object\n");
        printf("5. List Objects\n");
        printf("6. Exit\n");
        int choice = read_int("Choice (1-6): ", 1, 6);
        switch (choice) {
            case 1:
                render_scene(canvas);
                display_picture(canvas);
                break;
            case 2:
                add_object();
                break;
            case 3:
                delete_object();
                break;
            case 4:
                modify_object();
                break;
            case 5:
                list_objects();
                break;
            case 6:
                printf("\nExiting editor. Goodbye!\n");
                return 0;
        }
    }
}

// --- SHAPE DRAWING ALGORITHMS IMPLEMENTATION ---

// Bresenham's Line Algorithm
void draw_line(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;
    while (1) {
        if (x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT) {
            canvas[y1][x1] = '*';
        }
        if (x1 == x2 && y1 == y2) {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Outline Rectangle drawing
void draw_rectangle(char canvas[HEIGHT][WIDTH], int x, int y, int w, int h) {
    for (int i = 0; i < w; i++) {
        int px = x + i;
        if (px >= 0 && px < WIDTH) {
            if (y >= 0 && y < HEIGHT) {
                canvas[y][px] = '*';
            }
            if (y + h - 1 >= 0 && y + h - 1 < HEIGHT) {
                canvas[y + h - 1][px] = '*';
            }
        }
    }
    for (int j = 0; j < h; j++) {
        int py = y + j;
        if (py >= 0 && py < HEIGHT) {
            if (x >= 0 && x < WIDTH) {
                canvas[py][x] = '*';
            }
            if (x + w - 1 >= 0 && x + w - 1 < WIDTH) {
                canvas[py][x + w - 1] = '*';
            }
        }
    }
}

// Helper to plot 8-octant points for circle
void plot_circle_points(char canvas[HEIGHT][WIDTH], int xc, int yc, int x, int y) {
    int points[8][2] = {
        {xc + x, yc + y}, {xc - x, yc + y}, {xc + x, yc - y}, {xc - x, yc - y},
        {xc + y, yc + x}, {xc - y, yc + x}, {xc + y, yc - x}, {xc - y, yc - x}
    };
    for (int i = 0; i < 8; i++) {
        int px = points[i][0];
        int py = points[i][1];
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            canvas[py][px] = '*';
        }
    }
}

// Midpoint Circle Algorithm
void draw_circle(char canvas[HEIGHT][WIDTH], int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    plot_circle_points(canvas, xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plot_circle_points(canvas, xc, yc, x, y);
    }
}

// Triangle outline drawing by drawing 3 connecting lines
void draw_triangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(canvas, x1, y1, x2, y2);
    draw_line(canvas, x2, y2, x3, y3);
    draw_line(canvas, x3, y3, x1, y1);
}