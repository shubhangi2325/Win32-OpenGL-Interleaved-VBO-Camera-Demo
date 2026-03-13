// standred header files
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

// OpenGL related Header files
#include <GL/glew.h> //this header file must be included before gl/GL.h
#include <gl/GL.h>   //C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\um\gl path to GL.h file

// Custom files
#include "OGL.h"
#include "vmath.h"
using namespace vmath;

// OpenGL Related Libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib") // C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0\um\x64 OpenGL32.lib

// window sizes
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variables declarations
BOOL gbFullScreen = FALSE;
HWND ghwnd = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev;

// active window variables
BOOL gbActiveWindow = FALSE;

// Exit keypress related
BOOL gbEscapeKeyIsPressed = FALSE;

// variables related File I/O
char gszLogFileName[] = "Log.txt";
FILE *gpFile = NULL;

// opengl related global variables
HDC ghdc = NULL;
HGLRC ghrc = NULL; //(handle to graphics lib rendering context)

// shader related global variables
GLuint shaderProgramObject = 0;

enum
{
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_NORMAL,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_TEXCOORD
};

GLuint vao_Cube = 0;
GLuint vbo_Cube = 0;

GLuint modelMatrixUniform = 0; // mv- model view matrix
GLuint viewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0; // p -  projection matrix

//variables related to light
GLuint lAUniform = 0;
GLuint ldUniform = 0;
GLuint lSUniform = 0;
GLuint lightPositionUniform = 0;

GLuint kAUniform = 0;
GLuint kdUniform = 0;
GLuint kSUniform = 0;
GLuint materialShininessUniform = 0;

GLfloat lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightPosition[] = {100.0f, 100.0f, 100.0f, 1.0f};

GLfloat materialAmbient[] = {0.25f, 0.25f, 0.25f, 1.0f};
GLfloat materialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialShininess = 128.0f;

GLuint LKeyPressUniform = 0;
BOOL bLight = FALSE;

mat4 PerspectiveProjectionMatrix; 

GLfloat winWidth;
GLfloat winHeigth;

//variables related to texture
GLuint texture_Cube = 0;
GLuint textureSamplerUniform = 0;

// Rotation Angles
GLfloat angleCube = 0.0f; 

//camera related variables
vec3 cameraPos;
vec3 cameraFront;
vec3 cameraUp;

float cameraSpeed = 0.05f;

float lastX = 400.0f;
float lastY = 300.0f;
BOOL firstMouse = TRUE;   //mouse movement → control yaw & pitch
BOOL startMouseMove =  FALSE;

float yaw = -90.0f;   //left/right rotation  -90 Because OpenGL’s default forward direction is −Z.
float pitch = 0.0f;   //up/down rotation

float sensitivity = 0.1f;  //Mouse movement can be very large. Sensitivity reduces it so the camera moves smoothly.

float fov = 45.0f; 

// Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
    // Local Function Declarations
    int initialise(void);
    void display(void);
    void update(void);
    void uninitialise(void);

    // Variable declarations
    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;
    TCHAR szAppName[] = TEXT("RTR6");
    BOOL bDone = FALSE;

    // variables related window centering
    int iX_Position;
    int iY_Position;

    // code

    // Create LogFile
    gpFile = fopen(gszLogFileName, "w");
    if (gpFile == NULL)
    {
        MessageBox(NULL,
                   TEXT("LogFile Creation Failed"),
                   TEXT("File I/O Error"),
                   MB_OK);
        exit(0);
    }
    else
    {
        fprintf(gpFile, "Program Started Successfully...\n");
    }

    // window class initialization
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = NULL;
    wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

    // Registraton of Window Class
    if (!RegisterClassEx(&wndclass))
    {
        MessageBox(NULL,
                   TEXT("Window Class Registeration Failed!!!"),
                   TEXT("RegisterClassEx"),
                   MB_ICONERROR);
        return (0);
    }

    // window position x-coordinate and y-coordinate
    iX_Position = (GetSystemMetrics(SM_CXFULLSCREEN)) / 2 - (WIN_WIDTH / 2);
    iY_Position = (GetSystemMetrics(SM_CYFULLSCREEN)) / 2 - (WIN_HEIGHT / 2);

    // create window
    hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        szAppName,
        TEXT("Shubhangi R Bhadrashette"),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
        iX_Position,
        iY_Position,
        WIN_WIDTH,
        WIN_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL);

    ghwnd = hwnd;

    // Show window
    ShowWindow(hwnd, iCmdShow);

    // Paint the background of window
    UpdateWindow(hwnd);

    // initialise
    int result = initialise();
    if (result != 0)
    {
        fprintf(gpFile, "initialise failed....!!!\n");
        DestroyWindow(hwnd);
        hwnd = NULL;
    }
    else
    {
        fprintf(gpFile, "initialise Fun Completed Successfully...\n");
    }

    // set this as foreground and active window
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // game loop
    while (bDone == FALSE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                bDone = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            if (gbActiveWindow == TRUE)
            {
                if (gbEscapeKeyIsPressed == TRUE)
                {
                    bDone = TRUE;
                }
                // render
                display();

                // update
                update();
            }
        }
    }

    uninitialise();
    return ((int)(msg.wParam));
}

