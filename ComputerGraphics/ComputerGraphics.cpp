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

void generate_random_mat3(mat3* m, float min = -5.0f, float max = 5.0f) {
	vec3 col1;
	vec3 col2;
	vec3 col3;

	generate_random_vec3(&col3, min, max);
	generate_random_vec3(&col2, min, max);
	generate_random_vec3(&col1, min, max);
	*m = mat3(col1, col2, col3);
}

void random_invertable_mat3(mat3* m, float min = -5.0f, float max = 5.0f) {
	do {
		generate_random_mat3(m, min, max);
	} while (determinant(*m) == 0);
}

void create_coordinate_frame(vec3 view, vec3 up, Coordinate_frame* frame) {
	frame->v = normalize(view);
	frame->w = normalize(cross(up, frame->v));
	frame->u = normalize(cross(frame->v, frame->w));
}

bool compare_matrix(mat3 m1, mat3 m2) {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			if (epsilonNotEqual(m1[i][j], m1[i][j], THRESHOLD)) {
				return false;
			}
		}
	}
	return true;
}

bool matrix_inverse_distributive_property(mat3 m1, mat3 m2) {
	mat3 left = inverse(m1 * m2);
	mat3 right = inverse(m2) * inverse(m1);

	std::cout << "Inverse distributive property.\n"
		<< "Left side result (inv(m1 * m2)) = "
		<< to_string(left) << "\nRight side result (inv(m2) * inv(m1)) = "
		<< to_string(right) << std::endl;

	return compare_matrix(left, right);
}

vec3 rodrigues_vector_rotation_formula(vec3 axis, vec3 vector, float rads) {
	vec3 axis_norm = normalize(axis);

	return vector * cos(rads) +
		(cross(axis_norm, vector)) * sin(rads) +
		axis_norm * (dot(axis_norm, vector)) * (1 - cos(rads));
}

