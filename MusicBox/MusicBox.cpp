#include <iostream>
/* imgui */
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
/* tools */
#include "utils.hpp"
#include "stb_image.h"
#include "mesh.hpp"
#include "constants.hpp"
#include "Camera.hpp"
#include <random>
#include <cstdlib>
#include "AudioFile.h"
#include <irrklang/irrKlang.h>
#include "AudioWave.hpp"

/* opengl */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;
using namespace glm;
using namespace irrklang;

mt19937 rng;
uniform_real_distribution<float> noise;
// Random number generator for float
float random(float min, float max)
{
    random_device rd;
    rng = mt19937(rd());
    noise = uniform_real_distribution<float>(min, max);
    return noise(rng);
}

// Initialized functions
void initOpenGL();
void initGLFW();
void initState();
void initShaders();
void initTexture();
void initMesh();
void initFrameBufferObject();

// Util Functions
void configureSettings();
void getUniformLocationsInShader();
void initialCubeMapTexture(GLuint* texture, const char* texPath);
void initAudio();
void resetAudioConfig(int audioType);

// Callback functions
void display();
void idle();
void window_size(GLFWwindow* window, int width, int height);
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
void cleanup();

// Window Data
GLFWwindow* window;
GLuint width;
GLuint height;

// Vertex Buffer Data
GLuint vao;
GLuint vbuf;
GLint vcount;

// Audio Wave Data
AudioWave* audioWave;

// Audio
AudioFile<float> audioFile;
int frameSize = 512;
int sampleRate = 44100;
vector<int> audioSampleNums;        // Sample Number from all audio sources input
vector<float> audioTimes;           // Playing time from all audio sources input
vector<vector<float>> audioSamples; // Audio samples from all audio sources input
int numSamples = 0;                 // total samples for the current audio file
float audioTime;                    // Audio playing time for the current audio file
vector<float> samples;              // Audio samples for the current audio file
GLuint audioOffsetLoc = 0;          // uniform location
float timeSincAudioStart = 0;       // Time since audio start
float timeAudioStart = 0;           // Time point when audio start
ISoundEngine* soundEngine = createIrrKlangDevice();
static bool isPlay = false;         // control the audio to play
static bool isStop = false;
static bool isSwitch = false;
static int audioType = AUDIO_PASSION;
static const char* audioItems[] = { "Passion", "Breakout" };

// Shader Data
GLuint shader;
GLuint gpgpuShader;

// Framebuffer Object
GLuint reflectFrameBuffer;
GLuint reflectTexture;
vector<vec4> reflectTexData;

// Texture Data
GLuint uniSetTexture;
GLuint uniTextureType;
GLuint texture;
unsigned char* texData;
GLuint textureRock;
GLuint audioTexture;
vector<vec3> audioTexData;
GLuint textureGrass;
GLuint textureBrickwall;
GLuint textureBrickwallNorm;
GLuint textureSand;
GLuint textureFrabic;
GLuint textureFrabicNorm;
GLuint textureFrabicRough;

// Mesh Data
Mesh* sphereMesh;
Mesh* cubeMesh;
Mesh* edgeSphereMesh;
GLuint uniModel;

// Audio Wave Data
GLuint audioVao;
GLuint audioVbo;
GLuint audioIbo;
vector<int> indexs;
vector<vert> vertices;
GLuint audioVcount;
GLuint audioUniTime;
GLuint audioUniModel;
GLuint audioUniOffset;
GLuint audioWaveSize;

// Camera Data
GLuint uniView;
GLuint uniProj;
float camOrigin[3] = {0.f, 0.f, 3.f};
float lookAtPoint[3] = {0.f, 0.f, 0.f};
float fov = 45.f;

// Light Data
GLuint uniSetLight;
GLuint uniLightPos;
GLuint uniEyePos;
GLuint uniKa;
GLuint uniKd;
GLuint uniKs;
GLuint uniLa;
GLuint uniLd;
GLuint uniLs;
GLuint uniShininess;
GLuint uniAlpha;

// Other uniform variable location
GLuint uniCurrTime;
GLuint uniPrevTime;
GLuint uniDeltaTime;
GLuint uniPass;

// Time variables
float prevTime;
float currTime;
float deltaTime;

