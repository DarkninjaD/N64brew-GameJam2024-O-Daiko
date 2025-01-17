#include <libdragon.h>
#include "../../core.h"
#include "../../minigame.h"
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

const MinigameDef minigame_def = {
    .gamename = "something",
    .developername = "Your Name",
    .description = "This is an example game.",
    .instructions = "Press A to win."
};

float timer;

T3DViewport viewport;
T3DVec3 lightDirVec;
T3DVec3 camPos;
T3DVec3 camTarget;
surface_t *depthBuffer;
T3DModel *model;
T3DMat4 modelMat;
T3DMat4FP* modelMatFP;
float modelScale;
float rotAngle;

/*==============================
    minigame_init
    The minigame initialization function
==============================*/
void minigame_init()
{
  timer = 10.0f;
  modelScale = 1.0f;
  rotAngle = 0.02;
  t3d_mat4_identity(&modelMat);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  depthBuffer = display_get_zbuf();
  t3d_init((T3DInitParams){});
  viewport = t3d_viewport_create();
  modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  camPos = (T3DVec3){{0, 125.0f, 100.0f}};
  camTarget = (T3DVec3){{0, 0, 40}};
  lightDirVec = (T3DVec3){{1.0f, 1.0f, 1.0f}};

  t3d_vec3_norm(&lightDirVec);

  model = t3d_model_load("rom:/snake3d/snake.t3dm");

}

/*==============================
    minigame_fixedloop
    Code that is called every loop, at a fixed delta time.
    Use this function for stuff where a fixed delta time is
    important, like physics.
    @param  The fixed delta time for this tick
==============================*/
void minigame_fixedloop(float deltatime) {
    timer -= deltatime;
    if(timer < 0) {
        minigame_end();
    }

}

/*==============================
    minigame_loop
    Code that is called every loop.
    @param  The delta time for this tick
==============================*/
void minigame_loop(float deltatime) {
  uint8_t colorAmbient[4] = {0xAA, 0xAA, 0xAA, 0xFF};
  uint8_t colorDir[4]     = {0xFF, 0xAA, 0xAA, 0xFF};
  rotAngle -= 0.02f;

    t3d_mat4_from_srt_euler(&modelMat,
      (float[3]){modelScale, modelScale, modelScale},
      (float[3]){0.0f, rotAngle*0.2f, rotAngle},
      (float[3]){0,0,0}
    );
    t3d_mat4_to_fixed(modelMatFP, &modelMat);


  t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(90.0f), 20.0f, 160.0f);
  t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

  rdpq_attach(display_get(), depthBuffer);

  t3d_frame_start();
  t3d_viewport_attach(&viewport);

  t3d_screen_clear_color(RGBA32(224, 180, 96, 0xFF));
  t3d_screen_clear_depth();

  t3d_light_set_ambient(colorAmbient);
  t3d_light_set_directional(0, colorDir, &lightDirVec);
  t3d_light_set_count(1);

  t3d_matrix_push(modelMatFP);
  t3d_model_draw(model);
  t3d_matrix_pop(1);

  rdpq_detach_show();

}

/*==============================
    minigame_cleanup
    Clean up any memory used by your game just before it ends.
==============================*/
void minigame_cleanup(){
    t3d_model_free(model);
    t3d_destroy();
    display_close();

}