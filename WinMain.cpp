#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
LRESULT CALLBACK fnMessageProcessor(HWND, UINT, WPARAM, LPARAM); // Обработчик сообщений, имя функции можно взять любое, но структура должна быть именно такая
HWND hWnd;				// Дескриптор окна
MSG msg;				// Дескриптор обработчика
WNDCLASSEX wndclass;	// Структура окна
IDirect3D9* g_D3D = NULL;				// Класс direct9 (основной)
IDirect3DDevice9 *g_D3DDevice = NULL;	// Класс устройства отображения (видеокарты)
IDirect3DVertexBuffer9 *g_VB = NULL;	// Буфер точек
IDirect3DTexture9* pD3DTex;	// Класс текстуры
ID3DXFont* pD3DFont;

typedef struct 
{
	FLOAT x, y, z;     // 3д координаты
	FLOAT u, v;			// Uv координаты
} sVertex;	// Структура полигона
#define VERTEXFVF (D3DFVF_XYZ | D3DFVF_TEX1) // Определяем, как отображать полигон

void InitWndClass(HINSTANCE hInstance)
{
	wndclass.cbSize = sizeof(WNDCLASSEX);							// Размер структуры WNDCLASSEX в байтах
	wndclass.style = CS_HREDRAW | CS_VREDRAW;						// Стиль окна
	wndclass.lpfnWndProc = fnMessageProcessor;						// Указатель на обработчик сообщений
	wndclass.cbClsExtra = 0;										// Инзачальный размер структуры в байтах (хз для чего, зануляем)
	wndclass.cbWndExtra = 0;										// Изначальный размер окна в байтах (зануляем)
	wndclass.hInstance = hInstance;									// Дескриптор окна
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);				// Дескриптор класса значка: Первый параметр - Дескриптор на модуль, 
																		// который содержит значок - если NULL, то можем использовать стандартные значки. 
																		// Второй параметр - Указатель на строку, содержащий имя значка
	wndclass.hCursor = LoadCursor(NULL, IDC_CROSS);					// Дескрипток класса указателя (Параметры такие же, что и у иконки)
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	// Задний фон // GetStockObject - функция, которая получает кисть
	wndclass.lpszMenuName = NULL;									// Имя ресурса меню
	wndclass.lpszClassName = "Window Class";						// Имя класса окна
	wndclass.hIconSm = LoadIcon(NULL, IDI_ERROR);					// Значок самого окна, идентично hIcon

	if (RegisterClassEx(&wndclass) == 0)	// Регистрация окна в системе Windows аргумент - указатель на структуру она, инициализированную выше
	{
		exit(1);	// Если не смогли зарегистрировать, выходим
	}
}

void ClassRegister(HINSTANCE hInstance)
{
	hWnd = CreateWindowEx	// Функция создания окна
	(
		WS_EX_OVERLAPPEDWINDOW,	// Стиль окна, похоже на wndclass.style
		wndclass.lpszClassName,	// Имя структуры окна, описанной выше
		"Cube",			// Текст заголовка окна
		WS_OVERLAPPEDWINDOW,	// Позволяет задавать различные комбинации стилей окна
		0,						// Положение по X
		0,						// Положение по Y
		800,					// Ширина
		600,					// Высота
		NULL,					// Указатель на родитель окна (HWND)	
		NULL,					// Указатель на окно с меню
		hInstance,				// Дескриптор окна
		NULL					// Доп. данные (Обычно NULL)
	);

}