void ProcessMouseMovement(float xpos, float ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;

    if(pitch < -89.0f)
        pitch = -89.0f;

    vmath::vec3 direction;

    direction[0] = cos(radians(yaw)) * cos(radians(pitch));
    direction[1]= sin(radians(pitch));
    direction[2] = sin(radians(yaw)) * cos(radians(pitch));

    cameraFront = normalize(direction);
}

// Callback function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    // local function declarations
    void toggleFullScreen(void);
    void resize(int, int);
    void uninitialise(void);

    float xpos;
    float ypos;

    // code
    switch (iMsg)
    {
    case WM_CREATE:
        ZeroMemory((void *)&wpPrev, sizeof(WINDOWPLACEMENT));
        wpPrev.length = sizeof(WINDOWPLACEMENT);
        break;

    case WM_SETFOCUS:
        gbActiveWindow = TRUE;
        break;

    case WM_KILLFOCUS:
        gbActiveWindow = FALSE;
        break;

    case WM_ERASEBKGND:
        return (0);

    case WM_SIZE:
        winWidth = LOWORD(lParam);
        winHeigth =  HIWORD(lParam);
        resize(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            gbEscapeKeyIsPressed = TRUE;
            break;

        default:
            break;
        }
        break;

    case WM_MOUSEMOVE:
        if(startMouseMove == TRUE)
        {
            xpos = LOWORD(lParam);
            ypos = HIWORD(lParam);
            ProcessMouseMovement(xpos, ypos);
        }
        break; 
        
    case WM_MOUSEWHEEL:
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if(delta > 0)
                fov -= 1.0f;
            else
                fov += 1.0f;
        }
        break;    

    case WM_CHAR:
        switch (wParam)
        {
        case 'F':
        case 'f':
            if (gbFullScreen == FALSE)
            {
                toggleFullScreen();
                gbFullScreen = TRUE;
            }
            else
            {
                toggleFullScreen();
                gbFullScreen = FALSE;
            }
            break;

        case 'L':
        case 'l':
            if (bLight == FALSE)
            {
                bLight = TRUE;
            }
            else
            {
                bLight = FALSE;
            }
            break;

        case 'C':
        case 'c':
            if (startMouseMove == FALSE)
            {
                startMouseMove = TRUE;
            }
            else
            {
                startMouseMove = FALSE;
            }
            break;    

        case 'W':
        case 'w':
            cameraPos = cameraPos + (cameraSpeed * cameraFront);
            break; 
            
        case 'S':
        case 's':
            cameraPos = cameraPos - (cameraSpeed * cameraFront);
            break;     

        case 'D':
        case 'd':
            cameraPos = cameraPos + normalize(cross(cameraFront, cameraUp)) * cameraSpeed;   //cross product give 3rd vector that is perpendicular to both vectors i.e right direwction 
            break; 
            
        case 'A':
        case 'a':
            cameraPos = cameraPos - normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
            break;     
    
        default:
            break;
        }
        break;

    case WM_CLOSE:
        uninitialise();
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        break;
    }

    return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void toggleFullScreen(void)
{
    // variable declarations
    MONITORINFO mi;

    // code
    if (gbFullScreen == FALSE)
    {
        dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
        if (dwStyle & WS_OVERLAPPEDWINDOW)
        {
            ZeroMemory((void *)&mi, sizeof(MONITORINFO));
            mi.cbSize = sizeof(MONITORINFO);

            if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
            {
                SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        ShowCursor(FALSE);
    }
    else
    {
        SetWindowPlacement(ghwnd, &wpPrev);
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }
}

int initialise(void)
{
    // function declarations
    void printGLInfo(void);
    void resize(int width, int heigth);
    void uninitialise(void);
    BOOL LoadGLTexture(GLuint * , TCHAR []);

    // variables declarations
    PIXELFORMATDESCRIPTOR pfd; // every os has structure for giving info about pixel to os
    int iPixelFormatIndex = 0;
    GLenum glewResult;

    // code

    // pixelformatdescriptor initialization
    ZeroMemory((void *)&pfd, sizeof(PIXELFORMATDESCRIPTOR)); // own function of window to empty struct (set bytes to zero)

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // size of struture (nSize = n id prefix of short int)
    pfd.nVersion = 1;                          // ffp shiktana os ne support kelyalya opengl sobat karavi lagte tyamule pfd la opengl ffp cha recommended version dyav lagal that is 1
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    // mala window varti draw karaich aahe |
    // render mode api use karaich aahe  not retained mode (gdi graphic nahi karaich ) he sagat |
    // single buffering ekach buffer madhe tom annd bachhan dakhvaich
    // double buffering = front buffer samor aahe te and je swap karaich te  cccccxcxxcxback buffer
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cBlueBits = 8;
    pfd.cGreenBits = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;

    // GetDc
    ghdc = GetDC(ghwnd);
    if (ghdc == NULL)
    {
        fprintf(gpFile, " ERROR :GetDC() function failed\n");
        return (-1);
    }

    // Get Matching pixel format index using hdc and pfd
    iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
    if (iPixelFormatIndex == 0)
    {
        fprintf(gpFile, " ERROR : ChoosePixelFormat(): failed\n");
        return (-2);
    }

    // select the pixel formats of found index
    if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
    {
        fprintf(gpFile, "ERROR: SetPixelFormat() Function Failed..!!");
        return (-3);
    }

    // Create Rendering Context Using hdc pfd And PixelFormatRendering
    ghrc = wglCreateContext(ghdc); // bridging api now a days it is called as WSI (window system integration)
    if (ghrc == NULL)
    {
        fprintf(gpFile, "ERROR: wglCreateContext() Function Failed !!");
        return (-4);
    }

    // make this rendering context as current context
    if (wglMakeCurrent(ghdc, ghrc) == FALSE) // aata current context ha rendering context set zala
    {
        fprintf(gpFile, "ERROR: wglMakeCurrent() Function Failed !!");
        return (-5);
    }

    // initialise glew
    glewResult = glewInit();
    if (glewResult != GLEW_OK)
    {
        fprintf(gpFile, "ERROR: glewInit() Function Failed !!");
        return (-6);
    }

    // print gl info
    printGLInfo();

    // From here onword OpenGL Code Start

    //------------------------------------------------------------------------------------------------------------------------------------
    // Vertex Shader = compulsary shaders

    // 1) write the shader source code ->
    const GLchar *vertexShaderSourceCode =
        "#version 460 core\n" // OpenGL Version: 4.6 * 100 = 460
        "in vec4 aColor;\n"
        "out vec4 out_color;\n"
         "in vec2 aTexcoord;\n"
        "out vec2 out_Texcoord;\n"
        "in vec4 aPosition;\n" // 4 member aasnara (4ch array aasnara) konitari shader chya aata yet aahe tyatch nav aapn aposition dil aahe //glsl chya compiler la kalel he c and c++ compiler la nahi kalnar
        "in vec3 aNormal;\n"
        "uniform mat4 uMMatrix;\n"
        "uniform mat4 uVMatrix;\n"
        "uniform mat4 uPMatrix;\n"
        "uniform vec4 uLightPosition;\n"
        "uniform int uLKeyIsPressed;\n"
        "out vec3 out_transformedNormals;\n"
        "out vec3 out_lightDirection;\n"
        "out vec3 out_viwerVector;\n"
        "void main(void)\n"
        "{\n"
              "gl_Position =   uPMatrix * uVMatrix * uMMatrix * aPosition;\n" // order is important
                "if(uLKeyIsPressed == 1)\n"
                "{\n"
                "vec4 EyeCoordinates = uVMatrix * uMMatrix * aPosition;\n"
                "mat3 normalMatrix = mat3(transpose(inverse(uVMatrix * uMMatrix)));\n"
                "out_transformedNormals = normalMatrix * aNormal;\n"
                "out_lightDirection = vec3(uVMatrix * uLightPosition - EyeCoordinates);\n" // phong ne light_Direction ya name ne rename kel aahe lightSource var la
                "out_viwerVector = -EyeCoordinates.xyz;\n"                                 // <- this process is called Swizzling "vec3 viwerVector = normalize(vec3(EyeCoordinates));\n" one and the same
                "}\n"
            "out_color = aColor;\n"
            "out_Texcoord = aTexcoord;\n"
        "}\n";

    // 2) create the shader object ->
    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER); // he function shader object tayar karat

    // 3) give the shader source code to the shader object ->
    glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
    // 1st which shader object 2nd how many shaders are giving 3rd array of shaders 4th array of lengths of every shader

    // 4) compile the shader->
    glCompileShader(vertexShaderObject);

    // 5) do shader compilation error checking
    GLint status = 0;         // success zal ka nahi compilation he store karnyasathi
    GLint infoLogLength = 0;  // log info denari string chi length
    GLchar *szInfoLog = NULL; // error string store karnyasathi

    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) // error aasel tr aat ya
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0) // there are some error which does not have length which does not have string but still thay are logical error it must be solved by mannually
        {
            szInfoLog = (GLchar *)malloc(infoLogLength * sizeof(GLchar));
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog); // 3rd para actual length of error string det
                fprintf(gpFile, "Vertex Shader Compilation Log = %s\n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialise();
    }

    //------------------------------------------------------------------------------------------------------------------------------------
    // Fragment shader = compulsary shaders

    // 1) write the shader source code ->
    const GLchar *fragmentShaderSourceCode =
        "#version 460 core\n"
        "in vec4 out_color;\n"
        "out vec4 FragColor;\n" 
        "in vec2 out_Texcoord;\n"
        "uniform sampler2D uTextureSampler;\n"
        "in vec3 out_transformedNormals;\n"
        "in vec3 out_lightDirection;\n"
        "in vec3 out_viwerVector;\n"
        "uniform vec3 uLa;\n"
        "uniform vec3 uLd;\n"
        "uniform vec3 uLs;\n"
        "uniform vec3 uKa;\n"
        "uniform vec3 uKd;\n"
        "uniform vec3 uKs;\n"
        "uniform float uMaterialShininess;\n"
        "uniform int uLKeyIsPressed;\n"
        "void main(void)\n"
        "{\n" 
           "vec3 phong_ADS_Light;\n"
            "if(uLKeyIsPressed == 1)\n"
            "{\n"
                "vec3 normalisedTransformedNormals = normalize(out_transformedNormals);\n"
                "vec3 normalisedLightDirection = normalize(out_lightDirection);\n"
                "vec3 normalisedViewrVector = normalize(out_viwerVector);\n"
                "vec3 ambientLight = uLa * uKa;\n"
                "vec3 diffuseLight = uLd * uKd * max(dot(normalisedLightDirection , normalisedTransformedNormals), 0.0f);\n"
                "vec3 reflectionVector = reflect(-normalisedLightDirection, normalisedTransformedNormals);\n"
                "vec3 specularLight = uLs * uKs * pow(max(dot(reflectionVector, normalisedViewrVector), 0.0f), uMaterialShininess);\n"
                "phong_ADS_Light = ambientLight + diffuseLight + specularLight;\n"
            "}\n"
            "else\n"
            "{\n"
                "phong_ADS_Light = vec3(1.0f, 1.0f, 1.0f);\n"
            "}\n"
                "FragColor = out_color * (texture(uTextureSampler, out_Texcoord)) * vec4(phong_ADS_Light, 1.0f);\n" 
        "}\n";

    // 2) create the shader object ->
    GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // 3) give the shader source code to the shader object ->
    glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

    // 4) compile the shader->
    glCompileShader(fragmentShaderObject);

    // 5) do shader compilation error checking
    status = 0;        // success zal ka nahi compilation he store karnyasathi
    infoLogLength = 0; // log info denari string chi length
    szInfoLog = NULL;

    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLogLength * sizeof(GLchar));
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog);
                fprintf(gpFile, "Fragment Shader Compilation Log = %s\n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialise();
    }

    // 6) create shader program object
    shaderProgramObject = glCreateProgram();

    // 7) attach shader object to shader program object
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);

    // bind shader attribute at a certain index in shader to same index in host program
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");  
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_COLOR, "aColor"); 
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_NORMAL, "aNormal"); 
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXCOORD, "aTexcoord"); 

    // 8) tell to link shader objects to shader program objects
    glLinkProgram(shaderProgramObject);

    // 9) check for link error logs
    status = 0;        // success zal ka nahi compilation he store karnyasathi
    infoLogLength = 0; // log info denari string chi length
    szInfoLog = NULL;

    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLogLength * sizeof(GLchar));
            if (szInfoLog != NULL)
            {
                glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog);
                fprintf(gpFile, "Shader Program Link  Log = %s\n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialise();
    }
    //---------------------------------------------------------------------------------------------------------------------------
   
    // get the required uniform location from the shader    
    modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMMatrix");
    viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "uVMatrix");
    projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "uPMatrix");

    lAUniform = glGetUniformLocation(shaderProgramObject, "uLa");
    ldUniform = glGetUniformLocation(shaderProgramObject, "uLd");
    lSUniform = glGetUniformLocation(shaderProgramObject, "uLs");
    lightPositionUniform = glGetUniformLocation(shaderProgramObject, "uLightPosition");

    kAUniform = glGetUniformLocation(shaderProgramObject, "uKa");
    kdUniform = glGetUniformLocation(shaderProgramObject, "uKd");
    kSUniform = glGetUniformLocation(shaderProgramObject, "uKs");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject, "uMaterialShininess");

    LKeyPressUniform = glGetUniformLocation(shaderProgramObject, "uLKeyIsPressed");

    textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "uTextureSampler");

    GLfloat cube_PCNT[] =
    {
        // front
        // position				// color			 // normals				// texcoords
        1.0f,  1.0f,  1.0f,	    1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
        1.0f, -1.0f,  1.0f,	    1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
                            
        // right			 
        // position				// color			 // normals				// texcoords
        1.0f,  1.0f, -1.0f, 	0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
        1.0f,  1.0f,  1.0f,	    0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
        1.0f, -1.0f,  1.0f,	    0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,	    0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
                            
        // back				 
        // position				// color			 // normals				// texcoords
        1.0f,  1.0f, -1.0f,	    1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,	    1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
                            
        // left				 
        // position				// color			 // normals				// texcoords
        -1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
                            
        // top				 
        // position				// color			 // normals				// texcoords
        1.0f,  1.0f, -1.0f,	    0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
        1.0f,  1.0f,  1.0f,	    0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
                            
        // bottom			 
        // position				// color			 // normals				// texcoords
        1.0f, -1.0f,  1.0f,	    1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,	    1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
    };

    //-------------------------------Rectangle--------------------------------
      // vertex array object for arrays of vertex attributes
        glGenVertexArrays(1, &vao_Cube); // vao - vertex array object
        glBindVertexArray(vao_Cube);     //vao bind

            //create a common vbo for P, C, N, T
            glGenBuffers(1, &vbo_Cube);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_Cube);

            //Android size (24 * 11 * 4)
            glBufferData(GL_ARRAY_BUFFER, sizeof(cube_PCNT), cube_PCNT, GL_STATIC_DRAW );

            //for position
            glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0 * sizeof(float)));
            glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

            //for color
            glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

            //for Normal
            glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

            //for Texcoord
            glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
            glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);                     //5th para kitichi udu maru 6th kutun udi mari

            glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Vao unbind
        glBindVertexArray(0);
    //------------------------------------------------------------------------------------

    // Depth Related Function Call
    glClearDepth(1.0f); // display madhe gelyavr mazya depth buffer la 1 kr
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // LEQUAL = Less Than Or Equal , Aashya Fragment la pass kr jyachi value less than or equal to 1 aasel

    // Tell OpenGL to choose the color to clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //initialise camera variables
    cameraPos = vec3(0.0f, 0.0f, 3.0f);     //Camera position
    cameraFront = vec3(0.0f, 0.0f, -1.0f);  //the direction where camera looks that negative z
    cameraUp = vec3(0.0f, 1.0f, 0.0f);      //upward direction

    //Load Texture
    if(LoadGLTexture(&texture_Cube,MAKEINTRESOURCE(IDBITMAP_MARBAL)) == FALSE)
    {
        fprintf(gpFile, " LoadGLTexture(); Failed To Load Marbal Texture");
        return(-8);
    }

    //Enable Texturing
    glEnable(GL_TEXTURE_2D);

    PerspectiveProjectionMatrix = mat4::identity(); 

    // Wormup Resize
    resize(WIN_WIDTH, WIN_HEIGHT);
    return (0);
}

