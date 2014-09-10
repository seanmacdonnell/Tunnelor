//  Copyright 2014 Sean MacDonnell
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

#include "Direct3D11_View_FontShader.h"
#include "Exceptions.h"

namespace Tunnelour {
//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Direct3D11_View_FontShader::Direct3D11_View_FontShader() {
  m_hwnd = 0;
  m_d3d11device = 0;

  m_vertexshader = 0;
  m_pixelshader = 0;
  m_layout = 0;
  m_matrixbuffer = 0;
  m_sampleState = 0;
  m_pixelbuffer = 0;

  m_vertexshaderfile = L"resource/Direct3D11_View_FontVertexShader.vs";
  m_pixelshaderfile  = L"resource/Direct3D11_View_FontPixelShader.ps";

  m_is_initialised = false;
}

//------------------------------------------------------------------------------
Direct3D11_View_FontShader::~Direct3D11_View_FontShader() {
  m_hwnd = 0;
  m_d3d11device = 0;

  // Release the pixel constant buffer.
  if (m_pixelbuffer) {
    m_pixelbuffer->Release();
    m_pixelbuffer = 0;
  }

  // Release the sampler state.
  if (m_sampleState)  {
    m_sampleState->Release();
    m_sampleState = 0;
  }

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
void Direct3D11_View_FontShader::Init(ID3D11Device *d3d11device, HWND *hwnd) {
  m_hwnd = hwnd;
  m_d3d11device = d3d11device;

  ID3D10Blob* error = 0;
  ID3D10Blob* vertexshaderbuffer = 0;
  ID3D10Blob* pixelshaderbuffer = 0;
  D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
  unsigned int numberofelements;
  D3D11_BUFFER_DESC matrixbufferdescription;
  D3D11_SAMPLER_DESC samplerDesc;
  D3D11_BUFFER_DESC pixelBufferDesc;

  // Compile the vertex shader code.
  // Compile the vertex shader code.
  LPCSTR pProfile;
  if (d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_11_0) {
    pProfile = "vs_5_0";
  } else if (d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_10_1) {
    pProfile = "vs_4_1";
  } else if (d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_10_0) {
    pProfile = "vs_4_0";
  } else if (d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_9_3 || 
             d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_9_2 ||
             d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_9_1){
    pProfile = "vs_3_0";
  } else {
    pProfile = "vs_2_0";
  }

  // Compile the vertex shader code.
  if (FAILED(D3DX11CompileFromFile(m_vertexshaderfile,
                                   NULL,
                                   NULL,
                                   "FontVertexShader",
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
      throw Tunnelour::Exceptions::init_error("Direct3D11_View_FontShader: Missing Vertex Shader File");
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
  } else if (d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_10_1) {
    pProfile = "ps_4_1";
  } else if (d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_10_0) {
    pProfile = "ps_4_0";
  } else if (d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_9_3 || 
             d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_9_2 ||
             d3d11device->GetFeatureLevel() == D3D_FEATURE_LEVEL_9_1){
    pProfile = "ps_3_0";
  } else {
    pProfile = "ps_2_0";
  }

  // Compile the pixel shader code.
  if (FAILED(D3DX11CompileFromFile(m_pixelshaderfile,
                                   NULL,
                                   NULL,
                                   "FontPixelShader",
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

  polygonlayout[1].SemanticName = "TEXCOORD";
  polygonlayout[1].SemanticIndex = 0;
  polygonlayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
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

  // Create the matrix buffer pointer so we can access the vertex
  // shader constant buffer from within this class.
  if (FAILED(m_d3d11device->CreateBuffer(&matrixbufferdescription,
                                         NULL,
                                         &m_matrixbuffer))) {
    throw Tunnelour::Exceptions::init_error("CreateBuffer Failed!");
  }

  // Create a texture sampler state description.
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.MipLODBias = 0.0f;
  samplerDesc.MaxAnisotropy = 1;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  samplerDesc.BorderColor[0] = 0;
  samplerDesc.BorderColor[1] = 0;
  samplerDesc.BorderColor[2] = 0;
  samplerDesc.BorderColor[3] = 0;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

  if (FAILED(m_d3d11device->CreateSamplerState(&samplerDesc, &m_sampleState))) {
    throw Tunnelour::Exceptions::init_error("CreateSamplerState Failed!");
  }

  // Setup the description of the dynamic pixel constant buffer that is in the pixel shader.
  pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
  pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  pixelBufferDesc.MiscFlags = 0;
  pixelBufferDesc.StructureByteStride = 0;

  // Create the pixel constant buffer pointer so we can access the pixel shader
  // constant buffer from within this class.
  if (FAILED(m_d3d11device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelbuffer)))  {
    throw Tunnelour::Exceptions::init_error("CreateBuffer (PixelBuffer) Failed!");
  }

  m_is_initialised = true;
}

//------------------------------------------------------------------------------
void Direct3D11_View_FontShader::Render(ID3D11DeviceContext* devicecontext,
                                           int index,
                                           D3DXMATRIX world,
                                           D3DXMATRIX view,
                                           D3DXMATRIX projection,
                                           ID3D11ShaderResourceView* texture,
                                           D3DXCOLOR pixelcolor,
                                           float blend) {
  // Set the shader parameters that it will use for rendering.
  D3D11_MAPPED_SUBRESOURCE mappedresource;
  MatrixBufferType* dataptr;
  unsigned int buffernumber;
  PixelBufferType* dataPtr2;

  // Lock the constant buffer so it can be written to.
  if (FAILED(devicecontext->Map(m_matrixbuffer,
                                0,
                                D3D11_MAP_WRITE_DISCARD,
                                0,
                                &mappedresource))) {
    throw Tunnelour::Exceptions::init_error("Locking Failed!");
  }

  // Get a pointer to the data in the constant buffer.
  dataptr = reinterpret_cast<MatrixBufferType*>(mappedresource.pData);

  // Transpose the matrices to prepare them for the shader.
  D3DXMatrixTranspose(&world, &world);
  D3DXMatrixTranspose(&view, &view);
  D3DXMatrixTranspose(&projection, &projection);

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

  // Set shader texture resource in the pixel shader.
  devicecontext->PSSetShaderResources(0, 1, &texture);

  // Lock the pixel constant buffer so it can be written to.
  if (FAILED(devicecontext->Map(m_pixelbuffer,
                                0,
                                D3D11_MAP_WRITE_DISCARD,
                                0,
                                &mappedresource)))  {
    throw Tunnelour::Exceptions::init_error("Locking Failed!");
  }

  // Get a pointer to the data in the pixel constant buffer.
  dataPtr2 = reinterpret_cast<PixelBufferType*>(mappedresource.pData);

  // Copy the pixel color into the pixel constant buffer.
  dataPtr2->pixelColor = pixelcolor;

  // Copy the blend amount value into the transparent constant buffer.
  dataPtr2->blendAmount = blend;

  // Unlock the pixel constant buffer.
  devicecontext->Unmap(m_pixelbuffer, 0);

  // Set the position of the pixel constant buffer in the pixel shader.
  buffernumber = 0;

  // Now set the pixel constant buffer in the pixel shader with the
  // updated value.
  devicecontext->PSSetConstantBuffers(buffernumber, 1, &m_pixelbuffer);

  // Now render the prepared buffers with the shader.
  // Set the vertex input layout.
  devicecontext->IASetInputLayout(m_layout);

  // Set the vertex and pixel shaders that will be used to render this triangle.
  devicecontext->VSSetShader(m_vertexshader, NULL, 0);
  devicecontext->PSSetShader(m_pixelshader, NULL, 0);

  // Set the sampler state in the pixel shader.
  devicecontext->PSSetSamplers(0, 1, &m_sampleState);

  // Render the triangle.
  devicecontext->DrawIndexed(index, 0, 0);
}

//------------------------------------------------------------------------------
bool Direct3D11_View_FontShader::IsInitialised() {
  return m_is_initialised;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // namespace Tunnelour
