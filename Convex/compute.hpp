#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "geometry.hpp"
#include "custom/shader.hpp"

//GPU 삽입용 간소화 구조체
struct PlaneE
{
    double a;
    double b;
    double c;
    double k;
};

struct LineE
{
    double x;
    double y;
    double z;
    double dx;
    double dy;
    double dz;
};

struct VertexE
{
    double x;
    double y;
    double z;
};

//두 점 벡터 사이의 모든 거리를 구함
void getDistanceVertexGPU(ComputeShader shader, std::vector<Vertex*> &v1, std::vector<Vertex*> &v2, std::vector<double> &o)
{
    std::vector<VertexE> ve1;
    for(Vertex* vl : v1)
        ve1.push_back(VertexE{vl->x, vl->y, vl->z});
    std::vector<VertexE> ve2;
    for(Vertex* vl : v2)
        ve2.push_back(VertexE{vl->x, vl->y, vl->z});
    std::vector<double> gpubuf(v1.size() * v2.size(), 0);

    unsigned buf_vertex1;
    glGenBuffers(1, &buf_vertex1);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_vertex1);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VertexE) * ve1.size(), &ve1[0],  GL_DYNAMIC_COPY);

    unsigned buf_vertex2;
    glGenBuffers(1, &buf_vertex2);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_vertex2);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VertexE) * ve2.size(), &ve2[0],  GL_DYNAMIC_COPY);

    unsigned buf_out;
    glGenBuffers(1, &buf_out);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_out);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(double) * gpubuf.size(), &gpubuf[0], GL_DYNAMIC_COPY);

    shader.use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buf_vertex1);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buf_vertex2);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buf_out);
    glDispatchCompute(ve1.size(), ve2.size(), 1);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_out);
    auto* result = reinterpret_cast<double*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));

    for(int i = 0; i < ve1.size(); i++)
        for(int j = 0; j < ve2.size(); j++)
        {
            o[j * ve1.size() + i] = result[j * ve1.size() + i];
        }

}

//점 벡터와 직선 벡터 사이의 모든 거리를 구함
void getDistanceLineGPU(ComputeShader shader, std::vector<Line> &l, std::vector<Vertex*> &v, std::vector<double> &o)
{
    std::vector<LineE> le;
    for(Line li : l)
        le.push_back(LineE{li.point[0].x, li.point[0].y, li.point[0].z, li.point[1].x-li.point[0].x, li.point[1].y - li.point[0].y, li.point[1].z - li.point[0].z});

    std::vector<VertexE> ve;
    for(Vertex* vl : v)
        ve.push_back(VertexE{vl->x, vl->y, vl->z});

    std::vector<double> gpubuf(l.size() * v.size(), 0);

    unsigned buf_vertex;
    glGenBuffers(1, &buf_vertex);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_vertex);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VertexE) * ve.size(), &ve[0],  GL_DYNAMIC_COPY);

    unsigned buf_line;
    glGenBuffers(1, &buf_line);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_line);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LineE) * le.size(), &le[0],  GL_DYNAMIC_COPY);

    unsigned buf_out;
    glGenBuffers(1, &buf_out);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_out);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(double) * gpubuf.size(), &gpubuf[0], GL_DYNAMIC_COPY);

    shader.use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buf_vertex);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buf_line);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buf_out);
    glDispatchCompute(v.size(), l.size(), 1);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_out);
    auto* result = reinterpret_cast<double*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));

    for(int i = 0; i < v.size(); i++)
        for(int j = 0; j < l.size(); j++)
            o[j * v.size() + i] = result[j * v.size() + i];
}

