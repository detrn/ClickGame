#include "../include/raylib.h"
#include <stdio.h>
#define GAME_DIFFICULTY 5 
#define SQUARES 3 
#define CIRCLES 3 
#define WIDTH 600
#define HEIGHT 600
#define TOTAL_TARGETS (SQUARES + CIRCLES) 
#define MAX_OBJ_WIDTH 50
#define MAX_OBJ_HEIGHT 50
typedef struct{
    Vector2 center;
    float radius;//radius 
    Color color; 
}Circle; 

typedef enum{
    circle = 0, 
    rectangle
}TargetType;

typedef struct{ 
    union{
	Rectangle rec;
	Circle circ;
    };
    int isVisibile;
    double timeAlive ;
    TargetType type;
    Color color; 
}Target;

typedef struct{
    double time; 
    int scor; 
    int gameOver;
    Target targets[SQUARES + CIRCLES];
    int targets_drawn;
}Game;

Color randomColor(){
    return (Color){ .r =GetRandomValue(0,255), .g = GetRandomValue(0,255),.b=GetRandomValue(0,255), .a = 255};
}

void changePosition(Target* target){
    target->timeAlive = GAME_DIFFICULTY;
    Vector2 newPos = (Vector2){ .x = GetRandomValue(0, WIDTH - MAX_OBJ_WIDTH), .y = GetRandomValue(100, HEIGHT-MAX_OBJ_HEIGHT)};
    target->color = randomColor();
    if(target->type == circle){

	target->circ.center = newPos;
    }
    else{
	target->rec.x = newPos.x;
	target->rec.y = newPos.y;
    }
}


void targetInit(Game* game){
    for(int i = 0; i < TOTAL_TARGETS; i++){
	Target* aux = &game->targets[i];
	if(i%2==0){
	    aux->type = rectangle;
	    aux->rec.width = 50;
	    aux->rec.height = 50;
	    changePosition(&game->targets[i]);
	    aux->isVisibile = 0; 
	    aux->timeAlive = GAME_DIFFICULTY;
	}
	else{
	    aux->type = circle;
	    changePosition(&game->targets[i]);
	    aux->circ.radius = 25;
	}
	aux->color = randomColor();     
    }
    game->targets[0].isVisibile = 1;
    
}
void gameInit(Game* game){
    game->time = 10; //*scalar
    game->scor = 0; 
    game->gameOver = 0; 
    game->targets_drawn = 1; 
    targetInit(game);

}

void setDifficulty(Game* game){
    for(int i = 0 ; i < TOTAL_TARGETS; i++){
	game->targets[i].timeAlive = GAME_DIFFICULTY;
    }
}

void drawTarget(Target* target){
    if(target->type == circle) DrawCircle(target->circ.center.x ,target->circ.center.y, target->circ.radius, target->color);
    else DrawRectangle(target->rec.x, target->rec.y, target->rec.width, target->rec.height,target->color);
}
void debug_targets(Target* target){
    if(target->type == circle){
	printf("Circle x: %f, y: %f\n", target->circ.center.x, target->circ.center.y);
    }
    else printf("Rectangle x:%f, y:%f, w:%f, h:%f\n", target->rec.x,target->rec.y, target->rec.width,target->rec.height);
}

void gameOver(Game* game){
    ClearBackground(WHITE);
    char buffer[250];
    snprintf(buffer,sizeof(buffer),"GAME OVER\nScor:%d\nApasa click \npentru a reincepe\n",game->scor);
    DrawText(buffer,150, 200, 32, BLACK);
    if(IsMouseButtonPressed(0)) gameInit(game);
}

void drawGame(Game* game){
    if(game->gameOver){
	gameOver(game);
	return;
    }
    char buffer_scor[250];
    snprintf(buffer_scor, sizeof(buffer_scor),"Scor: %d",game->scor);
    char buffer_timp[250];
    snprintf(buffer_timp,sizeof(buffer_timp),"Timp Ramas:%0.f",game->time);

    DrawText(buffer_scor,10, 10, 32, BLACK);
    DrawText(buffer_timp,370,10,32,BLACK);
    for(int i = 0 ; i < game->targets_drawn; i++){
	if(game->targets[i].isVisibile) {
	    drawTarget(&game->targets[i]);
	}
    }
}
int isTargetClicked(Target* target){
    Vector2 vec = {.x = GetMouseX(), .y = GetMouseY()};
    if(target->type == circle && target->isVisibile){ 
	return CheckCollisionPointCircle(vec, target->circ.center,target->circ.radius);
    }
    if(target->isVisibile){
	return CheckCollisionPointRec(vec, target->rec );
    }
    return 0;
}

void clickAction(Game* game){
    if(!IsMouseButtonPressed(0)) return;
    Vector2 mouse = GetMousePosition();
    printf("\nCLICK: mouse=(%.1f, %.1f)\n", mouse.x, mouse.y);
    for(int i = 0 ; i < game->targets_drawn; i++){
	    printf("target[%d], type=%d, collision=%d\n",i,game->targets[i].type, isTargetClicked(&game->targets[i]));
	    debug_targets(&game->targets[i]);
    	    if(isTargetClicked(&game->targets[i])){
	    	changePosition(&game->targets[i]);
		game->scor++;
		if(game->targets_drawn < TOTAL_TARGETS){
		    game->targets[game->targets_drawn].isVisibile = 1;
		    game->targets_drawn++; //incepe: tdrwn = 2, aux = 1 (0,1); tdrwn = 3, aux = 2 (0,1,2); tdrwn = 4, aux = 3 (0,1,2,3); tdrwn = 5, aux = 4 (0,1,2,3,4)
		}
		break;
	    }
    }
}
void update(Game* game){
    if(game->time <= 0) {
	game->gameOver = 1;
	return;
    }
    float dt = GetFrameTime();
    game->time-=dt;
    for(int i = 0 ; i < game->targets_drawn; i++){
	if(game->targets[i].timeAlive <= 0) changePosition(&game->targets[i]);
	else{
	    float dt = GetFrameTime();
	    game->targets[i].timeAlive-=dt; 
	}
    }
    clickAction(game);
}
void debug(Game* game){
    static Game prev; 
    static int initialized = 0; 
    if(!initialized){
	//dump 
	for(int i = 0 ; i < TOTAL_TARGETS; i++){
	    printf("Tip forme: %d\n", game->targets[i].type);
	}
	prev = *game;
	initialized = 1;
	return;
    }
    
    if( prev.scor != game->scor) printf("Scor: %d\n",prev.scor);
    if(prev.targets_drawn != game->targets_drawn){
	printf("Targets drawn: %d\n", prev.targets_drawn);
    }
    //if(prev.time != game->time) printf("Timp: %f\n",prev.time);
    prev = *game;
    if(game->scor == 6){
	printf("Visibile status:\n");
	for(int i = 0 ; i < TOTAL_TARGETS; i++){
	    printf("%d ", game->targets[i].isVisibile);
	}
    }    
}

int main(){
    Game game; 
    gameInit(&game);
    SetTargetFPS(60);
    InitWindow(WIDTH,HEIGHT,"Test");
    while(!WindowShouldClose()){
	if(!game.gameOver)
	{
	    update(&game);
	    debug(&game);
	}
	BeginDrawing();
	ClearBackground(WHITE);
	drawGame(&game); 
    	EndDrawing();
    }
   CloseWindow();
   return 0;
}
