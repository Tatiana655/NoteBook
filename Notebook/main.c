#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#define Min(X, Y) (((int)(X) < (int)(Y)) ? (int)(X) : (int)(Y))
#define Max(X, Y) (((int)(X) > (int)(Y)) ? (int)(X) : (int)(Y))
#define Ceil(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define Abs(x) ((x)<0 ? -(x) : (x))

#define MAX_COUNT 65000
#define MAX_LENGTH 65000

#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "System.h"
#include "Menu.h"

#define MY_MAX_PATH 300
#define MY_MAX_FNAME 300
#define MY_MAX_EXT 300

static OPENFILENAME ofn;

//ôóíêöèÿ ïå÷àòè ñîñòîÿíèÿ ñèñòåìû//íå âëÿåò íà ðàáîòó ïðîãðàììû, ïðîñòî ïå÷àòàåò â  ôàéë f
void PrintStatus(UINT message, Viewer * viewer, FILE * f,  int mark)//PrintStatus(message, viewer, myfile, 0 );
{
    fprintf(f,"msg: %i; mark: %i\n", message, mark);
    fprintf(f,"iCur: %lu; deltaPos: %i;\n", viewer->curPos, viewer->deltaPos);
    fprintf(f,"iscrollMax.v: %lu; MODE: %i; iscrollPos.v: %i;\n", viewer->iscrollMax.v, viewer->mode,viewer->iscrollPos.v);
    fprintf(f,"iscrollMax.h: %lu; MODE: %i; iscrollPos.h: %i;\n", viewer->iscrollMax.h, viewer->mode,viewer->iscrollPos.h);
}

//îòêðûòèå îêíà äèàëîãà File Open (ïðîâîäíèê)
BOOL PopFileOpenDlg(HWND hwnd, PSTR pstrFileName)
{
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = pstrFileName;
    ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
    return GetOpenFileName(&ofn);
}

//èíèò ñòðóêòóðû ofn
void PopFileInitialize(HWND hwnd)
{
    static char szFilter[] = "Text Files(*.TXT)\0*.txt\0" \
                             "ASCII Files(*.ASC)\0*.asc\0" \
                             "All Files(*.*)\0*.*\0\0";
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = NULL;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = NULL; // Set in Open and Close functions
    ofn.nMaxFile = MY_MAX_PATH;
    ofn.lpstrFileTitle = NULL; // Set in Open and Close functions
    ofn.nMaxFileTitle = MY_MAX_FNAME + MY_MAX_EXT;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = 0; // Set in Open and Close functions
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = "txt";
    ofn.lCustData = 0L;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;
}


/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = "Class name";
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style =  CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = "Menu";               /* menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               "Class name",         /* Classname */
               "App",       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               544,                 /* The programs width */
               375,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               lpszArgument                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    static struct Model * model;
    static struct Viewer* viewer;
