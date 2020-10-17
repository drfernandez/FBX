#include "BinaryWriter.h"
#include <ctime>

void BinaryWriter::WriteMatrix(const FbxMatrix & m)
{
	for (int h = 0; h < 4; h++)
	{
		for (int w = 0; w < 4; w++)
		{
			float value = (float)m.mData[h][w];
			write.write((char*)&value, sizeof(float));
		}
	}
}

BinaryWriter * BinaryWriter::GetInstance(void)
{
	static BinaryWriter writer;
	return &writer;
}

void BinaryWriter::Initialize(void)
{
}

bool BinaryWriter::WriteMeshToFile(const char * filename, const std::vector<Mesh>& m)
{
	write.open(filename, std::ios_base::binary | std::ios_base::out);

	if (write.is_open())
	{
		// write the number of meshes
		size_t num_meshes = m.size();
		write.write((char*)&num_meshes, sizeof(size_t));
		// loop for the number of meshes
		for (size_t j = 0; j < m.size(); j++)
		{

			size_t v_size = m[j].m_Vertices.size();
			size_t i_size = m[j].m_Indicies.size();
			// write the number of vertices
			write.write((char*)&v_size, sizeof(size_t));
			// loop for the number of vertices
			for (size_t i = 0; i < v_size; i++)
			{
				const Vertex& v = m[j].m_Vertices[i];
				float x, y, z, w = 0.0f;
				// Write the position
				x = (float)v.position.mData[0];
				y = (float)v.position.mData[1];
				z = (float)v.position.mData[2];
				w = (float)v.position.mData[3];
				write.write((char*)&x, sizeof(float));
				write.write((char*)&y, sizeof(float));
				write.write((char*)&z, sizeof(float));
				write.write((char*)&w, sizeof(float));

				// Write the color
				x = (float)v.color.mRed;
				y = (float)v.color.mGreen;
				z = (float)v.color.mBlue;
				w = (float)v.color.mAlpha;
				write.write((char*)&x, sizeof(float));
				write.write((char*)&y, sizeof(float));
				write.write((char*)&z, sizeof(float));
				write.write((char*)&w, sizeof(float));

				// Write the texture coordinates
				x = (float)v.texcoord.mData[0];
				y = (float)v.texcoord.mData[1];
				write.write((char*)&x, sizeof(float));
				write.write((char*)&y, sizeof(float));

				// Write the normals
				x = (float)v.normal.mData[0];
				y = (float)v.normal.mData[1];
				z = (float)v.normal.mData[2];
				w = (float)v.normal.mData[3];
				write.write((char*)&x, sizeof(float));
				write.write((char*)&y, sizeof(float));
				write.write((char*)&z, sizeof(float));
				write.write((char*)&w, sizeof(float));

				// Write the tangents
				x = (float)v.tangent.mData[0];
				y = (float)v.tangent.mData[1];
				z = (float)v.tangent.mData[2];
				w = (float)v.tangent.mData[3];
				write.write((char*)&x, sizeof(float));
				write.write((char*)&y, sizeof(float));
				write.write((char*)&z, sizeof(float));
				write.write((char*)&w, sizeof(float));

				// Write the binormals
				x = (float)v.binormal.mData[0];
				y = (float)v.binormal.mData[1];
				z = (float)v.binormal.mData[2];
				w = (float)v.binormal.mData[3];
				write.write((char*)&x, sizeof(float));
				write.write((char*)&y, sizeof(float));
				write.write((char*)&z, sizeof(float));
				write.write((char*)&w, sizeof(float));

				// write the joint indicies
				x = (float)v.joint.mData[0];
				y = (float)v.joint.mData[1];
				z = (float)v.joint.mData[2];
				w = (float)v.joint.mData[3];
				write.write((char*)&x, sizeof(float));
				write.write((char*)&y, sizeof(float));
				write.write((char*)&z, sizeof(float));
				write.write((char*)&w, sizeof(float));

				// write the weights
				x = (float)v.weight.mData[0];
				y = (float)v.weight.mData[1];
				z = (float)v.weight.mData[2];
				w = (float)v.weight.mData[3];
				write.write((char*)&x, sizeof(float));
				write.write((char*)&y, sizeof(float));
				write.write((char*)&z, sizeof(float));
				write.write((char*)&w, sizeof(float));

			}// end loop for vertices

			// write the number of indicies
			write.write((char*)&i_size, sizeof(size_t));
			// write the list of indices
			write.write((char*)m[j].m_Indicies.data(), i_size * sizeof(unsigned int));

			size_t size = 0;
			// write the number of characters for the diffuse texture
			size = m[j].m_TextureDiffuseName.Size();
			write.write((char*)&size, sizeof(size_t));
			// write the string for the diffuse texture
			write.write(m[j].m_TextureDiffuseName.Buffer(), size);
			// write the number of characters for the normal texture
			size = m[j].m_TextureNormalName.Size();
			write.write((char*)&size, sizeof(size_t));
			// write the string for the normal texture
			write.write(m[j].m_TextureNormalName.Buffer(), size);
			// write the number of characters for the specular texture
			size = m[j].m_TextureSpecularName.Size();
			write.write((char*)&size, sizeof(size_t));
			// write the string for the specular texture
			write.write(m[j].m_TextureSpecularName.Buffer(), size);

		}// end loop for meshes

		// close the file
		write.close();
		return true;
	}
	
	return false;
}