// Imgui Control Data
GLint objType;
GLint texType;
float rotX;
float rotY;
float scaleSize;
float waveScaleSize;
const char* objItems[] = { "Sphere", "Cube", "Edge Sphere"};
const char* texItems[] = { "Rock", "Grass", "Brick Wall", "Sand", "Frabic" };
static bool setLight;
static bool setTexture;
float lightPos[] = { -0.45f, 1.0f, 1.0f };
static float ka[3] = { 0.901, 0.439, 0.658 };
static float kd[3] = { 0.984, 0.843, 0.525 };
static float ks[3] = { 0.941, 0.819, 0.878 };
static float La = 0.5;
static float Ld = 0.5;
static float Ls = 0.2;
static float shininess = 5.f;
static float alpha = 1.f;


int main(int argc, char** argv)
{
    try {
        initState();
        initGLFW();
        initOpenGL();
        initShaders();
        initMesh();
        initTexture();
        initFrameBufferObject();
        initAudio();
    }
    catch (const exception& e) {
        // Handle any errors
        cerr << "Fatal error: " << e.what() << endl;
        cleanup();
        return -1;
    }

    //Init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        idle();
        display();
        glfwPollEvents();
    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void initState()
{
    width = 1600;
    height = 800;
    shader = 0;
    vao = 0;
    vbuf = 0;
    vcount = 0;
    window = NULL;

    // texture
    uniSetTexture = 0;
    texData = NULL;
    texture = 0;
    textureRock = 0;
    textureGrass = 0;
    textureBrickwallNorm = 0;
    textureBrickwall = 0;
    textureSand = 0;
    textureFrabic = 0;
    textureFrabicNorm = 0;
    textureFrabicRough = 0;

    // Mesh
    uniModel = 0;
    sphereMesh = NULL;
    cubeMesh = NULL;
    edgeSphereMesh = NULL;

    // Audio Wave
    audioWave = NULL;
    audioWaveSize = 32;

    // Light
    uniEyePos = 0;
    uniLightPos = 0;
    uniKa = 0;
    uniKd = 0;
    uniKs = 0;
    uniLa = 0;
    uniLs = 0;
    uniLd = 0;
    uniShininess = 0;
    uniAlpha = 0;

    // Imgui
    // Object
    objType = OBJ_SPHERE;
    texType = TEX_ROCK;
    rotX = 0.f;
    rotY = 0.f;
    scaleSize = 1.f;
    waveScaleSize = 2.6f;
    setTexture = false;

    // Light
    setLight = true;

    // Time 
    uniCurrTime = 0;
    uniDeltaTime = 0;
    currTime = 0.f;
    prevTime = 0.f;
    deltaTime = 0.f;

    // Force images to load vertically flipped
    // OpenGL expects pixel data to start at the lower-left corner
    stbi_set_flip_vertically_on_load(1);
}

void initGLFW()
{
    glfwInit();
    if (!glfwInit())
    {
        string err = "failed to init glfw!";
        throw(runtime_error(err));
    }

    window = glfwCreateWindow(width, height, "MUSIC BOX", NULL, NULL);
    if (!window)
    {
        string err = "failed to create window!";
        glfwTerminate();
        throw(runtime_error(err));
    }

    // Register callback functions with glfw.
    glfwSetKeyCallback(window, keyboard);
    glfwSetWindowSizeCallback(window, window_size);

    glfwMakeContextCurrent(window);

}

// uniform in "shader"
void getUniformLocationsInShader()
{
    // Get uniform position
    uniModel = glGetUniformLocation(shader, "M");
    uniCurrTime = glGetUniformLocation(shader, "current_time");
    uniPrevTime = glGetUniformLocation(shader, "prev_time");
    uniDeltaTime = glGetUniformLocation(shader, "delta_time");
    uniPass = glGetUniformLocation(shader, "pass");
    /* Texture Uniform */
    GLuint uniTex = glGetUniformLocation(shader, "tex");
    uniSetTexture = glGetUniformLocation(shader, "set_texture");
    uniTextureType = glGetUniformLocation(shader, "texture_type");
    /* Light Uniform */
    uniSetLight = glGetUniformLocation(shader, "set_light");
    uniLightPos = glGetUniformLocation(shader, "light_pos");
    uniEyePos = glGetUniformLocation(shader, "eye_pos");
    uniKa = glGetUniformLocation(shader, "ka");
    uniKd = glGetUniformLocation(shader, "kd");
    uniKs = glGetUniformLocation(shader, "ks");
    uniLa = glGetUniformLocation(shader, "La");
    uniLd = glGetUniformLocation(shader, "Ld");
    uniLs = glGetUniformLocation(shader, "Ls");
    uniShininess = glGetUniformLocation(shader, "shininess");
    uniAlpha = glGetUniformLocation(shader, "alpha");

}

// Uniform in "gpgpu shader"
void getUniformLocationsInGPGPUShader()
{
    audioUniModel = glGetUniformLocation(gpgpuShader, "M");
    audioUniTime = glGetUniformLocation(gpgpuShader, "time");
    audioUniOffset = glGetUniformLocation(gpgpuShader, "audio_offset");
}

void initOpenGL()
{
    glewInit();
    
    // Basic setup
    glClearColor(0.670, 0.729, 0.670, 1.0);
    glClearDepth(1.f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    assert(glGetError() == GL_NO_ERROR);
}

void initShaders()
{
    // "shader" program
    // Compile and link shader program
    vector<GLuint> shaders;
    shaders.push_back(compileShader(GL_VERTEX_SHADER, "shaders/sh_v_disp.glsl"));
    shaders.push_back(compileShader(GL_FRAGMENT_SHADER, "shaders/sh_f_disp.glsl"));
    shader = linkProgram(shaders);
    // Release shader sources
    for (auto s = shaders.begin(); s != shaders.end(); ++s)
        glDeleteShader(*s);
    shaders.clear();

    getUniformLocationsInShader();

    glUseProgram(0);
    assert(glGetError() == GL_NO_ERROR);

    // "gpgpu shader" program
    vector<GLuint> gpgpuShaders;
    gpgpuShaders.push_back(compileShader(GL_VERTEX_SHADER, "shaders/sh_v_gpgpu.glsl"));
    gpgpuShaders.push_back(compileShader(GL_FRAGMENT_SHADER, "shaders/sh_f_gpgpu.glsl"));
    gpgpuShader = linkProgram(gpgpuShaders);
    // Release shader sources
    for (auto s = gpgpuShaders.begin(); s != gpgpuShaders.end(); ++s)
        glDeleteShader(*s);
    gpgpuShaders.clear();

    getUniformLocationsInGPGPUShader();

    glUseProgram(0);
    assert(glGetError() == GL_NO_ERROR);
}

void initMesh()
{
    cout << "Initializing mesh data, please wait..." << endl;
    // init geometry meshes
    if (!sphereMesh) sphereMesh = new Mesh(PATH_OBJ_SPHERE, TYPE_GEO);
    if (!cubeMesh) cubeMesh = new Mesh(PATH_OBJ_CUBE, TYPE_GEO);
    if (!edgeSphereMesh) edgeSphereMesh = new Mesh(PATH_OBJ_EDGE_SPHERE, TYPE_GEO);

    // init audio wave meshes
    if (!audioWave) audioWave = new AudioWave(gpgpuShader, audioWaveSize, audioWaveSize);
}

void initialCubeMapTexture(GLuint* texture, const char* texPath)
{
    int texWidth, texHeight, n_ch;

    // Create a cube map 
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *texture);
    for (int i = 0; i < 6; i++)
    {
        unsigned char* texData0 = stbi_load(texPath, &texWidth, &texHeight, &n_ch, 4);
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData0
        );
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    assert(glGetError() == GL_NO_ERROR);
}

void initTexture() 
{
    // Rock Texture
    initialCubeMapTexture(&textureRock, PATH_TEX_ROCK);

    // Grass Texture
    initialCubeMapTexture(&textureGrass, PATH_TEX_GRASS);

    // Brick Texture
    initialCubeMapTexture(&textureBrickwall, PATH_TEX_BRICKWALL);
    initialCubeMapTexture(&textureBrickwallNorm, PATH_TEX_BRICKWALL_NORMAL);

    // Sand Texture
    initialCubeMapTexture(&textureSand, PATH_TEX_SAND);

    // Brick Texture
    initialCubeMapTexture(&textureFrabic, PATH_TEX_BRICK_ALBEDO);
    initialCubeMapTexture(&textureFrabicNorm, PATH_TEX_BRICK_NORMAL);
    initialCubeMapTexture(&textureFrabicRough, PATH_TEX_BRICK_ROUGH);

}

void initFrameBufferObject()
{
    glGenFramebuffers(1, &reflectFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectFrameBuffer);
    glGenTextures(1, &reflectTexture);
    glBindTexture(GL_TEXTURE_2D, reflectTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, reflectTexData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "Error:: FrameBuffer is not complete!" << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initAudio()
{
    // Read all the .wav files from the storage
    cout << "Loading the audio data, please wait..." << endl;

    for (int i = 0; i < audios.size(); i++)
    {
        audioFile.load(audios[i]);
        int sampleNum = audioFile.getNumSamplesPerChannel();
        float audioTime = (float)sampleNum / (float)sampleRate;
        auto samples = audioFile.samples[1];  // Get right channel
        audioSampleNums.push_back(sampleNum);
        audioTimes.push_back(audioTime);
        audioSamples.push_back(samples);
        cout << "--- audio " << i << " ---" << endl;
        cout << "Audio Sample Number: " << sampleNum << endl;
        cout << "Audio Time: " << audioTime << endl;
    }

    // Get uniform location
    audioOffsetLoc = glGetUniformLocation(shader, "audio_offset");

}

void resetAudioConfig(int audioType)
{
    numSamples = audioSampleNums[audioType];
    audioTime = audioTimes[audioType];
    samples = audioSamples[audioType];

    switch (audioType)
    {
    case AUDIO_PASSION:
        soundEngine->play2D(PATH_AUDIO_PASSION, false);
        break;
    case AUDIO_BREAKOUT:
        soundEngine->play2D(PATH_AUDIO_BREAKOUT, false);
        break;

    default:
        break;
    }

    timeAudioStart = glfwGetTime();
    timeSincAudioStart = 0.f;
    
}

void draw_gui(GLFWwindow* window)
{
    // Begin imgui frame.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Object control window
    ImGui::Begin("Object Control Panel");
        ImGui::SetWindowFontScale(1.2f);

        // Object Combo
        ImGui::Combo("Object", &objType, objItems, IM_ARRAYSIZE(objItems));
        // Textures
        ImGui::Checkbox("set texture", &setTexture);
        ImGui::Combo("Texture", &texType, texItems, IM_ARRAYSIZE(texItems));
        
        // Object Attributes 
        ImGui::Text("Geometry");
        ImGui::SliderFloat("scale", &scaleSize, 0.1, 3.f);
        ImGui::SliderFloat("rotate X", &rotX, 0.f, 180.f);
        ImGui::SliderFloat("rotate Y", &rotY, 0.f, 180.f);

        // Wave Attributes
        ImGui::Text("Wave");
        ImGui::SliderFloat("wave scale", &waveScaleSize, 0.1, 3.f);


    ImGui::End();

    // Scene control window
    ImGui::Begin("Scene Control Panel");
        ImGui::SetWindowFontScale(1.2f);
        if (ImGui::CollapsingHeader("Camera"))
        {
            ImGui::SliderFloat3("Origin", camOrigin, -3.f, 3.f);
            //ImGui::SliderFloat3("Look Direction", lookAtPoint, -1.f, 3.f);
            ImGui::SliderFloat("Field of View", &fov, 10.f, 100.f);
        }
        
        ImGui::Checkbox("Shading and Lighting", &setLight);
        if (ImGui::CollapsingHeader("Light") && setLight)
        {
            ImGui::Text("Phong Lighting");
            ImGui::Text("Lighting Color");
            ImGui::SliderFloat3("Light Position", lightPos, -1.f, 1.f);
            ImGui::ColorEdit3("Ka", ka);
            ImGui::ColorEdit3("Kd", kd);
            ImGui::ColorEdit3("Ks", ks);
            ImGui::Text("Lighting Density");
            ImGui::SliderFloat("La", &La, 0.f, 1.f);
            ImGui::SliderFloat("Ld", &Ld, 0.f, 1.f);
            ImGui::SliderFloat("Ls", &Ls, 0.f, 1.f);
            ImGui::Text("Others");
            ImGui::SliderFloat("Shininess", &shininess, 0.f, 20.f);
        }
        //ImGui::SliderFloat("Alpha", &alpha, 0.f, 1.f);
        
    ImGui::End();

    ImGui::Begin("Audio Control Panel");
    ImGui::SetWindowFontScale(1.2f);
        if (ImGui::Combo("Audio Type", &audioType, audioItems, IM_ARRAYSIZE(audioItems)))
        {
            isSwitch = true;
        }
        if (ImGui::Button("    Play    "))
        {
            isPlay = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("    Stop    "))
        {
            isStop = true; // not true, should stop the sound imediately
        }
            
        //ImGui::Button("Restart");
    ImGui::End();

    /*static bool show_demo = true;
    ImGui::ShowDemoWindow(&show_demo);*/
    /*if (ImGui::Button("Quit"))
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }*/

    // End imgui frame.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Set uniform for "shader" when pass == 1
void configureSettings()
{
    // tranfer lighting attributes
    vec3 eyePos = vec3(camOrigin[0], camOrigin[1], camOrigin[2]);
    glUniform3fv(uniEyePos, 1, value_ptr(eyePos));
    vec3 lightPosition = vec3(lightPos[0], lightPos[1], lightPos[2]);
    glUniform3fv(uniLightPos, 1, value_ptr(lightPosition));

    // set texture
    if (setTexture)
    {
        // cube map texture
        switch (texType)
        {
        case TEX_ROCK:
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureRock);
            break;
        case TEX_GRASS:
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureGrass);
            break;
        case TEX_BRICKWALL:
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureBrickwall);
            glActiveTexture(GL_TEXTURE0 + 3);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureBrickwallNorm);
            break;
        case TEX_SAND:
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureSand);
            break;
        case TEX_BRICK:
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureFrabic);
            glActiveTexture(GL_TEXTURE0 + 3);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureFrabicNorm);
            glActiveTexture(GL_TEXTURE0 + 4);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureFrabicRough);
            break;

        default:
            break;
        }
    }
    glUniform1i(uniSetTexture, setTexture);
    glUniform1i(uniTextureType, texType);

    // other setting
    glUniform1i(uniSetLight, setLight);
    glUniform3fv(uniKa, 1, value_ptr(vec3(ka[0], ka[1], ka[2])));
    glUniform3fv(uniKd, 1, value_ptr(vec3(kd[0], kd[1], kd[2])));
    glUniform3fv(uniKs, 1, value_ptr(vec3(ks[0], ks[1], ks[2])));
    glUniform3fv(uniLa, 1, value_ptr(vec3(La)));
    glUniform3fv(uniLd, 1, value_ptr(vec3(Ld)));
    glUniform3fv(uniLs, 1, value_ptr(vec3(Ls)));
    glUniform1f(uniShininess, shininess);
    glUniform1f(uniAlpha, alpha);
    
}

