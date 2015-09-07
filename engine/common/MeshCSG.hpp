#pragma once

#include "Mesh.hpp"
#include "MathFuncs.hpp"

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
