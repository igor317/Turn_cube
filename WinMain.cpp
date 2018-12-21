#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
LRESULT CALLBACK fnMessageProcessor(HWND, UINT, WPARAM, LPARAM); // ���������� ���������, ��� ������� ����� ����� �����, �� ��������� ������ ���� ������ �����
HWND hWnd;				// ���������� ����
MSG msg;				// ���������� �����������
WNDCLASSEX wndclass;	// ��������� ����
IDirect3D9* g_D3D = NULL;				// ����� direct9 (��������)
IDirect3DDevice9 *g_D3DDevice = NULL;	// ����� ���������� ����������� (����������)
IDirect3DVertexBuffer9 *g_VB = NULL;	// ����� �����
typedef struct 
{
	FLOAT x, y, z;     // 3� ����������
	D3DCOLOR Diffuse;  // Diffuse color component
} sVertex;	// ��������� ��������
#define VERTEXFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE) // ����������, ��� ���������� �������

void InitWndClass(HINSTANCE hInstance)
{
	wndclass.cbSize = sizeof(WNDCLASSEX);							// ������ ��������� WNDCLASSEX � ������
	wndclass.style = CS_HREDRAW | CS_VREDRAW;						// ����� ����
	wndclass.lpfnWndProc = fnMessageProcessor;						// ��������� �� ���������� ���������
	wndclass.cbClsExtra = 0;										// ����������� ������ ��������� � ������ (�� ��� ����, ��������)
	wndclass.cbWndExtra = 0;										// ����������� ������ ���� � ������ (��������)
	wndclass.hInstance = hInstance;									// ���������� ����
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);				// ���������� ������ ������: ������ �������� - ���������� �� ������, 
																		// ������� �������� ������ - ���� NULL, �� ����� ������������ ����������� ������. 
																		// ������ �������� - ��������� �� ������, ���������� ��� ������
	wndclass.hCursor = LoadCursor(NULL, IDC_CROSS);					// ���������� ������ ��������� (��������� ����� ��, ��� � � ������)
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	// ������ ��� // GetStockObject - �������, ������� �������� �����
	wndclass.lpszMenuName = NULL;									// ��� ������� ����
	wndclass.lpszClassName = "Window Class";						// ��� ������ ����
	wndclass.hIconSm = LoadIcon(NULL, IDI_ERROR);					// ������ ������ ����, ��������� hIcon

	if (RegisterClassEx(&wndclass) == 0)	// ����������� ���� � ������� Windows �������� - ��������� �� ��������� ���, ������������������ ����
	{
		exit(1);	// ���� �� ������ ����������������, �������
	}
}

void ClassRegister(HINSTANCE hInstance)
{
	hWnd = CreateWindowEx	// ������� �������� ����
	(
		WS_EX_OVERLAPPEDWINDOW,	// ����� ����, ������ �� wndclass.style
		wndclass.lpszClassName,	// ��� ��������� ����, ��������� ����
		"Cube",			// ����� ��������� ����
		WS_OVERLAPPEDWINDOW,	// ��������� �������� ��������� ���������� ������ ����
		0,						// ��������� �� X
		0,						// ��������� �� Y
		800,					// ������
		600,					// ������
		NULL,					// ��������� �� �������� ���� (HWND)	
		NULL,					// ��������� �� ���� � ����
		hInstance,				// ���������� ����
		NULL					// ���. ������ (������ NULL)
	);

}