BOOL d3dInit()
{
	D3DPRESENT_PARAMETERS d3dpp;	// Параметры показа
	D3DDISPLAYMODE        d3ddm;	// Режим отображения
	D3DXMATRIX matProj, matView;	// Матрица проекции
	BYTE *Ptr;					// Выделяем память под полигоны
	LOGFONT lf;					// Шрифт
	D3DXFONT_DESC lff;

	//if (FAILED(D3DXCreateFontIndirect())
		//return FALSE;

	sVertex Verts[24] =			// Вершины куба
	{
		{ -100.0f,  100.0f, -100.0f,0,0},
		{  100.0f,  100.0f, -100.0f,0,1},
		{ -100.0f, -100.0f, -100.0f,1,0},
		{  100.0f, -100.0f, -100.0f,1,1},

		{	100.0f,  100.0f, -100.0f,0,0},
		{  100.0f,  100.0f, 100.0f,0,1},
		{ 100.0f, -100.0f,	-100.0,1,0},
		{  100.0f, -100.0f, 100.0f,1,1},

		{ 100.0f, 100.0f,	100.0f,0,0},
		{  -100.0f, 100.0f, 100.0f,0,1},
		{	100.0f,  -100.0f, 100.0f,1,0},
		{  -100.0f,  -100.0f, 100.0f,1,1},
		
		{ -100.0f, -100.0f,	-100.0f,0,0},
		{  -100.0f, -100.0f, 100.0f,0,1},
		{	-100.0f,  100.0f, -100.0f,1,0},
		{  -100.0f,  100.0f, 100.0f,1,1},

		{ -100.0f, 100.0f,	-100.0f,0,0},
		{  -100.0f, 100.0f, 100.0f,0,1},
		{	100.0f, 100.0f, -100.0f,1,0},
		{  100.0f,  100.0f, 100.0f,1,1},

		{	100.0f, -100.0f, -100.0f,0,0},
		{  100.0f,  -100.0f, 100.0f,0,1},
		{ -100.0f, -100.0f,	-100.0f,1,0},
		{  -100.0f, -100.0f, 100.0f,1,1},
	};

	if ((g_D3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)	// Пытаемся инициализировать основной класс direct9
		return FALSE;
	if (FAILED(g_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))	// Устанавлием режим отображения
		return FALSE;

	ZeroMemory(&d3dpp, sizeof(d3dpp));			// Чистим память
	d3dpp.Windowed = TRUE;						// Оконный режим
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	
	d3dpp.BackBufferFormat = d3ddm.Format;		//Формат цвета
	//d3dpp.FullScreen_RefreshRateInHz = 10;	// Частота кадров (в полноэкранном режиме)
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.BackBufferWidth = 800;  // Ширина
	d3dpp.BackBufferHeight = 600; // Высота

	if (FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, // Инициализируем класс устройства отображения(видеокарты)
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_D3DDevice)))
		return FALSE;

	g_D3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);	// Режим визуализации

	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.33333f, 1.0f, 1000.0f); // Создаем угол обзора камеры (проекции на экран)
	g_D3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);		// Устанавливаем матрицу


	D3DXMatrixLookAtLH(&matView,	// Создаем точку обзора
		&D3DXVECTOR3(0.0f, 0.0f, -500.0f),	// Координаты точки просмотра
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// Координаты цели
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));		// Верхнее направление	

	g_D3DDevice->SetTransform(D3DTS_VIEW, &matView);
	if (FAILED(g_D3DDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR)))	// Линейная фильтрация для большого расстояния
		return FALSE;
	if (FAILED(g_D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR))) // Линейная фильтрация для малого расстояния
		return FALSE;
	g_D3DDevice->CreateVertexBuffer(sizeof(sVertex) * 24, 0, // Создаем буфер точек (24 - количество вершин)
		VERTEXFVF, D3DPOOL_DEFAULT,
		&g_VB, NULL);
	g_VB->Lock(0, 0, (void**)&Ptr, 0);	// Запрещаем использовать забронированную память
	memcpy(Ptr, Verts, sizeof(Verts));	// Вносим в нее данные
	g_VB->Unlock();						// Разрешаем ее использовать

	ZeroMemory(&lff, sizeof(D3DXFONT_DESC));
	strcpy_s(lff.FaceName, "Times New Roman");
	lff.Height = -32;
	if (FAILED(D3DXCreateFontIndirect(g_D3DDevice, &lff, &pD3DFont)))
		return FALSE;

	return TRUE;
}