void display()
{
    // -------------- object drawing -----------------
    // --------------     pass 0     -----------------
    glBindFramebuffer(GL_FRAMEBUFFER, reflectFrameBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    // Create reflective camera view matrix (reverse the y-axis: y1 = -1 - y0 | fix the aspect)
    Camera camera0(
        shader,
        fov,
        800,
        800,  
        vec3(camOrigin[0], -1-camOrigin[1], camOrigin[2]),
        vec3(lookAtPoint[0], lookAtPoint[1], lookAtPoint[2])
    );

    configureSettings();
    glUniform1i(uniPass, 0);
    // Set the texture size
    glViewport(0, 0, 800, 800);

    // Create model matrix
    mat4 rot = rotate(mat4(1.0f), rotX, vec3(1.0, 0.0, 0.0));
    rot = rotate(rot, rotY, vec3(0.0, 1.0, 0.0));
    mat4 scal = scale(mat4(1.f), vec3(scaleSize));
    mat4 model = scal * rot;

    // Draw selected object
    switch (objType) {
    case OBJ_SPHERE:
        // TODO: create local fixing scale if need.
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(model * sphereMesh->fixBB()));
        sphereMesh->draw();
        break;
    case OBJ_CUBE:
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(model * cubeMesh->fixBB()));
        cubeMesh->draw();
        break;
    case OBJ_EDGE_SPHERE:
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(model * edgeSphereMesh->fixBB()));
        edgeSphereMesh->draw();
        break;
    default:
        break;
    }
    
    assert(glGetError() == GL_NO_ERROR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindTexture(GL_TEXTURE_1D, 0);
    glUseProgram(0);
   
    // Bind Back to default frame buffer obejct
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);

    // -------------- audio wave drawing ----------------
    glUseProgram(gpgpuShader);
    Camera camera1(
        gpgpuShader,
        fov,
        width,
        height,
        vec3(camOrigin[0], camOrigin[1], camOrigin[2]),
        vec3(lookAtPoint[0], lookAtPoint[1], lookAtPoint[2])
    );
    // Set model matrix
    mat4 waveScal0 = scale(mat4(1.f), vec3(1.f / (float)audioWaveSize));
    mat4 waveScal = scale(waveScal0, vec3(waveScaleSize));
    mat4 waveRot = rotate(mat4(1.f), -90.f, vec3(1.f, 0.f, 0.f));
    mat4 waveTrans = translate(mat4(1.f), vec3(0.f, -0.5f, 0.f));
    mat4 waveModel = waveTrans * waveScal * waveRot;
    glUniformMatrix4fv(audioUniModel, 1, GL_FALSE, value_ptr(waveModel));

    // Set reflection texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, reflectTexture);

    // Draw wave
    audioWave->draw();

    assert(glGetError() == GL_NO_ERROR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    
    // --------------     pass 1     -----------------
    // -------------- object drawing -----------------
    // Load objects in the world
    glUseProgram(shader);

    // Set up Camera (problem, will create Camera object each loop)
    Camera camera(
        shader,
        fov,
        width,
        height,
        vec3(camOrigin[0], camOrigin[1], camOrigin[2]),
        vec3(lookAtPoint[0], lookAtPoint[1], lookAtPoint[2])
    );

    // Uniform setting variables
    configureSettings();
    glUniform1i(uniPass, 1);

    // Create model matrix
    rot = rotate(mat4(1.0f), rotX, vec3(1.0, 0.0, 0.0));
    rot = rotate(rot, rotY, vec3(0.0, 1.0, 0.0));
    scal = scale(mat4(1.f), vec3(scaleSize));
    model = scal * rot;

    // Draw selected object
    switch (objType) {
    case OBJ_SPHERE:
        // TODO: create local fixing scale if need.
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(model * sphereMesh->fixBB()));
        sphereMesh->draw();
        break;
    case OBJ_CUBE:
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(model * cubeMesh->fixBB()));
        cubeMesh->draw();
        break;
    case OBJ_EDGE_SPHERE: 
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(model * edgeSphereMesh->fixBB()));
        edgeSphereMesh->draw();
        break;
    default:
        break;
    }

    assert(glGetError() == GL_NO_ERROR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_1D, 0);
    glUseProgram(0);

    // Draw ImGui
    draw_gui(window);

    // Audio Control (Switch, Stop, Play)
    if (isSwitch)
    {
        soundEngine->stopAllSounds();
        timeSincAudioStart = audioTime;
        resetAudioConfig(audioType);
        isSwitch = false;
    }
    if (isPlay)
    {
        soundEngine->stopAllSounds();  // may cause bugs, need to using isPlaying to control
        resetAudioConfig(audioType);
        isPlay = false;
    }
    if (isStop)
    {
        soundEngine->stopAllSounds();
        timeSincAudioStart = audioTime;
        
        isStop = false;
    }

    glfwSwapBuffers(window);
}