void printGLInfo(void)
{
    // variable declarations
    GLint numExtensions, i;

    // code
    // print opengl information
    fprintf(gpFile, "OpenGL Information!!\n");
    fprintf(gpFile, "********************\n");
    fprintf(gpFile, "OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
    fprintf(gpFile, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
    fprintf(gpFile, "OpenGL Version: %s\n", glGetString(GL_VERSION));
    fprintf(gpFile, "********************\n");

    // pp
    fprintf(gpFile, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // get number of extensions
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

    // printf opengl extensions
    for (i = 0; i < numExtensions; i++)
    {
        fprintf(gpFile, " OpenGL Extension No = %d : %s \n", i, glGetStringi(GL_EXTENSIONS, i));
    }
}
BOOL LoadGLTexture(GLuint *Texture, TCHAR ImageResourceID[])
{
    //Variable declaration
    HBITMAP hBitmap = NULL;
    BITMAP bmp;
    BOOL bResult = FALSE;

    //code
    //Load The Bitmap As Image
    hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), ImageResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

    if(hBitmap)
    {
        bResult = TRUE;
        
        //get bitmap structure from the loaded bitmap image
        GetObject(hBitmap, sizeof(BITMAP), &bmp);

        //Generate OpenGL Texture Object
        glGenTextures(1, Texture);  

        //bind to the newly Created texture object
        glBindTexture(GL_TEXTURE_2D, *Texture);

        //Unpack the image into memory for faster loading
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, (void*)bmp.bmBits);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        DeleteObject(hBitmap);
        hBitmap = NULL;
    }

    return(bResult);
}

void resize(int width, int height)
{
    // code

    // if heigth by accident is less than 0 than heigth should be 1
    if (height <= 0)
    {
        height = 1;
    }

    // set the viewport
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    PerspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void display(void)
{
    // code

    // clear OpenGL buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use shader program object
    glUseProgram(shaderProgramObject);

    //-------------------------Cube---------------------------------------
        //trasformations
        mat4 modelMatrix = mat4::identity(); 
        
        mat4 viewMatrix = mat4::identity(); 
        viewMatrix = vmath::lookat(cameraPos, cameraPos + cameraFront, cameraUp);        //The view matrix transforms the world so it looks like the camera is moving.
                                                                                        //cameraFront is a direction vector.
                                                                                        // lookAt() requires a point in space.
                                                                                        // So we convert direction → point by adding it to camera position. *Position + Direction = Target pointyes

        mat4 translationMatrix = mat4::identity();
        translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

        mat4 rotationMatrix = mat4::identity();
        mat4 rotationMatrix1 = mat4::identity();
        rotationMatrix1 = vmath::rotate(angleCube, 1.0f, 0.0f, 0.0f);  //x

        mat4 rotationMatrix2 = mat4::identity();
        rotationMatrix2 = vmath::rotate(angleCube, 0.0f, 1.0f, 0.0f);   //y

        mat4 rotationMatrix3 = mat4::identity();
        rotationMatrix3 = vmath::rotate(angleCube, 0.0f, 0.0f, 1.0f);   //Z

        rotationMatrix = rotationMatrix1 * rotationMatrix2 * rotationMatrix3;
        
        modelMatrix = translationMatrix *  rotationMatrix ;

        //Use FOV in Projection Matrix
        PerspectiveProjectionMatrix = vmath::perspective(fov, (GLfloat)winWidth/(GLfloat)winHeigth, 0.1f, 100.0f);
    
        // send above matrix to vertex shader in uniform
        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, PerspectiveProjectionMatrix);

        if (bLight == TRUE)
        {
            glUniform3fv(lAUniform, 1, lightAmbient);
            glUniform3fv(ldUniform, 1, lightDiffuse);
            glUniform3fv(lSUniform, 1, lightSpecular);
            glUniform4fv(lightPositionUniform, 1, lightPosition);

            glUniform3fv(kAUniform, 1, materialAmbient);
            glUniform3fv(kdUniform, 1, materialDiffuse);
            glUniform3fv(kSUniform, 1, materialSpecular);
            glUniform1f(materialShininessUniform, materialShininess);

            glUniform1i(LKeyPressUniform, 1);
        }
        else
        {
            glUniform1i(LKeyPressUniform, 0);
        }

          //for texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_Cube);
        glUniform1i(textureSamplerUniform, 0);

        //bind with vao
        glBindVertexArray(vao_Cube);
            // draw the vertex arrays
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
            glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
            glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
            glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
            glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
            glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0); // unbind with vao_Cube
    //-----------------------------------------------------------------------------

    // unUse shader program object
    glUseProgram(0);

    // swap the buffer
    SwapBuffers(ghdc);
}

