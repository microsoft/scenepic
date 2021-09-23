// ----------------------------------------------------------------------------
//
// io.h -- Code for loading Wavefront OBJ files
//
// Copyright (C) 2019 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#ifndef _SP_IO_H_
#define _SP_IO_H_

#include <string>
#include <iostream>
#include <memory>

#include "mesh_info.h"

namespace svt
{
/** Load the geometry of a WaveFront OBJ file from a location on disk.
 * 
 *  \param path location of the OBJ on disk
 *
 *  \return the mesh information from the OBJ
 */
std::shared_ptr<MeshInfo> load_obj(const std::string &path);

/** Load the geometry of a WaveFront OBJ file from a stream.
 * 
 *  \param stream an input stream containing data in OBJ format
 *
 *  \return the mesh information from the OBJ
 */
std::shared_ptr<MeshInfo> load_obj(std::istream &stream);
}

#endif