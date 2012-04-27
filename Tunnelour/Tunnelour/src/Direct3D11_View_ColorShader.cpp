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
//-----------------------------------------------------------------------------
// The code here was copied pasted and modified from the tutorials at
// http://rastertek.com/dx11tut04.html
//-----------------------------------------------------------------------------

#include "Direct3D11_View_ColorShader.h"
#include "Exceptions.h"

namespace Tunnelour {
//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Direct3D11_View_ColorShader::Direct3D11_View_ColorShader() {
  m_hwnd = 0;
  m_d3d11device = 0;
  m_vertexshader = 0;
  m_pixelshader = 0;
  m_layout = 0;
  m_matrixbuffer = 0;
  m_vertexshaderfile = L"resource/Direct3D11_View_ColorShader.vs";
  m_pixelshaderfile  = L"resource/Direct3D11_View_ColorShader.ps";
  m_is_initialised = false;
}

//------------------------------------------------------------------------------
Direct3D11_View_ColorShader::~Direct3D11_View_ColorShader() {
  m_hwnd = 0;
  m_d3d11device = 0;

  // Release the matrix constant buffer.
  if (m_matrixbuffer) {
    m_matrixbuffer->Release();
    m_matrixbuffer = 0;
  }

  // Release the layout.
  if (m_layout) {
    m_layout->Release();
    m_layout = 0;
  }

  // Release the pixel shader.
  if (m_pixelshader) {
    m_pixelshader->Release();
    m_pixelshader = 0;
  }

  // Release the vertex shader.
  if (m_vertexshader) {
    m_vertexshader->Release();
    m_vertexshader = 0;
  }
}

//------------------------------------------------------------------------------
void Direct3D11_View_ColorShader::Init(ID3D11Device *d3d11device, HWND *hwnd) {
  m_hwnd = hwnd;
  m_d3d11device = d3d11device;

  ID3D10Blob* error = 0;
  ID3D10Blob* vertexshaderbuffer = 0;
  ID3D10Blob* pixelshaderbuffer = 0;
  D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
  unsigned int numberofelements;
  D3D11_BUFFER_DESC matrixbufferdescription;

  LPCSTR pProfile;
  if (d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_11_0) {
    pProfile = "vs_5_0";
  } 
  else if (d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_10_0) {
    pProfile = "vs_4_0";
  }

  // Compile the vertex shader code.
  if (FAILED(D3DX11CompileFromFile(m_vertexshaderfile,
                                   NULL,
                                   NULL,
                                   "ColorVertexShader",
                                   pProfile,
                                   D3D10_SHADER_ENABLE_STRICTNESS,
                                   0,
                                   NULL,
                                   &vertexshaderbuffer,
                                   &error,
                                   NULL))) {
    if (error) {
      char* pCompileErrors = static_cast<char*>(error->GetBufferPointer());
      throw Tunnelour::Exceptions::init_error(pCompileErrors);
    } else {
      // If there was  nothing in the error message then
      // it simply could not find the shader file itself.
      throw Tunnelour::Exceptions::init_error("Missing Vertex Shader File");
    }
  }

  // Create the vertex shader from the buffer.
  if (FAILED(m_d3d11device->CreateVertexShader(vertexshaderbuffer->GetBufferPointer(),
                                               vertexshaderbuffer->GetBufferSize(),
                                               NULL,
                                               &m_vertexshader))) {
    throw Tunnelour::Exceptions::init_error("CreateVertexShader Failed!");
  }

  if (d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_11_0) {
    pProfile = "ps_5_0";
  } 
  else if (d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_10_0) {
    pProfile = "ps_4_0";
  }

  // Compile the pixel shader code.
  if (FAILED(D3DX11CompileFromFile(m_pixelshaderfile,
                                   NULL,
                                   NULL,
                                   "ColorPixelShader",
                                   pProfile,
                                   D3D10_SHADER_ENABLE_STRICTNESS,
                                   0,
                                   NULL,
                                   &pixelshaderbuffer,
                                   &error,
                                   NULL))) {
    if (error) {
        char* pCompileErrors = static_cast<char*>(error->GetBufferPointer());
        throw Tunnelour::Exceptions::init_error(pCompileErrors);
    } else {
      // If there was  nothing in the error message then
      // it simply could not find the shader file itself.
      throw Tunnelour::Exceptions::init_error("Missing Pixel Shader File");
    }
  }

  // Create the pixel shader from the buffer.
  if (FAILED(m_d3d11device->CreatePixelShader(pixelshaderbuffer->GetBufferPointer(),
                                              pixelshaderbuffer->GetBufferSize(),
                                              NULL,
                                              &m_pixelshader))) {
    throw Tunnelour::Exceptions::init_error("CreatePixelShader Failed!");
  }

  // Create the vertex input layout description.
  // This setup needs to match the Vertex_Type stucture in the
  // ModelClass and in the shader.
  polygonlayout[0].SemanticName = "POSITION";
  polygonlayout[0].SemanticIndex = 0;
  polygonlayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
  polygonlayout[0].InputSlot = 0;
  polygonlayout[0].AlignedByteOffset = 0;
  polygonlayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  polygonlayout[0].InstanceDataStepRate = 0;

  polygonlayout[1].SemanticName = "COLOR";
  polygonlayout[1].SemanticIndex = 0;
  polygonlayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
  polygonlayout[1].InputSlot = 0;
  polygonlayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
  polygonlayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  polygonlayout[1].InstanceDataStepRate = 0;

  // Get a count of the elements in the layout.
  numberofelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

  // Create the vertex input layout.
  if (FAILED(m_d3d11device->CreateInputLayout(polygonlayout,
                                              numberofelements,
                                              vertexshaderbuffer->GetBufferPointer(),
                                              vertexshaderbuffer->GetBufferSize(),
                                              &m_layout))) {
    throw Tunnelour::Exceptions::init_error("CreateInputLayout Failed!");
  }

  // Release the vertex shader buffer and pixel shader buffer since they
  // are no longer needed.
  vertexshaderbuffer->Release();
  vertexshaderbuffer = 0;

  pixelshaderbuffer->Release();
  pixelshaderbuffer = 0;

  // Setup the description of the dynamic matrix constant buffer that is
  // in the vertex shader.
  matrixbufferdescription.Usage = D3D11_USAGE_DYNAMIC;
  matrixbufferdescription.ByteWidth = sizeof(MatrixBufferType);
  matrixbufferdescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  matrixbufferdescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  matrixbufferdescription.MiscFlags = 0;
  matrixbufferdescription.StructureByteStride = 0;

  // Create the constant buffer pointer so we can access the vertex
  // shader constant buffer from within this class.
  if (FAILED(m_d3d11device->CreateBuffer(&matrixbufferdescription,
                                         NULL,
                                         &m_matrixbuffer))) {
    throw Tunnelour::Exceptions::init_error("CreateBuffer Failed!");
  }

  m_is_initialised = true;
}

