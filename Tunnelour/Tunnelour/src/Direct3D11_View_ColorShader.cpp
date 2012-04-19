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

namespace Tunnelour {
//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Direct3D11_View_ColorShader::Direct3D11_View_ColorShader(ID3D11Device *d3d11device, HWND *hwnd) {
  m_hwnd = hwnd;
  m_d3d11device = d3d11device;
  m_vertexShader = 0;
  m_pixelShader = 0;
  m_layout = 0;
  m_matrixBuffer = 0;
  m_vertexshaderfile = L"resource/Direct3D11_View_ColorShader.vs";
  m_pixelshaderfile  = L"resource/Direct3D11_View_ColorShader.ps";
  m_is_initialised = false;
}


Direct3D11_View_ColorShader::~Direct3D11_View_ColorShader()
{
 m_hwnd = 0;
 m_d3d11device = 0;

  // Release the matrix constant buffer.
 if(m_matrixBuffer) {
  m_matrixBuffer->Release();
  m_matrixBuffer = 0;
 }

 // Release the layout.
 if(m_layout) {
  m_layout->Release();
  m_layout = 0;
 }

 // Release the pixel shader.
 if(m_pixelShader) {
  m_pixelShader->Release();
  m_pixelShader = 0;
 }

 // Release the vertex shader.
 if(m_vertexShader) {
  m_vertexShader->Release();
  m_vertexShader = 0;
 }
}


bool Direct3D11_View_ColorShader::Init()
{
 HRESULT result;
 ID3D10Blob* errorMessage;
 ID3D10Blob* vertexShaderBuffer;
 ID3D10Blob* pixelShaderBuffer;
 D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
 unsigned int numElements;
 D3D11_BUFFER_DESC matrixBufferDesc;

 // Initialize the pointers this function will use to null.
 errorMessage = 0;
 vertexShaderBuffer = 0;
 pixelShaderBuffer = 0;

    // Compile the vertex shader code.
 result = D3DX11CompileFromFile(m_vertexshaderfile, NULL, NULL, "ColorVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
           &vertexShaderBuffer, &errorMessage, NULL);
 if(FAILED(result))
 {
  // If the shader failed to compile it should have writen something to the error message.
  if(errorMessage)
  {
   OutputShaderErrorMessage(errorMessage, *m_hwnd, m_vertexshaderfile);
  }
  // If there was  nothing in the error message then it simply could not find the shader file itself.
  else
  {
   MessageBox(*m_hwnd, m_vertexshaderfile, L"Missing Shader File", MB_OK);
  }

  return false;
 }

    // Compile the pixel shader code.
 result = D3DX11CompileFromFile(m_pixelshaderfile, NULL, NULL, "ColorPixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
           &pixelShaderBuffer, &errorMessage, NULL);
 if(FAILED(result))
 {
  // If the shader failed to compile it should have writen something to the error message.
  if(errorMessage)
  {
   OutputShaderErrorMessage(errorMessage, *m_hwnd, m_pixelshaderfile);
  }
  // If there was nothing in the error message then it simply could not find the file itself.
  else
  {
   MessageBox(*m_hwnd, m_pixelshaderfile, L"Missing Shader File", MB_OK);
  }

  return false;
 }

    // Create the vertex shader from the buffer.
    result = m_d3d11device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
 if(FAILED(result))
 {
  return false;
 }

    // Create the pixel shader from the buffer.
    result = m_d3d11device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
 if(FAILED(result))
 {
  return false;
 }

 // Create the vertex input layout description.
 // This setup needs to match the VertexType stucture in the ModelClass and in the shader.
 polygonLayout[0].SemanticName = "POSITION";
 polygonLayout[0].SemanticIndex = 0;
 polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
 polygonLayout[0].InputSlot = 0;
 polygonLayout[0].AlignedByteOffset = 0;
 polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 polygonLayout[0].InstanceDataStepRate = 0;

 polygonLayout[1].SemanticName = "COLOR";
 polygonLayout[1].SemanticIndex = 0;
 polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 polygonLayout[1].InputSlot = 0;
 polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
 polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 polygonLayout[1].InstanceDataStepRate = 0;

 // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

 // Create the vertex input layout.
 result = m_d3d11device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
            vertexShaderBuffer->GetBufferSize(), &m_layout);
 if(FAILED(result))
 {
  return false;
 }

 // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
 vertexShaderBuffer->Release();
 vertexShaderBuffer = 0;

 pixelShaderBuffer->Release();
 pixelShaderBuffer = 0;

 // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
 matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
 matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
 matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
 matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
 matrixBufferDesc.MiscFlags = 0;
 matrixBufferDesc.StructureByteStride = 0;

 // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
 result = m_d3d11device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
 if(FAILED(result))
 {
  return false;
 }

 m_is_initialised = true;
 return true;
}


bool Direct3D11_View_ColorShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
         D3DXMATRIX projectionMatrix)
{
 bool result;


 // Set the shader parameters that it will use for rendering.
 result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
 if(!result)
 {
  return false;
 }

 // Now render the prepared buffers with the shader.
 RenderShader(deviceContext, indexCount);

 return true;
}

//------------------------------------------------------------------------------
bool Direct3D11_View_ColorShader::IsInitialised() {
  return m_is_initialised;
}


void Direct3D11_View_ColorShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
 char* compileErrors;
 unsigned long bufferSize, i;
 ofstream fout;


 // Get a pointer to the error message text buffer.
 compileErrors = (char*)(errorMessage->GetBufferPointer());

 // Get the length of the message.
 bufferSize = errorMessage->GetBufferSize();

 // Open a file to write the error message to.
 fout.open("shader-error.txt");

 // Write out the error message.
 for(i=0; i<bufferSize; i++)
 {
  fout << compileErrors[i];
 }

 // Close the file.
 fout.close();

 // Release the error message.
 errorMessage->Release();
 errorMessage = 0;

 // Pop a message up on the screen to notify the user to check the text file for compile errors.
 MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

 return;
}


bool Direct3D11_View_ColorShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
             D3DXMATRIX projectionMatrix)
{
 HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
 MatrixBufferType* dataPtr;
 unsigned int bufferNumber;


 // Transpose the matrices to prepare them for the shader.
 D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
 D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
 D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

 // Lock the constant buffer so it can be written to.
 result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
 if(FAILED(result))
 {
  return false;
 }

 // Get a pointer to the data in the constant buffer.
 dataPtr = (MatrixBufferType*)mappedResource.pData;

 // Copy the matrices into the constant buffer.
 dataPtr->world = worldMatrix;
 dataPtr->view = viewMatrix;
 dataPtr->projection = projectionMatrix;

 // Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

 // Set the position of the constant buffer in the vertex shader.
 bufferNumber = 0;

 // Finanly set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

 return true;
}


void Direct3D11_View_ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
 // Set the vertex input layout.
 deviceContext->IASetInputLayout(m_layout);

 // Set the vertex and pixel shaders that will be used to render this triangle.
 deviceContext->VSSetShader(m_vertexShader, NULL, 0);
 deviceContext->PSSetShader(m_pixelShader, NULL, 0);

 // Render the triangle.
 deviceContext->DrawIndexed(indexCount, 0, 0);

 return;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

} // namespace Tunnelour
