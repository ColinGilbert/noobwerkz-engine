
#include "clipper/clipper.hpp"
#include "poly2tri/poly2tri.h"
#include "clip2tri/clip2tri.h"

#include <iostream>
#include <cstdio>

// TODO actually fill out some test data...  this code compiles, but doesn't work!
int main()
{
   std::vector<std::vector<c2t::Point>> input_polygons;
   std::vector<c2t::Point> output_triangles;  // Every 3 points is a triangle
   std::vector<c2t::Point> bounding_polygon;

   c2t::clip2tri clip2tri;
   clip2tri.triangulate(input_polygons, output_triangles, bounding_polygon);
}