//------------------------------------------------------------------------------
void Direct3D11_View_ColorShader::Render(ID3D11DeviceContext* devicecontext,
                                         int index,
                                         D3DXMATRIX world,
                                         D3DXMATRIX view,
                                         D3DXMATRIX projection) {
  // Set the shader parameters that it will use for rendering.
  D3D11_MAPPED_SUBRESOURCE mappedresource;
  MatrixBufferType* dataptr;
  unsigned int buffernumber;

  // Transpose the matrices to prepare them for the shader.
  D3DXMatrixTranspose(&world, &world);
  D3DXMatrixTranspose(&view, &view);
  D3DXMatrixTranspose(&projection, &projection);

  // Lock the constant buffer so it can be written to.
  if (FAILED(devicecontext->Map(m_matrixbuffer,
                                0,
                                D3D11_MAP_WRITE_DISCARD,
                                0,
                                &mappedresource))) {
    throw Tunnelour::Exceptions::init_error("Map Failed!");
  }

  // Get a pointer to the data in the constant buffer.
  dataptr = reinterpret_cast<MatrixBufferType*>(mappedresource.pData);

  // Copy the matrices into the constant buffer.
  dataptr->world = world;
  dataptr->view = view;
  dataptr->projection = projection;

  // Unlock the constant buffer.
  devicecontext->Unmap(m_matrixbuffer, 0);

  // Set the position of the constant buffer in the vertex shader.
  buffernumber = 0;

  // Finally set the constant buffer in the vertex shader with the
  // updated values.
  devicecontext->VSSetConstantBuffers(buffernumber, 1, &m_matrixbuffer);

  // Now render the prepared buffers with the shader.
  // Set the vertex input layout.
  devicecontext->IASetInputLayout(m_layout);

  // Set the vertex and pixel shaders that will be used to render this triangle.
  devicecontext->VSSetShader(m_vertexshader, NULL, 0);
  devicecontext->PSSetShader(m_pixelshader, NULL, 0);

  // Render the triangle.
  devicecontext->DrawIndexed(index, 0, 0);
}

//------------------------------------------------------------------------------
bool Direct3D11_View_ColorShader::IsInitialised() {
  return m_is_initialised;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
