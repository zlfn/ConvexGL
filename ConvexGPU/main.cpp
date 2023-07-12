//OpenGL
#include "glad/glad.h"
#include <GLFW/glfw3.h>

//GLM (Mathematics Library for OpenGL)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//ETC
#include <random>
#include "geometry.hpp"
#include "compute.hpp"
#include "custom/shader.hpp"

//Window 변수
int winWidth = 800;
int winHeight = 600;

//Array Object 들어갈 배열
std::vector<float> vertices;
std::vector<float> lines;
std::vector<float> planes;

//계산용 배열
std::vector<Vertex> points;
std::vector<Plane> polyhedron;

//ConvexHull용 배열
std::vector<Vertex*> inside;
std::vector<Vertex*> outside;

//카메라 벡터
glm::dvec3 cameraPos = glm::dvec3(0.0f, 0.0f, 3.0f);
glm::dvec3 cameraFront = glm::dvec3(0.0f, 0.0f, -1.0f);
glm::dvec3 cameraUp = glm::dvec3(0.0f, 1.0f, 0.0f);
double yaw = -90.0f, pitch = 0.0f;
double camRadius = 13.0f;

//입력 처리용 DeltaTime
double deltaTime = 0.0f;
double lastFrame = 0.0f;
double inputInterval = 0.0f;

//입력용 변수
double lastX = 400, lastY = 300;
bool getNext = false;
bool detachedNext = true;

void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main() {
    //OpenGL 기본 설정
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dist(0.0f, 1.0f);

    GLFWwindow* window = glfwCreateWindow(800, 600, "ConvexHull", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == nullptr)
        return -1;
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    glViewport(0, 0, winWidth, winHeight);
    glClearColor(0.4f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(5.0f);
    glLineWidth(5.0f);

    int size;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &size);
    std::cout << "Max Size of WorkGroupX: " << size << std::endl;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &size);
    std::cout << "Max Size of WorkGroupY: " << size << std::endl;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &size);
    std::cout << "Max Size of WorkGroupZ: " << size << std::endl;
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &size);
    std::cout << "Max Size of SSBO: " << size/1024/1024 << "MB" << std::endl;

    //정점 초기화 코드 시작
    for(int i = 0; i < 1000; i++)
    {
        Vertex p = Vertex(glm::vec3(dist(gen), dist(gen), dist(gen)), i);
        points.push_back(p);
    }
    for(Vertex& p : points)
        outside.push_back(&p);

    std::cout << "Generated Points" << std::endl;


    //Convex Hull 시작
    double startTime = glfwGetTime();

    CreateSimplex(polyhedron, points);

    ComputeShader disShader("ConvexGPU/shader/distance.comp");
    DivideOutsideGPU(disShader, polyhedron, inside, outside);

    /*
    while(true)
    {
        if(outside.empty()) break;
        DivideOutsideGPU(disShader, polyhedron, inside, outside);

        Vertex* FP = GetFurthestPointGPU(disShader, polyhedron, outside);
        NextPolyhedron(polyhedron, *FP, *inside[0]);
    }
    double endTime = glfwGetTime();
    std::cout << endTime - startTime << std::endl; */

    for(Vertex p : points)
    {
        p.draw(vertices);
    }

    for(Plane p : polyhedron)
    {
        p.draw(planes);
        p.drawSkeleton(lines);
        p.drawNormal(lines);
    }

    //VertexArrayObject 초기화
    unsigned int VAO1;
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);
    unsigned int VBO1;
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    unsigned int VBO2;
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lines.size(), &lines[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int VAO3;
    glGenVertexArrays(1, &VAO3);
    glBindVertexArray(VAO3);
    unsigned int VBO3;
    glGenBuffers(1, &VBO3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planes.size(), &planes[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);


    //셰이더 컴파일
    Shader shader("ConvexGPU/shader/vertex.vert",
                 "ConvexGPU/shader/fragment.frag");

    //렌더링 사이클 시작
    while(!glfwWindowShouldClose(window))
    {
        //입력 처리
        processInput(window);

        //계산 처리
        if(getNext && detachedNext)
        {
            getNext = detachedNext = false;

            if(outside.empty())
            {
                std::cout << "FULL" << std::endl;
                goto SKIP;
            }


            Vertex* FP = GetFurthestPointGPU(disShader, polyhedron, outside);
            NextPolyhedron(polyhedron, *FP, *inside[0]);

            DivideOutsideGPU(disShader,polyhedron, inside, outside);

            vertices.clear();
            lines.clear();
            planes.clear();

            for(Vertex p : points)
            {
                p.draw(vertices);
            }

            for(Plane p : polyhedron)
            {
                p.draw(planes);
                p.drawSkeleton(lines);
                p.drawNormal(lines);
            }

        }
        SKIP:


        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lines.size(), &lines[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, VBO3);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planes.size(), &planes[0], GL_STATIC_DRAW);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //렌더링 행렬 설정
        ///model
        glm::mat4 model = glm::mat4(1.0f);

        ///view
        double camX = sin(glfwGetTime()) * camRadius;
        double camZ = cos(glfwGetTime()) * camRadius;
        glm::mat4 view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        //glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        ///projection
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)winWidth / (float)winHeight, 0.1f, 100.0f);

        //렌더링
        shader.use();
        shader.setUniformMat4("model", model);
        shader.setUniformMat4("view", view);
        shader.setUniformMat4("projection", projection);
        glm::dvec3 cam(camX, 0.0f, camZ);
        shader.setUniformDvec3("camPos", cam);

        glPointSize(3.0f);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_POINTS, 0, vertices.size() / 6);

        glLineWidth(1.0f);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_LINES, 0, lines.size() / 6);

        glBindVertexArray(VAO3);
        glDrawArrays(GL_TRIANGLES, 0, planes.size() / 6);


        //deltaTime 계산
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //SwapBuffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    double xOffset = xPos - lastX;
    double yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    const double sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::dvec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    winWidth = width;
    winHeight = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    const double cameraSpeed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraFront;
        camRadius += cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraFront;
        camRadius -= cameraSpeed;
    }
    cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        getNext = true;
    }
    else {
        getNext = false;
        detachedNext = true;
    }
}
