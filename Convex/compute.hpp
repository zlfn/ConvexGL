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

struct VertexE
{
    double x;
    double y;
    double z;
};


// 두 점 사이의 거리를 구하는 것을 GPU로 가속
void getDistanceGPU(ComputeShader shader, std::vector<Plane> p, std::vector<Vertex*> v, std::vector<double> &o)
{
    std::vector<PlaneE> pe;
    for(Plane pl : p)
        pe.push_back(PlaneE{pl.a, pl.b, pl.c, pl.k});
    std::vector<VertexE> ve;
    for(Vertex* vl : v)
        ve.push_back(VertexE{vl->x, vl->y, vl->z});
    std::vector<double> gpubuf(v.size(), 0);

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
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buf_out);

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
        {
            o[j * v.size() + i] = result[j * v.size() + i];
        }
}

// 특정 평면 외부/내부 판별을 GPU를 이용하여 가속
void DivideOutsideGPU(ComputeShader cs, std::vector<Plane> &polyhedron, std::vector<Vertex*> &inside, std::vector<Vertex*> &outside, Color color1, Color color2)
{
    std::vector<bool> mark(outside.size(), false);
    std::vector<double> out(outside.size(), 0);

    for(int i = 0; i < polyhedron.size(); i++)
    {
        getDistanceGPU(cs, std::vector<Plane>{polyhedron[i]}, outside, out);
        for(int j = 0; j < outside.size(); j++)
        {
            if(out[j] > ZERO)
                mark[j] = true;
        }
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
    int mp = 0;
    int mv = 0;
    double maxDis = polyhedron[mp].getDistance(*outside[mv]);
    std::vector<double> out(outside.size());

    for(int i = 0; i < polyhedron.size(); i++)
    {
        getDistanceGPU(shader, std::vector<Plane>{polyhedron[i]}, outside, out);
        for(int j = 0; j < outside.size(); j++)
        {
            if(out[j] > maxDis)
            {
                mp = i; mv = j;
                maxDis = out[j];
            }
        }

    }

    return outside[mv];
}
