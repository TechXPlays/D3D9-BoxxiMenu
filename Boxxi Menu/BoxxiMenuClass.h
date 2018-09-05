#include "Functions.h"


typedef struct{
        int index;
        char * title;
        int *hack;
        int hackmaxval;
        int hacktype;
        DWORD HCOLOR;
}ITEM;

class QmoMenu {
        public:
        LPDIRECT3DDEVICE9 pDevice;
        LPD3DXFONT pFont;

        int hackcount;
        int selector;
        int x,y,w,h;
        DWORD COLOR;

        ITEM HACKITEM[99];
        char hackrval[256];

        void CreateItem(int index, char * title, int *hack,int hackmaxval=1,int hacktype=0);
        void BuildMenu(char * menuname, int x, int y, int h, int w, DWORD TITLECOL, DWORD BACKCOLOR, DWORD BORDERCOLOR, LPDIRECT3DDEVICE9 pDevice);
        void RenderMenu();
};

typedef HRESULT ( WINAPI* oReset )( LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters );
typedef HRESULT (WINAPI* oEndScene)(LPDIRECT3DDEVICE9 pDevice);


#define RED D3DCOLOR_ARGB(255, 255, 0, 0)
#define GREEN D3DCOLOR_ARGB(255, 0, 255, 0)
#define BLUE D3DCOLOR_ARGB(255, 0, 0, 255)
#define WHITE D3DCOLOR_ARGB(255, 255, 255, 255)
#define BLACK D3DCOLOR_ARGB(255, 0, 0, 0)
#define YELLOW D3DCOLOR_ARGB(255, 255, 255, 0)
#define TEAL D3DCOLOR_ARGB(255, 0, 255, 255)
#define PINK D3DCOLOR_ARGB(255, 255, 240, 0)
#define ORANGE D3DCOLOR_ARGB(255, 255, 132, 0)
#define LIME D3DCOLOR_ARGB(255, 198, 255, 0)
#define SKYBLUE D3DCOLOR_ARGB(255, 0, 180, 255)
#define MAROON D3DCOLOR_ARGB(255, 142, 30, 0)
#define LGRAY D3DCOLOR_ARGB(255, 174, 174, 174) 
#define DGRAY D3DCOLOR_ARGB(255, 71, 65, 64) 
#define BROWN D3DCOLOR_ARGB(255, 77, 46, 38)
#define SHIT D3DCOLOR_ARGB(255, 74, 38, 38)