#include "D3D11Shader.h"
#include <d3dcompiler.h>

/**
 * \brief 
 * \param shader 
 * \param render 
 */
D3D11Shader::D3D11Shader(Shader* shader, D3D11RenderManager* render)
 {
	ID3D11VertexShader* vertexShaderPtr;
	ID3D11PixelShader* pixelShaderPtr;
	this->ShaderData = shader;
	if (shader->Profile == "ps_5_0") {
		HRESULT hr =
		    render->D3dDevice->CreatePixelShader(shader->BinaryData, shader->BinaryDataSize, nullptr, &pixelShaderPtr);
		this->PixelShader.reset(pixelShaderPtr);
	} else if (shader->Profile == "vs_5_0") {
		render->D3dDevice->CreateVertexShader(shader->BinaryData, shader->BinaryDataSize, nullptr, &vertexShaderPtr);
		this->VertexShader.reset(vertexShaderPtr);
	}
	ID3D11ShaderReflection* pReflector = nullptr;
	D3DReflect(shader->BinaryData, shader->BinaryDataSize, IID_ID3D11ShaderReflection,
	           reinterpret_cast<void**>(&pReflector));

	D3D11_SHADER_DESC shaderRefData;

	pReflector->GetDesc(&shaderRefData);
	for (int i = 0; i < shaderRefData.ConstantBuffers; i++) {
		const auto params = pReflector->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC desc;
		params->GetDesc(&desc);
		ShaderConstantBuffer cb;
				;
		cb.Name = desc.Name;
		D3D11_SHADER_VARIABLE_DESC variableDesc;
		for (int j = 0; j < desc.Variables; j++) {
			const auto var = params->GetVariableByIndex(j);
			var->GetDesc(&variableDesc);
			ShaderVariable v { desc.Name,
				               variableDesc.StartOffset,
				               variableDesc.Size,
				               variableDesc.uFlags,
				               variableDesc.DefaultValue,
				               variableDesc.StartTexture,
				               variableDesc.TextureSize,
				               variableDesc.StartSampler,
				               variableDesc.SamplerSize };


			cb.ConstantBufferVariables.try_emplace(v.Name, v);
		}
		this->ShaderData->ConstantBuffers.try_emplace(cb.Name, cb);

	}

	return;
}

