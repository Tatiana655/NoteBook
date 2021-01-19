#include <stdio.h>

/** Model
 * allText - указатель на весь текст
 * offset - массив смещений (Где начинается новя строка? С какого номера символа в тексте)
 * lineCount - количество строк в тексте
 * maxLength - длина самой длинной строки
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
 * v - вертикальный компонент
 * h - норизонтальный компонент
 */
typedef struct Pairhv
{
    size_t v;
    size_t h;
} Pairhv;

/** Pairhv
 * x - x координата
 * y - у координата
 */
typedef struct Pairxy
{
    size_t x;
    size_t y;
} Pairxy;

/** Mode
 * LAYOUT - режим верстки
 * WITHOUT_LAYOUT - режим без верстки
 */
typedef enum
{
    WITH_ALL_LAYOUT, WITHOUT_LAYOUT
} Mode;

/** Viewer
 * mode - режим
 * cChar - ширина высота символа
 * cClient - размеры рабочей области
 * iscrollPos - позииця скролла
 * iscrollMax - максимальный скролл(MaxRange)
 *
 *
 */
typedef struct Viewer//коэфф
{
    Mode mode;
    struct Pairxy cChar, cClient;//cChar - ширина высота символа //cClient : размеры рабочей области
    struct Pairhv iscrollPos, iscrollMax;
    size_t curPos;//текущая позиция (строка)
    int deltaPos;//номер символа в этой строке
    struct Pairhv coef;//коэффициенты для верстки и больших файлов
} Viewer;

//Инициализация модели, имя файла с текстом
int InitModel(struct Model * model, char * filename);
//освобождение модели (освобождение памяти)
void CloseModel(struct Model * model);
