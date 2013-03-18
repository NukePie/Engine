#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

//INCLUDES
#include <D3D11.h>
#include <D3DX11tex.h>

//CLASS
class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	bool InitializeCubeTexture(ID3D11Device*, WCHAR*);/////////////???????????????????
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_texture;
};

#endif