mat3 rodrigues_matrix_rotation_formula(mat3 axis, float rads) {
	return mat3(1) + sin(rads) * axis + (1 - cos(rads)) * axis * axis;
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

	vec3 rodrigues1 = rodrigues_vector_rotation_formula(a1, v1, angle1);
	assert(all(epsilonEqual(rodrigues1, expe1, THRESHOLD)));
	std::cout << "Rodrigues case 1 result: " << to_string(rodrigues1) << "\nExpected: " << to_string(expe1) << std::endl;

	/* Case 2 */
	const vec3 a2 = { 0.f, 1.f, 0.f };
	const vec3 v2 = { 1.f, 0.f, 0.f };
	const vec3 expe2 = { 0.f, 0.f, -1.f };

	const float angle2 = radians(90.f);

	vec3 a_norm2 = normalize(a2);

	vec3 rodrigues2 = rodrigues_vector_rotation_formula(a2, v2, angle2);
	assert(all(epsilonEqual(rodrigues2, expe2, THRESHOLD)));
	std::cout << "Rodrigues case 2 result: " << to_string(rodrigues2) << "\nExpected: " << to_string(expe2) << std::endl;


	/* Case 3 */
	const vec3 a3 = { 1.f, 0.f, 0.f };
	const vec3 v3 = { 0.f, 0.f, 1.f };
	const vec3 expe3 = { 0.f, -1.f, 0.f };

	const float angle3 = radians(90.f);

	vec3 a_norm3 = normalize(a3);

	vec3 rodrigues3 = rodrigues_vector_rotation_formula(a3, v3, angle3);
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

void qtest_rodrigues_matrix_rotation() {
	//90º rotatation though x,y,z sequencially in all cases. Just to clarify
	float xaxis[9] = { 0, 0, 0, 0, 0, -1, 0, 1, 0 }, yaxis[9] = { 0,0,1,0,0,0,-1,0,0 }, zaxis[9] = { 0,-1,0,1,0,0,0,0,0 };
	mat3 matx = make_mat3(xaxis), maty = make_mat3(yaxis), matz = make_mat3(zaxis);

	//values to be used as the matrix to be rotated
	float xmat[9] = { 1,0,0,0,0,0,0,0,0 }, ymat[9] = { 0,0,0,0,1,0,0,0,0 }, zmat[9] = { 1,0,0,0,0,0,0,0,0 };
	mat3 rotx = make_mat3(xmat), roty = make_mat3(ymat), rotz = make_mat3(zmat);

	// xyz order means negative,otherwise it's positive. expected axis is the one that wasn't present yet
	float expxy[9] = { 0,0,0,0,0,0,0,0,-1 }, expxz[9] = { 0,0,0,0,1,0,0,0,0 }, expyz[9] = { -1,0,0,0,0,0,0,0,0 },
		expyx[9] = { 0,0,0,0,0,0,0,0,1 }, expzx[9] = { 0,0,0,0,-1,0,0,0,0 }, expzy[9] = { 1,0,0,0,0,0,0,0,0 };
	mat3 expXY = make_mat3(expxy), expXZ = make_mat3(expxz), expYZ = make_mat3(expyz),
		expYX = make_mat3(expyx), expZX = make_mat3(expzx), expZY = make_mat3(expzy);

	const float angle = radians(90.f);

	mat3 rodriguesX = rodrigues_matrix_rotation_formula(matx, angle),
		rodriguesY = rodrigues_matrix_rotation_formula(maty, angle),
		rodriguesZ = rodrigues_matrix_rotation_formula(matz, angle);

	assert(compare_matrix(rotx * rodriguesX, expXY));
	std::cout << "Rodrigues case XY result: " << to_string(rotx * rodriguesY) << "\nExpected: " << to_string(expXY) << std::endl;
	assert(compare_matrix(rotx * rodriguesZ, expXZ));
	std::cout << "Rodrigues case XZ result: " << to_string(rotx * rodriguesZ) << "\nExpected: " << to_string(expXZ) << std::endl;
	assert(compare_matrix(roty * rodriguesZ, expYZ));
	std::cout << "Rodrigues case YZ result: " << to_string(roty * rodriguesZ) << "\nExpected: " << to_string(expYZ) << std::endl;
	assert(compare_matrix(roty * rodriguesX, expYX));
	std::cout << "Rodrigues case YX result: " << to_string(roty * rodriguesX) << "\nExpected: " << to_string(expYX) << std::endl;
	assert(compare_matrix(rotz * rodriguesX, expZX));
	std::cout << "Rodrigues case ZX result: " << to_string(rotz * rodriguesX) << "\nExpected: " << to_string(expZX) << std::endl;
	assert(compare_matrix(rotz * rodriguesY, expZY));
	std::cout << "Rodrigues case ZY result: " << to_string(rotz * rodriguesY) << "\nExpected: " << to_string(expZY) << std::endl;
}

void qtest_matrix_transpose_property() {
	mat3 m1, m2;
	generate_random_mat3(&m1, -5.0f, 5.0f);
	generate_random_mat3(&m2, -5.0f, 5.0f);

	mat3 left_side = transpose(m1 * m2);
	mat3 right_side = transpose(m2) * transpose(m1);

	std::cout << "Transposition property.\n"
		<< "Left side result ( (A*B)^T ) = "
		<< to_string(left_side) << "\nRight side result ( B^T * A^T ) = "
		<< to_string(right_side) << std::endl;

	assert(compare_matrix(left_side, right_side));

	std::cout << "Matrix Transposition Property Success, matrixes used were: \nm1 = "
		<< to_string(m1) << "\nm2 = "
		<< to_string(m2) << std::endl;
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
	const auto tests = { qtest_triple_product, qtest_coordinate_frame, qtest_rodrigues_vector, qtest_rodrigues_matrix_rotation ,qtest_matrix_transpose_property ,qtest_matrix_inverse_distributive_property };
	for (const auto& test : tests) {
		test();
		std::cout << std::endl;
	}
	std::cout << std::endl;
	return EXIT_SUCCESS;
}
