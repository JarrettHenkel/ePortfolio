#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <learnOpengl/camera.h> // Camera class

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

//MPI definition 
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Jarrett Henkel Project"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;


    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbos[2];     // Handles for the vertex buffer objects
        GLuint nIndices;    // Number of indices of the mesh
        GLuint texture;     // Handle for the texture
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Cube mesh data
    GLMesh gMesh;
    // Sphere mesh data
    GLMesh gSphere;
    // Plane mesh data
    GLMesh gPlane;
    //Cylinder mesh data
    GLMesh gCylinder;
    //Torus mesh data
    GLMesh gTorus;
    // Shader program
    GLuint gProgramId;
    bool isPerspective = true;

    // Cube and light color
    glm::vec3 gObjectColor(0.5f, 0.2f, 0.0f);
    glm::vec3 gLightColor(1.0f, 0.9f, 0.8f);
    glm::vec3 gLightColor2(1.0f, 0.9f, 0.8f);

    // Light position and scale
    glm::vec3 gLightPosition(1.5f, 5.0f, 3.0f);
    glm::vec3 gLightScale(0.0f);

    // Second light source
    glm::vec3 gLightPosition2(-1.5f, 5.0f, -3.0f);
    glm::vec3 gLightScale2(0.0f);
}

// Camera variables
Camera gCamera(glm::vec3(0.0f, 0.0f, 3.0f));
float gLastX = WINDOW_WIDTH / 2.0f;
float gLastY = WINDOW_HEIGHT / 2.0f;
bool gFirstMouse = true;
float gSpeedMultiplier = 1.0f;

// Timing variables
float gDeltaTime = 0.0f; // Time between current frame and last frame
float gLastFrame = 0.0f;


/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh(GLMesh& mesh);
void UCreateSphere(GLMesh& mesh, float radius, int sectorCount, int stackCount);
void UCreatePlane(GLMesh& mesh);
void UCreateCylinder(GLMesh& mesh, float baseRadius, float topRadius, float height, int sectorCount, int stackCount);
void UCreateTorus(GLMesh& mesh, float ringRadius, float tubeRadius, int ringSectorCount, int tubeSectorCount);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyMesh(GLMesh& mesh);
void UDestroySphere(GLMesh& mesh);
void UDestroyPlane(GLMesh& mesh);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);


const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);

/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightColor2;
uniform vec3 lightPos2;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Texture sampler

void main()
{
    // Ambient lighting
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting for the first light source
    vec3 norm = normalize(vertexNormal);
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos);
    float impact = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = impact * lightColor;

    // Diffuse lighting for the second light source
    vec3 lightDirection2 = normalize(lightPos2 - vertexFragmentPos);
    float impact2 = max(dot(norm, lightDirection2), 0.0);
    vec3 diffuse2 = impact2 * lightColor2;

    // Combine the impact of both lights
    vec3 combinedDiffuse = diffuse + diffuse2;

    // Texture color
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate);

    // Calculate final color
    vec3 finalColor = (ambient + combinedDiffuse) * textureColor.xyz;

    fragmentColor = vec4(finalColor, 1.0); // Send final color to GPU
}
);

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}

