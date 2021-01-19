#include <stdio.h>

/** Model
 * allText - ��������� �� ���� �����
 * offset - ������ �������� (��� ���������� ���� ������? � ������ ������ ������� � ������)
 * lineCount - ���������� ����� � ������
 * maxLength - ����� ����� ������� ������
 */
typedef struct Model
{
    char *allText;
    size_t * offset;

    size_t lineCount;
    size_t maxLength;

} Model;

//Viewer
/** Pairhv
 * v - ������������ ���������
 * h - �������������� ���������
 */
typedef struct Pairhv
{
    size_t v;
    size_t h;
} Pairhv;

/** Pairhv
 * x - x ����������
 * y - � ����������
 */
typedef struct Pairxy
{
    size_t x;
    size_t y;
} Pairxy;

/** Mode
 * LAYOUT - ����� �������
 * WITHOUT_LAYOUT - ����� ��� �������
 */
typedef enum
{
    WITH_ALL_LAYOUT, WITHOUT_LAYOUT
} Mode;

/** Viewer
 * mode - �����
 * cChar - ������ ������ �������
 * cClient - ������� ������� �������
 * iscrollPos - ������� �������
 * iscrollMax - ������������ ������(MaxRange)
 *
 *
 */
typedef struct Viewer//�����
{
    Mode mode;
    struct Pairxy cChar, cClient;//cChar - ������ ������ ������� //cClient : ������� ������� �������
    struct Pairhv iscrollPos, iscrollMax;
    size_t curPos;//������� ������� (������)
    int deltaPos;//����� ������� � ���� ������
    struct Pairhv coef;//������������ ��� ������� � ������� ������
} Viewer;

//������������� ������, ��� ����� � �������
int InitModel(struct Model * model, char * filename);
//������������ ������ (������������ ������)
void CloseModel(struct Model * model);
