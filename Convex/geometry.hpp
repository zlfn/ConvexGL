#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>
#include "draw.hpp"

//부동소수점 오차를 고려
const double ZERO = 1e-7;

class Vertex {
public:
    double x;
    double y;
    double z;
    int ID = -1;
    glm::dvec3 vec;
    Color color;

    Vertex(glm::dvec3 p)
    {
        x = p[0];
        y = p[1];
        z = p[2];
        vec = p;
        color = WHITE;
    }
    Vertex(glm::dvec3 p, Color c)
    {
        x = p[0];
        y = p[1];
        z = p[2];
        vec = p;
        color = c;
    }
    Vertex(glm::dvec3 p, int id)
    {
        x = p[0];
        y = p[1];
        z = p[2];
        vec = p;
        color = WHITE;
        ID = id;
    }
    Vertex(glm::dvec3 p, Color c, int id)
    {
        x = p[0];
        y = p[1];
        z = p[2];
        vec = p;
        color = c;
        ID = id;
    }

    void draw(std::vector<float> &vertices)
    {
        drawVertex(vec, vertices, color);
    }

    double getDistance(Vertex p)
    {
        return glm::sqrt(glm::pow(x - p.x, 2) + glm::pow(y - p.x, 2) + glm::pow(z - p.z, 2));
    }
};

bool IDCom(Vertex a, Vertex b){
    return a.ID > b.ID;
}

class Line {
public:
    std::vector<Vertex> point;

    glm::dvec3 vec;
    Color color;
    Line(Vertex p1, Vertex p2)
    {
        point.push_back(p1);
        point.push_back(p2);
        std::sort(point.begin(), point.end(), IDCom);

        vec = p2.vec - p1.vec;
        color = p1.color;
    }
    Line(Vertex p1, Vertex p2, Color c)
    {
        point.push_back(p1);
        point.push_back(p2);
        std::sort(point.begin(), point.end(), IDCom);

        vec = p2.vec - p1.vec;
        color = c;
    }
    Line(glm::dvec3 p1, glm::dvec3 p2)
    {
        point.emplace_back(p1);
        point.emplace_back(p2);
        std::sort(point.begin(), point.end(), IDCom);

        vec = p2 - p1;
        color = WHITE;
    }
    Line(glm::dvec3 p1, glm::dvec3 p2, Color c)
    {
        point.emplace_back(p1, c);
        point.emplace_back(p2, c);
        std::sort(point.begin(), point.end(), IDCom);

        vec = p2 - p1;
        color = c;
    }

    double getDistance(Vertex p)
    {
        return glm::length(glm::cross(point[1].vec-point[0].vec, point[0].vec-p.vec)) / glm::length(point[1].vec-point[0].vec);
    }
    void draw(std::vector<float> &vertices)
    {
        drawLine(point[0].vec, point[1].vec, vertices, color);
    }
};

class Plane {
public:
    std::vector<Vertex> point;
    std::vector<Line> edge;

    glm::dvec3 normal;
    double a, b, c, k; //ax + by + cz + k = 0
    Color color;

    Plane(Vertex p1, Vertex p2, Vertex p3)
    {
        glm::vec3 l1 = p3.vec - p1.vec;
        glm::vec3 l2 = p2.vec - p1.vec;
        normal = glm::normalize(glm::cross(l1, l2));

        point.push_back(p1);
        point.push_back(p2);
        point.push_back(p3);
        std::sort(point.begin(), point.end(), IDCom);

        edge.emplace_back(p1, p2);
        edge.emplace_back(p2, p3);
        edge.emplace_back(p1, p3);

        a = normal.x;
        b = normal.y;
        c = normal.z;
        k = -(p1.x * a + p1.y * b + p1.z * c);

        color = p1.color;
    }

    Plane(Vertex p1, Vertex p2, Vertex p3, Color co)
    {
        glm::dvec3 l1 = p3.vec - p1.vec;
        glm::dvec3 l2 = p2.vec - p1.vec;
        normal = glm::normalize(glm::cross(l1, l2));

        point.push_back(p1);
        point.push_back(p2);
        point.push_back(p3);
        std::sort(point.begin(), point.end(), IDCom);

        edge.emplace_back(p1, p2, co);
        edge.emplace_back(p2, p3, co);
        edge.emplace_back(p1, p3, co);

        a = normal.x;
        b = normal.y;
        c = normal.z;
        k = -(p1.x * a + p1.y * b + p1.z * c);

        color = co;
    }

