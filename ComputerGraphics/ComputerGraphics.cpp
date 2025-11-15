// ComputerGraphics.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <glm/vec3.hpp> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

void print_matrix(mat2 m)
{
	std::cout << "Matrix 2x2!\n";
	for (int row = 0; row < 2; row++)
	{
		for (int col = 0; col < 2; col++) {
			std::cout << m[col][row] << " ";
		}
		std::cout << std::endl;
	}
}

int main()
{
	vec3 v1(1.f, 1.f, 1.f);
	vec3 v2(2.f, 2.f, 2.f);
	float l = length(v2);

	std::cout << "Vector length!\n" << l << std::endl;

	vec3 n = normalize(v2);

	std::cout << "Normalized Vector!\n" << n.x << ", " << n.y << ", " << n.z << std::endl;

	vec3 sum = v1 + v2;

	std::cout << "Vector Sum!\n" << sum.x << ", " << sum.y << ", " << sum.z << std::endl;

	vec2 v2d(v1);

	std::cout << "Vec2 from Vec3!\n" << v2d.x << ", " << v2d.y << std::endl;

	float arr[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	mat2 matrix1 = make_mat2(arr);

	float arr2[] = { 1.f, 2.f, 3.f, 4.f };
	mat2 matrix2 = make_mat2(arr2);

	mat2 matrixSum = matrix1 * matrix2;

	print_matrix(matrixSum);

	return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
