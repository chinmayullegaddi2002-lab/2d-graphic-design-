#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROWS    40
#define COLS    80
#define MAX_OBJ 50

#define OBJ_CIRCLE    1
#define OBJ_RECTANGLE 2
#define OBJ_LINE      3
#define OBJ_TRIANGLE  4

#define FILL_CHAR   '*'
#define BORDER_CHAR '_'
#define EMPTY_CHAR  ' '

typedef struct {
    int type;
    int active;
    int cx, cy, radius;
    int rx, ry, rw, rh;
    int x1, y1, x2, y2;
    int tx[3], ty[3];
    char ch;
    char label[16];
} Object;

int main() {
    char canvas[ROWS][COLS];
    Object objects[MAX_OBJ];
    int obj_count = 0;
    int choice = 0;

    // Initial canvas clearing
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            canvas[r][c] = EMPTY_CHAR;
        }
    }

    while (1) {
        // 1. Clear the canvas buffer for redrawing
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                canvas[r][c] = EMPTY_CHAR;
            }
        }

        // 2. Draw all active objects onto the buffer
        for (int i = 0; i < obj_count; i++) {
            if (!objects[i].active) continue;

            if (objects[i].type == OBJ_CIRCLE) {
                int cx = objects[i].cx;
                int cy = objects[i].cy;
                int rad = objects[i].radius;
                char ch = objects[i].ch;
                int x = 0, y = rad, d = 1 - rad;
                
                while (x <= y) {
                    if (cy+y >= 0 && cy+y < ROWS && cx+x >= 0 && cx+x < COLS) canvas[cy+y][cx+x] = ch;
                    if (cy+y >= 0 && cy+y < ROWS && cx-x >= 0 && cx-x < COLS) canvas[cy+y][cx-x] = ch;
                    if (cy-y >= 0 && cy-y < ROWS && cx+x >= 0 && cx+x < COLS) canvas[cy-y][cx+x] = ch;
                    if (cy-y >= 0 && cy-y < ROWS && cx-x >= 0 && cx-x < COLS) canvas[cy-y][cx-x] = ch;
                    if (cy+x >= 0 && cy+x < ROWS && cx+y >= 0 && cx+y < COLS) canvas[cy+x][cx+y] = ch;
                    if (cy+x >= 0 && cy+x < ROWS && cx-y >= 0 && cx-y < COLS) canvas[cy+x][cx-y] = ch;
                    if (cy-x >= 0 && cy-x < ROWS && cx+y >= 0 && cx+y < COLS) canvas[cy-x][cx+y] = ch;
                    if (cy-x >= 0 && cy-x < ROWS && cx-y >= 0 && cx-y < COLS) canvas[cy-x][cx-y] = ch;
                    if (d < 0) d += 2 * x + 3;
                    else { d += 2 * (x - y) + 5; y--; }
                    x++;
                }
            }
            else if (objects[i].type == OBJ_RECTANGLE) {
                int rx = objects[i].rx, ry = objects[i].ry, rw = objects[i].rw, rh = objects[i].rh;
                char ch = objects[i].ch;
                
                for (int c = rx; c < rx + rw; c++) {
                    if (ry >= 0 && ry < ROWS && c >= 0 && c < COLS) canvas[ry][c] = ch;
                    if (ry+rh-1 >= 0 && ry+rh-1 < ROWS && c >= 0 && c < COLS) canvas[ry+rh-1][c] = ch;
                }
                for (int r = ry; r < ry + rh; r++) {
                    if (r >= 0 && r < ROWS && rx >= 0 && rx < COLS) canvas[r][rx] = ch;
                    if (r >= 0 && r < ROWS && rx+rw-1 >= 0 && rx+rw-1 < COLS) canvas[r][rx+rw-1] = ch;
                }
                for (int r = ry + 1; r < ry + rh - 1; r++) {
                    for (int c = rx + 1; c < rx + rw - 1; c++) {
                        if (r >= 0 && r < ROWS && c >= 0 && c < COLS) canvas[r][c] = BORDER_CHAR;
                    }
                }
            }
            else if (objects[i].type == OBJ_LINE || objects[i].type == OBJ_TRIANGLE) {
                int lines = (objects[i].type == OBJ_LINE) ? 1 : 3;
                int pts[3][4]; 
                
                if (objects[i].type == OBJ_LINE) {
                    pts[0][0] = objects[i].x1; pts[0][1] = objects[i].y1;
                    pts[0][2] = objects[i].x2; pts[0][3] = objects[i].y2;
                } else {
                    pts[0][0] = objects[i].tx[0]; pts[0][1] = objects[i].ty[0]; pts[0][2] = objects[i].tx[1]; pts[0][3] = objects[i].ty[1];
                    pts[1][0] = objects[i].tx[1]; pts[1][1] = objects[i].ty[1]; pts[1][2] = objects[i].tx[2]; pts[1][3] = objects[i].ty[2];
                    pts[2][0] = objects[i].tx[2]; pts[2][1] = objects[i].ty[2]; pts[2][2] = objects[i].tx[0]; pts[2][3] = objects[i].ty[0];
                }

                for(int l = 0; l < lines; l++) {
                    int lx1 = pts[l][0], ly1 = pts[l][1], lx2 = pts[l][2], ly2 = pts[l][3];
                    char ch = objects[i].ch;
                    int dx = abs(lx2 - lx1), sx = (lx1 < lx2) ? 1 : -1;
                    int dy = abs(ly2 - ly1), sy = (ly1 < ly2) ? 1 : -1;
                    int err = dx - dy, e2;
                    
                    while(1) {
                        if (ly1 >= 0 && ly1 < ROWS && lx1 >= 0 && lx1 < COLS) canvas[ly1][lx1] = ch;
                        if (lx1 == lx2 && ly1 == ly2) break;
                        e2 = 2 * err;
                        if (e2 > -dy) { err -= dy; lx1 += sx; }
                        if (e2 <  dx) { err += dx; ly1 += sy; }
                    }
                }
            }
        }

        // 3. Print the Canvas
        printf("\n");
        for (int c = 0; c <= COLS + 1; c++) printf("-");
        printf("\n");
        for (int r = 0; r < ROWS; r++) {
            printf("|");
            for (int c = 0; c < COLS; c++) {
                putchar(canvas[r][c]);
            }
            printf("|\n");
        }
        for (int c = 0; c <= COLS + 1; c++) printf("-");
        printf("\nObjects active: %d\n\n", obj_count);

        // 4. Menu & Input Handling
        printf("--- 2D GRAPHICS EDITOR ---\n");
        printf("1. Add Object\n");
        printf("2. Delete Object\n");
        printf("3. Modify Object\n");
        printf("4. Clear Canvas\n");
        printf("5. Quit\n");
        printf("Select an option: ");

        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n'); 
            continue;
        }

        if (choice == 1) {
            if (obj_count >= MAX_OBJ) {
                printf("Error: Canvas full!\n");
                continue;
            }
            int type;
            printf("Select Type (1=Circle, 2=Rect, 3=Line, 4=Triangle): ");
            scanf("%d", &type);
            if (type < 1 || type > 4) {
                printf("Invalid type.\n");
                continue;
            }

            Object o;
            memset(&o, 0, sizeof(Object));
            o.type = type;
            o.active = 1;
            
            char chStr[8];
            printf("Enter fill character (e.g., * or _): ");
            scanf("%s", chStr);
            o.ch = chStr[0];

            if (type == OBJ_CIRCLE) {
                strcpy(o.label, "Circle");
                printf("Center X: "); scanf("%d", &o.cx);
                printf("Center Y: "); scanf("%d", &o.cy);
                printf("Radius: "); scanf("%d", &o.radius);
            } else if (type == OBJ_RECTANGLE) {
                strcpy(o.label, "Rectangle");
                printf("Top-Left X: "); scanf("%d", &o.rx);
                printf("Top-Left Y: "); scanf("%d", &o.ry);
                printf("Width: "); scanf("%d", &o.rw);
                printf("Height: "); scanf("%d", &o.rh);
            } else if (type == OBJ_LINE) {
                strcpy(o.label, "Line");
                printf("Start X: "); scanf("%d", &o.x1);
                printf("Start Y: "); scanf("%d", &o.y1);
                printf("End X: "); scanf("%d", &o.x2);
                printf("End Y: "); scanf("%d", &o.y2);
            } else if (type == OBJ_TRIANGLE) {
                strcpy(o.label, "Triangle");
                for (int i = 0; i < 3; i++) {
                    printf("Vertex %d X: ", i+1); scanf("%d", &o.tx[i]);
                    printf("Vertex %d Y: ", i+1); scanf("%d", &o.ty[i]);
                }
            }
            objects[obj_count++] = o;
            printf("Object added successfully!\n");
        } 
        else if (choice == 2) {
            printf("\n--- Active Objects ---\n");
            for (int i = 0; i < obj_count; i++) {
                if (objects[i].active) {
                    printf("[%d] %s (Char: '%c')\n", i, objects[i].label, objects[i].ch);
                }
            }
            int idx;
            printf("Enter index to delete: ");
            scanf("%d", &idx);
            if (idx >= 0 && idx < obj_count) {
                objects[idx].active = 0;
                printf("Object deleted.\n");
            } else {
                printf("Invalid index.\n");
            }
        } 
        else if (choice == 3) {
            printf("\n--- Active Objects ---\n");
            for (int i = 0; i < obj_count; i++) {
                if (objects[i].active) {
                    printf("[%d] %s (Char: '%c')\n", i, objects[i].label, objects[i].ch);
                }
            }
            int idx;
            printf("Enter index to modify: ");
            scanf("%d", &idx);
            if (idx >= 0 && idx < obj_count && objects[idx].active) {
                char chStr[8];
                printf("Enter new fill character: ");
                scanf("%s", chStr);
                objects[idx].ch = chStr[0];

                if (objects[idx].type == OBJ_CIRCLE) {
                    printf("New Center X: "); scanf("%d", &objects[idx].cx);
                    printf("New Center Y: "); scanf("%d", &objects[idx].cy);
                    printf("New Radius: "); scanf("%d", &objects[idx].radius);
                }
                // Can expand logic here for modifying Rect, Line, Triangle
                printf("Object modified.\n");
            } else {
                printf("Invalid or inactive index.\n");
            }
        } 
        else if (choice == 4) {
            obj_count = 0;
            printf("Canvas cleared.\n");
        } 
        else if (choice == 5) {
            break;
        }
    }
    
    return 0;
}