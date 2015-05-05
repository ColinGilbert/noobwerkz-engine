#include "Graphics.hpp"

bgfx::VertexDecl noob::graphics::pos_norm_uv_bones_vertex::ms_decl;

std::map<std::string, bgfx::TextureInfo*> noob::graphics::global_textures;
std::map<std::string, bgfx::ProgramHandle> noob::graphics::programs;