bool BinaryWriter::WriteBindPoseToFile(const char * filename, const std::vector<Joint> & j)
{
	write.open(filename, std::ios_base::binary | std::ios_base::out);

	if (write.is_open())
	{
		// write the number of joints in the vector
		size_t num_items = j.size();
		write.write((char*)&num_items, sizeof(size_t));
		for (size_t i = 0; i < num_items; i++)
		{
			const Joint& w = j[i];
			// matrix
			WriteMatrix(w.matrix);
			// name
			size_t name_size = w.name.Size();
			write.write((char*)&name_size, sizeof(size_t));
			write.write((char*)w.name.Buffer(), name_size);
			// parent_index
			size_t p_index = w.parent_index;
			write.write((char*)&p_index, sizeof(size_t));
		}
		write.close();
		return true;
	}

	return false;
}

bool BinaryWriter::WriteAnimationsToFile(const char * filename, const std::vector<Animation> & a)
{
	write.open(filename, std::ios_base::binary | std::ios_base::out);

	if (write.is_open())
	{
		// write the number of animations
		size_t num_animations = a.size();
		write.write((char*)&num_animations, sizeof(size_t));
		for (size_t i = 0; i < num_animations; i++)
		{
			const Animation& temp_anim = a[i];
			// anim.name
			size_t anim_name_size = temp_anim.name.Size();
			write.write((char*)&anim_name_size, sizeof(size_t));
			write.write((char*)temp_anim.name.Buffer(), anim_name_size);
			// anim.duration
			float duration = (float)temp_anim.duration;
			write.write((char*)&duration, sizeof(float));
			// anim.keyframes
			size_t num_keyframes = temp_anim.keyframes.size();
			write.write((char*)&num_keyframes, sizeof(size_t));
			for (size_t j = 0; j < num_keyframes; j++)
			{
				const KeyFrame& temp_keyframe = temp_anim.keyframes[j];
				// kf.time;
				float time = (float)temp_keyframe.time;
				write.write((char*)&time, sizeof(float));
				// kf.joints;
				size_t num_joints = temp_keyframe.joints.size();
				write.write((char*)&num_joints, sizeof(size_t));
				for (size_t k = 0; k < num_joints; k++)
				{
					const Joint& temp_joint = temp_keyframe.joints[k];
					//temp_joint.name;
					size_t joint_name_size = temp_joint.name.Size();
					write.write((char*)&joint_name_size, sizeof(size_t));
					write.write((char*)temp_joint.name.Buffer(), joint_name_size);
					//temp_joint.matrix;
					WriteMatrix(temp_joint.matrix);
					//temp_joint.parent_index;
					int temp_joint_parent_index = temp_joint.parent_index;
					write.write((char*)&temp_joint_parent_index, sizeof(int));
				}
			}
		}
		write.close();
		return true;
	}
	return false;
}

void BinaryWriter::Shutdown(void)
{

}
