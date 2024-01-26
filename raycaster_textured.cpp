#include <cmath>
#include <string>
#include <vector>
#include <iostream>

#include <SDL/SDL.h>
#include "Timer.hpp"

#include "quickcg.h"
using namespace QuickCG;

/*
g++ *.cpp -lSDL -O3 -W -Wall -ansi -pedantic
g++ raycaster_textured.cpp *.o -lSDL
*/


#define screenWidth 640
#define screenHeight 480
#define texWidth 64
#define texHeight 64
#define mapWidth 24
#define mapHeight 24

#define textureMax 9

struct Vec2INT {
  int x, y;
  public:
    Vec2INT(const int rx, const int ry) {
      x = rx;
      y = ry;
    }
};

struct Vec2 {
  double x, y;
    Vec2(const double rx, const double ry) {
    x = rx;
    y = ry;
  }
};

struct Vec3 {
  double x, y, z;
    Vec3(const double rx, const double ry, const double rz) {
    x = rx;
    y = ry;
    z = rz;
  }
};

int worldMap[mapWidth][mapHeight]=
{
  {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
  {4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
  {4,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,4,0,0,0,0,5,5,5,5,5,5,5,5,5,7,7,0,7,7,7,7,7},
  {4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,7,0,0,0,7,7,7,1},
  {4,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
  {4,0,7,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,7,7,7,1},
  {4,0,8,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
  {4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,7,7,7,1},
  {4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,7,7,7,7,7,7,7,1},
  {6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
  {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
  {6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
  {4,4,4,4,4,4,0,4,4,4,6,0,6,2,2,2,2,2,2,2,3,3,3,3},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,0,0,0,6,2,0,0,5,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
  {4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,2},
  {4,0,0,5,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
  {4,0,6,0,6,0,0,0,0,4,6,0,6,2,0,0,5,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
  {4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3}
};

std::vector<Uint32> texture[textureMax];
Uint32 buffer[screenHeight][screenWidth];


void clearBuffer(Uint32 buffer[screenHeight][screenWidth], int h, int w) {
  for(int y = 0; y < h; y++) for(int x = 0; x < w; x++) buffer[y][x] = 0;
}

void loadTextures() {

  for(int i = 0; i < textureMax; i++) texture[i].resize(texWidth * texHeight);

  unsigned long tw, th;
  loadImage(texture[0], tw, th, "pics/eagle.png");
  loadImage(texture[1], tw, th, "pics/redbrick.png");
  loadImage(texture[2], tw, th, "pics/purplestone.png");
  loadImage(texture[3], tw, th, "pics/greystone.png");
  loadImage(texture[4], tw, th, "pics/bluestone.png");
  loadImage(texture[5], tw, th, "pics/mossy.png");
  loadImage(texture[6], tw, th, "pics/wood.png");
  loadImage(texture[7], tw, th, "pics/colorstone.png");
  loadImage(texture[8], tw, th, "pics/enemy.png");
}

double calcStepInitialSideDist(const double rayDir, int & step, const int map, const double pos, const double deltaDist) {
  if(rayDir < 0) {
    step = -1;
    return (pos - map) * deltaDist;
  }
  step = 1;
  return (map + 1.0 - pos) * deltaDist;
}


double getFrameTime(double & time, double & oldTime) {
    //timing for input and FPS counter
    oldTime = time;
    time = getTicks();
    return (time - oldTime) / 1000.0; //frametime is the time this frame has taken, in seconds
}

void translate(Vec2 & pos, const Vec2 dir, const double moveSpeed, const Vec3 orgDir) {
  if(worldMap[int(pos.x + orgDir.x * dir.x * moveSpeed)][int(pos.y)] == false) pos.x += orgDir.x * dir.x * moveSpeed;
  if(worldMap[int(pos.x)][int(pos.y + orgDir.x * dir.y * moveSpeed)] == false) pos.y += orgDir.x * dir.y * moveSpeed;
}

void rotate(Vec2 & dir, Vec2 & plane, const double rotSpeed, const Vec3 orgDir) {
  double oldDirX = dir.x;

  dir.x = dir.x * cos(orgDir.z * rotSpeed) - dir.y * sin(orgDir.z * rotSpeed);
  dir.y = oldDirX * sin(orgDir.z * rotSpeed) + dir.y * cos(orgDir.z * rotSpeed);

  double oldPlaneX = plane.x;

  plane.x = plane.x * cos(orgDir.z * rotSpeed) - plane.y * sin(orgDir.z * rotSpeed);
  plane.y = oldPlaneX * sin(orgDir.z * rotSpeed) + plane.y * cos(orgDir.z * rotSpeed);
}

void keyManagment(Vec2 & pos, Vec2 & dir, Vec2 & plane, const double moveSpeed, const double rotSpeed) {
  //move forward if no wall in front of you
  if(keyDown(SDLK_UP)) {
    translate(pos, dir, moveSpeed, Vec3(1, 0, 0));
  }
  //move backwards if no wall behind you
  if(keyDown(SDLK_DOWN)) {
    translate(pos, dir, moveSpeed, Vec3(-1, 0, 0));
  }
  //rotate to the right
  if(keyDown(SDLK_RIGHT)) {
    rotate(dir, plane, rotSpeed, Vec3(0, 0, -1));
  }
  //rotate to the left
  if(keyDown(SDLK_LEFT)) {
    rotate(dir, plane, rotSpeed, Vec3(0, 0, 1));
  }
}


int main(int argc, char * argv[])
{
  Vec2 pos(22.0, 11.5);//x and y start position
  Vec2 dir(-1.0, 0.0); //initial direction vector
  Vec2 plane(0.0, 0.70); //the 2d raycaster version of camera plane


  Timer timer;

  screen(screenWidth,screenHeight, 0, "Raycaster");
  loadTextures();

  //start the main loop
  while(!done()) {
    for(int x = 0; x < w; x++) {
      //calculate ray position and direction
      double cameraX = 2 * x / (double)w - 1; //x-coordinate in camera space
      double rayDirX = dir.x + plane.x*cameraX;
      double rayDirY = dir.y + plane.y*cameraX;

      //which box of the map we're in
      int mapX = int(pos.x);
      int mapY = int(pos.y);

      //length of ray from current position to next x or y-side
      double sideDistX;
      double sideDistY;

      //length of ray from one x or y-side to next x or y-side
      double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
      double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);
      double perpWallDist;

      //what direction to step in x or y-direction (either +1 or -1)
      int stepX;
      int stepY;

      int hit = 0; //was there dir.x wall hit?
      int side; //was dir.x NS or dir.x EW wall hit?

      //calculate step and initial sideDist
      sideDistX = calcStepInitialSideDist(rayDirX, stepX, mapX, pos.x, deltaDistX);
      sideDistY = calcStepInitialSideDist(rayDirY, stepY, mapY, pos.y, deltaDistY);

      //perform DDA
      while (hit == 0) {
        //jump to next map square, either in x-direction, or in y-direction
        if(sideDistX < sideDistY) {
          sideDistX += deltaDistX;
          mapX += stepX;
          side = 0;
        } else {
          sideDistY += deltaDistY;
          mapY += stepY;
          side = 1;
        }
        //Check if ray has hit dir.x wall
        if(worldMap[mapX][mapY] > 0) hit = 1;
      }

      //Calculate distance of perpendicular ray (Euclidean distance would give fisheye effect!)
      if(side == 0) perpWallDist = (sideDistX - deltaDistX);
      else          perpWallDist = (sideDistY - deltaDistY);

      //Calculate height of line to draw on screen
      int lineHeight = (int)(h / perpWallDist);


      int pitch = 100;

      //calculate lowest and highest pixel to fill in current stripe
      int drawStart = -lineHeight / 2 + h / 2 + pitch;
      if(drawStart < 0) drawStart = 0;
      int drawEnd = lineHeight / 2 + h / 2 + pitch;
      if(drawEnd >= h) drawEnd = h - 1;

      //texturing calculations
      int texNum = worldMap[mapX][mapY] - 1; //1 subtracted from it so that texture 0 can be used!

      //calculate value of wallX
      double wallX; //where exactly the wall was hit
      if(side == 0) wallX = pos.y + perpWallDist * rayDirY;
      else          wallX = pos.x + perpWallDist * rayDirX;
      wallX -= floor((wallX));

      //x coordinate on the texture
      int texX = int(wallX * double(texWidth));
      if(side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
      if(side == 1 && rayDirY < 0) texX = texWidth - texX - 1;

      // TODO: an integer-only bresenham or DDA like algorithm could make the texture coordinate stepping faster
      // How much to increase the texture coordinate per screen pixel
      double step = 1.0 * texHeight / lineHeight;
      // Starting texture coordinate
      double texPos = (drawStart - pitch - h / 2 + lineHeight / 2) * step;
      for(int y = drawStart; y < drawEnd; y++) {
        // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
        int texY = (int)texPos & (texHeight - 1);
        texPos += step;
        Uint32 color = texture[texNum][texHeight * texY + texX];
        //make color darker for y-sides: R, G and B byte each divided through two with dir.x "shift" and an "and"
        if(side == 1) color = (color >> 1) & 8355711;
        buffer[y][x] = color;
      }
    }

    drawBuffer(buffer[0]);
    clearBuffer(buffer, h, w); //clear the buffer instead of cls()

    timer.update(QuickCG::getTicks());
    print(timer.frame_rate); //FPS counter

    redraw();

    //speed modifiers
    double moveSpeed = timer.frame_time * 5.0; //the constant value is in squares/second
    double rotSpeed = timer.frame_time * 3.0; //the constant value is in radians/second

    readKeys();

    keyManagment(pos, dir, plane, moveSpeed, rotSpeed);

    if(keyDown(SDLK_ESCAPE)) {
      break;
    }
    SDL_Delay(10);
  }
}
