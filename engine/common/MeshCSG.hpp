#pragma once

#include "Mesh.hpp"
#include "MathFuncs.hpp"

//#define CSGJS_HEADER_ONLY
#include <csgjs.hpp>

namespace noob
{
	static csgjs_model get_csg_model(const noob::basic_mesh& m)
	{
		csgjs_model model;
		
		for (size_t j = 0; j < m.vertices.size(); j++)
		{
			csgjs_vertex vert;
			vert.pos.x = m.vertices[j].v[0];
			vert.pos.y = m.vertices[j].v[1];
			vert.pos.z = m.vertices[j].v[2];

			vert.normal.x = m.normals[j].v[0];
			vert.normal.y = m.normals[j].v[1];
			vert.normal.z = m.normals[j].v[2];

			vert.uv.x = m.texcoords[j].v[0];
			vert.uv.y = m.texcoords[j].v[1];
			
			model.vertices.push_back(vert);
		}

		for (uint16_t i : m.indices)
		{
			model.indices.push_back(static_cast<int>(i));
		}

		return model;
	};

	static noob::basic_mesh mesh_csg(const noob::basic_mesh& a, const noob::basic_mesh& b, const noob::csg_op op)
	{
		csgjs_model model_a = get_csg_model(a);
		csgjs_model model_b = get_csg_model(b);

		csgjs_model resulting_csg_model;
		if (op == noob::csg_op::UNION)
		{
			resulting_csg_model = csgjs_union(model_a, model_b);
		}
		else if (op == noob::csg_op::DIFFERENCE)
		{
			resulting_csg_model = csgjs_difference(model_a, model_b);
		}
		else
		{
			resulting_csg_model = csgjs_intersection(model_a, model_b);
		}
		noob::basic_mesh results;
		
		
		for (size_t i = 0; i < resulting_csg_model.vertices.size(); i++)
		{
			noob::vec3 vert;
			vert.v[0] = resulting_csg_model.vertices[i].pos.x;
			vert.v[1] = resulting_csg_model.vertices[i].pos.y;
			vert.v[2] = resulting_csg_model.vertices[i].pos.z;
			results.vertices.push_back(vert);
			
			noob::vec3 norm;
			norm.v[0] = resulting_csg_model.vertices[i].normal.x;
			norm.v[1] = resulting_csg_model.vertices[i].normal.y;
			norm.v[2] = resulting_csg_model.vertices[i].normal.z;
			results.normals.push_back(norm);

			noob::vec3 uvw;
			uvw.v[0] = resulting_csg_model.vertices[i].uv.x;
			uvw.v[1] = resulting_csg_model.vertices[i].uv.y;
			results.texcoords.push_back(uvw);
		}
		
		for (int i : resulting_csg_model.indices)
		{
			results.indices.push_back(static_cast<uint16_t>(i));
		}

		
		return results;

	};
}


/*
#include <vtkPLYReader.h>
#include <vtkPLYWriter.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>


namespace noob
{
static noob::basic_mesh mesh_csg(const noob::basic_mesh& a, const noob::basic_mesh& b, const noob::csg_op op)
{
vtkSmartPointer<vtkPolyData> input1;
vtkSmartPointer<vtkPolyData> input2;

std::string input_filename_a("./temp/temp_csg_input_a.ply");
std::string input_filename_b("./temp/temp_csg_input_b.ply");

a.save(input_filename_a);
b.save(input_filename_b);

vtkSmartPointer<vtkPLYReader> reader1 = vtkSmartPointer<vtkPLYReader>::New();
reader1->SetFileName(input_filename_a.c_str());
reader1->Update();
input1 = reader1->GetOutput();

vtkSmartPointer<vtkPLYReader> reader2 = vtkSmartPointer<vtkPLYReader>::New();
reader2->SetFileName(input_filename_b.c_str());
reader2->Update();
input2 = reader2->GetOutput();

vtkSmartPointer<vtkPolyDataMapper> input1Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
input1Mapper->SetInputData(input1);
input1Mapper->ScalarVisibilityOff();

vtkSmartPointer<vtkPolyDataMapper> input2Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
input2Mapper->SetInputData(input2);
input2Mapper->ScalarVisibilityOff();

vtkSmartPointer<vtkBooleanOperationPolyDataFilter> boolean_operation = vtkSmartPointer<vtkBooleanOperationPolyDataFilter>::New();
if (op == noob::csg_op::UNION)
{
boolean_operation->SetOperationToUnion();
}
else if (op == noob::csg_op::INTERSECTION)
{
boolean_operation->SetOperationToIntersection();
}
else if (op == noob::csg_op::DIFFERENCE)
{
boolean_operation->SetOperationToDifference();
}

vtkSmartPointer<vtkPolyData> booleaned_mesh;

boolean_operation->SetInputData(0, input1);
boolean_operation->SetInputData(1, input2);

boolean_operation->Update();
booleaned_mesh = boolean_operation->GetOutput();

vtkSmartPointer<vtkPLYWriter> writer = vtkSmartPointer<vtkPLYWriter>::New();
writer->SetInputDataObject(booleaned_mesh);
writer->SetFileName("./temp/bool_results.ply");
writer->Update();

noob::basic_mesh m;
m.load_assimp("./temp/bool_results.ply");
return m;
//vtkSmartPointer<vtkPolyDataMapper> boolean_operation_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//boolean_operation_mapper->SetInputConnection(boolean_operation->GetOutputPort());
//boolean_operation_mapper->ScalarVisibilityOff();

}
}
*/
