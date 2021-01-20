#include <math.h>
#include <stdlib.h>
#include "System.h"
#include "..\readfileDLL\main.h"//путь к .h fileread

/** \brief
 *  //функция чтения
 * \param filename char* - имя файла
 * \return char* - считанный текст или NULL, если сломалось
 *
 */
 /*
char * readFile(char * filename)
{
    FILE * file = fopen (filename, "rb");//rt
    if (file == NULL) return NULL;
    fseek(file,0, SEEK_END);
    size_t sizeBuf = ftell(file);
    fseek(file,0, SEEK_SET);

    char * text = (char *)malloc(sizeBuf+1);
    if (text == NULL) return NULL;
    fread(text,1,sizeBuf,file);
    text[sizeBuf]='\0';
    fclose(file);
    return text;
}
*/

/** \brief
 *  //Инициализация модели
 * \param model struct Model* - какую структуру инитить
 * \param filename char*- имя файла, откуда читать
 * \return int- 1 -ошибка, 0 - ок
 *
 */
int InitModel(struct Model * model, char * filename)
{
    if ((filename[0]=='\0') || (filename == NULL))
    {
        model->allText = (char *)malloc(1);
        model->allText[0] = '\0';
        model->lineCount = 1;
        model->maxLength = 0;
        model->offset = (size_t *)malloc((model->lineCount+1)*sizeof(size_t));
        model->offset[0]=0;
        model->offset[1] = 1;
        return 0;
    }
    model->allText = readFile(filename);

    if (model->allText==NULL) return 1;
    model->lineCount = 1;
    model->maxLength = 0;
    size_t i=0;
    size_t strLen = 0;
    for (i = 0; (model->allText)[i] != '\0'; i++)
    {
        strLen++;
        if ((model->allText)[i] == '\n')
        {
            model->lineCount++;
            model->maxLength = strLen > model->maxLength? (strLen-1) : model->maxLength;
            strLen = 0;
        }
    }
    if (model->maxLength  == 0) model->maxLength = strLen;
    model->offset = (size_t *)malloc((model->lineCount+1)*sizeof(size_t));
    int k = 1;
    model->offset[0]=0;
    for (i = 0; model->allText[i] != '\0'; i++)
    {
        if (model->allText[i] == '\n')
        {
            model->offset[k] = i+1;
            k++;
        }
    }
    model->offset[k] = i;
    return 0;
}

/** \brief
 * освобождение памяти модели
 * \param model struct Model* -- какую модель освобождать
 * \return void
 *
 */
void CloseModel(struct Model * model)
{
    if ((model != NULL) && (model->allText != NULL))
    {
        free(model->offset);
        free(model->allText);
        model->allText = NULL;
        model->offset = NULL;
        free(model);
        model = NULL;
    }
}