    Plane(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3)
    {
        glm::vec3 l1 = p3 - p1;
        glm::vec3 l2 = p2 - p1;
        normal = glm::normalize(glm::cross(l1, l2));

        point.emplace_back(p1);
        point.emplace_back(p2);
        point.emplace_back(p3);
        std::sort(point.begin(), point.end(), IDCom);
        edge.emplace_back(p1, p2);
        edge.emplace_back(p2, p3);
        edge.emplace_back(p1, p3);


        a = normal.x;
        b = normal.y;
        c = normal.z;
        k = -(p1.x * a + p1.y * b + p1.z * c);

        color = WHITE;
    }

    Plane(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3, Color co)
    {
        glm::dvec3 l1 = p3 - p1;
        glm::dvec3 l2 = p2 - p1;
        normal = glm::normalize(glm::cross(l1, l2));

        point.emplace_back(p1, co);
        point.emplace_back(p2, co);
        point.emplace_back(p3, co);
        std::sort(point.begin(), point.end(), IDCom);

        edge.emplace_back(p1, p2, co);
        edge.emplace_back(p2, p3, co);
        edge.emplace_back(p1, p3, co);

        a = normal.x;
        b = normal.y;
        c = normal.z;
        k = -(p1.x * a + p1.y * b + p1.z * c);

        color = co;
    }

    void drawNormal(std::vector<float> &vertices)
    {
        glm::dvec3 center = (point[0].vec + point[1].vec + point[2].vec) / 3.0;
        glm::dvec3 norcen = normal * 0.5 + center;
        //drawLine(center, norcen, vertices, color);
    }

    void draw(std::vector<float> &vertices)
    {
        drawPlane(point[0].vec, point[1].vec, point[2].vec, vertices, color);
    }

    void drawSkeleton(std::vector<float> &vertices)
    {
        edge[0].draw(vertices);
        edge[1].draw(vertices);
        edge[2].draw(vertices);
    }

    double getDistance(Vertex p)
    {
        return (p.x * a + p.y * b + p.z * c + k) / sqrt(a*a + b*b + c*c);
    }

    double getAbsDistance(Vertex p)
    {
        return fabs(p.x * a + p.y * b + p.z * c + k) / sqrt(a*a + b*b + c*c);
    }
};

//병렬화&효율화
void DivideOutside(std::vector<Plane> &polyhedron, std::vector<Vertex*> &inside, std::vector<Vertex*> &outside, Color color1, Color color2)
{
    std::vector<Vertex*> Noutside;
    for(Vertex* vertex : outside)
    {
        bool flag = true;
        for(Plane p : polyhedron)
        {
            if(p.getDistance(*vertex) > ZERO)
            {
                vertex->color = color1;
                Noutside.push_back(vertex);
                flag = false;
                break;
            }
            else
            {
                vertex->color = color2;
            }
        }

        if(flag)
        {
            inside.push_back(vertex);
        }
    }

    outside.clear();
    outside = Noutside;
}

Vertex* GetFurthestPoint(std::vector<Plane> &polyhedron, std::vector<Vertex*> &outside)
{
    int mp = 0;
    int mv = 0;
    double maxDis = polyhedron[mp].getDistance(*outside[mv]);

    for(int i = 0; i < polyhedron.size(); i++)
        for(int j = 0; j < outside.size(); j++)
        {
            if(polyhedron[i].getDistance(*outside[j]) > maxDis)
            {
                mp = i; mv = j;
                maxDis = polyhedron[i].getDistance(*outside[j]);
            }
    }

    return outside[mv];
}

bool CheckVisible(Plane plane, Vertex vertex)
{
    return (glm::dot((vertex.vec - plane.point[0].vec), plane.normal) > ZERO);
}

//unordered_set 보조 함수
template <typename K>
bool isExist(std::set<K>& s, K key) {
    auto itr = s.find(key);
    if (itr != s.end()) {
        return true;
    } else {
        return false;
    }
}

