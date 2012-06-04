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
//  Author(s)  : Sean MacDonnell
//  Description: 
//

//-----------------------------------------------------------------------------
cbuffer MatrixBuffer {
  matrix worldMatrix;
  matrix viewMatrix;
  matrix projectionMatrix;
};

//-----------------------------------------------------------------------------
struct VertexInputType {
  float4 position : POSITION;
  float2 tex : TEXCOORD0;
};

//-----------------------------------------------------------------------------
struct PixelInputType {
  float4 position : SV_POSITION;
  float2 tex : TEXCOORD0;
};

//-----------------------------------------------------------------------------
PixelInputType TextureVertexShader(VertexInputType input) {
  PixelInputType output;
    
  // Change the position vector to be 4 units for proper matrix calculations.
  input.position.w = 1.0f;

  // Calculate the position of the vertex against the world, view,
  // and projection matrices.
  output.position = mul(input.position, worldMatrix);
  output.position = mul(output.position, viewMatrix);
  output.position = mul(output.position, projectionMatrix);
    
  // Store the texture coordinates for the pixel shader.
  output.tex = input.tex;
    
  return output;
}