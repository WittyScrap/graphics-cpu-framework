#include "MD2Loader.h"

// File reading
#include <iostream>
#include <fstream>

using namespace std;

// BYTE added in case Windows.h is not included.
typedef unsigned char BYTE; 

// Magic number for MD2 files  "IDP2" or 844121161
const int MD2_IDENT = (('2'<<24) + ('P'<<16) + ('D'<<8) + 'I');

// MS2 version
const int MD2_VERSION = 8;

struct Md2Header
{
		int indent;               // The magic number used to identify the file.
		int version;              // The file version number (must be 8).
		int skinWidth;            // The width in pixels of our image.
		int skinHeight;           // The height in pixels of our image.
		int frameSize;            // The size in bytes the frames are.
		int numSkins;             // The number of skins associated with the model.
		int numVertices;		  // The number of vertices.
		int numTexCoords;		  // The number of texture coordinates.
		int numTriangles;		  // The number of faces (polygons).
		int numGlCommands;        // The number of gl commands.
		int numFrames;			  // The number of animated frames.
		int offsetSkins;		  // The offset in the file for the skin data.
		int offsetTexCoords;	  // The offset in the file for the texture data.
		int offsetTriangles;	  // The offset in the file for the face data.
		int offsetFrames;		  // The offset in the file for the frames data.
		int offsetGlCommands;	  // The offset in the file for the gl commands data.
		int offsetEnd;            // The end of the file offset.
};

struct Md2Triangle
{
	short vertexIndex[3];   // Vertex indices of the triangle
	short uvIndex[3];       // Texture coordinate indices 
};

struct Md2Vertex
{
	BYTE v[3];                // Compressed vertex (x, y, z) coordinates
	BYTE lightNormalIndex;    // Index to a normal vector for the lighting
};

struct Md2Frame
{
	float       scale[3];       // Scale values
	float       translate[3];   // Translation vector
	char        name[16];       // Frame name
	Md2Vertex   verts[1];       // First vertex of this frame
};

// ----------------------------------------------
// LoadModel() - load model from file.
// ----------------------------------------------

bool MD2Loader::LoadModel(const char* md2Filename, Mesh& model, AddPolygon addPolygon, AddVertex addVertex)
{
	ifstream   file;           
	Md2Header header;

	// Try to open MD2 file
	file.open(md2Filename, ios::in | ios::binary);
	if (file.fail())
	{
		return false;
	}
	// Read file header
	file.read(reinterpret_cast<char*>(&header), sizeof(Md2Header));
		
	// Verify that this is a MD2 file (check for the magic number and version number)
	if ((header.indent != MD2_IDENT) && (header.version != MD2_VERSION))
	{
		// This is not a MD2 model
		file.close();
		return false;
	}

	// Allocate the memory we need
	Md2Triangle* triangles = new Md2Triangle[header.numTriangles];
	// We are only interested in the first frame 
	BYTE* frameBuffer = new BYTE[header.frameSize];
	Md2Frame* frame = reinterpret_cast<Md2Frame*>(frameBuffer);

	// Read polygon data...
	file.seekg(header.offsetTriangles, ios::beg);
	file.read(reinterpret_cast<char*>(triangles), sizeof(Md2Triangle) * header.numTriangles);	
		
	// Read frame data...
	file.seekg(header.offsetFrames, ios::beg);
	file.read(reinterpret_cast<char*>(frame), header.frameSize);	
		
	// Close the file 
	file.close();

	//----------------------------------------------------------------------------------------------

	// Polygon array initialization
	for ( int i = 0; i < header.numTriangles; ++i )
	{
		// Call supplied member function to add a new polygon to the list
		std::invoke(addPolygon, model, triangles[i].vertexIndex[0], triangles[i].vertexIndex[1], triangles[i].vertexIndex[2]);
	}

	// Vertex array initialization
	for( int i = 0; i < header.numVertices; ++i )
	{
		// The following are the expressions needed to access each of the co-ordinates.
		// 
		// X co-ordinate:   frame->verts[i].v[0] * frame->scale[0] + frame->translate[0]
		// Y co-ordinate:   frame->verts[i].v[2] * frame->scale[2] + frame->translate[2]
		// Z co-ordinate:   frame->verts[i].v[1] * frame->scale[1] + frame->translate[1]
		//
		// NOTE: We have to swap Y and Z over because Z is up in MD2 and we have Y as up-axis
		std::invoke(addVertex, model, 
					static_cast<float>((frame->verts[i].v[0] * frame->scale[0]) + frame->translate[0]),
					static_cast<float>((frame->verts[i].v[2] * frame->scale[2]) + frame->translate[2]),
					static_cast<float>((frame->verts[i].v[1] * frame->scale[1]) + frame->translate[1]));
	}

	// Free dynamically allocated memory
	delete [] triangles; // NOTE: this is 'array' delete. Must be sure to use this
	triangles = 0;

	delete [] frameBuffer;
	frameBuffer = 0;
	frame = 0;

	return true;
}
