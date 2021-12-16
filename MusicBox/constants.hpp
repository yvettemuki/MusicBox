#pragma once
// Constants for MusicBox

// Scene Type
const int TYPE_GEO = 1;

// Object Type
const int OBJ_SPHERE = 0;
const int OBJ_CUBE = 1;
const int OBJ_EDGE_SPHERE = 2;

// Texture Type
const int TEX_ROCK = 0;
const int TEX_GRASS = 1;
const int TEX_BRICKWALL = 2;
const int TEX_SAND = 3;
const int TEX_BRICK = 4;

// Audio Type
const int AUDIO_PASSION = 0;
const int AUDIO_BREAKOUT = 1;

// Obj File path
const string PATH_OBJ_SPHERE = "models/sphere.obj";
const string PATH_OBJ_CUBE = "models/cube.obj";
const string PATH_OBJ_EDGE_SPHERE = "models/edge_sphere.obj";

// Texture file path
const char* PATH_TEX_ROCK = "images/rock.jpg";
const char* PATH_TEX_GRASS = "images/grass.jpg";
const char* PATH_TEX_BRICKWALL = "images/brickwall.jpg";
const char* PATH_TEX_BRICKWALL_NORMAL = "images/brickwall_normal.jpg";
const char* PATH_TEX_SAND = "images/sand.jpg";
const char* PATH_TEX_BRICK_ALBEDO = "images/brick_albedo.jpg";
const char* PATH_TEX_BRICK_NORMAL = "images/brick_normal.jpg";
const char* PATH_TEX_BRICK_ROUGH = "images/brick_roughness.jpg";

// Audio file path
/*Passion by Sappheiros | https://soundcloud.com/sappheirosmusic */
const char* PATH_AUDIO_PASSION = "audios/passion.wav";
const char* PATH_AUDIO_BREAKOUT = "audios/breakout.wav";
vector<const char*> audios = {
    PATH_AUDIO_PASSION,
    PATH_AUDIO_BREAKOUT
};