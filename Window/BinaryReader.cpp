#include "BinaryReader.h"

BinaryReader * BinaryReader::GetInstance(void)
{
	static BinaryReader m_BinaryReader;
	return &m_BinaryReader;
}

void BinaryReader::Initialize(void)
{
	// nothing needed
}

bool BinaryReader::ReadMeshFromFile(const char * filename, std::vector<Mesh*>& m)
{
	// open the file
	read.open(filename, std::ios_base::binary | std::ios_base::in);

	// if the file is open
	if (read.is_open())
	{
		// store the number of meshes in the file
		size_t num_meshes = 0;
		read.read((char*)&num_meshes, sizeof(size_t));
		// resize the vector for easier indexing
		//m.resize(num_meshes);
		// loop for the number of meshes
		for (size_t j = 0; j < num_meshes; j++)
		{
			Mesh* temp_mesh = new Mesh();

			size_t v_size = 0;
			size_t i_size = 0;
			std::vector<Vertex> verts;
			std::vector<unsigned int> inds;
			// read the number of vertices
			read.read((char*)&v_size, sizeof(size_t));
			// loop for the number of vertices
			for (size_t i = 0; i < v_size; i++)
			{
				Vertex t;
				// read the position
				read.read((char*)&t.position.x, sizeof(float));
				read.read((char*)&t.position.y, sizeof(float));
				read.read((char*)&t.position.z, sizeof(float));
				read.read((char*)&t.position.w, sizeof(float));
				// read the color
				read.read((char*)&t.color.x, sizeof(float));
				read.read((char*)&t.color.y, sizeof(float));
				read.read((char*)&t.color.z, sizeof(float));
				read.read((char*)&t.color.w, sizeof(float));
				// read the texcoord
				read.read((char*)&t.texcoord.x, sizeof(float));
				read.read((char*)&t.texcoord.y, sizeof(float));
				// read the normals
				read.read((char*)&t.normal.x, sizeof(float));
				read.read((char*)&t.normal.y, sizeof(float));
				read.read((char*)&t.normal.z, sizeof(float));
				read.read((char*)&t.normal.w, sizeof(float));
				// read the tangents
				read.read((char*)&t.tangent.x, sizeof(float));
				read.read((char*)&t.tangent.y, sizeof(float));
				read.read((char*)&t.tangent.z, sizeof(float));
				read.read((char*)&t.tangent.w, sizeof(float));
				// read the binormals
				read.read((char*)&t.binormal.x, sizeof(float));
				read.read((char*)&t.binormal.y, sizeof(float));
				read.read((char*)&t.binormal.z, sizeof(float));
				read.read((char*)&t.binormal.w, sizeof(float));
				// read the joint indicies
				read.read((char*)&t.joint.x, sizeof(float));
				read.read((char*)&t.joint.y, sizeof(float));
				read.read((char*)&t.joint.z, sizeof(float));
				read.read((char*)&t.joint.w, sizeof(float));
				// read the weights
				read.read((char*)&t.weight.x, sizeof(float));
				read.read((char*)&t.weight.y, sizeof(float));
				read.read((char*)&t.weight.z, sizeof(float));
				read.read((char*)&t.weight.w, sizeof(float));
				// push into the vertex list at the current mesh iteration
				//m[j].m_Vertices.push_back(t);
				temp_mesh->m_Vertices.push_back(t);
			} // end number of vertices

			// read the number of indicies
			read.read((char*)&i_size, sizeof(size_t));
			// loop for the number of indicies
			for (size_t i = 0; i < i_size; i++)
			{
				unsigned int t;
				// read the index
				read.read((char*)&t, sizeof(unsigned int));
				// push into the index list at the current mesh iteration
				//m[j].m_Indicies.push_back(t);
				temp_mesh->m_Indicies.push_back(t);
			} // end number of indicies

			size_t size = 0;
			char* buffer = nullptr;
			// read the number of characters for the diffuse texture
			read.read((char*)&size, sizeof(size_t));
			// read the string for the diffuse texture
			buffer = new char[size+1];
			read.read(buffer, size);
			buffer[size] = '\0';
			temp_mesh->m_TextureDiffuseName = buffer;
			delete[] buffer;
			// read the number of characters for the normal texture
			read.read((char*)&size, sizeof(size_t));
			// read the string for the normal texture
			buffer = new char[size+1];
			read.read(buffer, size);
			buffer[size] = '\0';
			temp_mesh->m_TextureNormalName = buffer;
			delete[] buffer;
			// read the number of characters for the specular texture
			read.read((char*)&size, sizeof(size_t));
			// read the string for the specular texture
			buffer = new char[size+1];
			read.read(buffer, size);
			buffer[size] = '\0';
			temp_mesh->m_TextureSpecularName = buffer;
			delete[] buffer;

			m.push_back(temp_mesh);
		} // end number of meshes

		// close the file
		read.close();
		// success!
		return true;
	}
	else
	{
		// failure to open the file
		return false;
	}
}