void idle()
{
    // Calculate time variables
    currTime = static_cast<float>(glfwGetTime());
    deltaTime = currTime - prevTime;
    prevTime = currTime;

    // Send time to "shader"
    glUseProgram(shader);
    glUniform1f(uniCurrTime, currTime);
    glUniform1f(uniPrevTime, prevTime);
    glUniform1f(uniDeltaTime, deltaTime);
    glUseProgram(0);

    // Send time to "gpgpu shader"
    glUseProgram(gpgpuShader);
    glUniform1f(audioUniTime, currTime);
    glUseProgram(0);
    
    // Update the geometry by getting the audio buffer data (when isPlay)
    if (timeSincAudioStart < audioTime)
    {
        timeSincAudioStart = glfwGetTime() - timeAudioStart;
        int index = floor(timeSincAudioStart * sampleRate);
        //cout << timeSincAudioStart << endl;
        if (index < numSamples - 1)
        {
            // Set to "shader"
            glUseProgram(shader);
            glUniform1f(audioOffsetLoc, samples[index]); // right channel
            // Set to "gpgpu shader"
            glUseProgram(gpgpuShader);
            glUniform1f(audioUniOffset, samples[index]);
            glUseProgram(0);
        }
    }
    else
    {
        // Set to "shader"
        glUseProgram(shader);
        glUniform1f(audioOffsetLoc, 0.f);
        // Set to "gpgpu shader"
        glUseProgram(gpgpuShader);
        glUniform1f(audioUniOffset, 0.f);
        glUseProgram(0);
    }

}

void window_size(GLFWwindow* window, int width, int height)
{
    ::width = width;
    ::height = height;
    glViewport(0, 0, width, height);
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        }
    }
}

void cleanup()
{
    // Release all resources
    // deleteTextures();
    // deleteShaders();
    // deleteBuffers();
}



