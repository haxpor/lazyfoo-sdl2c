/**
 * 51 - SDL and Modern OpenGL
 */

#include "SDL.h"
#include <stdio.h>
#include <stdbool.h>
#include "common.h"
#include "LTimer.h"
#include "LTexture.h"
#include "LWindow.h"
#include "glew.h" // include this header before gl.h
#include "gl.h"
#include "glu.h"
#include <string.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SETFRAME(var, arg1, arg2, arg3, arg4)		\
  do {										\
    var.x = arg1;							\
    var.y = arg2;							\
    var.w = arg3;							\
    var.h = arg4;							\
  } while(0)

// cap thus using fixed deltaTime step
#define TARGET_FPS 60
#define FIXED_DELTATIME 1.0f / TARGET_FPS

// -- functions
bool init();
bool initGL();
bool setup();
void handleEvent(SDL_Event *e, float deltaTime);
void update(float deltaTime);
void render(float deltaTime);
void close();

// opengl context
SDL_GLContext opengl_context;

// -- variables
bool quit = false;

// independent time loop
Uint32 currTime = 0;
Uint32 prevTime = 0;

#ifndef DISABLE_FPS_CALC
#define FPS_BUFFER 7+1
char fpsText[FPS_BUFFER];
#endif

// content's rect to clear color in render loop
SDL_Rect content_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

// shader loading utility programs
void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);

// graphics program
GLuint g_programid = 0;
GLint g_vertex_pos2d_location = -1;
GLuint g_vbo = 0;
GLuint g_ibo = 0;
GLuint g_vao = 0;

// whether or not to render quad
bool is_render_quad = true;

