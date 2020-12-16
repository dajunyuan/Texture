#include "mesh.h"
#include <stdio.h>
#include<sstream>
#include <fstream>
#include <iosfwd>
#include <algorithm>
#include <gl/GL.h>
#include <math.h>
#include <algorithm>


My_Mesh::My_Mesh()
{

	vTranslation[0] = Theta[0] = 0;
	vTranslation[1] = Theta[1] = 0;
	vTranslation[2] = Theta[2] = 0;
	Theta[0] = 45;
}


My_Mesh::~My_Mesh()
{
	
}


void My_Mesh::load_obj(std::string obj_File)
{
	//请在此添加代码实现对含有UV坐标的obj文件的读取
	m_vertices_.clear();
	m_color_list_.clear();
	m_vt_list_.clear();
	m_normals_.clear();
	// -------------清理vector
	//
	std::fstream fs;
	char str[256];
	// 位置或颜色
	GLfloat x, y, z;
	// 索引
	GLint a, b, c;
	// 最大范围
	GLfloat max_x, max_y, max_z;
	// 最小范围
	GLfloat min_x, min_y, min_z;
	max_x = max_y = max_z = -1.;
	min_x = min_y = min_z = 1.;
	fs.open(obj_File, std::fstream::in);
	if(!fs.is_open()) {
		std::cout << "wrong open file" << std::endl;
		return;
	}

	while (!fs.eof()) {
		fs.getline(str, 250);

		std::stringstream ss(str);
		std::string s;

		// 读取文件
		std::getline(ss, s, ' ');
		if (s == "v") {
			// 顶点坐标
			s = ss.str();
			sscanf_s(s.data(), "%f%f%f", &x, &y, &z);


			m_vertices_.push_back(x);
			m_vertices_.push_back(y);
			m_vertices_.push_back(z);

			min_x = std::min(min_x, x);
			min_y = std::min(min_y, y);
			min_z = std::min(min_z, z);

			max_x = std::max(max_x, x);
			max_y = std::max(max_y, y);
			max_z = std::max(max_z, z);
		}
		else if (s == "vt") {
			// 贴图坐标
			s = ss.str();
			sscanf_s(s.data(), "%f%f%f", &x, &y, &z);
			m_vt_list_.push_back(x);
			m_vt_list_.push_back(y);
			m_vt_list_.push_back(z);
		}
		else if (s == "vn") {
			// 法线
			s = ss.str();
			sscanf_s(s.data(), "%f%f%f", &x, &y, &z);

			m_normals_.push_back(x);
			m_normals_.push_back(y);
			m_normals_.push_back(z);

			normal_to_color(x, y, z, x, y, z);
			m_color_list_.push_back(x);
			m_color_list_.push_back(y);
			m_color_list_.push_back(z);
		}
		else if (s == "f") {
			std::getline(ss, s, ' ');
			sscanf_s(s.data(), "%d/%d/%d", &a, &b, &c);

			--a, --b, --c;
			m_faces_.push_back(vec3(a, b, c));

			std::getline(ss, s, ' ');
			sscanf_s(s.data(), "%d/%d/%d", &a, &b, &c);

			--a, --b, --c;
			m_faces_.push_back(vec3(a, b, c));

			std::getline(ss, s, ' ');
			sscanf_s(s.data(), "%d/%d/%d", &a, &b, &c);

			--a, --b, --c;
			m_faces_.push_back(vec3(a, b, c));
		}

	}

	fs.close();

	// 确定边界
	m_center_ = point3f(0, 0, 0);
	m_min_box_ = point3f(min_x, min_y, min_z);
	m_max_box_ = point3f(max_x, max_y, max_z);

};

void My_Mesh::normal_to_color(float nx, float ny, float nz, float& r, float& g, float& b)
{
	r = float(std::min(std::max(0.5 * (nx + 1.0), 0.0), 1.0));
	g = float(std::min(std::max(0.5 * (ny + 1.0), 0.0), 1.0));
	b = float(std::min(std::max(0.5 * (nz + 1.0), 0.0), 1.0));
};

const VtList&  My_Mesh::get_vts()
{
	return this->m_vt_list_;
};
void My_Mesh::clear_data()
{
	m_vertices_.clear();
	m_normals_.clear();
	m_faces_.clear();
	m_color_list_.clear();
	m_vt_list_.clear();
};
void My_Mesh::get_boundingbox(point3f& min_p, point3f& max_p) const
{
	min_p = this->m_min_box_;
	max_p = this->m_max_box_;
};
const STLVectorf&  My_Mesh::get_colors()
{
	return this->m_color_list_;
};
const VertexList& My_Mesh::get_vertices()
{
	return this->m_vertices_;
};
const NormalList& My_Mesh::get_normals()
{
	return this->m_normals_;
};
const FaceList&   My_Mesh::get_faces()
{
	return this->m_faces_;
};

int My_Mesh::num_faces()
{
	return this->m_faces_.size()/3;
};
int My_Mesh::num_vertices()
{
	return this->m_vertices_.size()/3;
};

const point3f& My_Mesh::get_center()
{
	return this->m_center_;
};


void My_Mesh::generate_cylinder(int num_division, float height)
{
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-1, -1, -height);
	this->m_max_box_ = point3f(1, 1, height);

	int num_samples = num_division;
	float z = -height;
	float pi = 3.14159265;
	float step = 1.0 * 360 / num_samples;
	float rr = pi / 180;
	//圆柱体Z轴向上，按cos和sin生成x，y坐标
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step * rr;
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//法线由里向外
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		//这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}

	z = height;
	//圆柱体Z轴向上，按cos和sin生成x，y坐标，
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step * rr;
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//法线由里向外
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
		//这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
	}

	for (int i = 0; i < num_samples; i++)
	{
		m_faces_.push_back(i);
		m_faces_.push_back((i + 1) % num_samples);
		m_faces_.push_back((i + num_samples) % (num_samples)+num_samples);

		m_faces_.push_back((i + num_samples) % (num_samples)+num_samples);
		m_faces_.push_back((i + 1) % num_samples);
		m_faces_.push_back((i + num_samples + 1) % (num_samples)+num_samples);
		//生成三角面片

		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(0.0);
		//纹理坐标
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(0.0);
		//纹理坐标
		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(1.0);
		//纹理坐标

		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(1.0);
		//纹理坐标
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(0.0);
		//纹理坐标
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(1.0);
		//纹理坐标
	}

};


void My_Mesh::set_texture_file(std::string s)
{
	this->texture_file_name = s;
};
std::string My_Mesh::get_texture_file()
{
	return this->texture_file_name;
};

void My_Mesh::set_translate(float x, float y, float z)
{
	vTranslation[0] = x;
	vTranslation[1] = y;
	vTranslation[2] = z;

};
void My_Mesh::get_translate(float& x, float& y, float& z)
{
	x = vTranslation[0];
	y = vTranslation[1];
	z = vTranslation[2];
};

void My_Mesh::set_theta(float x, float y, float z)
{
	Theta[0] = x;
	Theta[1] = y;
	Theta[2] = z;
};
void My_Mesh::get_theta(float& x, float& y, float& z)
{
	x = Theta[0];
	y = Theta[1];
	z = Theta[2];
};

void My_Mesh::set_theta_step(float x, float y, float z)
{
	Theta_step[0] = x;
	Theta_step[1] = y;
	Theta_step[2] = z;
};

void My_Mesh::add_theta_step()
{
	Theta[0] = Theta[0] + Theta_step[0];
	Theta[1] = Theta[1] + Theta_step[1];
	Theta[2] = Theta[2] + Theta_step[2];
};