void update(void)
{

    angleCube = angleCube - 0.01f;
    if(angleCube <= 0.0f)
    {
        angleCube = angleCube + 360.0f;
    }
}

void uninitialise(void)
{
    // Local Function declarations
    void toggleFullScreen(void);

    // code

     if(texture_Cube)
    {
        glDeleteTextures(1, &texture_Cube);
        texture_Cube = 0;
    }

    // ifuser is exiting in full screen than restore the full screen back to normal
    if (gbFullScreen == TRUE)
    {
        toggleFullScreen();

        gbFullScreen = FALSE;
    }
       
      // free vbo of Color
    if(vbo_Cube)
    {
        glDeleteBuffers(1, &vbo_Cube);
        vbo_Cube = 0;
    }

    // free vao
    if(vao_Cube)
    {
        glDeleteVertexArrays(1, &vao_Cube);
        vao_Cube = 0;
    }

    // ditach , delete shader objects , and shader program object generically for any number and any type of shader
    if (shaderProgramObject)
    {
        // 1) check the shader program object is still there
        glUseProgram(shaderProgramObject);

        // 2) get number of  shaders and continue only if number of shaders is grater than 0
        GLint numShaders = 0;
        glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);
        if (numShaders > 0)
        {
            // 3) create a buffer / array to hold shader objects of obtained size
            GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
            if (pShaders != NULL)
            {
                // 4) get shader objects in this buffers
                glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);

                //5) start the loop for obtained number of shader and resize the group dettach and delete
                // every shader from the buffer / array
                for (GLint i = 0; i < numShaders; i++)
                {
                    glDetachShader(shaderProgramObject, pShaders[i]);
                    glDeleteShader(pShaders[i]);
                    pShaders[i] = 0;
                }
            }
            //6) free the buffer / array
            free(pShaders);
            pShaders = 0;
        }
        //7) delete the shader program object
        glUseProgram(0);
        glDeleteProgram(shaderProgramObject);
    }

    // make hdc as currnt context by relising rendering context as current context
    if (wglGetCurrentContext() == ghrc)
    {
        wglMakeCurrent(NULL, NULL);
    }

    // delete the rendering contexr
    if (ghrc)
    {
        wglDeleteContext(ghrc);
        ghrc = NULL;
    }

    // Release the DC
    if (ghdc)
    {
        ReleaseDC(ghwnd, ghdc);
        ghdc = NULL;
    }

    // Destroy window
    if (ghwnd)
    {
        DestroyWindow(ghwnd);
        ghwnd = NULL;
    }

    // Close the file
    if (gpFile)
    {
        fprintf(gpFile, "Program Terminated Successfully....\n");
        fclose(gpFile);
        gpFile = NULL;
    }
}
