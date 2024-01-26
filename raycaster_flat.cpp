
#include <cmath>
#include <string>
#include <vector>
#include <iostream>

#include "Map.hpp"
#include "Timer.hpp"

#include "quickcg.h"
using namespace QuickCG;

#define screenWidth 640
#define screenHeight 480

struct Player {
  double x, y;  //x and y start position
  double rx, ry; //initial direction vector

  Player(double posX, double posY, double dirX, double dirY) {
    x = posX;
    y = posY;
    rx = dirX;
    ry = dirY;
  }
};

ColorRGB select_color(int ID){
  ColorRGB color;
  switch(ID) {
    case 1:  color = RGB_Red;    break; 
    case 2:  color = RGB_Green;  break; 
    case 3:  color = RGB_Blue;   break; 
    case 4:  color = RGB_White;  break;
    default: color = RGB_Yellow; break;
  }
  return color;
}

void render(Player & p, Map & worldMap, double & planeX, double & planeY) {
  for(int x = 0; x < w; x++) {
      //calculate ray position and direction
      double cameraX = 2 * x / (double)w - 1; //x-coordinate in camera space
      double rayDirX = p.rx + planeX * cameraX;
      double rayDirY = p.ry + planeY * cameraX;
      //which box of the map we're in
      int mapX = int(p.x);
      int mapY = int(p.y);

      //length of ray from current position to next x or y-side
      double sideDistX;
      double sideDistY;

      //length of ray from one x or y-side to next x or y-side
      //these are derived as:
      //deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX))
      //deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY))
      //which can be simplified to abs(|rayDir| / rayDirX) and abs(|rayDir| / rayDirY)
      //where |rayDir| is the length of the vector (rayDirX, rayDirY). Its length,
      //unlike (p.rx, p.ry) is not 1, however this does not matter, only the
      //ratio between deltaDistX and deltaDistY matters, due to the way the DDA
      //stepping further below works. So the values can be computed as below.
      // Division through zero is prevented, even though technically that's not
      // needed in C++ with IEEE 754 floating point values.
      double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
      double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);

      double perpWallDist;

      //what direction to step in x or y-direction (either +1 or -1)
      int stepX;
      int stepY;

      int hit = 0; //was there a wall hit?
      int side; //was a NS or a EW wall hit?
      //calculate step and initial sideDist

      if(rayDirX < 0) {
        stepX = -1;
        sideDistX = (p.x - mapX) * deltaDistX;
      } else {
        stepX = 1;
        sideDistX = (mapX + 1.0 - p.x) * deltaDistX;
      }

      if(rayDirY < 0) {
        stepY = -1;
        sideDistY = (p.y - mapY) * deltaDistY;
      } else {
        stepY = 1;
        sideDistY = (mapY + 1.0 - p.y) * deltaDistY;
      }

      //perform DDA
      while(hit == 0) {
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
        //Check if ray has hit a wall
        if(worldMap(mapX, mapY) > 0) hit = 1;
      }

      //Calculate distance projected on camera direction. This is the shortest distance from the point where the wall is
      //hit to the camera plane. Euclidean to center camera point would give fisheye effect!
      //This can be computed as (mapX - p.x + (1 - stepX) / 2) / rayDirX for side == 0, or same formula with Y
      //for size == 1, but can be simplified to the code below thanks to how sideDist and deltaDist are computed:
      //because they were left scaled to |rayDir|. sideDist is the entire length of the ray above after the multiple
      //steps, but we subtract deltaDist once because one step more into the wall was taken above.
      
      if(side == 0) perpWallDist = (sideDistX - deltaDistX);
      else          perpWallDist = (sideDistY - deltaDistY);

      //Calculate height of line to draw on screen
      int lineHeight = (int)(h / perpWallDist);

      //calculate lowest and highest pixel to fill in current stripe
      int drawStart = -lineHeight / 2 + h / 2;
      if(drawStart < 0) drawStart = 0;
      int drawEnd = lineHeight / 2 + h / 2;
      if(drawEnd >= h) drawEnd = h - 1;

      //choose wall color
      ColorRGB color = select_color(worldMap(mapX, mapY));

      //give x and y sides different brightness
      if(side == 1) {color = color / 2;}

      //draw the pixels of the stripe as a vertical line
      verLine(x, drawStart, drawEnd, color);
    }
}



int main(int /*argc*/, char */*argv*/[])
{

  Player p(22, 12, -1, 0);
  
  double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane

  Timer timer;

  screen(screenWidth, screenHeight, 0, "Raycaster");

  Map worldMap("test.bin");

  while(!done()) {
    render(p, worldMap, planeX, planeY);
    //timing for input and FPS counter
    timer.update();
    print(1.0 / timer.frame_time);

    redraw();
    cls();

    //speed modifiers
    double moveSpeed = timer.frame_time * 5.0; //the constant value is in squares/second
    double rotSpeed = timer.frame_time * 3.0; //the constant value is in radians/second
    readKeys();
    //move forward if no wall in front of you
    if(keyDown(SDLK_UP)) {
      if(worldMap(int(p.x + p.rx * moveSpeed), int(p.y)) == false) p.x += p.rx * moveSpeed;
      if(worldMap(int(p.x), int(p.y + p.ry * moveSpeed)) == false) p.y += p.ry * moveSpeed;
    }
    //move backwards if no wall behind you
    if(keyDown(SDLK_DOWN)) {
      if(worldMap(int(p.x - p.rx * moveSpeed), int(p.y)) == false) p.x -= p.rx * moveSpeed;
      if(worldMap(int(p.x), int(p.y - p.ry * moveSpeed)) == false) p.y -= p.ry * moveSpeed;
    }
    //rotate to the right
    if(keyDown(SDLK_RIGHT)) {
      //both camera direction and camera plane must be rotated
      double oldDirX = p.rx;
      p.rx = p.rx * cos(-rotSpeed) - p.ry * sin(-rotSpeed);
      p.ry = oldDirX * sin(-rotSpeed) + p.ry * cos(-rotSpeed);
      double oldPlaneX = planeX;
      planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
      planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    }
    //rotate to the left
    if(keyDown(SDLK_LEFT)) {
      //both camera direction and camera plane must be rotated
      double oldDirX = p.rx;
      p.rx = p.rx * cos(rotSpeed) - p.ry * sin(rotSpeed);
      p.ry = oldDirX * sin(rotSpeed) + p.ry * cos(rotSpeed);
      double oldPlaneX = planeX;
      planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
      planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
    }
  }
}
