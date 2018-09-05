#include "BoxxiMenuClass.h"

oReset pReset;
oEndScene pEndScene;

QmoMenu dMenu;

LPDIRECT3DDEVICE9 g_pDevice = 0;
int xFontOffSet = 15;

int hackopt1;
int MenuHeight = 10;

int show=1;

int b = 0;
//==================================================================
//Menu HACK
int hack1 = 0;
int hack2 = 0;
int hack3 = 0;
int hack4 = 0;
int hack5 = 0;
//==================================================================

void QmoMenu::CreateItem(int index, char * title, int *hack, int hackmaxval,int hacktype)
{
        hackcount++;
        HACKITEM[hackcount].index = index;
        HACKITEM[hackcount].hack = hack;
        HACKITEM[hackcount].hackmaxval = hackmaxval;
        HACKITEM[hackcount].hacktype = hacktype;
        
        PrintText(title, xFontOffSet, index*15,HACKITEM[hackcount].HCOLOR,pFont); 
}

void QmoMenu::BuildMenu(char * menuname, int x, int y, int h, int w, DWORD TITLECOL, DWORD BACKCOLOR, DWORD BORDERCOLOR, LPDIRECT3DDEVICE9 pDevice)
{
        if(GetAsyncKeyState(VK_INSERT)&1)show=(!show);
        if(!show) {
                DrawBox(0,0, w, 20, BACKCOLOR, BORDERCOLOR, pDevice);
                PrintText("Boxxis D3D MENU", 5, 2, TITLECOL, pFont);
                return;
        }

        DrawBox(x,y, w, h, BACKCOLOR, BORDERCOLOR, pDevice);
        PrintText(menuname, x+10, y+2, TITLECOL, pFont);
        CreateItem(1,"Wallhack", &hack1);
        CreateItem(2,"Chams", &hack2); 
        CreateItem(3,"Crosshair", &hack3);
        CreateItem(4,"NO Smoke", &hack4);
        CreateItem(5,"?????", &hack5);
        RenderMenu();
}


void QmoMenu::RenderMenu()
{
        if(GetAsyncKeyState(VK_DOWN)&1) 
                        selector++;

        if(GetAsyncKeyState(VK_UP)&1)
                if(selector > 1)
                        selector--;

        if (GetAsyncKeyState(VK_RIGHT)<0){
                for(int i=0;i < (hackcount+1);i++){
           if(selector == HACKITEM[i].index){
                           if(*HACKITEM[i].hack < HACKITEM[i].hackmaxval)
                                        *HACKITEM[i].hack += 1;

                                        }
                        }
        }

        if (GetAsyncKeyState(VK_LEFT)<0){
                for(int i=0;i < (hackcount+1);i++){
           if(selector == HACKITEM[i].index){
                           *HACKITEM[i].hack = 0;
                           Sleep(200);
                                }
                        }
        }
        
        for(int i=0;i < (hackcount+1);i++){
                if(selector == HACKITEM[i].index)
                        HACKITEM[i].HCOLOR = GREEN;
                else
                        HACKITEM[i].HCOLOR = RED;
        }

        for(int i=1; i<(hackcount+1); i++){
                if(HACKITEM[i].hacktype == 0){
                if(*HACKITEM[i].hack == 1) 

                
                PrintText("On", xFontOffSet+100, HACKITEM[i].index*15,WHITE,pFont);
         else 
                PrintText("Off", xFontOffSet+100, HACKITEM[i].index*15,RED,pFont);
        
                }
        }

        if(selector < 1)
                selector = 1;

        if(selector > hackcount)
                selector = 1;

        hackcount = 0;
}

void TestThread()
{
        if( hack1 == 1)
                PrintText("Wallhack [ON] text will change color", 30, 200, GREEN, dMenu.pFont);
        else
                PrintText("Wallhack [ON] text will change color", 30, 200, RED, dMenu.pFont);
} 

void ReFont(LPDIRECT3DDEVICE9 pDevice)
{
    if (g_pDevice != pDevice)
    {
        g_pDevice = pDevice;
        try
        {
            if (dMenu.pFont != 0)
                dMenu.pFont->Release();
        } catch (...) {}
        dMenu.pFont = 0;
        D3DXCreateFontA(pDevice, 14, 0, FW_BOLD, 0, 0, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &dMenu.pFont ); 
    }
}

HRESULT WINAPI Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters )
{
        dMenu.pFont->OnLostDevice();

        HRESULT hRet = pReset(pDevice, pPresentationParameters);

        dMenu.pFont->OnResetDevice();

        return hRet;
}


HRESULT WINAPI EndScene(LPDIRECT3DDEVICE9 pDevice)
{
        ReFont(pDevice);
        dMenu.BuildMenu("Boxxis Menu",0,0,190,200,RED,BLACK,GREEN,pDevice);
        TestThread();
        return pEndScene(pDevice);
}

PVOID D3Ddiscover(void *tbl, int size)
{
	HWND				  hWnd;
	void				  *pInterface=0 ;
	D3DPRESENT_PARAMETERS d3dpp; 

	if ((hWnd=CreateWindowEx(NULL,WC_DIALOG,"",WS_OVERLAPPED,0,0,50,50,NULL,NULL,NULL,NULL))==NULL) return 0;
	ShowWindow(hWnd, SW_HIDE);

	LPDIRECT3D9			pD3D;
	LPDIRECT3DDEVICE9	pD3Ddev;
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION))!=NULL) 

	{
	    ZeroMemory(&d3dpp, sizeof(d3dpp));
	    d3dpp.Windowed         = TRUE;
		d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
	    d3dpp.hDeviceWindow    = hWnd;
	    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	    d3dpp.BackBufferWidth  = d3dpp.BackBufferHeight = 600;
		pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&pD3Ddev);
		if (pD3Ddev)  {
		    pInterface = (PDWORD)*(DWORD *)pD3Ddev;
			memcpy(tbl,(void *)pInterface,size);
			pD3Ddev->Release();
		}
		pD3D->Release();
	}
	DestroyWindow(hWnd);
	return pInterface;
}

int __stdcall D3Dinit(void)
{
       HINSTANCE	hD3D;
	DWORD		vTable[105];
	hD3D=0;
	do {
		hD3D = GetModuleHandle("d3d9.dll");
		if (!hD3D) Sleep(100);
	} while(!hD3D);


	if (D3Ddiscover((void *)&vTable[0],420)==0) return 0;
	{
		
		while(1)
		{
		if(memcmp((void*)vTable[82],(void*)(PBYTE)"\x8B\xFF",2)== 0)
		{
			 pReset    = (oReset)    DetourCreate((PBYTE)vTable[16]   , (PBYTE)Reset   ,5);
             pEndScene = (oEndScene) DetourCreate((PBYTE)vTable[42], (PBYTE)EndScene,5);
		}
	Sleep(50);
	}
	return 0;
	}
}

BOOL __stdcall DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hDll);
	if (dwReason == DLL_PROCESS_ATTACH)
	{  
        
	    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)D3Dinit, NULL, NULL, NULL);
		//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DevilHack,0, 0, 0);

	}	
	return TRUE;
}