bool init() {
  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return false;
  }

  // use opengl 3.1 core
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  // create window
  // if we set SDL_WINDOW_OPENGL flag then renderer won't be created for this window
  // thus make sure you cannot use LTexture anymore as it heavilty use renderer as created in LWindow
  gWindow = LWindow_new("51 - SDL and Modern OpenGL", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL, 0);
  if (gWindow == NULL) {
    SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
    return false;
  }

  // create opengl context
  opengl_context = SDL_GL_CreateContext(gWindow->window);
  if (opengl_context == NULL)
  {
    SDL_Log("OpenGL context could not be created: %s", SDL_GetError());
    return false;
  }

  // use vsync
  if (SDL_GL_SetSwapInterval(1) != 0)
  {
    SDL_Log("Warning: Unable to enable vsync! %s", SDL_GetError());
  }

  // init glew
  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK)
  {
    SDL_Log("Failed initialize glew! %s", glewGetErrorString(glewError));
    return false;
  }

  // init gl
  if (!initGL())
  {
    SDL_Log("Failed to initialize opengl");
    return false;
  }

  // check opengl version we got
  printf("OpenGL version %s\nGLSL version: %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

  // initialize png loading
  // see https://www.libsdl.org/projects/SDL_image/docs/SDL_image.html#SEC8
  // returned from IMG_Init is all flags initted, so we could check for all possible
  // flags we aim for
  int imgFlags = IMG_INIT_PNG;
  int inittedFlags = IMG_Init(imgFlags);
  if ( (inittedFlags & imgFlags) != imgFlags)
  {
    // from document, not always that error string from IMG_GetError() will be set
    // so don't depend on it, just for pure information
    SDL_Log("SDL_Image could not initialize! SDL_image Error: %s", IMG_GetError());
    return false;
  }

#ifndef DISABLE_SDL_TTF_LIB
  // initialize SDL_ttf
  if (TTF_Init() == -1)
  {
    SDL_Log("SDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError());
    return false;
  }
#endif

  return true;
}

bool initGL()
{
  // create program
  g_programid = glCreateProgram();

  // create vertex shader
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

  // get vertex source
  const GLchar* vertex_shader_source[] = {
    "#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4(LVertexPos2D.x, LVertexPos2D.y, 0, 1); }"
  };

  // set vertex source
  glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);

  // compile vertex source
  glCompileShader(vertex_shader);

  // check vertex shader
  GLint vertex_compiled = GL_FALSE;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_compiled);
  if (vertex_compiled != GL_TRUE)
  {
    SDL_Log("Unable to compile vertex shader %d", vertex_shader);
    printShaderLog(vertex_shader);
    return false;
  } 

  // attach vertex shader to program
  glAttachShader(g_programid, vertex_shader);

  // --

  // create fragment shader
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  // get fragment source
  const GLchar* fragment_shader_source[] = {
    "#version 140\nout vec4 LFragment; void main() { LFragment = vec4(1.0, 1.0, 1.0, 1.0); }"
  };

  // set fragment source
  glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);

  // compile fragment source
  glCompileShader(fragment_shader);

  // check fragment shader for errors
  GLint fragment_compiled = GL_FALSE;
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_compiled);
  if (fragment_compiled != GL_TRUE)
  {
    SDL_Log("Unable to compile fragment shader %d", fragment_shader);
    printShaderLog(fragment_shader);
    return false;
  }

  // attach fragment shader to program
  glAttachShader(g_programid, fragment_shader);

  // link program
  glLinkProgram(g_programid);

  // check for errors
  GLint program_success = GL_FALSE;
  glGetProgramiv(g_programid, GL_LINK_STATUS, &program_success);
  if (program_success != GL_TRUE)
  {
    SDL_Log("Error linking shader program %d", g_programid);
    printProgramLog(g_programid);
    return false;
  }

  // create vao for working with opengl 3.2+
  glGenVertexArrays(1, &g_vao);
   
  // get vertex attribute location
  g_vertex_pos2d_location = glGetAttribLocation(g_programid, "LVertexPos2D");
  if (g_vertex_pos2d_location == -1)
  {
    SDL_Log("LVertexPos2D is not a valid glsl program variable");
    return false;
  }

  // initialize clear color
  glClearColor(0.f, 0.f, 0.f, 1.f);

  // vbo data
  GLfloat vertex_data[] = 
  {
    -0.5f, -0.5f,
    0.5f, -0.5f,
    0.5f, 0.5f,
    -0.5f, 0.5f
  };

  // ibo data
  GLuint index_data[] = { 0, 1, 2, 3 };

  // create VBO
  glGenBuffers(1, &g_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertex_data, GL_STATIC_DRAW);

  // create IBO
  glGenBuffers(1, &g_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), index_data, GL_STATIC_DRAW);

  return true;
}

void printProgramLog(GLuint program)
{
  // make sure name is program
  if (glIsProgram(program))
  {
    // program log length
    int info_loglength = 0;
    int maxlength = info_loglength;

    // get info string length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxlength);

    // allocate string
    char infolog[maxlength];
    memset(infolog, 0, maxlength);
    
    // get info log
    glGetProgramInfoLog(program, maxlength, &info_loglength, infolog);
    if (info_loglength > 0)
    {
      printf("%s", infolog);
    }
  }
  else
  {
    SDL_Log("Name %d is not a program.", program);
  }
}

void printShaderLog(GLuint shader)
{
  // make sure name is shader
  if (glIsShader(shader))
  {
    // shader log length
    int info_loglength = 0;
    int maxlength = info_loglength;

    // get info string length
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxlength);

    // allocate string
    char infolog[maxlength];
    memset(infolog, 0, maxlength);

    // get info log
    glGetShaderInfoLog(shader, maxlength, &info_loglength, infolog);
    if (info_loglength > 0)
    {
      printf("%s", infolog);
    }
  }
  else
  {
    SDL_Log("Name %d is not a shader.", shader);
  }
}

// include any asset loading sequence, and preparation code here
bool setup()
{
  // load font
  gFont = TTF_OpenFont("../Minecraft.ttf", 16);
  if (gFont == NULL)
  {
    SDL_Log("Failed to load Minecraft.ttf font: %s", TTF_GetError());
    return false;
  }

  return true;
}