BOOL d3dInit()
{
	D3DPRESENT_PARAMETERS d3dpp;	// ��������� ������
	D3DDISPLAYMODE        d3ddm;	// ����� �����������
	D3DXMATRIX matProj, matView;	// ������� ��������
	BYTE *Ptr;					// �������� ������ ��� ��������
	sVertex Verts[24] =			// ������� ����
	{
		{ -100.0f,  100.0f, -100.0f, D3DCOLOR_RGBA(255,255,180,255) },
		{  100.0f,  100.0f, -100.0f,  D3DCOLOR_RGBA(255,140,255,255) },
		{ -100.0f, -100.0f, -100.0f,  D3DCOLOR_RGBA(255,57,255,255) },
		{  100.0f, -100.0f, -100.0f, D3DCOLOR_RGBA(44,57,255,255) },

		{	100.0f,  100.0f, -100.0f,  D3DCOLOR_RGBA(255,255,180,255) },
		{  100.0f,  100.0f, 100.0f, D3DCOLOR_RGBA(255,140,255,255) },
		{ 100.0f, -100.0f,	-100.0f,  D3DCOLOR_RGBA(255,57,255,255) },
		{  100.0f, -100.0f, 100.0f,  D3DCOLOR_RGBA(44,57,255,255) },

		{ 100.0f, 100.0f,	100.0f,  D3DCOLOR_RGBA(255,57,255,255) },
		{  -100.0f, 100.0f, 100.0f,  D3DCOLOR_RGBA(44,57,255,255) },
		{	100.0f,  -100.0f, 100.0f,  D3DCOLOR_RGBA(255,255,180,255) },
		{  -100.0f,  -100.0f, 100.0f, D3DCOLOR_RGBA(255,140,255,255) },
		
		{ -100.0f, -100.0f,	-100.0f,  D3DCOLOR_RGBA(255,57,255,255) },
		{  -100.0f, -100.0f, 100.0f,  D3DCOLOR_RGBA(44,57,255,255) },
		{	-100.0f,  100.0f, -100.0f,  D3DCOLOR_RGBA(255,255,180,255) },
		{  -100.0f,  100.0f, 100.0f, D3DCOLOR_RGBA(255,140,255,255) },

		{ -100.0f, 100.0f,	-100.0f,  D3DCOLOR_RGBA(255,57,255,255) },
		{  -100.0f, 100.0f, 100.0f,  D3DCOLOR_RGBA(44,57,255,255) },
		{	100.0f, 100.0f, -100.0f,  D3DCOLOR_RGBA(255,255,180,255) },
		{  100.0f,  100.0f, 100.0f, D3DCOLOR_RGBA(255,140,255,255) },

		{	100.0f, -100.0f, -100.0f,  D3DCOLOR_RGBA(255,255,180,255) },
		{  100.0f,  -100.0f, 100.0f, D3DCOLOR_RGBA(255,140,255,255) },
		{ -100.0f, -100.0f,	-100.0f,  D3DCOLOR_RGBA(255,57,255,255) },
		{  -100.0f, -100.0f, 100.0f,  D3DCOLOR_RGBA(44,57,255,255) },

	};

	if ((g_D3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)	// �������� ���������������� �������� ����� direct9
		return FALSE;
	if (FAILED(g_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))	// ����������� ����� �����������
		return FALSE;
	ZeroMemory(&d3dpp, sizeof(d3dpp));			// ������ ������
	d3dpp.Windowed = TRUE;						// ������� �����
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	
	d3dpp.BackBufferFormat = d3ddm.Format;		
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, // �������������� ����� ���������� �����������(����������)
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_D3DDevice)))
		return FALSE;

	g_D3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);	// ����� ������������
	g_D3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.33333f, 1.0f, 1000.0f); // ������� ���� ������ ������ (�������� �� �����)
	g_D3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);		// ������������� ��������� �� ������

	
	D3DXMatrixLookAtLH(&matView,	// ������� ����� ������
		&D3DXVECTOR3(0.0f, 0.0f, -500.0f),	// ���������� ����� ���������
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// ���������� ����
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));		// ������� �����������	

	g_D3DDevice->SetTransform(D3DTS_VIEW, &matView);


	g_D3DDevice->CreateVertexBuffer(sizeof(sVertex) * 24, 0, // ������� ����� ����� (24 - ���������� ������)
		VERTEXFVF, D3DPOOL_DEFAULT,
		&g_VB, NULL);
	g_VB->Lock(0, 0, (void**)&Ptr, 0);	// ��������� ������������ ��������������� ������
	memcpy(Ptr, Verts, sizeof(Verts));	// ������ � ��� ������
	g_VB->Unlock();						// ��������� �� ������������

	return TRUE;
}

BOOL DrawFrame()	// ��������� �����
{
	D3DXMATRIX matWorld;		// ���������� ������� ���������
	g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_RGBA(0, 64, 128, 255), 1.0f, 0);	// ������ ��� ��� �����

	if (SUCCEEDED(g_D3DDevice->BeginScene())) {	// �������� ������������ �����
		D3DXMatrixRotationAxis(&matWorld, &D3DXVECTOR3(1.0f, -1.0f, 1.0f), (float)timeGetTime() / 500.0f);		// ������� ���
		//D3DXMatrixTranslation(&matWorld, x, 0, 0);
		g_D3DDevice->SetTransform(D3DTS_WORLD, &matWorld); // ������ �������


		g_D3DDevice->SetStreamSource(0, g_VB, 0, sizeof(sVertex)); // ������������� ������������
		g_D3DDevice->SetFVF(VERTEXFVF);	// ������������� ����� ����������� ���������

		// ������ ���
		g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0,22);
		/*g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);
		g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, 2);
		g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 12, 2);
		g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 16, 2);
		g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 20, 2);*/

		g_D3DDevice->EndScene();	// ��������� ������������
	}

	g_D3DDevice->Present(NULL, NULL, NULL, NULL); // ��������� �� 2-�� ������ �� �������� �����

	return TRUE;
}

BOOL DoShutdown()
{
	// ������ ����� �����
	if (g_VB != NULL)
		g_VB->Release();

	// ������ ����� ����������
	if (g_D3DDevice != NULL)
		g_D3DDevice->Release();

	// ������ ����� directx
	if (g_D3D != NULL)
		g_D3D->Release();

	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) // ����� �����
{

	InitWndClass(hInstance); // ������������� ��������� ����
	ClassRegister(hInstance);	// �������� ����

	ShowWindow(hWnd, iCmdShow);	// �������� ���� (������ �������� - ���������� ����, 
									// ������ - �����, ������������ ��� ����� ������������(������ � ������, ������� � �.�.))
	UpdateWindow(hWnd);
	if (!d3dInit())	// �������� ������� ������������� direct9
		exit(1);


	while (GetMessage(&msg, NULL, 0, 0)) // ���� ���������. GetMessage(): ���������: (��������� �� ����������), 
										 // (��������� �� ����, � ������� ��������� ���������), (������ ������� ����� (������ 0), ������� ������� ����� (������ 0))
	{
		TranslateMessage(&msg);			// ����������� ��������� � ���������� ������
		DispatchMessage(&msg);			// �������� ��� ������ � ������� ���������
		DrawFrame();					// ������ ����
	}

	DoShutdown();	// �������� direct9

	UnregisterClass(wndclass.lpszClassName, hInstance); // ������ ��������� ����

	return (msg.wParam);				// ���������� ��������� �������� ����������� ��� ���������� ���������
}

LRESULT CALLBACK fnMessageProcessor(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) // ������������� �����������
{
	switch (iMsg)
	{
	case WM_CREATE:	// �������� ����
		return (0);
	case WM_PAINT: // ��������� ����
		return (0);
	case WM_DESTROY: // �������� ����
		PostQuitMessage(0);	// ������� ���������� ��������� Windows
		return(0);
	default:
		return DefWindowProc(hWnd, iMsg, wParam, lParam); // ���� ��������� ��� � ������, ������������ � ������� WinMain � ������� ���� �������

	}
}