bool BinaryReader::ReadBindPoseFromFile(const char * filename, std::vector<Joint*> & j)
{
	read.open(filename, std::ios_base::binary | std::ios_base::in);

	if (read.is_open())
	{
		size_t num_items = 0;
		read.read((char*)&num_items, sizeof(size_t));
		//j.resize(num_items);

		for (size_t i = 0; i < num_items; i++)
		{
			Joint* temp_joint = new Joint();

			read.read((char*)&temp_joint->matrix, sizeof(DirectX::XMMATRIX));

			size_t name_size = 0;
			read.read((char*)&name_size, sizeof(size_t));
			char* buffer = new char[name_size+1];
			read.read(buffer, name_size);
			buffer[name_size] = '\0';
			temp_joint->name = buffer;
			delete[] buffer;

			read.read((char*)&temp_joint->parent_index, sizeof(size_t));

			j.push_back(temp_joint);
		}

		read.close();
		return true;
	}

	return false;
}

bool BinaryReader::ReadAnimationsFromFile(const char * filename, std::vector<Animation*>& a)
{
	read.open(filename, std::ios_base::binary | std::ios_base::in);

	if (read.is_open())
	{
		// read the number of animations
		size_t num_animations = 0;
		read.read((char*)&num_animations, sizeof(size_t));
		for (size_t i = 0; i < num_animations; i++)
		{
			// temporary animation
			Animation* temp_animation = new Animation();
			// animation name
			size_t anim_name_size = 0;
			read.read((char*)&anim_name_size, sizeof(size_t));
			char* anim_name_buffer = new char[anim_name_size + 1];
			read.read(anim_name_buffer, anim_name_size);
			anim_name_buffer[anim_name_size] = '\0';
			temp_animation->name = anim_name_buffer;
			delete[] anim_name_buffer;
			// animation duration
			read.read((char*)&temp_animation->duration, sizeof(float));
			// animation keyframes
			size_t anim_num_keyframes = 0;
			read.read((char*)&anim_num_keyframes, sizeof(size_t));
			//temp_animation.keyframes.resize(anim_num_keyframes);
			for (size_t j = 0; j < anim_num_keyframes; j++)
			{
				// temporary keyframe
				KeyFrame* temp_keyframe = new KeyFrame();
				// keyframe time
				read.read((char*)&temp_keyframe->time, sizeof(float));
				// number of joints in the keyframe
				size_t num_joints = 0;
				read.read((char*)&num_joints, sizeof(size_t));
				for (size_t k = 0; k < num_joints; k++)
				{
					// temporary joint
					Joint* temp_joint = new Joint();
					// joint name
					size_t joint_name_size = 0;
					read.read((char*)&joint_name_size, sizeof(size_t));
					char* joint_name_buffer = new char[joint_name_size + 1];
					read.read(joint_name_buffer, joint_name_size);
					joint_name_buffer[joint_name_size] = '\0';
					temp_joint->name = joint_name_buffer;
					delete[] joint_name_buffer;
					// joint matrix
					read.read((char*)&temp_joint->matrix, sizeof(DirectX::XMMATRIX));
					// joint parent index
					read.read((char*)&temp_joint->parent_index, sizeof(int));
					// push back into the keyframe list
					temp_keyframe->joints.push_back(temp_joint);
				}
				// push back into the animations keyframe list
				temp_animation->keyframes.push_back(temp_keyframe);
			}
			// push back into the animation list
			a.push_back(temp_animation);
		}
		read.close();
		return true;
	}
	return false;
}

void BinaryReader::Shutdown(void)
{

}