int main(int argc, char* argv[])
{
    // Initialize GLFW and create a window
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh for the cube
    UCreateMesh(gMesh);

    // Load texture for the cube
    if (!UCreateTexture("../../resources/textures/Cube2.png", gMesh.texture)) {
        cout << "Failed to load texture for cube" << endl;
        return EXIT_FAILURE;
    }

    // Create the plane mesh
    UCreatePlane(gPlane);

    // Load texture for the plane
    if (!UCreateTexture("../../resources/textures/Granite.jpg", gPlane.texture)) {
        cout << "Failed to load texture for plane" << endl;
        return EXIT_FAILURE;
    }

    // Create the sphere mesh
    UCreateSphere(gSphere, 0.5f, 36, 18);

    // Load texture for the sphere
    if (!UCreateTexture("../../resources/textures/Styrofoam.jpg", gSphere.texture)) {
        cout << "Failed to load texture for sphere" << endl;
        return EXIT_FAILURE;
    }

    // Create the cylinder mesh
    UCreateCylinder(gCylinder, 0.5f, 0.5f, 2.0f, 36, 18);

    // Load texture for the cylinder
    if (!UCreateTexture("../../resources/textures/Leopard.jpg", gCylinder.texture)) {
        cout << "Failed to load texture for cylinder" << endl;
        return EXIT_FAILURE;
    }

    // Create the torus mesh
    UCreateTorus(gTorus, 0.7f, 0.3f, 36, 18); // Customize torus size and detail

    // Load texture for the torus
    if (!UCreateTexture("../../resources/textures/Wood.jpg", gTorus.texture)) {
        cout << "Failed to load texture for torus" << endl;
        return EXIT_FAILURE;
    }

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // Set the background color of the window to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Render loop
    while (!glfwWindowShouldClose(gWindow))
    {
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        UProcessInput(gWindow);
        URender();
        glfwPollEvents();
    }

    // Release mesh data for all objects
    UDestroyMesh(gMesh);
    UDestroyMesh(gSphere);
    UDestroyMesh(gPlane);
    UDestroyMesh(gCylinder); // Release mesh data for cylinder
    UDestroyMesh(gTorus);

    // Release texture data
    UDestroyTexture(gMesh.texture);
    UDestroyTexture(gSphere.texture);
    UDestroyTexture(gPlane.texture);
    UDestroyTexture(gCylinder.texture); // Release texture data for cylinder
    UDestroyTexture(gTorus.texture);

    // Release shader program
    UDestroyShaderProgram(gProgramId);

    glfwDestroyWindow(gWindow);
    glfwTerminate();

    return EXIT_SUCCESS;
}

// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        isPerspective = !isPerspective;
        glfwWaitEventsTimeout(0.2);
    }
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gSpeedMultiplier += yoffset * 0.1f; // Adjust the 0.1f value to control how fast the speed changes
    gSpeedMultiplier = max(0.1f, gSpeedMultiplier); // Prevent the speed from going negative

    gCamera.MovementSpeed = 2.5f * gSpeedMultiplier; // Adjust the base speed (2.5f) as needed
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// Functioned called to render a frame
void URender() {
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the shader program
    glUseProgram(gProgramId);

    // Camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Projection transformation
    glm::mat4 projection;
    if (isPerspective) {
        projection = glm::perspective(glm::radians(gCamera.Zoom),
            (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else {
        float orthoHeight = 5.0f;
        float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
        projection = glm::ortho(-orthoHeight * aspectRatio, orthoHeight * aspectRatio,
            -orthoHeight, orthoHeight, -10.0f, 10.0f);
    }

    // Retrieve and pass transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");
    GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
    GLint lightColorLoc2 = glGetUniformLocation(gProgramId, "lightColor2");
    GLint lightPositionLoc2 = glGetUniformLocation(gProgramId, "lightPos2");
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Set the light and material properties
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
    glUniform3f(viewPositionLoc, gCamera.Position.x, gCamera.Position.y, gCamera.Position.z);
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);

    // Draw the plane
    glBindTexture(GL_TEXTURE_2D, gPlane.texture);
    glm::mat4 model = glm::translate(glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(gPlane.vao);
    glDrawElements(GL_TRIANGLES, gPlane.nIndices, GL_UNSIGNED_SHORT, NULL);

    // Draw the cube
    glBindTexture(GL_TEXTURE_2D, gMesh.texture);
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(gMesh.vao);
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);

    // Draw the sphere
    glBindTexture(GL_TEXTURE_2D, gSphere.texture);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(gSphere.vao);
    glDrawElements(GL_TRIANGLES, gSphere.nIndices, GL_UNSIGNED_INT, NULL);

    // Draw the cylinder
    glBindTexture(GL_TEXTURE_2D, gCylinder.texture);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.1f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 0.6f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(gCylinder.vao);
    glDrawElements(GL_TRIANGLES, gCylinder.nIndices, GL_UNSIGNED_INT, NULL);

    // Draw the torus
    glBindTexture(GL_TEXTURE_2D, gTorus.texture);
    model = glm::translate(glm::vec3(-0.7f, -0.3f, 1.5f));
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.5f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(gTorus.vao);
    glDrawElements(GL_TRIANGLES, gTorus.nIndices, GL_UNSIGNED_INT, NULL);

    // Unbind VAO
    glBindVertexArray(0);

    // Swap the buffers
    glfwSwapBuffers(gWindow);
}


// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
    GLfloat verts[] = {
        // Positions          Normals              Texture Coords
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        // Left face
        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        // Right face
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
         // Top face
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
          // Bottom face
          -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
          -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
           0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
           0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };

    // Index data for drawing the triangles
    GLushort indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        8, 9, 10, 10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Top face
        16, 17, 18, 18, 19, 16,
        // Bottom face
        20, 21, 22, 22, 23, 20
    };

    // Set up the vertex array object, vertex buffer object, and element buffer object
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set up vertex attribute pointers
    GLint stride = 8 * sizeof(float); // 3 for position, 3 for normal, 2 for texture coord
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind the VAO
}