void handleEvent(SDL_Event *e, float deltaTime)
{
  // user requests quit
  if (e->type == SDL_QUIT ||
      (e->key.keysym.sym == SDLK_ESCAPE))
  {
    quit = true;
  }
  // toggle fullscreen via enter key
  else if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_RETURN)
  {
    if (gWindow->_fullscreen)
    {
      // 0 for windowed mode
      SDL_SetWindowFullscreen(gWindow->window, 0);
      gWindow->_fullscreen = false;
    }
    else
    {
      // SDL_WINDOW_FULLSCREEN_DESKTOP for "fake" fullscreen without changing videomode
      // depends on type of game, and performance aim i.e. FPS game might want to do "real" fullscreen
      // by changing videomode to get performance gain, but point and click with top-down tile-based
      // might not need to change videomode to match the desire spec.
      //
      // as well this needs to work with SDL_RenderSetLogicalSize() function to make it works.
      SDL_SetWindowFullscreen(gWindow->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
      gWindow->_fullscreen = true;
      gWindow->is_minimized = false;
    }
  }
}

void update(float deltaTime)
{

}

void render(float deltaTime)
{
  if (!gWindow->is_minimized)
  {
    // clear color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // render quad
    if (is_render_quad)
    {
      // bind program
      glUseProgram(g_programid);

      // bind vao
      glBindVertexArray(g_vao);

      // enable vertex position
      glEnableVertexAttribArray(g_vertex_pos2d_location);

      // set vertex data
      glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
      glVertexAttribPointer(g_vertex_pos2d_location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

      // set index data and render
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
      glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

      // unbind all stuff
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      // unbind program
      glUseProgram(0);
    }
  }
}

void close()
{
  // free font
  if (gFont != NULL)
  {
    TTF_CloseFont(gFont);
    gFont = NULL;
  }

  // destroy window
  LWindow_free(gWindow);

#ifndef DISABLE_SDL_TTF_LIB
  TTF_Quit();
#endif

  IMG_Quit();
  SDL_Quit();
}

int main(int argc, char* args[])
{
  // start up SDL and create window
  if (!init())
  {
    SDL_Log("Failed to initialize");
  }	
  else
  {
    // load media, and set up
    if (!setup())
    {
      SDL_Log("Failed to setup!");
    }
    else
    {
      // event handler
      SDL_Event e;

      // while application is running
      while (!quit)
      {
        // prepare delta time to feed to both handleEvent(), update(), and render()
        prevTime = currTime;
        currTime = SDL_GetTicks();
        // calculate per second
        float deltaTime = (currTime - prevTime) / 1000.0f;

#ifndef DISABLE_FPS_CALC
        // fixed step
        common_frameTime += deltaTime;
        common_frameAccumTime += deltaTime;
#endif
        if (common_frameTime >= FIXED_DELTATIME)
        {
#ifndef DISABLE_FPS_CALC
          common_frameCount++;

          // check to reset frame time
          if (common_frameAccumTime >= 1.0f)
          {
            common_avgFPS = common_frameCount / common_frameAccumTime;
            common_frameCount = 0;
            common_frameAccumTime -= 1.0f;
          }
#endif
          common_frameTime = 0.0f;

          // handle events on queue
          // if it's 0, then it has no pending event
          // we keep polling all event in each game loop until there is no more pending one left
          while (SDL_PollEvent(&e) != 0)
          {
            // update user's handleEvent()
            handleEvent(&e, FIXED_DELTATIME);
          }

          update(FIXED_DELTATIME);
          render(FIXED_DELTATIME);
        }
        else {
          render(0); 
        }

        // update screen
        SDL_GL_SwapWindow(gWindow->window);
      }
    }
  }

  // free resource and close SDL
  close();

  return 0;
}

