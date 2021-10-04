// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_IO_H_
#define _SCENEPIC_IO_H_

#include <string>
#include <iostream>
#include <memory>

#include "mesh_info.h"

namespace scenepic
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