void UCreateSphere(GLMesh& mesh, float radius, int sectorCount, int stackCount) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float x, y, z, xy;              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;  // normal
    float s, t;                     // texCoord

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);               // r * cos(u)
        z = radius * sinf(stackAngle);                // r * sin(u)

        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            vertices.push_back(s);
            vertices.push_back(t);
        }
    }

    int k1, k2;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    // Generate VAO, VBO, and EBO
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.vbos[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // Vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    mesh.nIndices = static_cast<GLuint>(indices.size());
}

void UCreatePlane(GLMesh& mesh) {
    // Define vertices with positions, normals, and texture coordinates
    GLfloat verts[] = {
        // Positions           Normals          Texture Coords
        -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Left back
         0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Right back
         0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Right front
        -0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f  // Left front
    };

    GLushort indices[] = {
        0, 1, 2,  // First triangle
        0, 2, 3   // Second triangle
    };

    // Generate and bind the Vertex Array Object (VAO)
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Generate two buffers: one for the vertex positions and one for the indices
    glGenBuffers(2, mesh.vbos);

    // Bind and set the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Bind and set the element buffer
    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Define the position attribute layout
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerTexCoord = 2;
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerTexCoord);

    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    // Define the normal attribute layout
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    // Define the texture coordinate attribute layout
    glVertexAttribPointer(2, floatsPerTexCoord, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    // Unbind the VAO (not the EBO, as it's stored in the VAO)
    glBindVertexArray(0);
}

