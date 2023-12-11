#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

enum Color {
    ORANGE,
    MINT,
    LIGHT_BLUE,
    RED,
    BLUE,
    YELLOW,
    GREEN,
    WHITE,
    DARK_MINT,
    DARK_ORANGE,
};

std::tuple<float, float, float, float> getColor(Color c)
{
    switch(c)
    {
        case ORANGE:
            return std::make_tuple(1.0f, 0.5f, 0.2f, 1.0f);
        case MINT:
            return std::make_tuple(0.0f, 1.0f, 1.0f, 1.0f);
        case RED:
            return std::make_tuple(1.0f, 0.0f, 0.0f, 1.0f);
        case GREEN:
            return std::make_tuple(0.0f, 1.0f, 0.0f, 1.0f);
        case BLUE:
            return std::make_tuple(0.0f, 0.0f, 1.0f, 1.0f);
        case LIGHT_BLUE:
            return std::make_tuple(0.1f, 0.7f, 1.0f, 1.0f);
        case YELLOW:
            return std::make_tuple(1.0f, 1.0f, 0.0f, 1.0f);
        case WHITE:
            return std::make_tuple(1.0f, 1.0f, 1.0f, 1.0f);
        case DARK_MINT:
            return std::make_tuple(0.2f, 0.3f, 0.3f, 1.0f);
        case DARK_ORANGE:
            return std::make_tuple(0.5f, 0.2f, 0.0f, 1.0f);
    }
}

void drawVertex(glm::vec3 p, std::vector<float> &vertices, Color color)
{
    vertices.push_back(p.x);
    vertices.push_back(p.y);
    vertices.push_back(p.z);
    std::tuple colorTuple = getColor(color);
    vertices.push_back(std::get<0>(colorTuple));
    vertices.push_back(std::get<1>(colorTuple));
    vertices.push_back(std::get<2>(colorTuple));
    vertices.push_back(std::get<3>(colorTuple));
}

void drawLine(glm::vec3 p1, glm::vec3 p2, std::vector<float> &vertices, Color color)
{
    std::tuple colorTuple = getColor(color);

    vertices.push_back(p1.x);
    vertices.push_back(p1.y);
    vertices.push_back(p1.z);
    vertices.push_back(std::get<0>(colorTuple));
    vertices.push_back(std::get<1>(colorTuple));
    vertices.push_back(std::get<2>(colorTuple));
    vertices.push_back(std::get<3>(colorTuple));
    vertices.push_back(p2.x);
    vertices.push_back(p2.y);
    vertices.push_back(p2.z);
    vertices.push_back(std::get<0>(colorTuple));
    vertices.push_back(std::get<1>(colorTuple));
    vertices.push_back(std::get<2>(colorTuple));
    vertices.push_back(std::get<3>(colorTuple));
}

void drawPlane(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, std::vector<float> &vertices, Color color)
{
    std::tuple colorTuple = getColor(color);

    vertices.push_back(p1.x);
    vertices.push_back(p1.y);
    vertices.push_back(p1.z);
    vertices.push_back(std::get<0>(colorTuple));
    vertices.push_back(std::get<1>(colorTuple));
    vertices.push_back(std::get<2>(colorTuple));
    vertices.push_back(std::get<3>(colorTuple)-0.9f);
    vertices.push_back(p2.x);
    vertices.push_back(p2.y);
    vertices.push_back(p2.z);
    vertices.push_back(std::get<0>(colorTuple));
    vertices.push_back(std::get<1>(colorTuple));
    vertices.push_back(std::get<2>(colorTuple));
    vertices.push_back(std::get<3>(colorTuple)-0.9f);
    vertices.push_back(p3.x);
    vertices.push_back(p3.y);
    vertices.push_back(p3.z);
    vertices.push_back(std::get<0>(colorTuple));
    vertices.push_back(std::get<1>(colorTuple));
    vertices.push_back(std::get<2>(colorTuple));
    vertices.push_back(std::get<3>(colorTuple)-0.9f);
}
