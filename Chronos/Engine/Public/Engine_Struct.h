#pragma once

namespace Engine
{
	typedef struct
	{
		HWND			hWnd;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		bool			isWindowsed;
	}ENGINE_DESC;

	typedef struct
	{
		class CTexture*	pMaterialTextures[AI_TEXTURE_TYPE_MAX];
	}MESH_MATERIAL;

	typedef struct
	{
		XMFLOAT3 vScale;
		XMFLOAT4 vRotation;
		XMFLOAT3 vTranslation;
		_double	TrackPosition;
	} KEYFRAME;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;

		static const unsigned int	iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOS;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];	
	}VTXPOSTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int	iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXCUBETEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXNORTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;		
		XMFLOAT3		vTangent;		

		static const unsigned int	iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMESH;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int	iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMESH;

	typedef struct ENGINE_DLL
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
	}VTXMATRIX;

	typedef struct ENGINE_DLL
	{
		static const unsigned int	iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXRECTINSTANCE;

}