//Cylinder Creation
void UCreateCylinder(GLMesh& mesh, float baseRadius, float topRadius, float height, int sectorCount, int stackCount) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Side vertices
    float stackHeight = height / stackCount;
    float radiusStep = (topRadius - baseRadius) / stackCount;
    float sectorStep = 2 * M_PI / sectorCount;
    float sectorAngle;

    for (int i = 0; i <= stackCount; ++i) {
        float currentRadius = baseRadius + i * radiusStep;
        float currentHeight = -0.5f * height + i * stackHeight;

        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;

            // Vertex position
            float x = currentRadius * cosf(sectorAngle);
            float y = currentHeight;
            float z = currentRadius * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Normal vector
            float nx = cosf(sectorAngle);
            float ny = 0;
            float nz = sinf(sectorAngle);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);

            // Texture coordinates
            float s = (float)j / sectorCount;
            float t = (float)i / stackCount;
            vertices.push_back(s);
            vertices.push_back(t);
        }
    }

    // Side indices
    int k1, k2;
    // Generating indices for side surfaces
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector, at each stack
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }

    // Top cap
    float topCenterY = height / 2.0f;
    int baseIndex = vertices.size() / 8; // 8 floats per vertex
    int centerIndex = baseIndex; // Center vertex index for the top cap

    // Center vertex for the top cap
    vertices.push_back(0.0f);
    vertices.push_back(topCenterY);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.5f);
    vertices.push_back(0.5f);
    baseIndex++;

    for (int j = 0; j <= sectorCount; ++j) {
        sectorAngle = j * sectorStep;
        float x = topRadius * cosf(sectorAngle);
        float z = topRadius * sinf(sectorAngle);

        vertices.push_back(x);
        vertices.push_back(topCenterY);
        vertices.push_back(z);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back((float)j / sectorCount);
        vertices.push_back(1.0f);
    }

    for (int j = 0; j < sectorCount; ++j, ++baseIndex) {
        indices.push_back(centerIndex);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
    }

    // Bottom cap
    float bottomCenterY = -height / 2.0f;
    centerIndex = vertices.size() / 8; // Center vertex index for the bottom cap

    // Center vertex for the bottom cap
    vertices.push_back(0.0f);
    vertices.push_back(bottomCenterY);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(-1.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.5f);
    vertices.push_back(0.5f);
    baseIndex++;

    for (int j = 0; j <= sectorCount; ++j) {
        sectorAngle = j * sectorStep;
        float x = baseRadius * cosf(sectorAngle);
        float z = baseRadius * sinf(sectorAngle);

        vertices.push_back(x);
        vertices.push_back(bottomCenterY);
        vertices.push_back(z);
        vertices.push_back(0.0f);
        vertices.push_back(-1.0f);
        vertices.push_back(0.0f);
        vertices.push_back((float)j / sectorCount);
        vertices.push_back(0.0f);
    }

    for (int j = 0; j < sectorCount; ++j, ++baseIndex) {
        indices.push_back(centerIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex);
    }

    // Generate VAO, VBO, and EBO for the cylinder
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.vbos[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    // Vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    mesh.nIndices = static_cast<GLuint>(indices.size());
}

//Torus Creation
void UCreateTorus(GLMesh& mesh, float outerRadius, float innerRadius, int nsides, int nrings) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Calculate torus vertices
    for (int i = 0; i <= nrings; ++i) {
        float ringPhi = float(i) / nrings * 2.0f * M_PI;
        for (int j = 0; j <= nsides; ++j) {
            float sideTheta = float(j) / nsides * 2.0f * M_PI;

            float cosRingPhi = cosf(ringPhi);
            float sinRingPhi = sinf(ringPhi);
            float cosSideTheta = cosf(sideTheta);
            float sinSideTheta = sinf(sideTheta);

            // Vertex position
            float x = (outerRadius + innerRadius * cosSideTheta) * cosRingPhi;
            float y = (outerRadius + innerRadius * cosSideTheta) * sinRingPhi;
            float z = innerRadius * sinSideTheta;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Normal
            float nx = cosRingPhi * cosSideTheta;
            float ny = sinRingPhi * cosSideTheta;
            float nz = sinSideTheta;
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);

            // Texture coordinates
            vertices.push_back(float(i) / nrings);
            vertices.push_back(float(j) / nsides);
        }
    }

    // Calculate torus indices
    for (int i = 0; i < nrings; ++i) {
        for (int j = 0; j < nsides; ++j) {
            int first = (i * (nsides + 1)) + j;
            int second = first + nsides + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    // Create VAO, VBO, and EBO for the torus
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.vbos[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    // Vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    mesh.nIndices = static_cast<GLuint>(indices.size());
}

/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}

void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, mesh.vbos);
}

void UDestroySphere(GLMesh& mesh) {
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, mesh.vbos);
}

void UDestroyPlane(GLMesh& mesh) {
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, mesh.vbos);
}

void UDestroyTexture(GLuint textureId)
{
    glDeleteTextures(1, &textureId);
}
// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}

