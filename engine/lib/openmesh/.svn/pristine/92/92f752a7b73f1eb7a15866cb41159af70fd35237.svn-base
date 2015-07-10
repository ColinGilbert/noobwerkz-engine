/* ========================================================================= *
 *                                                                           *
 *                               OpenMesh                                    *
 *           Copyright (c) 2001-2015, RWTH-Aachen University                 *
 *           Department of Computer Graphics and Multimedia                  *
 *                          All rights reserved.                             *
 *                            www.openmesh.org                               *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * This file is part of OpenMesh.                                            *
 *---------------------------------------------------------------------------*
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 *                                                                           *
 * 1. Redistributions of source code must retain the above copyright notice, *
 *    this list of conditions and the following disclaimer.                  *
 *                                                                           *
 * 2. Redistributions in binary form must reproduce the above copyright      *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. Neither the name of the copyright holder nor the names of its          *
 *    contributors may be used to endorse or promote products derived from   *
 *    this software without specific prior written permission.               *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED *
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              *
 *                                                                           *
 * ========================================================================= */

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/


//=============================================================================
//
//  Implements a writer module for PLY files
//
//=============================================================================


#ifndef __PLYWRITER_HH__
#define __PLYWRITER_HH__


//=== INCLUDES ================================================================


#include <stdio.h>
#include <string>
#include <fstream>

#include <OpenMesh/Core/System/config.h>
#include <OpenMesh/Core/Utils/SingletonT.hh>
#include <OpenMesh/Core/IO/exporter/BaseExporter.hh>
#include <OpenMesh/Core/IO/writer/BaseWriter.hh>


//== NAMESPACES ===============================================================


namespace OpenMesh {
namespace IO {


//=== IMPLEMENTATION ==========================================================


/**
    Implementation of the PLY format writer. This class is singleton'ed by
    SingletonT to PLYWriter.

    currently supported options:
    - VertexColors
    - Binary
    - Binary -> MSB
*/
class OPENMESHDLLEXPORT _PLYWriter_ : public BaseWriter
{
public:

  _PLYWriter_();

  /// Destructor
  virtual ~_PLYWriter_() {};

  std::string get_description() const { return "PLY polygon file format"; }
  std::string get_extensions() const  { return "ply"; }

  bool write(const std::string&, BaseExporter&, Options, std::streamsize _precision = 6) const;

  bool write(std::ostream&, BaseExporter&, Options, std::streamsize _precision = 6) const;

  size_t binary_size(BaseExporter& _be, Options _opt) const;

  enum ValueType {
    Unsupported ,
    ValueTypeFLOAT32, ValueTypeFLOAT,
    ValueTypeUINT8, ValueTypeINT32, ValueTypeINT ,
    ValueTypeUCHAR
  };

private:
  mutable Options options_;

protected:
  void writeValue(ValueType _type, std::ostream& _out, int value) const;
  void writeValue(ValueType _type, std::ostream& _out, unsigned int value) const;
  void writeValue(ValueType _type, std::ostream& _out, float value) const;

  bool write_ascii(std::ostream& _out, BaseExporter&, Options) const;
  bool write_binary(std::ostream& _out, BaseExporter&, Options) const;
  void write_header(std::ostream& _out, BaseExporter& _be, Options& _opt) const;
};


//== TYPE DEFINITION ==========================================================


/// Declare the single entity of the PLY writer.
extern _PLYWriter_  __PLYWriterInstance;
OPENMESHDLLEXPORT _PLYWriter_& PLYWriter();


//=============================================================================
} // namespace IO
} // namespace OpenMesh
//=============================================================================
#endif
//=============================================================================