//    static int last;//ïîëíûé ëè ïîñëåäíèé áëîê?
    HDC hdc;
    PAINTSTRUCT p;
    int i, x, y, iPaintBeg, iPaintEnd, iVscrollInc=0, iHscrollInc=0;
    hdc = GetDC(hwnd);
    TEXTMETRIC tm;//ïåðåìåííàÿ äëÿ õðàíåíèÿ ðàçìåðîâ ñèìâîëà
    //int lastDown = 0;
    static char szFileName[256];
    static FILE * myfile;

    static lastPosx, lastPosy;

    switch (message)                  /* handle the messages */
    {
    case WM_CREATE://Ok
    {
        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
        CREATESTRUCT *str = (CREATESTRUCT *)lParam;

        lstrcpy(szFileName,(PSTR)(((LPCREATESTRUCT) lParam)->lpCreateParams));
        PopFileInitialize(hwnd);

        myfile = fopen("status.txt", "w+");
        fprintf(myfile,"\nNEW_NEW_NEW\n");
            model = (Model*)malloc(sizeof(Model));
            if (model ==NULL) SendMessage(hwnd, WM_DESTROY, 0, 0);
            int ch = InitModel( model,(char *)str->lpCreateParams);
            if (ch == 1)
            {
        int msgboxID = MessageBox(
        NULL,
        (LPCWSTR)"Wrong file name",
        (LPCWSTR)"Error",
        MB_OK
        );
        switch (msgboxID)
        {
        case IDOK:
            SendMessage(hwnd, WM_DESTROY, 0, 0);
            // TODO: add code
        break;
        }
            }
            viewer = (Viewer*)malloc(sizeof(Viewer));
            if (viewer==NULL) {
                SendMessage(hwnd, WM_DESTROY, 0, 0);
            }
            viewer->cClient = (struct Pairxy)
            {
                0,0
            };
            viewer->cChar = (struct Pairxy)
            {
                0,0
            };
            viewer->curPos=0;
            viewer->deltaPos=0;
            viewer->iscrollPos.h=0;
            viewer->iscrollPos.v=0;
            viewer->iscrollMax.h=0;
            viewer->iscrollMax.v=0;

            viewer->mode = WITHOUT_LAYOUT;
           // last = 0;
            viewer->coef.v = 1;
            viewer->coef.h = 1;
            GetTextMetrics(hdc, &tm);//ïîëó÷åíèå ðàçìåðîâ ñèìâîëà
            viewer->cChar.x = tm.tmAveCharWidth;//øèðèíà ñèìâîëà
            viewer->cChar.y = tm.tmHeight + tm.tmExternalLeading;
            viewer->actPos = viewer->curPos;
            viewer->actDelta = viewer->actDelta;
            PrintStatus(message, viewer, myfile, 0 );
        break;

    }
    break;
    case WM_COMMAND :
    {
        switch(LOWORD(wParam))
        {
        case IDM_OPEN :
        {
            if(PopFileOpenDlg(hwnd, szFileName))
            {
                CloseModel(model);
                model = (Model*)malloc(sizeof(Model));
                if (model==NULL)
                {
                    SendMessage(hwnd, WM_DESTROY, 0, 0);
                }
                InitModel(model,szFileName);

                viewer->cClient = (struct Pairxy)
                {
                    0,0
                };
                viewer->iscrollPos.h=0;
                viewer->iscrollPos.v=0;
                viewer->curPos = 0;
                viewer->deltaPos = 0;
                viewer->actPos = viewer->curPos;
                viewer->actDelta = viewer->actDelta;
//                last = 0;
                viewer->mode = WITHOUT_LAYOUT;
                SendMessage(hwnd, WM_SIZE, 0, 0);
                InvalidateRect(hwnd,NULL,TRUE);
            }
        }
        break;
        case IDM_EXIT:
        {
            SendMessage(hwnd, WM_DESTROY, 0, 0);
        }

        break;
        case IDM_MOD:
        {
            viewer->mode= !viewer->mode;
            PrintStatus(message, viewer, myfile, 1 );
            //òóò íàäî îïðåäåëèòü áëîê ñ êîòîðîì íàõîäèòñÿ òåêóùàÿ ïîçèöèÿ è ïåðåäâèíóòü íà íà÷àëî áëîêà, åñëè ñ âåðñòêîé. Áåç âåðñêè âñ¸ íîðì
            viewer->deltaPos = 0;
            viewer->actDelta = 0;
            //îòïðàâòü ñîîáøåíèÿ WM_SIZE è â WM_PAINT
            SendMessage(hwnd, WM_SIZE, 0, 0);
            InvalidateRect(hwnd,NULL,TRUE);
        }
        break;
        default:
            break;
        }
    }
    break;
    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd,&p);
        size_t blockSize = viewer->cClient.x/viewer->cChar.x;
        if ((viewer->mode == WITHOUT_LAYOUT))
        {
            PrintStatus(message, viewer, myfile, 2 );
            iPaintBeg = Max(0,viewer->curPos /*+p.rcPaint.top / viewer->cChar.y*/ );
            iPaintEnd = (int)viewer->curPos + Min(model->lineCount - viewer->curPos , /*viewer->iscrollPos.v*viewer->coef.v*/ Ceil((double)viewer->cClient.y / (double)viewer->cChar.y));//?????(-)
            y=0;
            for(i = iPaintBeg; i < iPaintEnd; i++)
            {
                x = viewer->cChar.x *( - viewer->iscrollPos.h * viewer->coef.h);
               // y = viewer->cChar.y *( /*- viewer->iscrollPos.v*viewer->coef.v + i*/);
                TextOut(
                    hdc, x, y,
                    model->allText + model->offset[i],
                    model->offset[i+1] - model->offset[i]
                );
                y+=viewer->cChar.y;
            }
        }
        else
        {
            PrintStatus(message, viewer, myfile, 3 );

            int iCur = Max(0, viewer->curPos);
            int iMaxCount = Ceil((double)viewer->cClient.y/(double)viewer->cChar.y);//ñêîëüêî ñòðîê ïîìåùàåòñÿ â ðàáî÷óþ îáëàñòü


            int k = 0;
            int delt = viewer->deltaPos;
            y = 0;//ïîñòàâü âåðíûé ó
            while(1)
            {
                if ((k >= iMaxCount)|| (iCur >= (int)model->lineCount)) break;//k > iMaxCount
                //int tmpc =(model->allText[(model->offset[iCur + 1]-2)] == '\n')?(-2):(0);
                int tmpc = 0;
                if (*(model->allText + model->offset[iCur] + delt + Min( model->offset[iCur +1]- model->offset[iCur] - delt , blockSize) - 2) == '\r')
                    tmpc= -2;
                if (*(model->allText + model->offset[iCur] + delt + Min( model->offset[iCur +1]- model->offset[iCur] - delt , blockSize) - 1) == '\n')
                    tmpc= -1;
                if ((*(model->allText + model->offset[iCur] + delt) != '\n') && (*(model->allText + model->offset[iCur] + delt)) != '\r')
                    TextOut(
                        hdc, 0, y,
                        model->allText + model->offset[iCur] + delt,
                        Min( model->offset[iCur +1]- model->offset[iCur] - delt, blockSize) + tmpc
                    );

                delt += blockSize;
                if (delt > (int)(model->offset[iCur+1]-model->offset[iCur]))
                {
                    iCur++;
                    delt = 0;
                }
                y += viewer->cChar.y;
                k++;
            }

        }
        /*ñëåäè çà òåêóùåé ïîçèöèåé
        if (viewer->mode == WITH_ALL_LAYOUT){
        int tmp = viewer->curPos;
        int counter = 0;//êîîðäèíàòà y
        if (tmp < viewer->beforePos) counter += (size_t)Ceil((double)(model->offset[tmp+1]-model->offset[tmp] + 1 - viewer->deltaPos)/(double)blockSize);
        tmp++;
        while (tmp < viewer->beforePos)
        {
            counter += (size_t)Ceil((double)(model->offset[tmp+1]-model->offset[tmp] + 1)/(double)blockSize);
            tmp++;
        }
        counter += viewer->beforeDelta / blockSize;
        //if (viewer->beforeDelta % blockSize == 0) counter--;
        HPEN brush1 =CreatePen(1,1,RGB(255,255,255));
        SelectObject(hdc, brush1);
        Rectangle(hdc, 0,0,1,viewer->cClient.y);
        DeleteObject(brush1);

        HPEN brush = CreatePen(2,2,RGB(255,0,0));
        SelectObject(hdc, brush);

        lastPosx = (viewer->beforeDelta % blockSize) * viewer->cChar.x;
        lastPosy = viewer->cChar.y * counter;
        Rectangle(hdc, (viewer->beforeDelta % blockSize) * viewer->cChar.x, viewer->cChar.y * counter,
                  (viewer->beforeDelta % blockSize) * viewer->cChar.x + 1, viewer->cChar.y * (counter + 1));
        DeleteObject(brush);
        }*/
        EndPaint(hwnd, &p);
        break;
    }
    case WM_DESTROY:
        CloseModel(model);
        model = NULL;
        if (viewer != NULL)
        {
            free(viewer);
            viewer = NULL;
        }
        PostQuitMessage (0);
        /* send a WM_QUIT to the message queue */
        break;
    case WM_SIZE :
    {
        if (model == NULL) break;
        if (viewer == NULL) break;
        RECT rc;//ðàçìåðû ðàáî÷åé îáëàñòè
        GetClientRect(hwnd, &rc);
        viewer->cClient.x = rc.right;
        viewer->cClient.y = rc.bottom;
        if (viewer->mode == WITHOUT_LAYOUT)
        {
            //ïîìåíÿé iVscrollPos íà òåêóùóþ ñòðîêó
            viewer->iscrollPos.v = (viewer->curPos);

            viewer->iscrollMax.v = Max(0, (model->lineCount - 1 ));
            viewer->iscrollPos.v = Min(viewer->iscrollPos.v, viewer->iscrollMax.v);
            viewer->coef.v = Ceil((double)viewer->iscrollMax.v / (double)MAX_COUNT);
            if(viewer->coef.v ==0) viewer->coef.v=1;
            viewer->curPos -= viewer->curPos % viewer->coef.v;

            viewer->iscrollMax.v = viewer->iscrollMax.v/viewer->coef.v;
            viewer->iscrollPos.v =(viewer->iscrollPos.v/viewer->coef.v);
            SetScrollRange(hwnd, SB_VERT, 0, viewer->iscrollMax.v, FALSE);
            SetScrollPos(hwnd, SB_VERT, viewer->iscrollPos.v, TRUE);

            viewer->iscrollMax.h = Max(0, (model->maxLength - 1*viewer->coef.h));
            viewer->iscrollPos.h = Min(viewer->iscrollPos.h, viewer->iscrollMax.h);
            //coef+h_scroll
            viewer->coef.h = Ceil((double)viewer->iscrollMax.h / (double)MAX_LENGTH);
            if (viewer->coef.h ==0) viewer->coef.h =1;
            viewer->iscrollMax.h = Ceil((double)viewer->iscrollMax.h/(double)viewer->coef.h);
            viewer->iscrollPos.h =(viewer->iscrollPos.h/viewer->coef.h);

            SetScrollRange(hwnd, SB_HORZ, 0, viewer->iscrollMax.h, FALSE);
            SetScrollPos(hwnd, SB_HORZ, viewer->iscrollPos.h, TRUE);

            viewer->actPos = viewer->curPos;
             viewer->actDelta = viewer->deltaPos;;
        }
        else//ðåæèì âåðñòêè ïî÷åìó-òî ïðè óìåëè÷åíèè ðàáî÷åîáëàñòè âíèç ìåíÿåòñÿ ãëàâíàÿ ñòðîêà
        {
            int blockSize = viewer->cClient.x / viewer->cChar.x;//êîëè÷åñòâî ñèìâîëîâ â îäíîé ñòðîêå
            //int idNewBlock = viewer->deltaPos / blockSize;// â êàêîì áëîêå äåëüòà? (íîìåð áëîêà)
            //viewer->deltaPos = idNewBlock*blockSize;//äåëüòà óêàçûâàåò íà íà÷àëî íîâîãî áëîêà â currPos
            //óñòàíîâèòü ñêðîëëû
            //ýòî ÷òîáû óáðàòü ãîðèçîíòàëüíûé +-
            SetScrollRange(hwnd, SB_HORZ, 0, 0, FALSE);
            //ïîñòàâèòü âåðòèêàëüíûé  ñêðîëë è èçìåíè ìàêñèìóì ñêðîëëà

            viewer->iscrollMax.v = 0;
            size_t m=0;
            for (m=0; m < model->lineCount; m++)
            {
                viewer->iscrollMax.v += (size_t)Ceil((double)(model->offset[m+1]-model->offset[m] + 1)/(double)blockSize);//ñêîëüêî áëîêîâ â îäíîé ñòðîêå
            }

            viewer->iscrollMax.v = Max(0, viewer->iscrollMax.v);// ñ 1

            viewer->coef.v = Ceil((double)(viewer->iscrollMax.v-1) / (double)MAX_COUNT);
            if (viewer->coef.v == 0) viewer->coef.v=1;
            size_t iCur = 0;
            int iDelt = 0;
            int tmpCur = 0, tmpDelt = 0;
            //èùåì â êàêîì áëîêå òåêóùàÿ (before) ïîçèöèÿ è ìåíÿåì iCurPos+delt
            viewer->iscrollPos.v = 0;
            while (1)
            {
                if ((iCur < viewer->actPos ) || ((iCur <= viewer->actPos) && (iDelt <= viewer->actDelta) ))
                {
                    tmpCur = iCur;
                    tmpDelt = iDelt;
                    viewer->iscrollPos.v++;
                }
                else
                {
                    viewer->curPos = tmpCur;
                    viewer->deltaPos = tmpDelt;
                    viewer->iscrollPos.v--;
                    break;
                }

                //iDelt += Min (blockSize*viewer->coef.v, ); ïî-äðóãîìó
                int tmp = viewer->coef.v;
                while (tmp > 0)
                    {
                        iDelt += blockSize;
                        if (iCur + 1 >= model->lineCount) {iCur++; break;}
                        if (iDelt > model->offset[iCur + 1] - model->offset[iCur])
                        {
                            iCur++;
                            iDelt = 0;//òóò íå âñåãäà 0, íî ìá òóò !õèòðî!, ò.ê. óâåëè÷åíèå íå áîëüøå ÷åì íà áëîê, òî âñ¸ îê
                            if (iCur >=model->lineCount)
                                iCur = model->lineCount-1;
                        }
                        tmp--;
                    }
            }

            int tmperr = 0;
            if ((viewer->iscrollMax.v) % viewer->coef.v == 0) tmperr = -1;
//            if (tmperr == -1) last = 1; else last = 0;
            viewer->iscrollMax.v = Ceil((viewer->iscrollMax.v)/viewer->coef.v);
            //viewer->iscrollPos.v =(viewer->iscrollPos.v/viewer->coef.v);

            //if (viewer->curPos != 0)  viewer->curPos++;//Max(0,viewer->curPos+1 );//viewer->iscrollPos.v*viewer->coef.v  ;
            viewer->iscrollMax.v = viewer->iscrollMax.v + tmperr;
            SetScrollRange(hwnd, SB_VERT, 0,viewer->iscrollMax.v , FALSE);
            SetScrollPos(hwnd, SB_VERT, viewer->iscrollPos.v, TRUE);
        }

        UpdateWindow(hwnd);
        break;
    }
    case WM_VSCROLL ://äîáàâèòü êîýôôèöèåíò äëÿ âåðñòêè ê incScroll è ê Position
    {
        switch(LOWORD(wParam))
        {
        case SB_TOP :
            iVscrollInc = -viewer->iscrollPos.v;
            break;
        case SB_BOTTOM :
            iVscrollInc = viewer->iscrollMax.v - viewer->iscrollPos.v;
            break;
        case SB_LINEUP :
            iVscrollInc = -1;
            break;
        case SB_LINEDOWN :
            iVscrollInc = 1;
            break;
        case SB_PAGEUP :
            //ïåðåëèñòûâàíèå ïî ñòðàíè÷íî. òåêñò ðàçäåë¸í íà áëîêè. áëîêè äåëèòü íåëüçÿ.
            //ïîñëåäíèé íåöåëûé áëîê ïîïàäåò íà íîâóþ ñòðàíèöó, íå ñìîòðÿ íà òî, ÷òî îí âèäåí "êàê áû" íà ïðåäûäóùåé.
            iVscrollInc = -Max(1, (int)((viewer->cClient.y) / viewer->cChar.y))/(int)viewer->coef.v;
            break;
        case SB_PAGEDOWN :
            iVscrollInc = Max(1, viewer->cClient.y / viewer->cChar.y)/viewer->coef.v;
            break;
        case SB_THUMBTRACK :
            iVscrollInc = HIWORD(wParam) - viewer->iscrollPos.v;
            break;
        default :
            iVscrollInc = 0;
        }
        iVscrollInc = Max(-(viewer->iscrollPos.v),Min(iVscrollInc, viewer->iscrollMax.v - viewer->iscrollPos.v));
        if (iVscrollInc != 0)
        {
            viewer->iscrollPos.v += iVscrollInc;
            if ((viewer->iscrollPos.v > viewer->iscrollMax.v) || (viewer->iscrollPos.v < 0)) { viewer->iscrollPos.v -= iVscrollInc; break;}
            int blockSize = viewer->cClient.x / viewer->cChar.x;//êîëè÷åñòâî ñèìâîëîâ â îäíîé ñòðîêå
            if (viewer->mode == WITHOUT_LAYOUT)
            {
                viewer->curPos = (viewer->iscrollPos.v)*viewer->coef.v;
                viewer->actPos = viewer->curPos;
                viewer->actDelta = viewer->deltaPos;
            }
            else
            {
                //ïîìåíÿé ïîçèöèþ
                //ïîìåíÿé ïîçèöèþ
                //change delta and curPos
                PrintStatus(message, viewer, myfile, 6 );
                int tmp = Abs(iVscrollInc)*viewer->coef.v;//íà ñòîëüêî ñòðîê ñäâèíóòü


                if (iVscrollInc > 0)//DOWN
                {
                //size_t currentPos = viewer->iscrollPos.v*viewer->coef.v;//curPos + viewer->deltaPos;//
                //size_t lastPosition = viewer->iscrollMax.v*viewer->coef.v;//îòíèìè òåõ, ÷òî íå ïîëíûå( ??? )//model->lineCount - 1 + ((model->offset[model->lineCount] - model->offset[model->lineCount-1]) + 1)/ blockSize;
                //if (currentPos > lastPosition) break;
                /*if ((lastPosition - currentPos) < (viewer->coef.v * (Abs(iVscrollInc) ) - 1 ) )
                {
                    UpdateWindow(hwnd);
                    viewer->iscrollPos.v -= iVscrollInc;

                    //SetScrollPos(hwnd, SB_VERT, viewer->iscrollPos.v, TRUE);
                    break;
                }*/
                //if ((lastPosition - currentPos) < (viewer->coef.v * (Abs(iVscrollInc))) && (last == 1)) break;
                    while (tmp > 0)
                    {
                        viewer->deltaPos += blockSize;
                        if (viewer->deltaPos > model->offset[viewer->curPos + 1] - model->offset[viewer->curPos])
                        {
                            viewer->curPos++;
                            viewer->deltaPos = 0;//òóò íå âñåãäà 0, íî ìá òóò !õèòðî!, ò.ê. óâåëè÷åíèå íå áîëüøå ÷åì íà áëîê, òî âñ¸ îê
                            if (viewer->curPos >=model->lineCount)
                                viewer->curPos = model->lineCount-1;
                        }
                        tmp--;
                    }
                }
                else//UP
                {
                    if ((viewer->curPos == 0) && (viewer->deltaPos == 0))
                    {

                        SetScrollPos(hwnd, SB_VERT, 0, TRUE);
                                    UpdateWindow(hwnd);

                        break;
                    }
                      while (tmp > 0)
                    {
                        viewer->deltaPos -= blockSize;
                        if (viewer->deltaPos <0)
                        {
                            viewer->curPos--;
                            if (viewer->curPos < 0)
                            {
                                viewer->curPos = 0;
                                viewer->deltaPos = 0;
                            }
                            else
                            viewer->deltaPos = blockSize * ( ( model->offset[viewer->curPos + 1] - model->offset[viewer->curPos] ) / blockSize );
                        }
                        tmp--;
                    }
                }
            }
            PrintStatus(message, viewer, myfile, 7 );
            viewer->actPos = viewer->curPos;
            viewer->actDelta = viewer->deltaPos;
            SetScrollPos(hwnd, SB_VERT, viewer->iscrollPos.v, TRUE);

            ScrollWindow(hwnd, 0, -viewer->cChar.y * iVscrollInc * viewer->coef.v, NULL, NULL);

        }
        UpdateWindow(hwnd);
        break;
    }
    case WM_HSCROLL :
    {
        switch(LOWORD(wParam))
        {
        case SB_LINEUP :
            iHscrollInc = -1;
            break;
        case SB_LINEDOWN :
            iHscrollInc = 1;
            break;
        case SB_PAGEUP :
            iHscrollInc = -8;
            break;
        case SB_PAGEDOWN :
            iHscrollInc = 8;
            break;
        case SB_THUMBPOSITION :
            iHscrollInc = HIWORD(wParam) - viewer->iscrollPos.h;
            break;
        default :
            iHscrollInc = 0;
        }
        iHscrollInc = Max(-viewer->iscrollPos.h, Min(iHscrollInc, viewer->iscrollMax.h - viewer->iscrollPos.h));
        if (iHscrollInc != 0)
        {
            viewer->iscrollPos.h += iHscrollInc;
            ScrollWindow(hwnd, -viewer->cChar.x * iHscrollInc * viewer->coef.h, 0, NULL, NULL);
            SetScrollPos(hwnd, SB_HORZ, viewer->iscrollPos.h, TRUE);
            UpdateWindow(hwnd);
        }
        break;
    }
    case WM_KEYDOWN:
    {
        switch(wParam)
        {
        case VK_HOME :
            SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0L);
            break;
        case VK_END :
            SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0L);
            break;
        case VK_PRIOR :
            SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0L);
            break;
        case VK_NEXT :
            SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0L);
            break;
        case VK_UP :
            SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0L);
            break;
        case VK_DOWN :
            SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0L);
            break;
        case VK_LEFT :
            if (viewer->mode == WITHOUT_LAYOUT) SendMessage(hwnd, WM_HSCROLL, SB_PAGEUP, 0L);
            break;
        case VK_RIGHT :
            if (viewer->mode == WITHOUT_LAYOUT) SendMessage(hwnd, WM_HSCROLL, SB_PAGEDOWN, 0L);
            break;
        }
        break;
    }
    default:
        ReleaseDC(hwnd, hdc);             /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    ReleaseDC(hwnd, hdc);
    return 0;
}