BOOL DrawFrame()	// Отрисовка кадра
{
	D3DXMATRIX matWorld;		// Глобальная система координат
	RECT rect = { 0,0,200,200 };
	g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_RGBA(0, 0, 128, 255), 1.0f, 0);	// Отчистка вторичного буфера и покраска его в единый цвет(тут синий)

	if (FAILED(D3DXCreateTextureFromFile(g_D3DDevice, "Smile.bmp", &pD3DTex)))	// Загружаем текстуру из файла
		return FALSE;

	if (SUCCEEDED(g_D3DDevice->BeginScene())) {	// Начинаем визуализацию кадра
		//D3DXMatrixTranslation(&matWorld, x, 0, 0);
		//g_D3DDevice->SetTransform(D3DTS_WORLD, &matWorld); // Задаем позицию
		D3DXMatrixRotationAxis(&matWorld, &D3DXVECTOR3(1.0f, -1.0f, 1.0f), (float)timeGetTime() / 500.0f);		// Вращаем куб
		g_D3DDevice->SetTransform(D3DTS_WORLD, &matWorld); // Задаем позицию

		//g_D3DDevice->SetTexture(0, pD3DTex);	// Устанавливаем текстуру для полигонов
		g_D3DDevice->SetStreamSource(0, g_VB, 0, sizeof(sVertex)); // Устанавливаем визуализацию
		g_D3DDevice->SetFVF(VERTEXFVF);	// Устанавливаем вершинный шейдер

		// Рисуем куб
		//g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0,22);
		g_D3DDevice->SetTexture(0, pD3DTex);	// Устанавливаем текстуру для полигонов
		g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);
		g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, 2);
		g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 12, 2);
		g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 16, 2);
		g_D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 20, 2);

		pD3DFont->DrawTextA(NULL, "Cube Rotation", -1, &rect, 0, D3DCOLOR_RGBA(255, 255, 255, 255));

		g_D3DDevice->EndScene();	// Завершаем визуализацию
		g_D3DDevice->SetTexture(0, NULL);	// Освобождение ресурсов текстуры
	}
	g_D3DDevice->Present(NULL, NULL, NULL, NULL); // Переносим из 2-го буфера на основной экран


	return TRUE;
}

BOOL DoShutdown()
{
	// Чистим буфер точек
	if (g_VB != NULL)
		g_VB->Release();

	// Чистим класс устройства
	if (g_D3DDevice != NULL)
		g_D3DDevice->Release();

	// Чистим класс directx
	if (g_D3D != NULL)
		g_D3D->Release();

	if (pD3DFont != NULL)
		pD3DFont->Release();

	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) // Точка входа
{

	InitWndClass(hInstance); // Инициализация структуры окна
	ClassRegister(hInstance);	// Создание окна

	ShowWindow(hWnd, iCmdShow);	// Показать окно (первый аргумент - дискриптор окна, 
									// второй - Число, определяющее как будет отображаться(скрыто в начале, открыто и т.д.))
	UpdateWindow(hWnd);
	if (!d3dInit())	// Вызываем функцию инициализации direct9
		exit(1);

	while (GetMessage(&msg, NULL, 0, 0)) // Цикл обработки. GetMessage(): аргументы: (Указатель на обработчик), 
										 // (указатель на окно, в котором ожидается сообщение), (Нижняя граница кодов (ставим 0), Верхняя граница кодов (Ставим 0))
	{
		TranslateMessage(&msg);			// Транслирует сообщение в символьные данные
		DispatchMessage(&msg);			// Помещаем эти данные в очередь обработки
		DrawFrame();					// Рисуем кадр
	}

	DoShutdown();	// Отчистка direct9

	UnregisterClass(wndclass.lpszClassName, hInstance); // Чистим структуру окна

	return (msg.wParam);				// Возвращаем последнее значение обработчика при завершении программы
}

LRESULT CALLBACK fnMessageProcessor(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) // Инициализация обработчика
{
	switch (iMsg)
	{
	case WM_CREATE:	// Создание окна
		return (0);
	case WM_PAINT: // Отрисовка окна
		return (0);
	case WM_DESTROY: // Закрытие окна
		PostQuitMessage(0);	// Функция завершения программы Windows
		return(0);
	default:
		return DefWindowProc(hWnd, iMsg, wParam, lParam); // Если сообщение нет в списке, возвращаемся к функции WinMain с помощью этой функции

	}
}