//점 벡터와 평면 벡터 사이의 모든 거리를 구함
void getDistancePlaneGPU(ComputeShader shader, std::vector<Plane> &p, std::vector<Vertex*> &v, std::vector<double> &o)
{
    std::vector<PlaneE> pe;
    for(Plane pl : p)
        pe.push_back(PlaneE{pl.a, pl.b, pl.c, pl.k});
    std::vector<VertexE> ve;
    for(Vertex* vl : v)
        ve.push_back(VertexE{vl->x, vl->y, vl->z});
    std::vector<double> gpubuf(p.size() * v.size(), 0);

    unsigned buf_vertex;
    glGenBuffers(1, &buf_vertex);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_vertex);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VertexE) * ve.size(), &ve[0],  GL_DYNAMIC_COPY);

    unsigned buf_plane;
    glGenBuffers(1, &buf_plane);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_plane);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(PlaneE) * pe.size(), &pe[0],  GL_DYNAMIC_COPY);

    unsigned buf_out;
    glGenBuffers(1, &buf_out);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_out);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(double) * gpubuf.size(), &gpubuf[0], GL_DYNAMIC_COPY);

    shader.use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buf_vertex);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buf_plane);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buf_out);
    glDispatchCompute(v.size(), p.size(), 1);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_out);
    auto* result = reinterpret_cast<double*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));

    for(int i = 0; i < v.size(); i++)
        for(int j = 0; j < p.size(); j++)
            o[j * v.size() + i] = result[j * v.size() + i];
}

// 특정 평면 외부/내부 판별을 GPU를 이용하여 가속
void DivideOutsideGPU(ComputeShader cs, std::vector<Plane> &polyhedron, std::vector<Vertex*> &inside, std::vector<Vertex*> &outside, Color color1, Color color2)
{
    if(polyhedron.size() * outside.size() < 2000)
        return DivideOutside(polyhedron, inside, outside, color1, color2);

    std::vector<bool> mark(outside.size(), false);
    std::vector<double> out(polyhedron.size() * outside.size(), 0);

    getDistancePlaneGPU(cs, polyhedron, outside, out);
    for(int i = 0; i < out.size(); i++)
    {
        if(out[i] > ZERO)
            mark[i % outside.size()] = true;
    }

    std::vector<Vertex*> Noutside;
    for(int i = 0; i < outside.size(); i++)
    {
        if(mark[i])
        {
            outside[i]->color = color1;
            Noutside.push_back(outside[i]);
        }
        else
        {
            outside[i]->color = color2;
            inside.push_back(outside[i]);
        }
    }

    outside.clear();
    outside = Noutside;
}

// 최원점 찾기를 GPU를 이용해 가속
Vertex* GetFurthestPointGPU(ComputeShader shader, std::vector<Plane> &polyhedron, std::vector<Vertex*> &outside)
{
    return GetFurthestPoint(polyhedron, outside);
    int mv = 0;

    std::vector<double> out(polyhedron.size() * outside.size());

    double maxDis = polyhedron[0].getDistance(*outside[mv]);

    getDistancePlaneGPU(shader, polyhedron, outside, out);

    for(int i = 0; i < polyhedron.size() * outside.size(); i++)
    {
        if(out[i] > maxDis)
        {
            mv = i % outside.size();
            maxDis = out[i];
        }
    }

    return outside[mv];
}

void CreateSimplexGPU(ComputeShader s1, ComputeShader s2, ComputeShader s3, std::vector<Plane> &polyhedron, std::vector<Vertex> &p, Color color)
{
    if(p.size() < 1000000)
        return CreateSimplex(polyhedron, p, color);

    std::vector<int> pi = {0, 0, 0, 0};

    for(int i = 0; i < p.size(); i++)
        if(p[i].x > p[pi[0]].x)
            pi[0] = i;

    std::vector<double> out(p.size(), 0);
    std::vector<Vertex*> pp;
    for(int i = 0; i < p.size(); i++)
        pp.push_back(&p[i]);

    std::vector temp{&p[pi[0]]};
    getDistanceVertexGPU(s1, temp, pp, out);

    for(int i = 0; i < p.size(); i++)
        if(out[i] > out[pi[1]])
            pi[1] = i;

    Line firstLine = Line(p[pi[0]], p[pi[1]]);
    std::vector tempLine{firstLine};
    getDistanceLineGPU(s2, tempLine, pp, out);
    for(int i = 0; i < p.size(); i++)
        if(out[i] > out[pi[2]])
            pi[2] = i;

    Plane firstPlane = Plane(p[pi[0]], p[pi[1]], p[pi[2]], color);
    std::vector tempPlane{firstPlane};
    getDistancePlaneGPU(s3, tempPlane, pp, out);
    for(int i = 0; i < p.size(); i++)
        if(glm::abs(out[i]) > glm::abs(out[pi[3]]))
            pi[3] = i;

    std::cout << pi[0] << pi[1] << pi[2] << pi[3] << std::endl;

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
