// ComputerGraphics.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define GLM_ENABLE_EXPERIMENTAL

#include <cstdlib>
#include <random>
#include <iostream>
#include <glm/vec3.hpp> 
#include <glm/glm.hpp>
#include <glm/ext.hpp>             // Non-experimental extensions
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

constexpr float THRESHOLD = static_cast<float>(1.0e-5);

struct Coordinate_frame {
	vec3 u;
	vec3 v;
	vec3 w;
};


float randomFloat(float min = -5.0f, float max = 5.0f)
{
	float t = (float)rand() / (float)RAND_MAX;
	return min + t * (max - min);
}

void generate_random_vec3(vec3* v, float min = -5.0f, float max = 5.0f) {
	v->x = randomFloat(min, max);
	v->y = randomFloat(min, max);
	v->z = randomFloat(min, max);
}

void random_invertable_mat3(mat3* m, float min = -5.0f, float max = 5.0f) {
	vec3 col1; 
	vec3 col2; 
	vec3 col3; 

	do {
		generate_random_vec3(&col3, min, max);
		generate_random_vec3(&col2, min, max);
		generate_random_vec3(&col1, min, max);
		*m = mat3(col1, col2, col3);
	} while (determinant(*m) == 0);
}

void create_coordinate_frame(vec3 view, vec3 up, Coordinate_frame* frame) {
	frame->v = normalize(view);
	frame->w = normalize(cross(up, frame->v));
	frame->u = normalize(cross(frame->v, frame->w));
}

bool matrix_inverse_distributive_property(mat3 m1, mat3 m2) {
	mat3 left = inverse(m1 * m2);
	mat3 right = inverse(m2) * inverse(m1);

	std::cout << "Inverse distributive property.\n" 
			<< "Left side result (inv(m1 * m2)) = " 
			<< to_string(left) << "\nRight side result (inv(m2) * inv(m1)) = " 
			<< to_string(right) << std::endl;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			if (epsilonNotEqual(left[i][j], right[i][j], THRESHOLD)) {
				return false;
			}
		}
	}
	return true;
}

vec3 rodrigues_rotation_formula(vec3 axis, vec3 vector, float rads) {
	vec3 axis_norm = normalize(axis);

	return vector * cos(rads) +
		(cross(axis_norm, vector)) * sin(rads) +
		axis_norm * (dot(axis_norm, vector)) * (1 - cos(rads));
}

void qtest_triple_product() {
	/* Triple Product Proposition */
	vec3 i; generate_random_vec3(&i, 1.0f, 5.0f);
	vec3 j; generate_random_vec3(&j, 1.0f, 5.0f);
	vec3 k; generate_random_vec3(&k, 1.0f, 5.0f);

	vec3 r1 = cross(i, cross(j, k));
	vec3 r2 = j * (dot(i, k)) - k * (dot(i, j));

	std::cout << "Testing Triple Product Proposition with:\nr1 = " << to_string(r1) << "\nr2 = " << to_string(r2) << std::endl;

	assert(all(epsilonEqual(r1, r2, THRESHOLD)));
	std::cout << "Triple Product equality test success.\nResults are: \nleft side = " << to_string(r1) << "  \nrigth side = " << to_string(r2) << std::endl;
}

