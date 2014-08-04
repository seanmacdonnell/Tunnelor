//  Copyright 2012 Sean MacDonnell
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include "Geometry_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Geometry_Helper::Geometry_Helper() {
}

//------------------------------------------------------------------------------
Geometry_Helper::~Geometry_Helper() {
}

//------------------------------------------------------------------------------
bool Geometry_Helper::DoTheseLinesIntersect(D3DXVECTOR2 line_a_begin, D3DXVECTOR2 line_a_end, D3DXVECTOR2 line_b_begin, D3DXVECTOR2 line_b_end, D3DXVECTOR2 *out_intersecting_point) {
  //if (line_a_begin.y == line_b_begin.y || line_a_begin.y == line_b_end.y) {
  //  return false;
  //}

    float denom = ((line_a_end.y - line_a_begin.y)*(line_b_end.x - line_b_begin.x)) -
                  ((line_a_end.x - line_a_begin.x)*(line_b_end.y - line_b_begin.y));

    float nume_a = ((line_a_end.x - line_a_begin.x)*(line_b_begin.y - line_a_begin.y)) -
                    ((line_a_end.y - line_a_begin.y)*(line_b_begin.x - line_a_begin.x));

    float nume_b = ((line_b_end.x - line_b_begin.x)*(line_b_begin.y - line_a_begin.y)) -
                    ((line_b_end.y - line_b_begin.y)*(line_b_begin.x - line_a_begin.x));

    if(denom == 0.0f)
    {
        if(nume_a == 0.0f && nume_b == 0.0f)
        {
            // COINCIDENT
            return false;
        }
        // PARALLEL
        return false;
    }

    float ua = nume_a / denom;
    float ub = nume_b / denom;

    if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
    {
        // Get the intersection point.
        out_intersecting_point->x = line_b_begin.x + ua*(line_b_end.x - line_b_begin.x);
        out_intersecting_point->y = line_b_begin.y + ua*(line_b_end.y - line_b_begin.y);

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
double Geometry_Helper::WhatsTheDistanceBetweenThesePoints(D3DXVECTOR2 point_1, D3DXVECTOR2 point_2) {
    double x = point_1.x - point_2.x;
    double y = point_1.y - point_2.y;
    double dist;

    // Calculating distance by euclidean formula
    dist = pow(x,2)+pow(y,2);           
    dist = sqrt(dist);

    return dist;
}

} // Tunnelour