void NextPolyhedron(std::vector<Plane> &polyhedron, Vertex fp, std::vector<Vertex*> &ip, Color color1, Color color2)
{
    std::set<std::pair<int, int>> hiddenEdge;
    std::vector<Line> borderLine;

    std::vector<Plane> nPolyhedron;
    for(Plane p : polyhedron)
        if(not CheckVisible(p, fp))
        {
            p.color = color1;
            p.edge[0].color = color1;
            p.edge[1].color = color1;
            p.edge[2].color = color1;
            nPolyhedron.push_back(p);

            hiddenEdge.insert(std::pair(p.edge[0].point[0].ID, p.edge[0].point[1].ID));
            hiddenEdge.insert(std::pair(p.edge[1].point[0].ID, p.edge[1].point[1].ID));
            hiddenEdge.insert(std::pair(p.edge[2].point[0].ID, p.edge[2].point[1].ID));
        }

    for(Plane p : polyhedron)
        if(CheckVisible(p, fp))
        {
            if(isExist(hiddenEdge, std::pair(p.edge[0].point[0].ID, p.edge[0].point[1].ID)))
                borderLine.push_back(p.edge[0]);
            if(isExist(hiddenEdge, std::pair(p.edge[1].point[0].ID, p.edge[1].point[1].ID)))
                borderLine.push_back(p.edge[1]);
            if(isExist(hiddenEdge, std::pair(p.edge[2].point[0].ID, p.edge[2].point[1].ID)))
                borderLine.push_back(p.edge[2]);
        }

    polyhedron.clear();
    polyhedron = nPolyhedron;

    for(Line l : borderLine)
    {
        Plane p = Plane(l.point[0], l.point[1], fp, color2);

        int i = 0;
    while(-ZERO < p.getDistance(*ip[i]) && p.getDistance(*ip[i]) < ZERO)
            i++;

        if(p.getDistance(*ip[i]) > ZERO)
            p = Plane(fp, l.point[1], l.point[0], color2);
        polyhedron.push_back(p);
    }
}

//QuickHull을 시작할 4면체 Simplex를 구성
void CreateSimplex(std::vector<Plane> &polyhedron, std::vector<Vertex> &p, Color color)
{
    std::vector<int> pi = {0, 0, 0, 0};

    for(int i = 0; i < p.size(); i++)
        if(p[i].x > p[pi[0]].x)
            pi[0] = i;

    for(int i = 0; i < p.size(); i++)
        if(p[pi[0]].getDistance(p[i]) > p[pi[0]].getDistance(p[pi[1]]))
            pi[1] = i;

    Line firstLine = Line(p[pi[0]], p[pi[1]]);
    for(int i = 0; i < p.size(); i++)
        if(firstLine.getDistance(p[i]) > firstLine.getDistance(p[pi[2]]))
            pi[2] = i;

    Plane firstPlane = Plane(p[pi[0]], p[pi[1]], p[pi[2]], color);
    for(int i = 0; i < p.size(); i++)
        if(firstPlane.getAbsDistance(p[i]) > firstPlane.getAbsDistance(p[pi[3]]))
            pi[3] = i;

    Plane plane = Plane(p[pi[0]], p[pi[1]], p[pi[2]], color);
    if(plane.getDistance(p[pi[3]]) > 0)
        plane = Plane(p[pi[2]], p[pi[1]], p[pi[0]], color);
    polyhedron.push_back(plane);

    plane = Plane(p[pi[0]], p[pi[1]], p[pi[3]], color);
    if(plane.getDistance(p[pi[2]]) > 0)
        plane = Plane(p[pi[3]], p[pi[1]], p[pi[0]], color);
    polyhedron.push_back(plane);

    plane = Plane(p[pi[0]], p[pi[2]], p[pi[3]], color);
    if(plane.getDistance(p[pi[1]]) > 0)
        plane = Plane(p[pi[3]], p[pi[2]], p[pi[0]], color);
    polyhedron.push_back(plane);

    plane = Plane(p[pi[1]], p[pi[2]], p[pi[3]], color);
    if(plane.getDistance(p[pi[0]]) > 0)
        plane = Plane(p[pi[3]], p[pi[2]], p[pi[1]], color);
    polyhedron.push_back(plane);
}