void qtest_rodrigues_vector() {
	/* Case 1 */
	const vec3 a1 = { 0.f, 0.f, 1.f };
	const vec3 v1 = { 1.f, 0.f, 0.f };
	const vec3 expe1 = { 0.f, 1.f, 0.f };

	const float angle1 = radians(90.f);

	vec3 a_norm1 = normalize(a1);

	vec3 rodrigues1 = rodrigues_rotation_formula(a1, v1, angle1);
	assert(all(epsilonEqual(rodrigues1, expe1, THRESHOLD)));
	std::cout << "Rodrigues case 1 result: " << to_string(rodrigues1) << "\nExpected: " << to_string(expe1) << std::endl;

	/* Case 2 */
	const vec3 a2 = { 0.f, 1.f, 0.f };
	const vec3 v2 = { 1.f, 0.f, 0.f };
	const vec3 expe2 = { 0.f, 0.f, -1.f };

	const float angle2 = radians(90.f);

	vec3 a_norm2 = normalize(a2);

	vec3 rodrigues2 = rodrigues_rotation_formula(a2, v2, angle2);
	assert(all(epsilonEqual(rodrigues2, expe2, THRESHOLD)));
	std::cout << "Rodrigues case 2 result: " << to_string(rodrigues2) << "\nExpected: " << to_string(expe2) << std::endl;


	/* Case 3 */
	const vec3 a3 = { 1.f, 0.f, 0.f };
	const vec3 v3 = { 0.f, 0.f, 1.f };
	const vec3 expe3 = { 0.f, -1.f, 0.f };

	const float angle3 = radians(90.f);

	vec3 a_norm3 = normalize(a3);

	vec3 rodrigues3 = rodrigues_rotation_formula(a3, v3, angle3);
	assert(all(epsilonEqual(rodrigues3, expe3, THRESHOLD)));
	std::cout << "Rodrigues case 3 result: " << to_string(rodrigues3) << "\nExpected: " << to_string(expe3) << std::endl;
}

void qtest_coordinate_frame() {
	/* Case1 */
	vec3 view1 = { 2.0f, 3.0f, 5.0f };
	vec3 up1 = { 0.0f, 1.0f, 0.0f };

	Coordinate_frame frame1;
	create_coordinate_frame(view1, up1, &frame1);
	assert(epsilonEqual(degrees(angle(frame1.v, frame1.u)), 90.0f, THRESHOLD));
	assert(epsilonEqual(degrees(angle(frame1.v, frame1.w)), 90.0f, THRESHOLD));
	assert(epsilonEqual(degrees(angle(frame1.u, frame1.w)), 90.0f, THRESHOLD));
	std::cout << "Coordinate Frame 1 : \nu = " 
				<< to_string(frame1.u) << "\nv = " 
				<< to_string(frame1.v) << "\nw = " 
				<< to_string(frame1.w) << std::endl;



	/* Case2 */
	vec3 view2 = { 3.0f, 2.0f, 5.0f };
	vec3 up2 = { 0.0f, 1.0f, 0.0f };

	Coordinate_frame frame2;
	create_coordinate_frame(view2, up2, &frame2);
	assert(epsilonEqual(degrees(angle(frame2.v, frame2.u)), 90.0f, THRESHOLD));
	assert(epsilonEqual(degrees(angle(frame2.v, frame2.w)), 90.0f, THRESHOLD));
	assert(epsilonEqual(degrees(angle(frame2.u, frame2.w)), 90.0f, THRESHOLD));
	std::cout << "Coordinate Frame 2 : \nu = "
		<< to_string(frame2.u) << "\nv = "
		<< to_string(frame2.v) << "\nw = "
		<< to_string(frame2.w) << std::endl;


	/* Case3 */
	vec3 view3 = { 0.0f, 1.0f, 0.0f };
	vec3 up3 = { 0.0f, 1.0f, 0.0f };

	Coordinate_frame frame3;
	create_coordinate_frame(view3, up3, &frame3);
	std::cout << "Coordinate Frame 3 : \nu = "
		<< to_string(frame3.u) << "\nv = "
		<< to_string(frame3.v) << "\nw = "
		<< to_string(frame3.w) << std::endl;
}

void qtest_matrix_inverse_distributive_property() {
	mat3 m1;
	mat3 m2;
	random_invertable_mat3(&m1, -5.0f, 5.0f);
	random_invertable_mat3(&m2, -5.0f, 5.0f);

	bool result = matrix_inverse_distributive_property(m1, m2);

	assert(result);
	std::cout << "Matrix Inverse Distributive Property Success, matrixes used were: \nm1 = " 
			  << to_string(m1) << "\nm2 = " 
			  << to_string(m2) << std::endl;
}

int main() {
	srand(time(0));
	const auto tests = { qtest_triple_product,  qtest_rodrigues_vector, qtest_coordinate_frame, qtest_matrix_inverse_distributive_property };
	for (const auto& test : tests) {
		test();
		std::cout << std::endl;
	}
	std::cout << std::endl;
	return EXIT_SUCCESS;
}